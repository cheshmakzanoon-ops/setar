#include "setar/instrument/body_model.hpp"

#include <algorithm>
#include <cmath>
#include <numbers>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace setar::instrument {
namespace {
constexpr double pi = std::numbers::pi_v<double>;

void require(bool valid, const std::string& message) {
    if (!valid) {
        throw std::invalid_argument("body design: " + message);
    }
}

bool positive(double value) { return std::isfinite(value) && value > 0.0; }
bool ratio(double value) { return std::isfinite(value) && value >= 0.0 && value < 1.0; }

void validate_plate(const OrthotropicPlate& p) {
    require(positive(p.length_m), "plate.length_m must be finite and positive (m)");
    require(positive(p.width_m), "plate.width_m must be finite and positive (m)");
    require(positive(p.thickness_m), "plate.thickness_m must be finite and positive (m)");
    require(p.thickness_m < 0.1 * std::min(p.length_m, p.width_m),
            "plate.thickness_m must be thin (< 0.1 of the smaller plate dimension) for Kirchhoff "
            "theory");
    require(positive(p.density_kg_per_m3), "plate.density_kg_per_m3 must be finite and positive");
    require(positive(p.youngs_modulus_long_pa),
            "plate.youngs_modulus_long_pa must be finite and positive (Pa)");
    require(positive(p.youngs_modulus_cross_pa),
            "plate.youngs_modulus_cross_pa must be finite and positive (Pa)");
    require(positive(p.shear_modulus_pa),
            "plate.shear_modulus_pa must be finite and positive (Pa)");
    require(std::isfinite(p.poisson_long_cross), "plate.poisson_long_cross must be finite");
    // Positive-definite plane-stress compliance <=> 1 - nu_LR*nu_RL > 0 with positive moduli,
    // i.e. nu_LR^2 < E_L/E_R. A violating set describes no admissible elastic material.
    require(p.poisson_long_cross * p.poisson_long_cross <
                p.youngs_modulus_long_pa / p.youngs_modulus_cross_pa,
            "plate.poisson_long_cross violates |nu_LR| < sqrt(E_L/E_R) (compliance not positive "
            "definite)");
    require(ratio(p.damping_ratio), "plate.damping_ratio must be in [0, 1)");
}

double mode_volume(std::size_t m, std::size_t n, double a, double b) {
    // integral over the plate of sin(m pi x/a) sin(n pi y/b): 4ab/(m n pi^2) for odd m, n.
    if (m % 2 == 0 || n % 2 == 0) {
        return 0.0;
    }
    return 4.0 * a * b / (static_cast<double>(m * n) * pi * pi);
}

double mode_x_moment(std::size_t m, std::size_t n, double a, double b) {
    // integral of (x - a/2) sin(m pi x/a) sin(n pi y/b): -2 a^2 b/(m n pi^2) for even m, odd n.
    if (m % 2 != 0 || n % 2 == 0) {
        return 0.0;
    }
    return -2.0 * a * a * b / (static_cast<double>(m * n) * pi * pi);
}

double mode_shape(std::size_t m, std::size_t n, double a, double b, double x, double y) {
    return std::sin(static_cast<double>(m) * pi * x / a) *
           std::sin(static_cast<double>(n) * pi * y / b);
}

// Cyclic Jacobi eigen-decomposition of a symmetric matrix (row-major, n*n). On return
// `matrix` is diagonal (eigenvalues) and `vectors` holds eigenvectors as columns.
void jacobi_eigen(std::vector<double>& matrix, std::vector<double>& vectors, std::size_t n) {
    vectors.assign(n * n, 0.0);
    for (std::size_t i = 0; i < n; ++i) {
        vectors[i * n + i] = 1.0;
    }
    auto at = [&](std::size_t r, std::size_t c) -> double& { return matrix[r * n + c]; };
    double scale = 0.0;
    for (double v : matrix) {
        scale = std::max(scale, std::abs(v));
    }
    for (int sweep = 0; sweep < 100; ++sweep) {
        double off = 0.0;
        for (std::size_t r = 0; r < n; ++r) {
            for (std::size_t c = r + 1; c < n; ++c) {
                off = std::max(off, std::abs(at(r, c)));
            }
        }
        if (off <= 1.0e-14 * scale) {
            return;
        }
        for (std::size_t p = 0; p < n; ++p) {
            for (std::size_t q = p + 1; q < n; ++q) {
                const double apq = at(p, q);
                if (apq == 0.0) {
                    continue;
                }
                const double theta = (at(q, q) - at(p, p)) / (2.0 * apq);
                const double t = (theta >= 0.0 ? 1.0 : -1.0) /
                                 (std::abs(theta) + std::sqrt(theta * theta + 1.0));
                const double c = 1.0 / std::sqrt(t * t + 1.0);
                const double s = t * c;
                for (std::size_t k = 0; k < n; ++k) {
                    const double akp = at(k, p);
                    const double akq = at(k, q);
                    at(k, p) = c * akp - s * akq;
                    at(k, q) = s * akp + c * akq;
                }
                for (std::size_t k = 0; k < n; ++k) {
                    const double apk = at(p, k);
                    const double aqk = at(q, k);
                    at(p, k) = c * apk - s * aqk;
                    at(q, k) = s * apk + c * aqk;
                }
                for (std::size_t k = 0; k < n; ++k) {
                    const double vkp = vectors[k * n + p];
                    const double vkq = vectors[k * n + q];
                    vectors[k * n + p] = c * vkp - s * vkq;
                    vectors[k * n + q] = s * vkp + c * vkq;
                }
            }
        }
    }
    throw std::invalid_argument("body design: coupled eigenproblem did not converge");
}

struct PlateMode {
    std::size_t m;
    std::size_t n;
    double omega;
};

} // namespace

