#pragma once

// Performance layer: converts a text score or MIDI-like note events into physical
// GestureEvents (nail strokes, finger presses/moves/releases) on persistent strings.
//
// STATUS: proposal-grade control grammar, NOT practitioner-reviewed (REQ-PRF-002 is
// open). Stroke names, dynamics and ornaments here are engineering placeholders.
// Pitch is never set directly: a note chooses a physical string and finger position
// from the fret map; the realized pitch is whatever the physical model produces.

#include "setar/instrument/config.hpp"
#include "setar/instrument/gesture.hpp"

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace setar::instrument {

// Nail stroke defaults for one dynamic level.
struct StrokeDynamics {
    double speed_m_per_s = 0.0;
    double release_force_n = 0.0;
};

struct PerformanceSettings {
    double pluck_distance_from_bridge_m = 0.0;  // nail contact point = bridge - this
    double finger_offset_behind_fret_m = 0.0;   // pad position = fret - this (toward nut)
    double press_depth_m = 0.0;                 // pad target below the fret-top line
    double finger_speed_m_per_s = 0.0;          // pad descent/ascent speed
    double finger_gap_m = 0.0;                  // pad start height above the string
    double finger_lead_time_s = 0.0;            // press this long before the stroke
    double nail_gap_m = 0.0;
    double nail_travel_m = 0.0;
    double stroke_angle_rad = 0.0;              // magnitude; direction sign from stroke type
    double course_sweep_delay_s = 0.0;          // delay between strings within one stroke
    StrokeDynamics pp{}, p{}, mp{}, mf{}, f{}, ff{};
};

// Synthetic defaults (documented placeholders).
[[nodiscard]] PerformanceSettings default_performance_settings();

struct TimedGesture {
    double time_s = 0.0;
    GestureEvent event{}; // sample_offset/sequence are assigned by schedule()
};

struct Score {
    std::string title;
    std::string status;   // copied from the score header; must be shown in reports
    double end_time_s = 0.0;
    std::vector<TimedGesture> gestures; // sorted by time, stable for equal times
};

// Parses the line-oriented score format documented in docs/performance/07-score-format.md.
// Throws std::invalid_argument("line N: ...") on any malformed or physically invalid line.
[[nodiscard]] Score parse_score(std::string_view text, const InstrumentConfig& config,
                                const PerformanceSettings& settings);

struct ScheduledGesture {
    std::uint64_t sample_index = 0;
    GestureEvent event{};
};
// Converts times to absolute sample indices (round to nearest) with stable sequence
// numbers. Throws for a non-finite/invalid sample rate.
[[nodiscard]] std::vector<ScheduledGesture> schedule(const Score& score, double sample_rate_hz);

struct FretChoice {
    std::size_t string_index = 0;
    std::size_t fret_number = 0;      // 0 = open string, k = config.frets[k-1]
    double finger_position_m = 0.0;   // 0 for open
    double nominal_frequency_hz = 0.0;// ideal-string estimate (open f * L/(L - x_fret))
    double cents_error = 0.0;         // nominal vs requested
};

// Nearest available (string, fret) for a requested frequency given the open-string
// frequencies of the tuning. Prefers the lowest |cents_error|, then the lower fret.
// `allowed` restricts the candidate strings (empty = all). Throws if none is in range.
[[nodiscard]] FretChoice choose_fret(double frequency_hz, const InstrumentConfig& config,
                                     std::span<const double> open_frequency_hz,
                                     std::span<const std::size_t> allowed = {});

// Realtime-safe MIDI-like mapper (no allocation after prepare). One finger slot per
// string is used; note-on presses (or releases for open strings) then strokes.
class MidiGestureMapper {
  public:
    void prepare(const InstrumentConfig& config, std::span<const double> open_frequency_hz,
                 const PerformanceSettings& settings, double reference_a4_hz);
    // Writes at most out.size() events; returns the count. Unmappable notes return 0.
    std::size_t note_on(std::uint8_t note, std::uint8_t velocity, std::uint32_t sample_offset,
                        std::span<GestureEvent> out) noexcept;
    std::size_t note_off(std::uint8_t note, std::uint32_t sample_offset,
                         std::span<GestureEvent> out) noexcept;

  private:
    struct NoteRoute {
        bool valid = false;
        std::uint8_t string_index = 0;
        double finger_position_m = 0.0;
        double finger_height_m = 0.0;
    };
    std::vector<NoteRoute> routes_; // 128 entries after prepare
    std::vector<std::int16_t> sounding_note_; // per string, -1 = none
    PerformanceSettings settings_{};
    std::vector<double> nail_position_m_;     // per string
};

} // namespace setar::instrument
