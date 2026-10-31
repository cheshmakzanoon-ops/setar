#include "setar/instrument/numerics.hpp"

#include <algorithm>
#include <cmath>
#include <numbers>
#include <stdexcept>
#include <string>

namespace setar::instrument::numerics {
namespace {

void require(bool valid, const char* message) {
    if (!valid) {
        throw std::invalid_argument(message);
    }
}

// One-sided power law V = K/(a+1) p^(a+1), p = max(0,-z).
double power_potential(double k, double alpha, double z) noexcept {
    const double p = -z;
    return p > 0.0 ? k / (alpha + 1.0) * std::pow(p, alpha + 1.0) : 0.0;
}
double power_first(double k, double alpha, double z) noexcept { // dV/dz
    const double p = -z;
    return p > 0.0 ? -k * std::pow(p, alpha) : 0.0;
}
double power_second(double k, double alpha, double z) noexcept { // d2V/dz2
    const double p = -z;
    return p > 0.0 ? k * alpha * std::pow(p, alpha - 1.0) : 0.0;
}

} // namespace

ModalCoefficients modal_coefficients(double mass_kg, double omega_rad_per_s, double decay_per_s,
                                     double dt_s) {
    require(std::isfinite(mass_kg) && mass_kg > 0.0, "modal mass must be finite and positive (kg)");
    require(std::isfinite(omega_rad_per_s) && omega_rad_per_s > 0.0,
            "modal angular frequency must be finite and positive (rad/s)");
    require(std::isfinite(decay_per_s) && decay_per_s >= 0.0,
            "modal decay must be finite and nonnegative (1/s)");
    require(decay_per_s < omega_rad_per_s, "mode must be underdamped (decay < angular frequency)");
    require(std::isfinite(dt_s) && dt_s > 0.0, "time step must be finite and positive (s)");
    const double wd = std::sqrt((omega_rad_per_s - decay_per_s) * (omega_rad_per_s + decay_per_s));
    require(wd * dt_s < std::numbers::pi, "damped modal frequency must be below Nyquist");
    const double sk = decay_per_s * dt_s;
    const double e = std::exp(-sk);
    ModalCoefficients c;
    c.mass_kg = mass_kg;
    c.a = 2.0 * e * std::cos(wd * dt_s);
    c.b = e * e;
    c.g = dt_s * dt_s * (1.0 + c.b) / (2.0 * mass_kg);
    c.sigma_tilde_per_s = std::tanh(sk) / dt_s;
    const double sh = std::sinh(0.5 * sk);
    const double sn = std::sin(0.5 * wd * dt_s);
    c.omega_tilde_sq = 4.0 * (sh * sh + sn * sn) / (std::cosh(sk) * dt_s * dt_s);
    return c;
}

void fill_sine_shape(std::span<double> out, double theta_rad) noexcept {
    const double c1 = std::cos(theta_rad);
    const double s1 = std::sin(theta_rad);
    double c = c1;
    double s = s1;
    for (std::size_t i = 0; i < out.size(); ++i) {
        out[i] = s;
        const double cn = c * c1 - s * s1;
        const double sn = s * c1 + c * s1;
        // Renormalize the rotation to keep |(c, s)| = 1 to roundoff over many modes.
        const double scale = 1.5 - 0.5 * (cn * cn + sn * sn);
        c = cn * scale;
        s = sn * scale;
    }
}

void ModalBank::prepare(std::span<const double> mass_kg, std::span<const double> omega_rad_per_s,
                        std::span<const double> decay_per_s, double dt_s) {
    require(mass_kg.size() == omega_rad_per_s.size() && mass_kg.size() == decay_per_s.size(),
            "modal bank arrays must have equal length");
    const std::size_t n = mass_kg.size();
    std::vector<double> a(n), b(n), g(n), m(n), st(n), wt(n);
    for (std::size_t i = 0; i < n; ++i) {
        const ModalCoefficients c = modal_coefficients(mass_kg[i], omega_rad_per_s[i], decay_per_s[i], dt_s);
        a[i] = c.a;
        b[i] = c.b;
        g[i] = c.g;
        m[i] = c.mass_kg;
        st[i] = c.sigma_tilde_per_s;
        wt[i] = c.omega_tilde_sq;
    }
    // Commit only after every coefficient validated (transactional prepare).
    a_ = std::move(a);
    b_ = std::move(b);
    g_ = std::move(g);
    m_ = std::move(m);
    sigma_tilde_ = std::move(st);
    omega_tilde_sq_ = std::move(wt);
    previous_.assign(n, 0.0);
    current_.assign(n, 0.0);
    next_.assign(n, 0.0);
    dt_ = dt_s;
}

void ModalBank::reset() noexcept {
    std::fill(previous_.begin(), previous_.end(), 0.0);
    std::fill(current_.begin(), current_.end(), 0.0);
    std::fill(next_.begin(), next_.end(), 0.0);
}

void ModalBank::predict() noexcept {
    for (std::size_t i = 0; i < a_.size(); ++i) {
        next_[i] = a_[i] * current_[i] - b_[i] * previous_[i];
    }
}

void ModalBank::add_force(std::span<const double> shape, double generalized_force) noexcept {
    for (std::size_t i = 0; i < next_.size(); ++i) {
        next_[i] += g_[i] * shape[i] * generalized_force;
    }
}

namespace {
double dot(std::span<const double> x, std::span<const double> y) noexcept {
    double out = 0.0;
    for (std::size_t i = 0; i < x.size(); ++i) {
        out += x[i] * y[i];
    }
    return out;
}
} // namespace

double ModalBank::gather_previous(std::span<const double> shape) const noexcept {
    return dot(previous_, shape);
}
double ModalBank::gather_current(std::span<const double> shape) const noexcept {
    return dot(current_, shape);
}
double ModalBank::gather_next(std::span<const double> shape) const noexcept {
    return dot(next_, shape);
}

double ModalBank::compliance(std::span<const double> s1, std::span<const double> s2) const noexcept {
    double out = 0.0;
    for (std::size_t i = 0; i < g_.size(); ++i) {
        out += g_[i] * s1[i] * s2[i];
    }
    return out;
}

double ModalBank::step_dissipation_j() const noexcept {
    double out = 0.0;
    for (std::size_t i = 0; i < g_.size(); ++i) {
        const double v = (next_[i] - previous_[i]) / (2.0 * dt_);
        out += 2.0 * m_[i] * sigma_tilde_[i] * dt_ * v * v;
    }
    return out;
}

void ModalBank::commit() noexcept {
    previous_.swap(current_);
    current_.swap(next_);
}

double ModalBank::energy_j() const noexcept {
    double out = 0.0;
    for (std::size_t i = 0; i < m_.size(); ++i) {
        const double v = (current_[i] - previous_[i]) / dt_;
        out += 0.5 * m_[i] * (v * v + omega_tilde_sq_[i] * current_[i] * previous_[i]);
    }
    return out;
}

void ModalBank::set_displacement(std::span<const double> q) noexcept {
    for (std::size_t i = 0; i < m_.size(); ++i) {
        previous_[i] = q[i];
        current_[i] = q[i];
        next_[i] = q[i];
    }
}

double channel_potential_j(const ChannelLaw& law, double z_m) noexcept {
    switch (law.kind) {
    case ChannelLawKind::OneSidedPower:
        return power_potential(law.stiffness, law.exponent, z_m);
    case ChannelLawKind::LinearSpring: {
        const double d = z_m - law.anchor_m;
        return 0.5 * law.stiffness * d * d;
    }
    case ChannelLawKind::Damper:
        return 0.0;
    }
    return 0.0;
}

ChannelForce channel_force(const ChannelLaw& law, double z_prev_m, double s_m, double dt_s) noexcept {
    double gradient = 0.0;   // (V(z+s) - V(z))/s
    double slope = 0.0;      // d gradient / ds
    switch (law.kind) {
    case ChannelLawKind::OneSidedPower: {
        const double z1 = z_prev_m + s_m;
        const double k = law.stiffness;
        const double alpha = law.exponent;
        if (std::abs(s_m) <= 1.0e-7 * (std::abs(z_prev_m) + std::abs(z1))) {
            // Secant -> tangent limit; avoids cancellation when s is tiny relative to z.
            const double zm = z_prev_m + 0.5 * s_m;
            gradient = power_first(k, alpha, zm);
            slope = 0.5 * power_second(k, alpha, zm);
        } else {
            gradient = (power_potential(k, alpha, z1) - power_potential(k, alpha, z_prev_m)) / s_m;
            slope = (power_first(k, alpha, z1) - gradient) / s_m;
        }
        break;
    }
    case ChannelLawKind::LinearSpring:
        gradient = law.stiffness * (z_prev_m - law.anchor_m + 0.5 * s_m);
        slope = 0.5 * law.stiffness;
        break;
    case ChannelLawKind::Damper:
        break;
    }
    const double r = law.damping_n_s_per_m / (2.0 * dt_s);
    return ChannelForce{-gradient - r * s_m, -slope - r};
}

void SmallNewtonSolver::prepare(std::size_t max_channels) {
    require(max_channels >= 1 && max_channels <= 256, "Newton capacity must be in [1, 256]");
    jacobian_.assign(max_channels * max_channels, 0.0);
    residual_.assign(max_channels, 0.0);
    delta_.assign(max_channels, 0.0);
    derivative_.assign(max_channels, 0.0);
    trial_s_.assign(max_channels, 0.0);
    trial_force_.assign(max_channels, 0.0);
    pivot_.assign(max_channels, 0);
    capacity_ = max_channels;
}

NewtonResult SmallNewtonSolver::solve(std::size_t m, std::span<const double> compliance,
                                      std::span<const double> b, std::span<const ChannelLaw> laws,
                                      std::span<const double> z_prev, double dt_s, std::span<double> s,
                                      std::span<double> force, std::uint32_t max_iterations,
                                      double tolerance_m) noexcept {
    NewtonResult result;
    if (m == 0) {
        result.converged = true;
        return result;
    }
    if (m > capacity_) {
        return result; // not converged: caller reports capacity, never silently truncates
    }
    // r = s - b - M*F(s); fills force/derivative for the current s. Returns max-norm.
    auto evaluate = [&](std::span<const double> trial, std::span<double> f, bool keep_derivative) {
        for (std::size_t c = 0; c < m; ++c) {
            const ChannelForce cf = channel_force(laws[c], z_prev[c], trial[c], dt_s);
            f[c] = cf.force_n;
            if (keep_derivative) {
                derivative_[c] = cf.derivative;
            }
        }
        double norm = 0.0;
        for (std::size_t r = 0; r < m; ++r) {
            double mf = 0.0;
            for (std::size_t c = 0; c < m; ++c) {
                mf += compliance[r * m + c] * f[c];
            }
            residual_[r] = trial[r] - b[r] - mf;
            norm = std::max(norm, std::abs(residual_[r]));
        }
        return norm;
    };

    double norm = evaluate(s, force, true);
    for (;;) {
        result.residual_m = norm;
        if (!std::isfinite(norm)) {
            result.finite = false;
            return result;
        }
        if (norm <= tolerance_m) {
            result.converged = true;
            return result;
        }
        if (result.iterations >= max_iterations) {
            return result;
        }
        ++result.iterations;
        // J = I - M*diag(dF/ds); solve J*delta = -r by partial-pivot elimination.
        for (std::size_t r = 0; r < m; ++r) {
            for (std::size_t c = 0; c < m; ++c) {
                jacobian_[r * m + c] = (r == c ? 1.0 : 0.0) - compliance[r * m + c] * derivative_[c];
            }
            delta_[r] = -residual_[r];
        }
        for (std::size_t col = 0; col < m; ++col) {
            std::size_t best = col;
            for (std::size_t r = col + 1; r < m; ++r) {
                if (std::abs(jacobian_[r * m + col]) > std::abs(jacobian_[best * m + col])) {
                    best = r;
                }
            }
            if (best != col) {
                for (std::size_t c = 0; c < m; ++c) {
                    std::swap(jacobian_[col * m + c], jacobian_[best * m + c]);
                }
                std::swap(delta_[col], delta_[best]);
            }
            const double pivot = jacobian_[col * m + col];
            if (pivot == 0.0 || !std::isfinite(pivot)) {
                result.finite = false;
                return result;
            }
            for (std::size_t r = col + 1; r < m; ++r) {
                const double factor = jacobian_[r * m + col] / pivot;
                if (factor != 0.0) {
                    for (std::size_t c = col; c < m; ++c) {
                        jacobian_[r * m + c] -= factor * jacobian_[col * m + c];
                    }
                    delta_[r] -= factor * delta_[col];
                }
            }
        }
        for (std::size_t r = m; r-- > 0;) {
            double sum = delta_[r];
            for (std::size_t c = r + 1; c < m; ++c) {
                sum -= jacobian_[r * m + c] * delta_[c];
            }
            delta_[r] = sum / jacobian_[r * m + r];
        }
        // Backtracking on the residual max-norm; accept the last trial regardless so the
        // iteration count stays bounded.
        double step = 1.0;
        double trial_norm = 0.0;
        for (int halving = 0; halving < 8; ++halving) {
            for (std::size_t c = 0; c < m; ++c) {
                trial_s_[c] = s[c] + step * delta_[c];
            }
            trial_norm = evaluate(std::span<const double>(trial_s_.data(), m),
                                  std::span<double>(trial_force_.data(), m), false);
            if (trial_norm < norm || !std::isfinite(norm)) {
                break;
            }
            step *= 0.5;
        }
        for (std::size_t c = 0; c < m; ++c) {
            s[c] = trial_s_[c];
        }
        norm = evaluate(s, force, true);
    }
}

} // namespace setar::instrument::numerics
