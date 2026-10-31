// Performance-layer tests: score grammar, gesture expansion, scheduling, fret choice and
// the realtime MIDI mapper. The instrument configuration is a hand-built SYNTHETIC
// literal so these tests are independent of the preset, body and engine modules.
#include "setar/instrument/performance.hpp"

#include <array>
#include <atomic>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <new>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

// Allocation counter for the realtime check (replaces the global allocator in this TU).
namespace {
std::atomic<std::size_t> allocation_count{0};
}
// Every non-aligned form is replaced so sanitizer runtimes never pair their own
// allocation with this free().
void* counted_allocation(std::size_t size) {
    allocation_count.fetch_add(1, std::memory_order_relaxed);
    return std::malloc(size == 0 ? 1 : size);
}
void* operator new(std::size_t size) {
    if (void* p = counted_allocation(size)) {
        return p;
    }
    throw std::bad_alloc();
}
void* operator new[](std::size_t size) {
    if (void* p = counted_allocation(size)) {
        return p;
    }
    throw std::bad_alloc();
}
void* operator new(std::size_t size, const std::nothrow_t&) noexcept {
    return counted_allocation(size);
}
void* operator new[](std::size_t size, const std::nothrow_t&) noexcept {
    return counted_allocation(size);
}
void operator delete(void* p) noexcept { std::free(p); }
void operator delete[](void* p) noexcept { std::free(p); }
void operator delete(void* p, std::size_t) noexcept { std::free(p); }
void operator delete[](void* p, std::size_t) noexcept { std::free(p); }
void operator delete(void* p, const std::nothrow_t&) noexcept { std::free(p); }
void operator delete[](void* p, const std::nothrow_t&) noexcept { std::free(p); }