FlexuralRigidity flexural_rigidity(const OrthotropicPlate& p) {
    validate_plate(p);
    const double nu_rl =
        p.poisson_long_cross * p.youngs_modulus_cross_pa / p.youngs_modulus_long_pa;
    const double h3 = p.thickness_m * p.thickness_m * p.thickness_m;
    const double denominator = 12.0 * (1.0 - p.poisson_long_cross * nu_rl);
    FlexuralRigidity d;
    d.d11 = p.youngs_modulus_long_pa * h3 / denominator;
    d.d22 = p.youngs_modulus_cross_pa * h3 / denominator;
    d.d12 = p.poisson_long_cross * d.d22;
    d.d66 = p.shear_modulus_pa * h3 / 12.0;
    return d;
}

double poisson_cross_long(const OrthotropicPlate& p) {
    validate_plate(p);
    return p.poisson_long_cross * p.youngs_modulus_cross_pa / p.youngs_modulus_long_pa;
}

std::array<double, 9> plane_stress_compliance(const OrthotropicPlate& p) {
    const double nu_rl = poisson_cross_long(p);
    return {1.0 / p.youngs_modulus_long_pa,
            -nu_rl / p.youngs_modulus_cross_pa,
            0.0,
            -p.poisson_long_cross / p.youngs_modulus_long_pa,
            1.0 / p.youngs_modulus_cross_pa,
            0.0,
            0.0,
            0.0,
            1.0 / p.shear_modulus_pa};
}

bool reciprocal_poisson_pair(double youngs_long_pa, double youngs_cross_pa, double nu_long_cross,
                             double nu_cross_long, double relative_tolerance) {
    if (!positive(youngs_long_pa) || !positive(youngs_cross_pa) || !std::isfinite(nu_long_cross) ||
        !std::isfinite(nu_cross_long) || !std::isfinite(relative_tolerance) ||
        relative_tolerance < 0.0) {
        return false;
    }
    const double s12 = nu_long_cross / youngs_long_pa;
    const double s21 = nu_cross_long / youngs_cross_pa;
    const bool symmetric =
        std::abs(s12 - s21) <= relative_tolerance * std::max(std::abs(s12), std::abs(s21));
    return symmetric && nu_long_cross * nu_cross_long < 1.0;
}

double plate_mode_frequency_hz(const OrthotropicPlate& plate, std::size_t m, std::size_t n) {
    require(m >= 1 && n >= 1, "plate mode indices must be >= 1");
    const FlexuralRigidity d = flexural_rigidity(plate);
    const double ka = static_cast<double>(m) / plate.length_m;
    const double kb = static_cast<double>(n) / plate.width_m;
    const double omega_sq =
        std::pow(pi, 4) / (plate.density_kg_per_m3 * plate.thickness_m) *
        (d.d11 * std::pow(ka, 4) + 2.0 * (d.d12 + 2.0 * d.d66) * ka * ka * kb * kb +
         d.d22 * std::pow(kb, 4));
    return std::sqrt(omega_sq) / (2.0 * pi);
}

