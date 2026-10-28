#include "setar/numerical_prototype.hpp"

#include <cmath>
#include <cstddef>
#include <exception>
#include <iomanip>
#include <iostream>

// Reproducible *synthetic* analytical comparisons, with frequencies in Hz.
// A sweep is not proof of a best production solver, a CPU benchmark, or a
// measured Iranian setar. CSV is intended for checked-in report tooling.
int main() {
    try {
        std::cout << std::setprecision(17);
        std::cout << "grid_nodes,rate_hz,mode_number,continuum_hz,spatial_hz,midpoint_hz,"
                     "spatial_abs_error_hz,temporal_abs_error_hz,total_abs_error_hz,"
                     "dense_matrix_entries,leapfrog_cfl_min_rate_hz,leapfrog_stable,"
                     "leapfrog_hz,leapfrog_abs_error_hz\n";
        for (std::size_t nodes : {std::size_t{8}, std::size_t{16}, std::size_t{32},
                                  std::size_t{64}, std::size_t{128}}) {
            for (double sample_rate : {8000.0, 48000.0, 96000.0, 192000.0}) {
                for (std::size_t mode : {std::size_t{1}, std::size_t{3}, std::size_t{5}}) {
                    setar::experimental::StringGrid g;
                    g.interior_nodes = nodes;
                    g.sample_rate_hz = sample_rate;
                    const double continuum = setar::experimental::continuum_frequency_hz(g, mode);
                    const double spatial = setar::experimental::spatial_grid_frequency_hz(g, mode);
                    const double midpoint =
                        setar::experimental::midpoint_frequency_hz(spatial, sample_rate);
                    const double cfl_rate = setar::experimental::leapfrog_minimum_stable_rate_hz(g);
                    const bool explicit_allowed = sample_rate > 1.000000000001 * cfl_rate;
                    std::cout << nodes << ',' << sample_rate << ',' << mode << ','
                              << continuum << ',' << spatial << ',' << midpoint << ','
                              << std::abs(continuum - spatial) << ','
                              << std::abs(spatial - midpoint) << ','
                              << std::abs(continuum - midpoint) << ','
                              << nodes * nodes << ',' << cfl_rate << ',' << explicit_allowed << ',';
                    if (explicit_allowed) {
                        const double explicit_frequency =
                            setar::experimental::leapfrog_frequency_hz(spatial, sample_rate);
                        std::cout << explicit_frequency << ','
                                  << std::abs(explicit_frequency - spatial);
                    } else {
                        // Empty, NOT zero: unstable explicit grids have no valid phase result.
                        std::cout << ',';
                    }
                    std::cout << '\n';
                }
            }
        }
    } catch (const std::exception& error) {
        std::cerr << "numerical comparison failed: " << error.what() << '\n';
        return 1;
    }
}
