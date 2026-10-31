// setar_benchmark: wall-clock cost of Instrument::process() on THIS machine.
//
// Renders a dense deterministic gesture pattern (strokes on every string, fretted
// notes, slides, releases, course strokes) on synthetic_setar_config() for each
// sample rate x block size, and prints one CSV row per case. The numbers are a
// local measurement of this build on this computer: they are not a host, DAW or
// audio-hardware qualification and do not establish a deadline guarantee.
#include "setar/instrument/instrument.hpp"
#include "setar/instrument/setar_preset.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <iostream>
#include <span>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace {
using namespace setar::instrument;

struct Timed {
    std::uint64_t sample = 0;
    GestureEvent event{};
};

std::string compiler_identity() {
    std::ostringstream out;
#if defined(__clang__)
    out << "clang " << __clang_major__ << '.' << __clang_minor__ << '.' << __clang_patchlevel__;
#elif defined(__GNUC__)
    out << "gcc " << __GNUC__ << '.' << __GNUC_MINOR__ << '.' << __GNUC_PATCHLEVEL__;
#elif defined(_MSC_VER)
    out << "msvc " << _MSC_VER;
#else
    out << "unknown compiler";
#endif
#if defined(NDEBUG)
    out << " NDEBUG";
#else
    out << " assertions-on(likely Debug: not representative)";
#endif
    return out.str();
}

// Dense synthetic pattern: one gesture group every 60 ms cycling through strings and
// frets, so contacts, slides and re-strikes overlap. Placeholder performance values.
std::vector<Timed> dense_pattern(const InstrumentConfig& c, double rate_hz, double seconds) {
    std::vector<Timed> out;
    const auto at = [&](double t_s) { return static_cast<std::uint64_t>(std::llround(t_s * rate_hz)); };
    const std::size_t frets = c.frets.size();
    std::size_t step = 0;
    for (double t = 0.0; t < seconds; t += 0.06, ++step) {
        const std::size_t s = step % string_count;
        const StringConfig& str = c.strings[s];
        GestureEvent e;
        e.string_index = static_cast<std::uint8_t>(s);
        if (frets > 0 && step % 3 != 2) {
            const std::size_t k = (step * 5) % frets;
            const double x_fret = c.frets[k].position_m;
            e.kind = GestureKind::FingerPress;
            e.slot = 0;
            e.position_m = std::max(0.25 * x_fret, x_fret - 0.008);
            e.height_m = fret_top_height_m(str, x_fret) - 1.0e-3;
            e.speed_m_per_s = 0.2;
            e.gap_m = 1.0e-3;
            out.push_back({at(t), e});
        }
        GestureEvent n;
        n.kind = GestureKind::NailStroke;
        n.string_index = static_cast<std::uint8_t>(s);
        n.position_m = str.bridge_position_m - 0.12;
        n.angle_rad = (step % 2 == 0) ? 0.3 : 2.8;
        n.speed_m_per_s = 0.5;
        n.force_n = 0.4 + 0.1 * static_cast<double>(step % 5);
        n.gap_m = 0.5e-3;
        n.travel_m = 6.0e-3;
        out.push_back({at(t + 0.02), n});
        if (step % 4 == 3) { // course stroke on strings 2 and 3
            for (std::size_t cs : {std::size_t{2}, std::size_t{3}}) {
                GestureEvent cn = n;
                cn.string_index = static_cast<std::uint8_t>(cs);
                cn.position_m = c.strings[cs].bridge_position_m - 0.12;
                out.push_back({at(t + 0.03 + 0.002 * static_cast<double>(cs)), cn});
            }
        }
        if (frets > 0 && step % 6 == 1) {
            GestureEvent m;
            m.kind = GestureKind::FingerMove;
            m.string_index = static_cast<std::uint8_t>(s);
            m.position_m = std::max(0.25 * c.frets[0].position_m,
                                    c.frets[(step * 7) % frets].position_m - 0.008);
            m.speed_m_per_s = 0.3;
            out.push_back({at(t + 0.04), m});
        }
        if (step % 5 == 4) {
            GestureEvent r;
            r.kind = GestureKind::FingerRelease;
            r.string_index = static_cast<std::uint8_t>(s);
            r.speed_m_per_s = 0.2;
            r.gap_m = 1.0e-3;
            out.push_back({at(t + 0.05), r});
        }
    }
    std::stable_sort(out.begin(), out.end(), [](const Timed& a, const Timed& b) { return a.sample < b.sample; });
    std::uint32_t sequence = 0;
    for (auto& e : out) {
        e.event.sequence = sequence++;
    }
    return out;
}

