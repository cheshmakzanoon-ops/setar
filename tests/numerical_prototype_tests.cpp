#include "setar/numerical_prototype.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <exception>
#include <iostream>
#include <limits>
#include <numbers>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace {
using setar::experimental::MidpointStiffString;
using setar::experimental::MidpointTwoMassPort;
using setar::experimental::LeapfrogStiffString;
using setar::experimental::StringGrid;
using setar::experimental::TwoMassParameters;
using setar::experimental::TwoMassState;
constexpr double pi = std::numbers::pi_v<double>;

void check(bool condition, std::string_view message) {
    if (!condition) {
        throw std::runtime_error(std::string(message));
    }
}

void near(double actual, double expected, double atol, double rtol,
          std::string_view message) {
    const double difference = std::abs(actual - expected);
    const double limit = atol + rtol * std::abs(expected);
    if (!std::isfinite(actual) || !std::isfinite(expected) || difference > limit) {
        throw std::runtime_error(std::string(message) + " actual=" + std::to_string(actual) +
                                 " expected=" + std::to_string(expected) +
                                 " tolerance=" + std::to_string(limit));
    }
}

template <typename F> void rejected(F&& f, std::string_view message) {
    bool caught = false;
    try {
        f();
    } catch (const std::invalid_argument&) {
        caught = true;
    }
    check(caught, message);
}

void string_spatial_convergence() {
    for (std::size_t mode : {std::size_t{1}, std::size_t{3}}) {
        double previous_error = std::numeric_limits<double>::infinity();
        for (std::size_t nodes : {std::size_t{8}, std::size_t{16}, std::size_t{32},
                                  std::size_t{64}, std::size_t{128}}) {
            StringGrid g;
            g.interior_nodes = nodes;
            const double exact = setar::experimental::continuum_frequency_hz(g, mode);
            const double discrete = setar::experimental::spatial_grid_frequency_hz(g, mode);
            const double error = std::abs(discrete / exact - 1.0);
            check(error < previous_error, "grid eigenfrequency converges monotonically");
            if (nodes > 8) {
                check(error < 0.36 * previous_error,
                      "central spatial difference approaches second-order convergence");
            }
            previous_error = error;
            std::cout << "spatial nodes=" << nodes << " mode=" << mode
                      << " relative_frequency_error=" << error << '\n';
        }
    }
}

void force_interpolation_adjoint() {
    // For nodal force density (F/h)*w, external semi-discrete power is
    // h*sum(v_i*(F/h)*w_i) = F*sum(w_i*v_i) by direct transpose.
    StringGrid g;
    g.interior_nodes = 16;
    g.force_position_fraction = 0.375;
    MidpointStiffString string;
    string.prepare(g);
    std::vector<double> q(g.interior_nodes, 0.0);
    std::vector<double> v(g.interior_nodes, 0.0);
    for (std::size_t i = 0; i < v.size(); ++i) {
        v[i] = 0.005 * static_cast<double>(i + 1);
    }
    string.set_state(q, v);
    const double grid_position = g.force_position_fraction * static_cast<double>(v.size() + 1);
    double weighted_velocity = 0.0;
    for (std::size_t i = 0; i < v.size(); ++i) {
        const double w = std::max(0.0, 1.0 - std::abs(static_cast<double>(i + 1) - grid_position));
        weighted_velocity += w * v[i];
    }
    near(string.port_velocity_m_per_s(), weighted_velocity, 1.0e-15, 1.0e-14,
         "force port must use transpose of the input interpolation weights");
    const double before = string.energy_j();
    const auto step = string.advance(0.3);
    near(step.stored_energy_change_j, step.external_work_j, 1.0e-12, 1.0e-9,
         "midpoint force work must be accounted for in string energy");
    near(string.energy_j() - before, step.stored_energy_change_j, 1.0e-16, 1.0e-12,
         "reported energy delta and observed state agree");

    for (double endpoint : {0.0, 1.0}) {
        g.force_position_fraction = endpoint;
        string.prepare(g);
        string.set_state(q, v);
        near(string.port_velocity_m_per_s(), 0.0, 1.0e-15, 0.0,
             "fixed endpoint has zero port velocity");
        const auto boundary = string.advance(50.0);
        near(boundary.external_work_j, 0.0, 1.0e-16, 0.0,
             "force applied to fixed endpoint produces no work");
    }
}

