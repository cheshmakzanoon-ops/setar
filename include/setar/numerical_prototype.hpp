#pragma once

#include <cstddef>
#include <span>
#include <vector>

namespace setar::experimental {

// OFFLINE NUMERICAL EXPERIMENT, NOT A VALIDATED SETAR AND NOT A REALTIME SOLVER.
// A simply supported, uniform, linear, transverse stiff string with fixed ends:
// mu*y_tt = T*y_xx - EI*y_xxxx + F(t)*delta(x-x_port).
// The spatial operator is K = T*(-D2) + EI*(-D2)^2; midpoint time stepping
// conserves the finite-dimensional energy under zero external force.
// Every number is a synthetic fixture, not a measurement of an Iranian setar.
struct StringGrid {
    double length_m = 0.66;
    double tension_n = 35.0;
    double linear_density_kg_per_m = 0.0007;
    double bending_stiffness_n_m2 = 0.000015;
    double sample_rate_hz = 48000.0;
    double force_position_fraction = 0.32;
    std::size_t interior_nodes = 32;
};

// Power-conjugate force input (N) / interpolated transverse velocity (m/s).
// Time level is exactly (n+1/2), including the impulse/work calculation.
struct WorkStep {
    double port_velocity_mid_m_per_s = 0.0;
    double external_work_j = 0.0;
    double stored_energy_change_j = 0.0;
    double work_residual_j = 0.0;
};

// Builds and Cholesky-factorizes a dense SPD matrix at prepare(): O(N^3).
// Each advance() is O(N^2), with no allocations after prepare(). This is
// intentionally an oracle-scale prototype, not a production callback path.
class MidpointStiffString {
  public:
    void prepare(const StringGrid& parameters);
    void set_state(std::span<const double> displacement_m,
                   std::span<const double> velocity_m_per_s);
    void set_sine_state(double amplitude_m, std::size_t harmonic_number);
    void reset() noexcept;
    [[nodiscard]] WorkStep advance(double midpoint_force_n);
    [[nodiscard]] double energy_j() const noexcept;
    [[nodiscard]] double port_velocity_m_per_s() const noexcept;
    [[nodiscard]] double port_displacement_m() const noexcept;
    [[nodiscard]] std::span<const double> displacement_m() const noexcept;
    [[nodiscard]] std::span<const double> velocity_m_per_s() const noexcept;
    [[nodiscard]] bool prepared() const noexcept;
    [[nodiscard]] const StringGrid& parameters() const noexcept;

  private:
    StringGrid parameters_{};
    std::vector<double> stiffness_; // dense row-major, N*m -> N*m, symmetric
    std::vector<double> cholesky_;  // lower triangle of I + dt^2*K/(4*mu)
    std::vector<double> port_weights_;
    std::vector<double> q_;
    std::vector<double> v_;
    std::vector<double> rhs_;
    std::vector<double> next_q_;
    std::vector<double> scratch_;
    std::vector<double> next_v_;
};

// Independent closed-form frequency oracles for the explicitly stated fixed-
// end boundary conditions. Throws for an invalid mode/parameter set.
[[nodiscard]] double continuum_frequency_hz(const StringGrid& grid,
                                            std::size_t mode_number);
[[nodiscard]] double spatial_grid_frequency_hz(const StringGrid& grid,
                                               std::size_t mode_number);
[[nodiscard]] double midpoint_frequency_hz(double frequency_hz, double sample_rate_hz);

// Explicit second-order (leapfrog) finite-difference string. The discrete
// energy is a staggered, modified energy, not continuum instantaneous energy.
// An unstable CFL configuration is rejected in prepare(), unlike midpoint.
// O(N) work and fixed storage on each step; no contacts or moving boundaries.
struct LeapfrogWorkStep {
    double external_work_j = 0.0;
    double modified_energy_change_j = 0.0;
    double work_residual_j = 0.0;
};

class LeapfrogStiffString {
  public:
    void prepare(const StringGrid& parameters);
    void set_sine_state(double amplitude_m, std::size_t harmonic_number);
    void reset() noexcept;
    [[nodiscard]] LeapfrogWorkStep advance(double integer_level_force_n);
    [[nodiscard]] double modified_energy_j() const noexcept;
    [[nodiscard]] std::span<const double> displacement_m() const noexcept;
    [[nodiscard]] bool prepared() const noexcept;

  private:
    StringGrid parameters_{};
    std::vector<double> port_weights_;
    std::vector<double> previous_q_;
    std::vector<double> current_q_;
    std::vector<double> next_q_;
};

[[nodiscard]] double leapfrog_minimum_stable_rate_hz(const StringGrid& grid);
[[nodiscard]] double leapfrog_frequency_hz(double spatial_frequency_hz, double sample_rate_hz);

// Minimal two-mass/one-spring co-located port oracle, NOT string/body coupling.
// x1 and x2 are displacements (m); force at the spring is equal and opposite.
struct TwoMassParameters {
    double mass_1_kg = 0.004;
    double mass_2_kg = 0.009;
    double coupling_stiffness_n_per_m = 150.0;
    double sample_rate_hz = 48000.0;
};

struct TwoMassState {
    double x1_m = 0.0;
    double x2_m = 0.0;
    double v1_m_per_s = 0.0;
    double v2_m_per_s = 0.0;
};

struct CouplingStep {
    double force_on_1_mid_n = 0.0;
    double force_on_2_mid_n = 0.0;
    double port_1_velocity_mid_m_per_s = 0.0;
    double port_2_velocity_mid_m_per_s = 0.0;
    double spring_energy_change_j = 0.0;
    double port_work_sum_j = 0.0;
    double total_energy_change_j = 0.0;
};

class MidpointTwoMassPort {
  public:
    void prepare(const TwoMassParameters& parameters);
    void set_state(TwoMassState state);
    [[nodiscard]] CouplingStep advance();
    [[nodiscard]] TwoMassState state() const noexcept;
    [[nodiscard]] double energy_j() const noexcept;
    [[nodiscard]] double linear_momentum_kg_m_per_s() const noexcept;
    [[nodiscard]] bool prepared() const noexcept;

  private:
    TwoMassParameters parameters_{};
    TwoMassState state_{};
    bool prepared_ = false;
};

} // namespace setar::experimental
