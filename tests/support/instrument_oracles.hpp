#pragma once

// Independent analytic oracles for the setar::instrument prototype. Verification only:
// nothing here includes or calls the instrument libraries, so a test that compares a
// solver against these functions does not compute its expectation with the code under
// test. All quantities are SI; every function throws std::runtime_error when its own
// preconditions fail rather than returning a fabricated value.

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <functional>
#include <limits>
#include <numbers>
#include <span>
#include <stdexcept>
#include <vector>

namespace setar::verification {

inline constexpr double oracle_pi = std::numbers::pi_v<double>;

// Free response of m*(x'' + 2*s*x' + w^2*x) = 0 (s < w) through x(t0) = x0 and
// x(t1) = x1 (t1 != t0, not separated by a multiple of the damped half period).
// Returns x(t). Derived directly from x = exp(-s t)(C1 cos wd t + C2 sin wd t).
struct DampedOscillator {
    double sigma = 0.0;
    double omega_d = 0.0;
    double c1 = 0.0;
    double c2 = 0.0;
    [[nodiscard]] double operator()(double t) const {
        return std::exp(-sigma * t) * (c1 * std::cos(omega_d * t) + c2 * std::sin(omega_d * t));
    }
};

inline DampedOscillator damped_oscillator_through(double omega, double sigma, double t0, double x0,
                                                  double t1, double x1) {
    if (!(omega > sigma) || !(sigma >= 0.0)) {
        throw std::runtime_error("oracle requires an underdamped oscillator");
    }
    DampedOscillator o;
    o.sigma = sigma;
    o.omega_d = std::sqrt(omega * omega - sigma * sigma);
    // [cos(wd t0) sin(wd t0); cos(wd t1) sin(wd t1)] * [c1; c2] = [x0 e^{s t0}; x1 e^{s t1}]
    const double a11 = std::cos(o.omega_d * t0), a12 = std::sin(o.omega_d * t0);
    const double a21 = std::cos(o.omega_d * t1), a22 = std::sin(o.omega_d * t1);
    const double r1 = x0 * std::exp(sigma * t0), r2 = x1 * std::exp(sigma * t1);
    const double det = a11 * a22 - a12 * a21;
    if (std::abs(det) < 1.0e-14) {
        throw std::runtime_error("oracle sample instants do not determine the response");
    }
    o.c1 = (r1 * a22 - a12 * r2) / det;
    o.c2 = (a11 * r2 - a21 * r1) / det;
    return o;
}

// Response of m*(x'' + 2*s*x' + w^2*x) = F0*sin(W t) with x(0) = x'(0) = 0 (W != w).
struct SinusoidallyForcedOscillator {
    double sigma = 0.0, omega_d = 0.0, forcing = 0.0;
    double a = 0.0, b = 0.0, c1 = 0.0, c2 = 0.0;
    [[nodiscard]] double operator()(double t) const {
        return a * std::sin(forcing * t) + b * std::cos(forcing * t) +
               std::exp(-sigma * t) * (c1 * std::cos(omega_d * t) + c2 * std::sin(omega_d * t));
    }
};

inline SinusoidallyForcedOscillator sinusoidally_forced_oscillator(double mass, double omega,
                                                                   double sigma, double force,
                                                                   double forcing_omega) {
    if (!(omega > sigma) || !(sigma >= 0.0) || !(mass > 0.0)) {
        throw std::runtime_error("forced oracle requires an underdamped positive-mass oscillator");
    }
    SinusoidallyForcedOscillator o;
    o.sigma = sigma;
    o.omega_d = std::sqrt(omega * omega - sigma * sigma);
    o.forcing = forcing_omega;
    const double d = omega * omega - forcing_omega * forcing_omega;
    const double e = 2.0 * sigma * forcing_omega;
    const double denominator = d * d + e * e;
    if (denominator == 0.0) {
        throw std::runtime_error("undamped resonant forcing has no bounded particular solution");
    }
    o.a = force / mass * d / denominator;
    o.b = -force / mass * e / denominator;
    o.c1 = -o.b;
    o.c2 = (sigma * o.c1 - o.a * forcing_omega) / o.omega_d;
    return o;
}

// Ideal string (tension T, density mu, length L, fixed ends) with a grounded linear
// spring K at x0 in (0, L). A nonzero eigenpair satisfies continuity at x0 and the slope
// jump T*(y'(x0+) - y'(x0-)) = K*y(x0), i.e.
//     h(k) = T*k*(cot(k*x0) + cot(k*(L - x0))) + K = 0,   omega = k*sqrt(T/mu).
// When sin(k x0) or sin(k (L - x0)) vanishes the mode may have a node at the spring
// and the cot form is singular; callers must choose x0/L irrational-looking (e.g. 0.37).
// Returns the first `count` roots in increasing k by sign-change bracketing and
// bisection on h, excluding the poles of cot.
inline std::vector<double> point_spring_wavenumbers(double tension, double length, double x0,
                                                    double spring, std::size_t count) {
    if (!(tension > 0.0) || !(length > 0.0) || !(x0 > 0.0 && x0 < length) || !(spring >= 0.0)) {
        throw std::runtime_error("invalid point-spring oracle input");
    }
    const auto h = [&](double k) {
        return tension * k * (1.0 / std::tan(k * x0) + 1.0 / std::tan(k * (length - x0))) + spring;
    };
    std::vector<double> roots;
    // Poles of cot(k x0) and cot(k (L - x0)) split the axis; between neighbouring poles h
    // is continuous and strictly decreasing (d/dk of k*cot(k a) < 0 away from poles for
    // the relevant branch), so each interval contains at most one root.
    std::vector<double> poles;
    const double k_max = oracle_pi * static_cast<double>(count + 2) / std::min(x0, length - x0);
    for (double a : {x0, length - x0}) {
        for (std::size_t j = 1; oracle_pi * static_cast<double>(j) / a < k_max; ++j) {
            poles.push_back(oracle_pi * static_cast<double>(j) / a);
        }
    }
    poles.push_back(k_max);
    std::sort(poles.begin(), poles.end());
    double left = 0.0;
    for (double right : poles) {
        if (roots.size() == count) {
            break;
        }
        const double width = right - left;
        if (width <= 1.0e-9 * right) {
            left = right;
            continue;
        }
        double lo = left + 1.0e-12 * width;
        double hi = right - 1.0e-12 * width;
        double hlo = h(lo);
        double hhi = h(hi);
        if (lo == 0.0 || !std::isfinite(hlo)) {
            lo = left + 1.0e-9 * width;
            hlo = h(lo);
        }
        if (std::isfinite(hlo) && std::isfinite(hhi) && hlo * hhi < 0.0) {
            for (int iteration = 0; iteration < 200; ++iteration) {
                const double mid = 0.5 * (lo + hi);
                const double hm = h(mid);
                if ((hm > 0.0) == (hlo > 0.0)) {
                    lo = mid;
                    hlo = hm;
                } else {
                    hi = mid;
                }
            }
            roots.push_back(0.5 * (lo + hi));
        }
        left = right;
    }
    if (roots.size() != count) {
        throw std::runtime_error("point-spring oracle did not bracket the requested roots");
    }
    return roots;
}

// Frequency (Hz) of a single real sinusoid x[k] = A cos(theta k + phi) from the exact
// recurrence x[k+1] + x[k-1] = 2 cos(theta) x[k], solved by least squares.
// Throws if the signal is zero or the estimate falls outside (0, fs/2).
inline double single_sinusoid_frequency_hz(std::span<const double> x, double sample_rate_hz) {
    if (x.size() < 3) {
        throw std::runtime_error("frequency oracle needs at least three samples");
    }
    long double numerator = 0.0L, denominator = 0.0L;
    for (std::size_t k = 1; k + 1 < x.size(); ++k) {
        numerator += static_cast<long double>(x[k]) * (x[k + 1] + x[k - 1]);
        denominator += static_cast<long double>(x[k]) * x[k];
    }
    if (!(denominator > 0.0L)) {
        throw std::runtime_error("frequency oracle received a zero signal");
    }
    const double c = static_cast<double>(numerator / (2.0L * denominator));
    if (!(c > -1.0 && c < 1.0)) {
        throw std::runtime_error("frequency oracle estimate is outside (0, Nyquist)");
    }
    return std::acos(c) * sample_rate_hz / (2.0 * oracle_pi);
}

// Composite Simpson rule on [a, b] with an even number of intervals.
inline double simpson(const std::function<double(double)>& f, double a, double b,
                      std::size_t intervals) {
    if (intervals < 2 || intervals % 2 != 0) {
        throw std::runtime_error("Simpson rule needs an even interval count >= 2");
    }
    const double h = (b - a) / static_cast<double>(intervals);
    double sum = f(a) + f(b);
    for (std::size_t i = 1; i < intervals; ++i) {
        sum += f(a + h * static_cast<double>(i)) * (i % 2 == 1 ? 4.0 : 2.0);
    }
    return sum * h / 3.0;
}

} // namespace setar::verification