void stability_envelope() {
    // SPD K and A = I + dt^2*K/(4*mu) imply unimodular eigenvalues for the
    // midpoint update. This test verifies the declared *linear* domain only.
    for (double sample_rate : {8000.0, 48000.0, 192000.0}) {
        StringGrid g;
        g.sample_rate_hz = sample_rate;
        g.interior_nodes = 32;
        MidpointStiffString string;
        string.prepare(g);
        constexpr std::size_t mode = 23;
        string.set_sine_state(0.0001, mode);
        const double first_energy = string.energy_j();
        check(first_energy > 0.0, "initialized high mode must have nonzero energy");
        for (std::size_t i = 0; i < 1200; ++i) {
            const auto step = string.advance(0.0);
            near(step.stored_energy_change_j, 0.0, 1.0e-11 * first_energy, 0.0,
                 "midpoint has no single-step energy source");
        }
        near(string.energy_j(), first_energy, 5.0e-8 * first_energy, 0.0,
             "high spatial mode remains energy stable at each test rate");
        const double spatial_frequency = setar::experimental::spatial_grid_frequency_hz(g, mode);
        const double effective_frequency =
            setar::experimental::midpoint_frequency_hz(spatial_frequency, sample_rate);
        near(string.displacement_m()[10],
             0.0001 * std::sin(static_cast<double>(mode) * pi * 11.0 / 33.0) *
                 std::cos(2.0 * pi * effective_frequency * 1200.0 / sample_rate),
             1.0e-9, 1.0e-6, "high-mode midpoint time evolution follows its independent phase oracle");
    }
}

void discrete_energy_ledger() {
    MidpointStiffString string;
    StringGrid g;
    g.interior_nodes = 24;
    string.prepare(g);
    string.set_sine_state(0.0002, 2);
    const double initial_energy = string.energy_j();
    double integrated_work = 0.0;
    double largest_work_residual = 0.0;
    for (std::size_t i = 0; i < 600; ++i) {
        const double midpoint_force = 0.2 * std::sin(0.031 * static_cast<double>(i)) + 0.03;
        const auto step = string.advance(midpoint_force);
        integrated_work += step.external_work_j;
        largest_work_residual = std::max(largest_work_residual, std::abs(step.work_residual_j));
        near(step.stored_energy_change_j, step.external_work_j, 1.0e-10, 3.0e-7,
             "forced midpoint energy and work close each step");
    }
    near(string.energy_j() - initial_energy, integrated_work, 1.0e-9, 3.0e-6,
         "cumulative force work closes energy ledger");
    std::cout << "maximum_single_step_work_residual_j=" << largest_work_residual << '\n';
    string.reset();
    near(string.energy_j(), 0.0, 1.0e-18, 0.0, "reset is exactly zero energy");
}

void dispersion_grid_refinement() {
    for (double stiffness : {0.0, 0.000015, 0.00008}) {
        double prior_error = std::numeric_limits<double>::infinity();
        for (std::size_t n : {std::size_t{8}, std::size_t{16}, std::size_t{32},
                              std::size_t{64}}) {
            StringGrid g;
            g.bending_stiffness_n_m2 = stiffness;
            g.interior_nodes = n;
            const double continuous = setar::experimental::continuum_frequency_hz(g, 3);
            const double discrete = setar::experimental::spatial_grid_frequency_hz(g, 3);
            const double error = std::abs(continuous - discrete);
            check(error < prior_error, "stiff-string grid dispersion improves with refinement");
            prior_error = error;
        }
    }
    StringGrid g;
    const double fd = setar::experimental::spatial_grid_frequency_hz(g, 5);
    const double slow = setar::experimental::midpoint_frequency_hz(fd, 8000.0);
    const double fast = setar::experimental::midpoint_frequency_hz(fd, 192000.0);
    check(slow < fast && fast < fd, "midpoint adds separately quantifiable temporal dispersion");
    rejected([&] { (void)setar::experimental::continuum_frequency_hz(g, 0); },
             "zero harmonic must be rejected");
    rejected([&] { (void)setar::experimental::spatial_grid_frequency_hz(g, 1000); },
             "unrepresented harmonic must be rejected");
}

