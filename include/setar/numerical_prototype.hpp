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
//
// Optional passive loss (see docs/numerics/14-damped-midpoint-loss-verification.md):
//   mu*y_tt + 2*mu*(s0 - s1*d_xx)*y_t + 2*s2*L*y_t + L*y = F*delta,
//   with L = T*(-d_xx) + EI*d_xxxx,
// which on the uniform grid gives each sine mode the amplitude decay rate
//   gamma_n = decay_constant + decay_wavenumber_squared*k_n^2
//                            + decay_frequency_squared*omega_n^2   (1/s).
// All three coefficients must be finite and nonnegative so the damping operator is
// positive semidefinite by construction; there is no clipping of an unstable value.
// Only MidpointStiffString implements loss; LeapfrogStiffString rejects any nonzero
// coefficient instead of silently ignoring it.
struct StringLoss {
    double decay_constant_per_s = 0.0;               // sigma_0 (1/s)
    double decay_wavenumber_squared_m2_per_s = 0.0;  // sigma_1 (m^2/s)
    double decay_frequency_squared_s = 0.0;          // sigma_2 (s)
};

struct StringGrid {
    double length_m = 0.66;
    double tension_n = 35.0;
    double linear_density_kg_per_m = 0.0007;
    double bending_stiffness_n_m2 = 0.000015;
    double sample_rate_hz = 48000.0;
    double force_position_fraction = 0.32;
    std::size_t interior_nodes = 32;
    StringLoss loss{};
};

// Power-conjugate force input (N) / interpolated transverse velocity (m/s).
// Time level is exactly (n+1/2), including the impulse/work calculation.
// Ledger convention: work_residual_j = stored_energy_change_j - external_work_j
//                                       + dissipated_energy_j,
// where dissipated_energy_j >= 0 is computed from the damping operator applied to
// the midpoint velocity, independently of the energy before/after the step.
struct WorkStep {
    double port_velocity_mid_m_per_s = 0.0;
    double external_work_j = 0.0;
    double stored_energy_change_j = 0.0;
    double dissipated_energy_j = 0.0;
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
    std::vector<double> damping_;   // dense D = 2*(s0*I + s1*B + s2*K/mu) in 1/s, PSD
    std::vector<double> cholesky_;  // lower triangle of I + dt^2*K/(4*mu) + dt*D/2
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

// Loss-law oracles for the sine-mode fixture (uniform grid, fixed ends). Amplitude
// decay rates in 1/s. continuum_decay_rate uses the continuum k_n^2 and omega_n^2;
// spatial_grid_decay_rate uses the grid eigenvalue lambda_n and grid omega_n^2.
[[nodiscard]] double continuum_decay_rate_per_s(const StringGrid& grid, std::size_t mode_number);
[[nodiscard]] double spatial_grid_decay_rate_per_s(const StringGrid& grid, std::size_t mode_number);

// Roots of the midpoint amplification matrix for one damped grid mode, computed as
// the Cayley image r = (1 + s*dt/2)/(1 - s*dt/2) of the exact semidiscrete
// eigenvalues s = -gamma +/- sqrt(gamma^2 - Omega^2). This shares no code with the
// matrix solve in MidpointStiffString::advance.
// decay_rate_per_s = -ln(spectral_radius)/dt (amplitude), frequency_hz = |arg r|/(2*pi*dt).
// If gamma >= Omega the mode is not oscillatory: oscillatory is false and
// frequency_hz is exactly zero rather than a fabricated positive value.
struct MidpointRoots {
    double decay_rate_per_s = 0.0;
    double frequency_hz = 0.0;
    double spectral_radius = 0.0;
    bool oscillatory = false;
};
[[nodiscard]] MidpointRoots midpoint_damped_roots(const StringGrid& grid,
                                                  std::size_t mode_number);

// Measurement of one damped oscillation from a sampled scalar series (for example
// the projection of the string state onto one sine mode). It fits the linear
// recurrence x[k+1] = a*x[k] + b*x[k-1] by QR least squares and converts the roots
// of r^2 - a*r - b to amplitude decay and frequency (docs/numerics/08, eq. D7).
// The fit is gain-invariant. It does NOT separate several modes: pass a signal that
// contains one damped pair. residual_rms_over_signal_rms reports how well one pair
// explains the data; callers must check it. Throws std::invalid_argument for fewer
// than 8 samples, nonfinite data, a zero signal or a rank-deficient fit.
struct DecayEstimate {
    double decay_rate_per_s = 0.0;
    double frequency_hz = 0.0;
    double spectral_radius = 0.0;
    bool oscillatory = false;
    double residual_rms_over_signal_rms = 0.0;
    // r11/r22 of the QR factor: about 1/(omega*dt) for a lightly damped sinusoid.
    // The decay error is roundoff-limited near condition*eps/(2*gamma*dt), so a very
    // large value flags a window/sample-rate combination that cannot resolve the decay.
    double regressor_condition = 0.0;
};
[[nodiscard]] DecayEstimate estimate_decay(std::span<const double> samples,
                                           double sample_rate_hz);

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
