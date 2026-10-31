#include "setar/instrument/config.hpp"

#include <cmath>
#include <stdexcept>
#include <string>

namespace setar::instrument {
namespace {

[[noreturn]] void fail(const std::string& field, const char* rule) {
    throw std::invalid_argument(field + " " + rule);
}

void positive(double value, const std::string& field, const char* unit) {
    if (!std::isfinite(value) || value <= 0.0) {
        fail(field, (std::string("must be finite and positive (") + unit + ")").c_str());
    }
}

void nonnegative(double value, const std::string& field, const char* unit) {
    if (!std::isfinite(value) || value < 0.0) {
        fail(field, (std::string("must be finite and nonnegative (") + unit + ")").c_str());
    }
}

void finite(double value, const std::string& field) {
    if (!std::isfinite(value)) {
        fail(field, "must be finite");
    }
}

void contact_law(const ContactLaw& law, const std::string& field) {
    positive(law.stiffness, field + ".stiffness", "N/m^alpha");
    if (!std::isfinite(law.exponent) || law.exponent < 1.0 || law.exponent > 4.0) {
        fail(field + ".exponent", "must be finite and in [1, 4]");
    }
    nonnegative(law.damping_n_s_per_m, field + ".damping_n_s_per_m", "N*s/m");
}

} // namespace

void validate(const InstrumentConfig& config) {
    double min_bridge = INFINITY;
    for (std::size_t s = 0; s < string_count; ++s) {
        const StringConfig& st = config.strings[s];
        const std::string at = "strings[" + std::to_string(s) + "]";
        positive(st.total_length_m, at + ".total_length_m", "m");
        positive(st.bridge_position_m, at + ".bridge_position_m", "m");
        if (!(st.bridge_position_m < st.total_length_m)) {
            fail(at + ".bridge_position_m", "must be less than total_length_m");
        }
        positive(st.tension_n, at + ".tension_n", "N");
        positive(st.linear_density_kg_per_m, at + ".linear_density_kg_per_m", "kg/m");
        nonnegative(st.bending_stiffness_n_m2, at + ".bending_stiffness_n_m2", "N*m^2");
        nonnegative(st.loss.decay_constant_per_s, at + ".loss.decay_constant_per_s", "1/s");
        nonnegative(st.loss.decay_wavenumber_squared_m2_per_s,
                    at + ".loss.decay_wavenumber_squared_m2_per_s", "m^2/s");
        nonnegative(st.loss.decay_frequency_squared_s, at + ".loss.decay_frequency_squared_s", "s");
        positive(st.action_at_nut_m, at + ".action_at_nut_m", "m");
        positive(st.action_at_bridge_m, at + ".action_at_bridge_m", "m");
        min_bridge = std::fmin(min_bridge, st.bridge_position_m);
    }
    double previous = 0.0;
    for (std::size_t f = 0; f < config.frets.size(); ++f) {
        const std::string at = "frets[" + std::to_string(f) + "].position_m";
        positive(config.frets[f].position_m, at, "m");
        if (!(config.frets[f].position_m > previous)) {
            fail(at, "must be strictly increasing");
        }
        if (!(config.frets[f].position_m < min_bridge)) {
            fail(at, "must be less than every string's bridge_position_m");
        }
        previous = config.frets[f].position_m;
    }
    for (std::size_t j = 0; j < config.body.modes.size(); ++j) {
        const BodyMode& mode = config.body.modes[j];
        const std::string at = "body.modes[" + std::to_string(j) + "]";
        positive(mode.frequency_hz, at + ".frequency_hz", "Hz");
        if (!std::isfinite(mode.damping_ratio) || mode.damping_ratio < 0.0 ||
            mode.damping_ratio >= 1.0) {
            fail(at + ".damping_ratio", "must be finite and in [0, 1)");
        }
        positive(mode.modal_mass_kg, at + ".modal_mass_kg", "kg");
        for (std::size_t s = 0; s < string_count; ++s) {
            for (std::size_t p = 0; p < polarization_count; ++p) {
                finite(mode.bridge_participation[s][p], at + ".bridge_participation[" +
                                                            std::to_string(s) + "][" +
                                                            std::to_string(p) + "]");
            }
        }
        for (std::size_t o = 0; o < observation_count; ++o) {
            finite(mode.radiation_weight[o], at + ".radiation_weight[" + std::to_string(o) + "]");
        }
    }
    for (std::size_t p = 0; p < polarization_count; ++p) {
        positive(config.bridge.stiffness_n_per_m[p],
                 "bridge.stiffness_n_per_m[" + std::to_string(p) + "]", "N/m");
        nonnegative(config.bridge.damping_n_s_per_m[p],
                    "bridge.damping_n_s_per_m[" + std::to_string(p) + "]", "N*s/m");
    }
    contact_law(config.contacts.fret, "contacts.fret");
    contact_law(config.contacts.finger, "contacts.finger");
    contact_law(config.contacts.nail, "contacts.nail");
    nonnegative(config.contacts.finger_lateral_damping_n_s_per_m,
                "contacts.finger_lateral_damping_n_s_per_m", "N*s/m");
    nonnegative(config.contacts.fret_stick_stiffness_n_per_m,
                "contacts.fret_stick_stiffness_n_per_m", "N/m");
    nonnegative(config.contacts.fret_stick_damping_n_s_per_m,
                "contacts.fret_stick_damping_n_s_per_m", "N*s/m");
    positive(config.max_mode_frequency_hz, "max_mode_frequency_hz", "Hz");
}

void validate(const ProcessSpec& spec) {
    if (!std::isfinite(spec.sample_rate_hz) || spec.sample_rate_hz < 22050.0 ||
        spec.sample_rate_hz > 192000.0) {
        fail("sample_rate_hz", "must be finite and within [22050, 192000] Hz");
    }
    if (spec.max_block_frames < 1 || spec.max_block_frames > 65536) {
        fail("max_block_frames", "must be in [1, 65536]");
    }
    if (spec.max_events_per_block < 1 || spec.max_events_per_block > 65536) {
        fail("max_events_per_block", "must be in [1, 65536]");
    }
    // Eight linear bridge channels (4 strings x 2 polarizations) are always present.
    if (spec.max_interaction_channels < 8 || spec.max_interaction_channels > 256) {
        fail("max_interaction_channels", "must be in [8, 256]");
    }
    if (spec.max_newton_iterations < 1 || spec.max_newton_iterations > 1000) {
        fail("max_newton_iterations", "must be in [1, 1000]");
    }
    positive(spec.newton_tolerance_m, "newton_tolerance_m", "m");
    positive(spec.output_gain_per_pa, "output_gain_per_pa", "1/Pa");
}

} // namespace setar::instrument
