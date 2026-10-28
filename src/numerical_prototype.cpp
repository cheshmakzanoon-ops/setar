#include "setar/numerical_prototype.hpp"

#include <algorithm>
#include <cmath>
#include <numbers>
#include <stdexcept>
#include <utility>

namespace setar::experimental {
namespace {
constexpr double pi = std::numbers::pi_v<double>;

void require(bool valid, const char* message) {
    if (!valid) {
        throw std::invalid_argument(message);
    }
}

void require_positive(double value, const char* message) {
    require(std::isfinite(value) && value > 0.0, message);
}

void validate_grid(const StringGrid& g) {
    require_positive(g.length_m, "string length must be finite and positive (m)");
    require_positive(g.tension_n, "tension must be finite and positive (N)");
    require_positive(g.linear_density_kg_per_m, "linear density must be finite and positive (kg/m)");
    require(std::isfinite(g.bending_stiffness_n_m2) && g.bending_stiffness_n_m2 >= 0.0,
            "bending stiffness must be finite and nonnegative (N m^2)");
    require(std::isfinite(g.sample_rate_hz) && g.sample_rate_hz >= 8000.0 &&
                g.sample_rate_hz <= 384000.0,
            "sample rate must be finite and within [8000, 384000] Hz");
    require(std::isfinite(g.force_position_fraction) && g.force_position_fraction >= 0.0 &&
                g.force_position_fraction <= 1.0,
            "port fraction must be in [0, 1]");
    require(g.interior_nodes >= 4 && g.interior_nodes <= 128,
            "offline prototype supports 4..128 interior grid nodes");
    const double h = g.length_m / static_cast<double>(g.interior_nodes + 1);
    require_positive(h, "derived spatial grid spacing must be finite and positive");
    require_positive(g.linear_density_kg_per_m * h,
                     "derived nodal mass must be finite and positive");
}

double grid_eigenvalue(const StringGrid& g, std::size_t mode_number) {
    const double h = g.length_m / static_cast<double>(g.interior_nodes + 1);
    const double s = std::sin(static_cast<double>(mode_number) * pi /
                              (2.0 * static_cast<double>(g.interior_nodes + 1)));
    return 4.0 * s * s / (h * h);
}

double energy(const StringGrid& g, std::span<const double> k,
              std::span<const double> q, std::span<const double> v) noexcept {
    const std::size_t n = q.size();
    const double h = g.length_m / static_cast<double>(n + 1);
    double kinetic = 0.0;
    double potential = 0.0;
    for (std::size_t i = 0; i < n; ++i) {
        kinetic += v[i] * v[i];
        double kq = 0.0;
        for (std::size_t j = 0; j < n; ++j) {
            kq += k[i * n + j] * q[j];
        }
        potential += q[i] * kq;
    }
    return 0.5 * h * (g.linear_density_kg_per_m * kinetic + potential);
}

double two_mass_energy(const TwoMassParameters& p, const TwoMassState& s) noexcept {
    const double extension = s.x1_m - s.x2_m;
    return 0.5 * (p.mass_1_kg * s.v1_m_per_s * s.v1_m_per_s +
                  p.mass_2_kg * s.v2_m_per_s * s.v2_m_per_s +
                  p.coupling_stiffness_n_per_m * extension * extension);
}

double b_operator(std::span<const double> q, std::size_t i, double inv_h2) noexcept {
    if (i >= q.size()) {
        return 0.0;
    }
    const double left = i > 0 ? q[i - 1] : 0.0;
    const double right = i + 1 < q.size() ? q[i + 1] : 0.0;
    return (2.0 * q[i] - left - right) * inv_h2;
}

double stiffness_operator(const StringGrid& g, std::span<const double> q,
                          std::size_t i) noexcept {
    const double h = g.length_m / static_cast<double>(q.size() + 1);
    const double inv_h2 = 1.0 / (h * h);
    const double bq = b_operator(q, i, inv_h2);
    const double left = i > 0 ? b_operator(q, i - 1, inv_h2) : 0.0;
    const double right = i + 1 < q.size() ? b_operator(q, i + 1, inv_h2) : 0.0;
    const double b_squared_q = (2.0 * bq - left - right) * inv_h2;
    return g.tension_n * bq + g.bending_stiffness_n_m2 * b_squared_q;
}

double staggered_energy(const StringGrid& g, std::span<const double> previous,
                        std::span<const double> current) noexcept {
    const double h = g.length_m / static_cast<double>(current.size() + 1);
    const double rate = g.sample_rate_hz;
    double kinetic = 0.0;
    double potential = 0.0;
    for (std::size_t i = 0; i < current.size(); ++i) {
        const double difference = (current[i] - previous[i]) * rate;
        kinetic += difference * difference;
        potential += previous[i] * stiffness_operator(g, current, i);
    }
    return 0.5 * h * (g.linear_density_kg_per_m * kinetic + potential);
}
} // namespace

double continuum_frequency_hz(const StringGrid& g, std::size_t mode_number) {
    validate_grid(g);
    require(mode_number >= 1 && mode_number <= g.interior_nodes,
            "harmonic number must lie in [1, number of interior nodes]");
    const double wave_number = static_cast<double>(mode_number) * pi / g.length_m;
    const double squared_wave_number = wave_number * wave_number;
    const double omega_squared =
        (g.tension_n * squared_wave_number +
         g.bending_stiffness_n_m2 * squared_wave_number * squared_wave_number) /
        g.linear_density_kg_per_m;
    require_positive(omega_squared, "continuum frequency is not representable");
    return std::sqrt(omega_squared) / (2.0 * pi);
}

double spatial_grid_frequency_hz(const StringGrid& g, std::size_t mode_number) {
    validate_grid(g);
    require(mode_number >= 1 && mode_number <= g.interior_nodes,
            "harmonic number must lie in [1, number of interior nodes]");
    const double lambda = grid_eigenvalue(g, mode_number);
    const double omega_squared =
        (g.tension_n * lambda + g.bending_stiffness_n_m2 * lambda * lambda) /
        g.linear_density_kg_per_m;
    require_positive(omega_squared, "grid frequency is not representable");
    return std::sqrt(omega_squared) / (2.0 * pi);
}

double midpoint_frequency_hz(double frequency_hz, double sample_rate_hz) {
    require_positive(frequency_hz, "frequency must be finite and positive (Hz)");
    require_positive(sample_rate_hz, "sample rate must be finite and positive (Hz)");
    return sample_rate_hz * std::atan(pi * frequency_hz / sample_rate_hz) / pi;
}

double leapfrog_minimum_stable_rate_hz(const StringGrid& g) {
    validate_grid(g);
    return pi * spatial_grid_frequency_hz(g, g.interior_nodes);
}

double leapfrog_frequency_hz(double frequency_hz, double sample_rate_hz) {
    require_positive(frequency_hz, "discrete spatial frequency must be positive and finite");
    require_positive(sample_rate_hz, "sample rate must be positive and finite");
    require(pi * frequency_hz < sample_rate_hz,
            "leapfrog frequency requires a strictly stable CFL interval");
    return sample_rate_hz * std::asin(pi * frequency_hz / sample_rate_hz) / pi;
}

void LeapfrogStiffString::prepare(const StringGrid& g) {
    validate_grid(g);
    require(g.sample_rate_hz > 1.000000000001 * leapfrog_minimum_stable_rate_hz(g),
            "explicit stiff-string grid violates its analytically derived CFL bound");
    LeapfrogStiffString candidate;
    candidate.parameters_ = g;
    const std::size_t n = g.interior_nodes;
    candidate.previous_q_.assign(n, 0.0);
    candidate.current_q_.assign(n, 0.0);
    candidate.next_q_.assign(n, 0.0);
    candidate.port_weights_.resize(n);
    for (std::size_t i = 0; i < n; ++i) {
        const double port = g.force_position_fraction * static_cast<double>(n + 1);
        candidate.port_weights_[i] =
            std::max(0.0, 1.0 - std::abs(static_cast<double>(i + 1) - port));
    }
    *this = std::move(candidate);
}

void LeapfrogStiffString::set_sine_state(double amplitude_m, std::size_t mode) {
    if (!prepared()) {
        throw std::logic_error("prepare explicit grid before initializing its mode");
    }
    require(std::isfinite(amplitude_m), "explicit sine amplitude must be finite (m)");
    require(mode >= 1 && mode <= current_q_.size(), "invalid explicit spatial harmonic");
    const double frequency = spatial_grid_frequency_hz(parameters_, mode);
    const double ratio = 2.0 * pi * frequency / parameters_.sample_rate_hz;
    const double previous_factor = 1.0 - 0.5 * ratio * ratio;
    std::vector<double> next(current_q_.size());
    std::vector<double> previous(current_q_.size());
    for (std::size_t i = 0; i < next.size(); ++i) {
        next[i] = amplitude_m * std::sin(static_cast<double>(mode) * pi *
                                         static_cast<double>(i + 1) /
                                         static_cast<double>(next.size() + 1));
        previous[i] = previous_factor * next[i];
    }
    require(std::isfinite(staggered_energy(parameters_, previous, next)),
            "explicit mode initialization has nonfinite energy");
    std::copy(next.begin(), next.end(), current_q_.begin());
    std::copy(previous.begin(), previous.end(), previous_q_.begin());
}

void LeapfrogStiffString::reset() noexcept {
    std::fill(previous_q_.begin(), previous_q_.end(), 0.0);
    std::fill(current_q_.begin(), current_q_.end(), 0.0);
}

LeapfrogWorkStep LeapfrogStiffString::advance(double force_n) {
    if (!prepared()) {
        throw std::logic_error("prepare explicit grid before advancing");
    }
    require(std::isfinite(force_n), "integer-level external force must be finite (N)");
    const std::size_t n = current_q_.size();
    const double dt = 1.0 / parameters_.sample_rate_hz;
    const double h = parameters_.length_m / static_cast<double>(n + 1);
    const double factor = dt * dt / parameters_.linear_density_kg_per_m;
    const double old_energy = modified_energy_j();
    double port_velocity = 0.0;
    for (std::size_t i = 0; i < n; ++i) {
        const double stiffness = stiffness_operator(parameters_, current_q_, i);
        next_q_[i] = 2.0 * current_q_[i] - previous_q_[i] - factor * stiffness +
                     factor * force_n / h * port_weights_[i];
        require(std::isfinite(next_q_[i]), "explicit finite-difference update overflowed");
        port_velocity += port_weights_[i] * (next_q_[i] - previous_q_[i]) / (2.0 * dt);
    }
    const double new_energy = staggered_energy(parameters_, current_q_, next_q_);
    const double work = dt * force_n * port_velocity;
    require(std::isfinite(new_energy) && std::isfinite(work),
            "explicit step generated nonfinite modified energy or work");
    std::copy(current_q_.begin(), current_q_.end(), previous_q_.begin());
    std::copy(next_q_.begin(), next_q_.end(), current_q_.begin());
    return {work, new_energy - old_energy, new_energy - old_energy - work};
}

double LeapfrogStiffString::modified_energy_j() const noexcept {
    return prepared() ? staggered_energy(parameters_, previous_q_, current_q_) : 0.0;
}
std::span<const double> LeapfrogStiffString::displacement_m() const noexcept { return current_q_; }
bool LeapfrogStiffString::prepared() const noexcept { return !current_q_.empty(); }

void MidpointStiffString::prepare(const StringGrid& g) {
    validate_grid(g);
    MidpointStiffString candidate;
    const std::size_t n = g.interior_nodes;
    const double h = g.length_m / static_cast<double>(n + 1);
    const double inv_h2 = 1.0 / (h * h);
    const double dt = 1.0 / g.sample_rate_hz;
    const double alpha = dt * dt / (4.0 * g.linear_density_kg_per_m);
    std::vector<double> b(n * n, 0.0);
    candidate.stiffness_.assign(n * n, 0.0);
    candidate.cholesky_.assign(n * n, 0.0);
    candidate.port_weights_.resize(n);
    candidate.q_.assign(n, 0.0);
    candidate.v_.assign(n, 0.0);
    candidate.rhs_.resize(n);
    candidate.next_q_.resize(n);
    candidate.scratch_.resize(n);
    candidate.next_v_.resize(n);
    for (std::size_t i = 0; i < n; ++i) {
        b[i * n + i] = 2.0 * inv_h2;
        if (i > 0) {
            b[i * n + i - 1] = -inv_h2;
        }
        if (i + 1 < n) {
            b[i * n + i + 1] = -inv_h2;
        }
        const double nodal_location = static_cast<double>(i + 1);
        const double port_location = g.force_position_fraction * static_cast<double>(n + 1);
        candidate.port_weights_[i] = std::max(0.0, 1.0 - std::abs(nodal_location - port_location));
    }
    for (std::size_t i = 0; i < n; ++i) {
        for (std::size_t j = 0; j < n; ++j) {
            double b_squared = 0.0;
            for (std::size_t k = 0; k < n; ++k) {
                b_squared += b[i * n + k] * b[k * n + j];
            }
            const double value = g.tension_n * b[i * n + j] +
                                 g.bending_stiffness_n_m2 * b_squared;
            require(std::isfinite(value), "discrete stiffness matrix contains an invalid entry");
            candidate.stiffness_[i * n + j] = value;
            candidate.cholesky_[i * n + j] = (i == j ? 1.0 : 0.0) + alpha * value;
        }
    }
    // Dense Cholesky, even though the actual stiffness is pentadiagonal:
    // the high operation count is measured deliberately, not concealed.
    for (std::size_t i = 0; i < n; ++i) {
        for (std::size_t j = 0; j <= i; ++j) {
            double value = candidate.cholesky_[i * n + j];
            for (std::size_t k = 0; k < j; ++k) {
                value -= candidate.cholesky_[i * n + k] * candidate.cholesky_[j * n + k];
            }
            if (i == j) {
                require_positive(value, "midpoint solve matrix is not finite SPD");
                candidate.cholesky_[i * n + i] = std::sqrt(value);
            } else {
                candidate.cholesky_[i * n + j] = value / candidate.cholesky_[j * n + j];
            }
        }
    }
    candidate.parameters_ = g;
    *this = std::move(candidate); // Commit only after complete validation/factorization.
}

void MidpointStiffString::set_state(std::span<const double> q,
                                    std::span<const double> v) {
    if (!prepared()) {
        throw std::logic_error("prepare the grid before initializing its state");
    }
    require(q.size() == q_.size() && v.size() == v_.size(),
            "state spans must match the interior node count");
    for (std::size_t i = 0; i < q.size(); ++i) {
        require(std::isfinite(q[i]) && std::isfinite(v[i]),
                "state entries must be finite SI values");
    }
    require(std::isfinite(energy(parameters_, stiffness_, q, v)),
            "initial grid energy is not finite");
    std::copy(q.begin(), q.end(), q_.begin());
    std::copy(v.begin(), v.end(), v_.begin());
}

void MidpointStiffString::set_sine_state(double amplitude_m, std::size_t harmonic_number) {
    if (!prepared()) {
        throw std::logic_error("prepare the grid before initializing a sine mode");
    }
    require(std::isfinite(amplitude_m), "sine displacement amplitude must be finite (m)");
    require(harmonic_number >= 1 && harmonic_number <= q_.size(),
            "sine mode number must be in the spatial spectrum");
    std::vector<double> q(q_.size());
    std::vector<double> v(q_.size(), 0.0);
    for (std::size_t i = 0; i < q.size(); ++i) {
        q[i] = amplitude_m * std::sin(static_cast<double>(harmonic_number) * pi *
                                      static_cast<double>(i + 1) / static_cast<double>(q.size() + 1));
    }
    set_state(q, v);
}

void MidpointStiffString::reset() noexcept {
    std::fill(q_.begin(), q_.end(), 0.0);
    std::fill(v_.begin(), v_.end(), 0.0);
}

WorkStep MidpointStiffString::advance(double force_n) {
    if (!prepared()) {
        throw std::logic_error("prepare the grid before advancing");
    }
    require(std::isfinite(force_n), "midpoint external force must be finite (N)");
    const std::size_t n = q_.size();
    const double dt = 1.0 / parameters_.sample_rate_hz;
    const double mu = parameters_.linear_density_kg_per_m;
    const double h = parameters_.length_m / static_cast<double>(n + 1);
    const double alpha = dt * dt / (4.0 * mu);
    const double force_factor = dt * dt * force_n / (2.0 * mu * h);
    const double before = energy_j();
    for (std::size_t i = 0; i < n; ++i) {
        double kq = 0.0;
        for (std::size_t j = 0; j < n; ++j) {
            kq += stiffness_[i * n + j] * q_[j];
        }
        rhs_[i] = q_[i] + dt * v_[i] - alpha * kq + force_factor * port_weights_[i];
        require(std::isfinite(rhs_[i]), "forced midpoint right-hand side overflowed");
    }
    // Forward L*y = rhs; back-substitute L^T*q_next = y.
    for (std::size_t i = 0; i < n; ++i) {
        double value = rhs_[i];
        for (std::size_t j = 0; j < i; ++j) {
            value -= cholesky_[i * n + j] * next_q_[j];
        }
        next_q_[i] = value / cholesky_[i * n + i];
    }
    for (std::size_t ii = n; ii-- > 0;) {
        double value = next_q_[ii];
        for (std::size_t j = ii + 1; j < n; ++j) {
            value -= cholesky_[j * n + ii] * scratch_[j];
        }
        scratch_[ii] = value / cholesky_[ii * n + ii];
        next_v_[ii] = 2.0 * (scratch_[ii] - q_[ii]) / dt - v_[ii];
    }
    const double after = energy(parameters_, stiffness_, scratch_, next_v_);
    double velocity_mid = 0.0;
    for (std::size_t i = 0; i < n; ++i) {
        velocity_mid += port_weights_[i] * 0.5 * (v_[i] + next_v_[i]);
    }
    const double work = force_n * velocity_mid * dt;
    require(std::isfinite(after) && std::isfinite(work),
            "forced midpoint step would exceed finite representable energy");
    std::copy(scratch_.begin(), scratch_.end(), q_.begin());
    std::copy(next_v_.begin(), next_v_.end(), v_.begin());
    return {velocity_mid, work, after - before, (after - before) - work};
}

double MidpointStiffString::energy_j() const noexcept {
    if (!prepared()) {
        return 0.0;
    }
    return energy(parameters_, stiffness_, q_, v_);
}

double MidpointStiffString::port_velocity_m_per_s() const noexcept {
    double value = 0.0;
    for (std::size_t i = 0; i < v_.size(); ++i) {
        value += port_weights_[i] * v_[i];
    }
    return value;
}

double MidpointStiffString::port_displacement_m() const noexcept {
    double value = 0.0;
    for (std::size_t i = 0; i < q_.size(); ++i) {
        value += port_weights_[i] * q_[i];
    }
    return value;
}

std::span<const double> MidpointStiffString::displacement_m() const noexcept { return q_; }
std::span<const double> MidpointStiffString::velocity_m_per_s() const noexcept { return v_; }
bool MidpointStiffString::prepared() const noexcept { return !q_.empty(); }
const StringGrid& MidpointStiffString::parameters() const noexcept { return parameters_; }

void MidpointTwoMassPort::prepare(const TwoMassParameters& p) {
    require_positive(p.mass_1_kg, "first mass must be positive and finite (kg)");
    require_positive(p.mass_2_kg, "second mass must be positive and finite (kg)");
    require(std::isfinite(p.coupling_stiffness_n_per_m) && p.coupling_stiffness_n_per_m >= 0.0,
            "spring stiffness must be finite and nonnegative (N/m)");
    require(std::isfinite(p.sample_rate_hz) && p.sample_rate_hz >= 8000.0 &&
                p.sample_rate_hz <= 384000.0,
            "two-mass sample rate outside supported range (Hz)");
    require_positive(p.mass_1_kg + p.mass_2_kg, "total mass overflowed");
    const double reduced_mass = (p.mass_1_kg * p.mass_2_kg) /
                                (p.mass_1_kg + p.mass_2_kg);
    require_positive(reduced_mass, "reduced mass underflowed or overflowed");
    require(std::isfinite(p.coupling_stiffness_n_per_m / reduced_mass),
            "relative-mode frequency is not finite");
    parameters_ = p;
    state_ = {};
    prepared_ = true;
}

void MidpointTwoMassPort::set_state(TwoMassState s) {
    if (!prepared_) {
        throw std::logic_error("prepare two-mass port before assigning state");
    }
    require(std::isfinite(s.x1_m) && std::isfinite(s.x2_m) &&
                std::isfinite(s.v1_m_per_s) && std::isfinite(s.v2_m_per_s) &&
                std::isfinite(two_mass_energy(parameters_, s)),
            "two-mass state or energy is not finite");
    state_ = s;
}

CouplingStep MidpointTwoMassPort::advance() {
    if (!prepared_) {
        throw std::logic_error("prepare two-mass port before stepping");
    }
    const auto& p = parameters_;
    const auto s = state_;
    const double dt = 1.0 / p.sample_rate_hz;
    const double total_mass = p.mass_1_kg + p.mass_2_kg;
    const double reduced_mass = p.mass_1_kg * p.mass_2_kg / total_mass;
    const double extension = s.x1_m - s.x2_m;
    const double relative_velocity = s.v1_m_per_s - s.v2_m_per_s;
    const double midpoint_coefficient =
        0.25 * dt * dt * p.coupling_stiffness_n_per_m / reduced_mass;
    const double next_extension = ((1.0 - midpoint_coefficient) * extension +
                                   dt * relative_velocity) / (1.0 + midpoint_coefficient);
    const double next_relative_velocity = 2.0 * (next_extension - extension) / dt -
                                          relative_velocity;
    const double center = (p.mass_1_kg * s.x1_m + p.mass_2_kg * s.x2_m) / total_mass;
    const double center_velocity = (p.mass_1_kg * s.v1_m_per_s +
                                    p.mass_2_kg * s.v2_m_per_s) / total_mass;
    const double next_center = center + dt * center_velocity;
    const double weight_1 = p.mass_2_kg / total_mass;
    const double weight_2 = p.mass_1_kg / total_mass;
    TwoMassState next{next_center + weight_1 * next_extension,
                      next_center - weight_2 * next_extension,
                      center_velocity + weight_1 * next_relative_velocity,
                      center_velocity - weight_2 * next_relative_velocity};
    const double old_energy = energy_j();
    const double next_energy = two_mass_energy(p, next);
    require(std::isfinite(next_energy), "two-mass step would exceed finite energy");
    const double force_1 = -p.coupling_stiffness_n_per_m *
                           (extension + next_extension) / 2.0;
    const double velocity_1 = (s.v1_m_per_s + next.v1_m_per_s) / 2.0;
    const double velocity_2 = (s.v2_m_per_s + next.v2_m_per_s) / 2.0;
    const double spring_change = 0.5 * p.coupling_stiffness_n_per_m *
                                 (next_extension * next_extension - extension * extension);
    const double ports_work = dt * (force_1 * velocity_1 - force_1 * velocity_2);
    state_ = next;
    return {force_1, -force_1, velocity_1, velocity_2, spring_change,
            ports_work, next_energy - old_energy};
}

TwoMassState MidpointTwoMassPort::state() const noexcept { return state_; }
double MidpointTwoMassPort::energy_j() const noexcept {
    return prepared_ ? two_mass_energy(parameters_, state_) : 0.0;
}
double MidpointTwoMassPort::linear_momentum_kg_m_per_s() const noexcept {
    return parameters_.mass_1_kg * state_.v1_m_per_s +
           parameters_.mass_2_kg * state_.v2_m_per_s;
}
bool MidpointTwoMassPort::prepared() const noexcept { return prepared_; }

} // namespace setar::experimental