namespace {
using namespace setar::instrument;

void check(bool condition, std::string_view message) {
    if (!condition) {
        throw std::runtime_error(std::string(message));
    }
}

void near(double actual, double expected, double tolerance, std::string_view message) {
    if (!std::isfinite(actual) || std::abs(actual - expected) > tolerance) {
        throw std::runtime_error(std::string(message) + " actual=" + std::to_string(actual) +
                                 " expected=" + std::to_string(expected));
    }
}

constexpr double speaking_length = 0.68;
constexpr std::size_t fret_count = 12;

// Synthetic fixture: equal strings, 12-TET ideal fret positions (an analytic fixture,
// not a setar fret map).
InstrumentConfig fixture() {
    InstrumentConfig c;
    c.label = "performance test fixture (synthetic)";
    for (StringConfig& s : c.strings) {
        s.total_length_m = 0.72;
        s.bridge_position_m = speaking_length;
        s.tension_n = 30.0;
        s.linear_density_kg_per_m = 2.5e-4;
        s.action_at_nut_m = 0.0006;
        s.action_at_bridge_m = 0.0012;
    }
    for (std::size_t k = 1; k <= fret_count; ++k) {
        c.frets.push_back(
            FretConfig{speaking_length * (1.0 - std::exp2(-static_cast<double>(k) / 12.0)),
                       "12-TET fixture " + std::to_string(k)});
    }
    return c;
}

const std::array<double, string_count> open_hz{261.63, 196.0, 130.81, 261.63};

template <typename F> std::string error_of(F&& f) {
    try {
        f();
    } catch (const std::invalid_argument& e) {
        return e.what();
    }
    return {};
}

void parse_valid() {
    const InstrumentConfig c = fixture();
    const PerformanceSettings s = default_performance_settings();
    const std::string text = R"(# comment line
title Parser fixture
status synthetic demonstration; not a transcription of a performance or a validated gesture
tempo 120
pluck 1 string=0 fret=5 dyn=f dir=up   # beats: t = 0.5 s
pluck @0.75 string=1                    # seconds, no finger change
press @0.8 string=2 fret=3 slot=1
slide @0.9 string=2 slot=1 to_fret=7 dur=@0.2
release @1.2 string=2 slot=1
touch @1.3 string=3 at=0.5
pluck @1.4 string=3 dyn=pp
release @1.5 string=3
damp @1.6 string=1 dur=@0.1
pluck @1.8 string=0 fret=0
silence @2.0
end 5
)";
    const Score score = parse_score(text, c, s);
    check(score.title == "Parser fixture", "title");
    check(score.status.rfind("synthetic demonstration", 0) == 0, "status");
    near(score.end_time_s, 2.5, 1e-12, "end in beats at 120 bpm");
    const std::vector<GestureKind> kinds{
        GestureKind::FingerPress,   // fret 5 lead
        GestureKind::NailStroke,    // t 0.5
        GestureKind::NailStroke,    // t 0.75
        GestureKind::FingerPress,   // press 0.8
        GestureKind::FingerMove,    // slide 0.9
        GestureKind::FingerRelease, // 1.2
        GestureKind::FingerPress,   // touch 1.3
        GestureKind::NailStroke,    // 1.4
        GestureKind::FingerRelease, // 1.5
        GestureKind::FingerPress,   // damp 1.6
        GestureKind::FingerRelease, // damp release 1.7
        GestureKind::FingerRelease, // fret=0 lead 1.74
        GestureKind::NailStroke,    // 1.8
        GestureKind::Silence,       // 2.0
    };
    check(score.gestures.size() == kinds.size(),
          "gesture count " + std::to_string(score.gestures.size()));
    for (std::size_t i = 0; i < kinds.size(); ++i) {
        check(score.gestures[i].event.kind == kinds[i],
              "gesture kind order at " + std::to_string(i));
        if (i > 0) {
            check(score.gestures[i].time_s >= score.gestures[i - 1].time_s, "sorted by time");
        }
    }
    const GestureEvent& press = score.gestures[0].event;
    const double fret5 = c.frets[4].position_m;
    near(score.gestures[0].time_s, 0.5 - s.finger_lead_time_s, 1e-12, "finger lead time");
    near(press.position_m, fret5 - s.finger_offset_behind_fret_m, 1e-15, "pad behind fret");
    near(press.height_m, fret_top_height_m(c.strings[0], press.position_m) - s.press_depth_m, 1e-15,
         "pad target below fret-top line");
    check(press.slot == 0 && press.string_index == 0, "press slot/string");
    const GestureEvent& up = score.gestures[1].event;
    near(up.position_m, speaking_length - s.pluck_distance_from_bridge_m, 1e-15, "pluck point");
    check(std::cos(up.angle_rad) > 0.0 && std::sin(up.angle_rad) < 0.0,
          "up stroke: +h, toward soundboard");
    near(up.force_n, s.f.release_force_n, 0.0, "dyn=f force");
    near(up.speed_m_per_s, s.f.speed_m_per_s, 0.0, "dyn=f speed");
    near(up.gap_m, s.nail_gap_m, 0.0, "nail gap");
    near(up.travel_m, s.nail_travel_m, 0.0, "nail travel");
    const GestureEvent& down = score.gestures[2].event;
    check(std::cos(down.angle_rad) < 0.0 && std::sin(down.angle_rad) < 0.0,
          "default down stroke: -h");
    near(down.force_n, s.mf.release_force_n, 0.0, "default dyn mf");
    const GestureEvent& slide = score.gestures[4].event;
    const double x3 = c.frets[2].position_m - s.finger_offset_behind_fret_m;
    const double x7 = c.frets[6].position_m - s.finger_offset_behind_fret_m;
    near(slide.position_m, x7, 1e-15, "slide target");
    near(slide.speed_m_per_s, (x7 - x3) / 0.2, 1e-12, "slide speed from duration");
    check(slide.slot == 1, "slide slot");
    const GestureEvent& touch = score.gestures[6].event;
    near(touch.position_m, 0.5 * speaking_length, 1e-15, "touch node");
    near(touch.height_m, -s.touch_depth_m, 0.0, "touch height");
    near(score.gestures[10].time_s, 1.7, 1e-12, "damp release after dur");
    check(score.gestures[9].event.slot == 1, "damp uses slot 1");
    near(score.gestures[11].time_s, 1.8 - s.finger_lead_time_s, 1e-12, "open-string release lead");

