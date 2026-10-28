#include "setar/reference.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <exception>
#include <iostream>
#include <limits>
#include <numbers>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {
using setar::reference::ModalString;
using setar::reference::PassiveResonator;
using setar::reference::StringParameters;
constexpr long double pi = std::numbers::pi_v<long double>;

void check(bool condition, std::string_view message) {
    if (!condition) {
        throw std::runtime_error(std::string(message));
    }
}

void close(double actual, long double expected, long double absolute_tolerance,
           long double relative_tolerance, std::string_view message) {
    const long double error = std::abs(static_cast<long double>(actual) - expected);
    const long double limit = absolute_tolerance + relative_tolerance * std::abs(expected);
    if (!std::isfinite(actual) || !std::isfinite(expected) || error > limit) {
        throw std::runtime_error(std::string(message) + ": actual=" + std::to_string(actual) +
                                 ", expected=" + std::to_string(static_cast<double>(expected)));
    }
}

template <typename Function> void invalid(Function&& function, std::string_view message) {
    bool rejected = false;
    try {
        function();
    } catch (const std::invalid_argument&) {
        rejected = true;
    }
    check(rejected, message);
}

template <typename Function> void logic_error(Function&& function, std::string_view message) {
    bool rejected = false;
    try {
        function();
    } catch (const std::logic_error&) {
        rejected = true;
    }
    check(rejected, message);
}

long double reference_omega(const StringParameters& p, std::size_t mode_number) {
    // Independently evaluate the characteristic equation in extended precision.
    const long double n = static_cast<long double>(mode_number);
    const long double length = static_cast<long double>(p.length_m);
    const long double density = static_cast<long double>(p.linear_density_kg_per_m);
    const long double tension_term = static_cast<long double>(p.tension_n) * n * n * pi * pi /
                                     (density * length * length);
    const long double stiffness_term =
        static_cast<long double>(p.bending_stiffness_n_m2) * n * n * n * n * pi * pi * pi * pi /
        (density * length * length * length * length);
    return std::sqrt(tension_term + stiffness_term);
}

void parameters() {
    const StringParameters good;
    for (double rate : {8000.0, 44100.0, 48000.0, 96000.0, 192000.0, 384000.0}) {
        setar::reference::validate(good, rate);
    }
    const double nan = std::numeric_limits<double>::quiet_NaN();
    const double inf = std::numeric_limits<double>::infinity();
    for (double rate : {0.0, -48000.0, 7999.0, 384001.0, nan, inf}) {
        invalid([&] { setar::reference::validate(good, rate); }, "reject invalid sample rate");
    }
    for (double bad_value : {0.0, -1.0, nan, inf}) {
        auto p = good;
        p.length_m = bad_value;
        invalid([&] { setar::reference::validate(p, 48000.0); }, "reject invalid length");
        p = good;
        p.tension_n = bad_value;
        invalid([&] { setar::reference::validate(p, 48000.0); }, "reject invalid tension");
        p = good;
        p.linear_density_kg_per_m = bad_value;
        invalid([&] { setar::reference::validate(p, 48000.0); }, "reject invalid density");
    }
    for (double bad_value : {-1.0, nan, inf}) {
        auto p = good;
        p.bending_stiffness_n_m2 = bad_value;
        invalid([&] { setar::reference::validate(p, 48000.0); }, "reject invalid stiffness");
        p = good;
        p.decay_constant_per_s = bad_value;
        invalid([&] { setar::reference::validate(p, 48000.0); }, "reject invalid constant decay");
        p = good;
        p.decay_frequency_squared_s = bad_value;
        invalid([&] { setar::reference::validate(p, 48000.0); }, "reject invalid frequency decay");
    }
    for (std::size_t count : {std::size_t{0}, std::size_t{129},
                              std::numeric_limits<std::size_t>::max()}) {
        auto p = good;
        p.mode_count = count;
        invalid([&] { setar::reference::validate(p, 48000.0); }, "reject invalid mode count");
    }
    auto p = good;
    p.mode_count = 128;
    invalid([&] { setar::reference::validate(p, 8000.0); }, "reject out-of-band modes");
    p = good;
    p.decay_constant_per_s = 10000.0;
    invalid([&] { setar::reference::validate(p, 48000.0); }, "reject overcritical damping");
    p = good;
    p.length_m = std::numeric_limits<double>::min();
    invalid([&] { setar::reference::validate(p, 48000.0); }, "reject frequency overflow");
    p = good;
    p.linear_density_kg_per_m = std::numeric_limits<double>::max();
    p.length_m = 4.0;
    invalid([&] { setar::reference::validate(p, 48000.0); }, "reject modal mass overflow");
    ModalString unprepared;
    check(!unprepared.prepared(), "unprepared state is observable");
    check(unprepared.tick() == 0.0 && unprepared.energy() == 0.0, "unprepared render is silence");
    logic_error([&] { unprepared.pluck(0.0005, 0.23); }, "unprepared pluck rejected");
}

