#pragma once

// Physical gesture events accepted by setar::instrument::Instrument::process().
// A gesture refers to a physical string (0..3) and a physical location; it never
// creates a string, voice or sample. Note/MIDI mapping lives in performance.hpp.
//
// Ordering: within one process() call events must have nondecreasing
// sample_offset; equal offsets are applied in ascending `sequence`. An event is
// applied before the sample at its offset is computed. Out-of-range or
// out-of-order events are rejected and counted, never reordered silently.

#include <cstdint>

namespace setar::instrument {

enum class GestureKind : std::uint8_t {
    // A fingernail stroke. The nail surface starts `gap_m` behind the string's
    // current position along the stroke direction d = (sin(angle), cos(angle)) in
    // (vertical, horizontal) components, i.e. angle 0 pushes the string in +h,
    // angle pi pushes it in -h, angle +pi/2 pushes it away from the soundboard.
    // The surface then moves along d at `speed_m_per_s`. Contact uses
    // ContactConfig::nail. The nail slips off (contact removed, stored contact
    // energy booked as release dissipation) when the contact force reaches
    // `force_n`, or when the surface has travelled `travel_m` (stroke ends).
    // Fields: position_m (x from nut), angle_rad, speed_m_per_s (> 0), force_n (> 0),
    //         gap_m (>= 0), travel_m (> gap_m).
    NailStroke = 0,
    // Finger pad `slot` (0 or 1) placed at x = position_m starting `gap_m` above the
    // string's current vertical position, descending at speed_m_per_s to the
    // absolute vertical target height_m (m, string rest line = 0, negative toward
    // the soundboard; use fret_top_height_m() - depth to stop on a fret, or ~0 for
    // a light touch / harmonic). If the slot is already active it is moved, not
    // duplicated: the pad keeps its vertical state and retargets.
    // Fields: slot, position_m, height_m, speed_m_per_s (> 0), gap_m (>= 0).
    FingerPress = 1,
    // Slide active pad `slot` along the string to position_m at speed_m_per_s (> 0).
    FingerMove = 2,
    // Raise pad `slot` at speed_m_per_s (> 0); it deactivates once it is gap_m above
    // the string with no contact force.
    FingerRelease = 3,
    // Containment/panic: zero all mechanical state and gestures at this offset.
    // Booked in the ledger as an explicit reset, not as physics.
    Silence = 4,
};

struct GestureEvent {
    std::uint32_t sample_offset = 0;
    std::uint32_t sequence = 0;
    std::uint8_t string_index = 0;
    std::uint8_t slot = 0;
    GestureKind kind = GestureKind::NailStroke;
    double position_m = 0.0;
    double angle_rad = 0.0;
    double speed_m_per_s = 0.0;
    double force_n = 0.0;
    double gap_m = 0.0;
    double travel_m = 0.0;
    double height_m = 0.0;
};

} // namespace setar::instrument
