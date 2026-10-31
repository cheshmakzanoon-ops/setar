#pragma once

// Synthetic setar presets: string specifications, tunings, fret maps, courses and a
// complete InstrumentConfig for the playable prototype.
//
// STATUS: SYNTHETIC / EDUCATIONAL. String gauges, materials, tunings and fret maps
// are declared placeholders or historical-theory interval sets, NOT a measured
// specimen and NOT a statement of any performer's practice. Every preset carries a
// status string that the UI and reports must display.

#include "setar/instrument/body_model.hpp"
#include "setar/instrument/config.hpp"

#include <array>
#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace setar::instrument {

struct StringMaterial {
    std::string name;            // e.g. "synthetic plain steel"
    double density_kg_per_m3 = 0.0;
    double youngs_modulus_pa = 0.0;
};

// Plain (solid, round, homogeneous) string assumption: mu = rho*pi*d^2/4, EI = E*pi*d^4/64.
struct StringSpec {
    std::string label;
    StringMaterial material{};
    double diameter_m = 0.0;
};

struct TuningPreset {
    std::string id;                               // stable identifier
    std::string description;
    std::string status;                           // e.g. "synthetic educational example"
    std::array<double, string_count> open_frequency_hz{}; // physical strings 0..3
};

struct FretInterval {
    std::string label;   // descriptive interval label
    double ratio = 1.0;  // frequency ratio above the open string, > 1
};

struct FretMapDefinition {
    std::string id;
    std::string description;
    std::string status;  // provenance / approximation statement
    std::vector<FretInterval> intervals; // strictly increasing ratios
};

// Performance grouping of physical strings. A course is NOT a duplicated waveform.
struct Course {
    std::string label;
    std::vector<std::size_t> strings; // physical string indices, in stroke order for +h strokes
};

struct PresetOptions {
    std::string tuning_id;   // empty = default tuning
    std::string fret_map_id; // empty = default fret map
    double speaking_length_m = 0.0;   // 0 = default
    double afterlength_m = 0.0;       // 0 = default
    bool include_body = true;         // false: empty BodyConfig (bridge contacts grounded)
};

[[nodiscard]] std::span<const TuningPreset> tuning_presets();
[[nodiscard]] std::span<const FretMapDefinition> fret_map_definitions();
[[nodiscard]] const TuningPreset& find_tuning(std::string_view id);       // throws invalid_argument
[[nodiscard]] const FretMapDefinition& find_fret_map(std::string_view id); // throws invalid_argument
[[nodiscard]] std::array<StringSpec, string_count> synthetic_string_specs();
[[nodiscard]] std::vector<Course> setar_courses(); // {0}, {1}, {2,3} (index convention in docs)

// Linear density and bending stiffness of a plain string. Throws on invalid input.
[[nodiscard]] double linear_density_kg_per_m(const StringSpec& spec);
[[nodiscard]] double bending_stiffness_n_m2(const StringSpec& spec);

// Tension (N) that gives the simply supported stiff string of speaking length L a
// fundamental f: T = mu*(2*L*f)^2 - EI*(pi/L)^2. Throws if the result is not positive.
[[nodiscard]] double tension_for_frequency(double speaking_length_m, double linear_density_kg_per_m,
                                           double bending_stiffness_n_m2, double frequency_hz);

// Ideal-string fret positions x = L*(1 - 1/ratio) from the nut. Throws on invalid ratios.
[[nodiscard]] std::vector<FretConfig> fret_positions(double speaking_length_m,
                                                     const FretMapDefinition& map);

[[nodiscard]] double cents(double frequency_ratio); // 1200*log2(ratio), ratio > 0

// Full synthetic instrument: four strings, fret map, synthetic body (build_body of
// synthetic_setar_body_design), bridge and contact laws. Validated before return.
[[nodiscard]] InstrumentConfig synthetic_setar_config(const PresetOptions& options = {});

} // namespace setar::instrument
