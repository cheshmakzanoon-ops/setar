// setar_play: offline renderer for the playable setar prototype.
//
// Reads a text score, maps it to physical gestures with the performance layer and
// renders the synthetic four-string instrument block by block through
// setar::instrument::Instrument::process(). Output is a SYNTHETIC render of an
// unvalidated prototype: no measured specimen, no normalization, no clipping.

#include "setar/instrument/instrument.hpp"
#include "setar/instrument/performance.hpp"
#include "setar/instrument/setar_preset.hpp"

#include <algorithm>
#include <array>
#include <bit>
#include <charconv>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <locale>
#include <numbers>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <type_traits>
#include <vector>

namespace {
namespace si = setar::instrument;

constexpr double maximum_render_seconds = 600.0;
constexpr const char* disclaimer =
    "SYNTHETIC render of an unvalidated physical-model prototype with synthetic parameters. "
    "Not a measured or validated setar, not a recording, not a fidelity claim.";

enum class Format { Float32, Pcm24 };

struct Options {
    std::filesystem::path score_path;
    std::filesystem::path output_dir;
    std::uint32_t sample_rate = 48000;
    std::uint32_t block = 256;
    std::string tuning_id;
    std::string fret_map_id;
    double gain_per_pa = 1.0;
    Format format = Format::Float32;
    bool ledger = false;
    double tail_s = 2.0;
    bool include_body = true;
    bool allow_faults = false;
};

template <typename T> T parse_number(std::string_view text, std::string_view option) {
    T value{};
    if constexpr (std::is_floating_point_v<T>) {
        // Some supported Apple libc++ toolchains lack floating from_chars.
        std::istringstream input{std::string(text)};
        input.imbue(std::locale::classic());
        input >> std::noskipws >> value;
        if (!input || !input.eof()) {
            throw std::invalid_argument("invalid number for " + std::string(option));
        }
    } else {
        const auto result = std::from_chars(text.data(), text.data() + text.size(), value);
        if (result.ec != std::errc{} || result.ptr != text.data() + text.size()) {
            throw std::invalid_argument("invalid number for " + std::string(option));
        }
    }
    return value;
}

void print_help() {
    std::cout
        << "setar_play: SYNTHETIC render of the unvalidated setar prototype\n"
           "Usage: setar_play --score PATH --output-dir DIR [options]\n"
           "  --score PATH          Text score (docs/performance/07-score-format.md)\n"
           "  --output-dir DIR      New or empty output directory (never overwritten)\n"
           "  --sample-rate HZ      Integer rate [22050, 192000] (default 48000)\n"
           "  --block FRAMES        Process block size [1, 8192] (default 256)\n"
           "  --tuning ID           Synthetic tuning preset id (default: preset default)\n"
           "  --fret-map ID         Synthetic fret map id (default: preset default)\n"
           "  --gain-per-pa G       Fixed observation gain, full scale per pascal (default 1)\n"
           "  --format F            float32 (default) or pcm24; pcm24 fails rather than clips\n"
           "  --ledger              Enable the discrete energy ledger (extra work per sample)\n"
           "  --tail SECONDS        Render time after the score end [0, 60] (default 2)\n"
           "  --no-body             Grounded bridge, no body modes (observed output is zero)\n"
           "  --allow-faults        Exit 0 even if the engine reports faults\n"
           "  --help                Show this help\n"
           "Exit: 0 ok, 2 invalid input, 1 failure, 3 engine faults (outputs written).\n"
           "No normalization, no clipping, no audio device access.\n";
}

Options parse_options(int argc, char** argv) {
    Options options;
    bool have_score = false;
    bool have_output = false;
    for (int index = 1; index < argc; ++index) {
        const std::string_view option(argv[index]);
        if (option == "--ledger") {
            options.ledger = true;
            continue;
        }
        if (option == "--no-body") {
            options.include_body = false;
            continue;
        }
        if (option == "--allow-faults") {
            options.allow_faults = true;
            continue;
        }
        if (index + 1 >= argc) {
            throw std::invalid_argument("missing value for " + std::string(option));
        }
        const std::string_view value(argv[++index]);
        if (option == "--score") {
            if (value.empty()) {
                throw std::invalid_argument("score path cannot be empty");
            }
            options.score_path = std::string(value);
            have_score = true;
        } else if (option == "--output-dir") {
            if (value.empty()) {
                throw std::invalid_argument("output directory cannot be empty");
            }
            options.output_dir = std::string(value);
            have_output = true;
        } else if (option == "--sample-rate") {
            options.sample_rate = parse_number<std::uint32_t>(value, option);
        } else if (option == "--block") {
            options.block = parse_number<std::uint32_t>(value, option);
        } else if (option == "--tuning") {
            options.tuning_id = std::string(value);
        } else if (option == "--fret-map") {
            options.fret_map_id = std::string(value);
        } else if (option == "--gain-per-pa") {
            options.gain_per_pa = parse_number<double>(value, option);
        } else if (option == "--format") {
            if (value == "float32") {
                options.format = Format::Float32;
            } else if (value == "pcm24") {
                options.format = Format::Pcm24;
            } else {
                throw std::invalid_argument("--format must be float32 or pcm24");
            }
        } else if (option == "--tail") {
            options.tail_s = parse_number<double>(value, option);
        } else {
            throw std::invalid_argument("unknown option " + std::string(option));
        }
    }
    if (!have_score || !have_output) {
        throw std::invalid_argument("--score and --output-dir are required (see --help)");
    }
    if (options.sample_rate < 22050U || options.sample_rate > 192000U) {
        throw std::invalid_argument("sample rate must be an integer in [22050, 192000] Hz");
    }
    if (options.block < 1U || options.block > 8192U) {
        throw std::invalid_argument("block size must be in [1, 8192] frames");
    }
    if (!std::isfinite(options.gain_per_pa) || options.gain_per_pa <= 0.0) {
        throw std::invalid_argument("observation gain must be finite and positive (1/Pa)");
    }
    if (!std::isfinite(options.tail_s) || options.tail_s < 0.0 || options.tail_s > 60.0) {
        throw std::invalid_argument("tail must be finite and in [0, 60] seconds");
    }
    return options;
}

std::string read_score(const std::filesystem::path& path) {
    std::ifstream stream(path, std::ios::binary);
    if (!stream) {
        throw std::invalid_argument("cannot open score " + path.string());
    }
    return std::string(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>());
}

void require_empty_output(const std::filesystem::path& dir) {
    if (!std::filesystem::exists(dir)) {
        return;
    }
    if (!std::filesystem::is_directory(dir)) {
        throw std::invalid_argument("output path exists and is not a directory: " + dir.string());
    }
    if (!std::filesystem::is_empty(dir)) {
        throw std::invalid_argument("refusing to write into non-empty output directory " +
                                    dir.string());
    }
}

// ---- JSON helpers (strict: non-finite numbers become null) -------------------------

std::string json_string(std::string_view text) {
    std::string out = "\"";
    for (const char raw : text) {
        const auto c = static_cast<unsigned char>(raw);
        switch (raw) {
        case '"':
            out += "\\\"";
            break;
        case '\\':
            out += "\\\\";
            break;
        case '\n':
            out += "\\n";
            break;
        case '\r':
            out += "\\r";
            break;
        case '\t':
            out += "\\t";
            break;
        default:
            if (c < 0x20U) {
                static constexpr char hex[] = "0123456789abcdef";
                out += "\\u00";
                out += hex[(c >> 4U) & 0xfU];
                out += hex[c & 0xfU];
            } else {
                out += raw;
            }
        }
    }
    out += '"';
    return out;
}

std::string json_number(double value) {
    if (!std::isfinite(value)) {
        return "null";
    }
    std::ostringstream stream;
    stream.imbue(std::locale::classic());
    stream << std::setprecision(17) << value;
    return stream.str();
}

const char* provenance_name(si::Provenance p) {
    switch (p) {
    case si::Provenance::Synthetic:
        return "synthetic";
    case si::Provenance::Published:
        return "published";
    case si::Provenance::Measured:
        return "measured";
    case si::Provenance::Inferred:
        return "inferred";
    }
    return "unknown";
}

const char* fault_name(si::FaultCode code) {
    switch (code) {
    case si::FaultCode::None:
        return "none";
    case si::FaultCode::NotPrepared:
        return "not_prepared";
    case si::FaultCode::InvalidBlock:
        return "invalid_block";
    case si::FaultCode::InvalidEvent:
        return "invalid_event";
    case si::FaultCode::EventOverflow:
        return "event_overflow";
    case si::FaultCode::ChannelCapacityExceeded:
        return "channel_capacity_exceeded";
    case si::FaultCode::NewtonNotConverged:
        return "newton_not_converged";
    case si::FaultCode::NonFiniteState:
        return "non_finite_state";
    }
    return "unknown";
}

const char* nail_phase_name(si::NailPhase phase) {
    switch (phase) {
    case si::NailPhase::Idle:
        return "idle";
    case si::NailPhase::Approaching:
        return "approaching";
    case si::NailPhase::InContact:
        return "in_contact";
    case si::NailPhase::Released:
        return "released";
    }
    return "unknown";
}

// ---- WAV writing ---------------------------------------------------------------------

void write_u16(std::ostream& stream, std::uint16_t value) {
    const char bytes[2] = {static_cast<char>(value & 0xffU),
                           static_cast<char>((value >> 8U) & 0xffU)};
    stream.write(bytes, 2);
}

void write_u32(std::ostream& stream, std::uint32_t value) {
    const char bytes[4] = {static_cast<char>(value & 0xffU),
                           static_cast<char>((value >> 8U) & 0xffU),
                           static_cast<char>((value >> 16U) & 0xffU),
                           static_cast<char>((value >> 24U) & 0xffU)};
    stream.write(bytes, 4);
}

// Interleaved stereo. float32 uses WAVE_FORMAT_IEEE_FLOAT with a fact chunk; pcm24 is
// integer PCM with full scale 8388607 at |x| = 1 (the caller has rejected |x| > 1).
void write_wav(const std::filesystem::path& path, const std::vector<float>& left,
               const std::vector<float>& right, std::uint32_t sample_rate, Format format) {
    std::ofstream stream(path, std::ios::binary);
    stream.exceptions(std::ios::failbit | std::ios::badbit);
    const auto frames = static_cast<std::uint32_t>(left.size());
    const std::uint16_t channels = 2;
    const auto bytes_per_sample = static_cast<std::uint16_t>(format == Format::Float32 ? 4U : 3U);
    const auto block_align = static_cast<std::uint16_t>(channels * bytes_per_sample);
    const std::uint32_t data_size = frames * block_align;
    const bool is_float = format == Format::Float32;
    const std::uint32_t fmt_size = is_float ? 18U : 16U;
    const std::uint32_t fact_size = is_float ? 12U : 0U;
    stream.write("RIFF", 4);
    write_u32(stream, 4U + (8U + fmt_size) + fact_size + 8U + data_size);
    stream.write("WAVE", 4);
    stream.write("fmt ", 4);
    write_u32(stream, fmt_size);
    write_u16(stream, static_cast<std::uint16_t>(is_float ? 3U : 1U));
    write_u16(stream, channels);
    write_u32(stream, sample_rate);
    write_u32(stream, sample_rate * block_align);
    write_u16(stream, block_align);
    write_u16(stream, static_cast<std::uint16_t>(bytes_per_sample * 8U));
    if (is_float) {
        write_u16(stream, 0U); // cbSize
        stream.write("fact", 4);
        write_u32(stream, 4U);
        write_u32(stream, frames);
    }
    stream.write("data", 4);
    write_u32(stream, data_size);
    std::vector<char> bytes(static_cast<std::size_t>(data_size));
    std::size_t at = 0;
    for (std::size_t i = 0; i < left.size(); ++i) {
        for (const float sample : {left[i], right[i]}) {
            if (is_float) {
                const auto bits = std::bit_cast<std::uint32_t>(sample);
                for (unsigned shift = 0; shift < 32U; shift += 8U) {
                    bytes[at++] = static_cast<char>((bits >> shift) & 0xffU);
                }
            } else {
                const auto pcm = static_cast<std::int32_t>(
                    std::lround(static_cast<double>(sample) * 8388607.0));
                const auto bits = static_cast<std::uint32_t>(pcm);
                for (unsigned shift = 0; shift < 24U; shift += 8U) {
                    bytes[at++] = static_cast<char>((bits >> shift) & 0xffU);
                }
            }
        }
    }
    stream.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
    stream.close();
}

std::ofstream open_text(const std::filesystem::path& path) {
    std::ofstream stream(path, std::ios::binary);
    stream.exceptions(std::ios::failbit | std::ios::badbit);
    stream.imbue(std::locale::classic());
    stream << std::setprecision(17);
    return stream;
}

// ---- Nominal pitch bookkeeping (descriptive only) -------------------------------------

// Ideal simply supported stiff-string fundamental for speaking length L (Hz).
double nominal_frequency_hz(const si::StringConfig& s, double speaking_length_m) {
    const double k = std::numbers::pi_v<double> / speaking_length_m;
    const double omega2 = (s.tension_n * k * k + s.bending_stiffness_n_m2 * k * k * k * k) /
                          s.linear_density_kg_per_m;
    return std::sqrt(omega2) / (2.0 * std::numbers::pi_v<double>);
}

struct FingerTrack {
    bool active = false;
    double position_m = 0.0;
    double height_m = 0.0;
};

struct StrokeRecord {
    double time_s = 0.0;
    std::size_t string_index = 0;
    double position_m = 0.0;
    double angle_rad = 0.0;
    double speed_m_per_s = 0.0;
    double release_force_n = 0.0;
    std::string stop; // "open", "fret", "touch"
    std::size_t fret_number = 0;
    double speaking_length_m = 0.0;
    std::optional<double> nominal_hz;
};

// Replays finger gestures to label each nail stroke with the stopping state implied by
// the score. The engine's realized pitch is measured separately by the analyzer.
std::vector<StrokeRecord> describe_strokes(const std::vector<si::ScheduledGesture>& gestures,
                                           const si::InstrumentConfig& config,
                                           double sample_rate) {
    std::array<std::array<FingerTrack, 2>, si::string_count> fingers{};
    std::vector<StrokeRecord> strokes;
    for (const auto& scheduled : gestures) {
        const si::GestureEvent& e = scheduled.event;
        if (e.string_index >= si::string_count) {
            continue;
        }
        const std::size_t s = e.string_index;
        const std::size_t slot = e.slot < 2 ? e.slot : 0;
        switch (e.kind) {
        case si::GestureKind::FingerPress:
            fingers[s][slot] = FingerTrack{true, e.position_m, e.height_m};
            break;
        case si::GestureKind::FingerMove:
            fingers[s][slot].position_m = e.position_m;
            break;
        case si::GestureKind::FingerRelease:
            fingers[s][slot].active = false;
            break;
        case si::GestureKind::Silence:
            fingers = {};
            break;
        case si::GestureKind::NailStroke: {
            const si::StringConfig& string = config.strings[s];
            StrokeRecord r;
            r.time_s = static_cast<double>(scheduled.sample_index) / sample_rate;
            r.string_index = s;
            r.position_m = e.position_m;
            r.angle_rad = e.angle_rad;
            r.speed_m_per_s = e.speed_m_per_s;
            r.release_force_n = e.force_n;
            r.stop = "open";
            r.speaking_length_m = string.bridge_position_m;
            bool touched = false;
            double front = -1.0;
            for (const FingerTrack& f : fingers[s]) {
                if (!f.active) {
                    continue;
                }
                if (f.height_m < si::fret_top_height_m(string, f.position_m)) {
                    front = std::max(front, f.position_m);
                } else {
                    touched = true;
                }
            }
            if (front >= 0.0) {
                for (std::size_t k = 0; k < config.frets.size(); ++k) {
                    if (config.frets[k].position_m > front) {
                        r.stop = "fret";
                        r.fret_number = k + 1;
                        r.speaking_length_m = string.bridge_position_m - config.frets[k].position_m;
                        break;
                    }
                }
            }
            if (r.stop == "open" && touched) {
                r.stop = "touch"; // light touch: harmonic or damping, no nominal pitch
            } else {
                r.nominal_hz = nominal_frequency_hz(string, r.speaking_length_m);
            }
            strokes.push_back(r);
            break;
        }
        }
    }
    return strokes;
}

// ---- Rendering ----------------------------------------------------------------------

struct Totals {
    si::FaultCode first_fault = si::FaultCode::None;
    std::uint64_t rejected_events = 0;
    std::uint64_t newton_failures = 0;
    std::uint64_t capacity_overflows = 0;
    std::uint64_t containment_resets = 0;
    std::uint32_t max_newton_iterations = 0;
    std::uint32_t max_active_channels = 0;
    std::uint64_t blocks_with_faults = 0;
    void add(const si::ProcessSummary& s) {
        if (first_fault == si::FaultCode::None) {
            first_fault = s.first_fault;
        }
        rejected_events += s.rejected_events;
        newton_failures += s.newton_failures;
        capacity_overflows += s.capacity_overflows;
        containment_resets += s.containment_resets;
        max_newton_iterations = std::max(max_newton_iterations, s.max_newton_iterations);
        max_active_channels = std::max(max_active_channels, s.max_active_channels);
        if (s.first_fault != si::FaultCode::None) {
            ++blocks_with_faults;
        }
    }
    [[nodiscard]] bool faulted() const {
        return first_fault != si::FaultCode::None || rejected_events != 0 ||
               newton_failures != 0 || capacity_overflows != 0 || containment_resets != 0;
    }
};

int render(const Options& options) {
    require_empty_output(options.output_dir);
    const std::string score_text = read_score(options.score_path);

    si::PresetOptions preset;
    preset.tuning_id = options.tuning_id;
    preset.fret_map_id = options.fret_map_id;
    preset.include_body = options.include_body;
    const si::InstrumentConfig config = si::synthetic_setar_config(preset);
    std::optional<si::TuningPreset> tuning;
    std::optional<si::FretMapDefinition> fret_map;
    // Resolve defaults to their records so the summary always names what was rendered.
    tuning = si::find_tuning(options.tuning_id.empty() ? si::default_tuning_id
                                                       : std::string_view(options.tuning_id));
    fret_map = si::find_fret_map(options.fret_map_id.empty() ? si::default_fret_map_id
                                                             : std::string_view(options.fret_map_id));
    const si::PerformanceSettings settings = si::default_performance_settings();
    const si::Score score = si::parse_score(score_text, config, settings);
    const double rate = static_cast<double>(options.sample_rate);
    const std::vector<si::ScheduledGesture> scheduled = si::schedule(score, rate);

    double last_time = score.end_time_s;
    std::uint64_t last_index = 0;
    for (const auto& g : scheduled) {
        last_index = std::max(last_index, g.sample_index);
        last_time = std::max(last_time, static_cast<double>(g.sample_index) / rate);
    }
    if (!std::isfinite(last_time) || last_time < 0.0 ||
        last_time + options.tail_s > maximum_render_seconds) {
        throw std::invalid_argument("score end plus tail must be finite and at most 600 s");
    }
    std::uint64_t frames =
        static_cast<std::uint64_t>(std::ceil((last_time + options.tail_s) * rate));
    if (!scheduled.empty()) {
        frames = std::max(frames, last_index + 1U);
    }
    if (frames == 0) {
        throw std::invalid_argument("score and tail produce no samples; give --tail > 0");
    }
    const auto frame_count = static_cast<std::size_t>(frames);

    // Event capacity is chosen so the engine never has to drop a scheduled gesture.
    std::uint32_t needed_events = 0;
    {
        std::size_t g = 0;
        for (std::uint64_t start = 0; start < frames; start += options.block) {
            std::uint32_t in_block = 0;
            while (g < scheduled.size() && scheduled[g].sample_index < start + options.block) {
                ++in_block;
                ++g;
            }
            needed_events = std::max(needed_events, in_block);
        }
    }
    si::ProcessSpec spec;
    spec.sample_rate_hz = rate;
    spec.max_block_frames = options.block;
    spec.max_events_per_block = std::max<std::uint32_t>(64U, needed_events);
    spec.output_gain_per_pa = options.gain_per_pa;
    spec.enable_energy_ledger = options.ledger;

    si::Instrument instrument;
    instrument.prepare(config, spec);
    const si::PreparedInfo info = instrument.info();
    const std::vector<StrokeRecord> strokes = describe_strokes(scheduled, config, rate);

    std::vector<float> left(frame_count, 0.0F);
    std::vector<float> right(frame_count, 0.0F);
    std::vector<si::GestureEvent> events;
    events.reserve(spec.max_events_per_block);

    std::ostringstream diag;
    diag.imbue(std::locale::classic());
    diag << std::setprecision(17);
    diag << "block,start_sample,frames,time_end_s";
    for (std::size_t s = 0; s < si::string_count; ++s) {
        diag << ",string" << s << "_energy_j";
    }
    diag << ",body_energy_j,active_channels,max_newton_iterations,newton_failures,"
            "capacity_overflows,rejected_events,containment_resets,fault";
    for (std::size_t s = 0; s < si::string_count; ++s) {
        diag << ",nail_phase_" << s << ",nail_force_" << s << "_n";
    }
    for (std::size_t s = 0; s < si::string_count; ++s) {
        for (std::size_t k = 0; k < 2; ++k) {
            diag << ",finger_force_s" << s << "_k" << k << "_n";
        }
    }
    if (options.ledger) {
        diag << ",ledger_stored_j,ledger_interaction_potential_j,ledger_external_work_j,"
                "ledger_dissipation_j,ledger_switching_j,ledger_residual_j";
    }
    diag << ",wall_s\n";

    Totals totals;
    double wall_total = 0.0;
    double wall_block_max = 0.0;
    double residual_min = std::numeric_limits<double>::infinity();
    double residual_max = -std::numeric_limits<double>::infinity();
    double stored_max = 0.0;
    si::EnergyLedger last_ledger{};
    std::size_t next_event = 0;
    std::uint64_t block_index = 0;
    for (std::uint64_t start = 0; start < frames; start += options.block, ++block_index) {
        const auto n = static_cast<std::size_t>(std::min<std::uint64_t>(options.block, frames - start));
        events.clear();
        while (next_event < scheduled.size() && scheduled[next_event].sample_index < start + n) {
            si::GestureEvent e = scheduled[next_event].event;
            e.sample_offset = static_cast<std::uint32_t>(scheduled[next_event].sample_index - start);
            events.push_back(e);
            ++next_event;
        }
        const auto offset = static_cast<std::size_t>(start);
        const auto t0 = std::chrono::steady_clock::now();
        const si::ProcessSummary summary = instrument.process(
            events, std::span<float>(left.data() + offset, n), std::span<float>(right.data() + offset, n));
        const auto t1 = std::chrono::steady_clock::now();
        const double wall = std::chrono::duration<double>(t1 - t0).count();
        wall_total += wall;
        wall_block_max = std::max(wall_block_max, wall);
        totals.add(summary);
        if (summary.processed_frames != n && summary.first_fault == si::FaultCode::None) {
            throw std::runtime_error("engine processed fewer frames than requested without a fault");
        }

        const si::InstrumentSnapshot snap = instrument.snapshot();
        diag << block_index << ',' << start << ',' << n << ','
             << static_cast<double>(start + n) / rate;
        for (const auto& s : snap.strings) {
            diag << ',' << s.energy_j;
        }
        diag << ',' << snap.body_energy_j << ',' << snap.active_channels << ','
             << summary.max_newton_iterations << ',' << summary.newton_failures << ','
             << summary.capacity_overflows << ',' << summary.rejected_events << ','
             << summary.containment_resets << ',' << fault_name(summary.first_fault);
        for (const auto& s : snap.strings) {
            diag << ',' << nail_phase_name(s.nail_phase) << ',' << s.nail_force_n;
        }
        for (const auto& s : snap.strings) {
            diag << ',' << s.finger_force_n[0] << ',' << s.finger_force_n[1];
        }
        if (options.ledger) {
            const si::EnergyLedger l = instrument.ledger();
            double stored = l.body_energy_j + l.interaction_potential_j;
            for (const double e : l.string_energy_j) {
                stored += e;
            }
            residual_min = std::min(residual_min, l.residual_j);
            residual_max = std::max(residual_max, l.residual_j);
            stored_max = std::max(stored_max, stored);
            last_ledger = l;
            diag << ',' << stored << ',' << l.interaction_potential_j << ',' << l.external_work_j
                 << ',' << l.dissipation_j << ',' << l.switching_j << ',' << l.residual_j;
        }
        diag << ',' << wall << '\n';
    }
    if (next_event != scheduled.size()) {
        throw std::runtime_error("internal error: scheduled gestures were not all delivered");
    }

    double peak_left = 0.0;
    double peak_right = 0.0;
    for (std::size_t i = 0; i < frame_count; ++i) {
        const double l = static_cast<double>(left[i]);
        const double r = static_cast<double>(right[i]);
        if (!std::isfinite(l) || !std::isfinite(r)) {
            throw std::runtime_error("non-finite output sample; no audio was written");
        }
        peak_left = std::max(peak_left, std::abs(l));
        peak_right = std::max(peak_right, std::abs(r));
    }
    if (options.format == Format::Pcm24 && std::max(peak_left, peak_right) > 1.0) {
        std::ostringstream message;
        message << "pcm24 output would exceed full scale (peak " << std::max(peak_left, peak_right)
                << "); lower --gain-per-pa or use float32 (no clipping, no normalization)";
        throw std::invalid_argument(message.str());
    }

    std::filesystem::create_directories(options.output_dir);
    write_wav(options.output_dir / "render.wav", left, right, options.sample_rate, options.format);
    {
        auto file = open_text(options.output_dir / "diagnostics.csv");
        file << diag.str();
        file.close();
    }

    const double audio_s = static_cast<double>(frames) / rate;
    std::ostringstream j;
    j.imbue(std::locale::classic());
    j << "{\n";
    j << "  \"schema_version\": 1,\n";
    j << "  \"tool\": \"setar_play\",\n";
    j << "  \"evidence_label\": \"synthetic_unvalidated_prototype_render\",\n";
    j << "  \"disclaimer\": " << json_string(disclaimer) << ",\n";
    j << "  \"status\": " << json_string(totals.faulted() ? "engine_faults" : "ok") << ",\n";
    j << "  \"score\": {\"path\": " << json_string(options.score_path.generic_string())
      << ", \"title\": " << json_string(score.title) << ", \"status\": "
      << json_string(score.status) << ", \"end_time_s\": " << json_number(score.end_time_s)
      << ", \"gesture_count\": " << scheduled.size() << "},\n";
    j << "  \"preset\": {\n";
    j << "    \"tuning_id\": " << (tuning ? json_string(tuning->id) : "null")
      << ", \"tuning_status\": " << (tuning ? json_string(tuning->status) : "null")
      << ", \"tuning_description\": " << (tuning ? json_string(tuning->description) : "null")
      << ",\n";
    j << "    \"fret_map_id\": " << (fret_map ? json_string(fret_map->id) : "null")
      << ", \"fret_map_status\": " << (fret_map ? json_string(fret_map->status) : "null")
      << ", \"fret_map_description\": "
      << (fret_map ? json_string(fret_map->description) : "null") << ",\n";
    j << "    \"defaults_used\": " << (options.tuning_id.empty() || options.fret_map_id.empty() ? "true" : "false")
      << ", \"include_body\": " << (options.include_body ? "true" : "false") << "\n  },\n";
    j << "  \"config\": {\n";
    j << "    \"label\": " << json_string(config.label) << ", \"provenance\": "
      << json_string(provenance_name(config.provenance)) << ",\n";
    j << "    \"body_label\": " << json_string(config.body.label) << ", \"body_provenance\": "
      << json_string(provenance_name(config.body.provenance))
      << ", \"body_modes\": " << config.body.modes.size()
      << ", \"fret_count\": " << config.frets.size()
      << ", \"max_mode_frequency_hz\": " << json_number(config.max_mode_frequency_hz) << ",\n";
    j << "    \"strings\": [\n";
    for (std::size_t s = 0; s < si::string_count; ++s) {
        const si::StringConfig& c = config.strings[s];
        j << "      {\"index\": " << s << ", \"label\": " << json_string(c.label)
          << ", \"total_length_m\": " << json_number(c.total_length_m)
          << ", \"bridge_position_m\": " << json_number(c.bridge_position_m)
          << ", \"tension_n\": " << json_number(c.tension_n)
          << ", \"linear_density_kg_per_m\": " << json_number(c.linear_density_kg_per_m)
          << ", \"bending_stiffness_n_m2\": " << json_number(c.bending_stiffness_n_m2)
          << ", \"nominal_open_frequency_hz\": "
          << json_number(nominal_frequency_hz(c, c.bridge_position_m)) << "}"
          << (s + 1 < si::string_count ? ",\n" : "\n");
    }
    j << "    ]\n  },\n";
    j << "  \"engine\": {\"string_modes_per_polarization\": [";
    for (std::size_t s = 0; s < si::string_count; ++s) {
        j << info.string_modes[s] << (s + 1 < si::string_count ? ", " : "");
    }
    j << "], \"body_modes\": " << info.body_modes
      << ", \"total_degrees_of_freedom\": " << info.total_degrees_of_freedom
      << ", \"max_interaction_channels\": " << spec.max_interaction_channels
      << ", \"max_newton_iterations\": " << spec.max_newton_iterations
      << ", \"newton_tolerance_m\": " << json_number(spec.newton_tolerance_m) << "},\n";
    j << "  \"render\": {\"sample_rate_hz\": " << options.sample_rate
      << ", \"block_frames\": " << options.block << ", \"frames\": " << frames
      << ", \"duration_s\": " << json_number(audio_s) << ", \"tail_s\": "
      << json_number(options.tail_s) << ", \"format\": "
      << json_string(options.format == Format::Float32 ? "float32" : "pcm24")
      << ", \"channels\": 2, \"gain_per_pa\": " << json_number(options.gain_per_pa)
      << ", \"normalization\": \"none_fixed_gain\", \"clipping\": \"none\""
      << ", \"observation\": \"body_radiation_model_pressure_pa_times_gain\""
      << ", \"ledger_enabled\": " << (options.ledger ? "true" : "false")
      << ", \"max_events_per_block\": " << spec.max_events_per_block << "},\n";
    j << "  \"output\": {\"peak_left\": " << json_number(peak_left)
      << ", \"peak_right\": " << json_number(peak_right) << "},\n";
    j << "  \"faults\": {\"first_fault\": " << json_string(fault_name(totals.first_fault))
      << ", \"rejected_events\": " << totals.rejected_events
      << ", \"newton_failures\": " << totals.newton_failures
      << ", \"capacity_overflows\": " << totals.capacity_overflows
      << ", \"containment_resets\": " << totals.containment_resets
      << ", \"max_newton_iterations\": " << totals.max_newton_iterations
      << ", \"max_active_channels\": " << totals.max_active_channels
      << ", \"blocks_with_faults\": " << totals.blocks_with_faults << "},\n";
    j << "  \"timing\": {\"label\": "
      << json_string("wall-clock measurement on this machine for this run only; not a "
                     "realtime qualification")
      << ", \"wall_clock_s\": " << json_number(wall_total) << ", \"audio_s\": "
      << json_number(audio_s) << ", \"speed_vs_realtime\": "
      << json_number(wall_total > 0.0 ? audio_s / wall_total
                                      : std::numeric_limits<double>::quiet_NaN())
      << ", \"max_block_wall_s\": " << json_number(wall_block_max)
      << ", \"block_duration_s\": " << json_number(static_cast<double>(options.block) / rate)
      << "},\n";
    if (options.ledger) {
        double final_stored = last_ledger.body_energy_j + last_ledger.interaction_potential_j;
        for (const double e : last_ledger.string_energy_j) {
            final_stored += e;
        }
        j << "  \"ledger\": {\"enabled\": true, \"residual_min_j\": " << json_number(residual_min)
          << ", \"residual_max_j\": " << json_number(residual_max)
          << ", \"max_abs_residual_j\": "
          << json_number(std::max(std::abs(residual_min), std::abs(residual_max)))
          << ", \"max_stored_j\": " << json_number(stored_max)
          << ", \"final_stored_j\": " << json_number(final_stored)
          << ", \"final_external_work_j\": " << json_number(last_ledger.external_work_j)
          << ", \"final_dissipation_j\": " << json_number(last_ledger.dissipation_j)
          << ", \"final_switching_j\": " << json_number(last_ledger.switching_j)
          << ", \"initial_stored_j\": " << json_number(last_ledger.initial_stored_j) << "},\n";
    } else {
        j << "  \"ledger\": {\"enabled\": false},\n";
    }
    j << "  \"strokes_label\": "
      << json_string("nominal_hz is the ideal simply supported stiff-string fundamental of the "
                     "speaking length implied by the score, not a measurement")
      << ",\n";
    j << "  \"strokes\": [";
    for (std::size_t i = 0; i < strokes.size(); ++i) {
        const StrokeRecord& r = strokes[i];
        j << (i == 0 ? "\n" : ",\n") << "    {\"time_s\": " << json_number(r.time_s)
          << ", \"string\": " << r.string_index << ", \"position_m\": "
          << json_number(r.position_m) << ", \"angle_rad\": " << json_number(r.angle_rad)
          << ", \"speed_m_per_s\": " << json_number(r.speed_m_per_s)
          << ", \"release_force_n\": " << json_number(r.release_force_n)
          << ", \"stop\": " << json_string(r.stop) << ", \"fret\": " << r.fret_number
          << ", \"speaking_length_m\": " << json_number(r.speaking_length_m)
          << ", \"nominal_hz\": "
          << (r.nominal_hz ? json_number(*r.nominal_hz) : std::string("null")) << "}";
    }
    j << (strokes.empty() ? "]\n" : "\n  ]\n");
    j << "}\n";
    {
        auto file = open_text(options.output_dir / "summary.json");
        file << j.str();
        file.close();
    }

    std::cout << "SYNTHETIC prototype render: " << frames << " frames at " << options.sample_rate
              << " Hz (" << std::setprecision(6) << audio_s << " s), " << scheduled.size()
              << " gestures.\n"
              << "Peak L/R (fixed gain, not normalized): " << peak_left << " / " << peak_right
              << "; speed vs realtime on this machine: "
              << (wall_total > 0.0 ? audio_s / wall_total : 0.0) << "x.\n"
              << "Output: " << options.output_dir.string() << '\n'
              << "Not a measured or validated setar.\n";
    if (totals.faulted()) {
        std::cerr << "Engine faults: first=" << fault_name(totals.first_fault)
                  << " rejected_events=" << totals.rejected_events
                  << " newton_failures=" << totals.newton_failures
                  << " capacity_overflows=" << totals.capacity_overflows
                  << " containment_resets=" << totals.containment_resets << '\n';
        return options.allow_faults ? 0 : 3;
    }
    return 0;
}
} // namespace

int main(int argc, char** argv) {
    if (argc == 2 && std::string_view(argv[1]) == "--help") {
        print_help();
        return 0;
    }
    try {
        return render(parse_options(argc, argv));
    } catch (const std::invalid_argument& error) {
        std::cerr << "Invalid input: " << error.what() << '\n';
        return 2;
    } catch (const std::exception& error) {
        std::cerr << "Render failed: " << error.what() << '\n';
        return 1;
    }
}
