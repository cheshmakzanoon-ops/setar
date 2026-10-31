// Realtime-boundary evidence for setar::instrument::Instrument (REQ-RTS-001, REQ-EXC-002).
//
// These tests use a hand-built SYNTHETIC InstrumentConfig so that they depend only on
// the public engine contract, not on preset or body builders. They establish
// properties of the process() call on this build (no heap traffic, block-partition
// invariance, bounded iteration/channel counts, explicit capacity faults, bitwise
// replay). They do NOT establish a CPU deadline on any host or audio hardware.
//
// The runner receives the full CTest name as argv[1].
#include "setar/instrument/instrument.hpp"

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <iostream>
#include <new>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

// ---------------------------------------------------------------------------------
// Global allocation counters. Every replaceable allocation/deallocation function in
// this program routes through malloc/free and increments a counter while armed.
namespace {
std::atomic<bool> g_armed{false};
std::atomic<std::size_t> g_allocations{0};
std::atomic<std::size_t> g_deallocations{0};

void* counted_allocate(std::size_t size) {
    if (g_armed.load(std::memory_order_relaxed)) {
        g_allocations.fetch_add(1, std::memory_order_relaxed);
    }
    void* p = std::malloc(size == 0 ? 1 : size);
    if (p == nullptr) {
        throw std::bad_alloc();
    }
    return p;
}

void* counted_allocate_aligned(std::size_t size, std::align_val_t alignment) {
    if (g_armed.load(std::memory_order_relaxed)) {
        g_allocations.fetch_add(1, std::memory_order_relaxed);
    }
    const auto align = static_cast<std::size_t>(alignment);
    // Over-allocate and store the original pointer just before the aligned block, so
    // the matching delete can free it portably (std::aligned_alloc is absent on MSVC).
    void* raw = std::malloc(size + align + sizeof(void*));
    if (raw == nullptr) {
        throw std::bad_alloc();
    }
    const auto base = reinterpret_cast<std::uintptr_t>(raw) + sizeof(void*);
    const auto aligned = (base + align - 1) & ~(static_cast<std::uintptr_t>(align) - 1);
    reinterpret_cast<void**>(aligned)[-1] = raw;
    return reinterpret_cast<void*>(aligned);
}

void counted_free(void* p) noexcept {
    if (p == nullptr) {
        return;
    }
    if (g_armed.load(std::memory_order_relaxed)) {
        g_deallocations.fetch_add(1, std::memory_order_relaxed);
    }
    std::free(p);
}

void counted_free_aligned(void* p) noexcept {
    if (p == nullptr) {
        return;
    }
    if (g_armed.load(std::memory_order_relaxed)) {
        g_deallocations.fetch_add(1, std::memory_order_relaxed);
    }
    std::free(reinterpret_cast<void**>(p)[-1]);
}
} // namespace

void* operator new(std::size_t size) { return counted_allocate(size); }
void* operator new[](std::size_t size) { return counted_allocate(size); }
void* operator new(std::size_t size, const std::nothrow_t&) noexcept {
    try {
        return counted_allocate(size);
    } catch (...) {
        return nullptr;
    }
}
void* operator new[](std::size_t size, const std::nothrow_t&) noexcept {
    try {
        return counted_allocate(size);
    } catch (...) {
        return nullptr;
    }
}
void* operator new(std::size_t size, std::align_val_t a) { return counted_allocate_aligned(size, a); }
void* operator new[](std::size_t size, std::align_val_t a) { return counted_allocate_aligned(size, a); }
void operator delete(void* p) noexcept { counted_free(p); }
void operator delete[](void* p) noexcept { counted_free(p); }
void operator delete(void* p, std::size_t) noexcept { counted_free(p); }
void operator delete[](void* p, std::size_t) noexcept { counted_free(p); }
void operator delete(void* p, const std::nothrow_t&) noexcept { counted_free(p); }
void operator delete[](void* p, const std::nothrow_t&) noexcept { counted_free(p); }
void operator delete(void* p, std::align_val_t) noexcept { counted_free_aligned(p); }
void operator delete[](void* p, std::align_val_t) noexcept { counted_free_aligned(p); }
void operator delete(void* p, std::size_t, std::align_val_t) noexcept { counted_free_aligned(p); }
void operator delete[](void* p, std::size_t, std::align_val_t) noexcept { counted_free_aligned(p); }

