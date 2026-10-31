// Independent verification of the synthetic prototype body (src/instrument/body_model.cpp).
// Oracles are computed here from first principles: engineering-constant algebra, a
// finite-difference orthotropic plate eigen-solve, a closed-form two-degree-of-freedom
// plate/Helmholtz system and an analytic plate modal sum. The runner receives the full
// CTest name as argv[1].
#include "setar/instrument/body_model.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <complex>
#include <cstddef>
#include <exception>
#include <functional>
#include <iostream>
#include <limits>
#include <numbers>
#include <random>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {
using namespace setar::instrument;
constexpr double pi = std::numbers::pi_v<double>;
constexpr double eps = std::numeric_limits<double>::epsilon();

void check(bool condition, std::string_view message) {
    if (!condition) {
        throw std::runtime_error(std::string(message));
    }
}

void near(double actual, double expected, double atol, double rtol, std::string_view message) {
    const double limit = atol + rtol * std::abs(expected);
    if (!std::isfinite(actual) || !std::isfinite(expected) || std::abs(actual - expected) > limit) {
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

// Test-local engineering-constant algebra (independent of flexural_rigidity()).
struct TestRigidity {
    double d11, d22, d12, d66;
};
TestRigidity test_rigidity(const OrthotropicPlate& p) {
    const double nu_rl =
        p.poisson_long_cross * p.youngs_modulus_cross_pa / p.youngs_modulus_long_pa;
    const double h = p.thickness_m;
    const double den = 1.0 - p.poisson_long_cross * nu_rl;
    return {p.youngs_modulus_long_pa * h * h * h / (12.0 * den),
            p.youngs_modulus_cross_pa * h * h * h / (12.0 * den),
            nu_rl * p.youngs_modulus_long_pa * h * h * h / (12.0 * den),
            p.shear_modulus_pa * h * h * h / 12.0};
}

double test_plate_omega(const OrthotropicPlate& p, double m, double n) {
    const TestRigidity d = test_rigidity(p);
    const double x = m * pi / p.length_m;
    const double y = n * pi / p.width_m;
    return std::sqrt((d.d11 * x * x * x * x + 2.0 * (d.d12 + 2.0 * d.d66) * x * x * y * y +
                      d.d22 * y * y * y * y) /
                     (p.density_kg_per_m3 * p.thickness_m));
}

// Small cyclic Jacobi for symmetric k x k matrices (test-local implementation).
std::vector<double> symmetric_eigenvalues(std::vector<double> a, std::size_t k,
                                          std::vector<double>* vectors = nullptr) {
    std::vector<double> v(k * k, 0.0);
    for (std::size_t i = 0; i < k; ++i) {
        v[i * k + i] = 1.0;
    }
    for (int sweep = 0; sweep < 200; ++sweep) {
        double off = 0.0;
        double diag = 0.0;
        for (std::size_t i = 0; i < k; ++i) {
            diag = std::max(diag, std::abs(a[i * k + i]));
            for (std::size_t j = i + 1; j < k; ++j) {
                off += a[i * k + j] * a[i * k + j];
            }
        }
        if (std::sqrt(off) <= 1e-15 * std::max(diag, 1e-300)) {
            break;
        }
        for (std::size_t p = 0; p < k; ++p) {
            for (std::size_t q = p + 1; q < k; ++q) {
                const double apq = a[p * k + q];
                if (apq == 0.0) {
                    continue;
                }
                const double phi = 0.5 * std::atan2(2.0 * apq, a[q * k + q] - a[p * k + p]);
                const double c = std::cos(phi);
                const double s = std::sin(phi);
                for (std::size_t r = 0; r < k; ++r) {
                    const double arp = a[r * k + p];
                    const double arq = a[r * k + q];
                    a[r * k + p] = c * arp - s * arq;
                    a[r * k + q] = s * arp + c * arq;
                }
                for (std::size_t r = 0; r < k; ++r) {
                    const double apr = a[p * k + r];
                    const double aqr = a[q * k + r];
                    a[p * k + r] = c * apr - s * aqr;
                    a[q * k + r] = s * apr + c * aqr;
                }
                for (std::size_t r = 0; r < k; ++r) {
                    const double vrp = v[r * k + p];
                    const double vrq = v[r * k + q];
                    v[r * k + p] = c * vrp - s * vrq;
                    v[r * k + q] = s * vrp + c * vrq;
                }
            }
        }
    }
    std::vector<double> out(k);
    for (std::size_t i = 0; i < k; ++i) {
        out[i] = a[i * k + i];
    }
    if (vectors != nullptr) {
        *vectors = std::move(v);
    }
    return out;
}

OrthotropicPlate reference_plate() { return synthetic_setar_body_design().plate; }

void materials_reciprocity() {
    std::mt19937_64 rng(20261031);
    std::uniform_real_distribution<double> el(5.0e9, 16.0e9);
    std::uniform_real_distribution<double> ratio(0.04, 0.2);
    std::uniform_real_distribution<double> nu(0.1, 0.6);
    std::vector<OrthotropicPlate> plates{reference_plate()};
    for (int i = 0; i < 200; ++i) {
        OrthotropicPlate p = reference_plate();
        p.youngs_modulus_long_pa = el(rng);
        p.youngs_modulus_cross_pa = p.youngs_modulus_long_pa * ratio(rng);
        p.poisson_long_cross = nu(rng);
        plates.push_back(p);
    }
    for (const OrthotropicPlate& p : plates) {
        const double expected_nu_rl =
            p.poisson_long_cross * p.youngs_modulus_cross_pa / p.youngs_modulus_long_pa;
        near(poisson_cross_long(p), expected_nu_rl, 0.0, 4.0 * eps,
             "minor Poisson ratio from reciprocity");
        const std::array<double, 9> s = plane_stress_compliance(p);
        near(s[1], s[3], 0.0, 8.0 * eps, "plane-stress compliance is symmetric (S12 = S21)");
        near(s[3], -p.poisson_long_cross / p.youngs_modulus_long_pa, 0.0, 4.0 * eps,
             "S21 = -nu_LR/E_L");
        check(s[2] == 0.0 && s[5] == 0.0 && s[6] == 0.0 && s[7] == 0.0,
              "no normal-shear coupling in material axes");
        const FlexuralRigidity d = flexural_rigidity(p);
        const TestRigidity t = test_rigidity(p);
        near(d.d11, t.d11, 0.0, 1e-14, "D11");
        near(d.d22, t.d22, 0.0, 1e-14, "D22");
        near(d.d66, t.d66, 0.0, 1e-14, "D66");
        // D12 = nu_LR*D22 must equal nu_RL*D11: an identity only true under reciprocity.
        near(d.d12, t.d12, 0.0, 1e-13, "D12 = nu_LR D22 = nu_RL D11");
    }
    const OrthotropicPlate p = reference_plate();
    const double nu_rl =
        p.poisson_long_cross * p.youngs_modulus_cross_pa / p.youngs_modulus_long_pa;
    check(reciprocal_poisson_pair(p.youngs_modulus_long_pa, p.youngs_modulus_cross_pa,
                                  p.poisson_long_cross, nu_rl, 1e-12),
          "a reciprocal Poisson pair is accepted");
    // A contradictory reported pair (both ratios equal although E_L/E_R ~ 10) must be rejected.
    check(!reciprocal_poisson_pair(p.youngs_modulus_long_pa, p.youngs_modulus_cross_pa, 0.37, 0.37,
                                   0.05),
          "a non-reciprocal Poisson pair is rejected");
    check(!reciprocal_poisson_pair(p.youngs_modulus_long_pa, p.youngs_modulus_cross_pa,
                                   p.poisson_long_cross, 1.05 * nu_rl, 0.01),
          "a 5% reciprocity defect is rejected at 1% tolerance");
    check(!reciprocal_poisson_pair(-1.0, 1.0, 0.3, 0.3, 1.0), "nonpositive modulus is rejected");
    check(!reciprocal_poisson_pair(1.0, 1.0, std::nan(""), 0.3, 1.0),
          "nonfinite ratio is rejected");
    check(!reciprocal_poisson_pair(1.0, 1.0, 1.2, 1.2, 1e-12),
          "a reciprocal but indefinite pair (nu^2 >= 1 for equal moduli) is rejected");
}

void materials_positive_energy() {
    std::mt19937_64 rng(7);
    std::normal_distribution<double> stress(0.0, 1.0e6);
    const OrthotropicPlate p = reference_plate();
    const std::array<double, 9> s = plane_stress_compliance(p);
    // Leading principal minors of the compliance (and therefore of its inverse) are positive.
    check(s[0] > 0.0, "S11 > 0");
    check(s[0] * s[4] - s[1] * s[3] > 0.0, "2x2 minor > 0");
    check((s[0] * s[4] - s[1] * s[3]) * s[8] > 0.0, "det S > 0");
    for (int i = 0; i < 2000; ++i) {
        const std::array<double, 3> sigma{stress(rng), stress(rng), stress(rng)};
        double energy = 0.0;
        for (std::size_t r = 0; r < 3; ++r) {
            for (std::size_t c = 0; c < 3; ++c) {
                energy += sigma[r] * s[r * 3 + c] * sigma[c];
            }
        }
        check(energy > 0.0, "complementary strain energy is positive for nonzero stress");
    }
    const FlexuralRigidity d = flexural_rigidity(p);
    check(d.d11 > 0.0 && d.d22 > 0.0 && d.d66 > 0.0 && d.d11 * d.d22 - d.d12 * d.d12 > 0.0,
          "bending stiffness tensor is positive definite");
    // Admissibility boundary |nu_LR| < sqrt(E_L/E_R).
    const double limit = std::sqrt(p.youngs_modulus_long_pa / p.youngs_modulus_cross_pa);
    OrthotropicPlate inside = p;
    inside.poisson_long_cross = 0.99 * limit;
    (void)flexural_rigidity(inside);
    for (double factor : {1.0, 1.01, 2.0}) {
        OrthotropicPlate bad = p;
        bad.poisson_long_cross = factor * limit;
        rejected([&] { (void)flexural_rigidity(bad); },
                 "indefinite orthotropic constants rejected");
        BodyDesign design = synthetic_setar_body_design();
        design.plate = bad;
        rejected([&] { validate(design); }, "design with indefinite constants rejected");
        rejected([&] { (void)build_body(design); }, "build_body rejects indefinite constants");
    }
    OrthotropicPlate negative_shear = p;
    negative_shear.shear_modulus_pa = -1.0e8;
    rejected([&] { (void)flexural_rigidity(negative_shear); }, "negative shear modulus rejected");
}

// Simply supported orthotropic plate by second-order finite differences:
// A = D11 Lx^2 (x) I + 2(D12 + 2 D66) Lx (x) Ly + D22 I (x) Ly^2, Lx = -delta_xx (Dirichlet),
// whose square is the simply supported (w = w_xx = 0) fourth difference. The lowest
// eigenvalues are found by subspace iteration with a dense Cholesky factorization.
std::vector<double> fd_plate_frequencies(const OrthotropicPlate& p, std::size_t nx, std::size_t ny,
                                         std::size_t wanted) {
    const TestRigidity d = test_rigidity(p);
    const double hx = p.length_m / static_cast<double>(nx + 1);
    const double hy = p.width_m / static_cast<double>(ny + 1);
    auto second = [](std::size_t n, double h) {
        std::vector<double> l(n * n, 0.0);
        for (std::size_t i = 0; i < n; ++i) {
            l[i * n + i] = 2.0 / (h * h);
            if (i > 0) {
                l[i * n + i - 1] = -1.0 / (h * h);
            }
            if (i + 1 < n) {
                l[i * n + i + 1] = -1.0 / (h * h);
            }
        }
        return l;
    };
    auto square = [](const std::vector<double>& l, std::size_t n) {
        std::vector<double> out(n * n, 0.0);
        for (std::size_t i = 0; i < n; ++i) {
            for (std::size_t j = 0; j < n; ++j) {
                for (std::size_t k = 0; k < n; ++k) {
                    out[i * n + j] += l[i * n + k] * l[k * n + j];
                }
            }
        }
        return out;
    };
    const std::vector<double> lx = second(nx, hx);
    const std::vector<double> ly = second(ny, hy);
    const std::vector<double> lx2 = square(lx, nx);
    const std::vector<double> ly2 = square(ly, ny);
    const std::size_t n = nx * ny;
    std::vector<double> a(n * n, 0.0);
    const double mixed = 2.0 * (d.d12 + 2.0 * d.d66);
    for (std::size_t i = 0; i < nx; ++i) {
        for (std::size_t j = 0; j < ny; ++j) {
            const std::size_t row = i * ny + j;
            for (std::size_t k = 0; k < nx; ++k) {
                for (std::size_t l = 0; l < ny; ++l) {
                    const std::size_t col = k * ny + l;
                    double value = 0.0;
                    if (j == l) {
                        value += d.d11 * lx2[i * nx + k];
                    }
                    if (i == k) {
                        value += d.d22 * ly2[j * ny + l];
                    }
                    value += mixed * lx[i * nx + k] * ly[j * ny + l];
                    a[row * n + col] = value;
                }
            }
        }
    }
    std::vector<double> chol(n * n, 0.0);
    for (std::size_t j = 0; j < n; ++j) {
        double diagonal = a[j * n + j];
        for (std::size_t k = 0; k < j; ++k) {
            diagonal -= chol[j * n + k] * chol[j * n + k];
        }
        check(diagonal > 0.0, "finite-difference plate operator is SPD");
        chol[j * n + j] = std::sqrt(diagonal);
        for (std::size_t i = j + 1; i < n; ++i) {
            double sum = a[i * n + j];
            for (std::size_t k = 0; k < j; ++k) {
                sum -= chol[i * n + k] * chol[j * n + k];
            }
            chol[i * n + j] = sum / chol[j * n + j];
        }
    }
    const std::size_t k = wanted + 3;
    std::vector<std::vector<double>> x(k, std::vector<double>(n));
    std::mt19937_64 rng(11);
    std::uniform_real_distribution<double> uniform(-1.0, 1.0);
    for (auto& column : x) {
        for (double& value : column) {
            value = uniform(rng);
        }
    }
    std::vector<double> ritz;
    for (int iteration = 0; iteration < 50; ++iteration) {
        for (auto& column : x) { // column <- A^-1 column
            for (std::size_t i = 0; i < n; ++i) {
                double sum = column[i];
                for (std::size_t c = 0; c < i; ++c) {
                    sum -= chol[i * n + c] * column[c];
                }
                column[i] = sum / chol[i * n + i];
            }
            for (std::size_t i = n; i-- > 0;) {
                double sum = column[i];
                for (std::size_t c = i + 1; c < n; ++c) {
                    sum -= chol[c * n + i] * column[c];
                }
                column[i] = sum / chol[i * n + i];
            }
        }
        for (int pass = 0; pass < 2; ++pass) { // modified Gram-Schmidt, twice
            for (std::size_t c = 0; c < k; ++c) {
                for (std::size_t q = 0; q < c; ++q) {
                    double projection = 0.0;
                    for (std::size_t i = 0; i < n; ++i) {
                        projection += x[c][i] * x[q][i];
                    }
                    for (std::size_t i = 0; i < n; ++i) {
                        x[c][i] -= projection * x[q][i];
                    }
                }
                double norm = 0.0;
                for (double value : x[c]) {
                    norm += value * value;
                }
                norm = std::sqrt(norm);
                for (double& value : x[c]) {
                    value /= norm;
                }
            }
        }
        std::vector<std::vector<double>> ax(k, std::vector<double>(n, 0.0));
        for (std::size_t c = 0; c < k; ++c) {
            for (std::size_t i = 0; i < n; ++i) {
                double sum = 0.0;
                for (std::size_t j = 0; j < n; ++j) {
                    sum += a[i * n + j] * x[c][j];
                }
                ax[c][i] = sum;
            }
        }
        std::vector<double> h(k * k, 0.0);
        for (std::size_t r = 0; r < k; ++r) {
            for (std::size_t c = 0; c < k; ++c) {
                double sum = 0.0;
                for (std::size_t i = 0; i < n; ++i) {
                    sum += x[r][i] * ax[c][i];
                }
                h[r * k + c] = sum;
            }
        }
        for (std::size_t r = 0; r < k; ++r) {
            for (std::size_t c = r + 1; c < k; ++c) {
                const double sym = 0.5 * (h[r * k + c] + h[c * k + r]);
                h[r * k + c] = sym;
                h[c * k + r] = sym;
            }
        }
        std::vector<double> v;
        ritz = symmetric_eigenvalues(h, k, &v);
        std::vector<std::vector<double>> rotated(k, std::vector<double>(n, 0.0));
        for (std::size_t c = 0; c < k; ++c) {
            for (std::size_t q = 0; q < k; ++q) {
                for (std::size_t i = 0; i < n; ++i) {
                    rotated[c][i] += x[q][i] * v[q * k + c];
                }
            }
        }
        x = std::move(rotated);
    }
    std::sort(ritz.begin(), ritz.end());
    std::vector<double> out;
    for (std::size_t i = 0; i < wanted; ++i) {
        out.push_back(std::sqrt(ritz[i] / (p.density_kg_per_m3 * p.thickness_m)) / (2.0 * pi));
    }
    return out;
}

void body_plate_frequency_oracle() {
    const OrthotropicPlate p = reference_plate();
    // Analytic formula versus the test-local frequency.
    std::vector<double> analytic;
    for (std::size_t m = 1; m <= 6; ++m) {
        for (std::size_t n = 1; n <= 6; ++n) {
            const double f = plate_mode_frequency_hz(p, m, n);
            near(f,
                 test_plate_omega(p, static_cast<double>(m), static_cast<double>(n)) / (2.0 * pi),
                 0.0, 1e-13, "plate_mode_frequency_hz matches the independent formula");
            analytic.push_back(f);
        }
    }
    std::sort(analytic.begin(), analytic.end());
    constexpr std::size_t wanted = 4;
    const std::array<std::pair<std::size_t, std::size_t>, 3> grids{{{7, 5}, {15, 11}, {31, 23}}};
    std::array<double, wanted> previous_error{};
    previous_error.fill(std::numeric_limits<double>::infinity());
    for (std::size_t g = 0; g < grids.size(); ++g) {
        const std::vector<double> fd =
            fd_plate_frequencies(p, grids[g].first, grids[g].second, wanted);
        for (std::size_t i = 0; i < wanted; ++i) {
            const double error = std::abs(fd[i] / analytic[i] - 1.0);
            std::cout << "grid " << grids[g].first << "x" << grids[g].second << " mode " << i
                      << " fd_hz=" << fd[i] << " analytic_hz=" << analytic[i]
                      << " relative_error=" << error << '\n';
            check(error < previous_error[i],
                  "finite-difference plate frequency converges monotonically");
            if (g > 0) {
                // h halves: second-order error ratio should approach 4.
                check(previous_error[i] / error > 3.3,
                      "finite-difference plate frequency is second order");
            }
            previous_error[i] = error;
        }
    }
    for (double error : previous_error) {
        check(error < 5e-3, "finest grid agrees with the analytic plate formula within 0.5%");
    }
    rejected([&] { (void)plate_mode_frequency_hz(p, 0, 1); }, "mode index 0 rejected");
}

BodyDesign two_dof_design() {
    BodyDesign d = synthetic_setar_body_design();
    d.lateral.enabled = false;
    d.bridge_mass_kg = 0.0;
    d.max_frequency_hz = 800.0;
    d.coupling_basis_factor = 1.0; // basis {(1,1), (1,2), (2,1)}: only (1,1) has net volume
    return d;
}

std::pair<double, double> two_dof_frequencies(const BodyDesign& d) {
    const OrthotropicPlate& p = d.plate;
    const HelmholtzCavity& c = d.cavity;
    const double mp = p.density_kg_per_m3 * p.thickness_m * p.length_m * p.width_m / 4.0;
    const double wp = test_plate_omega(p, 1.0, 1.0);
    const double kp = mp * wp * wp;
    const double vp = 4.0 * p.length_m * p.width_m / (pi * pi);
    const double ma = c.air_density_kg_per_m3 * c.opening_area_m2 * c.opening_effective_length_m;
    const double kappa =
        c.air_density_kg_per_m3 * c.sound_speed_m_per_s * c.sound_speed_m_per_s / c.volume_m3;
    const double k11 = kp + kappa * vp * vp;
    const double k22 = kappa * c.opening_area_m2 * c.opening_area_m2;
    // det(K - lambda M) = mp ma lambda^2 - (mp k22 + ma k11) lambda + kp k22 = 0.
    const double qa = mp * ma;
    const double qb = mp * k22 + ma * k11;
    const double qc = kp * k22;
    const double root = std::sqrt(qb * qb - 4.0 * qa * qc);
    const double upper = (qb + root) / (2.0 * qa);
    const double lower = qc / (qa * upper); // product of roots, avoids cancellation
    return {std::sqrt(lower) / (2.0 * pi), std::sqrt(upper) / (2.0 * pi)};
}

void body_cavity_coupling_oracle() {
    const BodyDesign d = two_dof_design();
    const BodyConfig body = build_body(d);
    const auto [f_low, f_high] = two_dof_frequencies(d);
    const double f12 = test_plate_omega(d.plate, 1.0, 2.0) / (2.0 * pi);
    const double f21 = test_plate_omega(d.plate, 2.0, 1.0) / (2.0 * pi);
    std::vector<double> expected{f_low, f_high, f12, f21};
    std::sort(expected.begin(), expected.end());
    check(body.modes.size() == expected.size(), "two-DOF fixture retains exactly four modes");
    for (std::size_t i = 0; i < expected.size(); ++i) {
        std::cout << "mode " << i << " build_body_hz=" << body.modes[i].frequency_hz
                  << " oracle_hz=" << expected[i] << '\n';
        near(body.modes[i].frequency_hz, expected[i], 0.0, 1e-10,
             "coupled frequency matches closed form");
    }
    // The volume-free (1,2) and (2,1) modes are untouched by the cavity.
    const double a = d.plate.length_m;
    const double b = d.plate.width_m;
    const double mp = d.plate.density_kg_per_m3 * d.plate.thickness_m * a * b / 4.0;
    const double rho0 = d.observation.air_density_kg_per_m3;
    for (const BodyMode& mode : body.modes) {
        const bool is12 = std::abs(mode.frequency_hz / f12 - 1.0) < 1e-10;
        const bool is21 = std::abs(mode.frequency_hz / f21 - 1.0) < 1e-10;
        if (!is12 && !is21) {
            continue;
        }
        const double m = is12 ? 1.0 : 2.0;
        const double n = is12 ? 2.0 : 1.0;
        near(mode.modal_mass_kg, mp, 0.0, 1e-10, "uncoupled plate modal mass rho*h*a*b/4");
        near(mode.damping_ratio, d.plate.damping_ratio, 0.0, 1e-10, "uncoupled plate damping");
        for (std::size_t s = 0; s < string_count; ++s) {
            const double expected_w = std::sin(m * pi * d.bridge_point_m[s][0] / a) *
                                      std::sin(n * pi * d.bridge_point_m[s][1] / b);
            near(mode.bridge_participation[s][0], expected_w, 1e-12, 1e-10,
                 "uncoupled participation");
            check(mode.bridge_participation[s][1] == 0.0,
                  "plate mode has no horizontal participation");
        }
        // (1,2): no monopole and no x-moment. (2,1): x-moment -2 a^2 b/(m n pi^2) only.
        const double moment = is21 ? -2.0 * a * a * b / (m * n * pi * pi) : 0.0;
        for (std::size_t o = 0; o < observation_count; ++o) {
            const double expected_weight = rho0 / (4.0 * pi * d.observation.distance_m[o]) *
                                           d.observation.dipole_weight_per_m[o] * moment;
            near(mode.radiation_weight[o], expected_weight, 1e-15, 1e-9, "dipole radiation weight");
        }
    }
    // Weak-coupling limit: scale V and A together (Helmholtz frequency unchanged).
    const double fp = test_plate_omega(d.plate, 1.0, 1.0) / (2.0 * pi);
    const double fh = helmholtz_frequency_hz(d.cavity);
    near(fh,
         d.cavity.sound_speed_m_per_s / (2.0 * pi) *
             std::sqrt(d.cavity.opening_area_m2 /
                       (d.cavity.volume_m3 * d.cavity.opening_effective_length_m)),
         0.0, 1e-14, "Helmholtz formula");
    double previous = std::numeric_limits<double>::infinity();
    for (double scale : {1.0, 1e1, 1e2, 1e3, 1e4}) {
        BodyDesign weak = d;
        weak.cavity.volume_m3 *= scale;
        weak.cavity.opening_area_m2 *= scale;
        const auto [lo, hi] = two_dof_frequencies(weak);
        const BodyConfig wb = build_body(weak);
        near(wb.modes.front().frequency_hz, lo, 0.0, 1e-9, "weakly coupled lower frequency");
        const double deviation =
            std::max(std::abs(lo - std::min(fp, fh)), std::abs(hi - std::max(fp, fh))) /
            std::max(fp, fh);
        std::cout << "coupling scale " << scale << " max_relative_shift=" << deviation << '\n';
        check(deviation < previous,
              "coupled frequencies approach the uncoupled pair monotonically");
        previous = deviation;
    }
    check(previous < 1e-3, "weak-coupling limit recovers the uncoupled frequencies");
    BodyDesign off = d;
    off.cavity.enabled = false;
    const BodyConfig uncoupled = build_body(off);
    near(uncoupled.modes.front().frequency_hz, fp, 0.0, 1e-10,
         "cavity disabled recovers plate (1,1)");
    near(uncoupled.modes.front().modal_mass_kg, mp, 0.0, 1e-10, "cavity disabled keeps plate mass");
}

void body_mobility_reciprocity() {
    const BodyConfig body = build_body(synthetic_setar_body_design());
    const std::array<double, 7> frequencies{20.0, 155.0, 314.05, 1000.0, 1600.0, 4321.0, 15000.0};
    for (double f : frequencies) {
        double scale = 0.0;
        for (std::size_t s = 0; s < string_count; ++s) {
            for (std::size_t p = 0; p < polarization_count; ++p) {
                scale =
                    std::max(scale, std::abs(bridge_mobility(body, s, static_cast<Polarization>(p),
                                                             s, static_cast<Polarization>(p), f)));
            }
        }
        for (std::size_t so = 0; so < string_count; ++so) {
            for (std::size_t po = 0; po < polarization_count; ++po) {
                for (std::size_t si = 0; si < string_count; ++si) {
                    for (std::size_t pi_ = 0; pi_ < polarization_count; ++pi_) {
                        const auto pout = static_cast<Polarization>(po);
                        const auto pin = static_cast<Polarization>(pi_);
                        const std::complex<double> y = bridge_mobility(body, so, pout, si, pin, f);
                        const std::complex<double> yt = bridge_mobility(body, si, pin, so, pout, f);
                        check(std::abs(y - yt) <= 1e-13 * scale, "transfer mobility is reciprocal");
                        if (po != pi_) {
                            check(
                                y == std::complex<double>(0.0, 0.0),
                                "vertical and horizontal bridge motion are uncoupled in this body");
                        }
                    }
                }
            }
        }
    }
    // Uncoupled body: the reduced mobility equals an analytic plate modal sum.
    BodyDesign d = synthetic_setar_body_design();
    d.cavity.enabled = false;
    d.bridge_mass_kg = 0.0;
    d.lateral.enabled = true;
    d.max_frequency_hz = 3000.0;
    d.coupling_basis_factor = 1.0;
    const BodyConfig plain = build_body(d);
    const double a = d.plate.length_m;
    const double b = d.plate.width_m;
    const double mp = d.plate.density_kg_per_m3 * d.plate.thickness_m * a * b / 4.0;
    for (double f : {80.0, 202.0, 777.0, 2500.0}) {
        const double w = 2.0 * pi * f;
        for (std::size_t so = 0; so < string_count; ++so) {
            for (std::size_t si = 0; si < string_count; ++si) {
                std::complex<double> expected{0.0, 0.0};
                for (double m = 1.0; m < 40.0; m += 1.0) {
                    for (double n = 1.0; n < 40.0; n += 1.0) {
                        const double wm = test_plate_omega(d.plate, m, n);
                        if (wm / (2.0 * pi) >= d.max_frequency_hz) {
                            continue;
                        }
                        const double wo = std::sin(m * pi * d.bridge_point_m[so][0] / a) *
                                          std::sin(n * pi * d.bridge_point_m[so][1] / b);
                        const double wi = std::sin(m * pi * d.bridge_point_m[si][0] / a) *
                                          std::sin(n * pi * d.bridge_point_m[si][1] / b);
                        expected +=
                            std::complex<double>(0.0, w) * wo * wi /
                            (mp * std::complex<double>(wm * wm - w * w,
                                                       2.0 * d.plate.damping_ratio * wm * w));
                    }
                }
                const std::complex<double> actual = bridge_mobility(
                    plain, so, Polarization::Vertical, si, Polarization::Vertical, f);
                check(std::abs(actual - expected) <= 1e-9 * std::abs(expected) + 1e-18,
                      "reduced vertical mobility equals the analytic plate modal sum");
            }
        }
        const double wl = 2.0 * pi * d.lateral.frequency_hz;
        const std::complex<double> lateral =
            std::complex<double>(0.0, w) /
            (d.lateral.modal_mass_kg *
             std::complex<double>(wl * wl - w * w, 2.0 * d.lateral.damping_ratio * wl * w));
        check(std::abs(bridge_mobility(plain, 1, Polarization::Horizontal, 2,
                                       Polarization::Horizontal, f) -
                       lateral) <= 1e-12 * std::abs(lateral),
              "horizontal mobility equals the lateral oscillator");
    }
}

void body_mobility_passivity() {
    const BodyDesign design = synthetic_setar_body_design();
    const BodyConfig body = build_body(design);
    check(body.provenance == Provenance::Synthetic, "synthetic body is labelled Synthetic");
    check(body.label.find("synthetic") != std::string::npos, "label states synthetic status");
    double previous = 0.0;
    for (const BodyMode& mode : body.modes) {
        check(std::isfinite(mode.frequency_hz) && mode.frequency_hz > previous &&
                  mode.frequency_hz < design.max_frequency_hz,
              "modes are finite, sorted, distinct and below max_frequency_hz");
        previous = mode.frequency_hz;
        check(mode.modal_mass_kg > 0.0 && std::isfinite(mode.modal_mass_kg), "positive modal mass");
        check(mode.damping_ratio >= 0.0 && mode.damping_ratio < 1.0, "damping ratio in [0, 1)");
        for (const auto& row : mode.bridge_participation) {
            check(std::isfinite(row[0]) && std::isfinite(row[1]), "finite participation");
        }
        check(std::isfinite(mode.radiation_weight[0]) && std::isfinite(mode.radiation_weight[1]),
              "finite radiation weights");
    }
    constexpr std::size_t contacts = string_count * polarization_count;
    double minimum_eigenvalue_ratio = std::numeric_limits<double>::infinity();
    for (int i = 0; i <= 2000; ++i) {
        const double f =
            10.0 * std::pow(2000.0, static_cast<double>(i) / 2000.0); // 10 Hz .. 20 kHz
        std::vector<double> real_part(contacts * contacts, 0.0);
        double scale = 0.0;
        for (std::size_t r = 0; r < contacts; ++r) {
            for (std::size_t c = 0; c < contacts; ++c) {
                const std::complex<double> y =
                    bridge_mobility(body, r / 2, static_cast<Polarization>(r % 2), c / 2,
                                    static_cast<Polarization>(c % 2), f);
                real_part[r * contacts + c] = y.real();
                if (r == c) {
                    check(y.real() >= 0.0, "driving-point mobility has nonnegative real part");
                    scale = std::max(scale, y.real());
                }
            }
        }
        // Reciprocal Y is complex symmetric, so its Hermitian part is Re(Y): it must be PSD.
        const std::vector<double> eigen = symmetric_eigenvalues(real_part, contacts);
        const double smallest = *std::min_element(eigen.begin(), eigen.end());
        check(smallest >= -1e-12 * scale, "multiport mobility is positive real (passive)");
        minimum_eigenvalue_ratio =
            std::min(minimum_eigenvalue_ratio, smallest / std::max(scale, 1e-300));
    }
    std::cout << "body modes=" << body.modes.size()
              << " first_hz=" << body.modes.front().frequency_hz
              << " last_hz=" << body.modes.back().frequency_hz
              << " min_eig(Re Y)/max(Re Y_ii)=" << minimum_eigenvalue_ratio << '\n';
}

void body_invalid_design() {
    const BodyDesign base = synthetic_setar_body_design();
    (void)build_body(base);
    using Mutation = std::function<void(BodyDesign&)>;
    const std::vector<std::pair<std::string, Mutation>> mutations{
        {"nan length", [](BodyDesign& d) { d.plate.length_m = std::nan(""); }},
        {"zero width", [](BodyDesign& d) { d.plate.width_m = 0.0; }},
        {"zero thickness", [](BodyDesign& d) { d.plate.thickness_m = 0.0; }},
        {"thick plate", [](BodyDesign& d) { d.plate.thickness_m = 0.05; }},
        {"negative density", [](BodyDesign& d) { d.plate.density_kg_per_m3 = -650.0; }},
        {"zero cross modulus", [](BodyDesign& d) { d.plate.youngs_modulus_cross_pa = 0.0; }},
        {"infinite long modulus",
         [](BodyDesign& d) {
             d.plate.youngs_modulus_long_pa = std::numeric_limits<double>::infinity();
         }},
        {"poisson indefinite", [](BodyDesign& d) { d.plate.poisson_long_cross = 3.5; }},
        {"plate damping one", [](BodyDesign& d) { d.plate.damping_ratio = 1.0; }},
        {"plate damping negative", [](BodyDesign& d) { d.plate.damping_ratio = -0.01; }},
        {"bridge on edge", [](BodyDesign& d) { d.bridge_point_m[0][0] = 0.0; }},
        {"bridge outside", [](BodyDesign& d) { d.bridge_point_m[3][1] = d.plate.width_m + 0.01; }},
        {"cavity zero volume", [](BodyDesign& d) { d.cavity.volume_m3 = 0.0; }},
        {"cavity zero area", [](BodyDesign& d) { d.cavity.opening_area_m2 = 0.0; }},
        {"cavity zero neck", [](BodyDesign& d) { d.cavity.opening_effective_length_m = 0.0; }},
        {"cavity damping", [](BodyDesign& d) { d.cavity.damping_ratio = 1.5; }},
        {"opening outside", [](BodyDesign& d) { d.cavity.opening_position_m = {-0.01, 0.05}; }},
        {"cavity air density", [](BodyDesign& d) { d.cavity.air_density_kg_per_m3 = 0.0; }},
        {"lateral zero mass", [](BodyDesign& d) { d.lateral.modal_mass_kg = 0.0; }},
        {"lateral zero frequency", [](BodyDesign& d) { d.lateral.frequency_hz = 0.0; }},
        {"lateral negative area", [](BodyDesign& d) { d.lateral.radiation_area_m2 = -1.0; }},
        {"observation distance", [](BodyDesign& d) { d.observation.distance_m[1] = 0.0; }},
        {"observation dipole nan",
         [](BodyDesign& d) { d.observation.dipole_weight_per_m[0] = std::nan(""); }},
        {"observation density", [](BodyDesign& d) { d.observation.air_density_kg_per_m3 = -1.0; }},
        {"max frequency zero", [](BodyDesign& d) { d.max_frequency_hz = 0.0; }},
        {"coupling factor", [](BodyDesign& d) { d.coupling_basis_factor = 0.5; }},
        {"bridge mass negative", [](BodyDesign& d) { d.bridge_mass_kg = -1e-3; }},
        {"no retained mode",
         [](BodyDesign& d) {
             d.max_frequency_hz = 1.0;
             d.lateral.enabled = false;
         }},
    };
    for (const auto& [name, mutate] : mutations) {
        BodyDesign d = base;
        mutate(d);
        rejected([&] { (void)build_body(d); }, "build_body must reject: " + name);
        if (name != "no retained mode") {
            rejected([&] { validate(d); }, "validate must reject: " + name);
        }
    }
    // Disabled subsystems are not validated: a disabled cavity with blank fields is admissible.
    BodyDesign no_cavity = base;
    no_cavity.cavity = HelmholtzCavity{};
    (void)build_body(no_cavity);
    const BodyConfig body = build_body(base);
    rejected(
        [&] {
            (void)bridge_mobility(body, 4, Polarization::Vertical, 0, Polarization::Vertical,
                                  100.0);
        },
        "string index 4 rejected");
    rejected(
        [&] {
            (void)bridge_mobility(body, 0, Polarization::Vertical, 0, Polarization::Vertical, -1.0);
        },
        "negative frequency rejected");
    rejected(
        [&] {
            (void)bridge_mobility(body, 0, static_cast<Polarization>(2), 0, Polarization::Vertical,
                                  1.0);
        },
        "invalid polarization rejected");
    BodyConfig broken = body;
    broken.modes.front().modal_mass_kg = 0.0;
    rejected(
        [&] {
            (void)bridge_mobility(broken, 0, Polarization::Vertical, 0, Polarization::Vertical,
                                  1.0);
        },
        "zero modal mass rejected by mobility");
    HelmholtzCavity blank;
    rejected([&] { (void)helmholtz_frequency_hz(blank); },
             "blank cavity rejected by Helmholtz formula");
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
    const std::array<Case, 7> tests{{
        {"materials.reciprocity", materials_reciprocity},
        {"materials.positive_energy", materials_positive_energy},
        {"body.plate_frequency_oracle", body_plate_frequency_oracle},
        {"body.cavity_coupling_oracle", body_cavity_coupling_oracle},
        {"body.mobility_reciprocity", body_mobility_reciprocity},
        {"body.mobility_passivity", body_mobility_passivity},
        {"body.invalid_design", body_invalid_design},
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