void frequencies() {
    StringParameters p;
    p.mode_count = 32;
    ModalString model;
    model.prepare(p, 96000.0);
    for (std::size_t index = 0; index < model.modes().size(); ++index) {
        close(model.modes()[index].angular_frequency_rad_per_s, reference_omega(p, index + 1),
              1.0e-10L, 5.0e-15L, "stiff-string characteristic frequency");
        close(model.modes()[index].mass_kg,
              static_cast<long double>(p.linear_density_kg_per_m) * p.length_m / 2.0L,
              1.0e-18L, 1.0e-15L, "modal mass with unit-amplitude sine normalization");
    }
    p.bending_stiffness_n_m2 = 0.0;
    model.prepare(p, 96000.0);
    const long double first = model.modes().front().angular_frequency_rad_per_s;
    for (std::size_t index = 0; index < model.modes().size(); ++index) {
        close(model.modes()[index].angular_frequency_rad_per_s,
              first * static_cast<long double>(index + 1), 1.0e-10L, 5.0e-15L,
              "zero-stiffness harmonic relationship");
    }
}

void analytic_response() {
    StringParameters p;
    p.decay_constant_per_s = 2.0;
    p.decay_frequency_squared_s = 2.0e-9;
    ModalString model;
    model.prepare(p, 48000.0);
    model.pluck(0.0005, 0.23);
    const std::vector<setar::reference::ModeState> initial(model.modes().begin(), model.modes().end());
    constexpr std::size_t steps = 6000;
    for (std::size_t step = 0; step < steps; ++step) {
        model.advance();
    }
    const long double time = static_cast<long double>(steps) / 48000.0L;
    for (std::size_t index = 0; index < initial.size(); ++index) {
        const long double omega = reference_omega(p, index + 1);
        const long double sigma = static_cast<long double>(p.decay_constant_per_s) +
                                  static_cast<long double>(p.decay_frequency_squared_s) * omega * omega;
        const long double wd = std::sqrt(omega * omega - sigma * sigma);
        const long double amplitude = initial[index].displacement_m;
        const long double decay = std::exp(-sigma * time);
        const long double expected_q = amplitude * decay *
                                       (std::cos(wd * time) + sigma / wd * std::sin(wd * time));
        const long double expected_v =
            -amplitude * decay * omega * omega / wd * std::sin(wd * time);
        close(model.modes()[index].displacement_m, expected_q, 3.0e-15L, 2.0e-10L,
              "analytic modal displacement after repeated updates");
        close(model.modes()[index].velocity_m_per_s, expected_v, 3.0e-12L, 2.0e-10L,
              "analytic modal velocity after repeated updates");
    }
}

void lossless_energy() {
    StringParameters p;
    p.decay_constant_per_s = 0.0;
    ModalString model;
    model.prepare(p, 48000.0);
    model.pluck(0.0005, 0.23);
    const double initial_energy = model.energy();
    check(initial_energy > 0.0, "nonzero initial energy");
    double largest_relative_error = 0.0;
    for (std::size_t index = 0; index < 48000; ++index) {
        model.advance();
        largest_relative_error = std::max(largest_relative_error,
                                          std::abs(model.energy() / initial_energy - 1.0));
    }
    check(largest_relative_error < 1.0e-10, "lossless finite-mode energy error below 1e-10");
    std::cout << "maximum_relative_energy_error=" << largest_relative_error << '\n';
}

void passivity() {
    StringParameters p;
    p.decay_constant_per_s = 3.0;
    p.decay_frequency_squared_s = 2.0e-9;
    ModalString model;
    model.prepare(p, 48000.0);
    model.pluck(0.0005, 0.23);
    const double initial_energy = model.energy();
    double previous_energy = initial_energy;
    for (std::size_t index = 0; index < 24000; ++index) {
        model.advance();
        const double energy = model.energy();
        check(std::isfinite(energy) && energy >= 0.0, "finite nonnegative modal energy");
        check(energy <= previous_energy + 1.0e-13 * initial_energy,
              "no unforced passive energy increase above rounding allowance");
        previous_energy = energy;
    }
    check(previous_energy < 0.1 * initial_energy, "specified decay produces energy loss");
}

