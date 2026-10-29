#include "setar/reference.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <numbers>
#include <stdexcept>
#include <string>

namespace setar::reference {
namespace {
constexpr double pi = std::numbers::pi_v<double>;

void require(bool condition, const char* message) {
    if (!condition) {
        throw std::invalid_argument(message);
    }
}

void require_positive(double value, const char* message) {
    require(std::isfinite(value) && value > 0.0, message);
}

void require_nonnegative(double value, const char* message) {
    require(std::isfinite(value) && value >= 0.0, message);
}

void validate_sample_rate(double sample_rate_hz) {
    require(std::isfinite(sample_rate_hz) && sample_rate_hz >= minimum_sample_rate_hz &&
                sample_rate_hz <= maximum_sample_rate_hz,
            "sample rate must be finite and between 8000 and 384000 Hz");
}

double angular_frequency(const StringParameters& parameters, std::size_t mode_number) {
    const double k = static_cast<double>(mode_number) * pi / parameters.length_m;
    const double k2 = k * k;
    return std::sqrt((parameters.tension_n / parameters.linear_density_kg_per_m) * k2 +
                     (parameters.bending_stiffness_n_m2 /
                      parameters.linear_density_kg_per_m) *
                         k2 * k2);
}

// Amplitude decay of one mode; every term is nonnegative, so no sign is chosen by hand.
double decay_rate(const StringParameters& parameters, std::size_t mode_number, double omega) {
    const double k = static_cast<double>(mode_number) * pi / parameters.length_m;
    return parameters.decay_constant_per_s +
           parameters.decay_wavenumber_squared_m2_per_s * k * k +
           parameters.decay_frequency_squared_s * omega * omega;
}

struct Coefficients {
    double qq;
    double qv;
    double vq;
    double vv;
};

Coefficients coefficients(double omega, double sigma, double sample_rate_hz) {
    // Stable near critical damping: (omega - sigma)*(omega + sigma) avoids
    // cancellation in omega*omega - sigma*sigma. sigma < omega is enforced.
    const double omega_d = std::sqrt((omega - sigma) * (omega + sigma));
    const double dt = 1.0 / sample_rate_hz;
    const double theta = omega_d * dt;
    const double decay = std::exp(-sigma * dt);
    const double c = std::cos(theta);
    // sin(theta)/theta is near one, not ill-conditioned, at small theta.
    const double sinc = theta == 0.0 ? 1.0 : std::sin(theta) / theta;
    const double s = dt * sinc;
    return {decay * (c + sigma * s), decay * s, -decay * omega * omega * s,
            decay * (c - sigma * s)};
}

double state_energy(const ModeState& state) noexcept {
    const double weighted_displacement =
        state.angular_frequency_rad_per_s * state.displacement_m;
    return 0.5 * state.mass_kg *
           (state.velocity_m_per_s * state.velocity_m_per_s +
            weighted_displacement * weighted_displacement);
}
} // namespace

void validate(const StringParameters& parameters, double sample_rate_hz) {
    validate_sample_rate(sample_rate_hz);
    require_positive(parameters.length_m, "length must be positive and finite (m)");
    require_positive(parameters.tension_n, "tension must be positive and finite (N)");
    require_positive(parameters.linear_density_kg_per_m,
                     "linear density must be positive and finite (kg/m)");
    require_nonnegative(parameters.bending_stiffness_n_m2,
                        "bending stiffness must be nonnegative and finite (N m^2)");
    require_nonnegative(parameters.decay_constant_per_s,
                        "constant decay must be nonnegative and finite (1/s)");
    require_nonnegative(parameters.decay_frequency_squared_s,
                        "frequency-squared decay coefficient must be nonnegative and finite (s)");
    require_nonnegative(parameters.decay_wavenumber_squared_m2_per_s,
                        "wavenumber-squared decay coefficient must be nonnegative and finite (m^2/s)");
    require(parameters.mode_count >= 1 && parameters.mode_count <= maximum_mode_count,
            "mode count must be between 1 and 128");
    const double mass = parameters.linear_density_kg_per_m * parameters.length_m / 2.0;
    require_positive(mass, "derived modal mass is non-finite or underflowed");
    for (std::size_t mode_number = 1; mode_number <= parameters.mode_count; ++mode_number) {
        const double omega = angular_frequency(parameters, mode_number);
        require_positive(omega, "derived angular frequency is non-finite or underflowed");
        require(omega / (2.0 * pi) < maximum_frequency_fraction * sample_rate_hz,
                "mode exceeds the strict 0.45 * sample-rate observation limit");
        const double sigma = decay_rate(parameters, mode_number, omega);
        require(std::isfinite(sigma) && sigma >= 0.0 && sigma < omega,
                "each mode must have finite nonnegative undercritical decay (sigma < omega)");
    }
}

void PassiveResonator::prepare(double mass_kg, double frequency_hz, double decay_per_s,
                              double sample_rate_hz) {
    validate_sample_rate(sample_rate_hz);
    require_positive(mass_kg, "resonator mass must be positive and finite (kg)");
    require_positive(frequency_hz, "resonator frequency must be positive and finite (Hz)");
    require(frequency_hz < maximum_frequency_fraction * sample_rate_hz,
            "resonator frequency exceeds observation limit");
    const double omega = 2.0 * pi * frequency_hz;
    require_positive(omega * omega, "resonator squared frequency is non-finite or underflowed");
    require(std::isfinite(decay_per_s) && decay_per_s >= 0.0 && decay_per_s < omega,
            "resonator decay must be finite, nonnegative, and undercritical");
    const auto transition = coefficients(omega, decay_per_s, sample_rate_hz);
    state_ = {0.0, 0.0, 0.0, omega, decay_per_s, mass_kg};
    qq_ = transition.qq;
    qv_ = transition.qv;
    vq_ = transition.vq;
    vv_ = transition.vv;
    prepared_ = true;
}

void PassiveResonator::set_state(double displacement_m, double velocity_m_per_s) {
    if (!prepared_) {
        throw std::logic_error("prepare the resonator before setting state");
    }
    require(std::isfinite(displacement_m) && std::isfinite(velocity_m_per_s),
            "resonator initial state must be finite");
    ModeState candidate = state_;
    candidate.displacement_m = displacement_m;
    candidate.velocity_m_per_s = velocity_m_per_s;
    require(std::isfinite(state_energy(candidate)), "resonator initial energy would overflow");
    state_ = candidate;
}

void PassiveResonator::apply_impulse(double impulse_n_s) {
    if (!prepared_) {
        throw std::logic_error("prepare the resonator before applying an impulse");
    }
    require(std::isfinite(impulse_n_s), "impulse must be finite (N s)");
    set_state(state_.displacement_m, state_.velocity_m_per_s + impulse_n_s / state_.mass_kg);
}

void PassiveResonator::reset() noexcept {
    state_.displacement_m = 0.0;
    state_.velocity_m_per_s = 0.0;
}

void PassiveResonator::advance() noexcept {
    const double q = state_.displacement_m;
    const double v = state_.velocity_m_per_s;
    state_.displacement_m = qq_ * q + qv_ * v;
    state_.velocity_m_per_s = vq_ * q + vv_ * v;
}

double PassiveResonator::displacement() const noexcept { return state_.displacement_m; }
double PassiveResonator::velocity() const noexcept { return state_.velocity_m_per_s; }
double PassiveResonator::energy() const noexcept { return state_energy(state_); }
bool PassiveResonator::prepared() const noexcept { return prepared_; }

void ModalString::prepare(const StringParameters& parameters, double sample_rate_hz,
                          double pickup_fraction) {
    validate(parameters, sample_rate_hz);
    require(std::isfinite(pickup_fraction) && pickup_fraction >= 0.0 && pickup_fraction <= 1.0,
            "pickup fraction must be finite and in [0, 1]");
    std::vector<ModeState> next_modes;
    std::vector<Transition> next_transitions;
    next_modes.reserve(parameters.mode_count);
    next_transitions.reserve(parameters.mode_count);
    const double mass = parameters.linear_density_kg_per_m * parameters.length_m / 2.0;
    for (std::size_t n = 1; n <= parameters.mode_count; ++n) {
        const double omega = angular_frequency(parameters, n);
        const double sigma = decay_rate(parameters, n, omega);
        const auto transition = coefficients(omega, sigma, sample_rate_hz);
        const double weight = pickup_fraction == 0.0 || pickup_fraction == 1.0
                                  ? 0.0
                                  : std::sin(static_cast<double>(n) * pi * pickup_fraction);
        next_modes.push_back({0.0, 0.0, 0.0, omega, sigma, mass});
        next_transitions.push_back(
            {transition.qq, transition.qv, transition.vq, transition.vv, weight});
    }
    modes_.swap(next_modes);
    transitions_.swap(next_transitions);
    parameters_ = parameters;
    sample_rate_hz_ = sample_rate_hz;
    pickup_fraction_ = pickup_fraction;
}

void ModalString::configure_pluck(double height_m, double position_fraction) {
    if (!prepared()) {
        throw std::logic_error("prepare the string before configuring a pluck");
    }
    require(std::isfinite(height_m), "pluck height must be finite (m)");
    require(std::isfinite(position_fraction) && position_fraction > 0.0 &&
                position_fraction < 1.0,
            "pluck position must be finite and strictly inside (0, 1)");
    const double shortest_segment =
        parameters_.length_m * std::min(position_fraction, 1.0 - position_fraction);
    require(std::abs(height_m) <= 0.1 * shortest_segment,
            "initial triangle segment slope exceeds the linear fixture limit of 0.1");
    std::array<double, maximum_mode_count> amplitudes{};
    double total_energy = 0.0;
    for (std::size_t index = 0; index < modes_.size(); ++index) {
        const double n = static_cast<double>(index + 1);
        // Fourier sine coefficient of a continuous triangular displacement.
        const double amplitude =
            2.0 * height_m * std::sin(n * pi * position_fraction) /
            (n * n * pi * pi * position_fraction * (1.0 - position_fraction));
        require(std::isfinite(amplitude), "pluck modal amplitude is non-finite");
        ModeState candidate = modes_[index];
        candidate.displacement_m = amplitude;
        candidate.velocity_m_per_s = 0.0;
        total_energy += state_energy(candidate);
        amplitudes[index] = amplitude;
    }
    require(std::isfinite(total_energy), "pluck initial modal energy would overflow");
    for (std::size_t index = 0; index < modes_.size(); ++index) {
        modes_[index].initial_displacement_m = amplitudes[index];
    }
}

void ModalString::pluck(double height_m, double position_fraction) {
    configure_pluck(height_m, position_fraction);
    retrigger();
}

void ModalString::retrigger() noexcept {
    for (auto& mode : modes_) {
        mode.displacement_m = mode.initial_displacement_m;
        mode.velocity_m_per_s = 0.0;
    }
}

void ModalString::reset() noexcept {
    for (auto& mode : modes_) {
        mode.displacement_m = 0.0;
        mode.velocity_m_per_s = 0.0;
    }
}

void ModalString::advance() noexcept {
    for (std::size_t index = 0; index < modes_.size(); ++index) {
        auto& mode = modes_[index];
        const auto& transition = transitions_[index];
        const double q = mode.displacement_m;
        const double v = mode.velocity_m_per_s;
        mode.displacement_m = transition.qq * q + transition.qv * v;
        mode.velocity_m_per_s = transition.vq * q + transition.vv * v;
    }
}

double ModalString::displacement() const noexcept {
    double value = 0.0;
    for (std::size_t index = 0; index < modes_.size(); ++index) {
        value += modes_[index].displacement_m * transitions_[index].observation_weight;
    }
    return value;
}

double ModalString::tick() noexcept {
    const double value = displacement();
    advance();
    return value;
}

void ModalString::render(std::span<double> output_m) noexcept {
    for (double& value : output_m) {
        value = tick();
    }
}

double ModalString::energy() const noexcept {
    double value = 0.0;
    for (const auto& mode : modes_) {
        value += state_energy(mode);
    }
    return value;
}

bool ModalString::prepared() const noexcept { return !modes_.empty(); }
std::span<const ModeState> ModalString::modes() const noexcept { return modes_; }
const StringParameters& ModalString::parameters() const noexcept { return parameters_; }
double ModalString::sample_rate() const noexcept { return sample_rate_hz_; }
double ModalString::pickup_fraction() const noexcept { return pickup_fraction_; }

} // namespace setar::reference
