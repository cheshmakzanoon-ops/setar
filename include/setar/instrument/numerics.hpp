#pragma once

// Numerical building blocks of the playable prototype (docs/numerics/15-modal-contact-solver.md).
//
// 1. Exact-modal three-level oscillator scheme. For one mode with mass m, undamped
//    angular frequency w and amplitude decay s (s < w), with time step k:
//        q[n+1] = a*q[n] - b*q[n-1] + g*f[n]
//        a = 2*exp(-s*k)*cos(wd*k),  b = exp(-2*s*k),  g = k^2*(1+b)/(2*m),  wd = sqrt(w^2 - s^2).
//    It is the scheme  dtt q + 2*s~*dt. q + w~^2*q[n] = f/m  with
//        s~ = tanh(s*k)/k,   w~^2*k^2 = 4*(sinh^2(s*k/2) + sin^2(wd*k/2))/cosh(s*k),
//    so the free response equals the sampled continuous damped oscillator exactly.
//    Discrete energy (>= 0 because w~*k < 2 whenever wd*k < pi):
//        H[n+1/2] = m/2*((q[n+1]-q[n])/k)^2 + m/2*w~^2*q[n+1]*q[n]
//    and   H[n+1/2] - H[n-1/2] = -2*m*s~*k*((q[n+1]-q[n-1])/(2k))^2 + f[n]*(q[n+1]-q[n-1])/2.
//
// 2. Interaction channels. A channel is a scalar relative coordinate z = B*q - u(t)
//    with potential V(z) and optional damping R. Its generalized force at level n is
//    the discrete gradient
//        F(s) = -(V(z_prev + s) - V(z_prev))/s - R*s/(2k),   s = z[n+1] - z[n-1],
//    which makes  F*s = -(V[n+1]-V[n-1]) - R*s^2/(2k)  hold exactly.
//    For convex V, dF/ds <= 0.
//
// 3. Small dense Newton solve of  s - b - M*F(s) = 0  with M = B*G*B^T symmetric
//    positive semidefinite (m x m compliance), b = z_free[n+1] - z[n-1].
//    The Jacobian I - M*diag(dF/ds) is nonsingular for convex laws.
//
// Realtime: prepare() allocates/throws; every other member is noexcept and allocation-free.

#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace setar::instrument::numerics {

struct ModalCoefficients {
    double a = 0.0;
    double b = 0.0;
    double g = 0.0;                  // s^2/kg
    double sigma_tilde_per_s = 0.0;  // tanh(s*k)/k
    double omega_tilde_sq = 0.0;     // rad^2/s^2
    double mass_kg = 0.0;
};

// Throws std::invalid_argument unless mass > 0, omega > 0, 0 <= decay < omega, dt > 0
// and sqrt(omega^2 - decay^2)*dt < pi (all finite).
[[nodiscard]] ModalCoefficients modal_coefficients(double mass_kg, double omega_rad_per_s,
                                                   double decay_per_s, double dt_s);

// out[i] = sin((i+1)*theta) for i = 0..out.size()-1, by unit-complex rotation.
void fill_sine_shape(std::span<double> out, double theta_rad) noexcept;

// Structure-of-arrays bank of independent modes sharing one time step.
class ModalBank {
  public:
    void prepare(std::span<const double> mass_kg, std::span<const double> omega_rad_per_s,
                 std::span<const double> decay_per_s, double dt_s);
    [[nodiscard]] std::size_t size() const noexcept { return a_.size(); }
    [[nodiscard]] double time_step_s() const noexcept { return dt_; }
    void reset() noexcept;

    // Free update: next = a*current - b*previous.
    void predict() noexcept;
    // next += g * shape * generalized_force (shape.size() must equal size()).
    void add_force(std::span<const double> shape, double generalized_force) noexcept;
    [[nodiscard]] double gather_previous(std::span<const double> shape) const noexcept;
    [[nodiscard]] double gather_current(std::span<const double> shape) const noexcept;
    [[nodiscard]] double gather_next(std::span<const double> shape) const noexcept;
    // sum_i g_i * s1_i * s2_i  (m/N): this bank's contribution to channel compliance M.
    [[nodiscard]] double compliance(std::span<const double> s1,
                                    std::span<const double> s2) const noexcept;
    // Modal damping energy of the pending step (call after forces, before commit), J >= 0.
    [[nodiscard]] double step_dissipation_j() const noexcept;
    // previous <- current, current <- next (no allocation).
    void commit() noexcept;
    // H between current and previous level (after commit: H[n+1/2]), J.
    [[nodiscard]] double energy_j() const noexcept;
    // current = previous = next = q (zero discrete velocity).
    void set_displacement(std::span<const double> q) noexcept;

    [[nodiscard]] std::span<const double> previous() const noexcept { return previous_; }
    [[nodiscard]] std::span<const double> current() const noexcept { return current_; }
    [[nodiscard]] std::span<const double> next() const noexcept { return next_; }
    [[nodiscard]] std::span<const double> gains() const noexcept { return g_; }
    [[nodiscard]] std::span<const double> masses() const noexcept { return m_; }

  private:
    std::vector<double> a_, b_, g_, m_, sigma_tilde_, omega_tilde_sq_;
    std::vector<double> previous_, current_, next_;
    double dt_ = 0.0;
};

enum class ChannelLawKind : std::uint8_t {
    OneSidedPower, // V = K/(alpha+1) * max(0,-z)^(alpha+1): active (penetrating) for z < 0
    LinearSpring,  // V = K/2 * (z - anchor)^2
    Damper,        // V = 0 (damping only)
};

struct ChannelLaw {
    ChannelLawKind kind = ChannelLawKind::LinearSpring;
    double stiffness = 0.0;          // N/m^alpha (power) or N/m (linear); >= 0
    double exponent = 1.0;           // alpha >= 1 (power only)
    double damping_n_s_per_m = 0.0;  // R >= 0, applied as -R*s/(2k) for the whole step
    double anchor_m = 0.0;           // linear spring rest coordinate
};

[[nodiscard]] double channel_potential_j(const ChannelLaw& law, double z_m) noexcept;

struct ChannelForce {
    double force_n = 0.0;       // F(s)
    double derivative = 0.0;    // dF/ds (N/m), <= 0 for admissible laws
};
// Discrete-gradient force for the step z_prev -> z_prev + s with time step dt.
[[nodiscard]] ChannelForce channel_force(const ChannelLaw& law, double z_prev_m, double s_m,
                                         double dt_s) noexcept;

struct NewtonResult {
    std::uint32_t iterations = 0;
    double residual_m = 0.0;   // max-norm of s - b - M*F(s) at the returned s
    bool converged = false;
    bool finite = true;
};

class SmallNewtonSolver {
  public:
    void prepare(std::size_t max_channels);
    [[nodiscard]] std::size_t capacity() const noexcept { return capacity_; }
    // m <= capacity(). compliance: row-major m*m. s: in = initial guess, out = solution.
    // force: out = F(s) at the returned s (consistent with residual_m).
    NewtonResult solve(std::size_t m, std::span<const double> compliance,
                       std::span<const double> b, std::span<const ChannelLaw> laws,
                       std::span<const double> z_prev, double dt_s, std::span<double> s,
                       std::span<double> force, std::uint32_t max_iterations,
                       double tolerance_m) noexcept;

  private:
    std::size_t capacity_ = 0;
    std::vector<double> jacobian_, residual_, delta_, derivative_, trial_s_, trial_force_;
    std::vector<std::size_t> pivot_;
};

} // namespace setar::instrument::numerics
