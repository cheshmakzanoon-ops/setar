#include "setar/numerical_prototype.hpp"
#include "setar/reference.hpp"
#include "support/modal_crosscheck.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <iostream>
#include <limits>
#include <numbers>
#include <random>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {
using setar::experimental::MidpointStiffString;
using setar::experimental::MidpointTwoMassPort;
using setar::experimental::LeapfrogStiffString;
using setar::experimental::StringGrid;
using setar::experimental::StringLoss;
using setar::experimental::TwoMassParameters;
using setar::experimental::TwoMassState;
constexpr double pi = std::numbers::pi_v<double>;
constexpr double machine_epsilon = std::numeric_limits<double>::epsilon();

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

// Platform-independent pseudo-random numbers in [-1, 1): the mt19937_64 sequence is
// fixed by the C++ standard, unlike std::uniform_real_distribution.
class DeterministicUniform {
  public:
    explicit DeterministicUniform(std::uint64_t seed) : engine_(seed) {}
    double next() {
        return static_cast<double>(engine_() >> 11) * (2.0 / 9007199254740992.0) - 1.0;
    }

  private:
    std::mt19937_64 engine_;
};

// Unit 2-norm eigenvector sqrt(2/(N+1))*sin(m*pi*(i+1)/(N+1)) of the uniform fixed-end
// second difference. Built here, not in the library, so projections are independent.
std::vector<double> sine_mode(std::size_t nodes, std::size_t mode) {
    std::vector<double> shape(nodes);
    for (std::size_t i = 0; i < nodes; ++i) {
        shape[i] = std::sqrt(2.0 / static_cast<double>(nodes + 1)) *
                   std::sin(static_cast<double>(mode) * pi * static_cast<double>(i + 1) /
                            static_cast<double>(nodes + 1));
    }
    return shape;
}

double project(std::span<const double> state, std::span<const double> shape) {
    double total = 0.0;
    for (std::size_t i = 0; i < state.size(); ++i) {
        total += shape[i] * state[i];
    }
    return total;
}

// Sets q = sum_m amplitude_m * (unit sine mode m), v = 0.
void set_modal_state(MidpointStiffString& string, std::size_t nodes,
                     const std::vector<std::pair<std::size_t, double>>& amplitudes) {
    std::vector<double> q(nodes, 0.0);
    const std::vector<double> v(nodes, 0.0);
    for (const auto& [mode, amplitude] : amplitudes) {
        const auto shape = sine_mode(nodes, mode);
        for (std::size_t i = 0; i < nodes; ++i) {
            q[i] += amplitude * shape[i];
        }
    }
    string.set_state(q, v);
}

// Unforced modal coordinate x_k = <q(k*dt), mode> for k = 0..samples-1.
std::vector<double> modal_series(MidpointStiffString& string, std::size_t mode,
                                 std::size_t samples) {
    const auto shape = sine_mode(string.displacement_m().size(), mode);
    std::vector<double> series;
    series.reserve(samples);
    for (std::size_t k = 0; k < samples; ++k) {
        series.push_back(project(string.displacement_m(), shape));
        if (k + 1 < samples) {
            (void)string.advance(0.0);
        }
    }
    return series;
}

// Window long enough to resolve the decay but short enough that the mode has not
// vanished into the roundoff of the other modes (about e^-8 of its start).
std::size_t decay_window(double sample_rate_hz, double expected_decay_per_s,
                         std::size_t maximum) {
    const double wanted = std::ceil(8.0 * sample_rate_hz / expected_decay_per_s);
    return static_cast<std::size_t>(std::clamp(wanted, 64.0, static_cast<double>(maximum)));
}