void explicit_cfl_rejection() {
    StringGrid g;
    g.interior_nodes = 64;
    g.sample_rate_hz = 8000.0;
    const double minimum = setar::experimental::leapfrog_minimum_stable_rate_hz(g);
    check(minimum > g.sample_rate_hz, "stiff-string CFL must reject coarse audio rate");
    LeapfrogStiffString explicit_grid;
    rejected([&] { explicit_grid.prepare(g); },
             "unstable explicit stiffness grid must be rejected at preparation");
    MidpointStiffString implicit_grid;
    implicit_grid.prepare(g);
    check(implicit_grid.prepared(), "midpoint admits this fixed SPD grid despite CFL ratio");
    g.sample_rate_hz = 48000.0;
    explicit_grid.prepare(g);
    check(explicit_grid.prepared(), "sufficient sample rate admits explicit spatial operator");
    constexpr std::size_t mode = 47;
    constexpr double amplitude = 0.0001;
    explicit_grid.set_sine_state(amplitude, mode);
    const double initial_energy = explicit_grid.modified_energy_j();
    const double f = setar::experimental::spatial_grid_frequency_hz(g, mode);
    const double f_numerical = setar::experimental::leapfrog_frequency_hz(f, g.sample_rate_hz);
    check(f_numerical > f, "explicit leapfrog produces upward temporal frequency warping");
    for (std::size_t i = 0; i < 1800; ++i) {
        (void)explicit_grid.advance(0.0);
    }
    near(explicit_grid.modified_energy_j(), initial_energy, 1.0e-10 * initial_energy, 0.0,
         "explicit stable-domain staggered energy remains conserved");
    near(explicit_grid.displacement_m()[10],
         amplitude * std::sin(static_cast<double>(mode) * pi * 11.0 / 65.0) *
             std::cos(2.0 * pi * f_numerical * 1800.0 / g.sample_rate_hz),
         1.0e-9, 2.0e-6, "explicit mode phase follows independent three-level recurrence");
    rejected([&] { (void)setar::experimental::leapfrog_frequency_hz(f, 8000.0); },
             "unstable leapfrog phase is not a real frequency");
}

void explicit_work_ledger() {
    StringGrid g;
    g.interior_nodes = 24;
    g.sample_rate_hz = 96000.0;
    LeapfrogStiffString string;
    string.prepare(g);
    string.set_sine_state(0.0002, 3);
    const double first_energy = string.modified_energy_j();
    double total_work = 0.0;
    for (std::size_t i = 0; i < 400; ++i) {
        const double force = 0.05 * std::sin(0.017 * static_cast<double>(i));
        const auto step = string.advance(force);
        near(step.modified_energy_change_j, step.external_work_j, 1.0e-12, 5.0e-8,
             "three-level modified energy equals integer-time force work");
        total_work += step.external_work_j;
    }
    near(string.modified_energy_j() - first_energy, total_work, 1.0e-10, 1.0e-7,
         "explicit forcing conserves total energy plus performed work");
    string.reset();
    near(string.modified_energy_j(), 0.0, 0.0, 0.0, "explicit reset leaves exact silence");
}

