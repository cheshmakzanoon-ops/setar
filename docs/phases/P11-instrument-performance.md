# P11 — Assemble a playable, persistent physical instrument

**Status:** specification. **Prerequisites:** P06, P09, and P10. **Exit evidence:**
a deterministic instrument/performance contract that supports the intended
gestures without replacing physical state with independent note samples.

## Instrument identity and performance are different layers

The model has physical strings, contacts, a body, and acoustic observations.
The performer sends intentions that must be translated into motion and force.
A MIDI note number is not itself a complete physical gesture. This phase joins
those layers while preserving the difference between a realistic instrument
state and a convenient keyboard interface.

Read [performance](../performance/README.md),
[ethnomusicology](../ethnomusicology/README.md),
[contacts](../contacts/README.md), and [plugin](../plugin/README.md).
Practitioner review belongs in the control design, not only in the final
marketing demonstration.

## P11-T01: define persistent state ownership

Create an inventory of physical state: each string's motion, active contacts,
body/cavity state, finger/nail state, tuning/setup, and observation filters.
Specify which event changes which state and which state must continue ringing.
A note-on must not erase all previous motion simply because a conventional
synth voice allocator would start a new voice.

Separate physical string identity from playing course, MIDI channel, and
keyboard note. Define what happens when two incoming requests compete for one
physical string. Options such as physically plausible reassignment, explicit
rejection, or a declared expanded-instrument mode have different meanings.
Do not silently add extra physical strings and still describe the result as
one faithful specimen.

Keep state lifetimes and reset semantics precise. An emergency reset, a musical
mute, a host transport stop, and a sample-rate reprepare are distinct operations.
Test each with a ringing instrument and verify the stated behavior.

## P11-T02: specify a sample-stamped event model

Define event types, units, timestamps, order for simultaneous events, admissible
ranges, and queue capacity. Keep the engine contract independent of JUCE event
classes. Adapters translate host data into this contract and expose unsupported
or ambiguous requests rather than silently dropping them.

Use tests that render one event sequence with many block partitions. Compare
results within the declared deterministic/tolerance contract. Include zero-
length blocks, events at a block boundary, several events at one sample, and
overflow behavior. A controller change must not move in time because a UI
thread happened to run later.

Record external stochastic choices through a seed and algorithm version when
reproducibility matters. Random variability must have a physical/performance
interpretation and a controllable range. It should not camouflage missing
contact dynamics or create unstable parameter combinations.

## P11-T03: distinguish tuning from fingering

Represent open-string tuning, fret positions, chosen fingering, and pitch
expression separately. Changing the tuning can change tension and therefore
dispersion, body loading, and coupling. Changing a stopped position changes
contact geometry. A generic pitch multiplier does not reproduce both effects.

Implement a transparent tuning representation with culturally appropriate
metadata and editable intervals. Do not impose a universal quarter-tone grid
or treat one transcription system as a complete account of Iranian practice.
Document the chosen reference, source, and practitioner interpretation.

Verify that importing or editing a tuning does not create invalid tensions or
geometry. Display a meaningful rejection or constrained supported mode. Keep
the musical mapping and physical feasibility checks independently testable.

## P11-T04: define gesture trajectories

Specify right-hand excitation through direction, contact sequence, position,
depth, speed, and any supported force/compliance controls. Specify left-hand
stopping, damping, release, and pitch motion through their relevant physical
variables. Describe how each exposed musical control maps to those trajectories.

Use a small repertoire of documented gestures rather than an unbounded list of
articulation names with no implementation. For each gesture, state entry state,
trajectory, permitted overlap, expected physical work, and observable outcome.
Include gestures performed on already ringing strings and transitions between
open and stopped conditions.

Calibrate controller ranges through measurements and player feedback. A velocity
value need not map linearly to nail speed or delivered energy. Preserve monotonic
intent where appropriate, but let the physical model explain nonlinear outcome
instead of simply assigning a louder sample or larger output gain.

## P11-T05: implement keyboard and expressive adapters

Start with one clearly specified mapping. Define channel ownership, pitch-bend
range, note priority, sustain behavior if offered, and expression routing.
Conventional keyboard sustain may be a convenience feature rather than a native
setar action; label it accordingly. An expressive protocol can carry useful
control but does not solve the inference from note intent to physical gesture.

Test duplicate note-ons, note-offs without a matching note, lost controllers,
all-notes-off, and rapid rearticulation. Keep panic behavior bounded and safe.
Do not let a host-specific MIDI interpretation change the numerical reference
model. Preserve recorded engine events so the same performance can be replayed
headlessly for regression tests.

## P11-T06: conduct a practitioner-centered review

Ask players to evaluate control predictability, range, gesture transitions,
damping, and expressive response separately from isolated timbre. Record
what they were asked to do and the control setup they used. A preference for
one interface is not automatically evidence of a better physical model.

Translate feedback into specific hypotheses: missing contact timing, unnatural
release, excessive parameter coupling, or an ambiguous mapping. Reproduce the
case in the event harness before changing code. Keep culturally specific
terminology reviewed and avoid promising complete coverage of a tradition from
a small participant group.

> Assemble one persistent four-string instrument state and one explicit control
> mapping. Define every event's units, ordering, capacity, and physical effect.
> Test ringing-state transitions and block-partition invariance. Demonstrate
> tuning, fingering, and excitation as separate mechanisms. Collect scoped
> practitioner evidence and keep convenience mappings distinct from physically
> native gestures. Do not create unlimited independent voices and call that a
> faithful single setar.