void loss_law_passivity() {
    struct Law {
        std::string_view name;
        StringLoss loss;
    };
    const std::array<Law, 4> laws{{{"constant", {3.0, 0.0, 0.0}},
                                   {"wavenumber_squared", {0.0, 0.03, 0.0}},
                                   {"frequency_squared", {0.0, 0.0, 5.0e-7}},
                                   {"combined", {3.0, 0.03, 5.0e-7}}}};
    // A rough pseudo-random state excites every grid mode, including the stiffest
    // ones where a wrongly signed or non-positive-semidefinite damping matrix shows first.
    DeterministicUniform random(0x5e7a12U);
    for (const Law& law : laws) {
        StringGrid g;
        g.interior_nodes = 24;
        g.loss = law.loss;
        MidpointStiffString string;
        string.prepare(g);
        std::vector<double> q(g.interior_nodes);
        std::vector<double> v(g.interior_nodes);
        for (std::size_t i = 0; i < q.size(); ++i) {
            q[i] = 1.0e-4 * random.next();
            v[i] = 0.05 * random.next();
        }
        string.set_state(q, v);
        const double first_energy = string.energy_j();
        check(first_energy > 0.0, "rough state must have nonzero energy");
        double previous_energy = first_energy;
        double total_dissipated = 0.0;
        double residual_sum = 0.0;
        double uncorrected_sum = 0.0;
        for (std::size_t i = 0; i < 2000; ++i) {
            const auto step = string.advance(0.0);
            check(step.dissipated_energy_j >= -1.0e-14 * first_energy,
                  "each loss law dissipates nonnegative energy per step");
            check(step.stored_energy_change_j <= 1.0e-14 * first_energy,
                  "unforced lossy string never gains stored energy in one step");
            check(string.energy_j() <= previous_energy + 1.0e-14 * first_energy,
                  "stored energy is nonincreasing");
            near(step.work_residual_j, 0.0, 1.0e-12 * first_energy, 0.0,
                 "ledger stored change + dissipation closes every step");
            previous_energy = string.energy_j();
            total_dissipated += step.dissipated_energy_j;
            residual_sum += std::abs(step.work_residual_j);
            uncorrected_sum += std::abs(step.stored_energy_change_j - step.external_work_j);
        }
        near(string.energy_j() - first_energy, -total_dissipated, 1.0e-11 * first_energy, 0.0,
             "accumulated dissipation equals the stored-energy loss");
        check(total_dissipated > 1.0e-3 * first_energy, "the declared law actually dissipates");
        // The ledger must have teeth: leaving the loss term out of it leaves a
        // residual many orders of magnitude above roundoff.
        check(uncorrected_sum > 1.0e4 * residual_sum,
              "a ledger that omits dissipation is rejected by the residual");
        std::cout << "loss law=" << law.name << " dissipated_fraction="
                  << total_dissipated / first_energy
                  << " summed_ledger_residual_j=" << residual_sum << '\n';
    }

    {
        // Driven, lossy run: E_end - E_0 = (integrated work) - (integrated dissipation).
        StringGrid g;
        g.interior_nodes = 24;
        g.loss = laws[3].loss;
        MidpointStiffString string;
        string.prepare(g);
        string.set_sine_state(0.0002, 2);
        const double first_energy = string.energy_j();
        double peak_energy = first_energy;
        double work_sum = 0.0;
        double dissipated_sum = 0.0;
        double worst_residual = 0.0;
        for (std::size_t i = 0; i < 600; ++i) {
            const double force = 0.2 * std::sin(0.031 * static_cast<double>(i)) + 0.03;
            const auto step = string.advance(force);
            peak_energy = std::max(peak_energy, string.energy_j());
            work_sum += step.external_work_j;
            dissipated_sum += step.dissipated_energy_j;
            worst_residual = std::max(worst_residual, std::abs(step.work_residual_j));
            check(step.dissipated_energy_j >= -1.0e-14 * peak_energy,
                  "driven step still dissipates nonnegative energy");
        }
        near(worst_residual, 0.0, 1.0e-12 * peak_energy, 0.0,
             "driven lossy ledger is exact to roundoff at every step");
        near(string.energy_j() - first_energy, work_sum - dissipated_sum, 1.0e-12 * peak_energy,
             0.0, "driven ledger: stored change = work - dissipation");
        check(dissipated_sum > 1.0e-3 * std::abs(work_sum), "driven run dissipates measurably");
        std::cout << "driven_lossy_worst_residual_j=" << worst_residual
                  << " peak_energy_j=" << peak_energy << '\n';
    }

    {
        // Zero-loss limit, level 1: no declared loss means dissipation is exactly zero.
        StringGrid g;
        g.interior_nodes = 24;
        MidpointStiffString string;
        string.prepare(g);
        string.set_sine_state(0.0002, 2);
        for (std::size_t i = 0; i < 300; ++i) {
            const auto step = string.advance(0.2 * std::sin(0.031 * static_cast<double>(i)));
            check(step.dissipated_energy_j == 0.0, "zero loss dissipates exactly zero");
        }
    }

    {
        // Zero-loss limit, level 2 (oracle): the decay rate tends to the declared
        // constant linearly, with the midpoint rule's own -(omega*dt/2)^2 relative error,
        // and equals exactly the lossless closed-form frequency at zero loss.
        StringGrid g;
        g.interior_nodes = 24;
        const double spatial = setar::experimental::spatial_grid_frequency_hz(g, 3);
        const double half_phase = pi * spatial / g.sample_rate_hz; // omega*dt/2
        for (double sigma : {1.0e-1, 1.0e-2, 1.0e-3, 1.0e-6}) {
            g.loss = {sigma, 0.0, 0.0};
            const auto roots = setar::experimental::midpoint_damped_roots(g, 3);
            check(roots.oscillatory, "lightly damped mode is oscillatory");
            check(std::abs(roots.decay_rate_per_s / sigma - 1.0) <= 1.1 * half_phase * half_phase,
                  "midpoint decay tends to the declared constant at the (omega*dt/2)^2 rate");
        }
        g.loss = {};
        const auto lossless = setar::experimental::midpoint_damped_roots(g, 3);
        check(lossless.decay_rate_per_s == 0.0 && lossless.spectral_radius == 1.0,
              "lossless midpoint mode has unit spectral radius");
        near(lossless.frequency_hz, setar::experimental::midpoint_frequency_hz(spatial, g.sample_rate_hz),
             0.0, 1.0e-12, "zero-loss limit reproduces the lossless closed-form frequency");

        // Level 3 (simulation): the measured decay of a simulated mode follows the oracle
        // as the loss is reduced; at zero loss the measured decay vanishes to roundoff.
        for (double sigma : {1.0e-1, 1.0e-2, 1.0e-3, 0.0}) {
            g.loss = {sigma, 0.0, 0.0};
            MidpointStiffString string;
            string.prepare(g);
            set_modal_state(string, g.interior_nodes, {{3, 1.0e-4}});
            const auto series = modal_series(string, 3, 3000);
            const auto measured = setar::experimental::estimate_decay(series, g.sample_rate_hz);
            const auto oracle = setar::experimental::midpoint_damped_roots(g, 3);
            near(measured.frequency_hz, oracle.frequency_hz, 0.0, 1.0e-9,
                 "simulated frequency matches the amplification oracle as loss vanishes");
            if (sigma > 0.0) {
                near(measured.decay_rate_per_s, oracle.decay_rate_per_s, 0.0, 1.0e-5,
                     "simulated decay matches the amplification oracle at small loss");
            } else {
                near(measured.decay_rate_per_s, 0.0, 1.0e-8, 0.0,
                     "measured decay at zero loss is zero to roundoff (absolute, 1/s)");
            }
        }
    }
}

