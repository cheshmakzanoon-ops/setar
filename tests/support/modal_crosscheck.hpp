#pragma once

// Verification harness, NOT part of the engine: it deliberately depends on both the
// finite-difference prototype and the analytic modal reference so that neither library
// links the other. Shared by tests/numerical_prototype_tests.cpp and apps/loss_study.cpp
// so the CTest and the published CSV evidence use one definition of the comparison.

#include "setar/numerical_prototype.hpp"
#include "setar/reference.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <numbers>
#include <stdexcept>
#include <vector>

namespace setar::verification {

// Maximum |u_FD - u_modal| / max|u_modal| at an on-grid pickup over [0, duration_s].
//
// Both models solve the same band-limited initial-value problem on a simply supported
// string: the finite-difference grid starts from the analytic reference's own modal
// amplitudes (triangular pluck, 1 mm at 30% of the length, first `band_modes` sine
// modes) sampled at the nodes. The reference is exact in time and space, so the
// difference is discretization error plus any mismatch between `fd_loss` and
// `reference_loss`. The pickup at 25% of the length must coincide with a grid node
// ((nodes + 1) divisible by 4) so that observation adds no interpolation error.
// Throws std::runtime_error when that or another precondition fails.
inline double modal_reference_error(const experimental::StringLoss& fd_loss,
                                    const experimental::StringLoss& reference_loss,
                                    double bending_stiffness_n_m2, std::size_t nodes,
                                    double rate_hz, std::size_t band_modes, double duration_s) {
    constexpr double pickup = 0.25;
    constexpr double pluck_height_m = 1.0e-3;
    constexpr double pluck_position = 0.3;
    constexpr double pi = std::numbers::pi_v<double>;
    const double node_position = pickup * static_cast<double>(nodes + 1);
    if (std::abs(node_position - std::round(node_position)) >= 1.0e-12) {
        throw std::runtime_error("pickup must coincide with a grid node");
    }
    if (band_modes < 1 || band_modes > nodes) {
        throw std::runtime_error("band must be representable on the grid");
    }
    experimental::StringGrid g;
    g.interior_nodes = nodes;
    g.sample_rate_hz = rate_hz;
    g.bending_stiffness_n_m2 = bending_stiffness_n_m2;
    g.force_position_fraction = pickup;
    g.loss = fd_loss;
    reference::StringParameters p;
    p.bending_stiffness_n_m2 = bending_stiffness_n_m2;
    p.decay_constant_per_s = reference_loss.decay_constant_per_s;
    p.decay_wavenumber_squared_m2_per_s = reference_loss.decay_wavenumber_squared_m2_per_s;
    p.decay_frequency_squared_s = reference_loss.decay_frequency_squared_s;
    p.mode_count = band_modes;
    reference::ModalString analytic;
    analytic.prepare(p, rate_hz, pickup);
    analytic.pluck(pluck_height_m, pluck_position);
    experimental::MidpointStiffString string;
    string.prepare(g);
    std::vector<double> q(nodes, 0.0);
    const std::vector<double> v(nodes, 0.0);
    for (std::size_t mode = 1; mode <= band_modes; ++mode) {
        const double amplitude = analytic.modes()[mode - 1].initial_displacement_m;
        for (std::size_t i = 0; i < nodes; ++i) {
            q[i] += amplitude * std::sin(static_cast<double>(mode) * pi *
                                         static_cast<double>(i + 1) /
                                         static_cast<double>(nodes + 1));
        }
    }
    string.set_state(q, v);
    const std::size_t steps = static_cast<std::size_t>(duration_s * rate_hz);
    double largest_difference = 0.0;
    double largest_reference = 0.0;
    for (std::size_t k = 0; k <= steps; ++k) {
        const double exact = analytic.tick();
        const double numerical = string.port_displacement_m();
        largest_difference = std::max(largest_difference, std::abs(numerical - exact));
        largest_reference = std::max(largest_reference, std::abs(exact));
        (void)string.advance(0.0);
    }
    if (!(largest_reference > 0.0)) {
        throw std::runtime_error("reference waveform must be nonzero");
    }
    return largest_difference / largest_reference;
}

} // namespace setar::verification