    // Comma-step frets (8.2 mm and 4.1 mm apart, as in the 17-step Pythagorean map): the
    // pad must stay between the target fret and the previous one: offset 0.4*gap < 8 mm.
    InstrumentConfig close = c;
    close.frets = {FretConfig{0.040, "a"}, FretConfig{0.0482, "b"}, FretConfig{0.0523, "c"},
                   FretConfig{0.30, "d"}};
    near(finger_position_for_fret(close, 1, s), 0.040 - s.finger_offset_behind_fret_m, 1e-15,
         "wide gap keeps the nominal offset");
    near(finger_position_for_fret(close, 2, s), 0.0482 - 0.4 * 0.0082, 1e-15,
         "8.2 mm gap clamps offset");
    near(finger_position_for_fret(close, 3, s), 0.0523 - 0.4 * 0.0041, 1e-15,
         "4.1 mm gap clamps offset");
    near(finger_position_for_fret(close, 4, s), 0.30 - s.finger_offset_behind_fret_m, 1e-15,
         "large gap");
    InstrumentConfig near_nut = c;
    near_nut.frets = {FretConfig{0.005, "a"}};
    near(finger_position_for_fret(near_nut, 1, s), 0.005 - 0.4 * 0.005, 1e-15,
         "first fret uses the nut");
    for (std::size_t k = 1; k <= close.frets.size(); ++k) {
        const double x = finger_position_for_fret(close, k, s);
        const double previous = k == 1 ? 0.0 : close.frets[k - 2].position_m;
        check(x > previous && x < close.frets[k - 1].position_m, "pad between neighbouring frets");
    }
    const Score clamped = parse_score("title t\nstatus synthetic\npress @0.1 string=0 fret=3\n"
                                      "slide @0.2 string=0 to_fret=2 dur=@0.05\n"
                                      "pluck @0.5 string=1 fret=2\nend @1\n",
                                      close, s);
    near(clamped.gestures[0].event.position_m, finger_position_for_fret(close, 3, s), 0.0,
         "press path");
    near(clamped.gestures[1].event.position_m, finger_position_for_fret(close, 2, s), 0.0,
         "slide path");
    near(clamped.gestures[2].event.position_m, finger_position_for_fret(close, 2, s), 0.0,
         "pluck path");
    check(!error_of([&] { (void)finger_position_for_fret(close, 0, s); }).empty(),
          "fret 0 has no pad");
    check(!error_of([&] { (void)finger_position_for_fret(close, 5, s); }).empty(),
          "fret beyond map");
}

void parse_errors() {
    const InstrumentConfig c = fixture();
    const PerformanceSettings s = default_performance_settings();
    const std::string head = "title t\nstatus synthetic\n";
    struct Case {
        std::string text;
        std::string expected; // substring of the error message
    };
    const std::vector<Case> cases{
        {"title t\npluck @0.1 string=0\nend @1\n", "status"},
        {head + "pluck @0.1 string=0\n", "end"},
        {head + "strike @0.1 string=0\nend @1\n", "line 3: unknown command"},
        {head + "pluck @0.1 string=0 colour=red\nend @1\n", "line 3: unknown key 'colour'"},
        {head + "pluck @0.1 string=4\nend @1\n", "line 3: string must be 0..3"},
        {head + "pluck @0.1 string=0 fret=13\nend @1\n", "line 3: fret 13 exceeds"},
        {head + "pluck @0.1 string=0 dyn=fff\nend @1\n", "line 3: dyn must be"},
        {head + "pluck @0.1 string=0 dir=sideways\nend @1\n", "line 3: dir must be"},
        {head + "pluck @0.5 string=0\npluck @0.4 string=1\nend @1\n", "line 4: command times"},
        {head + "pluck 1 string=0\nend @2\n", "line 3: time '1' is in beats"},
        {head + "slide @0.2 string=0 to_fret=3 dur=@0.1\nend @1\n",
         "line 3: slide needs an active finger"},
        {head + "release @0.2 string=0\nend @1\n", "line 3: release needs an active finger"},
        {head + "pluck @0.01 string=0 fret=3\nend @1\n",
         "line 3: an expanded gesture falls before t = 0"},
        {head + "pluck @0.1 string=0 dyn=p\npluck @0.12 string=0 fret=2\nend @1\n",
         "line 4: finger change at"},
        {head + "pluck @x string=0\nend @1\n", "line 3: invalid number"},
        {head + "pluck @0.1 string=0 string=1\nend @1\n", "line 3: duplicate key"},
        {head + "damp @0.9 string=0 dur=@0.5\nend @1\n", "line 4: a gesture at"},
        {head + "end @1\npluck @1 string=0\n", "line 4: no command may follow"},
        {head + "strum @0.1 course=3\nend @1\n", "line 3: course must be"},
        {head + "strum @0.1 strings=0,,1\nend @1\n", "line 3: strings list has an empty entry"},
        {head + "strum @0.1 course=1 strings=1\nend @1\n", "line 3: strum needs exactly one"},
        {head + "tremolo @0.1 string=0 dur=@0.01 rate=10\nend @1\n", "line 3: tremolo duration"},
        {head + "touch @0.1 string=0 at=1.0\nend @1\n", "line 3: touch at="},
        {head + "pluck @0.1 string=0\ntempo 90\nend @1\n", "line 4: tempo must precede"},
        {head + "pluck string=0\nend @1\n", "line 3: command 'pluck' needs a time"},
    };
    for (const Case& k : cases) {
        const std::string message = error_of([&] { (void)parse_score(k.text, c, s); });
        check(message.find(k.expected) != std::string::npos, "expected error containing '" +
                                                                 k.expected + "', got '" + message +
                                                                 "' for:\n" + k.text);
    }
    PerformanceSettings bad = s;
    bad.nail_travel_m = bad.nail_gap_m;
    check(error_of([&] { (void)parse_score(head + "end @1\n", c, bad); }).find("nail_travel_m") !=
              std::string::npos,
          "invalid settings rejected");
    InstrumentConfig unordered = c;
    std::swap(unordered.frets[0], unordered.frets[1]);
    check(error_of([&] {
              (void)parse_score(head + "end @1\n", unordered, s);
          }).find("fret positions") != std::string::npos,
          "unordered frets rejected");
}