void loss_decay_estimator() {
    // The estimator is validated on signals whose roots are known in closed form.
    struct Case {
        double rate;
        double frequency;
        double decay;
        std::size_t samples;
    };
    for (const Case c : {Case{8000.0, 440.0, 20.0, 400}, Case{48000.0, 1000.0, 5.0, 4000},
                         Case{96000.0, 170.0, 1.5, 3000}, Case{192000.0, 170.0, 1.5, 3400}}) {
        const double omega = 2.0 * pi * c.frequency;
        std::vector<double> x(c.samples);
        for (std::size_t k = 0; k < x.size(); ++k) {
            const double t = static_cast<double>(k) / c.rate;
            x[k] = 3.0e-4 * std::exp(-c.decay * t) * std::cos(omega * t + 0.7);
        }
        const auto est = setar::experimental::estimate_decay(x, c.rate);
        check(est.oscillatory, "damped cosine has a complex root pair");
        // Roundoff-limited accuracy is documented as eps*cond/(2*gamma*dt) for decay and
        // eps*cond^2 for frequency; allow a factor of 200 over those scales.
        const double decay_bound = std::max(
            1.0e-12, 200.0 * machine_epsilon * est.regressor_condition * c.rate / (2.0 * c.decay));
        const double frequency_bound = std::max(
            1.0e-12, 200.0 * machine_epsilon * est.regressor_condition * est.regressor_condition);
        // The roundoff envelope above is deliberately pessimistic; the regression gate is
        // the tighter of it and 1e-8 relative.
        near(est.decay_rate_per_s, c.decay, 0.0, std::min(decay_bound, 1.0e-8),
             "estimator recovers a known decay");
        near(est.frequency_hz, c.frequency, 0.0, frequency_bound,
             "estimator recovers a known frequency");
        check(est.residual_rms_over_signal_rms < 1.0e-10, "one damped pair explains its own signal");
        std::cout << "estimator rate=" << c.rate << " condition=" << est.regressor_condition
                  << " decay_rel_error=" << std::abs(est.decay_rate_per_s / c.decay - 1.0)
                  << " bound=" << decay_bound << '\n';
    }
    {
        // Overdamped: two real positive roots; no frequency may be fabricated.
        std::vector<double> x(200);
        for (std::size_t k = 0; k < x.size(); ++k) {
            x[k] = 2.0 * std::pow(0.9, static_cast<double>(k)) +
                   1.0 * std::pow(0.5, static_cast<double>(k));
        }
        const auto est = setar::experimental::estimate_decay(x, 1000.0);
        check(!est.oscillatory && est.frequency_hz == 0.0,
              "non-oscillatory data reports no frequency");
        near(est.decay_rate_per_s, -std::log(0.9) * 1000.0, 0.0, 1.0e-8,
             "slowest real root sets the reported decay");
    }
    {
        // Growth must be reported as negative decay, not hidden or clipped.
        std::vector<double> x(400);
        for (std::size_t k = 0; k < x.size(); ++k) {
            const double t = static_cast<double>(k) / 8000.0;
            x[k] = 1.0e-3 * std::exp(5.0 * t) * std::cos(2.0 * pi * 300.0 * t);
        }
        const auto est = setar::experimental::estimate_decay(x, 8000.0);
        near(est.decay_rate_per_s, -5.0, 0.0, 1.0e-8, "growing oscillation has negative decay");
    }
    {
        // Two simultaneous modes violate the one-pair model; the residual must say so.
        std::vector<double> x(400);
        for (std::size_t k = 0; k < x.size(); ++k) {
            const double t = static_cast<double>(k) / 8000.0;
            x[k] = std::cos(2.0 * pi * 440.0 * t) + 0.5 * std::cos(2.0 * pi * 1230.0 * t);
        }
        const auto est = setar::experimental::estimate_decay(x, 8000.0);
        check(est.residual_rms_over_signal_rms > 1.0e-3,
              "a two-mode signal is flagged by the fit residual");
    }
    {
        // Gain invariance, including extreme gains.
        std::vector<double> x(1000);
        for (std::size_t k = 0; k < x.size(); ++k) {
            const double t = static_cast<double>(k) / 48000.0;
            x[k] = std::exp(-12.0 * t) * std::cos(2.0 * pi * 500.0 * t + 0.3);
        }
        const auto reference = setar::experimental::estimate_decay(x, 48000.0);
        for (double gain : {1.0e-12, 3.7e-3, 2.5e9}) {
            std::vector<double> scaled = x;
            for (double& value : scaled) {
                value *= gain;
            }
            const auto est = setar::experimental::estimate_decay(scaled, 48000.0);
            near(est.decay_rate_per_s, reference.decay_rate_per_s, 0.0, 1.0e-9,
                 "decay estimate is independent of gain");
            near(est.frequency_hz, reference.frequency_hz, 0.0, 1.0e-9,
                 "frequency estimate is independent of gain");
        }
    }
    const double nan = std::numeric_limits<double>::quiet_NaN();
    rejected([] { (void)setar::experimental::estimate_decay(std::vector<double>(7, 1.0), 48000.0); },
             "fewer than eight samples rejected");
    rejected([] { (void)setar::experimental::estimate_decay(std::vector<double>(64, 0.0), 48000.0); },
             "identically zero input rejected");
    rejected([] { (void)setar::experimental::estimate_decay(std::vector<double>(64, 1.0), 48000.0); },
             "constant input is rank deficient and rejected");
    std::vector<double> with_nan(64, 1.0);
    with_nan[10] = nan;
    rejected([&] { (void)setar::experimental::estimate_decay(with_nan, 48000.0); },
             "nonfinite input rejected");
    rejected([] { (void)setar::experimental::estimate_decay(std::vector<double>(64, 1.0), 0.0); },
             "nonpositive sample rate rejected");
}

