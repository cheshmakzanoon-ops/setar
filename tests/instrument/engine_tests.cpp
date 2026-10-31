// Independent tests of the four-string prototype engine (setar::instrument::Instrument).
// Configurations are hand-built SYNTHETIC fixtures; they are not setar measurements.
// Oracles are independent of the engine code: closed-form stiff-string frequencies,
// a dense Jacobi eigen-solution of the same truncated linear model, quasi-static point
// stiffness of a pinned string, exact bitwise identities and the energy ledger.
#include "setar/instrument/instrument.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <complex>
#include <cstdint>
#include <exception>
#include <functional>
#include <iostream>
#include <limits>
#include <numbers>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace {
using namespace setar::instrument;
constexpr double pi = std::numbers::pi_v<double>;

void check(bool condition, std::string_view message) {
    if (!condition) {
        throw std::runtime_error(std::string(message));
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

double cents(double ratio) { return 1200.0 * std::log2(ratio); }

// ---------------------------------------------------------------------------------------
// Synthetic fixtures
StringConfig fixture_string(double tension_n, double density) {
    StringConfig s;
    s.label = "synthetic test string";
    s.total_length_m = 0.70;
    s.bridge_position_m = 0.66;
    s.tension_n = tension_n;
    s.linear_density_kg_per_m = density;
    s.bending_stiffness_n_m2 = 2.0e-5;
    s.action_at_nut_m = 0.6e-3;
    s.action_at_bridge_m = 2.0e-3;
    return s;
}

double fret_position(double speaking, int semitones) {
    return speaking * (1.0 - std::pow(2.0, -static_cast<double>(semitones) / 12.0));
}

// Lossless unless `lossy`; body of three plate-like vertical modes and one lateral mode.
InstrumentConfig fixture(bool lossy, bool with_body) {
    InstrumentConfig c;
    c.label = "base";
    c.strings[0] = fixture_string(40.0, 8.0e-4);
    c.strings[1] = fixture_string(32.0, 9.0e-4);
    c.strings[2] = fixture_string(28.0, 1.6e-3);
    c.strings[3] = fixture_string(40.0, 8.0e-4);
    for (int k = 1; k <= 7; ++k) {
        c.frets.push_back(FretConfig{fret_position(0.66, k), "12-TET test fret"});
    }
    if (with_body) {
        const std::array<double, 3> freq{{185.0, 420.0, 960.0}};
        const std::array<double, 3> mass{{0.030, 0.020, 0.012}};
        for (std::size_t j = 0; j < 3; ++j) {
            BodyMode m;
            m.frequency_hz = freq[j];
            m.modal_mass_kg = mass[j];
            m.damping_ratio = lossy ? 0.02 : 0.0;
            for (std::size_t s = 0; s < string_count; ++s) {
                m.bridge_participation[s][0] = 0.9 - 0.1 * static_cast<double>(j) +
                                               0.02 * static_cast<double>(s);
                m.bridge_participation[s][1] = 0.0;
            }
            m.radiation_weight = {{0.05 / static_cast<double>(j + 1), 0.04}};
            c.body.modes.push_back(m);
        }
        BodyMode lateral;
        lateral.frequency_hz = 1500.0;
        lateral.modal_mass_kg = 0.02;
        lateral.damping_ratio = lossy ? 0.05 : 0.0;
        for (std::size_t s = 0; s < string_count; ++s) {
            lateral.bridge_participation[s][1] = 1.0;
        }
        lateral.radiation_weight = {{0.0, 0.01}};
        c.body.modes.push_back(lateral);
    }
    c.bridge.stiffness_n_per_m = {{2.0e6, 2.0e6}};
    c.bridge.damping_n_s_per_m = {{lossy ? 0.5 : 0.0, lossy ? 0.5 : 0.0}};
    c.contacts.fret = ContactLaw{1.0e9, 1.5, lossy ? 0.2 : 0.0};
    c.contacts.finger = ContactLaw{5.0e4, 1.5, lossy ? 2.0 : 0.0};
    c.contacts.nail = ContactLaw{2.0e6, 1.5, lossy ? 0.05 : 0.0};
    c.contacts.finger_lateral_damping_n_s_per_m = lossy ? 1.0 : 0.0;
    c.contacts.fret_stick_stiffness_n_per_m = 5.0e4;
    c.contacts.fret_stick_damping_n_s_per_m = lossy ? 0.5 : 0.0;
    if (lossy) {
        for (auto& s : c.strings) {
            s.loss = StringLossLaw{0.8, 2.0e-4, 1.0e-9};
        }
    }
    c.max_mode_frequency_hz = 6000.0;
    return c;
}

ProcessSpec fixture_spec(double rate, bool ledger) {
    ProcessSpec p;
    p.sample_rate_hz = rate;
    p.max_block_frames = 512;
    p.max_events_per_block = 32;
    p.max_interaction_channels = 40;
    p.max_newton_iterations = 50;
    p.newton_tolerance_m = 1.0e-14;
    p.enable_energy_ledger = ledger;
    return p;
}

GestureEvent nail(std::uint8_t string_index, double x, double angle, double speed, double force) {
    GestureEvent e;
    e.kind = GestureKind::NailStroke;
    e.string_index = string_index;
    e.position_m = x;
    e.angle_rad = angle;
    e.speed_m_per_s = speed;
    e.force_n = force;
    e.gap_m = 2.0e-4;
    e.travel_m = 8.0e-3;
    return e;
}

GestureEvent press(const InstrumentConfig& c, std::uint8_t string_index, double fret_x, double depth) {
    GestureEvent e;
    e.kind = GestureKind::FingerPress;
    e.string_index = string_index;
    e.position_m = fret_x - 0.008;
    e.height_m = fret_top_height_m(c.strings[string_index], e.position_m) - depth;
    e.speed_m_per_s = 0.15;
    e.gap_m = 5.0e-4;
    return e;
}

GestureEvent release(std::uint8_t string_index) {
    GestureEvent e;
    e.kind = GestureKind::FingerRelease;
    e.string_index = string_index;
    e.speed_m_per_s = 0.15;
    e.gap_m = 5.0e-4;
    return e;
}

GestureEvent move(std::uint8_t string_index, double x, double speed) {
    GestureEvent e;
    e.kind = GestureKind::FingerMove;
    e.string_index = string_index;
    e.position_m = x;
    e.speed_m_per_s = speed;
    return e;
}

struct Timed {
    std::uint64_t sample;
    GestureEvent event;
};

// Renders `frames` samples in blocks; `on_block(start, left, right)` after each block.
void render(Instrument& inst, std::vector<Timed> events, std::uint64_t frames, std::uint32_t block,
            const std::function<void(std::uint64_t, std::span<const float>, std::span<const float>,
                                     const ProcessSummary&)>& on_block) {
    std::stable_sort(events.begin(), events.end(),
                     [](const Timed& a, const Timed& b) { return a.sample < b.sample; });
    std::vector<float> left(block), right(block);
    std::vector<GestureEvent> local;
    std::size_t next = 0;
    std::uint32_t sequence = 0;
    for (std::uint64_t start = 0; start < frames; start += block) {
        const auto n = static_cast<std::size_t>(std::min<std::uint64_t>(block, frames - start));
        local.clear();
        while (next < events.size() && events[next].sample < start + n) {
            GestureEvent e = events[next].event;
            e.sample_offset = static_cast<std::uint32_t>(events[next].sample - start);
            e.sequence = sequence++;
            local.push_back(e);
            ++next;
        }
        const ProcessSummary summary =
            inst.process(local, std::span<float>(left.data(), n), std::span<float>(right.data(), n));
        if (on_block) {
            on_block(start, std::span<const float>(left.data(), n), std::span<const float>(right.data(), n),
                     summary);
        }
    }
}

double stored(const EnergyLedger& l) {
    double out = l.body_energy_j + l.interaction_potential_j;
    for (double e : l.string_energy_j) {
        out += e;
    }
    return out;
}

// Independent frequency estimator: Hann-windowed DFT magnitude maximized over [lo, hi]
// by a dense scan followed by golden-section refinement.
double peak_frequency(const std::vector<double>& x, double rate, double lo, double hi) {
    const std::size_t n = x.size();
    std::vector<double> w(n);
    for (std::size_t k = 0; k < n; ++k) {
        w[k] = x[k] * (0.5 - 0.5 * std::cos(2.0 * pi * static_cast<double>(k) / static_cast<double>(n - 1)));
    }
    auto magnitude = [&](double f) {
        const double step = 2.0 * pi * f / rate;
        std::complex<double> rot(std::cos(step), -std::sin(step));
        std::complex<double> phase(1.0, 0.0);
        std::complex<double> sum(0.0, 0.0);
        for (std::size_t k = 0; k < n; ++k) {
            sum += w[k] * phase;
            phase *= rot;
            if ((k & 1023U) == 0) {
                phase /= std::abs(phase);
            }
        }
        return std::abs(sum);
    };
    const double grid = rate / static_cast<double>(n) / 4.0;
    double best_f = lo;
    double best = -1.0;
    for (double f = lo; f <= hi; f += grid) {
        const double m = magnitude(f);
        if (m > best) {
            best = m;
            best_f = f;
        }
    }
    double a = best_f - grid;
    double b = best_f + grid;
    const double g = (std::sqrt(5.0) - 1.0) / 2.0;
    double c1 = b - g * (b - a);
    double c2 = a + g * (b - a);
    double m1 = magnitude(c1);
    double m2 = magnitude(c2);
    for (int i = 0; i < 40; ++i) {
        if (m1 > m2) {
            b = c2;
            c2 = c1;
            m2 = m1;
            c1 = b - g * (b - a);
            m1 = magnitude(c1);
        } else {
            a = c1;
            c1 = c2;
            m1 = m2;
            c2 = a + g * (b - a);
            m2 = magnitude(c2);
        }
    }
    return 0.5 * (a + b);
}

double stiff_string_frequency(const StringConfig& s, double length) {
    const double k = pi / length;
    return std::sqrt(s.tension_n / s.linear_density_kg_per_m * k * k +
                     s.bending_stiffness_n_m2 / s.linear_density_kg_per_m * k * k * k * k) /
           (2.0 * pi);
}

// Eigenvalues of a dense symmetric matrix by cyclic Jacobi rotations (independent of
// every engine routine).
std::vector<double> jacobi_eigenvalues(std::vector<double> a, std::size_t n) {
    for (int sweep = 0; sweep < 100; ++sweep) {
        double off = 0.0;
        for (std::size_t p = 0; p < n; ++p) {
            for (std::size_t q = p + 1; q < n; ++q) {
                off += a[p * n + q] * a[p * n + q];
            }
        }
        if (off < 1.0e-30) {
            break;
        }
        for (std::size_t p = 0; p < n; ++p) {
            for (std::size_t q = p + 1; q < n; ++q) {
                const double apq = a[p * n + q];
                if (std::abs(apq) < 1.0e-300) {
                    continue;
                }
                const double theta = (a[q * n + q] - a[p * n + p]) / (2.0 * apq);
                const double t = (theta >= 0.0 ? 1.0 : -1.0) / (std::abs(theta) + std::sqrt(theta * theta + 1.0));
                const double c = 1.0 / std::sqrt(t * t + 1.0);
                const double sn = t * c;
                for (std::size_t k = 0; k < n; ++k) {
                    const double akp = a[k * n + p];
                    const double akq = a[k * n + q];
                    a[k * n + p] = c * akp - sn * akq;
                    a[k * n + q] = sn * akp + c * akq;
                }
                for (std::size_t k = 0; k < n; ++k) {
                    const double apk = a[p * n + k];
                    const double aqk = a[q * n + k];
                    a[p * n + k] = c * apk - sn * aqk;
                    a[q * n + k] = sn * apk + c * aqk;
                }
            }
        }
    }
    std::vector<double> out(n);
    for (std::size_t i = 0; i < n; ++i) {
        out[i] = a[i * n + i];
    }
    std::sort(out.begin(), out.end());
    return out;
}

// Truncated linear model of one polarization of string s: N sine modes on the total
// length plus a grounded bridge spring K. Returns the eigenfrequency nearest `near_hz`.
double truncated_model_frequency(const StringConfig& s, std::size_t modes, double bridge_k, double near_hz) {
    // A = K/m with K_ab = delta_ab * m*w_a^2 + K_bridge*phi_a*phi_b, m = mu*L/2,
    // m*w_a^2 = (L/2)*(T k_a^2 + EI k_a^4).
    const double m = s.linear_density_kg_per_m * s.total_length_m / 2.0;
    std::vector<double> k(modes * modes, 0.0);
    std::vector<double> phi(modes);
    for (std::size_t n = 0; n < modes; ++n) {
        phi[n] = std::sin(static_cast<double>(n + 1) * pi * s.bridge_position_m / s.total_length_m);
    }
    for (std::size_t a = 0; a < modes; ++a) {
        for (std::size_t b = 0; b < modes; ++b) {
            k[a * modes + b] = bridge_k * phi[a] * phi[b] / m;
        }
        const double kn = static_cast<double>(a + 1) * pi / s.total_length_m;
        k[a * modes + a] += s.total_length_m / 2.0 *
                            (s.tension_n * kn * kn + s.bending_stiffness_n_m2 * kn * kn * kn * kn) / m;
    }
    const auto eig = jacobi_eigenvalues(k, modes);
    double best = 0.0;
    double best_d = std::numeric_limits<double>::infinity();
    for (double lambda : eig) {
        const double f = std::sqrt(std::max(lambda, 0.0)) / (2.0 * pi);
        if (std::abs(f - near_hz) < best_d) {
            best_d = std::abs(f - near_hz);
            best = f;
        }
    }
    return best;
}

// Same truncated model, but in the engine's discrete time: with s(w) = 4 sin^2(w k/2)/k^2
// (each free mode exact) and the bridge spring force -K (z[n+1] + z[n-1])/2, a coupled
// mode of frequency W satisfies 1 + K cos(W k)/m * sum_n phi_n^2 / (s(w_n) - s(W)) = 0.
// Roots are bracketed between consecutive poles and found by bisection.
double discrete_model_frequency(const StringConfig& s, std::size_t modes, double bridge_k, double rate,
                                double near_hz) {
    const double k = 1.0 / rate;
    const double m = s.linear_density_kg_per_m * s.total_length_m / 2.0;
    std::vector<double> pole(modes);
    std::vector<double> phi2(modes);
    for (std::size_t n = 0; n < modes; ++n) {
        const double kn = static_cast<double>(n + 1) * pi / s.total_length_m;
        const double w = std::sqrt((s.tension_n * kn * kn + s.bending_stiffness_n_m2 * kn * kn * kn * kn) /
                                   s.linear_density_kg_per_m);
        pole[n] = 4.0 * std::pow(std::sin(0.5 * w * k), 2) / (k * k);
        const double ph = std::sin(kn * s.bridge_position_m);
        phi2[n] = ph * ph;
    }
    auto g = [&](double sigma) {
        double sum = 0.0;
        for (std::size_t n = 0; n < modes; ++n) {
            sum += phi2[n] / (pole[n] - sigma);
        }
        return 1.0 + bridge_k * (1.0 - 0.5 * sigma * k * k) / m * sum;
    };
    double best = 0.0;
    double best_d = std::numeric_limits<double>::infinity();
    for (std::size_t j = 0; j + 1 < modes && j < 12; ++j) {
        double lo = pole[j] * (1.0 + 1e-12);
        double hi = pole[j + 1] * (1.0 - 1e-12);
        if (!(g(lo) < 0.0 && g(hi) > 0.0)) {
            continue;
        }
        for (int it = 0; it < 200; ++it) {
            const double mid = 0.5 * (lo + hi);
            (g(mid) < 0.0 ? lo : hi) = mid;
        }
        const double sigma = 0.5 * (lo + hi);
        const double f = 2.0 / k * std::asin(std::sqrt(sigma) * k / 2.0) / (2.0 * pi);
        if (std::abs(f - near_hz) < best_d) {
            best_d = std::abs(f - near_hz);
            best = f;
        }
    }
    return best;
}

// Records one string probe (horizontal) per sample, from sample `skip`.
std::vector<double> probe_series(Instrument& inst, const std::vector<Timed>& events, std::uint64_t frames,
                                 std::uint64_t skip, std::size_t string_index, Polarization p, double x) {
    std::vector<double> out;
    out.reserve(static_cast<std::size_t>(frames - skip));
    render(inst, events, frames, 1,
           [&](std::uint64_t start, std::span<const float>, std::span<const float>, const ProcessSummary& sm) {
               check(sm.first_fault == FaultCode::None, "probe render must not fault");
               if (start >= skip) {
                   out.push_back(inst.string_displacement_m(string_index, p, x));
               }
           });
    return out;
}

// ---------------------------------------------------------------------------------------
void config_validation() {
    const InstrumentConfig base = fixture(true, true);
    const ProcessSpec spec = fixture_spec(48000.0, false);
    Instrument inst;
    inst.prepare(base, spec);
    check(inst.prepared(), "valid fixture prepares");
    using Mutation = std::function<void(InstrumentConfig&)>;
    const std::vector<std::pair<std::string, Mutation>> mutations{
        {"total length", [](InstrumentConfig& c) { c.strings[0].total_length_m = 0.0; }},
        {"bridge beyond tail", [](InstrumentConfig& c) { c.strings[1].bridge_position_m = 0.71; }},
        {"tension", [](InstrumentConfig& c) { c.strings[2].tension_n = -1.0; }},
        {"density NaN", [](InstrumentConfig& c) { c.strings[3].linear_density_kg_per_m = std::nan(""); }},
        {"negative EI", [](InstrumentConfig& c) { c.strings[0].bending_stiffness_n_m2 = -1.0e-6; }},
        {"negative loss", [](InstrumentConfig& c) { c.strings[0].loss.decay_constant_per_s = -0.1; }},
        {"loss k2 inf", [](InstrumentConfig& c) {
             c.strings[0].loss.decay_wavenumber_squared_m2_per_s = std::numeric_limits<double>::infinity();
         }},
        {"loss w2", [](InstrumentConfig& c) { c.strings[1].loss.decay_frequency_squared_s = -1.0e-9; }},
        {"action nut", [](InstrumentConfig& c) { c.strings[0].action_at_nut_m = 0.0; }},
        {"action bridge", [](InstrumentConfig& c) { c.strings[0].action_at_bridge_m = -1.0e-3; }},
        {"fret order", [](InstrumentConfig& c) { std::swap(c.frets[1].position_m, c.frets[2].position_m); }},
        {"fret beyond bridge", [](InstrumentConfig& c) { c.frets.back().position_m = 0.67; }},
        {"fret zero", [](InstrumentConfig& c) { c.frets.front().position_m = 0.0; }},
        {"body frequency", [](InstrumentConfig& c) { c.body.modes[0].frequency_hz = 0.0; }},
        {"body damping", [](InstrumentConfig& c) { c.body.modes[0].damping_ratio = 1.0; }},
        {"body mass", [](InstrumentConfig& c) { c.body.modes[1].modal_mass_kg = -0.01; }},
        {"participation NaN", [](InstrumentConfig& c) { c.body.modes[1].bridge_participation[2][0] = std::nan(""); }},
        {"radiation inf", [](InstrumentConfig& c) {
             c.body.modes[1].radiation_weight[1] = std::numeric_limits<double>::infinity();
         }},
        {"bridge stiffness", [](InstrumentConfig& c) { c.bridge.stiffness_n_per_m[1] = 0.0; }},
        {"bridge damping", [](InstrumentConfig& c) { c.bridge.damping_n_s_per_m[0] = -1.0; }},
        {"fret stiffness", [](InstrumentConfig& c) { c.contacts.fret.stiffness = 0.0; }},
        {"finger exponent", [](InstrumentConfig& c) { c.contacts.finger.exponent = 0.5; }},
        {"nail damping", [](InstrumentConfig& c) { c.contacts.nail.damping_n_s_per_m = -0.1; }},
        {"lateral damping", [](InstrumentConfig& c) { c.contacts.finger_lateral_damping_n_s_per_m = -1.0; }},
        {"stick stiffness", [](InstrumentConfig& c) { c.contacts.fret_stick_stiffness_n_per_m = std::nan(""); }},
        {"stick damping", [](InstrumentConfig& c) { c.contacts.fret_stick_damping_n_s_per_m = -1.0; }},
        {"mode cap", [](InstrumentConfig& c) { c.max_mode_frequency_hz = 0.0; }},
        {"mode cap below fundamental", [](InstrumentConfig& c) { c.max_mode_frequency_hz = 50.0; }},
        {"overdamped fundamental", [](InstrumentConfig& c) { c.strings[2].loss.decay_constant_per_s = 2000.0; }},
    };
    for (const auto& [name, mutate] : mutations) {
        InstrumentConfig bad = base;
        bad.label = "bad";
        mutate(bad);
        rejected([&] { inst.prepare(bad, spec); }, "config mutation must be rejected: " + name);
        check(inst.prepared() && inst.config().label == "base", "failed prepare is transactional: " + name);
    }
    using SpecMutation = std::function<void(ProcessSpec&)>;
    const std::vector<std::pair<std::string, SpecMutation>> spec_mutations{
        {"rate low", [](ProcessSpec& p) { p.sample_rate_hz = 8000.0; }},
        {"rate high", [](ProcessSpec& p) { p.sample_rate_hz = 384000.0; }},
        {"rate NaN", [](ProcessSpec& p) { p.sample_rate_hz = std::nan(""); }},
        {"block", [](ProcessSpec& p) { p.max_block_frames = 0; }},
        {"events", [](ProcessSpec& p) { p.max_events_per_block = 0; }},
        {"channels low", [](ProcessSpec& p) { p.max_interaction_channels = 7; }},
        {"channels high", [](ProcessSpec& p) { p.max_interaction_channels = 257; }},
        {"newton", [](ProcessSpec& p) { p.max_newton_iterations = 0; }},
        {"tolerance", [](ProcessSpec& p) { p.newton_tolerance_m = 0.0; }},
        {"gain", [](ProcessSpec& p) { p.output_gain_per_pa = std::numeric_limits<double>::infinity(); }},
    };
    for (const auto& [name, mutate] : spec_mutations) {
        ProcessSpec bad = spec;
        mutate(bad);
        rejected([&] { inst.prepare(base, bad); }, "spec mutation must be rejected: " + name);
        check(inst.prepared() && inst.spec().sample_rate_hz == 48000.0, "failed prepare keeps spec: " + name);
    }
    rejected([&] { validate(fixture_spec(1000.0, false)); }, "validate(ProcessSpec) rejects rate");
    InstrumentConfig bad = base;
    bad.strings[0].tension_n = 0.0;
    rejected([&] { validate(bad); }, "validate(InstrumentConfig) rejects tension");
    const PreparedInfo& info = inst.info();
    check(info.body_modes == 4 && info.string_modes[0] > 10 && info.total_degrees_of_freedom > 80,
          "prepared info reports retained modes");
}

void ledger_scenario(bool lossy) {
    const InstrumentConfig c = fixture(lossy, true);
    Instrument inst;
    inst.prepare(c, fixture_spec(48000.0, true));
    const double rate = 48000.0;
    const std::vector<Timed> events{
        {0, press(c, 1, c.frets[1].position_m, 1.0e-3)},
        {static_cast<std::uint64_t>(0.04 * rate), nail(1, 0.55, 0.4, 0.4, 0.6)},
        {static_cast<std::uint64_t>(0.06 * rate), nail(0, 0.52, -0.3, 0.5, 0.8)},
        {static_cast<std::uint64_t>(0.08 * rate), nail(2, 0.50, 2.8, 0.5, 0.7)},
        {static_cast<std::uint64_t>(0.30 * rate), release(1)},
    };
    double peak = 0.0;
    double worst = 0.0;
    double last_dissipation = 0.0;
    double last_stored_after_actuators = std::numeric_limits<double>::infinity();
    render(inst, events, static_cast<std::uint64_t>(0.6 * rate), 256,
           [&](std::uint64_t start, std::span<const float> l, std::span<const float>, const ProcessSummary& sm) {
               check(sm.first_fault == FaultCode::None, "ledger scenario must not fault");
               check(sm.rejected_events == 0, "no event may be rejected");
               for (float v : l) {
                   check(std::isfinite(v), "output must stay finite");
               }
               const EnergyLedger led = inst.ledger();
               const double e = stored(led);
               peak = std::max(peak, e);
               worst = std::max(worst, std::abs(led.residual_j));
               check(led.dissipation_j >= last_dissipation - 1e-18, "cumulative dissipation is nondecreasing");
               last_dissipation = led.dissipation_j;
               const InstrumentSnapshot snap = inst.snapshot();
               bool idle = true;
               for (const auto& s : snap.strings) {
                   idle = idle && s.nail_phase != NailPhase::InContact && s.nail_phase != NailPhase::Approaching &&
                          !s.finger_active[0] && !s.finger_active[1];
               }
               if (lossy && idle && start > static_cast<std::uint64_t>(0.35 * rate)) {
                   // No actuator: stored energy may only decrease (switching is reported separately).
                   check(e <= last_stored_after_actuators + std::abs(led.switching_j) + 1e-15,
                         "unforced lossy instrument does not gain energy");
                   last_stored_after_actuators = e;
               }
           });
    const EnergyLedger led = inst.ledger();
    std::cout << (lossy ? "lossy" : "lossless") << " peak_stored_j=" << peak << " residual_j=" << led.residual_j
              << " max_abs_residual_j=" << worst << " work_j=" << led.external_work_j
              << " dissipation_j=" << led.dissipation_j << " switching_j=" << led.switching_j << '\n';
    check(peak > 1.0e-6, "gestures inject a nontrivial amount of energy");
    check(worst <= 1.0e-9 * peak, "energy ledger closes to roundoff relative to peak stored energy");
    check(led.external_work_j > 0.0, "prescribed nail motion performs positive net work");
    check(std::abs(led.switching_j) <= 1.0e-3 * peak, "channel switching energy is small and reported");
    if (lossy) {
        check(stored(led) < 0.8 * peak, "lossy instrument decays");
        check(led.dissipation_j > 0.1 * peak, "losses dissipate a substantial share");
    }
}

void lossless_energy_ledger() { ledger_scenario(false); }
void lossy_energy_ledger() { ledger_scenario(true); }

void open_string_pitch() {
    const double rate = 48000.0;
    double previous_error = std::numeric_limits<double>::infinity();
    for (double cap : {5000.0, 10000.0, 20000.0}) {
        InstrumentConfig c = fixture(false, false);
        c.max_mode_frequency_hz = cap;
        c.bridge.stiffness_n_per_m = {{1.0e8, 1.0e8}};
        Instrument inst;
        inst.prepare(c, fixture_spec(rate, false));
        const std::size_t modes = inst.info().string_modes[0];
        const std::vector<Timed> events{{0, nail(0, 0.55, 0.0, 0.5, 0.5)}};
        const auto series = probe_series(inst, events, static_cast<std::uint64_t>(1.05 * rate),
                                         static_cast<std::uint64_t>(0.05 * rate), 0, Polarization::Horizontal, 0.30);
        const double expected = stiff_string_frequency(c.strings[0], c.strings[0].bridge_position_m);
        const double measured = peak_frequency(series, rate, 0.9 * expected, 1.1 * expected);
        const double truncated = truncated_model_frequency(c.strings[0], modes, 1.0e8, expected);
        const double discrete = discrete_model_frequency(c.strings[0], modes, 1.0e8, rate, expected);
        const double model_error = cents(measured / discrete);
        const double warping = cents(discrete / truncated);
        const double continuum_error = cents(measured / expected);
        // Error budget: point constraints in an N-mode sine basis converge from above as
        // ~180/N cents (Rayleigh-Ritz upper bound, dense eigen study with N = 25..400);
        // 250/N is the stated budget. The scheme is exact per free mode, but a coupled
        // constraint sees the warped modal stiffness s(w_n) of high modes, which lowers the
        // pitch by up to ~1 cent at a 20 kHz cap (checked against the discrete oracle).
        const double budget = 250.0 / static_cast<double>(modes);
        std::cout << "cap_hz=" << cap << " modes=" << modes << " measured_hz=" << measured
                  << " discrete_model_hz=" << discrete << " truncated_continuous_time_hz=" << truncated
                  << " continuum_hz=" << expected << " vs_discrete_model_cents=" << model_error
                  << " time_warping_cents=" << warping << " vs_continuum_cents=" << continuum_error
                  << " budget_cents=" << budget << '\n';
        check(std::abs(model_error) <= 0.05, "engine pitch equals its exact discrete-time model");
        check(warping <= 0.0 && warping >= -1.5, "coupled time-discretization warping is flat and bounded");
        check(continuum_error >= -0.2 && continuum_error <= budget, "open pitch within truncation budget");
        check(continuum_error < previous_error, "pitch error decreases with retained modes");
        previous_error = continuum_error;
    }
}

void fretted_pitch() {
    const double rate = 48000.0;
    InstrumentConfig c = fixture(false, false);
    c.max_mode_frequency_hz = 20000.0;
    c.bridge.stiffness_n_per_m = {{1.0e8, 1.0e8}};
    Instrument inst;
    inst.prepare(c, fixture_spec(rate, false));
    const std::size_t modes = inst.info().string_modes[0];
    const double fret_x = c.frets[4].position_m; // fifth 12-TET test fret
    const std::vector<Timed> events{
        {0, press(c, 0, fret_x, 1.0e-3)},
        {static_cast<std::uint64_t>(0.05 * rate), nail(0, 0.55, 0.0, 0.5, 0.5)},
    };
    const auto series = probe_series(inst, events, static_cast<std::uint64_t>(1.1 * rate),
                                     static_cast<std::uint64_t>(0.1 * rate), 0, Polarization::Horizontal, 0.45);
    const double expected = stiff_string_frequency(c.strings[0], c.strings[0].bridge_position_m - fret_x);
    const double measured = peak_frequency(series, rate, 0.9 * expected, 1.1 * expected);
    const double error = cents(measured / expected);
    // Budget: two point constraints (bridge, fret) each ~180/N cents sharp; the fret is a
    // stiff one-sided contact (1e9 N/m^1.5) and the horizontal polarization is held by the
    // 5e4 N/m stick spring, whose compliance lengthens the segment (flat, < 3 cents here).
    const double budget = 2.0 * 250.0 / static_cast<double>(modes);
    std::cout << "modes=" << modes << " fretted_measured_hz=" << measured << " continuum_hz=" << expected
              << " error_cents=" << error << " budget_cents=+" << budget << "/-3" << '\n';
    check(error <= budget && error >= -3.0, "fretted pitch within the stated error budget");
    const InstrumentSnapshot snap = inst.snapshot();
    check(snap.strings[0].finger_active[0] && snap.strings[0].finger_force_n[0] > 0.1,
          "finger pad still presses the string");
}

void event_ordering() {
    const InstrumentConfig c = fixture(true, true);
    Instrument inst;
    std::vector<float> l(64), r(64);
    {
        const ProcessSummary sm = inst.process({}, l, r);
        check(sm.first_fault == FaultCode::NotPrepared, "unprepared process reports NotPrepared");
    }
    ProcessSpec spec = fixture_spec(48000.0, false);
    spec.max_events_per_block = 4;
    inst.prepare(c, spec);
    auto ev = [&](std::uint32_t offset, std::uint32_t seq) {
        GestureEvent e = nail(0, 0.5, 0.0, 0.5, 0.5);
        e.sample_offset = offset;
        e.sequence = seq;
        return e;
    };
    {
        const std::vector<GestureEvent> events{ev(5, 0), ev(3, 1)};
        const ProcessSummary sm = inst.process(events, l, r);
        check(sm.rejected_events == 1 && sm.first_fault == FaultCode::InvalidEvent, "out-of-order offset rejected");
    }
    {
        const std::vector<GestureEvent> events{ev(5, 7), ev(5, 6)};
        const ProcessSummary sm = inst.process(events, l, r);
        check(sm.rejected_events == 1, "descending sequence at equal offset rejected");
    }
    {
        const std::vector<GestureEvent> events{ev(64, 0)};
        const ProcessSummary sm = inst.process(events, l, r);
        check(sm.rejected_events == 1 && sm.processed_frames == 64, "offset beyond block rejected");
    }
    {
        std::vector<GestureEvent> bad;
        GestureEvent e = ev(1, 0);
        e.string_index = 4;
        bad.push_back(e);
        e = ev(2, 1);
        e.speed_m_per_s = 0.0;
        bad.push_back(e);
        e = ev(3, 2);
        e.position_m = 0.67;
        bad.push_back(e);
        GestureEvent m = move(1, 0.2, 0.1);
        m.sample_offset = 4;
        m.sequence = 3;
        bad.push_back(m); // FingerMove on an inactive pad
        const ProcessSummary sm = inst.process(bad, l, r);
        check(sm.rejected_events == 4, "invalid event fields rejected");
    }
    {
        const std::vector<GestureEvent> events{ev(0, 0), ev(1, 1), ev(2, 2), ev(3, 3), ev(4, 4), ev(5, 5)};
        const ProcessSummary sm = inst.process(events, l, r);
        check(sm.first_fault == FaultCode::EventOverflow && sm.rejected_events == 2, "event overflow counted");
    }
    {
        std::vector<float> small(32);
        ProcessSummary sm = inst.process({}, l, small);
        check(sm.first_fault == FaultCode::InvalidBlock, "mismatched channel sizes rejected");
        std::vector<float> big(600), big2(600);
        sm = inst.process({}, big, big2);
        check(sm.first_fault == FaultCode::InvalidBlock, "oversized block rejected");
    }
    {
        // Sample accuracy: one 64-frame block with the event at offset 10 equals a 10 + 54
        // split with the event at offset 0 of the second block, bitwise.
        Instrument a;
        Instrument b;
        a.prepare(c, fixture_spec(48000.0, false));
        b.prepare(c, fixture_spec(48000.0, false));
        std::vector<float> la(64), ra(64), lb(64), rb(64);
        const std::vector<GestureEvent> one{ev(10, 0)};
        a.process(one, la, ra);
        b.process({}, std::span<float>(lb.data(), 10), std::span<float>(rb.data(), 10));
        const std::vector<GestureEvent> two{ev(0, 0)};
        b.process(two, std::span<float>(lb.data() + 10, 54), std::span<float>(rb.data() + 10, 54));
        for (int block = 0; block < 20; ++block) {
            std::vector<float> xa(64), ya(64), xb(64), yb(64);
            a.process({}, xa, ya);
            b.process({}, xb, yb);
            check(xa == xb && ya == yb, "block partition with a sample-accurate event is bitwise identical");
        }
        check(la == lb && ra == rb, "first block identical");
    }
}

void newton_failure_reporting() {
    const InstrumentConfig c = fixture(true, true);
    for (std::uint32_t iterations : {1U, 50U}) {
        ProcessSpec spec = fixture_spec(48000.0, false);
        spec.max_newton_iterations = iterations;
        spec.newton_tolerance_m = 1.0e-16;
        Instrument inst;
        inst.prepare(c, spec);
        std::uint32_t failures = 0;
        FaultCode first = FaultCode::None;
        render(inst, {{0, nail(0, 0.5, 0.3, 1.0, 1.5)}}, 4800, 256,
               [&](std::uint64_t, std::span<const float> l, std::span<const float>, const ProcessSummary& sm) {
                   check(sm.max_newton_iterations <= iterations,
                         "reported Newton iterations never exceed the declared per-solve cap");
                   failures += sm.newton_failures;
                   if (first == FaultCode::None) {
                       first = sm.first_fault;
                   }
                   for (float v : l) {
                       check(std::isfinite(v), "output finite even when Newton is capped (containment)");
                   }
               });
        std::cout << "max_iterations=" << iterations << " newton_failures=" << failures << '\n';
        if (iterations == 1) {
            check(failures > 0 && first == FaultCode::NewtonNotConverged, "capped Newton failures are reported");
        } else {
            check(failures == 0 && first == FaultCode::None, "converged contact solve reports no failure");
        }
    }
}

void nail_passivity() {
    const double rate = 48000.0;
    const InstrumentConfig c = fixture(false, true);
    Instrument inst;
    inst.prepare(c, fixture_spec(rate, true));
    const double limit = 0.7;
    double max_force = 0.0;
    double release_energy = -1.0;
    double max_after = 0.0;
    double min_after = std::numeric_limits<double>::infinity();
    double release_dissipation = 0.0;
    double release_potential = 0.0;
    render(inst, {{10, nail(0, 0.55, 0.5, 0.3, limit)}}, static_cast<std::uint64_t>(0.3 * rate), 1,
           [&](std::uint64_t, std::span<const float>, std::span<const float>, const ProcessSummary& sm) {
               check(sm.first_fault == FaultCode::None, "nail scenario must not fault");
               const InstrumentSnapshot snap = inst.snapshot();
               const EnergyLedger led = inst.ledger();
               max_force = std::max(max_force, snap.strings[0].nail_force_n);
               if (snap.strings[0].nail_phase == NailPhase::Released) {
                   const double e = stored(led);
                   if (release_energy >= 0.0) { // from the step after the slip-off booking
                       max_after = std::max(max_after, e);
                       min_after = std::min(min_after, e);
                   }
                   if (release_energy < 0.0) {
                       release_energy = e;
                       release_dissipation = led.dissipation_j;
                       release_potential = led.interaction_potential_j;
                   }
               }
           });
    const EnergyLedger led = inst.ledger();
    std::cout << "work_j=" << led.external_work_j << " stored_j=" << stored(led) << " release_loss_j="
              << release_dissipation << " max_force_n=" << max_force << " residual_j=" << led.residual_j << '\n';
    check(release_energy > 0.0, "the nail released the string");
    check(led.external_work_j > 0.0, "the nail performed positive work");
    // Lossless instrument: the only dissipation is the slip-off loss, booked when the nail
    // channel is removed; it cannot exceed the interaction potential at release.
    check(release_dissipation == 0.0 && led.dissipation_j > 0.0 && led.dissipation_j <= release_potential,
          "in a lossless instrument the only dissipation is the nail release loss");
    check(std::abs(led.external_work_j - stored(led) - led.dissipation_j + led.switching_j) <=
              1.0e-9 * led.external_work_j,
          "actuator work equals stored energy plus release loss");
    check(max_after - min_after <= 1.0e-9 * release_energy, "no energy is created after release (lossless)");
    check(max_force <= limit * 1.05, "contact force overshoots the slip threshold by less than 5%");
    check(max_force >= limit * 0.95, "release occurs at the slip threshold, not by travel");
}

void release_convergence() {
    const double limit = 0.5;
    const double x = 0.55;
    std::vector<double> displacement;
    for (double rate : {48000.0, 96000.0, 192000.0}) {
        InstrumentConfig c = fixture(false, false);
        c.max_mode_frequency_hz = 18000.0;
        c.bridge.stiffness_n_per_m = {{1.0e8, 1.0e8}};
        Instrument inst;
        inst.prepare(c, fixture_spec(rate, false));
        GestureEvent e = nail(0, x, 0.0, 0.05, limit);
        e.gap_m = 2.0e-5;
        double at_release = std::nan("");
        render(inst, {{0, e}}, static_cast<std::uint64_t>(0.1 * rate), 1,
               [&](std::uint64_t, std::span<const float>, std::span<const float>, const ProcessSummary&) {
                   if (std::isnan(at_release) && inst.snapshot().strings[0].nail_phase == NailPhase::Released) {
                       at_release = inst.string_displacement_m(0, Polarization::Horizontal, x);
                   }
               });
        check(std::isfinite(at_release), "nail released within the window");
        displacement.push_back(at_release);
        const StringConfig& s = c.strings[0];
        const double length = s.bridge_position_m;
        const double kappa = s.tension_n * length / (x * (length - x));
        const double quasi_static = limit / kappa;
        std::cout << "rate_hz=" << rate << " release_displacement_m=" << at_release
                  << " quasi_static_m=" << quasi_static << " ratio=" << at_release / quasi_static << '\n';
        // Budget: bending stiffness and modal truncation stiffen the point (a few %), and
        // the slow (0.05 m/s) stroke leaves a small dynamic oscillation about equilibrium.
        check(std::abs(at_release / quasi_static - 1.0) <= 0.05, "release displacement matches quasi-static oracle");
    }
    const double d1 = std::abs(displacement[0] - displacement[2]);
    const double d2 = std::abs(displacement[1] - displacement[2]);
    std::cout << "diff_48_192=" << d1 << " diff_96_192=" << d2 << '\n';
    check(d2 < 0.5 * d1 && d1 <= 1.0e-2 * std::abs(displacement[2]),
          "release state converges across sample rates");
}

void fret_crossing_work() {
    const double rate = 48000.0;
    const InstrumentConfig c = fixture(false, true);
    Instrument inst;
    inst.prepare(c, fixture_spec(rate, true));
    const double x_from = c.frets[1].position_m - 0.008;
    const double x_to = c.frets[3].position_m - 0.008; // crosses fret index 2
    GestureEvent slide = move(0, x_to, 0.25);
    const std::vector<Timed> events{
        {0, press(c, 0, c.frets[1].position_m, 1.0e-3)},
        {static_cast<std::uint64_t>(0.04 * rate), nail(0, 0.55, 0.2, 0.4, 0.6)},
        {static_cast<std::uint64_t>(0.15 * rate), slide},
    };
    double before = 0.0;
    double worst = 0.0;
    double peak = 0.0;
    render(inst, events, static_cast<std::uint64_t>(0.5 * rate), 128,
           [&](std::uint64_t start, std::span<const float>, std::span<const float>, const ProcessSummary& sm) {
               check(sm.first_fault == FaultCode::None, "slide scenario must not fault");
               const EnergyLedger led = inst.ledger();
               peak = std::max(peak, stored(led));
               worst = std::max(worst, std::abs(led.residual_j));
               if (start + 128 >= static_cast<std::uint64_t>(0.15 * rate) && before == 0.0) {
                   before = led.string_energy_j[0];
               }
           });
    const EnergyLedger led = inst.ledger();
    const InstrumentSnapshot snap = inst.snapshot();
    std::cout << "string_energy_before_slide_j=" << before << " after_j=" << led.string_energy_j[0]
              << " finger_x_m=" << snap.strings[0].finger_position_m[0] << " max_residual_j=" << worst
              << " switching_j=" << led.switching_j << '\n';
    (void)x_from;
    check(std::abs(snap.strings[0].finger_position_m[0] - x_to) < 1e-12, "finger reached the target");
    check(worst <= 1.0e-9 * peak, "ledger closes through the fret crossing");
    check(led.string_energy_j[0] > 0.2 * before, "fretting transition does not erase string energy");
    // Stick onsets at a new stopping fret book K/8*(dz)^2 each (declared, not physics).
    check(std::abs(led.switching_j) <= 1.0e-2 * peak, "switching energy remains small and reported");
}

void release_afterlength() {
    const double rate = 48000.0;
    InstrumentConfig c = fixture(false, false);
    c.max_mode_frequency_hz = 20000.0;
    c.bridge.stiffness_n_per_m = {{1.0e8, 1.0e8}};
    Instrument inst;
    inst.prepare(c, fixture_spec(rate, true));
    const double fret_x = c.frets[4].position_m;
    const std::uint64_t release_at = static_cast<std::uint64_t>(0.3 * rate);
    const std::vector<Timed> events{
        {0, press(c, 0, fret_x, 1.0e-3)},
        {static_cast<std::uint64_t>(0.05 * rate), nail(0, 0.55, 0.0, 0.5, 0.5)},
        {release_at, release(0)},
    };
    std::vector<double> behind;
    std::vector<double> after;
    double max_speaking = 0.0;
    double energy_before = 0.0;
    double energy_after = 0.0;
    double peak = 0.0;
    double worst = 0.0;
    render(inst, events, static_cast<std::uint64_t>(1.4 * rate), 1,
           [&](std::uint64_t start, std::span<const float>, std::span<const float>, const ProcessSummary& sm) {
               check(sm.first_fault == FaultCode::None, "release scenario must not fault");
               const EnergyLedger led = inst.ledger();
               peak = std::max(peak, stored(led));
               worst = std::max(worst, std::abs(led.residual_j));
               if (start > static_cast<std::uint64_t>(0.1 * rate) && start < release_at) {
                   behind.push_back(inst.string_displacement_m(0, Polarization::Horizontal, 0.08));
                   max_speaking = std::max(max_speaking,
                                           std::abs(inst.string_displacement_m(0, Polarization::Horizontal, 0.45)));
                   energy_before = stored(led);
               }
               if (start > release_at + static_cast<std::uint64_t>(0.1 * rate)) {
                   after.push_back(inst.string_displacement_m(0, Polarization::Vertical, 0.30));
                   if (energy_after == 0.0) {
                       energy_after = stored(led);
                   }
               }
           });
    double max_behind = 0.0;
    for (double v : behind) {
        max_behind = std::max(max_behind, std::abs(v));
    }
    const double open = stiff_string_frequency(c.strings[0], c.strings[0].bridge_position_m);
    const double measured = peak_frequency(after, rate, 0.85 * open, 1.15 * open);
    const std::size_t modes = inst.info().string_modes[0];
    const double error = cents(measured / open);
    std::cout << "behind_segment_peak_m=" << max_behind << " speaking_peak_m=" << max_speaking
              << " open_after_release_hz=" << measured << " continuum_open_hz=" << open << " error_cents=" << error
              << " energy_before_j=" << energy_before << " energy_after_j=" << energy_after
              << " max_residual_j=" << worst << '\n';
    check(max_behind > 0.0 && max_behind < 0.2 * max_speaking,
          "segment behind the finger keeps small physical motion while stopped");
    check(error >= -0.2 && error <= 250.0 / static_cast<double>(modes), "released string returns to open pitch");
    // The rising pad recovers the static pressing energy (negative actuator work), so only
    // a fraction of the stored energy remains as vibration; it must not vanish.
    check(energy_after > 0.1 * energy_before, "lifting the finger does not erase string energy");
    check(worst <= 1.0e-9 * peak, "ledger closes through the finger release");
}

double string_energy(const Instrument& inst, std::size_t s) { return inst.snapshot().strings[s].energy_j; }

void cross_string_transfer() {
    const double rate = 48000.0;
    const InstrumentConfig c = fixture(true, true); // strings 0 and 3 are identical (unison)
    Instrument inst;
    inst.prepare(c, fixture_spec(rate, false));
    double peak0 = 0.0;
    double peak1 = 0.0;
    double peak3 = 0.0;
    render(inst, {{0, nail(0, 0.55, 1.2, 0.5, 0.8)}}, static_cast<std::uint64_t>(0.8 * rate), 256,
           [&](std::uint64_t, std::span<const float>, std::span<const float>, const ProcessSummary&) {
               peak0 = std::max(peak0, string_energy(inst, 0));
               peak1 = std::max(peak1, string_energy(inst, 1));
               peak3 = std::max(peak3, string_energy(inst, 3));
           });
    std::cout << "plucked_peak_j=" << peak0 << " unison_peak_j=" << peak3 << " detuned_peak_j=" << peak1 << '\n';
    check(peak3 > 1.0e-5 * peak0, "unplucked unison string is driven through the body");
    check(peak3 > 3.0 * peak1, "sympathetic transfer is frequency selective (unison > detuned)");
    // Remove only string 3's body participation: it must stay exactly at rest.
    InstrumentConfig isolated = c;
    for (auto& m : isolated.body.modes) {
        m.bridge_participation[3] = {{0.0, 0.0}};
    }
    Instrument b;
    b.prepare(isolated, fixture_spec(rate, false));
    render(b, {{0, nail(0, 0.55, 1.2, 0.5, 0.8)}}, static_cast<std::uint64_t>(0.2 * rate), 256,
           [&](std::uint64_t, std::span<const float>, std::span<const float>, const ProcessSummary&) {
               check(string_energy(b, 3) == 0.0, "no transfer without a shared structural path");
           });
}

void uncoupled_limit() {
    const double rate = 48000.0;
    InstrumentConfig c = fixture(true, true);
    for (auto& m : c.body.modes) {
        for (std::size_t s = 1; s < string_count; ++s) {
            m.bridge_participation[s] = {{0.0, 0.0}};
        }
    }
    Instrument inst;
    inst.prepare(c, fixture_spec(rate, false));
    render(inst, {{0, nail(0, 0.55, 0.8, 0.5, 0.8)}}, static_cast<std::uint64_t>(0.3 * rate), 64,
           [&](std::uint64_t, std::span<const float>, std::span<const float>, const ProcessSummary&) {
               for (std::size_t s = 1; s < string_count; ++s) {
                   for (double x : {0.1, 0.33, 0.6, 0.68}) {
                       check(inst.string_displacement_m(s, Polarization::Vertical, x) == 0.0 &&
                                 inst.string_displacement_m(s, Polarization::Horizontal, x) == 0.0,
                             "uncoupled strings remain exactly at rest");
                   }
               }
           });
    check(string_energy(inst, 0) > 0.0 && inst.snapshot().body_energy_j > 0.0, "the plucked string drives the body");
}

void persistent_state() {
    const double rate = 48000.0;
    const InstrumentConfig c = fixture(true, true);
    const std::uint64_t restrike = static_cast<std::uint64_t>(0.3 * rate);
    const std::uint64_t frames = restrike + 2000;
    Instrument a;
    Instrument b;
    a.prepare(c, fixture_spec(rate, false));
    b.prepare(c, fixture_spec(rate, false));
    const std::size_t dof = b.info().total_degrees_of_freedom;
    std::vector<double> ea;
    std::vector<double> eb;
    render(a, {{0, nail(0, 0.55, 0.4, 0.5, 0.6)}}, frames, 1,
           [&](std::uint64_t, std::span<const float>, std::span<const float>, const ProcessSummary&) {
               ea.push_back(string_energy(a, 0));
           });
    GestureEvent second = nail(0, 0.55, 0.4, 0.5, 0.6);
    second.gap_m = 1.0e-3;
    render(b, {{0, nail(0, 0.55, 0.4, 0.5, 0.6)}, {restrike, second}}, frames, 1,
           [&](std::uint64_t, std::span<const float>, std::span<const float>, const ProcessSummary& sm) {
               check(sm.rejected_events == 0, "restrike accepted");
               eb.push_back(string_energy(b, 0));
           });
    for (std::uint64_t i = 0; i < restrike; ++i) {
        check(ea[i] == eb[i], "history before the restrike is identical");
    }
    const auto r = static_cast<std::size_t>(restrike);
    check(ea[r - 1] > 0.0 && std::abs(eb[r] / ea[r] - 1.0) < 1.0e-6,
          "the restrike continues from the ringing state (no reset to rest)");
    double min_after = std::numeric_limits<double>::infinity();
    bool differs = false;
    for (std::size_t i = r; i < eb.size(); ++i) {
        min_after = std::min(min_after, eb[i]);
        differs = differs || ea[i] != eb[i];
    }
    check(min_after > 0.05 * ea[r - 1], "the string never returns to rest during the restrike");
    check(differs, "the restrike changes the response");
    check(b.info().total_degrees_of_freedom == dof && b.snapshot().sample_clock == frames,
          "note events do not allocate strings or restart the clock");
}

// Response to a restrike with the string already vibrating in mode 1 with `amplitude`.
std::vector<double> restrike_response(const InstrumentConfig& c, double amplitude, bool strike) {
    const double rate = 48000.0;
    Instrument inst;
    inst.prepare(c, fixture_spec(rate, false));
    std::vector<double> q(inst.info().string_modes[0], 0.0);
    q[0] = amplitude;
    inst.set_string_modal_displacement(0, Polarization::Horizontal, q);
    std::vector<Timed> events;
    if (strike) {
        GestureEvent e = nail(0, 0.55, 0.0, 0.5, 0.5);
        e.gap_m = 1.0e-5;
        events.push_back({480, e});
    }
    return probe_series(inst, events, 480 + 9600, 480, 0, Polarization::Horizontal, 0.4);
}

void restrike_memory() {
    InstrumentConfig c = fixture(true, false);
    c.max_mode_frequency_hz = 5000.0;
    const auto rest = restrike_response(c, 0.0, true);
    double norm_rest = 0.0;
    for (double v : rest) {
        norm_rest += v * v;
    }
    double relative_tiny = 0.0;
    double relative_large = 0.0;
    for (double amplitude : {1.0e-9, 1.5e-3}) {
        const auto ringing = restrike_response(c, amplitude, false);
        const auto struck = restrike_response(c, amplitude, true);
        double diff = 0.0;
        for (std::size_t i = 0; i < rest.size(); ++i) {
            const double d = (struck[i] - ringing[i]) - rest[i];
            diff += d * d;
        }
        const double rel = std::sqrt(diff / norm_rest);
        (amplitude < 1.0e-6 ? relative_tiny : relative_large) = rel;
    }
    std::cout << "superposition_defect_tiny_prior=" << relative_tiny
              << " superposition_defect_large_prior=" << relative_large << '\n';
    check(relative_tiny < 1.0e-3, "with negligible prior motion the restrike superposes on free ringing");
    check(relative_large > 0.05, "a ringing string changes the nail interaction (state memory)");
}

void observation_invariance() {
    const double rate = 48000.0;
    const InstrumentConfig c = fixture(true, true);
    InstrumentConfig other = c;
    for (std::size_t j = 0; j < other.body.modes.size(); ++j) {
        other.body.modes[j].radiation_weight = {{-0.3 * static_cast<double>(j + 1), 0.7}};
    }
    ProcessSpec spec = fixture_spec(rate, true);
    ProcessSpec loud = spec;
    loud.output_gain_per_pa = 2.0;
    Instrument a;
    Instrument b;
    Instrument d;
    a.prepare(c, spec);
    b.prepare(other, spec);
    d.prepare(c, loud);
    const std::vector<Timed> events{{0, press(c, 1, c.frets[2].position_m, 1.0e-3)},
                                    {960, nail(1, 0.55, 0.6, 0.5, 0.7)}};
    std::vector<float> oa, ob, od;
    auto collect = [](std::vector<float>& out) {
        return [&out](std::uint64_t, std::span<const float> l, std::span<const float>, const ProcessSummary&) {
            out.insert(out.end(), l.begin(), l.end());
        };
    };
    render(a, events, 9600, 256, collect(oa));
    render(b, events, 9600, 256, collect(ob));
    render(d, events, 9600, 256, collect(od));
    const EnergyLedger la = a.ledger();
    const EnergyLedger lb = b.ledger();
    const EnergyLedger ld = d.ledger();
    check(la.residual_j == lb.residual_j && la.dissipation_j == lb.dissipation_j &&
              la.external_work_j == lb.external_work_j && la.string_energy_j == lb.string_energy_j &&
              la.body_energy_j == lb.body_energy_j && la.residual_j == ld.residual_j &&
              la.body_energy_j == ld.body_energy_j,
          "observation weights and gain leave the mechanical ledger bit-identical");
    for (double x : {0.2, 0.5}) {
        check(a.string_displacement_m(1, Polarization::Vertical, x) ==
                  b.string_displacement_m(1, Polarization::Vertical, x),
              "observation leaves mechanical state bit-identical");
    }
    bool differs = false;
    for (std::size_t i = 0; i < oa.size(); ++i) {
        differs = differs || oa[i] != ob[i];
        check(od[i] == 2.0f * oa[i], "observation gain scales output exactly");
    }
    check(differs, "different radiation weights change the observed output");
}

void sweep_continuity() {
    const double rate = 48000.0;
    const InstrumentConfig c = fixture(true, true);
    Instrument inst;
    inst.prepare(c, fixture_spec(rate, true));
    const double speed = 0.1;
    const std::uint64_t slide_at = static_cast<std::uint64_t>(0.1 * rate);
    // A FingerPress on the already-active slot retargets it: it must slide at the event
    // speed (here equal to the slide speed), never jump.
    const std::uint64_t repress_at = static_cast<std::uint64_t>(0.4 * rate);
    GestureEvent repress = press(c, 0, c.frets[5].position_m, 1.0e-3);
    repress.speed_m_per_s = speed;
    const std::vector<Timed> events{
        {0, press(c, 0, c.frets[2].position_m, 1.0e-3)},
        {static_cast<std::uint64_t>(0.05 * rate), nail(0, 0.55, 0.3, 0.4, 0.6)},
        {slide_at, move(0, c.frets[2].position_m - 0.008 + 0.02, speed)},
        {repress_at, repress},
    };
    double last_x = 0.0;
    double last_force = 0.0;
    double last_disp = 0.0;
    double max_force_step_static = 0.0;
    double max_force_step_slide = 0.0;
    double max_disp_step_static = 0.0;
    double max_disp_step_slide = 0.0;
    double peak = 0.0;
    double worst = 0.0;
    std::uint64_t n = 0;
    double max_energy_before_repress = 0.0;
    double max_energy_after_repress = 0.0;
    render(inst, events, static_cast<std::uint64_t>(1.15 * rate), 1,
           [&](std::uint64_t start, std::span<const float>, std::span<const float>, const ProcessSummary& sm) {
               check(sm.first_fault == FaultCode::None, "slide and re-press must not fault");
               (start < repress_at ? max_energy_before_repress : max_energy_after_repress) =
                   std::max(start < repress_at ? max_energy_before_repress : max_energy_after_repress,
                            string_energy(inst, 0));
               const InstrumentSnapshot snap = inst.snapshot();
               const double x = snap.strings[0].finger_position_m[0];
               const double force = snap.strings[0].finger_force_n[0];
               const double disp = inst.string_displacement_m(0, Polarization::Vertical, 0.45);
               const EnergyLedger led = inst.ledger();
               peak = std::max(peak, stored(led));
               worst = std::max(worst, std::abs(led.residual_j));
               if (n > 0 && start > static_cast<std::uint64_t>(0.07 * rate)) {
                   check(x >= last_x - 1e-15 && x - last_x <= speed / rate + 1e-15,
                         "pad moves monotonically at no more than the slide speed");
                   const double df = std::abs(force - last_force);
                   const double dd = std::abs(disp - last_disp);
                   if (start < slide_at) {
                       max_force_step_static = std::max(max_force_step_static, df);
                       max_disp_step_static = std::max(max_disp_step_static, dd);
                   } else if (start < repress_at) {
                       max_force_step_slide = std::max(max_force_step_slide, df);
                       max_disp_step_slide = std::max(max_disp_step_slide, dd);
                   }
               }
               last_x = x;
               last_force = force;
               last_disp = disp;
               ++n;
           });
    std::cout << "force_step_static_n=" << max_force_step_static << " force_step_slide_n=" << max_force_step_slide
              << " disp_step_static_m=" << max_disp_step_static << " disp_step_slide_m=" << max_disp_step_slide
              << " max_residual_j=" << worst << '\n';
    check(max_force_step_slide <= 3.0 * max_force_step_static + 0.05,
          "pad force stays continuous during the slide (no jumps beyond the static vibration bound)");
    check(max_disp_step_slide <= 3.0 * max_disp_step_static + 1e-6, "string displacement stays continuous");
    check(worst <= 1.0e-9 * peak, "ledger closes during the slide");
    const InstrumentSnapshot snap = inst.snapshot();
    std::cout << "repress_final_x_m=" << snap.strings[0].finger_position_m[0]
              << " energy_before_repress_j=" << max_energy_before_repress
              << " energy_after_repress_j=" << max_energy_after_repress << '\n';
    check(std::abs(snap.strings[0].finger_position_m[0] - repress.position_m) < 1e-12,
          "re-pressed pad slides to its new position");
    check(max_energy_after_repress < 2.0 * max_energy_before_repress,
          "re-pressing an active pad does not inject a position-jump impulse");
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
    const std::array<Case, 17> tests{{
        {"engine.config_validation", config_validation},
        {"engine.lossless_energy_ledger", lossless_energy_ledger},
        {"engine.lossy_energy_ledger", lossy_energy_ledger},
        {"engine.open_string_pitch", open_string_pitch},
        {"engine.fretted_pitch", fretted_pitch},
        {"engine.event_ordering", event_ordering},
        {"engine.newton_failure_reporting", newton_failure_reporting},
        {"contact.nail_passivity", nail_passivity},
        {"contact.release_convergence", release_convergence},
        {"contact.fret_crossing_work", fret_crossing_work},
        {"contact.release_afterlength", release_afterlength},
        {"coupling.cross_string_transfer", cross_string_transfer},
        {"coupling.uncoupled_limit", uncoupled_limit},
        {"performance.persistent_state", persistent_state},
        {"performance.restrike_memory", restrike_memory},
        {"acoustics.observation_invariance", observation_invariance},
        {"controls.sweep_continuity", sweep_continuity},
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