void schedule_ordering() {
    const InstrumentConfig c = fixture();
    const PerformanceSettings s = default_performance_settings();
    const Score score = parse_score("title t\nstatus synthetic\n"
                                    "strum @0.5 strings=0,1,2,3 dir=up\n"
                                    "pluck @0.5 string=0 dyn=pp\n"
                                    "pluck @1.0000104 string=1\n"
                                    "end @2\n",
                                    c, s);
    const std::vector<ScheduledGesture> sched = schedule(score, 48000.0);
    check(sched.size() == 6, "scheduled count");
    for (std::size_t i = 0; i < sched.size(); ++i) {
        check(sched[i].event.sequence == i, "stable ascending sequence");
        check(sched[i].event.sample_offset == 0, "block offset left to the renderer");
        if (i > 0) {
            check(sched[i].sample_index >= sched[i - 1].sample_index, "nondecreasing sample index");
        }
    }
    check(sched[0].sample_index == 24000 && sched[0].event.string_index == 0,
          "strum first string at 0.5 s");
    // The separate pluck at 0.5 s shares the first stroke's time and keeps file order.
    check(sched[1].sample_index == 24000 && sched[1].event.string_index == 0 &&
              sched[1].event.force_n == s.pp.release_force_n,
          "equal-time gestures keep file order");
    check(sched[2].sample_index == 24000 + 384, "8 ms sweep delay = 384 samples");
    check(sched[5].sample_index == 48000, "round to nearest sample (1.0000104 s)");
    const std::vector<ScheduledGesture> at96 = schedule(score, 96000.0);
    check(at96[5].sample_index == 96001, "rate-dependent rounding");
    check(!error_of([&] { (void)schedule(score, 0.0); }).empty(), "zero sample rate rejected");
    check(!error_of([&] { (void)schedule(score, std::nan("")); }).empty(),
          "NaN sample rate rejected");
    Score unsorted = score;
    std::swap(unsorted.gestures.front(), unsorted.gestures.back());
    check(!error_of([&] { (void)schedule(unsorted, 48000.0); }).empty(), "unsorted score rejected");
}