double helmholtz_frequency_hz(const HelmholtzCavity& c) {
    require(positive(c.volume_m3), "cavity.volume_m3 must be finite and positive (m^3)");
    require(positive(c.opening_area_m2),
            "cavity.opening_area_m2 must be finite and positive (m^2)");
    require(positive(c.opening_effective_length_m),
            "cavity.opening_effective_length_m must be finite and positive (m)");
    require(positive(c.sound_speed_m_per_s),
            "cavity.sound_speed_m_per_s must be finite and positive");
    return c.sound_speed_m_per_s / (2.0 * pi) *
           std::sqrt(c.opening_area_m2 / (c.volume_m3 * c.opening_effective_length_m));
}

void validate(const BodyDesign& design) {
    validate_plate(design.plate);
    const double a = design.plate.length_m;
    const double b = design.plate.width_m;
    require(positive(design.max_frequency_hz), "max_frequency_hz must be finite and positive (Hz)");
    require(std::isfinite(design.coupling_basis_factor) && design.coupling_basis_factor >= 1.0,
            "coupling_basis_factor must be finite and >= 1");
    require(std::isfinite(design.bridge_mass_kg) && design.bridge_mass_kg >= 0.0,
            "bridge_mass_kg must be finite and nonnegative (kg)");
    for (std::size_t s = 0; s < string_count; ++s) {
        const auto& point = design.bridge_point_m[s];
        require(std::isfinite(point[0]) && point[0] > 0.0 && point[0] < a,
                "bridge_point_m[" + std::to_string(s) + "] x_p must lie strictly inside the plate");
        require(std::isfinite(point[1]) && point[1] > 0.0 && point[1] < b,
                "bridge_point_m[" + std::to_string(s) + "] y_p must lie strictly inside the plate");
    }
    if (design.cavity.enabled) {
        const auto& c = design.cavity;
        (void)helmholtz_frequency_hz(c);
        require(positive(c.air_density_kg_per_m3),
                "cavity.air_density_kg_per_m3 must be finite and positive");
        require(ratio(c.damping_ratio), "cavity.damping_ratio must be in [0, 1)");
        require(std::isfinite(c.opening_position_m[0]) && c.opening_position_m[0] > 0.0 &&
                    c.opening_position_m[0] < a && std::isfinite(c.opening_position_m[1]) &&
                    c.opening_position_m[1] > 0.0 && c.opening_position_m[1] < b,
                "cavity.opening_position_m must lie strictly inside the plate");
    }
    if (design.lateral.enabled) {
        const auto& l = design.lateral;
        require(positive(l.frequency_hz), "lateral.frequency_hz must be finite and positive (Hz)");
        require(positive(l.modal_mass_kg),
                "lateral.modal_mass_kg must be finite and positive (kg)");
        require(ratio(l.damping_ratio), "lateral.damping_ratio must be in [0, 1)");
        require(std::isfinite(l.radiation_area_m2) && l.radiation_area_m2 >= 0.0,
                "lateral.radiation_area_m2 must be finite and nonnegative (m^2)");
    }
    require(positive(design.observation.air_density_kg_per_m3),
            "observation.air_density_kg_per_m3 must be finite and positive");
    for (std::size_t o = 0; o < observation_count; ++o) {
        require(positive(design.observation.distance_m[o]),
                "observation.distance_m[" + std::to_string(o) +
                    "] must be finite and positive (m)");
        require(std::isfinite(design.observation.dipole_weight_per_m[o]),
                "observation.dipole_weight_per_m[" + std::to_string(o) + "] must be finite (1/m)");
    }
}

