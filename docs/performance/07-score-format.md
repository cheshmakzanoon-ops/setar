# Prototype score format and gesture expansion

**Status: implemented proposal-grade control grammar, not practitioner-reviewed.**
The grammar below is executable in
[`performance.hpp`](../../include/setar/instrument/performance.hpp) and
[`performance.cpp`](../../src/instrument/performance.cpp), and tested by the
`score.*` CTests in
[`performance_tests.cpp`](../../tests/instrument/performance_tests.cpp). It is an
engineering placeholder for REQ-PRF-002, which remains open: no stroke name,
dynamic level, ornament or default parameter here has been reviewed by an
identified setar player. Do not describe a rendered score as an authentic
technique, a transcription, or a validated gesture.

## Purpose and boundary

A score is a deterministic, human-editable list of **physical gestures** on the
four persistent strings of the playable prototype. It never selects a recording,
a sample, or an oscillator pitch. Each command expands to the
[`GestureEvent`](../../include/setar/instrument/gesture.hpp) kinds the engine
accepts — `NailStroke`, `FingerPress`, `FingerMove`, `FingerRelease`, `Silence` —
with string indices, positions in metres from the nut, speeds in m/s, forces in
newtons and heights in metres relative to the string rest line. The realized
pitch, attack and decay are whatever the physical model produces from those
gestures.

Inputs: score text, a validated `InstrumentConfig` (strings, fret positions,
action line) and `PerformanceSettings`. Output: a `Score` whose gestures are
sorted by time (stable for equal times), plus `schedule()`, which converts times to
absolute sample indices (`round(t * fs)`) and assigns ascending `sequence`
numbers. The renderer assigns block-relative `sample_offset` values.

## Lexical rules

- One command per line. `#` starts a comment that runs to the end of the line.
- Header lines: `title <text>` (optional), `status <text>` (**required**; reports
  must print it), `tempo <bpm>` (optional, at most once, before every timed command).
- Times: `@<seconds>` is absolute seconds. A bare number is in **beats** and is
  accepted only after a `tempo` header (`seconds = beats * 60 / bpm`). Durations
  (`dur=`) use the same two forms. `rate=` is always strokes per second.
- Timed commands must be **nondecreasing in file order**; every command after the
  first `end` is rejected. `end <time>` is **required** and must not precede any
  expanded gesture.
- Keys are `key=value` with no spaces. Unknown, duplicate or missing required keys,
  malformed numbers, string indices outside 0–3, frets beyond the map and invalid
  settings are rejected with `line N: <reason>`. Nothing is clamped or reordered.

## Commands

| Command | Keys | Expansion |
| --- | --- | --- |
| `pluck <t>` | `string=` (req), `fret=`, `dyn=`, `dir=`, `from_bridge=` | Optional finger change `finger_lead_time_s` before `t` (fret ≥ 1: `FingerPress` slot 0; `fret=0`: `FingerRelease` of slot 0 if it is down; omitted: no change), then one `NailStroke` at `t`. |
| `strum <t>` | exactly one of `course=0..2` or `strings=a,b,..`; `dyn=`, `dir=`, `from_bridge=` | One `NailStroke` per string, `course_sweep_delay_s` apart, in sweep order (below). Courses are {0}, {1}, {2,3}. |
| `tremolo <t>` | `string=`, `dur=`, `rate=` (req), `fret=`, `dyn=`, `dir=` (first stroke), `from_bridge=` | Optional finger change as for `pluck`, then `floor(dur*rate)` strokes at `t + i/rate`, alternating direction. A placeholder for the tremolo players often call *riz*; it is not a model of that technique. |
| `press <t>` | `string=`, `fret=` ≥ 1 (req), `slot=` | `FingerPress` at `t` (no lead). |
| `slide <t>` | `string=`, `to_fret=` ≥ 1, `dur=` (req), `slot=` | `FingerMove` of an active pad to the new stop position at speed `|Δx|/dur`. |
| `release <t>` | `string=` (req), `slot=` | `FingerRelease` of an active pad. |
| `touch <t>` | `string=`, `at=` in (0, 1) (req), `slot=` | Light `FingerPress` at `x = at * speaking length` with target height `-touch_depth_m` (harmonic or damping touch). |
| `damp <t>` | `string=` (req), `dur=` | Light pad (slot 1) at `damp_position_fraction` of the speaking length, lifted after `dur` (default `damp_hold_s`). |
| `silence <t>` | — | Engine `Silence` (containment reset of all mechanical state); all pad states are cleared. |
| `end <t>` | — | Score duration. |

`dyn=` is one of `pp p mp mf f ff` (default `mf`); each level supplies a nail
surface speed and a slip-off force. `dir=` is `down` (default) or `up`. `slot=` is
0 or 1 (default 0). Strokes contact the string at `bridge_position_m - from_bridge`
(default `pluck_distance_from_bridge_m`).

### Finger stop position

A pad that stops fret *k* sits at

\[
x_\mathrm{pad} = x_k - \min\!\left(o,\; 0.4\,(x_k - x_{k-1})\right),\qquad x_0 = 0,
\]

where `o = finger_offset_behind_fret_m`. The clamp keeps the pad strictly between
the target fret and the previous one where comma-step frets are only millimetres
apart (the default 17-step Pythagorean map has gaps near 8 mm in the first octave
and 4 mm in the second). `finger_position_for_fret()` implements this for the
score and MIDI paths. Its vertical target is `fret_top_height_m(x_pad) - press_depth_m`.