void pluck_projection() {
    ModalString model;
    model.prepare(StringParameters{}, 48000.0);
    constexpr long double height = 0.0005L;
    constexpr long double position = 0.25L;
    model.pluck(static_cast<double>(height), static_cast<double>(position));
    // Independent Simpson quadrature: 2*integral_0^1 triangle(x)*sin(n*pi*x) dx.
    // The kink is exactly on the quadrature grid; production uses a closed formula.
    constexpr std::size_t intervals = 16384;
    for (std::size_t mode_index = 0; mode_index < model.modes().size(); ++mode_index) {
        long double integral = 0.0L;
        for (std::size_t index = 0; index <= intervals; ++index) {
            const long double x = static_cast<long double>(index) / intervals;
            const long double triangle =
                x <= position ? height * x / position : height * (1.0L - x) / (1.0L - position);
            const long double weight = index == 0 || index == intervals ? 1.0L
                                       : index % 2 == 0                ? 2.0L
                                                                       : 4.0L;
            integral += weight * triangle * std::sin(static_cast<long double>(mode_index + 1) * pi * x);
        }
        integral *= 2.0L / (3.0L * static_cast<long double>(intervals));
        close(model.modes()[mode_index].displacement_m, integral, 2.0e-13L, 1.0e-10L,
              "triangle coefficient against independent quadrature");
        check(model.modes()[mode_index].velocity_m_per_s == 0.0, "pluck begins at rest");
    }
    const double prior_displacement = model.displacement();
    for (double location : {0.0, 1.0, -0.2, 1.2, std::numeric_limits<double>::quiet_NaN()}) {
        invalid([&] { model.pluck(0.0005, location); }, "reject invalid pluck location");
        check(model.displacement() == prior_displacement, "failed pluck preserves current state");
    }
    invalid([&] { model.pluck(1.0, 0.23); }, "reject slope outside linear domain");
    invalid([&] { model.pluck(std::numeric_limits<double>::infinity(), 0.23); },
            "reject infinite pluck height");
}

long double triangle_error(std::size_t modes) {
    StringParameters p;
    p.mode_count = modes;
    ModalString model;
    model.prepare(p, 192000.0);
    model.pluck(0.0005, 0.25);
    long double squared_error = 0.0L;
    constexpr std::size_t points = 4096;
    for (std::size_t index = 0; index <= points; ++index) {
        const long double x = static_cast<long double>(index) / points;
        const long double triangle = x <= 0.25L ? 0.0005L * x / 0.25L
                                               : 0.0005L * (1.0L - x) / 0.75L;
        long double reconstructed = 0.0L;
        for (std::size_t mode = 0; mode < modes; ++mode) {
            reconstructed += static_cast<long double>(model.modes()[mode].displacement_m) *
                             std::sin(static_cast<long double>(mode + 1) * pi * x);
        }
        const long double difference = reconstructed - triangle;
        squared_error += difference * difference;
    }
    return std::sqrt(squared_error / static_cast<long double>(points + 1));
}

void convergence() {
    long double previous_error = triangle_error(4);
    const long double first_error = previous_error;
    for (std::size_t modes : {std::size_t{8}, std::size_t{16}, std::size_t{32}, std::size_t{64}}) {
        const long double error = triangle_error(modes);
        check(error < previous_error, "more modes reduce displacement reconstruction L2 error");
        previous_error = error;
        std::cout << "modes=" << modes << ", displacement_L2_error_m="
                  << static_cast<double>(error) << '\n';
    }
    check(previous_error < 0.03L * first_error, "64-mode displacement error reduction");
    // NOT energy convergence: a sharp triangle has infinite continuum bending
    // energy. A finite-mode projection regularizes the kink at this truncation.
    ModalString low_rate;
    ModalString high_rate;
    low_rate.prepare(StringParameters{}, 48000.0);
    high_rate.prepare(StringParameters{}, 96000.0);
    low_rate.pluck(0.0005, 0.23);
    high_rate.pluck(0.0005, 0.23);
    for (std::size_t index = 0; index < 6000; ++index) {
        low_rate.advance();
        high_rate.advance();
        high_rate.advance();
    }
    for (std::size_t index = 0; index < low_rate.modes().size(); ++index) {
        close(low_rate.modes()[index].displacement_m, high_rate.modes()[index].displacement_m,
              3.0e-15L, 1.0e-9L, "sample-rate agreement at common physical time");
    }
}

