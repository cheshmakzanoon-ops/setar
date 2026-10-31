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
    double pluck_distance_from_bridge_m = 0.0; // nail contact point = bridge - this
    double finger_offset_behind_fret_m = 0.0;  // nominal pad offset behind the fret (toward nut)
    double press_depth_m = 0.0;                // pad target below the fret-top line
    double finger_speed_m_per_s = 0.0;         // pad descent/ascent speed
    double finger_gap_m = 0.0;                 // pad start height above the string
    double finger_lead_time_s = 0.0;           // press this long before the stroke
    double nail_gap_m = 0.0;
    double nail_travel_m = 0.0;
    double stroke_angle_rad = 0.0;     // magnitude; direction sign from stroke type
    double course_sweep_delay_s = 0.0; // delay between strings within one stroke
    StrokeDynamics pp{}, p{}, mp{}, mf{}, f{}, ff{};
    // Light-touch pad (harmonic / damping): target height -touch_depth_m below the rest line.
    double touch_depth_m = 0.0;
    double damp_position_fraction = 0.0; // damp pad at this fraction of the speaking length
    double damp_hold_s = 0.0;            // default damp duration before the pad lifts
};

// Throws std::invalid_argument naming the first nonfinite/out-of-domain field.
void validate(const PerformanceSettings& settings);

// Synthetic defaults (documented placeholders).
[[nodiscard]] PerformanceSettings default_performance_settings();

// Pad position (m from the nut) that stops fret k >= 1:
//   x_k - min(finger_offset_behind_fret_m, 0.4*(x_k - x_{k-1})),  x_0 = 0 (nut),
// so the pad always lies between fret k and the previous fret even where comma-step
// frets are only millimetres apart. Throws std::invalid_argument for k = 0 or k > frets.
[[nodiscard]] double finger_position_for_fret(const InstrumentConfig& config, std::size_t fret,
                                              const PerformanceSettings& settings);

struct TimedGesture {
    double time_s = 0.0;
    GestureEvent event{}; // sample_offset/sequence are assigned by schedule()
};

struct Score {
    std::string title;
    std::string status; // copied from the score header; must be shown in reports
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
    std::size_t fret_number = 0;       // 0 = open string, k = config.frets[k-1]
    double finger_position_m = 0.0;    // 0 for open
    double nominal_frequency_hz = 0.0; // ideal-string estimate (open f * L/(L - x_fret))
    double cents_error = 0.0;          // nominal vs requested
};

// Nearest available (string, fret) for a requested frequency given the open-string
// frequencies of the tuning. Prefers the lowest |cents_error|; candidates within 1 cent
// of the best are ties, resolved by the lower fret, then the lower string index.
// `allowed` restricts the candidate strings (empty = all). Throws if none is in range.
// Candidates with |cents_error| > max_abs_cents are out of range.
[[nodiscard]] FretChoice choose_fret(double frequency_hz, const InstrumentConfig& config,
                                     std::span<const double> open_frequency_hz,
                                     std::span<const std::size_t> allowed = {},
                                     double max_abs_cents = 100.0);

// Realtime-safe MIDI-like mapper (no allocation after prepare). Note frequencies are
// 12-TET from reference_a4_hz; setar fret maps are not 12-TET, so each note maps to the
// nearest fret within 50 cents (FretChoice::cents_error is kept in route()) or is
// unmappable. Note-on: lift the damping pad (slot 1) if down, press slot 0 (fretted) or
// lift it (open), then a down-stroke. The nail starts finger_lead_time_s * speed farther
// away so the stroke lands after the finger settles; this is the mapper's latency.
// Note-off of the sounding note: lower the damping pad (slot 1, light touch) and lift
// slot 0. Events are written only if all fit in `out`; otherwise 0 and no state change.
// Each emitted event gets the next value of one ascending sequence counter.
class MidiGestureMapper {
  public:
    void prepare(const InstrumentConfig& config, std::span<const double> open_frequency_hz,
                 const PerformanceSettings& settings, double reference_a4_hz);
    // Writes at most out.size() events; returns the count. Unmappable notes return 0.
    std::size_t note_on(std::uint8_t note, std::uint8_t velocity, std::uint32_t sample_offset,
                        std::span<GestureEvent> out) noexcept;
    std::size_t note_off(std::uint8_t note, std::uint32_t sample_offset,
                         std::span<GestureEvent> out) noexcept;
    // Route of a note after prepare (valid == false for unmappable notes).
    struct NoteRoute {
        bool valid = false;
        std::uint8_t string_index = 0;
        std::uint8_t fret_number = 0; // 0 = open
        double finger_position_m = 0.0;
        double finger_height_m = 0.0;
        double cents_error = 0.0;
    };
    [[nodiscard]] NoteRoute route(std::uint8_t note) const noexcept;
    [[nodiscard]] bool prepared() const noexcept { return !routes_.empty(); }

  private:
    std::vector<NoteRoute> routes_;           // 128 entries after prepare
    std::vector<std::int16_t> sounding_note_; // per string, -1 = none
    PerformanceSettings settings_{};
    std::vector<double> nail_position_m_;   // per string
    std::vector<double> damp_position_m_;   // per string
    std::vector<std::uint8_t> finger_down_; // per string, slot 0
    std::vector<std::uint8_t> damp_down_;   // per string, slot 1
    std::uint32_t next_sequence_ = 0;       // ascending across calls (equal-offset order)
};

} // namespace setar::instrument