void fret_choice() {
    const InstrumentConfig c = fixture();
    // Ideal-string fret k of the 12-TET fixture is exactly k semitones above the open string.
    const double f5 = open_hz[0] * std::exp2(5.0 / 12.0);
    FretChoice choice = choose_fret(f5, c, open_hz, std::array<std::size_t, 1>{0});
    check(choice.string_index == 0 && choice.fret_number == 5, "fret 5 on string 0");
    near(choice.cents_error, 0.0, 1e-9, "exact fixture pitch");
    near(choice.finger_position_m, c.frets[4].position_m, 0.0, "fret position reported");
    near(choice.nominal_frequency_hz, f5, 1e-9, "nominal frequency");
    // 30 cents sharp request: nearest fret is still 5 with a -30 cent error (nominal is flat).
    const double sharp = f5 * std::exp2(30.0 / 1200.0);
    choice = choose_fret(sharp, c, open_hz, std::array<std::size_t, 1>{0});
    check(choice.fret_number == 5, "nearest fret for detuned request");
    near(choice.cents_error, -30.0, 1e-9, "signed cents error");
    // Ties in |cents|: lower fret wins (open string 0 vs string 1 at the fourth above G).
    const std::array<double, string_count> fourth{261.63, 261.63 * std::exp2(-5.0 / 12.0), 130.81,
                                                  100.0};
    choice = choose_fret(261.63, c, fourth);
    check(choice.string_index == 0 && choice.fret_number == 0, "lower fret wins a tie");
    // Equal frets on equal strings: lower string index wins.
    choice = choose_fret(open_hz[0] * std::exp2(2.0 / 12.0), c, open_hz,
                         std::array<std::size_t, 2>{3, 0});
    check(choice.string_index == 0 && choice.fret_number == 2, "lower string wins full tie");
    // Allowed strings restrict the search.
    choice = choose_fret(open_hz[0], c, open_hz, std::array<std::size_t, 1>{2});
    check(choice.string_index == 2 && choice.fret_number == 12, "allowed string 2 octave fret");
    check(!error_of([&] { (void)choose_fret(open_hz[0] * 8.0, c, open_hz); }).empty(),
          "far out of range");
    check(!error_of([&] { (void)choose_fret(open_hz[2] * 0.5, c, open_hz); }).empty(),
          "below every string");
    check(!error_of([&] { (void)choose_fret(f5, c, std::array<double, 2>{1.0, 2.0}); }).empty(),
          "open frequency count");
    check(!error_of([&] {
               (void)choose_fret(f5, c, open_hz, std::array<std::size_t, 1>{4});
           }).empty(),
          "invalid allowed string");
    check(!error_of([&] {
               (void)choose_fret(f5 * std::exp2(45.0 / 1200.0), c, open_hz,
                                 std::array<std::size_t, 1>{0}, 40.0);
           }).empty(),
          "max_abs_cents bound");
}

void course_sweep() {
    const InstrumentConfig c = fixture();
    const PerformanceSettings s = default_performance_settings();
    const Score score = parse_score("title t\nstatus synthetic\n"
                                    "strum @0.2 course=2 dir=down\n"
                                    "strum @0.5 course=2 dir=up\n"
                                    "strum @1.0 strings=1,3,0,2 dyn=ff\n"
                                    "tremolo @1.5 string=1 fret=2 dur=@0.25 rate=16\n"
                                    "end @3\n",
                                    c, s);
    const auto& g = score.gestures;
    const double d = s.course_sweep_delay_s;
    check(g[0].event.string_index == 3 && g[1].event.string_index == 2,
          "down stroke: descending index");
    near(g[0].time_s, 0.2, 1e-12, "first string at command time");
    near(g[1].time_s, 0.2 + d, 1e-12, "second string after sweep delay");
    check(g[2].event.string_index == 2 && g[3].event.string_index == 3,
          "up stroke: ascending index");
    for (std::size_t i = 0; i < 4; ++i) {
        const bool down = i < 2;
        check((std::cos(g[i].event.angle_rad) < 0.0) == down, "stroke direction sign");
    }
    const std::array<std::uint8_t, 4> expected{3, 2, 1, 0};
    for (std::size_t i = 0; i < 4; ++i) {
        check(g[4 + i].event.string_index == expected[i], "strings list sorted by sweep order");
        near(g[4 + i].time_s, 1.0 + static_cast<double>(i) * d, 1e-12, "uniform sweep delay");
        near(g[4 + i].event.force_n, s.ff.release_force_n, 0.0, "strum dynamics");
    }
    // Tremolo: lead press then floor(0.25*16) = 4 alternating strokes at 1/16 s.
    check(g[8].event.kind == GestureKind::FingerPress, "tremolo fret press");
    check(g.size() == 13, "tremolo stroke count");
    for (std::size_t i = 0; i < 4; ++i) {
        const GestureEvent& e = g[9 + i].event;
        near(g[9 + i].time_s, 1.5 + static_cast<double>(i) / 16.0, 1e-12, "tremolo period");
        check((std::cos(e.angle_rad) < 0.0) == (i % 2 == 0), "tremolo alternates down/up");
    }
}