void explicit_implicit_comparison() {
    StringGrid g;
    g.interior_nodes = 16;
    g.sample_rate_hz = 96000.0;
    constexpr std::size_t mode = 4;
    constexpr std::size_t steps = 700;
    const double f = setar::experimental::spatial_grid_frequency_hz(g, mode);
    const double explicit_f = setar::experimental::leapfrog_frequency_hz(f, g.sample_rate_hz);
    const double implicit_f = setar::experimental::midpoint_frequency_hz(f, g.sample_rate_hz);
    check(implicit_f < f && f < explicit_f,
          "the two time integrators warp a common spatial mode in opposite directions");
    LeapfrogStiffString explicit_grid;
    explicit_grid.prepare(g);
    explicit_grid.set_sine_state(0.0001, mode);
    MidpointStiffString implicit_grid;
    implicit_grid.prepare(g);
    implicit_grid.set_sine_state(0.0001, mode);
    for (std::size_t step = 0; step < steps; ++step) {
        (void)explicit_grid.advance(0.0);
        (void)implicit_grid.advance(0.0);
    }
    const double position_sine = std::sin(static_cast<double>(mode) * pi * 7.0 / 17.0);
    const double time_s = static_cast<double>(steps) / g.sample_rate_hz;
    near(explicit_grid.displacement_m()[6],
         0.0001 * position_sine * std::cos(2.0 * pi * explicit_f * time_s),
         1.0e-10, 2.0e-6, "explicit path matches closed-form discrete frequency");
    near(implicit_grid.displacement_m()[6],
         0.0001 * position_sine * std::cos(2.0 * pi * implicit_f * time_s),
         1.0e-10, 2.0e-6, "implicit path matches independent rational discrete frequency");
    const double high_rate = 192000.0;
    check(std::abs(setar::experimental::leapfrog_frequency_hz(f, high_rate) - f) <
              std::abs(explicit_f - f) &&
              std::abs(setar::experimental::midpoint_frequency_hz(f, high_rate) - f) <
                  std::abs(implicit_f - f),
          "raising integration rate reduces both temporal phase errors");
}

void two_mass_oracle() {
    MidpointTwoMassPort port;
    TwoMassParameters p;
    port.prepare(p);
    const TwoMassState initial{0.001, -0.0002, 0.02, -0.03};
    port.set_state(initial);
    const double first_energy = port.energy_j();
    const double initial_momentum = port.linear_momentum_kg_m_per_s();
    const double reduced_mass = p.mass_1_kg * p.mass_2_kg / (p.mass_1_kg + p.mass_2_kg);
    const double omega = std::sqrt(p.coupling_stiffness_n_per_m / reduced_mass);
    const double observed_omega = 2.0 * p.sample_rate_hz *
                                  std::atan(omega / (2.0 * p.sample_rate_hz));
    constexpr std::size_t steps = 800;
    for (std::size_t i = 0; i < steps; ++i) {
        (void)port.advance();
        near(port.energy_j(), first_energy, 1.0e-11, 3.0e-7,
             "lossless two-mass midpoint total energy conserved");
        near(port.linear_momentum_kg_m_per_s(), initial_momentum, 1.0e-12, 1.0e-8,
             "symmetric internal spring forces conserve total momentum");
    }
    const double t = static_cast<double>(steps) / p.sample_rate_hz;
    const double expected_extension = (initial.x1_m - initial.x2_m) *
                                      std::cos(observed_omega * t) +
                                      (initial.v1_m_per_s - initial.v2_m_per_s) *
                                          std::sin(observed_omega * t) / observed_omega;
    near(port.state().x1_m - port.state().x2_m, expected_extension,
         2.0e-9, 2.0e-6, "relative coordinate matches independent harmonic oracle");

    p.coupling_stiffness_n_per_m = 0.0;
    port.prepare(p);
    port.set_state(initial);
    const auto uncoupled_step = port.advance();
    near(uncoupled_step.force_on_1_mid_n, 0.0, 1.0e-16, 0.0,
         "zero stiffness is uncoupled limit");
    near(port.state().x1_m, initial.x1_m + initial.v1_m_per_s / p.sample_rate_hz,
         1.0e-15, 1.0e-12, "free mass advances linearly");
}

