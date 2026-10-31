// Independent checks of the synthetic setar presets. Expected values are recomputed
// here from first principles (stiff-string dispersion, chain-of-fifths integer
// arithmetic, logarithms), never by calling the function under test twice.
#include "setar/instrument/setar_preset.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <cstdint>
#include <exception>
#include <iostream>
#include <limits>
#include <numbers>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {
using namespace setar::instrument;
constexpr double pi = std::numbers::pi_v<double>;

void check(bool condition, std::string_view message) {
    if (!condition) {
        throw std::runtime_error(std::string(message));
    }
}

void near(double actual, double expected, double rtol, std::string_view message) {
    const double limit = rtol * std::abs(expected);
    if (!std::isfinite(actual) || std::abs(actual - expected) > limit) {
        throw std::runtime_error(std::string(message) + " actual=" + std::to_string(actual) +
                                 " expected=" + std::to_string(expected));
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

bool mentions_synthetic(std::string text) {
    std::transform(text.begin(), text.end(), text.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return text.find("synthetic") != std::string::npos;
}

// Forward relation, coded independently of tension_for_frequency:
// omega_1^2 = (T/mu) k^2 + (EI/mu) k^4, k = pi/L.
double stiff_string_fundamental_hz(double length, double mu, double ei, double tension) {
    const double k = pi / length;
    return std::sqrt(tension / mu * k * k + ei / mu * k * k * k * k) / (2.0 * pi);
}

void tension_inverse() {
    struct Case {
        double length, mu, ei, f;
    };
    const std::array<Case, 5> cases{{{0.68, 2.466e-4, 1.571e-5, 261.6},
                                     {0.66, 8.18e-4, 7.37e-5, 130.8},
                                     {0.70, 3.85e-4, 3.83e-5, 196.0},
                                     {0.50, 1.0e-3, 0.0, 440.0},
                                     {1.20, 5.0e-4, 1.0e-4, 55.0}}};
    for (const auto& c : cases) {
        const double t = tension_for_frequency(c.length, c.mu, c.ei, c.f);
        check(t > 0.0, "tension positive");
        near(stiff_string_fundamental_hz(c.length, c.mu, c.ei, t), c.f, 1e-12,
             "forward frequency reproduces the target");
    }
    // EI = 0 reduces to the ideal-string relation T = mu*(2 L f)^2.
    near(tension_for_frequency(0.5, 1.0e-3, 0.0, 440.0), 1.0e-3 * 440.0 * 440.0, 1e-14,
         "ideal-string limit");
    // Bending stiffness alone exceeding the target frequency is infeasible, not clamped.
    rejected([] { (void)tension_for_frequency(0.68, 2.466e-4, 1.0, 50.0); },
             "infeasible (negative) tension is rejected");
    // Every preset string reproduces its tuning.
    const auto specs = synthetic_string_specs();
    for (const auto& tuning : tuning_presets()) {
        for (std::size_t i = 0; i < string_count; ++i) {
            const double mu = linear_density_kg_per_m(specs[i]);
            const double ei = bending_stiffness_n_m2(specs[i]);
            near(mu,
                 specs[i].material.density_kg_per_m3 * pi * specs[i].diameter_m *
                     specs[i].diameter_m / 4.0,
                 1e-15, "plain-string linear density");
            near(ei,
                 specs[i].material.youngs_modulus_pa * pi * std::pow(specs[i].diameter_m, 4.0) /
                     64.0,
                 1e-12, "plain-string bending stiffness");
            const double t = tension_for_frequency(default_speaking_length_m, mu, ei,
                                                   tuning.open_frequency_hz[i]);
            near(stiff_string_fundamental_hz(default_speaking_length_m, mu, ei, t),
                 tuning.open_frequency_hz[i], 1e-12, "preset string reproduces tuning");
            check(t > 10.0 && t < 60.0, "synthetic tension stays in the declared 10..60 N range");
            std::cout << tuning.id << " string " << i << " mu=" << mu << " EI=" << ei << " T=" << t
                      << '\n';
        }
    }
}

struct Fraction {
    std::uint64_t n;
    std::uint64_t d;
};

// Independent construction: 3^k for k = -12..4 reduced into [1, 2) by powers of two,
// then sorted, then the octave appended.
std::vector<Fraction> chain_of_fifths_octave() {
    std::vector<Fraction> out;
    for (int k = -12; k <= 4; ++k) {
        if (k == 0) {
            continue;
        }
        std::uint64_t power = 1;
        for (int i = 0; i < std::abs(k); ++i) {
            power *= 3;
        }
        Fraction f = k > 0 ? Fraction{power, 1} : Fraction{1, power};
        while (f.n >= 2 * f.d) {
            f.d *= 2;
        }
        while (f.n < f.d) {
            f.n *= 2;
        }
        out.push_back(f);
    }
    std::sort(out.begin(), out.end(), [](Fraction a, Fraction b) { return a.n * b.d < b.n * a.d; });
    out.push_back({2, 1});
    return out;
}

void fret_positions_test() {
    for (const auto& map : fret_map_definitions()) {
        check(mentions_synthetic(map.status), "fret map status declares synthetic use");
        for (double length : {0.68, 0.66, 0.70}) {
            const auto frets = fret_positions(length, map);
            check(frets.size() == map.intervals.size(), "one fret per interval");
            double previous = 0.0;
            for (std::size_t i = 0; i < frets.size(); ++i) {
                check(frets[i].position_m > previous && frets[i].position_m < length,
                      "fret positions strictly increasing inside the speaking length");
                previous = frets[i].position_m;
                near(length / (length - frets[i].position_m), map.intervals[i].ratio, 1e-12,
                     "ideal-string speaking-length ratio reproduces the interval");
            }
        }
    }
    const auto& pyth = find_fret_map(default_fret_map_id);
    const auto expected = chain_of_fifths_octave();
    check(expected.size() == 17, "chain of fifths gives 17 intervals per octave");
    check(pyth.intervals.size() == 25, "default map has 25 frets");
    for (std::size_t i = 0; i < pyth.intervals.size(); ++i) {
        const Fraction e =
            i < 17 ? expected[i] : Fraction{2 * expected[i - 17].n, expected[i - 17].d};
        check(pyth.intervals[i].numerator == e.n && pyth.intervals[i].denominator == e.d,
              "Pythagorean fret " + std::to_string(i + 1) +
                  " matches the chain-of-fifths fraction");
        near(pyth.intervals[i].ratio, static_cast<double>(e.n) / static_cast<double>(e.d), 1e-15,
             "rational and floating ratio agree");
    }
    // Exact step check: within the first octave every step is a limma or a comma,
    // twelve limmas and five commas (12 L + 5 C = octave).
    int limmas = 0;
    int commas = 0;
    Fraction previous{1, 1};
    for (std::size_t i = 0; i < 17; ++i) {
        const Fraction f{pyth.intervals[i].numerator, pyth.intervals[i].denominator};
        const std::uint64_t num = f.n * previous.d;
        const std::uint64_t den = f.d * previous.n;
        if (num * 243 == den * 256) {
            ++limmas;
        } else if (num * 524288 == den * 531441) {
            ++commas;
        } else {
            throw std::runtime_error("step is neither limma nor comma at fret " +
                                     std::to_string(i + 1));
        }
        previous = f;
    }
    check(limmas == 12 && commas == 5, "octave = 12 limmas + 5 commas");
    const auto& edo = find_fret_map("edo24-quarter-tone-grid");
    check(edo.intervals.size() == 25, "24-EDO grid has 25 frets");
    for (std::size_t i = 0; i < edo.intervals.size(); ++i) {
        near(edo.intervals[i].ratio, std::exp2(static_cast<double>(i + 2) / 24.0), 1e-14,
             "24-EDO step ratio");
        check(edo.intervals[i].numerator == 0 && edo.intervals[i].denominator == 0,
              "irrational steps carry no fraction");
    }
}

void check_config(const InstrumentConfig& config, bool with_body) {
    check(mentions_synthetic(config.label), "config label says synthetic");
    check(config.provenance == Provenance::Synthetic, "config provenance synthetic");
    const auto& tuning = find_tuning(default_tuning_id);
    for (std::size_t i = 0; i < string_count; ++i) {
        const auto& s = config.strings[i];
        check(mentions_synthetic(s.label), "string label says synthetic");
        near(s.bridge_position_m, default_speaking_length_m, 1e-15, "bridge at speaking length");
        near(s.total_length_m, default_speaking_length_m + default_afterlength_m, 1e-15,
             "total = speaking + afterlength");
        near(stiff_string_fundamental_hz(s.bridge_position_m, s.linear_density_kg_per_m,
                                         s.bending_stiffness_n_m2, s.tension_n),
             tuning.open_frequency_hz[i], 1e-12, "configured string reproduces default tuning");
        check(s.action_at_nut_m > 0.0 && s.action_at_bridge_m > s.action_at_nut_m, "action rises");
    }
    check(config.frets.size() == 25, "25 frets");
    check(config.frets.back().position_m < config.strings[0].bridge_position_m,
          "frets before bridge");
    check(mentions_synthetic(config.body.label), "body label says synthetic");
    check(config.body.provenance == Provenance::Synthetic, "body provenance synthetic");
    check(with_body ? !config.body.modes.empty() : config.body.modes.empty(),
          "body modes present exactly when requested");
    check(config.contacts.fret.exponent >= 1.0 && config.contacts.finger.exponent >= 1.0 &&
              config.contacts.nail.exponent >= 1.0,
          "contact exponents >= 1");
    check(config.max_mode_frequency_hz == 18000.0, "declared mode truncation");
}

void config_admissibility() {
    for (const auto& tuning : tuning_presets()) {
        check(!tuning.id.empty() && !tuning.description.empty(), "tuning has id and description");
        check(mentions_synthetic(tuning.status), "tuning status says synthetic");
    }
    const auto courses = setar_courses();
    check(courses.size() == 3, "three courses");
    check(courses[0].strings == std::vector<std::size_t>{0} &&
              courses[1].strings == std::vector<std::size_t>{1} &&
              courses[2].strings == std::vector<std::size_t>{2, 3},
          "courses {0}, {1}, {2,3}");
    for (const auto& course : courses) {
        check(!course.label.empty(), "course label");
    }
    PresetOptions no_body;
    no_body.include_body = false;
    const InstrumentConfig grounded = synthetic_setar_config(no_body);
    check_config(grounded, false);
    validate(grounded); // engine-side admissibility, independent of the preset builder
    const InstrumentConfig full = synthetic_setar_config();
    check_config(full, true);
    validate(full);
    PresetOptions alternative;
    alternative.tuning_id = "synthetic-c4-f3-c3-c4";
    alternative.fret_map_id = "edo24-quarter-tone-grid";
    alternative.include_body = false;
    const InstrumentConfig alt = synthetic_setar_config(alternative);
    check(alt.frets.size() == 25, "alternative fret map gives 25 frets");
    near(stiff_string_fundamental_hz(
             alt.strings[1].bridge_position_m, alt.strings[1].linear_density_kg_per_m,
             alt.strings[1].bending_stiffness_n_m2, alt.strings[1].tension_n),
         find_tuning(alternative.tuning_id).open_frequency_hz[1], 1e-12,
         "alternative tuning reaches its string 1 target");
    validate(alt);
}

void tuning_cents() {
    near(cents(2.0), 1200.0, 1e-15, "octave");
    check(cents(1.0) == 0.0, "unison");
    near(cents(3.0 / 2.0), 701.955000865387, 1e-12, "pure fifth");
    // Independent forms: limma = 2^8/3^5, comma = 3^12/2^19, written via log2(3).
    const double log2_3 = std::log(3.0) / std::log(2.0);
    near(cents(256.0 / 243.0), 1200.0 * (8.0 - 5.0 * log2_3), 1e-12, "limma");
    near(cents(256.0 / 243.0), 90.22499567, 1e-9, "limma ~90.225 cents");
    near(cents(531441.0 / 524288.0), 1200.0 * (12.0 * log2_3 - 19.0), 1e-10, "Pythagorean comma");
    near(cents(531441.0 / 524288.0), 23.46001038, 1e-9, "Pythagorean comma ~23.46 cents");
    near(cents(std::exp2(1.0 / 24.0)), 50.0, 1e-12, "quarter tone");
    near(cents(0.5), -1200.0, 1e-15, "downward octave");
    const auto& base = find_tuning(default_tuning_id).open_frequency_hz;
    near(cents(base[3] / base[2]), 1200.0, 1e-12, "paired course is an octave");
    near(cents(base[0] / base[1]), 1200.0 * std::log2(4.0 / 3.0), 1e-12, "pure fourth");
    near(base[0], 440.0 * std::exp2(-0.75), 1e-14, "C4 reference from A4 = 440 Hz");
    const auto& alt = find_tuning("synthetic-c4-f3-c3-c4").open_frequency_hz;
    near(cents(alt[0] / alt[1]), 1200.0 * std::log2(1.5), 1e-12, "pure fifth tuning");
}

void invalid_inputs() {
    const double nan = std::numeric_limits<double>::quiet_NaN();
    const double inf = std::numeric_limits<double>::infinity();
    rejected([] { (void)tension_for_frequency(0.0, 1e-3, 0.0, 100.0); }, "zero length");
    rejected([&] { (void)tension_for_frequency(nan, 1e-3, 0.0, 100.0); }, "NaN length");
    rejected([] { (void)tension_for_frequency(0.6, -1e-3, 0.0, 100.0); }, "negative density");
    rejected([] { (void)tension_for_frequency(0.6, 1e-3, -1.0, 100.0); }, "negative EI");
    rejected([&] { (void)tension_for_frequency(0.6, 1e-3, inf, 100.0); }, "infinite EI");
    rejected([] { (void)tension_for_frequency(0.6, 1e-3, 0.0, 0.0); }, "zero frequency");
    const auto& map = find_fret_map(default_fret_map_id);
    rejected([&] { (void)fret_positions(0.0, map); }, "zero speaking length");
    rejected([&] { (void)fret_positions(-0.5, map); }, "negative speaking length");
    FretMapDefinition bad{"bad", "bad", "synthetic", {}};
    rejected([&] { (void)fret_positions(0.6, bad); }, "empty map");
    bad.intervals = {{"unison", 1.0, 1, 1}};
    rejected([&] { (void)fret_positions(0.6, bad); }, "ratio must exceed one");
    bad.intervals = {{"a", 1.2, 0, 0}, {"b", 1.1, 0, 0}};
    rejected([&] { (void)fret_positions(0.6, bad); }, "nonincreasing ratios");
    bad.intervals = {{"a", nan, 0, 0}};
    rejected([&] { (void)fret_positions(0.6, bad); }, "NaN ratio");
    rejected([] { (void)cents(0.0); }, "zero ratio");
    rejected([] { (void)cents(-2.0); }, "negative ratio");
    rejected([&] { (void)cents(nan); }, "NaN ratio cents");
    rejected([] { (void)find_tuning("not-a-tuning"); }, "unknown tuning");
    rejected([] { (void)find_tuning(""); }, "empty tuning id is not a preset");
    rejected([] { (void)find_fret_map("not-a-map"); }, "unknown fret map");
    StringSpec spec = synthetic_string_specs()[0];
    spec.diameter_m = 0.0;
    rejected([&] { (void)linear_density_kg_per_m(spec); }, "zero diameter");
    spec = synthetic_string_specs()[0];
    spec.material.density_kg_per_m3 = -1.0;
    rejected([&] { (void)linear_density_kg_per_m(spec); }, "negative material density");
    spec = synthetic_string_specs()[0];
    spec.material.youngs_modulus_pa = nan;
    rejected([&] { (void)bending_stiffness_n_m2(spec); }, "NaN modulus");
    PresetOptions options;
    options.include_body = false;
    options.speaking_length_m = -0.68;
    rejected([&] { (void)synthetic_setar_config(options); }, "negative speaking length option");
    options.speaking_length_m = 0.0;
    options.afterlength_m = nan;
    rejected([&] { (void)synthetic_setar_config(options); }, "NaN afterlength option");
    options.afterlength_m = 0.0;
    options.tuning_id = "unknown";
    rejected([&] { (void)synthetic_setar_config(options); }, "unknown tuning option");
    options.tuning_id.clear();
    options.fret_map_id = "unknown";
    rejected([&] { (void)synthetic_setar_config(options); }, "unknown fret map option");
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
    const std::array<Case, 5> tests{{
        {"preset.tension_inverse", tension_inverse},
        {"preset.fret_positions", fret_positions_test},
        {"preset.config_admissibility", config_admissibility},
        {"preset.tuning_cents", tuning_cents},
        {"preset.invalid_inputs", invalid_inputs},
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
