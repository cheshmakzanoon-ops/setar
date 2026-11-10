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

## Entry packet and research map

Start executable work from the synthetic prototype, its configuration revision,
`ProcessSpec`, score, and initial ringing state. Keep the physical P06/P09/P10
validation prerequisites open: the current contacts, shared body and observation
are not measurements. Record each task with the
[finding template](../../templates/research-finding.md), preserving raw and derived
IDs, inspected source locations, uncertainty, disposition, and downstream owner.
The following source locations are reading routes; the
[source atlas](../research/source-atlas.md) records the access actually achieved.

| Question | Primary route and precise starting location | Project experiment/contract |
| --- | --- | --- |
| Courses, stroke direction, ringing and contextual tuning | `R-SETAR-TABASSIAN`, [performer-authored handout](https://www.atlasensemble.nl/assets/files/instruments/Setar/Setar%20by%20Kiya%20Tabassian.pdf), pp. 1–2 | [Elicitation](../ethnomusicology/elicitation-and-terminology.md); acquisition channel `gestures.performer` |
| Fret placement versus intended intervals | `R-FRETTING-POORHAYDARI-2023`, [journal article](https://journal.iftawm.org/previous/2023-volume-11-no-2/poorhaydari/), “Methodology and Initial Tests,” [7]–[8] | [Setup measurement](../measurement/strings-frets-and-setup.md); keep observation and theoretical proposal separate |
| Host timing and lifecycle | `R-SDK-JUCE-AUDIOPROCESSOR`, [official class reference](https://docs.juce.com/master/classjuce_1_1AudioProcessor.html), `processBlock`, `prepareToPlay`, state/reset methods; check the pinned checkout | [Event schema](../performance/02-event-schema.md) and [deterministic transitions](../performance/04-deterministic-transitions.md) |

Neither musical source supplies calibrated nail trajectories or certifies a
universal tuning. The public [campaign template](../../data/measurement-campaign-template.json)
contains no acquired practitioner evidence. Prepare questions and synthetic
fixtures now; participant contact and recording require the established authorized
acquisition route.

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

### Execution recipe

**Question and research.** Can a new stroke, selective damping action or competing
note request act on four continuing string histories without destroying unrelated
ringing? Read `R-SETAR-TABASSIAN`, pp. 1–2, for course and resonance descriptions,
then [physical ports and ownership](../architecture/02-physical-ports-and-state-ownership.md)
as a design contract. Search `setar paired course sympathetic resonance selective
damping` and the atlas's Persian playing-action terms if a description is unclear.
The empirical discriminator is synchronized motion of struck/unstruck strings and
bridge under the `interactions.cross_string` acquisition packet, with support and
damping intervention recorded; a stereo waveform alone cannot assign that motion.

1. Draw an ownership table for each string's two polarizations, each nail, the two
   finger slots, fret-contact memory, shared body, configuration and observation.
   Anchor string IDs to the configured physical order; `Course` groups IDs and
   never owns another oscillator. Record the current synthetic grouping separately
   from any later specimen/player grouping.
2. Freeze configuration, initial displacement/velocity, sample rate and event
   timeline. Compare a stroke from rest with the same stroke after residual motion;
   compare coupled and grounded-body conditions; apply damping on one string while
   tracing the others. Log displacement in m, force in N, energy in J and sample
   time, with numerical error separated from later sensor uncertainty.
3. Specify conflicting-request and lifecycle outcomes in a transition table.
   Musical damping changes contact; panic is nonphysical energy removal. Test
   transport-only bookkeeping and sample-rate changes against their declared
   policy before adding those proposed lifecycle distinctions.

**Translate and retain.** Current ownership is `Instrument` in
[instrument.hpp](../../include/setar/instrument/instrument.hpp) and
[instrument.cpp](../../src/instrument/instrument.cpp); `snapshot()`, `ledger()` and
offline displacement/velocity probes expose different diagnostics. `reset()` clears
all mechanical state and the clock; a contact-only reset API is not present.
Retain state/energy traces, course inventory, source findings and every conflict
outcome. `include/setar/engine/instrument.hpp` in the work order is a proposed
location, not a reason to duplicate the current owner.

**Acceptance and handoff.** `ENG-P11-T01` covers `REQ-PRF-001` and `REQ-CPL-002`.
Present CTests `performance.persistent_state`, `performance.restrike_memory`,
`coupling.cross_string_transfer` and `coupling.uncoupled_limit` check scoped synthetic
cases. Add independent transition expectations for untested policies; reject
silent string creation, unrelated state clearing or unexplained energy removal.
Record accepted/rejected/inconclusive/blocked scope and hand the ownership table to
P11-T02/P11-T05; measured sympathy remains blocked without the acquisition packet.

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

### Execution recipe

**Question and research.** Does one physical gesture happen at the same sample and
against the same existing state for every supported block partition? Read
`R-SDK-JUCE-AUDIOPROCESSOR` at `processBlock` against the pinned source, then the
[event schema](../performance/02-event-schema.md) and
[transition contract](../performance/04-deterministic-transitions.md). Search
`JUCE MidiBuffer samplePosition equal timestamp ordering`; use official versioned
documentation to resolve adapter behavior. For measured gesture replay, obtain the
`gestures.performer` clock/synchronization record; camera time or UI arrival time
is not automatically the audio sample clock.

1. Publish a table from physical action to `GestureKind` and each used field's SI
   unit, coordinate/sign, valid range and side effect. In current
   [gesture.hpp](../../include/setar/instrument/gesture.hpp), x starts at the nut;
   `sample_offset` is block-local, and equal-offset events require strictly
   increasing `sequence`. Events act before the sample is advanced. Define
   absolute-to-block conversion with `[start,start+frames)` ownership.
2. Compile a small canonical score through `parse_score()` and `schedule()` in
   [performance.cpp](../../src/instrument/performance.cpp). Retain original seconds,
   sample rate, rounded absolute sample indices and stable equal-time order.
   Independently calculate expected indices, including rounding collisions, then
   supply block offsets without rescheduling the physical trajectory.
3. Replay with one-frame, fixed and irregular blocks; include simultaneous
   press/stroke/release, a trajectory spanning blocks, zero frames, the exact end
   boundary, duplicate sequence, stale/late input and capacity overflow. Current
   `process()` rejects offsets outside its frames, including events passed in an
   empty block, and discards excess events with counters. Do not describe a future
   release-priority queue as already implemented.

**Translate and retain.** Keep expected/applied timelines, configuration/score
hashes, `ProcessSummary`, snapshots, pressure output and failure examples. The
handbook's epoch, event/owner IDs, transactions and resource revisions remain
proposed additions beyond `GestureEvent`; document their required semantics before
implementation. Timing uncertainty belongs in captured-source conversion, not an
audio comparison tolerance that excuses a one-sample shift.

**Acceptance and handoff.** Under `ENG-P11-T02`/`REQ-PRF-002`, present
`score.schedule_ordering`, `engine.event_ordering`, `realtime.block_partition`,
`realtime.capacity_overflow` and `contact.gesture_replay` check specific synthetic
paths. `performance.gesture_matrix` remains a planned acceptance ID. Require
matching event targets/order before same-build bitwise audio comparison; reject
silent reorder, double application or undocumented overflow. Hand accepted traces
and remaining schema gaps to P11-T04/P11-T05 and the P14 adapter work.

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

### Execution recipe

**Question and research.** Which discrepancy is a tuning target, a fret geometry
change, a stopping action or an observed pitch trajectory? Read
`R-SETAR-TABASSIAN`, pp. 1–2, alongside `R-FRETTING-POORHAYDARI-2023`, “Methodology
and Initial Tests,” [7]–[8]; record performer observations separately from the
author's theoretical proposals. Use [performance and tuning](../ethnomusicology/performance-and-tuning.md)
and [string/fret measurement](../measurement/strings-frets-and-setup.md). Search
`setar fretting intervals intended played pitch` or `سه‌تار پرده بندی کوک`;
stop with contextual alternatives or a specific missing measurement, not one
supposedly authoritative interval grid.

1. Prepare four linked tables: physical string/open-frequency target in Hz;
   measured fret contact location in m from the nut; selected string/finger action;
   measured pitch versus time. Preserve absolute reference, ratios/cents, notation,
   player/context, configuration version and uncertainty. Use calibrated length and
   frequency references, recording clock accuracy and fret-contact convention.
2. Compare open/stopped notes with finger maintained versus lifted/replaced, other
   strings damped versus ringing, and a declared alternate tuning. Keep support,
   pluck position and estimator window controlled; record drift before/after.
   Retain raw motion/audio and rejected peak assignments. The ideal relation
   `x=L*(1-1/r)` is a comparator, not authority to reposition a player's fret.
3. Audit import/edit feasibility before preparing a configuration: ordered frets,
   positive tension, geometry, admissible loss and supported range. Current
   `StringConfig.total_length_m` is nut-to-tail; `bridge_position_m` is speaking
   length. Confusing them corrupts both tension inference and fingering.

**Translate and retain.** Current [setar_preset.hpp](../../include/setar/instrument/setar_preset.hpp)
has `TuningPreset`, `FretMapDefinition`, `tension_for_frequency()` and
`fret_positions()`; `choose_fret()` returns a nominal ideal-string estimate and
signed cents error, not measured played pitch. Keep the synthetic preset unchanged
as a comparator. Tension-rise physics and continuous physical retuning are absent
from the present linear-string prototype; a frequency multiplier cannot stand in
for either. Retain four-table joins, geometry/pitch plots and accepted, rejected,
inconclusive or blocked interpretations.

**Acceptance and handoff.** `ENG-P11-T02`/`REQ-PRF-002` owns the grammar. Present
`preset.tension_inverse`, `preset.fret_positions`, `preset.invalid_inputs`,
`score.fret_choice` and `engine.fretted_pitch` verify fixture arithmetic/behavior;
planned `performance.gesture_matrix` requires reviewed practice. Reject invalid
geometry, unreported retuning resets or a universal-tuning claim from one source.
Pass contextual maps, observed uncertainty and model limits to P11-T04 and P12;
absent specimen/player records keep empirical acceptance blocked.

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

### Execution recipe

**Question and research.** Can the chosen control distinguish two intended attacks
or damping transitions through motion and transferred work, including on a ringing
string? Begin with `R-SETAR-TABASSIAN`, pp. 1–2, and the
[gesture verification](../performance/01-gesture-verification.md) and
[pluck/contact measurement](../measurement/pluck-contact-and-motion.md) chapters.
For the collision method, inspect `R-CONTACT-BILBAO-2014` through the atlas and
record the actually read potential/work/solve sections; the atlas does not claim
every derivation reviewed. Search `setar fingernail approach release trajectory`
and `physical modelling nail string contact work`. This supplies a method and
elicitation route, not setar nail coefficients.

1. Select a bounded matrix: opposite stroke directions, single-string versus
   paired-course sweep, press–pluck–release, and restrike while ringing. For each,
   record intended contrast, initial state, contacted string order, allowed
   overlap and observable contact/release time. Obtain authorized synchronized
   motion, force where available, and acoustic channels through `excitation.nail_flesh`
   and `gestures.performer`; calibrate spatial scale, clock offset and sensor axes.
2. Translate trajectories to x from nut in m, vertical positive away from the
   board, horizontal toward string 3, angle in rad, speed in m/s and force in N.
   Current `NailStroke` uses direction `(sin(angle),cos(angle))`, prescribed speed
   and force/travel release. Separate measured force from its release-force
   parameter, and contact width from an interpolation choice. Bound uncertainty
   from occlusion, frame timing and unavailable force data.
3. Freeze body/observation and compare direct `GestureEvent` traces with
   `parse_score()` expansion. Sweep one `PerformanceSettings` quantity at a time;
   compare onset, release, finger trajectory and `EnergyLedger` work, including
   phase-dependent restrikes. A monotonic isolated-stroke control need not produce
   monotonic loudness on every ringing state.

**Translate and retain.** Keep the original traces, calibrated derivatives,
trajectory/event table, energy/timing plots and disputed gesture labels. Current
`PerformanceSettings` and `GestureKind::FingerPress`, `FingerMove`, `FingerRelease`
are in [performance.hpp](../../include/setar/instrument/performance.hpp) and
[gesture.hpp](../../include/setar/instrument/gesture.hpp). A general recorded-path
player or anatomical hand model is proposed, not present.

**Acceptance and handoff.** `ENG-P11-T02`/`REQ-PRF-002` connects to planned
`performance.gesture_matrix`; present `score.parse_valid`, `score.course_sweep`,
`contact.release_convergence` and `performance.restrike_memory` cover engineering
fixtures. Predeclare timing/work tolerances from capture and numerical uncertainty;
reject reset-dependent attacks, unexplained energy or contact timing outside that
budget. An accepted synthetic trajectory remains unreviewed musical evidence.
Hand accepted/rejected/inconclusive/blocked cases to P11-T05/P11-T06 and P05/P06
when the contact law, rather than the controller mapping, is implicated.

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

### Execution recipe

**Question and research.** Does each supported host intent resolve to feasible
actions on the four strings, preserving ownership and timing during reuse? Read
`R-SDK-JUCE-AUDIOPROCESSOR` against the pinned checkout and the official MIDI
Association MPE specification linked by
[MIDI/MPE and tuning](../performance/03-midi-mpe-and-tuning.md). Record its edition
before specifying zones, pitch-bend ranges and expression initialization. Search
`MPE member channel reuse initial expression pitch bend range` in official sources;
protocol semantics do not provide calibrated physical force or a practitioner
mapping. Use the P11-T06 controlled controller task as the measured-data route.

1. Write the supported mapping table before extending code: input identity/channel,
   requested pitch, feasible string/fret, priority/conflict policy, note-off action,
   controller interpolation, latency and rejection. Current
   `MidiGestureMapper::prepare()` precomputes 12-TET note routes against the supplied
   fret map within 50 cents; `route()` exposes cents error. It is a MIDI-like
   note-on/off mapper without channel/MPE, pitch-bend or pressure APIs.
2. Exercise `note_on()`/`note_off()` in an adapter-only harness using expected
   physical events calculated from the table. Include zero-velocity note-on,
   duplicate/stale release, impossible pitch, rearticulation, insufficient output
   capacity and competing requests on one string. Inspect the current all-or-nothing
   emission rule and single sounding-note bookkeeping before designing richer
   ownership; MIDI channel number must never become a fifth physical string.
3. For proposed MPE, add expression-before-note, channel reuse, zone/range changes,
   disconnect, dense updates and distinct all-notes-off/all-sound-off cases.
   Normalize to sample-stamped events, then compare the headless direct-event
   render with the adapter render under identical state and block partitions.
   Calibrate physical controller ranges separately from display normalization;
   record unsupported bend/tension mechanisms rather than silently transposing.

**Translate and retain.** Current implementation is
[performance.cpp](../../src/instrument/performance.cpp), with optional host use in
[SetarProcessor.cpp](../../plugin/Source/SetarProcessor.cpp). Keep raw host-message
fixtures, resolved target/ownership tables, engine events, rejection counts and
latency traces. `src/plugin/event_adapter.cpp` and MPE extensions remain proposed;
source inspection is not a compiled/qualified host result.

**Acceptance and handoff.** The exact crosswalk is `ENG-P11-T02`/`REQ-PRF-002` plus
`ENG-P14-T02`/`REQ-PLG-002`. Present `score.midi_mapper` checks routing, timing,
capacity and allocation on its covered path. Planned `plugin.event_partition`,
`plugin.state_roundtrip`, `plugin.malformed_state` and `performance.gesture_matrix`
remain separate gates. Reject stale expression changing an unrelated string,
unreported dropped release or block-dependent timing; retain a blocked result for
unimplemented protocols. Hand the explicit support matrix to P14 and practitioner
review before calling the mapping expressive or idiomatic.

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

### Execution recipe

**Question and research.** Can a practitioner reliably produce and distinguish the
agreed stroke, damping and transition contrasts with the declared interface?
Read `R-SETAR-TABASSIAN`, pp. 1–2, and `R-FRETTING-POORHAYDARI-2023`, “Methodology
and Initial Tests,” [7]–[8], as contextual starting points, then
[elicitation](../ethnomusicology/elicitation-and-terminology.md),
[consent and rights](../ethnomusicology/participants-consent-and-rights.md) and
[performance validation](../performance/05-performance-validation.md). Search
`setar stroke damping ornament demonstration` plus participant-supplied Persian
terms. Do not treat one account, survey population or proposed scale as universal
practice. No participant has reviewed this prototype; preparing the study does
not authorize contact.

1. Prepare an open question guide, participant-role scope, consent/retention plan
   and task matrix. Ask participants, once authorized, how they choose tuning,
   frets and contrastive actions before presenting engineering labels. Keep
   intended action, observed demonstration and developer interpretation in
   separate fields; allow “not representative” takes and contextual alternatives.
2. Pilot a controlled task and a natural phrase for each contrast: continuing
   paired-course resonance versus selective damping, opposite strokes, and an
   open–stopped–released transition. Record instrument/setup, controller model,
   settings, event stream, latency path, monitoring and prior prototype exposure.
   Synchronize permitted motion/audio/control capture with calibration uncertainty;
   do not infer a nail force from a preference score.
3. Separate exploratory feedback from confirmatory evaluation. Reserve player,
   session or phrase groups before response tuning, retaining advisory exposure
   history. Counterbalance variants where comparisons are intended and keep
   prompts neutral. Translate each issue into a reproducible event fixture and a
   falsifiable alternative: scheduling delay, contact release, tuning map,
   observation coloration or missing physical mechanism.

**Translate and retain.** Retain permission-controlled raw records and a public-safe
task/annotation manifest through `gestures.performer`, with reviewed terminology,
disagreement, failed attempts and uncertainty. Use the finding template to give
each hypothesis an accepted/rejected/inconclusive/blocked disposition. Proposed
`data/performance/gesture-taxonomy.json` and `tests/performance/phrase_tests.cpp`
are handoff destinations in `ENG-P11-T02`; current
[score files](../../data/scores/README.md) and `PerformanceSettings` provide
engineering replay, not recorded or endorsed repertoire.

**Acceptance and handoff.** `REQ-PRF-002` requires planned
`performance.gesture_matrix` and `performance.repertoire_holdout`. Present
`score.course_sweep` and `contact.gesture_replay` establish repeatable synthetic
actions only. Predeclare task success and uncertainty reporting after the pilot,
before confirmatory data; reject claims based solely on pleasant timbre, leading
questions or feedback repeatedly used for tuning. Without authorized participants,
deliver the protocol and replay matrix as blocked practitioner evidence. Pass
scoped feedback to P12/P15/P17 with held-out-group and attribution restrictions.

> Assemble one persistent four-string instrument state and one explicit control
> mapping. Define every event's units, ordering, capacity, and physical effect.
> Test ringing-state transitions and block-partition invariance. Demonstrate
> tuning, fingering, and excitation as separate mechanisms. Collect scoped
> practitioner evidence and keep convenience mappings distinct from physically
> native gestures. Do not create unlimited independent voices and call that a
> faithful single setar.

## Exit and handoff check

- The same configuration, initial state and normalized event stream reproduce the
  declared transition traces; all four physical string histories remain explicit.
- Tuning targets, fret geometry, chosen contacts and played pitch have separate
  versioned records; unsupported physical transitions and controller protocols
  have named outcomes.
- The finding packet includes source access/locations, raw/derived hashes, event
  coordinates, work/timing diagnostics, failed cases and independent comparators.
- Existing CTests, proposed acceptance cases and practitioner evidence have
  separate result columns. Replaying a score cannot close `REQ-PRF-002`.
- P12 receives parameter/gesture uncertainty and frozen acquisition groups; P14
  receives timing/ownership/capacity contracts; P15/P17 receive reviewed tasks and
  the exact limits of participant coverage. Missing records remain missing.