struct Result {
    double wall_s = 0.0;
    std::vector<double> block_us;
    ProcessSummary worst{};
    std::uint64_t newton_failures = 0;
    std::uint64_t capacity_overflows = 0;
    std::uint64_t rejected = 0;
    std::uint64_t containment = 0;
    double peak = 0.0;
};

Result run_case(const InstrumentConfig& config, double rate_hz, std::uint32_t block, double seconds,
                bool ledger, PreparedInfo& info) {
    ProcessSpec spec;
    spec.sample_rate_hz = rate_hz;
    spec.max_block_frames = block;
    spec.max_events_per_block = 64;
    spec.max_interaction_channels = 32;
    spec.max_newton_iterations = 50;
    spec.newton_tolerance_m = 1.0e-14;
    spec.output_gain_per_pa = 1.0;
    spec.enable_energy_ledger = ledger;
    Instrument instrument;
    instrument.prepare(config, spec);
    info = instrument.info();
    const auto events = dense_pattern(config, rate_hz, seconds);
    const auto frames = static_cast<std::size_t>(std::llround(seconds * rate_hz));
    std::vector<float> left(block), right(block);
    std::vector<GestureEvent> pending;
    pending.reserve(events.size());
    Result r;
    r.block_us.reserve(frames / block + 1);
    std::size_t next = 0;
    const auto start_all = std::chrono::steady_clock::now();
    for (std::size_t start = 0; start < frames; start += block) {
        const std::size_t n = std::min<std::size_t>(block, frames - start);
        pending.clear();
        while (next < events.size() && events[next].sample < start + n) {
            GestureEvent e = events[next].event;
            e.sample_offset = static_cast<std::uint32_t>(events[next].sample - start);
            pending.push_back(e);
            ++next;
        }
        const auto t0 = std::chrono::steady_clock::now();
        const ProcessSummary s = instrument.process(pending, std::span<float>(left.data(), n),
                                                    std::span<float>(right.data(), n));
        const auto t1 = std::chrono::steady_clock::now();
        r.block_us.push_back(std::chrono::duration<double, std::micro>(t1 - t0).count());
        r.worst.max_newton_iterations = std::max(r.worst.max_newton_iterations, s.max_newton_iterations);
        r.worst.max_active_channels = std::max(r.worst.max_active_channels, s.max_active_channels);
        r.newton_failures += s.newton_failures;
        r.capacity_overflows += s.capacity_overflows;
        r.rejected += s.rejected_events;
        r.containment += s.containment_resets;
        for (std::size_t i = 0; i < n; ++i) {
            r.peak = std::max(r.peak, static_cast<double>(std::abs(left[i])));
        }
    }
    r.wall_s = std::chrono::duration<double>(std::chrono::steady_clock::now() - start_all).count();
    return r;
}

double percentile(std::vector<double> v, double q) {
    std::sort(v.begin(), v.end());
    const auto index = static_cast<std::size_t>(q * static_cast<double>(v.size() - 1) + 0.5);
    return v[std::min(index, v.size() - 1)];
}