void reset() {
    ModalString model;
    model.prepare(StringParameters{}, 48000.0);
    model.configure_pluck(0.0005, 0.23);
    check(model.displacement() == 0.0 && model.energy() == 0.0, "configuration does not trigger");
    model.retrigger();
    std::vector<double> expected(1024);
    model.render(expected);
    model.reset();
    for (std::size_t index = 0; index < 1024; ++index) {
        check(model.tick() == 0.0 && model.energy() == 0.0, "reset yields exact silence");
    }
    model.retrigger();
    for (double value : expected) {
        check(model.tick() == value, "retrigger is bit-identical within one executable");
    }
    const double before = model.displacement();
    auto bad = StringParameters{};
    bad.length_m = 0.0;
    invalid([&] { model.prepare(bad, 48000.0); }, "bad prepare rejected");
    check(model.displacement() == before && model.sample_rate() == 48000.0,
          "failed prepare preserves prior usable instance");
    model.prepare(StringParameters{}, 44100.0);
    check(model.displacement() == 0.0 && model.energy() == 0.0, "reprepare clears state");
    model.retrigger();
    check(model.displacement() == 0.0, "reprepare clears configured excitation");
}

void block_partition() {
    ModalString continuous;
    ModalString partitioned;
    continuous.prepare(StringParameters{}, 48000.0);
    partitioned.prepare(StringParameters{}, 48000.0);
    continuous.pluck(0.0005, 0.23);
    partitioned.pluck(0.0005, 0.23);
    std::vector<double> expected(1024);
    std::vector<double> actual(1024);
    continuous.render(expected);
    std::size_t offset = 0;
    for (std::size_t count : {std::size_t{1}, std::size_t{7}, std::size_t{31}, std::size_t{64},
                              std::size_t{127}, std::size_t{257}, std::size_t{537}}) {
        partitioned.render(std::span<double>(actual).subspan(offset, count));
        partitioned.render(std::span<double>{});
        offset += count;
    }
    check(offset == actual.size(), "block fixture covers all samples");
    check(actual == expected, "block partition does not alter samples");
}

void resonator() {
    PassiveResonator oscillator;
    check(!oscillator.prepared(), "resonator begins unprepared");
    oscillator.advance();
    check(oscillator.displacement() == 0.0 && oscillator.energy() == 0.0,
          "unprepared resonator remains silent");
    logic_error([&] { oscillator.set_state(0.0, 1.0); }, "unprepared state rejected");
    oscillator.prepare(0.01, 340.0, 2.0, 48000.0);
    oscillator.apply_impulse(0.002);
    close(oscillator.velocity(), 0.2L, 1.0e-15L, 1.0e-15L, "impulse momentum jump I/m");
    close(oscillator.energy(), 0.002L * 0.002L / (2.0L * 0.01L), 1.0e-18L, 1.0e-14L,
          "impulse energy I^2/(2m)");
    double previous_energy = oscillator.energy();
    for (std::size_t index = 0; index < 4800; ++index) {
        oscillator.advance();
        const double energy = oscillator.energy();
        check(energy <= previous_energy + 1.0e-17, "passive single-resonator step");
        previous_energy = energy;
    }
    const long double omega = 2.0L * pi * 340.0L;
    const long double wd = std::sqrt(omega * omega - 4.0L);
    const long double time = 0.1L;
    const long double expected_q = 0.2L * std::exp(-2.0L * time) * std::sin(wd * time) / wd;
    const long double expected_v = 0.2L * std::exp(-2.0L * time) *
                                   (std::cos(wd * time) - 2.0L * std::sin(wd * time) / wd);
    close(oscillator.displacement(), expected_q, 1.0e-15L, 1.0e-10L, "impulse analytic displacement");
    close(oscillator.velocity(), expected_v, 1.0e-12L, 1.0e-10L, "impulse analytic velocity");
    const double before = oscillator.displacement();
    invalid([&] { oscillator.set_state(std::numeric_limits<double>::max(), 0.0); },
            "overflowing initial energy rejected");
    check(oscillator.displacement() == before, "invalid state does not mutate resonator");
    invalid([&] { oscillator.apply_impulse(std::numeric_limits<double>::infinity()); },
            "infinite impulse rejected");
    invalid([&] { oscillator.prepare(0.0, 340.0, 2.0, 48000.0); }, "zero mass rejected");
    invalid([&] { oscillator.prepare(0.01, 24000.0, 2.0, 48000.0); }, "Nyquist frequency rejected");
    invalid([&] { oscillator.prepare(0.01, 340.0, -1.0, 48000.0); }, "negative decay rejected");
    const double critical = 2.0 * std::numbers::pi_v<double> * 1000.0;
    invalid([&] { oscillator.prepare(0.01, 1000.0, critical, 48000.0); },
            "critical damping outside supported domain rejected");
    oscillator.prepare(0.01, 1000.0, std::nextafter(critical, 0.0), 48000.0);
    oscillator.set_state(0.001, 0.0);
    for (std::size_t index = 0; index < 100; ++index) {
        oscillator.advance();
    }
    const long double critical_time = 100.0L / 48000.0L;
    const long double critical_omega = 2.0L * pi * 1000.0L;
    const long double critical_limit =
        0.001L * std::exp(-critical_omega * critical_time) * (1.0L + critical_omega * critical_time);
    close(oscillator.displacement(), critical_limit, 1.0e-16L, 1.0e-8L,
          "near-critical finite limit without division instability");
    oscillator.reset();
    check(oscillator.energy() == 0.0 && oscillator.velocity() == 0.0, "resonator reset");
}