void port_power_balance() {
    MidpointTwoMassPort port;
    port.prepare(TwoMassParameters{});
    port.set_state({0.001, -0.0003, 0.03, -0.02});
    const double initial_energy = port.energy_j();
    double integrated_balance = 0.0;
    for (std::size_t i = 0; i < 1400; ++i) {
        const auto step = port.advance();
        near(step.force_on_1_mid_n + step.force_on_2_mid_n, 0.0, 0.0, 0.0,
             "interface forces are equal and opposite at the same time level");
        near(step.port_work_sum_j + step.spring_energy_change_j, 0.0,
             1.0e-12, 1.0e-8, "pair port work is negative spring stored-energy increment");
        near(step.total_energy_change_j, 0.0, 1.0e-12, 0.0,
             "no hidden energy source in co-located coupling fixture");
        integrated_balance += step.port_work_sum_j + step.spring_energy_change_j;
    }
    near(integrated_balance, 0.0, 1.0e-11, 0.0,
         "summed interface work residual remains bounded");
    near(port.energy_j(), initial_energy, 1.0e-11, 1.0e-6,
         "interface energy preserved across repeated steps");
}

void invalid_parameters() {
    MidpointStiffString string;
    check(!string.prepared(), "new string is unprepared");
    StringGrid good;
    string.prepare(good);
    string.set_sine_state(0.0001, 1);
    const double old_energy = string.energy_j();
    const double nan = std::numeric_limits<double>::quiet_NaN();
    for (double bad : {0.0, -1.0, nan, std::numeric_limits<double>::infinity()}) {
        StringGrid g = good;
        g.linear_density_kg_per_m = bad;
        rejected([&] { string.prepare(g); }, "bad mass density rejected");
    }
    near(string.energy_j(), old_energy, 0.0, 0.0,
         "failed prepare leaves old grid and state untouched");
    rejected([&] { (void)string.advance(nan); }, "nonfinite force rejected");
    near(string.energy_j(), old_energy, 0.0, 0.0,
         "failed advance leaves old grid and state untouched");
    rejected([&] { string.set_sine_state(nan, 1); }, "invalid amplitude rejected");
    rejected([&] { string.set_sine_state(0.001, 0); }, "invalid harmonic rejected");
    MidpointTwoMassPort port;
    port.prepare(TwoMassParameters{});
    const double prior_energy = port.energy_j();
    rejected([&] { port.set_state({0.0, 0.0, nan, 0.0}); }, "invalid velocity rejected");
    near(port.energy_j(), prior_energy, 0.0, 0.0, "invalid mass state is transactional");
}

} // namespace

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "usage: setar_numerical_tests <case-name>\n";
        return 2;
    }
    const std::string_view requested(argv[1]);
    struct Case { std::string_view name; void (*run)(); };
    constexpr std::array<Case, 11> tests{{
        {"string_spatial_convergence", string_spatial_convergence},
        {"force_interpolation_adjoint", force_interpolation_adjoint},
        {"stability_envelope", stability_envelope},
        {"discrete_energy_ledger", discrete_energy_ledger},
        {"dispersion_grid_refinement", dispersion_grid_refinement},
        {"explicit_cfl_rejection", explicit_cfl_rejection},
        {"explicit_work_ledger", explicit_work_ledger},
        {"explicit_implicit_comparison", explicit_implicit_comparison},
        {"two_mass_oracle", two_mass_oracle},
        {"port_power_balance", port_power_balance},
        {"invalid_parameters", invalid_parameters},
    }};
    try {
        for (const auto& test : tests) {
            if (test.name == requested) {
                test.run();
                std::cout << "PASS " << requested << '\n';
                return 0;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "FAIL " << requested << ": " << e.what() << '\n';
        return 1;
    }
    std::cerr << "unknown numerical test: " << requested << '\n';
    return 2;
}