void loss_decay_oracle() {
    // (a) Several modes decay simultaneously inside one simulated string. Each is
    // isolated by projection onto an independently built sine vector, then its decay
    // and frequency are measured from the simulated samples alone and compared with
    // the Cayley-transform amplification oracle. Loss that coupled modes would leave
    // a large single-pair fit residual.
    StringGrid g;
    g.interior_nodes = 31;
    g.sample_rate_hz = 96000.0;
    g.loss = {1.2, 0.01, 2.0e-7};
    {
        MidpointStiffString string;
        string.prepare(g);
        const std::vector<std::size_t> modes = {1, 2, 3, 5, 8, 13, 21};
        std::vector<std::pair<std::size_t, double>> amplitudes;
        for (std::size_t mode : modes) {
            amplitudes.emplace_back(mode, (mode % 2 == 0 ? -1.0 : 1.0) * 1.0e-4 /
                                              static_cast<double>(mode));
        }
        set_modal_state(string, g.interior_nodes, amplitudes);
        const std::size_t samples = 4000;
        std::vector<std::vector<double>> series(modes.size());
        {
            std::vector<std::vector<double>> shapes;
            for (std::size_t mode : modes) {
                shapes.push_back(sine_mode(g.interior_nodes, mode));
            }
            for (std::size_t k = 0; k < samples; ++k) {
                for (std::size_t m = 0; m < modes.size(); ++m) {
                    series[m].push_back(project(string.displacement_m(), shapes[m]));
                }
                if (k + 1 < samples) {
                    (void)string.advance(0.0);
                }
            }
        }
        for (std::size_t m = 0; m < modes.size(); ++m) {
            const auto measured = setar::experimental::estimate_decay(series[m], g.sample_rate_hz);
            const auto oracle = setar::experimental::midpoint_damped_roots(g, modes[m]);
            near(measured.decay_rate_per_s, oracle.decay_rate_per_s, 0.0, 1.0e-8,
                 "measured modal decay equals the amplification-root oracle");
            near(measured.frequency_hz, oracle.frequency_hz, 0.0, 1.0e-9,
                 "measured damped frequency equals the amplification-root oracle");
            check(measured.residual_rms_over_signal_rms < 1.0e-10,
                  "each isolated mode is a single damped pair: no loss-induced mode coupling");
            std::cout << "mode=" << modes[m] << " measured_decay=" << measured.decay_rate_per_s
                      << " oracle_decay=" << oracle.decay_rate_per_s << " continuum_decay="
                      << setar::experimental::continuum_decay_rate_per_s(g, modes[m]) << '\n';
        }
        // The numerical decay differs from the continuum law at high modes: an
        // oracle that merely echoed the declared law would hide this algorithmic loss
        // error, which docs/numerics/08 requires to be reported separately.
        const auto high = setar::experimental::midpoint_damped_roots(g, 21);
        check(std::abs(high.decay_rate_per_s /
                           setar::experimental::continuum_decay_rate_per_s(g, 21) -
                       1.0) > 1.0e-3,
              "oracle exposes the scheme's own decay error rather than echoing the input law");
        check(high.decay_rate_per_s < setar::experimental::continuum_decay_rate_per_s(g, 21),
              "the midpoint rule under-damps (never over-damps) a resolved mode");

        // (b) Gain independence of the extracted decay profile.
        for (double gain : {1.0e-6, 1.0e6}) {
            std::vector<double> scaled = series[4];
            for (double& value : scaled) {
                value *= gain;
            }
            const auto measured = setar::experimental::estimate_decay(scaled, g.sample_rate_hz);
            const auto unit = setar::experimental::estimate_decay(series[4], g.sample_rate_hz);
            near(measured.decay_rate_per_s, unit.decay_rate_per_s, 0.0, 1.0e-9,
                 "decay profile cannot be changed by output gain");
        }
    }

    // (c) A wrong loss law, calibrated to match the fundamental, is still rejected by
    // the higher modes. Bending-dominated synthetic stress fixture (EI is not a setar
    // value) so that the k^2 and omega^2 laws genuinely differ.
    {
        StringGrid stiff;
        stiff.interior_nodes = 31;
        stiff.sample_rate_hz = 96000.0;
        stiff.bending_stiffness_n_m2 = 0.01;
        stiff.loss = {1.0, 0.02, 1.0e-6};
        const std::vector<std::size_t> modes = {1, 4, 8, 16, 24};
        std::vector<double> measured(modes.size());
        for (std::size_t m = 0; m < modes.size(); ++m) {
            MidpointStiffString string;
            string.prepare(stiff);
            set_modal_state(string, stiff.interior_nodes, {{modes[m], 1.0e-4}});
            const std::size_t window = decay_window(
                stiff.sample_rate_hz, setar::experimental::spatial_grid_decay_rate_per_s(stiff, modes[m]), 4000);
            const auto series = modal_series(string, modes[m], window);
            const auto estimate = setar::experimental::estimate_decay(series, stiff.sample_rate_hz);
            const auto oracle = setar::experimental::midpoint_damped_roots(stiff, modes[m]);
            near(estimate.decay_rate_per_s, oracle.decay_rate_per_s, 0.0, 1.0e-8,
                 "stiff fixture: measured decay equals the amplification-root oracle");
            measured[m] = estimate.decay_rate_per_s;
        }
        StringGrid constant_law = stiff;
        constant_law.loss = {1.0, 0.0, 0.0};
        StringGrid wavenumber_law = stiff;
        wavenumber_law.loss = {0.0, 1.0, 0.0};
        StringGrid frequency_law = stiff;
        frequency_law.loss = {0.0, 0.0, 1.0};
        for (const StringGrid* wrong : {&constant_law, &wavenumber_law, &frequency_law}) {
            double worst_mismatch = 0.0;
            for (std::size_t m = 0; m < modes.size(); ++m) {
                const double profile =
                    setar::experimental::spatial_grid_decay_rate_per_s(*wrong, modes[m]) /
                    setar::experimental::spatial_grid_decay_rate_per_s(*wrong, 1);
                const double predicted = measured[0] * profile; // calibrated at the fundamental
                worst_mismatch = std::max(worst_mismatch, std::abs(predicted / measured[m] - 1.0));
            }
            check(worst_mismatch >= 0.25,
                  "a single-term loss law calibrated at mode 1 is rejected by the higher modes");
            std::cout << "wrong_law worst_relative_mismatch=" << worst_mismatch << '\n';
        }

        // Identifiability limit, stated numerically: on a setar-scale bending stiffness
        // the k^2 and omega^2 profiles are collinear to better than 1e-3 over modes 1-8,
        // so measured decay cannot tell them apart; on the stress fixture it can.
        for (double stiffness : {1.5e-5, 0.01}) {
            StringGrid a = stiff;
            StringGrid b = stiff;
            a.bending_stiffness_n_m2 = b.bending_stiffness_n_m2 = stiffness;
            a.loss = {0.0, 1.0, 0.0};
            b.loss = {0.0, 0.0, 1.0};
            double worst = 0.0;
            for (std::size_t mode = 2; mode <= 8; ++mode) {
                const double pa = setar::experimental::spatial_grid_decay_rate_per_s(a, mode) /
                                  setar::experimental::spatial_grid_decay_rate_per_s(a, 1);
                const double pb = setar::experimental::spatial_grid_decay_rate_per_s(b, mode) /
                                  setar::experimental::spatial_grid_decay_rate_per_s(b, 1);
                worst = std::max(worst, std::abs(pa / pb - 1.0));
            }
            if (stiffness < 1.0e-3) {
                check(worst < 1.0e-3, "setar-scale k^2 and omega^2 loss laws are collinear");
            } else {
                check(worst > 0.2, "bending-dominated fixture separates the two loss laws");
            }
            std::cout << "identifiability EI=" << stiffness << " worst_profile_difference=" << worst
                      << '\n';
        }
    }

    // (e) Overdamped modes (gamma >= Omega): two real roots, no oscillation. The oracle
    // must classify them and give no fabricated frequency, and the slow root must match
    // the simulation. Heavier damping then decays the slow root LESS, which is physical.
    {
        StringGrid heavy;
        heavy.interior_nodes = 31;
        heavy.sample_rate_hz = 96000.0;
        double previous_decay = std::numeric_limits<double>::infinity();
        for (double sigma : {1200.0, 1500.0, 3000.0, 20000.0}) {
            heavy.loss = {sigma, 0.0, 0.0};
            const auto oracle = setar::experimental::midpoint_damped_roots(heavy, 1);
            check(!oracle.oscillatory && oracle.frequency_hz == 0.0,
                  "overdamped mode 1 is reported as non-oscillatory with no frequency");
            check(oracle.spectral_radius > 0.0 && oracle.spectral_radius < 1.0,
                  "overdamped midpoint mode is strictly contractive");
            check(oracle.decay_rate_per_s < previous_decay,
                  "beyond critical damping the slowest root decays less as loss grows");
            previous_decay = oracle.decay_rate_per_s;
            MidpointStiffString string;
            string.prepare(heavy);
            set_modal_state(string, heavy.interior_nodes, {{1, 1.0e-4}});
            const auto series = modal_series(
                string, 1, decay_window(heavy.sample_rate_hz, oracle.decay_rate_per_s, 4000));
            const auto measured = setar::experimental::estimate_decay(series, heavy.sample_rate_hz);
            check(!measured.oscillatory && measured.frequency_hz == 0.0,
                  "simulated overdamped mode is classified non-oscillatory");
            // Looser than the underdamped gate: regressor conditioning reaches ~1e5 here.
            near(measured.decay_rate_per_s, oracle.decay_rate_per_s, 0.0, 1.0e-6,
                 "overdamped slow-root decay matches the simulation");
        }
        heavy.loss = {1500.0, 0.0, 0.0};
        check(setar::experimental::midpoint_damped_roots(heavy, 2).oscillatory,
              "mode 2 of the same string is still underdamped");
    }

    // (d) Convergence of the algorithmic decay and frequency to the continuum values
    // under joint refinement of h and dt, from the oracle and from simulation.
    {
        const std::array<std::pair<std::size_t, double>, 4> levels{
            {{15, 24000.0}, {31, 48000.0}, {63, 96000.0}, {127, 192000.0}}};
        for (std::size_t mode : {std::size_t{1}, std::size_t{3}}) {
            double previous_decay_error = 0.0;
            double previous_frequency_error = 0.0;
            for (std::size_t level = 0; level < levels.size(); ++level) {
                StringGrid refined;
                refined.interior_nodes = levels[level].first;
                refined.sample_rate_hz = levels[level].second;
                refined.loss = {1.2, 0.01, 2.0e-7};
                const auto oracle = setar::experimental::midpoint_damped_roots(refined, mode);
                const double decay_error = std::abs(
                    oracle.decay_rate_per_s /
                        setar::experimental::continuum_decay_rate_per_s(refined, mode) - 1.0);
                const double frequency_error = std::abs(
                    oracle.frequency_hz / setar::experimental::continuum_frequency_hz(refined, mode) -
                    1.0);
                // Simulated measurement over three periods of the mode.
                MidpointStiffString string;
                string.prepare(refined);
                set_modal_state(string, refined.interior_nodes, {{mode, 1.0e-4}});
                const std::size_t samples = static_cast<std::size_t>(
                    std::ceil(3.0 * refined.sample_rate_hz / oracle.frequency_hz));
                const auto series = modal_series(string, mode, samples);
                const auto measured =
                    setar::experimental::estimate_decay(series, refined.sample_rate_hz);
                near(measured.decay_rate_per_s, oracle.decay_rate_per_s, 0.0, 1.0e-8,
                     "refinement level: simulation equals oracle");
                if (level > 0) {
                    const double decay_order = std::log2(previous_decay_error / decay_error);
                    const double frequency_order =
                        std::log2(previous_frequency_error / frequency_error);
                    check(decay_order > 1.9 && decay_order < 2.1,
                          "decay converges to the continuum law at second order");
                    check(frequency_order > 1.9 && frequency_order < 2.1,
                          "damped frequency converges to the continuum value at second order");
                }
                std::cout << "refine mode=" << mode << " nodes=" << refined.interior_nodes
                          << " rate=" << refined.sample_rate_hz
                          << " decay_rel_error=" << decay_error
                          << " frequency_rel_error=" << frequency_error << '\n';
                previous_decay_error = decay_error;
                previous_frequency_error = frequency_error;
            }
        }
    }
}