void endpoints() {
    for (double endpoint : {0.0, 1.0}) {
        ModalString model;
        model.prepare(StringParameters{}, 48000.0, endpoint);
        model.pluck(0.0005, 0.23);
        check(model.energy() > 0.0, "endpoint silence does not remove physical energy");
        for (std::size_t index = 0; index < 1000; ++index) {
            check(model.tick() == 0.0, "simply-supported endpoint displacement exactly zero");
        }
    }
    ModalString model;
    invalid([&] { model.prepare(StringParameters{}, 48000.0, -0.01); }, "negative pickup rejected");
    invalid([&] { model.prepare(StringParameters{}, 48000.0, 1.01); }, "pickup beyond string rejected");
    model.prepare(StringParameters{}, 48000.0);
    model.pluck(0.0, 0.23);
    for (std::size_t index = 0; index < 1000; ++index) {
        check(model.tick() == 0.0 && model.energy() == 0.0, "zero-height pluck silent");
    }
    ModalString positive;
    ModalString negative;
    positive.prepare(StringParameters{}, 48000.0);
    negative.prepare(StringParameters{}, 48000.0);
    positive.pluck(0.0005, 0.23);
    negative.pluck(-0.0005, 0.23);
    for (std::size_t index = 0; index < 1000; ++index) {
        check(positive.tick() == -negative.tick(), "linear sign-reversal symmetry");
    }
}
} // namespace

int main(int argc, char** argv) {
    const std::array cases = {
        std::pair<std::string_view, void (*)()>{"parameters", parameters},
        std::pair<std::string_view, void (*)()>{"frequencies", frequencies},
        std::pair<std::string_view, void (*)()>{"analytic_response", analytic_response},
        std::pair<std::string_view, void (*)()>{"lossless_energy", lossless_energy},
        std::pair<std::string_view, void (*)()>{"passivity", passivity},
        std::pair<std::string_view, void (*)()>{"pluck_projection", pluck_projection},
        std::pair<std::string_view, void (*)()>{"convergence", convergence},
        std::pair<std::string_view, void (*)()>{"reset", reset},
        std::pair<std::string_view, void (*)()>{"block_partition", block_partition},
        std::pair<std::string_view, void (*)()>{"resonator", resonator},
        std::pair<std::string_view, void (*)()>{"endpoints", endpoints}};
    if (argc != 2) {
        std::cerr << "Expected one named test case.\n";
        return 2;
    }
    const std::string_view selected(argv[1]);
    for (const auto& [name, function] : cases) {
        if (name == selected) {
            try {
                function();
                std::cout << "PASS " << name << '\n';
                return 0;
            } catch (const std::exception& error) {
                std::cerr << "FAIL " << name << ": " << error.what() << '\n';
                return 1;
            }
        }
    }
    std::cerr << "Unknown test case: " << selected << '\n';
    return 2;
}
