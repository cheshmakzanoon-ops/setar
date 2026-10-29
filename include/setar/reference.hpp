#pragma once

#include <cstddef>
#include <span>
#include <vector>

namespace setar::reference {

// SYNTHETIC NUMERICAL FIXTURE. These defaults are not measurements of a setar.
// Simply-supported, small-displacement, uniform stiff string, one polarization.
struct StringParameters {
    double length_m = 0.66;
    double tension_n = 35.0;
    double linear_density_kg_per_m = 0.0007;
    double bending_stiffness_n_m2 = 0.000015;
    double decay_constant_per_s = 0.8;
    double decay_frequency_squared_s = 0.0;
    std::size_t mode_count = 16;
    // Appended after mode_count to keep existing aggregate initialization valid.
    // Amplitude decay of mode n is
    //   sigma_n = decay_constant + decay_wavenumber_squared*k_n^2 + decay_frequency_squared*omega_n^2
    // with k_n = n*pi/L (rad/m): the u_txx loss of docs/strings/03-loss-and-dispersion.md.
    double decay_wavenumber_squared_m2_per_s = 0.0;
};

inline constexpr std::size_t maximum_mode_count = 128;
inline constexpr double minimum_sample_rate_hz = 8000.0;
inline constexpr double maximum_sample_rate_hz = 384000.0;
inline constexpr double maximum_frequency_fraction = 0.45;

// Throws std::invalid_argument on unsupported or non-finite inputs. No clamping.
void validate(const StringParameters& parameters, double sample_rate_hz);

struct ModeState {
    double displacement_m = 0.0;
    double velocity_m_per_s = 0.0;
    double initial_displacement_m = 0.0;
    double angular_frequency_rad_per_s = 0.0;
    double decay_per_s = 0.0;
    double mass_kg = 0.0;
};

// Exact (up to floating-point arithmetic) sampled free response of one linear,
// underdamped oscillator: m*q'' + 2*m*sigma*q' + m*omega^2*q = 0.
// prepare(), set_state(), and apply_impulse() are control/offline operations.
// advance(), displacement(), velocity(), and energy() do not allocate or throw.
class PassiveResonator {
  public:
    void prepare(double mass_kg, double frequency_hz, double decay_per_s,
                 double sample_rate_hz);
    void set_state(double displacement_m, double velocity_m_per_s);
    void apply_impulse(double impulse_n_s);
    void reset() noexcept;
    void advance() noexcept;
    [[nodiscard]] double displacement() const noexcept;
    [[nodiscard]] double velocity() const noexcept;
    [[nodiscard]] double energy() const noexcept;
    [[nodiscard]] bool prepared() const noexcept;

  private:
    ModeState state_{};
    double qq_ = 0.0;
    double qv_ = 0.0;
    double vq_ = 0.0;
    double vv_ = 0.0;
    bool prepared_ = false;
};

// Reference modal model, not a completed instrument engine. Preparation allocates;
// the bounded render path does not. prepare() is transactional on invalid input.
// Control and render operations must not run concurrently on the same instance.
class ModalString {
  public:
    void prepare(const StringParameters& parameters, double sample_rate_hz,
                 double pickup_fraction = 0.73);

    // Configures a triangular displacement with zero velocity; does not trigger.
    // Interior location and a maximum initial segment slope of 0.1 are required.
    // This is a linear fixture initialization, not a nail-contact simulation.
    void configure_pluck(double height_m, double position_fraction);
    void pluck(double height_m, double position_fraction);
    void retrigger() noexcept;
    void reset() noexcept;
    void advance() noexcept;
    [[nodiscard]] double displacement() const noexcept;
    [[nodiscard]] double tick() noexcept; // Observe at t, then advance to t + dt.
    void render(std::span<double> output_m) noexcept;
    [[nodiscard]] double energy() const noexcept;
    [[nodiscard]] bool prepared() const noexcept;
    [[nodiscard]] std::span<const ModeState> modes() const noexcept;
    [[nodiscard]] const StringParameters& parameters() const noexcept;
    [[nodiscard]] double sample_rate() const noexcept;
    [[nodiscard]] double pickup_fraction() const noexcept;

  private:
    struct Transition {
        double qq;
        double qv;
        double vq;
        double vv;
        double observation_weight;
    };
    StringParameters parameters_{};
    std::vector<ModeState> modes_;
    std::vector<Transition> transitions_;
    double sample_rate_hz_ = 0.0;
    double pickup_fraction_ = 0.0;
};

} // namespace setar::reference

