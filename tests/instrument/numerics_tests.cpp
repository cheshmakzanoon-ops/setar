// Independent verification of the setar::instrument numerical building blocks:
// exact-modal oscillator bank, discrete-gradient interaction channels and the small
// dense Newton solve (docs/numerics/15-modal-contact-solver.md). Every expectation is
// computed by code in this file or tests/support/instrument_oracles.hpp, never by the
// function under test. The runner receives the full CTest name as argv[1].
#include "setar/instrument/numerics.hpp"
#include "support/instrument_oracles.hpp"

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
#include <vector>

namespace {
namespace nm = setar::instrument::numerics;
namespace ver = setar::verification;
using nm::ChannelLaw;
using nm::ChannelLawKind;
constexpr double pi = std::numbers::pi_v<double>;

void check(bool condition, std::string_view message) {
    if (!condition) {
        throw std::runtime_error(std::string(message));
    }
}

void near(double actual, double expected, double atol, double rtol, std::string_view message) {
    const double difference = std::abs(actual - expected);
    const double limit = atol + rtol * std::abs(expected);
    if (!std::isfinite(actual) || !std::isfinite(expected) || difference > limit) {
        throw std::runtime_error(std::string(message) + " actual=" + std::to_string(actual) +
                                 " expected=" + std::to_string(expected) +
                                 " difference=" + std::to_string(difference) +
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

// ---------------------------------------------------------------------------------------
// Test-side definitions of the contact potentials and their discrete gradient, written
// from docs/numerics/15 without calling the library.
double test_potential(const ChannelLaw& law, double z) {
    switch (law.kind) {
    case ChannelLawKind::OneSidedPower: {
        const double p = std::max(0.0, -z);
        return law.stiffness * std::pow(p, law.exponent + 1.0) / (law.exponent + 1.0);
    }
    case ChannelLawKind::LinearSpring:
        return 0.5 * law.stiffness * (z - law.anchor_m) * (z - law.anchor_m);
    case ChannelLawKind::Damper:
        return 0.0;
    }
    throw std::runtime_error("unknown law");
}

double test_potential_slope(const ChannelLaw& law, double z) { // dV/dz
    switch (law.kind) {
    case ChannelLawKind::OneSidedPower: {
        const double p = std::max(0.0, -z);
        return -law.stiffness * std::pow(p, law.exponent);
    }
    case ChannelLawKind::LinearSpring:
        return law.stiffness * (z - law.anchor_m);
    case ChannelLawKind::Damper:
        return 0.0;
    }
    throw std::runtime_error("unknown law");
}

// Secant force -(V(z+s)-V(z))/s - R s/(2k); tangent value at s == 0 only.
double test_force(const ChannelLaw& law, double z, double s, double dt) {
    const double gradient = s == 0.0 ? test_potential_slope(law, z)
                                     : (test_potential(law, z + s) - test_potential(law, z)) / s;
    return -gradient - law.damping_n_s_per_m * s / (2.0 * dt);
}

double test_force_derivative(const ChannelLaw& law, double z, double s, double dt) {
    // d/ds of the secant: (V'(z+s) - G)/s. Callers use |s| well above roundoff.
    const double gradient = (test_potential(law, z + s) - test_potential(law, z)) / s;
    return -(test_potential_slope(law, z + s) - gradient) / s -
           law.damping_n_s_per_m / (2.0 * dt);
}

ChannelLaw power_law(double k, double alpha, double r = 0.0) {
    ChannelLaw law;
    law.kind = ChannelLawKind::OneSidedPower;
    law.stiffness = k;
    law.exponent = alpha;
    law.damping_n_s_per_m = r;
    return law;
}

ChannelLaw linear_law(double k, double anchor, double r = 0.0) {
    ChannelLaw law;
    law.kind = ChannelLawKind::LinearSpring;
    law.stiffness = k;
    law.anchor_m = anchor;
    law.damping_n_s_per_m = r;
    return law;
}

ChannelLaw damper_law(double r) {
    ChannelLaw law;
    law.kind = ChannelLawKind::Damper;
    law.damping_n_s_per_m = r;
    return law;
}

// ---------------------------------------------------------------------------------------
void modal_exact_free_response() {
    struct ModeCase {
        double frequency_hz;
        double sigma;
        double mass;
    };
    for (double fs : {44100.0, 48000.0, 96000.0}) {
        const double dt = 1.0 / fs;
        const std::vector<ModeCase> cases{
            {27.5, 0.0, 0.01},        {27.5, 0.7, 2.0e-4},  {440.0, 0.0, 1.0},
            {440.0, 25.0, 0.003},     {4000.0, 3.0, 1.0e-4}, {0.45 * fs, 300.0, 0.05},
            {0.49 * fs, 0.0, 0.02},   {100.0, 0.9 * 2.0 * pi * 100.0, 0.5},
        };
        std::vector<double> mass, omega, sigma, q0;
        for (std::size_t i = 0; i < cases.size(); ++i) {
            mass.push_back(cases[i].mass);
            omega.push_back(2.0 * pi * cases[i].frequency_hz);
            sigma.push_back(cases[i].sigma);
            q0.push_back(1.0e-3 * static_cast<double>(i + 1));
        }
        nm::ModalBank bank;
        bank.prepare(mass, omega, sigma, dt);
        check(bank.size() == cases.size(), "bank size");
        bank.set_displacement(q0); // q[-1] = q[0] = q0
        std::vector<ver::DampedOscillator> exact;
        for (std::size_t i = 0; i < cases.size(); ++i) {
            exact.push_back(ver::damped_oscillator_through(omega[i], sigma[i], -dt, q0[i], 0.0, q0[i]));
        }
        std::vector<double> worst(cases.size(), 0.0);
        const auto steps = static_cast<std::size_t>(2.0 * fs); // 2 s: 55 periods at 27.5 Hz
        for (std::size_t n = 1; n <= steps; ++n) {
            bank.predict();
            bank.commit();
            const double t = static_cast<double>(n) * dt;
            for (std::size_t i = 0; i < cases.size(); ++i) {
                worst[i] = std::max(worst[i], std::abs(bank.current()[i] - exact[i](t)) / q0[i]);
            }
        }
        for (std::size_t i = 0; i < cases.size(); ++i) {
            std::cout << "fs=" << fs << " f=" << cases[i].frequency_hz << " sigma=" << cases[i].sigma
                      << " max|q-exact|/q0=" << worst[i] << '\n';
            // Only roundoff separates the bank from the oscillator: rounding a = 2e cos(wd k)
            // perturbs the phase per step, worst near Nyquist (observed ~4e-9 at 0.49 fs over
            // 2 s). 1e-7 leaves margin yet rejects any O((wk)^2) frequency warping, which
            // would be ~1e-4 per period for the 4 kHz mode at 44.1 kHz.
            check(worst[i] < 1.0e-7, "exact-modal free response matches the continuous oscillator");
        }
    }
}

void modal_energy_identity() {
    const double fs = 48000.0;
    const double dt = 1.0 / fs;
    const std::vector<double> mass{0.01, 3.0e-4, 1.0, 0.02, 5.0e-3, 0.4};
    const std::vector<double> omega{2.0 * pi * 60.0, 2.0 * pi * 440.0, 2.0 * pi * 3100.0,
                                    2.0 * pi * 9000.0, 2.0 * pi * 0.47 * fs, 2.0 * pi * 1500.0};
    const std::vector<double> sigma{0.0, 1.5, 40.0, 0.0, 900.0, 0.95 * 2.0 * pi * 1500.0};
    nm::ModalBank bank;
    bank.prepare(mass, omega, sigma, dt);
    // Independent coefficients: s~ k = (1-b)/(1+b) and w~^2 k^2 = 2 - 2a/(1+b) from the
    // exact roots, rather than the library's tanh/sinh/sin forms.
    std::vector<double> st(mass.size()), wt(mass.size());
    for (std::size_t i = 0; i < mass.size(); ++i) {
        const double wd = std::sqrt(omega[i] * omega[i] - sigma[i] * sigma[i]);
        const double a = 2.0 * std::exp(-sigma[i] * dt) * std::cos(wd * dt);
        const double b = std::exp(-2.0 * sigma[i] * dt);
        st[i] = (1.0 - b) / ((1.0 + b) * dt);
        wt[i] = (2.0 - 2.0 * a / (1.0 + b)) / (dt * dt);
    }
    auto test_energy = [&](std::span<const double> cur, std::span<const double> prev) {
        double e = 0.0;
        for (std::size_t i = 0; i < mass.size(); ++i) {
            const double v = (cur[i] - prev[i]) / dt;
            e += 0.5 * mass[i] * (v * v + wt[i] * cur[i] * prev[i]);
        }
        return e;
    };
    std::mt19937_64 rng(20261031);
    std::normal_distribution<double> unit(0.0, 1.0);
    std::vector<double> shape(mass.size());
    for (double& s : shape) {
        s = unit(rng);
    }
    std::vector<double> q0{1.0e-3, -2.0e-4, 5.0e-5, 1.0e-5, 0.0, 3.0e-4};
    bank.set_displacement(q0);
    double worst_identity = 0.0, worst_scheme = 0.0, worst_energy = 0.0;
    std::vector<double> prev(mass.size()), cur(mass.size()), next(mass.size());
    for (int n = 0; n < 20000; ++n) {
        const double force = 0.05 * unit(rng);
        const double h_before = test_energy(bank.current(), bank.previous());
        std::copy(bank.previous().begin(), bank.previous().end(), prev.begin());
        std::copy(bank.current().begin(), bank.current().end(), cur.begin());
        bank.predict();
        bank.add_force(shape, force);
        std::copy(bank.next().begin(), bank.next().end(), next.begin());
        double dissipation = 0.0, work = 0.0, scale = 0.0;
        for (std::size_t i = 0; i < mass.size(); ++i) {
            const double f = shape[i] * force;
            // Defining difference equation of the scheme, scaled by m/k^2.
            const double lhs = (1.0 + st[i] * dt) * next[i] - (2.0 - wt[i] * dt * dt) * cur[i] +
                               (1.0 - st[i] * dt) * prev[i];
            const double magnitude = std::abs(next[i]) + 2.0 * std::abs(cur[i]) + std::abs(prev[i]) +
                                     std::abs(dt * dt * f / mass[i]);
            worst_scheme = std::max(worst_scheme, std::abs(lhs - dt * dt * f / mass[i]) / magnitude);
            const double v = (next[i] - prev[i]) / (2.0 * dt);
            dissipation += 2.0 * mass[i] * st[i] * dt * v * v;
            work += f * (next[i] - prev[i]) / 2.0;
            scale += std::abs(f * (next[i] - prev[i]));
        }
        near(bank.step_dissipation_j(), dissipation, 1.0e-300, 1.0e-9, "bank dissipation formula");
        bank.commit();
        const double h_after = test_energy(bank.current(), bank.previous());
        near(bank.energy_j(), h_after, 1.0e-300, 1.0e-9, "bank energy formula");
        check(h_after >= 0.0, "discrete modal energy is nonnegative");
        const double defect = (h_after - h_before) + dissipation - work;
        worst_identity = std::max(worst_identity, std::abs(defect) / (h_before + h_after + scale + dissipation));
        worst_energy = std::max(worst_energy, h_after);
    }
    std::cout << "relative scheme-equation defect=" << worst_scheme
              << " relative energy-identity defect=" << worst_identity
              << " max energy=" << worst_energy << " J\n";
    check(worst_scheme < 1.0e-13, "bank update satisfies the declared difference equation");
    check(worst_identity < 1.0e-12, "per-step energy identity holds to roundoff");
}

void modal_forced_convergence() {
    const double mass = 2.0e-3, omega = 2.0 * pi * 200.0, sigma = 2.0, force = 1.0e-3;
    const double forcing = 2.0 * pi * 330.0, duration = 0.05;
    const ver::SinusoidallyForcedOscillator exact =
        ver::sinusoidally_forced_oscillator(mass, omega, sigma, force, forcing);
    std::vector<double> errors;
    const std::array<double, 6> rates{4000.0, 8000.0, 16000.0, 32000.0, 64000.0, 128000.0};
    for (double fs : rates) {
        const double dt = 1.0 / fs;
        nm::ModalBank bank;
        const std::array<double, 1> m{mass}, w{omega}, s{sigma}, unit{1.0};
        bank.prepare(m, w, s, dt);
        double worst = 0.0, peak = 0.0;
        const auto steps = static_cast<std::size_t>(std::llround(duration * fs));
        for (std::size_t n = 0; n < steps; ++n) {
            bank.predict();
            bank.add_force(unit, force * std::sin(forcing * static_cast<double>(n) * dt));
            bank.commit();
            const double t = static_cast<double>(n + 1) * dt;
            worst = std::max(worst, std::abs(bank.current()[0] - exact(t)));
            peak = std::max(peak, std::abs(exact(t)));
        }
        errors.push_back(worst / peak);
        std::cout << "fs=" << fs << " max relative forced-response error=" << errors.back() << '\n';
    }
    for (std::size_t i = 1; i < errors.size(); ++i) {
        const double order = std::log2(errors[i - 1] / errors[i]);
        std::cout << "observed order " << rates[i - 1] << "->" << rates[i] << " = " << order << '\n';
        check(errors[i] < errors[i - 1], "forced error decreases with the time step");
        if (i >= 2) {
            check(order > 1.8 && order < 2.2, "forced response converges at second order");
        }
    }
    check(errors.back() < 1.0e-4, "finest forced response within 1e-4 of the analytic solution");
}

void sine_shape_accuracy() {
    std::vector<double> thetas{1.0e-9, 1.0e-6, 1.0e-3, 0.1, 1.0, pi / 2.0, 2.0, 3.1,
                               pi - 1.0e-6, pi, 0.0};
    std::mt19937_64 rng(7);
    std::uniform_real_distribution<double> uniform(0.0, pi);
    for (int i = 0; i < 20; ++i) {
        thetas.push_back(uniform(rng));
    }
    constexpr std::size_t n_max = 1024;
    std::vector<double> out(n_max);
    double worst = 0.0;
    for (double theta : thetas) {
        nm::fill_sine_shape(out, theta);
        for (std::size_t i = 0; i < n_max; ++i) {
            const long double exact =
                std::sin(static_cast<long double>(i + 1) * static_cast<long double>(theta));
            const double error = std::abs(out[i] - static_cast<double>(exact));
            worst = std::max(worst, error);
            // Rotation error grows about linearly with the index (one rounding per step,
            // renormalized magnitude); 1e-15 per mode is ~5 ulp per step.
            check(error <= 1.0e-15 * static_cast<double>(i + 1) + 1.0e-15,
                  "rotated sine shape matches long-double sine");
        }
    }
    std::cout << "max |fill_sine_shape - sin| over N=1024 = " << worst << '\n';
    std::array<double, 1> single{-7.0};
    nm::fill_sine_shape(single, 0.3);
    near(single[0], std::sin(0.3), 1.0e-16, 0.0, "single-mode shape");
    nm::fill_sine_shape(std::span<double>{}, 0.3); // empty span: no write, no crash
}

void discrete_gradient_identity() {
    const double dt = 1.0 / 48000.0;
    std::vector<ChannelLaw> laws;
    for (double alpha : {1.0, 1.5, 2.0, 3.2}) {
        for (double k : {1.0e3, 1.0e9}) {
            for (double r : {0.0, 2.5}) {
                laws.push_back(power_law(k, alpha, r));
            }
        }
    }
    laws.push_back(linear_law(1.0e6, 0.0));
    laws.push_back(linear_law(3.0e4, 2.0e-5, 0.7));
    laws.push_back(damper_law(4.0));
    struct Pair {
        double z;
        double s;
    };
    std::vector<Pair> pairs{
        {1.0e-4, 1.0e-5},   // separated at both levels
        {1.0e-5, -3.0e-5},  // entering contact
        {-2.0e-5, 5.0e-5},  // leaving contact
        {-1.0e-3, 1.0e-9},  // deep, |s| << |z| (tangent branch)
        {-1.0e-3, -1.0e-11},
        {0.0, -1.0e-6},     // contact onset exactly at zero gap
        {0.0, 1.0e-6},
        {-4.0e-6, 4.0e-6},  // lands exactly on zero gap
    };
    std::mt19937_64 rng(11);
    std::uniform_real_distribution<double> sign(-1.0, 1.0);
    std::uniform_real_distribution<double> exponent(-16.0, -2.0);
    for (int i = 0; i < 3000; ++i) {
        const double z = std::copysign(std::pow(10.0, exponent(rng)), sign(rng));
        const double s = std::copysign(std::pow(10.0, exponent(rng)), sign(rng));
        pairs.push_back({z, s});
    }
    double worst = 0.0;
    for (const ChannelLaw& law : laws) {
        for (const Pair& p : pairs) {
            const nm::ChannelForce f = nm::channel_force(law, p.z, p.s, dt);
            const double v0 = test_potential(law, p.z);
            const double v1 = test_potential(law, p.z + p.s);
            near(nm::channel_potential_j(law, p.z), v0, 1.0e-300, 1.0e-13, "channel potential");
            const double damping = law.damping_n_s_per_m * p.s * p.s / (2.0 * dt);
            const double rhs = -(v1 - v0) - damping;
            const double scale = std::abs(v0) + std::abs(v1) + damping;
            const double defect = std::abs(f.force_n * p.s - rhs);
            if (scale == 0.0) {
                check(f.force_n == 0.0, "separated undamped contact exerts exactly zero force");
            } else {
                worst = std::max(worst, defect / scale);
                check(defect <= 1.0e-12 * scale, "F*s equals minus the potential change minus damping");
            }
        }
        // s == 0: the force is the tangent -V'(z) (no damping contribution).
        for (double z : {-3.0e-4, 0.0, 2.0e-4}) {
            const nm::ChannelForce f0 = nm::channel_force(law, z, 0.0, dt);
            near(f0.force_n, -test_potential_slope(law, z), 1.0e-300, 1.0e-12, "tangent force at s=0");
        }
    }
    std::cout << "worst relative discrete-gradient defect=" << worst << " over " << pairs.size()
              << " pairs x " << laws.size() << " laws\n";
}

void contact_law_monotonicity() {
    const double dt = 1.0 / 48000.0;
    std::vector<ChannelLaw> laws;
    for (double alpha : {1.0, 1.5, 2.0, 3.0}) {
        laws.push_back(power_law(1.0e8, alpha, 0.0));
        laws.push_back(power_law(5.0e4, alpha, 1.2));
    }
    laws.push_back(linear_law(2.0e5, 1.0e-5, 0.3));
    laws.push_back(damper_law(2.0));
    std::vector<double> zs, ss;
    for (int i = -40; i <= 40; ++i) {
        zs.push_back(2.5e-5 * static_cast<double>(i));
    }
    for (int i = -60; i <= 60; ++i) {
        if (i != 0) {
            ss.push_back(1.7e-5 * static_cast<double>(i) + 3.1e-8);
        }
    }
    std::size_t fd_checks = 0;
    double worst_fd = 0.0;
    for (const ChannelLaw& law : laws) {
        const double r_term = law.damping_n_s_per_m / (2.0 * dt);
        for (double z : zs) {
            double previous_force = std::numeric_limits<double>::infinity();
            for (double s : ss) {
                const nm::ChannelForce f = nm::channel_force(law, z, s, dt);
                // Convexity makes the secant nondecreasing in s, so dF/ds <= 0; allow a
                // relative roundoff from the cancellation in (V'(z+s) - G)/s.
                const double stiffness_scale =
                    law.stiffness * std::max(1.0, std::pow(std::abs(z) + std::abs(s), law.exponent - 1.0)) +
                    r_term;
                check(f.derivative <= 1.0e-9 * stiffness_scale, "dF/ds <= 0");
                check(f.force_n <= previous_force + 1.0e-12 * (std::abs(previous_force) + 1.0),
                      "F is nonincreasing in s");
                previous_force = f.force_n;
                if (law.kind == ChannelLawKind::OneSidedPower && z >= 0.0 && z + s >= 0.0) {
                    check(f.force_n == -r_term * s, "separated contact: damping force only");
                    check(f.derivative == -r_term, "separated contact: damping slope only");
                }
                // Independent analytic derivative of the test-side secant.
                near(f.derivative, test_force_derivative(law, z, s, dt), 1.0e-9 * stiffness_scale, 1.0e-6,
                     "analytic secant derivative");
                // Central finite difference of the library force, away from the zero-gap
                // kink where alpha = 1 has a second-derivative jump.
                const double h = 1.0e-4 * std::abs(s);
                if (std::abs(z + s) > 10.0 * h) {
                    const double fd = (nm::channel_force(law, z, s + h, dt).force_n -
                                       nm::channel_force(law, z, s - h, dt).force_n) /
                                      (2.0 * h);
                    const double error = std::abs(fd - f.derivative);
                    worst_fd = std::max(worst_fd, error / stiffness_scale);
                    check(error <= 1.0e-5 * std::abs(f.derivative) + 1.0e-7 * stiffness_scale,
                          "derivative matches central finite difference");
                    ++fd_checks;
                }
            }
        }
    }
    std::cout << "finite-difference derivative checks=" << fd_checks
              << " worst scaled error=" << worst_fd << '\n';
}

// Bisection on h(s) = s - b - M*F(s), strictly increasing because M >= 0 and dF/ds <= 0.
double bisection_root(const ChannelLaw& law, double m, double b, double z, double dt) {
    const auto h = [&](double s) { return s - b - m * test_force(law, z, s, dt); };
    double width = std::abs(b) + std::abs(z) + 1.0e-6;
    double lo = b - width, hi = b + width;
    for (int i = 0; i < 200 && !(h(lo) < 0.0 && h(hi) > 0.0); ++i) {
        width *= 2.0;
        lo = b - width;
        hi = b + width;
    }
    check(h(lo) < 0.0 && h(hi) > 0.0, "bisection bracket");
    for (int i = 0; i < 400; ++i) {
        const double mid = 0.5 * (lo + hi);
        if (mid == lo || mid == hi) {
            break;
        }
        (h(mid) < 0.0 ? lo : hi) = mid;
    }
    return 0.5 * (lo + hi);
}

void newton_scalar_oracle() {
    const double dt = 1.0 / 48000.0;
    nm::SmallNewtonSolver solver;
    solver.prepare(4);
    std::mt19937_64 rng(42);
    std::uniform_real_distribution<double> unit(0.0, 1.0);
    std::uint32_t max_iterations_seen = 0;
    double worst = 0.0;
    for (int trial = 0; trial < 600; ++trial) {
        const double m = std::pow(10.0, -10.0 + 5.0 * unit(rng));
        const double k = std::pow(10.0, 4.0 + 6.0 * unit(rng));
        const double alpha = std::array<double, 3>{1.0, 1.5, 2.0}[static_cast<std::size_t>(trial % 3)];
        const double r = std::array<double, 3>{0.0, 0.5, 5.0}[static_cast<std::size_t>((trial / 3) % 3)];
        const ChannelLaw law = power_law(k, alpha, r);
        const double z = (unit(rng) - 0.5) * 2.0e-4;
        const double b = (unit(rng) - 0.5) * 4.0e-4;
        const double tolerance = 1.0e-12 * (std::abs(b) + std::abs(z) + 1.0e-12);
        std::array<double, 1> s{b}, force{0.0};
        const std::array<double, 1> mm{m}, bb{b}, zz{z};
        const std::array<ChannelLaw, 1> ll{law};
        const nm::NewtonResult result = solver.solve(1, mm, bb, ll, zz, dt, s, force, 50, tolerance);
        check(result.converged && result.finite, "scalar Newton converges");
        check(result.residual_m <= tolerance, "reported residual within tolerance");
        max_iterations_seen = std::max(max_iterations_seen, result.iterations);
        const double expected = bisection_root(law, m, b, z, dt);
        const double error = std::abs(s[0] - expected);
        worst = std::max(worst, error / (std::abs(b) + std::abs(z) + 1.0e-12));
        // h' >= 1, so |s - s*| <= |residual|; add the bisection resolution.
        check(error <= 2.0 * tolerance + 4.0 * std::numeric_limits<double>::epsilon() *
                                              (std::abs(expected) + std::abs(b)),
              "Newton root equals the bisection root");
        if (std::abs(s[0]) > 1.0e-9 * (std::abs(z) + 1.0e-12)) {
            near(force[0], test_force(law, z, s[0], dt), 1.0e-9, 1.0e-8,
                 "returned force is the law evaluated at the returned s");
        }
    }
    std::cout << "scalar contact solves=600 max iterations=" << max_iterations_seen
              << " worst relative |s - s_bisection|=" << worst << '\n';
}

// Primal oracle: minimize 1/2 y^T G^-1 y + sum_c Lambda_c(B_c y + b_c) over modal
// increments y by its own Newton iteration (SPD Hessian, Cholesky), independent of the
// channel-space solver under test. s = B*y + b at the minimizer.
std::vector<double> primal_oracle(const std::vector<std::vector<double>>& bmat,
                                  const std::vector<double>& g, const std::vector<double>& b,
                                  const std::vector<ChannelLaw>& laws,
                                  const std::vector<double>& z, double dt) {
    const std::size_t m = bmat.size(), n = g.size();
    std::vector<double> y(n, 0.0), s(m), grad(n), hess(n * n), step(n);
    auto channel_s = [&](const std::vector<double>& yy) {
        std::vector<double> out(m);
        for (std::size_t c = 0; c < m; ++c) {
            double sum = b[c];
            for (std::size_t i = 0; i < n; ++i) {
                sum += bmat[c][i] * yy[i];
            }
            out[c] = sum;
        }
        return out;
    };
    auto gradient_norm = [&](const std::vector<double>& yy, std::vector<double>& out) {
        const std::vector<double> ss = channel_s(yy);
        double norm = 0.0;
        for (std::size_t i = 0; i < n; ++i) {
            double sum = yy[i] / g[i];
            for (std::size_t c = 0; c < m; ++c) {
                sum -= bmat[c][i] * test_force(laws[c], z[c], ss[c], dt); // Gamma = -F
            }
            out[i] = sum;
            norm = std::max(norm, std::abs(sum) * g[i]); // in metres
        }
        return norm;
    };
    double norm = gradient_norm(y, grad);
    for (int iteration = 0; iteration < 300 && norm > 1.0e-22; ++iteration) {
        s = channel_s(y);
        for (std::size_t i = 0; i < n; ++i) {
            for (std::size_t j = 0; j < n; ++j) {
                double sum = i == j ? 1.0 / g[i] : 0.0;
                for (std::size_t c = 0; c < m; ++c) {
                    const double h = std::max(1.0e-12 * std::abs(s[c]), 1.0e-18);
                    const double slope = -(test_force(laws[c], z[c], s[c] + h, dt) -
                                           test_force(laws[c], z[c], s[c] - h, dt)) /
                                         (2.0 * h);
                    sum += bmat[c][i] * std::max(slope, 0.0) * bmat[c][j];
                }
                hess[i * n + j] = sum;
            }
        }
        // Cholesky solve hess * step = -grad.
        std::vector<double> l(n * n, 0.0);
        for (std::size_t i = 0; i < n; ++i) {
            for (std::size_t j = 0; j <= i; ++j) {
                double sum = hess[i * n + j];
                for (std::size_t k = 0; k < j; ++k) {
                    sum -= l[i * n + k] * l[j * n + k];
                }
                if (i == j) {
                    check(sum > 0.0, "primal Hessian is SPD");
                    l[i * n + i] = std::sqrt(sum);
                } else {
                    l[i * n + j] = sum / l[j * n + j];
                }
            }
        }
        for (std::size_t i = 0; i < n; ++i) {
            double sum = -grad[i];
            for (std::size_t k = 0; k < i; ++k) {
                sum -= l[i * n + k] * step[k];
            }
            step[i] = sum / l[i * n + i];
        }
        for (std::size_t i = n; i-- > 0;) {
            double sum = step[i];
            for (std::size_t k = i + 1; k < n; ++k) {
                sum -= l[k * n + i] * step[k];
            }
            step[i] = sum / l[i * n + i];
        }
        double t = 1.0;
        std::vector<double> trial(n), trial_grad(n);
        double trial_norm = norm;
        for (int halving = 0; halving < 40; ++halving) {
            for (std::size_t i = 0; i < n; ++i) {
                trial[i] = y[i] + t * step[i];
            }
            trial_norm = gradient_norm(trial, trial_grad);
            if (trial_norm < norm) {
                break;
            }
            t *= 0.5;
        }
        if (!(trial_norm < norm)) {
            break; // stalled at roundoff
        }
        y = trial;
        grad = trial_grad;
        norm = trial_norm;
    }
    return channel_s(y);
}

void newton_coupled_oracle() {
    const double dt = 1.0 / 48000.0;
    nm::SmallNewtonSolver solver;
    solver.prepare(8);
    std::mt19937_64 rng(1234);
    std::normal_distribution<double> normal(0.0, 1.0);
    std::uniform_real_distribution<double> unit(0.0, 1.0);
    const std::vector<ChannelLaw> palette{power_law(1.0e8, 1.5), linear_law(1.0e5, 1.0e-6, 0.2),
                                          damper_law(3.0), power_law(1.0e7, 1.0, 0.2),
                                          power_law(3.0e9, 2.0), linear_law(4.0e3, -2.0e-6),
                                          power_law(2.0e6, 1.3, 1.0)};
    std::uint32_t max_iterations_seen = 0;
    double worst = 0.0;
    for (int trial = 0; trial < 120; ++trial) {
        const std::size_t m = 2 + static_cast<std::size_t>(trial % 6); // 2..7 channels
        const std::size_t n = 12;
        std::vector<double> g(n);
        for (double& gi : g) {
            gi = std::pow(10.0, -7.0 + 2.0 * unit(rng));
        }
        std::vector<std::vector<double>> bmat(m, std::vector<double>(n));
        for (auto& row : bmat) {
            for (double& x : row) {
                x = normal(rng);
            }
        }
        if (trial % 3 == 0) { // near-duplicate rows: M is numerically singular
            for (std::size_t i = 0; i < n; ++i) {
                bmat[1][i] = bmat[0][i] + 1.0e-9 * normal(rng);
            }
        }
        std::vector<double> compliance(m * m), b(m), z(m), s(m), force(m);
        std::vector<ChannelLaw> laws(m);
        for (std::size_t c = 0; c < m; ++c) {
            laws[c] = palette[(c + static_cast<std::size_t>(trial)) % palette.size()];
            z[c] = (unit(rng) - 0.5) * 4.0e-5;
            b[c] = (unit(rng) - 0.5) * 8.0e-5;
            s[c] = b[c];
            for (std::size_t d = 0; d < m; ++d) {
                double sum = 0.0;
                for (std::size_t i = 0; i < n; ++i) {
                    sum += bmat[c][i] * g[i] * bmat[d][i];
                }
                compliance[c * m + d] = sum;
            }
        }
        const double tolerance = 1.0e-17;
        const nm::NewtonResult result =
            solver.solve(m, compliance, b, laws, z, dt, s, force, 60, tolerance);
        check(result.finite, "coupled Newton stays finite");
        check(result.converged, "coupled Newton converges within the bound");
        check(result.iterations <= 60, "iteration bound respected");
        max_iterations_seen = std::max(max_iterations_seen, result.iterations);
        // (a) Residual recomputed with the test-side force law and compliance.
        for (std::size_t c = 0; c < m; ++c) {
            double mf = 0.0;
            for (std::size_t d = 0; d < m; ++d) {
                mf += compliance[c * m + d] * test_force(laws[d], z[d], s[d], dt);
            }
            const double residual = s[c] - b[c] - mf;
            check(std::abs(residual) <= 1.0e-15, "independent residual of the returned solution");
        }
        // (b) Agreement with the primal minimizer (unique by strict convexity).
        const std::vector<double> oracle = primal_oracle(bmat, g, b, laws, z, dt);
        for (std::size_t c = 0; c < m; ++c) {
            const double error = std::abs(s[c] - oracle[c]);
            worst = std::max(worst, error);
            check(error <= 1.0e-14, "channel-space Newton equals the primal minimizer");
        }
    }
    std::cout << "coupled solves=120 max iterations=" << max_iterations_seen
              << " worst |s - s_primal|=" << worst << " m\n";

    // Iteration cap: a stiff nonlinear solve limited to one iteration must report failure,
    // with a residual consistent with the returned state.
    {
        const std::array<double, 1> mm{1.0e-4}, bb{-1.0e-4}, zz{0.0};
        const std::array<ChannelLaw, 1> ll{power_law(1.0e10, 2.0)};
        std::array<double, 1> s{bb[0]}, force{0.0};
        const nm::NewtonResult capped = solver.solve(1, mm, bb, ll, zz, dt, s, force, 1, 1.0e-18);
        check(!capped.converged && capped.finite, "capped solve reports non-convergence");
        check(capped.iterations == 1, "capped solve performed exactly the permitted iteration");
        const double residual = s[0] - bb[0] - mm[0] * test_force(ll[0], zz[0], s[0], dt);
        near(capped.residual_m, std::abs(residual), 1.0e-20, 1.0e-6, "capped residual is truthful");
        check(capped.residual_m > 1.0e-18, "capped residual exceeds tolerance");
        near(force[0], test_force(ll[0], zz[0], s[0], dt), 1.0e-12, 1.0e-9, "capped force is F(s)");
    }
    // Capacity and non-finite input are reported, never claimed as converged.
    {
        std::vector<double> big(81, 0.0), vec(9, 0.0), out(9, 0.0), out_force(9, 0.0);
        std::vector<ChannelLaw> laws(9, linear_law(1.0, 0.0));
        const nm::NewtonResult over = solver.solve(9, big, vec, laws, vec, dt, out, out_force, 10, 1.0e-12);
        check(!over.converged && over.iterations == 0, "m > capacity is rejected");
        const std::array<double, 1> mm{1.0e-6}, bb{std::numeric_limits<double>::quiet_NaN()}, zz{0.0};
        const std::array<ChannelLaw, 1> ll{power_law(1.0e6, 1.5)};
        std::array<double, 1> s{0.0}, force{0.0};
        const nm::NewtonResult nan = solver.solve(1, mm, bb, ll, zz, dt, s, force, 10, 1.0e-12);
        check(!nan.converged && !nan.finite, "NaN input is reported as non-finite, not converged");
        const nm::NewtonResult empty = solver.solve(0, {}, {}, {}, {}, dt, {}, {}, 10, 1.0e-12);
        check(empty.converged && empty.iterations == 0, "empty system is trivially solved");
    }
}

void point_spring_eigen_oracle() {
    // Ideal string (EI = 0, lossless) with a grounded linear spring at 0.37 L, modelled by
    // the sine-mode bank plus one LinearSpring channel. Synthetic fixture values.
    const double tension = 35.0, density = 7.0e-4, length = 0.66, x0 = 0.37 * length, spring = 300.0;
    const double c = std::sqrt(tension / density);
    const std::vector<double> k_exact = ver::point_spring_wavenumbers(tension, length, x0, spring, 3);
    nm::SmallNewtonSolver solver;
    solver.prepare(1);
    const ChannelLaw law = linear_law(spring, 0.0);
    auto measure = [&](std::size_t modes, std::size_t which, double fs, double& energy_drift) {
        const double dt = 1.0 / fs;
        const double k = k_exact[which];
        const double a_left = 1.0 / std::sin(k * x0), a_right = 1.0 / std::sin(k * (length - x0));
        const auto eigenfunction = [&](double x) {
            return x <= x0 ? a_left * std::sin(k * x) : a_right * std::sin(k * (length - x));
        };
        std::vector<double> mass(modes, 0.5 * density * length), omega(modes), sigma(modes, 0.0);
        std::vector<double> q0(modes), shape(modes);
        for (std::size_t n = 0; n < modes; ++n) {
            const double kn = pi * static_cast<double>(n + 1) / length;
            omega[n] = c * kn;
            const auto integrand = [&](double x) { return eigenfunction(x) * std::sin(kn * x); };
            q0[n] = 1.0e-3 * 2.0 / length *
                    (ver::simpson(integrand, 0.0, x0, 4000) + ver::simpson(integrand, x0, length, 4000));
            shape[n] = std::sin(kn * x0);
        }
        nm::ModalBank bank;
        bank.prepare(mass, omega, sigma, dt);
        bank.set_displacement(q0);
        const auto steps = static_cast<std::size_t>(0.25 * fs);
        std::vector<double> projection;
        projection.reserve(steps);
        double e0 = -1.0;
        energy_drift = 0.0;
        for (std::size_t step = 0; step < steps; ++step) {
            const double z_prev = bank.gather_previous(shape);
            const double z_cur = bank.gather_current(shape);
            bank.predict();
            const double m = bank.compliance(shape, shape);
            const std::array<double, 1> mm{m}, bb{bank.gather_next(shape) - z_prev}, zz{z_prev};
            const std::array<ChannelLaw, 1> ll{law};
            std::array<double, 1> s{bb[0]}, force{0.0};
            const nm::NewtonResult r = solver.solve(1, mm, bb, ll, zz, dt, s, force, 10, 1.0e-20);
            check(r.converged, "linear spring channel solves");
            bank.add_force(shape, force[0]);
            const double z_next = bank.gather_next(shape);
            bank.commit();
            // Lossless coupled ledger: H + (V(z[n+1]) + V(z[n]))/2 is invariant.
            const double e = bank.energy_j() + 0.25 * spring * (z_next * z_next + z_cur * z_cur);
            if (e0 < 0.0) {
                e0 = e;
            }
            energy_drift = std::max(energy_drift, std::abs(e - e0) / e0);
            double p = 0.0;
            for (std::size_t n = 0; n < modes; ++n) {
                p += q0[n] * bank.current()[n];
            }
            projection.push_back(p);
        }
        return ver::single_sinusoid_frequency_hz(projection, fs);
    };
    for (std::size_t which = 0; which < 3; ++which) {
        const double f_exact = k_exact[which] * c / (2.0 * pi);
        std::vector<double> errors;
        const std::array<std::size_t, 4> counts{16, 32, 64, 128};
        for (std::size_t modes : counts) {
            double drift = 0.0;
            const double f = measure(modes, which, 96000.0, drift);
            errors.push_back(std::abs(f / f_exact - 1.0));
            std::cout << "coupled mode " << which + 1 << " exact=" << f_exact << " Hz modes=" << modes
                      << " simulated=" << f << " Hz relative error=" << errors.back()
                      << " energy drift=" << drift << '\n';
            check(drift < 1.0e-10, "lossless string+spring energy is conserved");
        }
        for (std::size_t i = 1; i < errors.size(); ++i) {
            const double order = std::log2(errors[i - 1] / errors[i]);
            std::cout << "  observed order " << counts[i - 1] << "->" << counts[i] << " = " << order << '\n';
            check(errors[i] < errors[i - 1], "eigenfrequency error decreases with retained modes");
        }
        check(std::log2(errors[2] / errors[3]) > 0.8, "truncation error converges at least first order");
        check(errors.back() < 2.0e-3, "128-mode coupled eigenfrequency within 0.2 % of the continuum");
        // Time step is not the limiting error: doubling fs changes the estimate far less
        // than the truncation error at the same mode count.
        double drift = 0.0;
        const double f96 = measure(64, which, 96000.0, drift);
        const double f192 = measure(64, which, 192000.0, drift);
        std::cout << "  64 modes: |f(96k) - f(192k)|/f=" << std::abs(f96 / f192 - 1.0) << '\n';
        check(std::abs(f96 / f192 - 1.0) < 0.1 * errors[2], "temporal error is below truncation error");
    }
}

void invalid_inputs() {
    const double nan = std::numeric_limits<double>::quiet_NaN();
    const double inf = std::numeric_limits<double>::infinity();
    const double dt = 1.0 / 48000.0, w = 2.0 * pi * 440.0;
    for (double mass : {0.0, -1.0, nan, inf}) {
        rejected([&] { (void)nm::modal_coefficients(mass, w, 0.0, dt); }, "invalid mass rejected");
    }
    for (double omega : {0.0, -w, nan, inf}) {
        rejected([&] { (void)nm::modal_coefficients(1.0, omega, 0.0, dt); }, "invalid omega rejected");
    }
    for (double decay : {-1.0e-9, nan, inf, w, 1.5 * w}) {
        rejected([&] { (void)nm::modal_coefficients(1.0, w, decay, dt); },
                 "negative, nonfinite or non-underdamped decay rejected");
    }
    for (double step : {0.0, -dt, nan, inf}) {
        rejected([&] { (void)nm::modal_coefficients(1.0, w, 0.0, step); }, "invalid dt rejected");
    }
    // wd*dt >= pi: exactly at and above Nyquist.
    rejected([&] { (void)nm::modal_coefficients(1.0, pi / dt, 0.0, dt); }, "Nyquist mode rejected");
    rejected([&] { (void)nm::modal_coefficients(1.0, 1.2 * pi / dt, 0.0, dt); }, "super-Nyquist rejected");
    (void)nm::modal_coefficients(1.0, 0.999 * pi / dt, 0.0, dt); // just below is admissible

    nm::ModalBank bank;
    const std::array<double, 2> mass{0.01, 0.02}, omega{w, 2.0 * w}, sigma{1.0, 2.0};
    bank.prepare(mass, omega, sigma, dt);
    const std::array<double, 2> q{1.0e-3, -5.0e-4};
    bank.set_displacement(q);
    bank.predict();
    bank.commit();
    const double energy = bank.energy_j();
    const std::vector<double> before(bank.current().begin(), bank.current().end());
    const std::array<double, 3> m3{0.01, 0.02, 0.03}, w3{w, 2.0 * w, 3.0 * w};
    const std::array<double, 3> bad_sigma{1.0, 2.0, 3.0 * w}; // last mode overdamped
    rejected([&] { bank.prepare(m3, w3, bad_sigma, dt); }, "bank rejects one invalid mode");
    const std::array<double, 1> short_array{0.01};
    rejected([&] { bank.prepare(m3, w3, short_array, dt); }, "bank rejects unequal array lengths");
    rejected([&] { bank.prepare({}, {}, {}, -dt); }, "empty bank still rejects an invalid step");
    check(bank.size() == 2 && bank.time_step_s() == dt, "failed prepare leaves the bank prepared");
    check(std::equal(before.begin(), before.end(), bank.current().begin()), "failed prepare keeps state");
    near(bank.energy_j(), energy, 0.0, 0.0, "failed prepare keeps energy");

    nm::SmallNewtonSolver solver;
    solver.prepare(3);
    rejected([&] { solver.prepare(0); }, "zero Newton capacity rejected");
    rejected([&] { solver.prepare(257); }, "excessive Newton capacity rejected");
    check(solver.capacity() == 3, "failed Newton prepare keeps capacity");
}

} // namespace

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "usage: runner <ctest-name>\n";
        return 2;
    }
    const std::string_view requested(argv[1]);
    struct Case {
        std::string_view name;
        void (*run)();
    };
    const std::array<Case, 10> tests{{
        {"solver.modal_exact_free_response", modal_exact_free_response},
        {"solver.modal_energy_identity", modal_energy_identity},
        {"solver.modal_forced_convergence", modal_forced_convergence},
        {"solver.sine_shape_accuracy", sine_shape_accuracy},
        {"solver.discrete_gradient_identity", discrete_gradient_identity},
        {"solver.contact_law_monotonicity", contact_law_monotonicity},
        {"solver.newton_scalar_oracle", newton_scalar_oracle},
        {"solver.newton_coupled_oracle", newton_coupled_oracle},
        {"solver.point_spring_eigen_oracle", point_spring_eigen_oracle},
        {"solver.invalid_inputs", invalid_inputs},
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
    std::cerr << "unknown test: " << requested << '\n';
    return 2;
}