BodyDesign synthetic_setar_body_design() {
    // SYNTHETIC placeholders; see docs/body/10-synthetic-prototype-body.md for each rationale.
    BodyDesign d;
    d.label = "synthetic orthotropic plate + Helmholtz cavity + lateral bridge mode (not a "
              "measured setar)";
    d.plate.length_m = 0.20;
    d.plate.width_m = 0.13;
    d.plate.thickness_m = 0.003;
    d.plate.density_kg_per_m3 = 650.0;
    d.plate.youngs_modulus_long_pa = 9.5e9;
    d.plate.youngs_modulus_cross_pa = 0.9e9;
    d.plate.shear_modulus_pa = 0.7e9;
    d.plate.poisson_long_cross = 0.37;
    d.plate.damping_ratio = 0.012;
    d.cavity.enabled = true;
    d.cavity.volume_m3 = 1.3e-3;
    d.cavity.opening_area_m2 = 2.0e-4;
    d.cavity.opening_effective_length_m = 7.3e-3;
    d.cavity.air_density_kg_per_m3 = 1.2;
    d.cavity.sound_speed_m_per_s = 343.0;
    d.cavity.damping_ratio = 0.03;
    d.cavity.opening_position_m = {0.05, 0.065};
    d.lateral.enabled = true;
    d.lateral.frequency_hz = 1600.0;
    d.lateral.modal_mass_kg = 0.04;
    d.lateral.damping_ratio = 0.05;
    d.lateral.radiation_area_m2 = 2.0e-4;
    d.observation.distance_m = {0.5, 0.5};
    d.observation.dipole_weight_per_m = {4.0, -4.0};
    d.observation.air_density_kg_per_m3 = 1.2;
    // Bridge across y_p, monotone in physical string index (synthetic ordering assumption).
    d.bridge_point_m = {{{0.145, 0.057}, {0.145, 0.062}, {0.145, 0.067}, {0.145, 0.070}}};
    d.bridge_mass_kg = 1.5e-3;
    d.max_frequency_hz = 7000.0;
    d.coupling_basis_factor = 1.5;
    return d;
}