namespace {
using namespace setar::instrument;

void check(bool condition, std::string_view message) {
    if (!condition) {
        throw std::runtime_error(std::string(message));
    }
}

// --- SYNTHETIC fixture -------------------------------------------------------------
// Placeholder string/body/contact values chosen for numerical exercise only; they are
// not measurements of any instrument. Strings are reduced to a 6 kHz band so the
// Debug-build tests stay short; the realtime properties under test do not depend on it.
constexpr double speaking_length_m = 0.68;
constexpr double total_length_m = 0.72;
constexpr std::size_t fret_count = 8;

double fret_position(std::size_t k) {
    // Ideal-string positions of k equal-tempered semitones: a fixture, not a fret map.
    return speaking_length_m * (1.0 - std::pow(2.0, -static_cast<double>(k) / 12.0));
}

InstrumentConfig synthetic_fixture(double action_scale = 1.0) {
    InstrumentConfig c;
    c.label = "realtime-test synthetic fixture";
    c.provenance = Provenance::Synthetic;
    const std::array<double, string_count> tension{31.0, 27.0, 26.0, 31.0};
    const std::array<double, string_count> density{2.47e-4, 3.85e-4, 8.18e-4, 2.47e-4};
    const std::array<double, string_count> stiffness{1.6e-5, 2.4e-5, 5.0e-5, 1.6e-5};
    for (std::size_t s = 0; s < string_count; ++s) {
        StringConfig& str = c.strings[s];
        str.label = "synthetic string " + std::to_string(s);
        str.total_length_m = total_length_m;
        str.bridge_position_m = speaking_length_m;
        str.tension_n = tension[s];
        str.linear_density_kg_per_m = density[s];
        str.bending_stiffness_n_m2 = stiffness[s];
        str.loss = StringLossLaw{0.6, 0.0, 2.0e-9};
        str.action_at_nut_m = 0.6e-3 * action_scale;
        str.action_at_bridge_m = 1.5e-3 * action_scale;
    }
    for (std::size_t k = 1; k <= fret_count; ++k) {
        c.frets.push_back(FretConfig{fret_position(k), "fixture semitone " + std::to_string(k)});
    }
    const std::array<double, 6> f{180.0, 310.0, 450.0, 700.0, 1100.0, 1700.0};
    for (std::size_t j = 0; j < f.size(); ++j) {
        BodyMode mode;
        mode.frequency_hz = f[j];
        mode.damping_ratio = 0.02;
        mode.modal_mass_kg = 0.03;
        for (std::size_t s = 0; s < string_count; ++s) {
            mode.bridge_participation[s][0] = 0.8 - 0.05 * static_cast<double>(s) +
                                              0.03 * static_cast<double>(j % 3);
            mode.bridge_participation[s][1] = 0.0;
        }
        mode.radiation_weight = {0.01 / static_cast<double>(j + 1), 0.008 / static_cast<double>(j + 1)};
        c.body.modes.push_back(mode);
    }
    BodyMode lateral;
    lateral.frequency_hz = 1400.0;
    lateral.damping_ratio = 0.05;
    lateral.modal_mass_kg = 0.02;
    for (std::size_t s = 0; s < string_count; ++s) {
        lateral.bridge_participation[s] = {0.0, 0.5};
    }
    lateral.radiation_weight = {0.001, -0.001};
    c.body.modes.push_back(lateral);
    c.body.label = "synthetic 7-mode fixture body";
    c.body.provenance = Provenance::Synthetic;
    c.bridge.stiffness_n_per_m = {1.0e6, 1.0e6};
    c.bridge.damping_n_s_per_m = {0.0, 0.0};
    c.contacts.fret = ContactLaw{1.0e8, 1.5, 0.5};
    c.contacts.finger = ContactLaw{5.0e4, 1.5, 2.0};
    c.contacts.nail = ContactLaw{1.0e6, 1.5, 0.05};
    c.contacts.finger_lateral_damping_n_s_per_m = 2.0;
    c.contacts.fret_stick_stiffness_n_per_m = 1.0e5;
    c.contacts.fret_stick_damping_n_s_per_m = 1.0;
    c.max_mode_frequency_hz = 6000.0;
    return c;
}

ProcessSpec fixture_spec(bool ledger = false) {
    ProcessSpec spec;
    spec.sample_rate_hz = 48000.0;
    spec.max_block_frames = 512;
    spec.max_events_per_block = 64;
    spec.max_interaction_channels = 32;
    spec.max_newton_iterations = 50;
    spec.newton_tolerance_m = 1.0e-14;
    spec.output_gain_per_pa = 1.0;
    spec.enable_energy_ledger = ledger;
    return spec;
}

// --- Gesture construction ----------------------------------------------------------
struct Timed {
    std::uint64_t sample = 0;
    GestureEvent event{};
};

GestureEvent nail(std::size_t s, double force_n, double angle_rad = 0.3) {
    GestureEvent e;
    e.kind = GestureKind::NailStroke;
    e.string_index = static_cast<std::uint8_t>(s);
    e.position_m = speaking_length_m - 0.12;
    e.angle_rad = angle_rad;
    e.speed_m_per_s = 0.5;
    e.force_n = force_n;
    e.gap_m = 0.5e-3;
    e.travel_m = 6.0e-3;
    return e;
}

GestureEvent press(const InstrumentConfig& c, std::size_t s, std::size_t fret, std::uint8_t slot = 0) {
    GestureEvent e;
    e.kind = GestureKind::FingerPress;
    e.string_index = static_cast<std::uint8_t>(s);
    e.slot = slot;
    e.position_m = fret_position(fret) - 0.008;
    e.height_m = fret_top_height_m(c.strings[s], fret_position(fret)) - 1.0e-3;
    e.speed_m_per_s = 0.2;
    e.gap_m = 1.0e-3;
    return e;
}

GestureEvent slide(std::size_t s, std::size_t fret, std::uint8_t slot = 0) {
    GestureEvent e;
    e.kind = GestureKind::FingerMove;
    e.string_index = static_cast<std::uint8_t>(s);
    e.slot = slot;
    e.position_m = fret_position(fret) - 0.008;
    e.speed_m_per_s = 0.3;
    return e;
}

GestureEvent lift(std::size_t s, std::uint8_t slot = 0) {
    GestureEvent e;
    e.kind = GestureKind::FingerRelease;
    e.string_index = static_cast<std::uint8_t>(s);
    e.slot = slot;
    e.speed_m_per_s = 0.2;
    e.gap_m = 1.0e-3;
    return e;
}

GestureEvent silence() {
    GestureEvent e;
    e.kind = GestureKind::Silence;
    return e;
}

std::uint64_t ms(double milliseconds) {
    return static_cast<std::uint64_t>(std::llround(milliseconds * 48.0)); // 48 kHz fixture
}

// A phrase exercising every gesture kind, two simultaneous fingers, a course pair,
// re-strikes of a ringing string, slides across frets and a final Silence.
std::vector<Timed> phrase(const InstrumentConfig& c) {
    std::vector<Timed> p{
        {ms(0), press(c, 0, 3)},
        {ms(30), nail(0, 0.5)},
        {ms(40), nail(2, 0.4, 2.8)},
        {ms(42), nail(3, 0.4, 2.8)},
        {ms(120), slide(0, 5)},
        {ms(200), press(c, 1, 2)},
        {ms(220), nail(1, 0.6)},
        {ms(260), nail(0, 0.7)}, // re-strike while ringing
        {ms(300), lift(0)},
        {ms(330), press(c, 2, 1, 1)},
        {ms(340), nail(2, 0.5)},
        {ms(400), lift(1)},
        {ms(420), lift(2, 1)},
        {ms(440), nail(3, 0.8)},
        {ms(560), silence()},
        {ms(600), nail(1, 0.4)},
    };
    for (std::size_t i = 0; i < p.size(); ++i) {
        p[i].event.sequence = static_cast<std::uint32_t>(i);
    }
    return p;
}

struct Render {
    std::vector<float> left;
    std::vector<float> right;
    ProcessSummary worst{};
    std::uint32_t total_rejected = 0;
    std::uint32_t total_newton_failures = 0;
    std::uint32_t total_capacity_overflows = 0;
    std::uint32_t total_containment = 0;
    std::vector<FaultCode> faults;
    std::vector<double> block_seconds;
};

void accumulate(Render& r, const ProcessSummary& s) {
    if (s.first_fault != FaultCode::None) {
        r.faults.push_back(s.first_fault);
    }
    r.total_rejected += s.rejected_events;
    r.total_newton_failures += s.newton_failures;
    r.total_capacity_overflows += s.capacity_overflows;
    r.total_containment += s.containment_resets;
    r.worst.max_newton_iterations = std::max(r.worst.max_newton_iterations, s.max_newton_iterations);
    r.worst.max_active_channels = std::max(r.worst.max_active_channels, s.max_active_channels);
}

// Renders `frames` samples in blocks of `block`, slicing absolute-time events into
// block-relative offsets. Heap work (vectors) happens only outside armed regions.
Render render(Instrument& instrument, const std::vector<Timed>& events, std::size_t frames,
              std::size_t block, bool count_allocations = false, bool time_blocks = false) {
    Render r;
    r.left.assign(frames, 0.0f);
    r.right.assign(frames, 0.0f);
    r.block_seconds.reserve(frames / block + 1);
    std::vector<GestureEvent> pending;
    pending.reserve(events.size());
    std::size_t next = 0;
    for (std::size_t start = 0; start < frames; start += block) {
        const std::size_t n = std::min(block, frames - start);
        pending.clear();
        while (next < events.size() && events[next].sample < start + n) {
            GestureEvent e = events[next].event;
            e.sample_offset = static_cast<std::uint32_t>(events[next].sample - start);
            pending.push_back(e);
            ++next;
        }
        const std::span<float> l(r.left.data() + start, n);
        const std::span<float> rr(r.right.data() + start, n);
        if (count_allocations) {
            g_armed.store(true, std::memory_order_relaxed);
        }
        const auto t0 = std::chrono::steady_clock::now();
        const ProcessSummary s = instrument.process(pending, l, rr);
        const auto t1 = std::chrono::steady_clock::now();
        const InstrumentSnapshot snap = instrument.snapshot();
        const EnergyLedger ledger = instrument.ledger();
        if (count_allocations) {
            g_armed.store(false, std::memory_order_relaxed);
        }
        static_cast<void>(snap);
        static_cast<void>(ledger);
        check(s.processed_frames == n, "process() must report every requested frame");
        if (time_blocks) {
            r.block_seconds.push_back(std::chrono::duration<double>(t1 - t0).count());
        }
        accumulate(r, s);
    }
    return r;
}

bool bitwise_equal(const std::vector<float>& a, const std::vector<float>& b) {
    return a.size() == b.size() && std::memcmp(a.data(), b.data(), a.size() * sizeof(float)) == 0;
}

void check_finite_and_audible(const Render& r) {
    double peak = 0.0;
    for (std::size_t i = 0; i < r.left.size(); ++i) {
        if (!std::isfinite(r.left[i]) || !std::isfinite(r.right[i])) {
            throw std::runtime_error("output must be finite: first non-finite sample " + std::to_string(i) +
                                     " (containment_resets=" + std::to_string(r.total_containment) +
                                     " newton_failures=" + std::to_string(r.total_newton_failures) +
                                     " capacity_overflows=" + std::to_string(r.total_capacity_overflows) + ")");
        }
        peak = std::max(peak, static_cast<double>(std::abs(r.left[i])));
    }
    check(peak > 0.0, "fixture phrase must produce nonzero observed pressure");
}

void require_no_faults(const Render& r, std::string_view context) {
    if (!r.faults.empty() || r.total_rejected != 0 || r.total_newton_failures != 0 ||
        r.total_capacity_overflows != 0 || r.total_containment != 0) {
        throw std::runtime_error(std::string(context) + ": unexpected fault (faults=" +
                                 std::to_string(r.faults.size()) + " first=" +
                                 std::to_string(r.faults.empty() ? 0 : static_cast<int>(r.faults[0])) +
                                 " rejected=" + std::to_string(r.total_rejected) +
                                 " newton=" + std::to_string(r.total_newton_failures) +
                                 " capacity=" + std::to_string(r.total_capacity_overflows) +
                                 " containment=" + std::to_string(r.total_containment) + ")");
    }
}

// --- Tests -------------------------------------------------------------------------

void allocation_hook() {
    // Sanity: the hook must actually observe heap traffic, otherwise zero proves nothing.
    g_allocations.store(0);
    g_armed.store(true);
    {
        auto probe = std::make_unique<std::array<double, 16>>();
        static_cast<void>(probe);
    }
    g_armed.store(false);
    check(g_allocations.load() >= 1 && g_deallocations.load() >= 1,
          "allocation hook must count a deliberate allocation");

    const InstrumentConfig config = synthetic_fixture();
    for (const bool ledger : {false, true}) {
        Instrument instrument;
        instrument.prepare(config, fixture_spec(ledger)); // may allocate: not counted
        const auto events = phrase(config);
        g_allocations.store(0);
        g_deallocations.store(0);
        // Several block sizes, including 1 frame and the declared maximum.
        for (const std::size_t block : {std::size_t{1}, std::size_t{64}, std::size_t{512}}) {
            instrument.reset();
            const Render r = render(instrument, events, ms(700), block, true);
            require_no_faults(r, "allocation_hook");
            check_finite_and_audible(r);
        }
        g_armed.store(true);
        instrument.reset();
        g_armed.store(false);
        std::cout << "ledger=" << ledger << " allocations_during_process=" << g_allocations.load()
                  << " deallocations_during_process=" << g_deallocations.load() << '\n';
        check(g_allocations.load() == 0, "process/snapshot/ledger/reset must not allocate");
        check(g_deallocations.load() == 0, "process/snapshot/ledger/reset must not free");
    }
}

void block_partition() {
    const InstrumentConfig config = synthetic_fixture();
    const auto events = phrase(config);
    const std::size_t frames = ms(700);
    Instrument reference_instrument;
    reference_instrument.prepare(config, fixture_spec());
    const Render reference = render(reference_instrument, events, frames, 1);
    require_no_faults(reference, "block_partition reference");
    check_finite_and_audible(reference);
    for (const std::size_t block :
         {std::size_t{7}, std::size_t{64}, std::size_t{256}, std::size_t{512}}) {
        Instrument instrument;
        instrument.prepare(config, fixture_spec());
        const Render r = render(instrument, events, frames, block);
        require_no_faults(r, "block_partition");
        std::size_t first_difference = frames;
        for (std::size_t i = 0; i < frames; ++i) {
            if (std::memcmp(&r.left[i], &reference.left[i], sizeof(float)) != 0 ||
                std::memcmp(&r.right[i], &reference.right[i], sizeof(float)) != 0) {
                first_difference = i;
                break;
            }
        }
        std::cout << "block=" << block << " first_difference_sample="
                  << (first_difference == frames ? std::string("none") : std::to_string(first_difference))
                  << '\n';
        check(first_difference == frames, "output must be bitwise independent of block partition");
    }
}

// Stress: low action (buzz against frets), every string plucked hard and repeatedly
// with both finger slots active on several strings.
std::vector<Timed> stress_events(const InstrumentConfig& config) {
    std::vector<Timed> events;
    for (std::size_t s = 0; s < string_count; ++s) {
        events.push_back({ms(0), press(config, s, 2 + s, 0)});
        events.push_back({ms(5), press(config, s, 6, 1)});
    }
    for (std::size_t round = 0; round < 3; ++round) {
        const double t0 = 90.0 * static_cast<double>(round);
        for (std::size_t s = 0; s < string_count; ++s) {
            events.push_back({ms(30.0 + t0 + 3.0 * static_cast<double>(s)),
                              nail(s, 1.2, (round % 2 == 0) ? 0.4 : 2.7)});
        }
        events.push_back({ms(70.0 + t0), slide(round % string_count, 1 + round, 0)});
    }
    std::stable_sort(events.begin(), events.end(),
                     [](const Timed& a, const Timed& b) { return a.sample < b.sample; });
    std::uint32_t sequence = 0;
    for (auto& e : events) {
        e.event.sequence = sequence++;
    }
    return events;
}

void max_work() {
    const InstrumentConfig config = synthetic_fixture(0.35);
    ProcessSpec spec = fixture_spec();
    spec.max_interaction_channels = 64; // large enough that this stress stays within capacity
    Instrument instrument;
    instrument.prepare(config, spec);
    const auto events = stress_events(config);
    const std::size_t block = 256;
    const Render r = render(instrument, events, ms(300), block, false, true);
    check_finite_and_audible(r);
    check(r.total_rejected == 0, "stress events are valid and within capacity");
    check(r.total_capacity_overflows == 0, "stress must stay within the prepared channel capacity");
    check(r.worst.max_newton_iterations <= spec.max_newton_iterations,
          "Newton iterations never exceed the prepared cap");
    check(r.worst.max_active_channels <= spec.max_interaction_channels,
          "active channels never exceed the prepared capacity");
    check(r.worst.max_active_channels >= 8 + string_count,
          "stress must actually exercise bridge plus contact channels");
    std::vector<double> t = r.block_seconds;
    std::sort(t.begin(), t.end());
    const double block_audio_s = static_cast<double>(block) / spec.sample_rate_hz;
    const double p50 = t[t.size() / 2];
    const double p99 = t[std::min(t.size() - 1, (t.size() * 99) / 100)];
    std::cout << "THIS-MACHINE MEASUREMENT (build type as configured; not a host qualification)\n"
              << "blocks=" << t.size() << " block_frames=" << block
              << " max_active_channels=" << r.worst.max_active_channels
              << " max_newton_iterations=" << r.worst.max_newton_iterations
              << " newton_failures=" << r.total_newton_failures
              << " capacity_overflows=" << r.total_capacity_overflows << '\n'
              << "block_wall_us p50=" << p50 * 1e6 << " p99=" << p99 * 1e6
              << " max=" << t.back() * 1e6 << " block_audio_us=" << block_audio_s * 1e6 << '\n';
    // Failures and overflows are permitted only if reported; the stress case must not
    // need containment resets.
    check(r.total_containment == 0, "stress phrase must not trigger non-finite containment");
}

void capacity_overflow() {
    const InstrumentConfig config = synthetic_fixture();
    // (a) Event capacity.
    {
        ProcessSpec spec = fixture_spec();
        spec.max_events_per_block = 8;
        Instrument instrument;
        instrument.prepare(config, spec);
        std::vector<GestureEvent> events;
        for (std::uint32_t i = 0; i < 12; ++i) {
            GestureEvent e = nail(i % string_count, 0.4);
            e.sample_offset = i;
            e.sequence = i;
            events.push_back(e);
        }
        std::vector<float> l(64), r(64);
        const ProcessSummary s = instrument.process(events, l, r);
        std::cout << "event overflow: first_fault=" << static_cast<int>(s.first_fault)
                  << " rejected=" << s.rejected_events << '\n';
        check(s.first_fault == FaultCode::EventOverflow, "excess events must raise EventOverflow");
        check(s.rejected_events == 4, "exactly the 4 excess events are rejected and counted");
        check(s.processed_frames == 64, "the block is still processed");
    }
    // (b) Interaction-channel capacity: find the smallest capacity prepare() accepts,
    // then demand more simultaneous channels than that.
    {
        Instrument instrument;
        ProcessSpec spec = fixture_spec();
        bool accepted = false;
        for (std::uint32_t cap = 1; cap <= 32 && !accepted; ++cap) {
            spec.max_interaction_channels = cap;
            try {
                instrument.prepare(config, spec);
                accepted = true;
            } catch (const std::invalid_argument&) {
            }
        }
        check(accepted, "some channel capacity in [1, 32] must be accepted");
        std::cout << "smallest accepted max_interaction_channels=" << spec.max_interaction_channels << '\n';
        std::vector<Timed> events;
        std::uint32_t sequence = 0;
        for (std::size_t s = 0; s < string_count; ++s) {
            events.push_back({0, press(config, s, 3, 0)});
            events.push_back({0, press(config, s, 6, 1)});
        }
        for (std::size_t s = 0; s < string_count; ++s) {
            events.push_back({ms(20), nail(s, 1.0)});
        }
        for (auto& e : events) {
            e.event.sequence = sequence++;
        }
        const Render r = render(instrument, events, ms(150), 64);
        bool reported = false;
        for (const FaultCode f : r.faults) {
            reported = reported || f == FaultCode::ChannelCapacityExceeded;
        }
        std::cout << "channel overflow: capacity_overflows=" << r.total_capacity_overflows
                  << " max_active_channels=" << r.worst.max_active_channels
                  << " fault_reported=" << reported << '\n';
        check(reported && r.total_capacity_overflows > 0,
              "exceeding the channel capacity must be reported as ChannelCapacityExceeded");
        check(r.worst.max_active_channels <= spec.max_interaction_channels,
              "the solver never runs more channels than prepared");
        for (std::size_t i = 0; i < r.left.size(); ++i) {
            check(std::isfinite(r.left[i]) && std::isfinite(r.right[i]),
                  "overflow containment keeps the output finite");
        }
    }
    // (c) Default capacity (32) under the low-action stress phrase: overflow must be
    // reported and, per the FaultCode contract, never surface as non-finite output.
    {
        const InstrumentConfig stressed = synthetic_fixture(0.35);
        Instrument instrument;
        instrument.prepare(stressed, fixture_spec());
        const Render r = render(instrument, stress_events(stressed), ms(300), 256);
        std::size_t non_finite = 0;
        for (std::size_t i = 0; i < r.left.size(); ++i) {
            non_finite += (std::isfinite(r.left[i]) && std::isfinite(r.right[i])) ? 0u : 1u;
        }
        std::cout << "default-capacity stress: capacity_overflows=" << r.total_capacity_overflows
                  << " newton_failures=" << r.total_newton_failures
                  << " containment_resets=" << r.total_containment << " non_finite_samples=" << non_finite
                  << '\n';
        check(r.total_capacity_overflows > 0, "the stress phrase must exceed the default capacity of 32");
        check(non_finite == 0, "faulted processing must never emit non-finite output samples");
    }
}

void gesture_replay() {
    // The engine has no random number source: identical prepared configuration, initial
    // state and gesture sequence must reproduce the output bit for bit.
    const InstrumentConfig config = synthetic_fixture();
    const auto events = phrase(config);
    const std::size_t frames = ms(700);
    Instrument a;
    Instrument b;
    a.prepare(config, fixture_spec(true));
    b.prepare(config, fixture_spec(true));
    const Render ra = render(a, events, frames, 64);
    const Render rb = render(b, events, frames, 64);
    require_no_faults(ra, "gesture_replay");
    check_finite_and_audible(ra);
    check(bitwise_equal(ra.left, rb.left) && bitwise_equal(ra.right, rb.right),
          "two fresh instances must render identical output");
    const EnergyLedger la = a.ledger();
    a.reset();
    const Render rc = render(a, events, frames, 64);
    check(bitwise_equal(ra.left, rc.left) && bitwise_equal(ra.right, rc.right),
          "reset() followed by replay must reproduce the first render");
    const EnergyLedger lc = a.ledger();
    check(std::memcmp(&la, &lc, sizeof(EnergyLedger)) == 0, "replayed energy ledger must be identical");
    // Negative control: a changed gesture parameter must change the output.
    auto altered = events;
    altered[1].event.force_n *= 1.5;
    Instrument d;
    d.prepare(config, fixture_spec(true));
    const Render rd = render(d, altered, frames, 64);
    check(!bitwise_equal(ra.left, rd.left), "a different gesture must produce a different output");
    std::cout << "replay identical across instances and after reset; altered gesture differs\n";
}

} // namespace

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "usage: runner <ctest-name>\n";
        return 2;
    }
    const std::string_view requested(argv[1]);
    struct Case {
        std::string_view name;
        void (*run)();
    };
    const std::array<Case, 5> tests{{
        {"realtime.allocation_hook", allocation_hook},
        {"realtime.block_partition", block_partition},
        {"realtime.max_work", max_work},
        {"realtime.capacity_overflow", capacity_overflow},
        {"contact.gesture_replay", gesture_replay},
    }};
    try {
        for (const auto& test : tests) {
            if (test.name == requested) {
                test.run();
                std::cout << "PASS " << requested << '\n';
                return 0;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "FAIL " << requested << ": " << e.what() << '\n';
        return 1;
    }
    std::cerr << "unknown test: " << requested << '\n';
    return 2;
}
