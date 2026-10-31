#include "setar/instrument/setar_preset.hpp"

#include <cmath>
#include <numbers>
#include <stdexcept>
#include <string>
#include <utility>

// Every numeric value in this file is a SYNTHETIC placeholder documented, with its
// rationale and admissible range, in docs/strings/08-prototype-string-and-fret-presets.md.

namespace setar::instrument {
namespace {
constexpr double pi = std::numbers::pi_v<double>;

void require(bool valid, const std::string& message) {
    if (!valid) {
        throw std::invalid_argument(message);
    }
}

bool positive(double value) { return std::isfinite(value) && value > 0.0; }

// Twelve-tone equal-temperament C4 for A4 = 440 Hz: a convenience reference only.
const double reference_c4_hz = 440.0 * std::pow(2.0, -9.0 / 12.0);

const std::vector<TuningPreset>& tuning_table() {
    static const std::vector<TuningPreset> table{
        {std::string(default_tuning_id),
         "Strings 0..3 = C4, G3 (pure fourth below string 0), C3 and C4 (string 3 an octave "
         "above string 2 as the paired course); C4 from A4 = 440 Hz",
         "synthetic educational example, not a documented performer tuning",
         {reference_c4_hz, reference_c4_hz * 3.0 / 4.0, reference_c4_hz / 2.0, reference_c4_hz}},
        {"synthetic-c4-f3-c3-c4",
         "Strings 0..3 = C4, F3 (pure fifth below string 0), C3 and C4 (octave pair); "
         "C4 from A4 = 440 Hz",
         "synthetic educational example, not a documented performer tuning",
         {reference_c4_hz, reference_c4_hz * 2.0 / 3.0, reference_c4_hz / 2.0, reference_c4_hz}},
    };
    return table;
}

FretInterval rational(std::uint64_t numerator, std::uint64_t denominator) {
    return FretInterval{std::to_string(numerator) + "/" + std::to_string(denominator),
                        static_cast<double>(numerator) / static_cast<double>(denominator),
                        numerator, denominator};
}

// Pythagorean chain of fifths 3^k (k = -12..4) reduced into one octave, in ascending
// order, plus the octave: 17 steps of limma (256/243) or comma (531441/524288).
// The second octave repeats the first eight intervals doubled, giving 25 frets.
std::vector<FretInterval> pythagorean_17_intervals() {
    const std::pair<std::uint64_t, std::uint64_t> octave[] = {
        {256, 243},   {65536, 59049},    {9, 8}, {32, 27},  {8192, 6561},   {81, 64}, {4, 3},
        {1024, 729},  {262144, 177147},  {3, 2}, {128, 81}, {32768, 19683}, {27, 16}, {16, 9},
        {4096, 2187}, {1048576, 531441}, {2, 1}};
    std::vector<FretInterval> out;
    for (const auto& [n, d] : octave) {
        out.push_back(rational(n, d));
    }
    for (std::size_t i = 0; i < 8; ++i) {
        const auto& [n, d] = octave[i];
        // Doubling keeps the fraction reduced because every denominator is odd.
        out.push_back(rational(2 * n, d));
    }
    return out;
}

std::vector<FretInterval> edo24_intervals() {
    std::vector<FretInterval> out;
    for (int step = 2; step <= 26; ++step) {
        out.push_back(FretInterval{"24-EDO step " + std::to_string(step) + " (" +
                                       std::to_string(50 * step) + " cents)",
                                   std::pow(2.0, static_cast<double>(step) / 24.0), 0, 0});
    }
    return out;
}

const std::vector<FretMapDefinition>& fret_map_table() {
    static const std::vector<FretMapDefinition> table{
        {std::string(default_fret_map_id),
         "17 Pythagorean intervals per octave (chain of fifths 3^-12..3^4; limma/comma steps) "
         "as reconstructed from Safi al-Din Urmawi's 13th-century theory, extended by eight "
         "doubled intervals into the second octave (25 frets)",
         "historical theory interval set used as a synthetic fret map; not a measured setar "
         "fret map and not a statement of performance practice",
         pythagorean_17_intervals()},
        {"edo24-quarter-tone-grid",
         "Every 24-tone equal division step from 100 to 1300 cents (25 frets)",
         "equal-division approximation for engineering tests only; synthetic, not a setar "
         "fret map",
         edo24_intervals()},
    };
    return table;
}

double option_or_default(double value, double fallback, const char* field) {
    if (value == 0.0) {
        return fallback;
    }
    require(positive(value), std::string("preset option ") + field +
                                 " must be 0 (default) or finite and positive (m)");
    return value;
}

} // namespace

std::span<const TuningPreset> tuning_presets() { return tuning_table(); }

std::span<const FretMapDefinition> fret_map_definitions() { return fret_map_table(); }

const TuningPreset& find_tuning(std::string_view id) {
    for (const auto& tuning : tuning_table()) {
        if (tuning.id == id) {
            return tuning;
        }
    }
    throw std::invalid_argument("unknown tuning preset: " + std::string(id));
}

const FretMapDefinition& find_fret_map(std::string_view id) {
    for (const auto& map : fret_map_table()) {
        if (map.id == id) {
            return map;
        }
    }
    throw std::invalid_argument("unknown fret map: " + std::string(id));
}

std::array<StringSpec, string_count> synthetic_string_specs() {
    const StringMaterial steel{"synthetic plain steel", 7850.0, 2.0e11};
    const StringMaterial brass{"synthetic brass-like copper alloy", 8500.0, 1.0e11};
    return {{{"string 0 (synthetic plain steel, 0.20 mm)", steel, 0.20e-3},
             {"string 1 (synthetic plain steel, 0.25 mm)", steel, 0.25e-3},
             {"string 2 (synthetic brass-like, 0.35 mm)", brass, 0.35e-3},
             {"string 3 (synthetic plain steel, 0.20 mm)", steel, 0.20e-3}}};
}

std::vector<Course> setar_courses() {
    return {{"course 1 (string 0)", {0}},
            {"course 2 (string 1)", {1}},
            {"course 3 (strings 2 and 3)", {2, 3}}};
}

double linear_density_kg_per_m(const StringSpec& spec) {
    require(positive(spec.diameter_m), "string diameter must be finite and positive (m)");
    require(positive(spec.material.density_kg_per_m3),
            "string density must be finite and positive (kg/m^3)");
    return spec.material.density_kg_per_m3 * pi * spec.diameter_m * spec.diameter_m / 4.0;
}

double bending_stiffness_n_m2(const StringSpec& spec) {
    require(positive(spec.diameter_m), "string diameter must be finite and positive (m)");
    require(positive(spec.material.youngs_modulus_pa),
            "string Young's modulus must be finite and positive (Pa)");
    const double d2 = spec.diameter_m * spec.diameter_m;
    return spec.material.youngs_modulus_pa * pi * d2 * d2 / 64.0;
}

double tension_for_frequency(double speaking_length_m, double linear_density_kg_per_m,
                             double bending_stiffness_n_m2, double frequency_hz) {
    require(positive(speaking_length_m), "speaking length must be finite and positive (m)");
    require(positive(linear_density_kg_per_m), "linear density must be finite and positive (kg/m)");
    require(std::isfinite(bending_stiffness_n_m2) && bending_stiffness_n_m2 >= 0.0,
            "bending stiffness must be finite and nonnegative (N m^2)");
    require(positive(frequency_hz), "target frequency must be finite and positive (Hz)");
    const double wave_speed = 2.0 * speaking_length_m * frequency_hz;
    const double k = pi / speaking_length_m;
    const double tension =
        linear_density_kg_per_m * wave_speed * wave_speed - bending_stiffness_n_m2 * k * k;
    // Never clamp: a nonpositive result means incompatible inputs.
    require(positive(tension), "target frequency is infeasible: derived tension is not positive");
    return tension;
}

std::vector<FretConfig> fret_positions(double speaking_length_m, const FretMapDefinition& map) {
    require(positive(speaking_length_m), "speaking length must be finite and positive (m)");
    require(!map.intervals.empty(), "fret map must contain at least one interval");
    std::vector<FretConfig> frets;
    double previous_ratio = 1.0;
    for (const auto& interval : map.intervals) {
        require(std::isfinite(interval.ratio) && interval.ratio > previous_ratio,
                "fret ratios must be finite, > 1 and strictly increasing: " + interval.label);
        previous_ratio = interval.ratio;
        frets.push_back(
            FretConfig{speaking_length_m * (1.0 - 1.0 / interval.ratio), interval.label});
    }
    return frets;
}

double cents(double frequency_ratio) {
    require(positive(frequency_ratio), "frequency ratio must be finite and positive");
    return 1200.0 * std::log2(frequency_ratio);
}

InstrumentConfig synthetic_setar_config(const PresetOptions& options) {
    const TuningPreset& tuning = find_tuning(
        options.tuning_id.empty() ? default_tuning_id : std::string_view(options.tuning_id));
    const FretMapDefinition& map = find_fret_map(
        options.fret_map_id.empty() ? default_fret_map_id : std::string_view(options.fret_map_id));
    const double speaking = option_or_default(options.speaking_length_m, default_speaking_length_m,
                                              "speaking_length_m");
    const double afterlength =
        option_or_default(options.afterlength_m, default_afterlength_m, "afterlength_m");

    InstrumentConfig config;
    config.label = "synthetic setar prototype (tuning " + tuning.id + ", fret map " + map.id + ")";
    config.provenance = Provenance::Synthetic;
    const auto specs = synthetic_string_specs();
    // Synthetic loss laws: steel strings 0, 1, 3; the brass-like string 2 slightly lossier.
    const std::array<StringLossLaw, string_count> losses{
        {{0.8, 0.0, 2.0e-9}, {0.8, 0.0, 2.0e-9}, {1.2, 0.0, 3.0e-9}, {0.8, 0.0, 2.0e-9}}};
    for (std::size_t i = 0; i < string_count; ++i) {
        StringConfig& s = config.strings[i];
        s.label = specs[i].label;
        s.total_length_m = speaking + afterlength;
        s.bridge_position_m = speaking;
        s.linear_density_kg_per_m = linear_density_kg_per_m(specs[i]);
        s.bending_stiffness_n_m2 = bending_stiffness_n_m2(specs[i]);
        s.tension_n = tension_for_frequency(speaking, s.linear_density_kg_per_m,
                                            s.bending_stiffness_n_m2, tuning.open_frequency_hz[i]);
        s.loss = losses[i];
        s.action_at_nut_m = 0.6e-3;
        s.action_at_bridge_m = 3.0e-3;
    }
    config.frets = fret_positions(speaking, map);
    if (options.include_body) {
        config.body = build_body(synthetic_setar_body_design());
    } else {
        config.body = BodyConfig{"no body: bridge contacts grounded (synthetic test configuration)",
                                 Provenance::Synthetic,
                                 {}};
    }
    config.bridge.stiffness_n_per_m = {1.0e6, 1.0e6};
    config.bridge.damping_n_s_per_m = {0.0, 0.0};
    config.contacts.fret = ContactLaw{1.0e9, 1.5, 0.05};
    config.contacts.finger = ContactLaw{5.0e4, 1.5, 0.8};
    config.contacts.nail = ContactLaw{1.0e6, 1.5, 0.02};
    config.contacts.finger_lateral_damping_n_s_per_m = 0.5;
    config.contacts.fret_stick_stiffness_n_per_m = 1.0e5;
    config.contacts.fret_stick_damping_n_s_per_m = 0.05;
    config.max_mode_frequency_hz = 18000.0;
    validate(config);
    return config;
}

} // namespace setar::instrument