BodyConfig build_body(const BodyDesign& design) {
    validate(design);
    const OrthotropicPlate& plate = design.plate;
    const double a = plate.length_m;
    const double b = plate.width_m;
    const double plate_mass = plate.density_kg_per_m3 * plate.thickness_m * a * b / 4.0;
    const double basis_cutoff_hz = design.coupling_basis_factor * design.max_frequency_hz;

    std::vector<PlateMode> basis;
    for (std::size_t m = 1;; ++m) {
        if (plate_mode_frequency_hz(plate, m, 1) >= basis_cutoff_hz) {
            break;
        }
        for (std::size_t n = 1;; ++n) {
            const double f = plate_mode_frequency_hz(plate, m, n);
            if (f >= basis_cutoff_hz) {
                break;
            }
            basis.push_back({m, n, 2.0 * pi * f});
        }
    }
    require(!basis.empty() || design.cavity.enabled || design.lateral.enabled,
            "no plate mode lies below the coupling basis cutoff");
    require(basis.size() <= 400, "coupling basis exceeds 400 plate modes; lower max_frequency_hz");

    const bool cavity = design.cavity.enabled;
    const std::size_t plate_count = basis.size();
    const std::size_t n = plate_count + (cavity ? 1 : 0);
    std::vector<double> mass(n * n, 0.0);
    std::vector<double> stiffness(n * n, 0.0);
    std::vector<double> damping(n * n, 0.0);
    std::vector<double> volume(n, 0.0);
    std::vector<double> moment(n, 0.0);

    double xb = 0.0;
    double yb = 0.0;
    for (const auto& point : design.bridge_point_m) {
        xb += point[0] / static_cast<double>(string_count);
        yb += point[1] / static_cast<double>(string_count);
    }
    for (std::size_t i = 0; i < plate_count; ++i) {
        const PlateMode& pm = basis[i];
        mass[i * n + i] = plate_mass;
        stiffness[i * n + i] = plate_mass * pm.omega * pm.omega;
        damping[i * n + i] = 2.0 * plate.damping_ratio * pm.omega * plate_mass;
        volume[i] = mode_volume(pm.m, pm.n, a, b);
        moment[i] = mode_x_moment(pm.m, pm.n, a, b);
    }
    // Lumped bridge mass: kinetic energy m_b/2 * (sum_i W_i(x_b) eta_i')^2.
    for (std::size_t i = 0; i < plate_count; ++i) {
        const double wi = mode_shape(basis[i].m, basis[i].n, a, b, xb, yb);
        for (std::size_t j = 0; j < plate_count; ++j) {
            const double wj = mode_shape(basis[j].m, basis[j].n, a, b, xb, yb);
            mass[i * n + j] += design.bridge_mass_kg * wi * wj;
        }
    }
    if (cavity) {
        const HelmholtzCavity& c = design.cavity;
        const std::size_t air = plate_count;
        const double air_mass =
            c.air_density_kg_per_m3 * c.opening_area_m2 * c.opening_effective_length_m;
        const double omega_h = 2.0 * pi * helmholtz_frequency_hz(c);
        mass[air * n + air] = air_mass;
        damping[air * n + air] = 2.0 * c.damping_ratio * omega_h * air_mass;
        volume[air] = c.opening_area_m2;
        moment[air] = c.opening_area_m2 * (c.opening_position_m[0] - 0.5 * a);
        // Cavity potential kappa/2 * (outward volume change)^2, kappa = rho c^2 / V.
        const double kappa =
            c.air_density_kg_per_m3 * c.sound_speed_m_per_s * c.sound_speed_m_per_s / c.volume_m3;
        for (std::size_t i = 0; i < n; ++i) {
            for (std::size_t j = 0; j < n; ++j) {
                stiffness[i * n + j] += kappa * volume[i] * volume[j];
            }
        }
    }

    // Generalized symmetric eigenproblem K phi = lambda M phi via M = L L^T.
    std::vector<double> lower(n * n, 0.0);
    for (std::size_t j = 0; j < n; ++j) {
        double diagonal = mass[j * n + j];
        for (std::size_t k = 0; k < j; ++k) {
            diagonal -= lower[j * n + k] * lower[j * n + k];
        }
        require(diagonal > 0.0, "mass matrix is not positive definite");
        lower[j * n + j] = std::sqrt(diagonal);
        for (std::size_t i = j + 1; i < n; ++i) {
            double sum = mass[i * n + j];
            for (std::size_t k = 0; k < j; ++k) {
                sum -= lower[i * n + k] * lower[j * n + k];
            }
            lower[i * n + j] = sum / lower[j * n + j];
        }
    }
    // reduced = L^-1 K L^-T, column by column (forward substitutions).
    std::vector<double> temp(n * n, 0.0); // L^-1 K
    for (std::size_t col = 0; col < n; ++col) {
        for (std::size_t i = 0; i < n; ++i) {
            double sum = stiffness[i * n + col];
            for (std::size_t k = 0; k < i; ++k) {
                sum -= lower[i * n + k] * temp[k * n + col];
            }
            temp[i * n + col] = sum / lower[i * n + i];
        }
    }
    std::vector<double> reduced(n * n, 0.0); // (L^-1 (L^-1 K)^T)^T = L^-1 K L^-T
    for (std::size_t row = 0; row < n; ++row) {
        for (std::size_t i = 0; i < n; ++i) {
            double sum = temp[row * n + i];
            for (std::size_t k = 0; k < i; ++k) {
                sum -= lower[i * n + k] * reduced[row * n + k];
            }
            reduced[row * n + i] = sum / lower[i * n + i];
        }
    }
    for (std::size_t i = 0; i < n; ++i) {
        for (std::size_t j = i + 1; j < n; ++j) {
            const double sym = 0.5 * (reduced[i * n + j] + reduced[j * n + i]);
            reduced[i * n + j] = sym;
            reduced[j * n + i] = sym;
        }
    }
    std::vector<double> y;
    if (n > 0) {
        jacobi_eigen(reduced, y, n);
    }

    BodyConfig body;
    body.label = design.label.empty() ? std::string("synthetic body (not measured)") : design.label;
    body.provenance = Provenance::Synthetic;
    const double rho0 = design.observation.air_density_kg_per_m3;
    std::vector<double> phi(n, 0.0);
    for (std::size_t r = 0; r < n; ++r) {
        const double lambda = reduced[r * n + r];
        require(lambda > 0.0, "coupled body mode has a nonpositive stiffness eigenvalue");
        const double omega = std::sqrt(lambda);
        const double frequency = omega / (2.0 * pi);
        if (frequency >= design.max_frequency_hz) {
            continue;
        }
        // phi = L^-T y_r (back substitution), M-orthonormal before scaling.
        std::size_t dominant = 0;
        for (std::size_t i = 0; i < n; ++i) {
            if (std::abs(y[i * n + r]) > std::abs(y[dominant * n + r])) {
                dominant = i;
            }
        }
        for (std::size_t i = n; i-- > 0;) {
            double sum = y[i * n + r];
            for (std::size_t k = i + 1; k < n; ++k) {
                sum -= lower[k * n + i] * phi[k];
            }
            phi[i] = sum / lower[i * n + i];
        }
        const double scale = phi[dominant];
        require(scale != 0.0 && std::isfinite(scale), "degenerate coupled mode normalization");
        for (double& value : phi) {
            value /= scale;
        }
        double modal_mass = 0.0;
        double modal_damping = 0.0;
        for (std::size_t i = 0; i < n; ++i) {
            for (std::size_t j = 0; j < n; ++j) {
                modal_mass += phi[i] * mass[i * n + j] * phi[j];
                modal_damping += phi[i] * damping[i * n + j] * phi[j];
            }
        }
        // Projected (diagonal) modal damping: an approximation that ignores the
        // off-diagonal damping coupling between coupled modes.
        const double zeta = modal_damping / (2.0 * omega * modal_mass);
        require(ratio(zeta), "projected modal damping ratio must be in [0, 1)");

        BodyMode mode;
        mode.frequency_hz = frequency;
        mode.damping_ratio = zeta;
        mode.modal_mass_kg = modal_mass;
        for (std::size_t s = 0; s < string_count; ++s) {
            double w = 0.0;
            for (std::size_t i = 0; i < plate_count; ++i) {
                w += phi[i] * mode_shape(basis[i].m, basis[i].n, a, b, design.bridge_point_m[s][0],
                                         design.bridge_point_m[s][1]);
            }
            mode.bridge_participation[s][static_cast<std::size_t>(Polarization::Vertical)] = w;
            mode.bridge_participation[s][static_cast<std::size_t>(Polarization::Horizontal)] = 0.0;
        }
        double q = 0.0;
        double q1 = 0.0;
        for (std::size_t i = 0; i < n; ++i) {
            q += phi[i] * volume[i];
            q1 += phi[i] * moment[i];
        }
        for (std::size_t o = 0; o < observation_count; ++o) {
            mode.radiation_weight[o] = rho0 / (4.0 * pi * design.observation.distance_m[o]) *
                                       (q + design.observation.dipole_weight_per_m[o] * q1);
        }
        body.modes.push_back(mode);
    }
    if (design.lateral.enabled && design.lateral.frequency_hz < design.max_frequency_hz) {
        BodyMode mode;
        mode.frequency_hz = design.lateral.frequency_hz;
        mode.damping_ratio = design.lateral.damping_ratio;
        mode.modal_mass_kg = design.lateral.modal_mass_kg;
        for (std::size_t s = 0; s < string_count; ++s) {
            mode.bridge_participation[s][static_cast<std::size_t>(Polarization::Horizontal)] = 1.0;
        }
        for (std::size_t o = 0; o < observation_count; ++o) {
            mode.radiation_weight[o] = rho0 / (4.0 * pi * design.observation.distance_m[o]) *
                                       design.lateral.radiation_area_m2;
        }
        body.modes.push_back(mode);
    }
    std::stable_sort(
        body.modes.begin(), body.modes.end(),
        [](const BodyMode& l, const BodyMode& r) { return l.frequency_hz < r.frequency_hz; });
    require(!body.modes.empty(), "no body mode lies below max_frequency_hz");
    return body;
}