### Ordering guard for finger changes

A finger change requested by `pluck`/`tremolo` (or a `press`) may not be scheduled
before the previous stroke on the same string; otherwise that earlier note would
ring at the new stop. Such a score is rejected (`finger change at ... precedes the
previous stroke`); space the notes further apart or use `slide`.

## Stroke direction convention (placeholder)

`NailStroke` uses direction `d = (sin a, cos a)` in (vertical, horizontal)
components. `dir=down` gives `a = -π + ε`, so `d_h < 0` (toward the string-0 side);
`dir=up` gives `a = -ε`, so `d_h > 0`. Both lean toward the soundboard (`d_v < 0`)
by `ε = stroke_angle_rad`. A multi-string stroke meets strings in the order its
motion crosses them, assuming the lateral bridge order equals the physical index
order: up (+h) strokes visit ascending indices, down (−h) strokes descending
indices. Whether "down" corresponds to a particular Persian stroke name, hand
orientation or nail face is **not established** here and needs practitioner
elicitation ([gesture verification](01-gesture-verification.md)).

## MIDI-like mapping

`MidiGestureMapper` is the realtime adapter used by host code. `prepare()` (control
thread, may allocate) maps each MIDI note — 12-TET frequency from `reference_a4_hz`
— to the nearest (string, fret) of the configured fret map within 50 cents using
`choose_fret()`; other notes are unmappable and produce no events. Setar fret maps
are not 12-TET, so every route keeps its `cents_error`; a keyboard is a convenience
interface, not a tuning claim. `choose_fret()` prefers the smallest |cents error|
and treats candidates within 1 cent of it as ties, resolved by the lower fret and
then the lower string index.

`note_on`/`note_off` are `noexcept` and allocation-free (verified by
`score.midi_mapper` with a counting global allocator). Note-on lifts the damping
pad (slot 1) if down, presses slot 0 for a fretted note or lifts it for an open
note, then issues a down-stroke whose nail starts `speed * finger_lead_time_s`
farther away, so contact occurs after the finger has settled. That extra approach
is the mapper's latency (about `finger_lead_time_s`). Velocity 1–127 interpolates
linearly across the six dynamic levels; velocity 0 is a note-off. Note-off of the
string's sounding note lowers the damping pad and lifts slot 0; a stale note-off
(another note owns the string) emits nothing. Events are written only if all fit in
the output span; every event gets the next value of one ascending sequence counter.

## Synthetic defaults

`default_performance_settings()` returns placeholders inside order-of-magnitude
ranges chosen for numerical exercise, not measured hand behavior:

| Field | Value | Note |
| --- | --- | --- |
| `pluck_distance_from_bridge_m` | 0.12 | contact point from the bridge |
| `finger_offset_behind_fret_m` | 0.008 | nominal, clamped as above |
| `press_depth_m` | 0.0008 | pad target below the fret-top line |
| `finger_speed_m_per_s`, `finger_gap_m` | 0.1, 0.001 | pad descent and start height |
| `finger_lead_time_s` | 0.06 | covers gap + action + depth at the pad speed |
| `nail_gap_m`, `nail_travel_m` | 0.0008, 0.008 | stroke start clearance and maximum travel |
| `stroke_angle_rad` | 0.15 | elevation toward the soundboard |
| `course_sweep_delay_s` | 0.008 | between strings of one stroke |
| `pp … ff` speed (m/s) | 0.12, 0.18, 0.25, 0.35, 0.5, 0.7 | nail surface speed |
| `pp … ff` slip-off force (N) | 0.2, 0.35, 0.55, 0.8, 1.2, 1.8 | release threshold |
| `touch_depth_m`, `damp_position_fraction`, `damp_hold_s` | 0.0001, 0.37, 0.2 | light touch and damping |

## Demonstration scores

[`data/scores/`](../../data/scores/README.md) holds four scores of at most 8 s,
each with the status line *synthetic demonstration; not a transcription of a
performance or a validated gesture*. The `integration.setar_play` CTest renders
every `*.score` file there; the scores assume the default fret map has at least 17
frets.

## Tests, limitations and next actions

| CTest | Checks |
| --- | --- |
| `score.parse_valid` | Every command's expansion, beats/seconds, lead times, pad heights, stroke signs, slide speed, damp release, comma-step pad clamp in press/slide/pluck paths |
| `score.parse_errors` | 25 malformed scores reject with the expected `line N` message; invalid settings and unordered frets reject |
| `score.schedule_ordering` | Rounded sample indices, stable equal-time order, ascending sequence, invalid rate/unsorted rejection |
| `score.fret_choice` | Exact fixture fret, signed cents error, tie rules, allowed strings, range limits |
| `score.course_sweep` | Down/up sweep order and delays, list sorting, tremolo count/period/alternation |
| `score.midi_mapper` | Routes, determinism, unmappable/stale/insufficient-capacity cases, damping cycle, no allocation |

Limitations: one tempo per score; no continuous controller curves (vibrato,
lateral bends, pressure); slides move one pad at constant speed; a stroke always
contacts a single point; ornaments are fixed expansions, not context-dependent
realizations; MIDI uses one fretting slot per string, no MPE, no pitch bend.
Next actions: practitioner elicitation of stroke vocabulary and timing
([gesture verification](01-gesture-verification.md)), recorded gesture
trajectories replacing the constant-speed placeholders, and an MPE mapping per
[the MIDI/MPE chapter](03-midi-mpe-and-tuning.md).