void midi_mapper() {
    const InstrumentConfig c = fixture();
    const PerformanceSettings s = default_performance_settings();
    MidiGestureMapper a;
    MidiGestureMapper b;
    a.prepare(c, open_hz, s, 440.0);
    b.prepare(c, open_hz, s, 440.0);
    check(a.prepared(), "prepared");
    // MIDI 60 (261.63 Hz at A4 = 440) is open string 0 (lower string index wins the tie with 3).
    MidiGestureMapper::NoteRoute r60 = a.route(60);
    check(r60.valid && r60.string_index == 0 && r60.fret_number == 0, "note 60 open string 0");
    check(std::abs(r60.cents_error) < 0.1, "near-zero cents for the fixture tuning");
    const MidiGestureMapper::NoteRoute r65 = a.route(65);
    check(r65.valid && r65.fret_number == 5 && r65.string_index == 0, "note 65 = string 0 fret 5");
    check(!a.route(10).valid && !a.route(127).valid, "notes outside every string are unmappable");
    near(r65.finger_position_m, finger_position_for_fret(c, 5, s), 0.0,
         "MIDI pad uses the clamped offset");

    std::array<GestureEvent, 8> out_a{};
    std::array<GestureEvent, 8> out_b{};
    const std::size_t before = allocation_count.load();
    const std::size_t na = a.note_on(65, 100, 17, out_a);
    const std::size_t nb = b.note_on(65, 100, 17, out_b);
    const std::size_t none = a.note_on(10, 100, 0, out_a); // must not touch out_a contents count
    const std::size_t off = a.note_off(65, 40, std::span<GestureEvent>(out_a).subspan(na));
    const std::size_t stale = a.note_off(65, 41, std::span<GestureEvent>(out_a).subspan(na + off));
    const std::size_t small = b.note_on(62, 100, 0, std::span<GestureEvent>(out_b).subspan(0, 1));
    const std::size_t after = allocation_count.load();
    check(after == before, "no allocation in note_on/note_off");
    check(none == 0 && stale == 0, "unmappable note and stale note-off emit nothing");
    check(small == 0, "insufficient output capacity emits nothing");
    check(na == 2 && nb == 2, "fretted note-on: press + stroke");
    for (std::size_t i = 0; i < na; ++i) {
        check(out_a[i].kind == out_b[i].kind && out_a[i].position_m == out_b[i].position_m &&
                  out_a[i].force_n == out_b[i].force_n && out_a[i].sequence == out_b[i].sequence,
              "deterministic mapping");
        check(out_a[i].sample_offset == 17, "offset preserved");
    }
    check(out_a[0].kind == GestureKind::FingerPress && out_a[0].slot == 0, "press first");
    near(out_a[0].position_m, c.frets[4].position_m - s.finger_offset_behind_fret_m, 1e-15,
         "pad position");
    const GestureEvent& stroke = out_a[1];
    check(stroke.kind == GestureKind::NailStroke && stroke.force_n > s.mf.release_force_n &&
              stroke.force_n < s.f.release_force_n,
          "velocity 100 interpolates between mf and f");
    near(stroke.gap_m - s.nail_gap_m, stroke.speed_m_per_s * s.finger_lead_time_s, 1e-15,
         "nail lead gap gives the finger its lead time");
    check(stroke.travel_m > stroke.gap_m, "travel exceeds gap");
    check(out_a[1].sequence > out_a[0].sequence, "ascending sequence");
    check(off == 2 && out_a[2].kind == GestureKind::FingerPress && out_a[2].slot == 1 &&
              out_a[3].kind == GestureKind::FingerRelease && out_a[3].slot == 0,
          "note-off: damp pad down, fretting finger up");
    check(out_a[3].sequence > out_a[2].sequence && out_a[2].sequence > out_a[1].sequence,
          "sequence ascends across calls");
    // Next note on the same string first lifts the damping pad.
    std::array<GestureEvent, 4> next{};
    const std::size_t n2 = a.note_on(60, 1, 0, next);
    check(n2 == 2 && next[0].kind == GestureKind::FingerRelease && next[0].slot == 1 &&
              next[1].kind == GestureKind::NailStroke,
          "open note after damp: lift damp, stroke");
    near(next[1].force_n, s.pp.release_force_n, 1e-15, "velocity 1 = pp");
    const std::size_t zero_velocity = a.note_on(60, 0, 5, next);
    check(zero_velocity == 1 && next[0].kind == GestureKind::FingerPress && next[0].slot == 1,
          "velocity 0 acts as note-off");
    MidiGestureMapper unprepared;
    check(unprepared.note_on(60, 100, 0, next) == 0, "unprepared mapper emits nothing");
    check(!error_of([&] { unprepared.prepare(c, open_hz, s, -1.0); }).empty(),
          "invalid A4 rejected");
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
    const std::array<Case, 6> tests{{
        {"score.parse_valid", parse_valid},
        {"score.parse_errors", parse_errors},
        {"score.schedule_ordering", schedule_ordering},
        {"score.fret_choice", fret_choice},
        {"score.course_sweep", course_sweep},
        {"score.midi_mapper", midi_mapper},
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