std::complex<double> bridge_mobility(const BodyConfig& body, std::size_t s_out, Polarization p_out,
                                     std::size_t s_in, Polarization p_in, double frequency_hz) {
    if (s_out >= string_count || s_in >= string_count) {
        throw std::invalid_argument("bridge_mobility: string index must be in [0, 3]");
    }
    const auto po = static_cast<std::size_t>(p_out);
    const auto pin = static_cast<std::size_t>(p_in);
    if (po >= polarization_count || pin >= polarization_count) {
        throw std::invalid_argument("bridge_mobility: invalid polarization");
    }
    if (!std::isfinite(frequency_hz) || frequency_hz < 0.0) {
        throw std::invalid_argument(
            "bridge_mobility: frequency must be finite and nonnegative (Hz)");
    }
    const double w = 2.0 * pi * frequency_hz;
    std::complex<double> y{0.0, 0.0};
    for (const BodyMode& mode : body.modes) {
        if (!positive(mode.frequency_hz) || !positive(mode.modal_mass_kg) ||
            !ratio(mode.damping_ratio)) {
            throw std::invalid_argument("bridge_mobility: body mode must have positive "
                                        "frequency/mass and damping in [0, 1)");
        }
        const double big_w = 2.0 * pi * mode.frequency_hz;
        const std::complex<double> denominator(big_w * big_w - w * w,
                                               2.0 * mode.damping_ratio * big_w * w);
        y += std::complex<double>(0.0, w) * mode.bridge_participation[s_out][po] *
             mode.bridge_participation[s_in][pin] / (mode.modal_mass_kg * denominator);
    }
    return y;
}

} // namespace setar::instrument
