#include "setar/reference.hpp"

#include <algorithm>
#include <charconv>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <locale>
#include <numbers>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <type_traits>
#include <vector>

namespace {
struct Options {
    std::filesystem::path output_dir = "artifacts/reference";
    setar::reference::StringParameters parameters{};
    std::uint32_t sample_rate = 48000;
    double duration_s = 2.0;
    double height_m = 0.0005;
    double pluck_fraction = 0.23;
    double pickup_fraction = 0.73;
    double gain_per_m = 800.0;
};

template <typename T> T parse_number(std::string_view text, std::string_view option) {
    T value{};
    if constexpr (std::is_floating_point_v<T>) {
        // Some supported Apple libc++ toolchains lack floating from_chars.
        // This control/offline parser is strict and explicitly locale independent.
        std::istringstream input{std::string(text)};
        input.imbue(std::locale::classic());
        input >> std::noskipws >> value;
        if (!input || !input.eof()) {
            throw std::invalid_argument("invalid number for " + std::string(option));
        }
    } else {
        const auto result = std::from_chars(text.data(), text.data() + text.size(), value);
        if (result.ec != std::errc{} || result.ptr != text.data() + text.size()) {
            throw std::invalid_argument("invalid number for " + std::string(option));
        }
    }
    return value;
}

void print_help() {
    std::cout << "setar_render: SYNTHETIC reference fixture, not a validated setar\n"
                 "Usage: setar_render [options]\n"
                 "  --output-dir PATH       New/empty output destination (default artifacts/reference)\n"
                 "  --duration SECONDS      Render duration in (0, 30] (default 2)\n"
                 "  --sample-rate HZ        Integer sample rate [8000, 384000] (default 48000)\n"
                 "  --modes COUNT           Retained modes [1, 128] (default 16)\n"
                 "  --length-m VALUE        String length (default 0.66)\n"
                 "  --tension-n VALUE       Static tension (default 35)\n"
                 "  --density-kg-m VALUE    Linear density (default 0.0007)\n"
                 "  --stiffness-n-m2 VALUE  Bending stiffness EI (default 0.000015)\n"
                 "  --decay-per-s VALUE     Constant modal decay (default 0.8)\n"
                 "  --decay-freq2-s VALUE   Frequency-squared decay coefficient (default 0)\n"
                 "  --height-m VALUE        Triangle peak displacement (default 0.0005)\n"
                 "  --pluck-position VALUE Interior fraction of length (default 0.23)\n"
                 "  --pickup-position VALUE Observation fraction [0, 1] (default 0.73)\n"
                 "  --gain-per-m VALUE      Fixed PCM observation gain (default 800)\n"
                 "  --help                  Show this help\n"
                 "No automatic normalization, clipping, output overwrite, or audio device access.\n";
}

Options parse_options(int argc, char** argv) {
    Options options;
    for (int index = 1; index < argc; ++index) {
        const std::string_view option(argv[index]);
        if (index + 1 >= argc) {
            throw std::invalid_argument("missing value for " + std::string(option));
        }
        const std::string_view value(argv[++index]);
        if (option == "--output-dir") {
            if (value.empty()) {
                throw std::invalid_argument("output directory cannot be empty");
            }
            options.output_dir = std::string(value);
        } else if (option == "--sample-rate") {
            options.sample_rate = parse_number<std::uint32_t>(value, option);
        } else if (option == "--modes") {
            options.parameters.mode_count = parse_number<std::size_t>(value, option);
        } else if (option == "--duration") {
            options.duration_s = parse_number<double>(value, option);
        } else if (option == "--height-m") {
            options.height_m = parse_number<double>(value, option);
        } else if (option == "--pluck-position") {
            options.pluck_fraction = parse_number<double>(value, option);
        } else if (option == "--pickup-position") {
            options.pickup_fraction = parse_number<double>(value, option);
        } else if (option == "--gain-per-m") {
            options.gain_per_m = parse_number<double>(value, option);
        } else if (option == "--length-m") {
            options.parameters.length_m = parse_number<double>(value, option);
        } else if (option == "--tension-n") {
            options.parameters.tension_n = parse_number<double>(value, option);
        } else if (option == "--density-kg-m") {
            options.parameters.linear_density_kg_per_m = parse_number<double>(value, option);
        } else if (option == "--stiffness-n-m2") {
            options.parameters.bending_stiffness_n_m2 = parse_number<double>(value, option);
        } else if (option == "--decay-per-s") {
            options.parameters.decay_constant_per_s = parse_number<double>(value, option);
        } else if (option == "--decay-freq2-s") {
            options.parameters.decay_frequency_squared_s = parse_number<double>(value, option);
        } else {
            throw std::invalid_argument("unknown option " + std::string(option));
        }
    }
    if (!std::isfinite(options.duration_s) || options.duration_s <= 0.0 ||
        options.duration_s > 30.0) {
        throw std::invalid_argument("duration must be finite and in (0, 30] seconds");
    }
    if (!std::isfinite(options.gain_per_m) || options.gain_per_m <= 0.0) {
        throw std::invalid_argument("fixed observation gain must be positive and finite (1/m)");
    }
    return options;
}

std::ofstream open_text(const std::filesystem::path& path) {
    std::ofstream stream(path, std::ios::binary);
    stream.exceptions(std::ios::failbit | std::ios::badbit);
    stream << std::setprecision(17);
    return stream;
}

void write_u16(std::ostream& stream, std::uint16_t value) {
    const char bytes[2] = {static_cast<char>(value & 0xffU),
                           static_cast<char>((value >> 8U) & 0xffU)};
    stream.write(bytes, 2);
}

void write_u32(std::ostream& stream, std::uint32_t value) {
    const char bytes[4] = {static_cast<char>(value & 0xffU),
                           static_cast<char>((value >> 8U) & 0xffU),
                           static_cast<char>((value >> 16U) & 0xffU),
                           static_cast<char>((value >> 24U) & 0xffU)};
    stream.write(bytes, 4);
}

void write_wav(const std::filesystem::path& path, std::span<const double> samples,
               const Options& options) {
    std::ofstream stream(path, std::ios::binary);
    stream.exceptions(std::ios::failbit | std::ios::badbit);
    const auto data_size = static_cast<std::uint32_t>(samples.size() * 2U);
    stream.write("RIFF", 4);
    write_u32(stream, 36U + data_size);
    stream.write("WAVEfmt ", 8);
    write_u32(stream, 16U);
    write_u16(stream, 1U); // PCM, not IEEE float.
    write_u16(stream, 1U); // One observation channel, not radiated stereo sound.
    write_u32(stream, options.sample_rate);
    write_u32(stream, options.sample_rate * 2U);
    write_u16(stream, 2U);
    write_u16(stream, 16U);
    stream.write("data", 4);
    write_u32(stream, data_size);
    for (double sample : samples) {
        const auto pcm = static_cast<std::int16_t>(std::lround(sample * options.gain_per_m * 32767.0));
        write_u16(stream, static_cast<std::uint16_t>(pcm));
    }
    stream.close();
}

int render(const Options& options) {
    setar::reference::ModalString model;
    model.prepare(options.parameters, static_cast<double>(options.sample_rate),
                  options.pickup_fraction);
    model.pluck(options.height_m, options.pluck_fraction);
    const auto sample_count = static_cast<std::size_t>(
        std::floor(options.duration_s * static_cast<double>(options.sample_rate)));
    if (sample_count == 0) {
        throw std::invalid_argument("duration must produce at least one sample");
    }
    const std::vector<setar::reference::ModeState> initial_modes(model.modes().begin(),
                                                               model.modes().end());
    std::vector<double> samples(sample_count);
    std::vector<double> energies(sample_count);
    double peak = 0.0;
    for (std::size_t index = 0; index < sample_count; ++index) {
        energies[index] = model.energy();
        samples[index] = model.tick();
        if (!std::isfinite(samples[index]) || !std::isfinite(energies[index])) {
            throw std::runtime_error("non-finite reference state; no audio was written");
        }
        peak = std::max(peak, std::abs(samples[index]));
    }
    const double peak_linear = peak * options.gain_per_m;
    if (!std::isfinite(peak_linear) || peak_linear > 1.0) {
        throw std::invalid_argument(
            "fixed-gain PCM output would clip; lower --gain-per-m or --height-m (no normalization)");
    }
    for (const char* name : {"render.wav", "samples.csv", "modes.csv", "metadata.json"}) {
        if (std::filesystem::exists(options.output_dir / name)) {
            throw std::runtime_error("refusing to overwrite existing output " +
                                     (options.output_dir / name).string());
        }
    }
    std::filesystem::create_directories(options.output_dir);
    write_wav(options.output_dir / "render.wav", samples, options);
    auto sample_file = open_text(options.output_dir / "samples.csv");
    sample_file << "sample_index,time_s,displacement_m,pcm_linear,energy_j\n";
    for (std::size_t index = 0; index < sample_count; ++index) {
        sample_file << index << ','
                    << static_cast<double>(index) / static_cast<double>(options.sample_rate) << ','
                    << samples[index] << ',' << samples[index] * options.gain_per_m << ','
                    << energies[index] << '\n';
    }
    sample_file.close();
    auto mode_file = open_text(options.output_dir / "modes.csv");
    mode_file << "mode,frequency_hz,damped_frequency_hz,decay_per_s,modal_mass_kg,initial_displacement_m\n";
    for (std::size_t index = 0; index < initial_modes.size(); ++index) {
        const auto& mode = initial_modes[index];
        const double omega = mode.angular_frequency_rad_per_s;
        mode_file << index + 1 << ',' << omega / (2.0 * std::numbers::pi_v<double>) << ','
                  << std::sqrt((omega - mode.decay_per_s) * (omega + mode.decay_per_s)) /
                         (2.0 * std::numbers::pi_v<double>)
                  << ',' << mode.decay_per_s << ',' << mode.mass_kg << ','
                  << mode.initial_displacement_m << '\n';
    }
    mode_file.close();
    auto metadata = open_text(options.output_dir / "metadata.json");
    const auto& p = options.parameters;
    metadata << "{\n"
             << "  \"schema_version\": 1,\n"
             << "  \"model\": \"simply_supported_linear_stiff_string_modal_reference\",\n"
             << "  \"evidence_label\": \"synthetic_not_measured_setar\",\n"
             << "  \"integrator\": \"exact_underdamped_free_response_transition\",\n"
             << "  \"observation\": \"transverse_displacement_at_one_point_not_sound_pressure\",\n"
             << "  \"normalization\": \"none_fixed_gain\",\n"
             << "  \"wav_encoding\": \"mono_pcm16_little_endian\",\n"
             << "  \"sample_rate_hz\": " << options.sample_rate << ",\n"
             << "  \"sample_count\": " << sample_count << ",\n"
             << "  \"requested_duration_s\": " << options.duration_s << ",\n"
             << "  \"last_sample_time_s\": "
             << static_cast<double>(sample_count - 1) / static_cast<double>(options.sample_rate)
             << ",\n"
             << "  \"mode_count\": " << p.mode_count << ",\n"
             << "  \"length_m\": " << p.length_m << ",\n"
             << "  \"tension_n\": " << p.tension_n << ",\n"
             << "  \"linear_density_kg_per_m\": " << p.linear_density_kg_per_m << ",\n"
             << "  \"bending_stiffness_n_m2\": " << p.bending_stiffness_n_m2 << ",\n"
             << "  \"decay_constant_per_s\": " << p.decay_constant_per_s << ",\n"
             << "  \"decay_frequency_squared_s\": " << p.decay_frequency_squared_s << ",\n"
             << "  \"pluck_height_m\": " << options.height_m << ",\n"
             << "  \"pluck_position_fraction\": " << options.pluck_fraction << ",\n"
             << "  \"pickup_position_fraction\": " << options.pickup_fraction << ",\n"
             << "  \"gain_per_m\": " << options.gain_per_m << ",\n"
             << "  \"peak_displacement_m\": " << peak << ",\n"
             << "  \"peak_pcm_linear\": " << peak_linear << ",\n"
             << "  \"initial_energy_j\": " << energies.front() << ",\n"
             << "  \"final_sample_energy_j\": " << energies.back() << '\n'
             << "}\n";
    metadata.close();
    std::cout << "Synthetic reference render completed: " << sample_count << " samples at "
              << options.sample_rate << " Hz; " << p.mode_count << " modes.\n"
              << "Peak fixed-gain level: " << std::setprecision(8) << peak_linear
              << "; initial energy: " << energies.front() << " J.\n"
              << "Output: " << options.output_dir.string() << '\n'
              << "This is string displacement, not a measured or validated setar sound field.\n";
    return 0;
}
} // namespace

int main(int argc, char** argv) {
    if (argc == 2 && std::string_view(argv[1]) == "--help") {
        print_help();
        return 0;
    }
    try {
        return render(parse_options(argc, argv));
    } catch (const std::invalid_argument& error) {
        std::cerr << "Invalid input: " << error.what() << '\n';
        return 2;
    } catch (const std::exception& error) {
        std::cerr << "Render failed: " << error.what() << '\n';
        return 1;
    }
}
