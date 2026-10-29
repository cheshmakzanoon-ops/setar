#include "setar/numerical_prototype.hpp"
#include "support/modal_crosscheck.hpp"

#include <array>
#include <cmath>
#include <cstddef>
#include <exception>
#include <iomanip>
#include <iostream>
#include <numbers>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

// Reproducible SYNTHETIC loss-law study for the linear simply supported stiff string.
// Nothing here is a measurement of an Iranian setar, a fitted loss law, or a claim of
// acoustic fidelity. Output is CSV on stdout for tools/generate_loss_report.py.
//
//   setar_loss_study decay [--max-nodes N]
//       per-mode amplitude decay: continuum law, midpoint amplification-root oracle, and
//       the value measured from a simulated, projected mode, across joint (h, dt) refinement
//   setar_loss_study waveform [--max-nodes N]
//       finite-difference waveform error against the analytic modal reference, matched
//       and deliberately mismatched
// --max-nodes drops refinement levels with more interior nodes (default: all four, up
// to 127). The dense O(N^2)-per-step solver makes the finest level cost about eight
// times the previous one, so the CTest uses 63 and the CI evidence step uses the default.
namespace {
using setar::experimental::StringGrid;
using setar::experimental::StringLoss;
constexpr double pi = std::numbers::pi_v<double>;

struct NamedLaw {
    std::string_view name;
    StringLoss loss;
};

// Loss magnitudes are arbitrary synthetic test values chosen so each term matters
// within the first eight modes of the default fixture; they are not material data.
constexpr std::array<NamedLaw, 4> laws{{{"constant", {3.0, 0.0, 0.0}},
                                       {"wavenumber_squared", {0.0, 0.03, 0.0}},
                                       {"frequency_squared", {0.0, 0.0, 5.0e-7}},
                                       {"combined", {3.0, 0.03, 5.0e-7}}}};

// Joint refinement of the spatial step and the time step: both are halved per level.
constexpr std::array<std::pair<std::size_t, double>, 4> levels{
    {{15, 24000.0}, {31, 48000.0}, {63, 96000.0}, {127, 192000.0}}};

std::vector<double> unit_sine_mode(std::size_t nodes, std::size_t mode) {
    std::vector<double> shape(nodes);
    for (std::size_t i = 0; i < nodes; ++i) {
        shape[i] = std::sqrt(2.0 / static_cast<double>(nodes + 1)) *
                   std::sin(static_cast<double>(mode) * pi * static_cast<double>(i + 1) /
                            static_cast<double>(nodes + 1));
    }
    return shape;
}

void decay_table(std::size_t max_nodes) {
    std::cout << "law,decay_constant_per_s,decay_wavenumber_squared_m2_per_s,"
                 "decay_frequency_squared_s,grid_nodes,rate_hz,mode_number,continuum_decay_per_s,"
                 "grid_decay_per_s,oracle_decay_per_s,measured_decay_per_s,continuum_hz,"
                 "oracle_hz,measured_hz,oracle_vs_continuum_decay_rel,"
                 "measured_vs_oracle_decay_rel,fit_residual,regressor_condition\n";
    for (const NamedLaw& law : laws) {
        for (const auto& [nodes, rate] : levels) {
            if (nodes > max_nodes) {
                continue;
            }
            for (std::size_t mode : {std::size_t{1}, std::size_t{2}, std::size_t{3},
                                     std::size_t{4}, std::size_t{6}, std::size_t{8}}) {
                StringGrid g;
                g.interior_nodes = nodes;
                g.sample_rate_hz = rate;
                g.loss = law.loss;
                const double continuum = setar::experimental::continuum_decay_rate_per_s(g, mode);
                const double grid = setar::experimental::spatial_grid_decay_rate_per_s(g, mode);
                const auto oracle = setar::experimental::midpoint_damped_roots(g, mode);
                setar::experimental::MidpointStiffString string;
                string.prepare(g);
                const auto shape = unit_sine_mode(nodes, mode);
                std::vector<double> q(nodes);
                const std::vector<double> v(nodes, 0.0);
                for (std::size_t i = 0; i < nodes; ++i) {
                    q[i] = 1.0e-4 * shape[i];
                }
                string.set_state(q, v);
                // Three oscillation periods: the same window at every level.
                const std::size_t samples =
                    static_cast<std::size_t>(std::ceil(3.0 * rate / oracle.frequency_hz));
                std::vector<double> series;
                series.reserve(samples);
                for (std::size_t k = 0; k < samples; ++k) {
                    double coordinate = 0.0;
                    for (std::size_t i = 0; i < nodes; ++i) {
                        coordinate += shape[i] * string.displacement_m()[i];
                    }
                    series.push_back(coordinate);
                    if (k + 1 < samples) {
                        (void)string.advance(0.0);
                    }
                }
                const auto measured = setar::experimental::estimate_decay(series, rate);
                const double continuum_hz = setar::experimental::continuum_frequency_hz(g, mode);
                std::cout << law.name << ',' << law.loss.decay_constant_per_s << ','
                          << law.loss.decay_wavenumber_squared_m2_per_s << ','
                          << law.loss.decay_frequency_squared_s << ',' << nodes << ',' << rate
                          << ',' << mode << ',' << continuum << ',' << grid << ','
                          << oracle.decay_rate_per_s << ',' << measured.decay_rate_per_s << ','
                          << continuum_hz << ',' << oracle.frequency_hz << ','
                          << measured.frequency_hz << ','
                          << std::abs(oracle.decay_rate_per_s / continuum - 1.0) << ','
                          << std::abs(measured.decay_rate_per_s / oracle.decay_rate_per_s - 1.0)
                          << ',' << measured.residual_rms_over_signal_rms << ','
                          << measured.regressor_condition << '\n';
            }
        }
    }
}

void waveform_table(std::size_t max_nodes) {
    std::cout << "law,reference_law,bending_stiffness_n_m2,band_modes,grid_nodes,rate_hz,"
                 "duration_s,relative_linf_error\n";
    constexpr double duration = 0.03;
    const StringLoss none{};
    const auto emit = [&](std::string_view name, std::string_view reference_name,
                          const StringLoss& fd, const StringLoss& reference, double stiffness,
                          std::size_t band, std::size_t nodes, double rate) {
        std::cout << name << ',' << reference_name << ',' << stiffness << ',' << band << ','
                  << nodes << ',' << rate << ',' << duration << ','
                  << setar::verification::modal_reference_error(fd, reference, stiffness, nodes,
                                                                rate, band, duration)
                  << '\n';
    };
    // Matched reference: same loss law on both sides. Band 3 is resolved on every level;
    // band 6 is NOT resolved on the coarsest grids and is kept to show that boundary.
    for (const auto& [nodes, rate] : levels) {
        if (nodes > max_nodes) {
            continue;
        }
        emit("lossless", "same", none, none, 0.000015, 3, nodes, rate);
        for (const NamedLaw& law : laws) {
            emit(law.name, "same", law.loss, law.loss, 0.000015, 3, nodes, rate);
        }
        emit("combined", "same", laws[3].loss, laws[3].loss, 0.00008, 3, nodes, rate);
        emit("lossless", "same", none, none, 0.000015, 6, nodes, rate);
        emit("combined", "same", laws[3].loss, laws[3].loss, 0.000015, 6, nodes, rate);
        // Deliberately wrong references for the combined law: the comparison must reject them.
        emit("combined", "zero_loss", laws[3].loss, none, 0.000015, 3, nodes, rate);
        emit("combined", "constant_only", laws[3].loss, laws[0].loss, 0.000015, 3, nodes, rate);
    }
}
} // namespace

int main(int argc, char** argv) {
    std::size_t max_nodes = 127;
    if (argc == 4 && std::string_view(argv[2]) == "--max-nodes") {
        try {
            max_nodes = static_cast<std::size_t>(std::stoul(argv[3]));
        } catch (const std::exception&) {
            std::cerr << "--max-nodes needs a positive integer\n";
            return 2;
        }
    } else if (argc != 2) {
        std::cerr << "usage: setar_loss_study decay|waveform [--max-nodes N]\n";
        return 2;
    }
    if (max_nodes < levels.front().first) {
        std::cerr << "--max-nodes must admit at least the coarsest level (15)\n";
        return 2;
    }
    const std::string_view table(argv[1]);
    try {
        std::cout << std::setprecision(17);
        if (table == "decay") {
            decay_table(max_nodes);
        } else if (table == "waveform") {
            waveform_table(max_nodes);
        } else {
            std::cerr << "unknown table: " << table << '\n';
            return 2;
        }
    } catch (const std::exception& error) {
        std::cerr << "loss study failed: " << error.what() << '\n';
        return 1;
    }
    return 0;
}
