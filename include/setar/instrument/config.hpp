#pragma once

// Physical configuration of the playable setar prototype (namespace setar::instrument).
//
// STATUS: implementation prototype. Every default value produced by the synthetic
// preset builders is a SYNTHETIC fixture, not a measurement of an Iranian setar.
// See docs/engineering/11-playable-prototype.md and
// docs/numerics/15-modal-contact-solver.md for the governing equations.
//
// Coordinates and sign conventions (binding for every module in this namespace):
// - x (m) runs along each string from the nut (x = 0, rigid) to the tail anchor
//   (x = total_length_m, rigid). The bridge contact is at x = bridge_position_m;
//   the speaking (open) length is therefore bridge_position_m. The segment
//   bridge_position_m < x < total_length_m is the physical afterlength.
// - Polarization::Vertical (index 0): transverse displacement normal to the
//   soundboard, positive AWAY from the soundboard. Fret tops lie at negative v.
// - Polarization::Horizontal (index 1): transverse displacement parallel to the
//   soundboard plane, positive toward the side of physical string index 3.
// - Physical string indices 0..3 identify individual strings, not courses.
//   Courses are performance groupings (see setar_preset.hpp).
// All quantities are SI. Angles are radians. No field is a "normalized" control.

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace setar::instrument {

inline constexpr std::size_t string_count = 4;
inline constexpr std::size_t polarization_count = 2;
inline constexpr std::size_t observation_count = 2; // left/right observation channels

enum class Polarization : std::uint8_t { Vertical = 0, Horizontal = 1 };

// Evidence class of a configuration as a whole. Metadata only; the engine never
// changes behaviour based on it, and nothing in this prototype is Measured.
enum class Provenance : std::uint8_t { Synthetic, Published, Measured, Inferred };

// Amplitude decay of string mode n (1/s):
//   sigma_n = decay_constant + decay_wavenumber_squared*k_n^2 + decay_frequency_squared*omega_n^2,
// k_n = n*pi/total_length_m. Same law as setar::reference / setar::experimental.
// All coefficients finite and >= 0. A mode with sigma_n >= omega_n is rejected.
struct StringLossLaw {
    double decay_constant_per_s = 0.0;
    double decay_wavenumber_squared_m2_per_s = 0.0;
    double decay_frequency_squared_s = 0.0;
};

// One uniform stiff string, simply supported at x = 0 and x = total_length_m,
// with identical properties in both transverse polarizations.
struct StringConfig {
    std::string label;
    double total_length_m = 0.0;          // nut to tail anchor
    double bridge_position_m = 0.0;       // nut to bridge contact; 0 < bridge < total
    double tension_n = 0.0;
    double linear_density_kg_per_m = 0.0;
    double bending_stiffness_n_m2 = 0.0;  // EI >= 0
    StringLossLaw loss{};
    // Distance (m, > 0) from the string rest line down to the fret-top line, linear in x:
    //   action(x) = action_at_nut_m + (action_at_bridge_m - action_at_nut_m) * x / bridge_position_m
    double action_at_nut_m = 0.0;
    double action_at_bridge_m = 0.0;
};

// Vertical position (m) of the fret-top line under string `s` at `x_m`. Negative.
[[nodiscard]] inline double fret_top_height_m(const StringConfig& s, double x_m) noexcept {
    const double action = s.action_at_nut_m +
                          (s.action_at_bridge_m - s.action_at_nut_m) * x_m / s.bridge_position_m;
    return -action;
}

// A tied fret. Shared by all strings at the same distance from the nut.
// Positions must be strictly increasing, > 0 and < every string's bridge position.
struct FretConfig {
    double position_m = 0.0;
    std::string label; // e.g. interval name; descriptive only
};

// One structural mode of the body as seen from the bridge.
// Equation: M*(eta'' + 2*zeta*Omega*eta' + Omega^2*eta) = sum_c psi_c * F_c,
// where the displacement of bridge contact c = (string s, polarization p) is
//   w_c = sum_j bridge_participation[s][p] * eta_j  (dimensionless shape value, m per m).
// radiation_weight[o] (Pa*s^2/m) maps modal ACCELERATION to observed pressure at
// observation channel o. Observation never feeds back into mechanics.
struct BodyMode {
    double frequency_hz = 0.0;     // undamped, > 0
    double damping_ratio = 0.0;    // zeta in [0, 1)
    double modal_mass_kg = 0.0;    // > 0
    std::array<std::array<double, polarization_count>, string_count> bridge_participation{};
    std::array<double, observation_count> radiation_weight{};
};

struct BodyConfig {
    std::string label;
    Provenance provenance = Provenance::Synthetic;
    std::vector<BodyMode> modes; // may be empty: then bridge contacts are rigidly grounded
};

// Linear connection (spring + optional damper) between each string at its bridge
// position and the body bridge contact, per polarization. Very stiff in practice.
struct BridgeConfig {
    std::array<double, polarization_count> stiffness_n_per_m{};       // > 0
    std::array<double, polarization_count> damping_n_s_per_m{};       // >= 0
};

// One-sided power-law contact: potential V = K/(alpha+1) * pen^(alpha+1) for pen > 0,
// force magnitude K*pen^alpha, plus damping force -R * (penetration rate) while in
// contact. K in N/m^alpha (> 0), alpha >= 1, R in N*s/m (>= 0).
struct ContactLaw {
    double stiffness = 0.0;
    double exponent = 1.0;
    double damping_n_s_per_m = 0.0;
};

struct ContactConfig {
    ContactLaw fret{};    // string on fret top (vertical, obstacle below the string)
    ContactLaw finger{};  // finger pad pressing the string (vertical, from above)
    ContactLaw nail{};    // fingernail along the stroke direction
    // While a finger pad is in contact: lateral (horizontal) viscous flesh damping
    // at the pad position (N*s/m, >= 0).
    double finger_lateral_damping_n_s_per_m = 0.0;
    // While the string is in contact with a fret: horizontal stick spring anchored
    // at the string position at contact onset (N/m, >= 0) and its damper (N*s/m).
    double fret_stick_stiffness_n_per_m = 0.0;
    double fret_stick_damping_n_s_per_m = 0.0;
};

struct InstrumentConfig {
    std::string label;
    Provenance provenance = Provenance::Synthetic;
    std::array<StringConfig, string_count> strings{};
    std::vector<FretConfig> frets;
    BodyConfig body{};
    BridgeConfig bridge{};
    ContactConfig contacts{};
    // String and body modes at or above min(max_mode_frequency_hz, 0.45*fs) are not
    // retained. Truncation is a declared model reduction, not an error.
    double max_mode_frequency_hz = 18000.0;
};

// Realtime processing capacities, fixed at prepare(). All must be > 0.
struct ProcessSpec {
    double sample_rate_hz = 48000.0;            // [22050, 192000]
    std::uint32_t max_block_frames = 512;
    std::uint32_t max_events_per_block = 64;
    std::uint32_t max_interaction_channels = 32; // simultaneous linear+contact channels
    std::uint32_t max_newton_iterations = 50;
    double newton_tolerance_m = 1.0e-14;        // absolute residual on channel displacement
    double output_gain_per_pa = 1.0;            // fixed observation gain, no normalization
    bool enable_energy_ledger = false;          // O(total modes) extra work per sample
};

// Throws std::invalid_argument naming the first failing field. Never clamps.
void validate(const InstrumentConfig& config);
void validate(const ProcessSpec& spec);

} // namespace setar::instrument
