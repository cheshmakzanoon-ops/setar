#pragma once

// Synthetic, physics-derived body for the playable prototype: a thin orthotropic
// simply supported soundboard plate, optionally coupled to a Helmholtz air cavity,
// plus a lateral bridge mode, reduced to BodyConfig modes at the bridge contacts.
//
// STATUS: SYNTHETIC. Dimensions and material constants are declared placeholders
// within generic wood/air ranges, not measurements of a setar specimen. The plate
// is not the real pear-shaped soundboard and the bowl is rigid except through the
// cavity air stiffness. See docs/body/10-synthetic-prototype-body.md and docs/body/06
// for the measurement route that must replace it.
//
// Plate frame: origin at a soundboard corner, x_p (m) along the string direction
// in [0, length_m] (x_p = 0 at the neck end), y_p (m) across in [0, width_m],
// displacement normal to the plate positive AWAY from the cavity interior (same sign
// as Polarization::Vertical). The opening's air-plug coordinate is positive outward.

#include "setar/instrument/config.hpp"

#include <array>
#include <complex>
#include <cstddef>
#include <string>

namespace setar::instrument {

struct OrthotropicPlate {
    double length_m = 0.0; // along grain (x_p)
    double width_m = 0.0;  // across grain (y_p)
    double thickness_m = 0.0;
    double density_kg_per_m3 = 0.0;
    double youngs_modulus_long_pa = 0.0;
    double youngs_modulus_cross_pa = 0.0;
    double shear_modulus_pa = 0.0;
    double poisson_long_cross = 0.0; // nu_LR; nu_RL = nu_LR * E_R / E_L by reciprocity
    double damping_ratio = 0.0;      // per plate mode, in [0, 1)
};

struct HelmholtzCavity {
    bool enabled = false;
    double volume_m3 = 0.0;
    double opening_area_m2 = 0.0;
    double opening_effective_length_m = 0.0; // neck length plus end corrections
    double air_density_kg_per_m3 = 1.2;
    double sound_speed_m_per_s = 343.0;
    double damping_ratio = 0.0;
    // Centre of the (equivalent) opening on the plate (x_p, y_p), m. Used only for the
    // observation dipole moment; the hole is not removed from the plate stiffness.
    std::array<double, 2> opening_position_m{};
};

// Single lateral (horizontal) bridge/top resonance seen by the horizontal polarization.
struct LateralBridgeMode {
    bool enabled = false;
    double frequency_hz = 0.0;
    double modal_mass_kg = 0.0;
    double damping_ratio = 0.0;
    // Synthetic equivalent volume displacement per metre of lateral bridge motion (m^2),
    // so the horizontal polarization is not silent in the observation. Observation only.
    double radiation_area_m2 = 0.0;
};

struct BodyObservation {
    // Far-field monopole/dipole observation point distances (m) and dipole axis weights
    // per observation channel; pressure = rho0/(4*pi*r) * (volume acceleration
    // + dipole_weight * first x_p-moment of the acceleration). Observation only.
    std::array<double, observation_count> distance_m{};
    std::array<double, observation_count> dipole_weight_per_m{};
    double air_density_kg_per_m3 = 1.2; // exterior air density rho0
};

struct BodyDesign {
    std::string label;
    OrthotropicPlate plate{};
    HelmholtzCavity cavity{};
    LateralBridgeMode lateral{};
    BodyObservation observation{};
    // Bridge contact point of each string on the plate (x_p, y_p), m.
    std::array<std::array<double, 2>, string_count> bridge_point_m{};
    double max_frequency_hz = 0.0; // retain coupled modes below this frequency
    // Lumped bridge mass (kg, >= 0) added to the plate at the mean bridge point.
    double bridge_mass_kg = 0.0;
    // Plate modes below coupling_basis_factor * max_frequency_hz (>= 1) enter the
    // coupled plate/cavity/bridge-mass eigenproblem before truncation.
    double coupling_basis_factor = 1.5;
};

// Synthetic default design (documented placeholder values, Provenance::Synthetic).
[[nodiscard]] BodyDesign synthetic_setar_body_design();

// Throws std::invalid_argument for inadmissible designs, including an orthotropic
// compliance that is not positive definite (|nu_LR| >= sqrt(E_L/E_R)).
void validate(const BodyDesign& design);

// Builds the reduced BodyConfig (sorted by frequency). Throws on invalid design.
// Each coupled mode is scaled so its dominant mass-weighted component has unit
// amplitude; in the uncoupled limit a plate mode keeps mass rho*h*a*b/4 and shape
// sin(m*pi*x/a)*sin(n*pi*y/b).
[[nodiscard]] BodyConfig build_body(const BodyDesign& design);

// Undamped frequency (Hz) of simply supported orthotropic plate mode (m, n) >= 1:
// omega^2 = pi^4/(rho*h) * (D11 (m/a)^4 + 2 (D12 + 2 D66)(m a^-1 n b^-1)^2 + D22 (n/b)^4).
[[nodiscard]] double plate_mode_frequency_hz(const OrthotropicPlate& plate, std::size_t m,
                                             std::size_t n);

// Bridge transfer mobility (m/s per N) of a BodyConfig between contacts
// (s_out, p_out) and (s_in, p_in) at frequency f (Hz), from its modal sum:
// Y = sum_j i*w*psi_out*psi_in / (M_j (W_j^2 - w^2 + 2 i zeta_j W_j w)).
[[nodiscard]] std::complex<double> bridge_mobility(const BodyConfig& body, std::size_t s_out,
                                                   Polarization p_out, std::size_t s_in,
                                                   Polarization p_in, double frequency_hz);

// Plate flexural rigidities (N*m) of the thin orthotropic plate (Kirchhoff, plane stress):
// D11 = E_L h^3/(12(1 - nu_LR nu_RL)), D22 = E_R h^3/(12(1 - nu_LR nu_RL)),
// D12 = nu_LR D22 (= nu_RL D11), D66 = G h^3/12. Throws if the plate is inadmissible.
struct FlexuralRigidity {
    double d11 = 0.0;
    double d22 = 0.0;
    double d12 = 0.0;
    double d66 = 0.0;
};
[[nodiscard]] FlexuralRigidity flexural_rigidity(const OrthotropicPlate& plate);

// Minor Poisson ratio nu_RL = nu_LR * E_R / E_L implied by reciprocity.
[[nodiscard]] double poisson_cross_long(const OrthotropicPlate& plate);

// Row-major 3x3 plane-stress compliance S, [eps_L, eps_R, gamma_LR] = S [s_L, s_R, tau_LR]
// (1/Pa). Symmetric by construction when nu_RL follows reciprocity.
[[nodiscard]] std::array<double, 9> plane_stress_compliance(const OrthotropicPlate& plate);

// True when an independently reported Poisson pair satisfies nu_LR/E_L = nu_RL/E_R within
// the relative tolerance (and the resulting compliance is positive definite). Used to
// screen published constants before they may enter a plate; no correction is applied.
[[nodiscard]] bool reciprocal_poisson_pair(double youngs_long_pa, double youngs_cross_pa,
                                           double nu_long_cross, double nu_cross_long,
                                           double relative_tolerance);

// Uncoupled Helmholtz frequency c/(2*pi)*sqrt(A/(V*L_eff)) (Hz). Throws if invalid.
[[nodiscard]] double helmholtz_frequency_hz(const HelmholtzCavity& cavity);

} // namespace setar::instrument