// The comparison driver is shared with the evidence app: see tests/support/modal_crosscheck.hpp.
double modal_reference_error(const StringLoss& fd_loss, const StringLoss& reference_loss,
                             double bending_stiffness, std::size_t nodes, double rate_hz,
                             std::size_t band_modes, double duration_s) {
    return setar::verification::modal_reference_error(fd_loss, reference_loss, bending_stiffness,
                                                      nodes, rate_hz, band_modes, duration_s);
}

void loss_modal_reference_crosscheck() {
    struct Law {
        std::string_view name;
        StringLoss loss;
    };
    const std::array<Law, 5> laws{{{"lossless", {}},
                                   {"constant", {3.0, 0.0, 0.0}},
                                   {"wavenumber_squared", {0.0, 0.03, 0.0}},
                                   {"frequency_squared", {0.0, 0.0, 5.0e-7}},
                                   {"combined", {3.0, 0.03, 5.0e-7}}}};
    // Band: three modes, resolved on the coarsest 16-interval grid (mode/(N+1) <= 3/16).
    // A wider band is not resolved there and would measure pre-asymptotic error instead.
    constexpr std::size_t band_modes = 3;
    constexpr double duration = 0.03; // about five periods of the fundamental
    const std::array<std::pair<std::size_t, double>, 3> levels{
        {{15, 24000.0}, {31, 48000.0}, {63, 96000.0}}};
    for (double stiffness : {0.000015, 0.00008}) {
        for (const Law& law : laws) {
            const bool stiffer_string = stiffness > 0.00002;
            if (stiffer_string && law.name != "combined") {
                continue; // the stiffer string repeats only the full three-term law
            }
            double previous = 0.0;
            double finest = 0.0;
            for (std::size_t level = 0; level < levels.size(); ++level) {
                const double error = modal_reference_error(
                    law.loss, law.loss, stiffness, levels[level].first, levels[level].second,
                    band_modes, duration);
                if (level > 0) {
                    const double order = std::log2(previous / error);
                    check(order > 1.8 && order < 2.3,
                          "waveform error against the analytic modal reference converges at second order");
                }
                std::cout << "crosscheck EI=" << stiffness << " law=" << law.name
                          << " nodes=" << levels[level].first << " rate=" << levels[level].second
                          << " relative_linf_error=" << error << '\n';
                previous = error;
                finest = error;
            }
            // The claim is the convergence order above, not the absolute error of this
            // coarse CTest level (about 2% at 64 intervals, 4.2e-3 at 128). This bound
            // only guards against a mis-scaled or unrelated waveform.
            check(finest < 0.05, "finest CTest level is within a 5% sanity bound");
        }
    }
    // The comparison has teeth: the same strongly lossy string measured against a
    // reference with the wrong (zero) loss law is far worse than against the right one.
    const StringLoss strong = laws[4].loss;
    const double right = modal_reference_error(strong, strong, 0.000015, 63, 96000.0, band_modes,
                                               duration);
    const double wrong = modal_reference_error(strong, StringLoss{}, 0.000015, 63, 96000.0,
                                               band_modes, duration);
    const double wrong_constant_only = modal_reference_error(
        strong, StringLoss{3.0, 0.0, 0.0}, 0.000015, 63, 96000.0, band_modes, duration);
    check(wrong > 10.0 * right, "a reference with no loss is rejected by a lossy string");
    check(wrong_constant_only > 3.0 * right,
          "a reference missing the k^2 and omega^2 loss terms is rejected");
    std::cout << "crosscheck_teeth right=" << right << " zero_loss_reference=" << wrong
              << " constant_only_reference=" << wrong_constant_only << '\n';
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
    // Loss coefficients: every term must be finite and nonnegative; nothing is clipped.
    const double infinity = std::numeric_limits<double>::infinity();
    for (double bad : {-1.0e-12, nan, infinity}) {
        for (std::size_t term = 0; term < 3; ++term) {
            StringGrid g = good;
            double* coefficient = term == 0 ? &g.loss.decay_constant_per_s
                                            : term == 1 ? &g.loss.decay_wavenumber_squared_m2_per_s
                                                        : &g.loss.decay_frequency_squared_s;
            *coefficient = bad;
            rejected([&] { string.prepare(g); }, "negative or nonfinite loss coefficient rejected");
            near(string.energy_j(), old_energy, 0.0, 0.0,
                 "rejected loss coefficient leaves the prepared string untouched");
            rejected([&] { (void)setar::experimental::midpoint_damped_roots(g, 1); },
                     "loss oracle rejects an invalid coefficient");
        }
    }
    {
        StringGrid lossy = good;
        lossy.loss.decay_constant_per_s = 1.0;
        LeapfrogStiffString explicit_grid;
        rejected([&] { explicit_grid.prepare(lossy); },
                 "explicit prototype rejects loss instead of silently ignoring it");
        check(!explicit_grid.prepared(), "failed explicit prepare leaves the object unprepared");
        rejected([&] { (void)setar::experimental::midpoint_damped_roots(good, 0); },
                 "mode zero rejected by the loss oracle");
        rejected([&] { (void)setar::experimental::continuum_decay_rate_per_s(good, 1000); },
                 "unrepresented mode rejected by the continuum loss oracle");
    }
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
    constexpr std::array<Case, 15> tests{{
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
        {"loss_law_passivity", loss_law_passivity},
        {"loss_decay_estimator", loss_decay_estimator},
        {"loss_decay_oracle", loss_decay_oracle},
        {"loss_modal_reference_crosscheck", loss_modal_reference_crosscheck},
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