void print_help() {
    std::cout << "setar_benchmark: wall time of Instrument::process on THIS machine (synthetic preset)\n"
                 "Usage: setar_benchmark [--seconds S] [--rates R1,R2,...] [--blocks B1,B2,...] [--ledger]\n"
                 "Defaults: --seconds 10 --rates 44100,48000,96000 --blocks 64,256,512\n"
                 "cpu_load = wall_time / audio_time (1.0 = exactly real time on one core).\n";
}

std::vector<double> parse_list(std::string_view text) {
    std::vector<double> out;
    std::string item;
    std::istringstream in{std::string(text)};
    while (std::getline(in, item, ',')) {
        std::size_t used = 0;
        const double v = std::stod(item, &used);
        if (used != item.size() || !std::isfinite(v) || v <= 0.0) {
            throw std::invalid_argument("invalid list value: " + item);
        }
        out.push_back(v);
    }
    if (out.empty()) {
        throw std::invalid_argument("empty list");
    }
    return out;
}

} // namespace

int main(int argc, char** argv) {
    try {
        double seconds = 10.0;
        std::vector<double> rates{44100.0, 48000.0, 96000.0};
        std::vector<double> blocks{64.0, 256.0, 512.0};
        bool ledger = false;
        for (int i = 1; i < argc; ++i) {
            const std::string_view a(argv[i]);
            const auto value = [&]() -> std::string_view {
                if (i + 1 >= argc) {
                    throw std::invalid_argument("missing value for " + std::string(a));
                }
                return argv[++i];
            };
            if (a == "--help") {
                print_help();
                return 0;
            } else if (a == "--seconds") {
                seconds = parse_list(value()).at(0);
            } else if (a == "--rates") {
                rates = parse_list(value());
            } else if (a == "--blocks") {
                blocks = parse_list(value());
            } else if (a == "--ledger") {
                ledger = true;
            } else {
                throw std::invalid_argument("unknown option " + std::string(a));
            }
        }
        const InstrumentConfig config = synthetic_setar_config();
        std::cout << "# THIS-MACHINE MEASUREMENT of a SYNTHETIC prototype; not a host/DAW qualification\n"
                  << "# preset=" << config.label << "\n# compiler=" << compiler_identity()
                  << "\n# ledger=" << (ledger ? "on" : "off") << " seconds=" << seconds << '\n'
                  << "sample_rate_hz,block_frames,string_modes_0,string_modes_1,string_modes_2,"
                     "string_modes_3,body_modes,total_dof,audio_s,wall_s,cpu_load,block_us_p50,"
                     "block_us_p99,block_us_max,block_budget_us,max_active_channels,"
                     "max_newton_iterations,newton_failures,capacity_overflows,rejected_events,"
                     "containment_resets,peak_output\n";
        for (const double rate : rates) {
            for (const double b : blocks) {
                const auto block = static_cast<std::uint32_t>(b);
                PreparedInfo info;
                const Result r = run_case(config, rate, block, seconds, ledger, info);
                const double audio_s = std::round(seconds * rate) / rate;
                std::cout << rate << ',' << block << ',' << info.string_modes[0] << ','
                          << info.string_modes[1] << ',' << info.string_modes[2] << ','
                          << info.string_modes[3] << ',' << info.body_modes << ','
                          << info.total_degrees_of_freedom << ',' << audio_s << ',' << r.wall_s << ','
                          << r.wall_s / audio_s << ',' << percentile(r.block_us, 0.50) << ','
                          << percentile(r.block_us, 0.99) << ','
                          << *std::max_element(r.block_us.begin(), r.block_us.end()) << ','
                          << 1.0e6 * static_cast<double>(block) / rate << ','
                          << r.worst.max_active_channels << ',' << r.worst.max_newton_iterations << ','
                          << r.newton_failures << ',' << r.capacity_overflows << ',' << r.rejected << ','
                          << r.containment << ',' << r.peak << '\n';
            }
        }
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "setar_benchmark: " << e.what() << '\n';
        return 1;
    }
}
