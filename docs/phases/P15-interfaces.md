# P15 — Build musician and engineering interfaces from verified state

**Status:** specification. **Prerequisites:** P14, with research and interface
prototypes permitted earlier. **Exit evidence:** usable, accessible interfaces
that present physical controls and diagnostic evidence honestly without imposing
unsafe real-time work on the audio process.

## Keep musical intent and physical state legible

The musician should be able to play and configure the setar without operating a
finite-element solver. An engineer must be able to identify why a sound changed
without guessing which of hundreds of hidden parameters did so. These are related
but distinct interfaces over one documented engine contract.

Read [musician and engineering UX](../ux/README.md),
[performance](../performance/README.md),
[plugin](../plugin/README.md), and the
[reference reporting contract](../engineering/artifacts-and-reproducibility.md).
The existing UX wireframe is a *proposed design*, not a screenshot of an
implemented instrument. The synthetic HTML renderer report is a working
engineering interface and must retain its evidence labels.

### Entry evidence and research routes

Work from the current [adapter source and limits](../plugin/08-prototype-juce-adapter.md),
not the older diagnostic-only architecture examples. The custom editor in
[SetarEditor.cpp](../../plugin/Source/SetarEditor.cpp) has two sliders, two
selectors and status text; its source is not a recorded running-host or
accessibility result. The [offline player](../../apps/setar_play.cpp) and
[analyzer](../../tools/analyze_render.py) are existing synthetic report paths.
Freeze their revision, configuration, observation gain and event fixture before
designing comparisons. Instrument, player-review and native-host evidence remain
unacquired.

| Research decision | Primary route and inspected/access limit | What must be acquired next |
| --- | --- | --- |
| Gesture and setup vocabulary | `R-SETAR-TABASSIAN`, [performer handout](https://www.atlasensemble.nl/assets/files/instruments/Setar/Setar%20by%20Kiya%20Tabassian.pdf), pages 1–2; `R-FRETTING-POORHAYDARI-2023`, [journal methodology](https://journal.iftawm.org/previous/2023-volume-11-no-2/poorhaydari/), paragraphs [7]–[8]. Available source material, not project participant evidence. | Contextual demonstrations, original wording and reviewed interpretations; no universal preset or contact coefficients follow from these texts. |
| Host and accessible controls | `R-SDK-JUCE-AUDIOPROCESSOR`, [official processor reference](https://docs.juce.com/master/classjuce_1_1AudioProcessor.html), plus the [AccessibilityHandler reference](https://docs.juce.com/master/classjuce_1_1AccessibilityHandler.html). Public pages are readable; `master` is not a version pin. | Inspect corresponding headers in the exact authorized JUCE checkout. The recorded adapter syntax check used 8.0.10; tagged GitHub header retrieval was unavailable during this documentation pass. |
| Meaningful quantities and uncertainty | `R-METROLOGY-JCGM-100`, [official record](https://www.bipm.org/en/doi/10.59161/jcgm100-2008e), sections 4, 5 and 7; access details in the [source atlas](../research/source-atlas.md). | Calibration and measurement equation for any real force, pressure, trajectory or latency claim; synthetic units alone are not calibration. |
| Participant interpretation and permission | `R-ETHICS-OHA`, [ethics guidance](https://oralhistory.org/oha-statement-on-ethics/), preparation/communication route retained in the [evidence ledger](../research/evidence-ledger.md). | Authorized consent and review process using the [elicitation protocol](../ethnomusicology/elicitation-and-terminology.md); no participant contact is part of these recipes' documentation work. |

Use the [finding workflow](../research/research-to-model-workflow.md) for
accepted, rejected, inconclusive or blocked findings. Each record identifies
source locations, uncertainty, current versus proposed implementation, and the
next experiment. When acquisition is blocked, continue synthetic event, schema
and layout fixtures while leaving practitioner and host gates open.

## P15-T01: construct a control inventory

Classify each candidate control as performance input, specimen/setup parameter,
physical calibration, presentation choice, or diagnostic. For every exposed
control, define stable identifier, internal unit, musical presentation unit,
legal range, default source, smoothing/state-change semantics, and whether
changes involve physical work. Do not place a raw Young's-modulus slider beside
a musical volume knob without explaining which is user performance and which
changes the modeled specimen.

Separate calibrated and exploratory modes. A musician may deliberately explore
unrealistic construction parameters; the interface must not silently write those
changes into the validated specimen profile. Preserve a reversible comparison
between a documented reference setup and an edited variant.

Define the status language for unsupported values and configurations. Avoid a
false impression that every input combination is mechanically valid. If a
chosen string tension, fret position, or contact setting violates solver
constraints, state the condition and the supported alternatives. A hidden clamp
is not an understandable musical control.

### Execution recipe

**Question and source route.** Can a musician change attack strength without
mistaking observation gain for physical excitation or changing the identity of
a calibrated setup? Start with `R-SETAR-TABASSIAN` pages 1–2 and the
[musician-view chapter](../ux/01-musician-view-and-interaction.md), “Controls
with physical meaning.” Search the bounded terms `setar nail stroke dynamics`
and `سه‌تار مضراب ناخن` only to resolve a named ambiguous action; a performer
description does not supply force calibration.

1. Extract the present dictionary from
   [SetarProcessor::create_layout](../../plugin/Source/SetarProcessor.cpp)
   and the identifiers in [SetarProcessor.h](../../plugin/Source/SetarProcessor.h):

   | Stable ID | Current range/default | State consequence |
   | --- | --- | --- |
   | `outputGainDb` | −48 to +24 dB / 0 | Post-engine observation gain, ramped over the host block; no mechanical feedback. |
   | `dynamicsScale` | 0.25–2 / 1 | Multiplies MIDI velocity, limited to 1–127 before the mapper selects stroke speed/release force. Dimensionless, not measured newtons. |
   | `tuning`, `fretMap` | Preset-list choices / index 0 | Nonautomatable selections; suspended re-preparation resets motion. |

2. For each proposed control, specify its exact event/configuration field,
   display conversion, admissible range source, target string, timing, and
   work/reset policy. Use [config.hpp](../../include/setar/instrument/config.hpp)
   and [gesture.hpp](../../include/setar/instrument/gesture.hpp): `position_m`
   starts at the nut, `bridge_position_m` is speaking length, and
   `total_length_m` includes afterlength. A “pluck position %” must name its
   denominator. Keep measured/inferred/synthetic provenance per quantity in
   a sidecar; the package-level `Provenance` enum cannot represent mixed evidence.
3. Freeze a synthetic gesture/configuration and compare two gain settings with
   unchanged mechanics, then two stroke settings at fixed gain. Retain requested
   values, canonical events, energy/force traces, defaults' source locations and
   rejected edits. Real controller-to-force claims require an authorized,
   calibrated force/trajectory experiment; otherwise retain dimensionless input.

An independent reader checks conversions and audible-versus-mechanical effects.
Current `acoustics.observation_invariance` and `score.midi_mapper` cover engine
observation separation and mapping, not UI operation. `ui.control_semantics`
is planned. Reject hidden physical clamps, renamed IDs, unsupported controls
that appear active, and edits that overwrite a reference profile.
`ENG-P15-T01` / `REQ-UXI-001` receive the dictionary and disposition record;
`docs/ux/control-dictionary.md` is a proposed destination, not an existing
registry. Hand accepted semantics to P15-T02 and the P14 state contract.


## P15-T02: design the musician interaction path

Map a playable gesture into observable engine events. Explain how a simple
note-on produces the chosen fingering/excitation approximation and how an
expressive control changes nail trajectory, damping, or pitch behavior. Keep
convenience mappings identifiable as such. Provide an intelligible way to see
four physical strings and their playing-course grouping without turning the
screen into a literal image of a generic six-string guitar.

Support tuning and fret systems as editable, versioned configurations. Names,
interval labels, Persian script, and performance terms must be reviewed in
context with practitioners. Do not hardcode one supposed universal radif,
dastgah, or microtonal grid into the numerical core. A UI may offer a documented
starter mapping while retaining a way to describe its actual provenance.

Show physical-state continuity: a ringing string remains ringing when an editor
panel changes. Provide clear musical stop/release functions with different
semantics from technical reset. Test multiple open/close cycles, resizes, focus
changes, plugin instance counts, and host transport transitions during sound.

### Execution recipe

**Question and sources.** Does the interaction express an intended attack,
stop or release on persistent strings, including the requested continuing
resonance? Read `R-SETAR-TABASSIAN` pages 1–2 and
`R-FRETTING-POORHAYDARI-2023` “Methodology and Initial Tests”; separate observed
practice from proposed scale systems. Use the [elicitation protocol](../ethnomusicology/elicitation-and-terminology.md)
and [score format](../performance/07-score-format.md). Target searches such as
`سه تار راست چپ` and `setar selective damping` stop at a contextual action
definition or an explicit missing demonstration, not a new universal label.

First write paired tasks: open versus stopped note, weak versus strong stroke,
release versus damping, and one string stopped while a companion rings. Freeze
tuning, fret map, initial ringing state, output gain, sample rate and controller.
For future authorized demonstrations retain nut-origin distances in metres,
stroke direction, calibrated time base, camera scale, synchronization error and
the player's intended contrast. Without those inputs, execute only synthetic
routing fixtures and retain the empirical question as blocked.

Trace each action through [MidiGestureMapper](../../src/instrument/performance.cpp)
and [GestureEvent](../../include/setar/instrument/gesture.hpp). Plain MIDI uses
12-TET requests and a nearest available fret within 50 cents; expose
`NoteRoute::cents_error` as a nominal mapping discrepancy, not measured played
intonation. Note-on prepares finger state and a nail stroke; note-off introduces
a damping touch and releases the stopping finger. `Silence` zeros mechanics as
containment. The stroke vector is `(sin(angle_rad), cos(angle_rad))` in vertical
and horizontal coordinates; map practitioner direction names only after review.
The current plugin ignores pitch bend, aftertouch and MPE; score-level course
sweeps do not make those plugin controls implemented.

Retain input logs, chosen routes, accepted events, sample offsets, contact/energy
and observed-output traces, glossary alternatives and an explicit disposition.
A separate reviewer compares the same absolute event timeline in the headless
engine and proposed UI harness, checking that editor navigation adds no gestures.
Current `score.fret_choice`, `score.course_sweep`, `score.midi_mapper` and
`performance.persistent_state` provide limited synthetic comparators;
`ui.control_semantics` remains planned. Stop if note-off silently resets unrelated
strings, terminology cannot be tied to an action, or a cosmetic bend substitutes
for an unsupported physical transition. `ENG-P15-T01` / `REQ-UXI-001` own
the interaction map; unresolved practitioner semantics return to P11
(`REQ-PRF-002`), with lifecycle cases handed to P14/P16.


## P15-T03: design calibrated observability

Specify bounded diagnostic streams for physical energy, solver residuals,
contact status, bridge force/velocity, model version, load failures, and timing
where appropriate. Each stream has a unit, sampling cadence, loss/overflow
policy, and trust boundary. The UI must not directly dereference mutable solver
vectors from another thread. Screens are projections of telemetry, not hidden
alternate numerical paths.

In the reference application, display the source revision, input manifest,
model assumption list, evidence level, and checks alongside each plot. Use
actual computed data with known transformations. Distinguish displacement,
velocity, force, acoustic pressure, and output PCM; a waveform glyph with no
unit or variable label can mislead even if the underlying numbers are correct.

When plots downsample data, describe the method and avoid hiding narrow spikes
with simple arbitrary point skipping. For energy and failure dashboards,
maximum-preserving envelopes or other justified aggregations may be necessary.
Store the unaggregated evidence packet for verification.

### Execution recipe

**Question and research route.** Can a diagnostic distinguish stored motion,
actuator work and solver failure without becoming another owner of physical
state? Read [engineering observability](../ux/02-engineering-observability.md),
[concurrency and telemetry](../realtime/04-concurrency-state-and-telemetry.md),
and `R-CONTACT-BILBAO-2014` via its [versioned preprint](https://arxiv.org/abs/1405.2589v1).
Inspect the actual potential/discrete-work derivation before transferring it;
the atlas does not claim every derivation reviewed. `R-METROLOGY-JCGM-100`
supplies the uncertainty-reporting method for later calibrated observations,
not measured prototype energy.

Inventory [InstrumentSnapshot, EnergyLedger and ProcessSummary](../../include/setar/instrument/instrument.hpp)
against [SetarProcessor::processBlock](../../plugin/Source/SetarProcessor.cpp).
The editor's `total_energy_j` sums string/body modal energy; it excludes
interaction potential and cannot certify the full work balance. The plugin
disables the ledger. Zero ledger fields when disabled are unavailable evidence,
not zero residual. The offline ledger uses
`residual = (stored - initial_stored) - external_work + dissipation - switching`,
with energy/work in joules; distinguish signed bridge force on the body in
newtons from acoustic output.

For each proposed stream define producer, sample clock/configuration generation,
unit, cadence, capacity, age, overflow and incident retention. Existing relaxed
atomic scalars do not make one coherent timestamped multi-field record.
`snapshot()` and `ledger()` remain single-audio-owner calls; the UI must consume
copies. The proposed `src/ui/telemetry_views.cpp` and a coherent publication
protocol do not yet exist. Freeze a synthetic contact/release fixture, inspect
ledger enabled/disabled states, deliberately stall the consumer, and compare
audio and physical traces with diagnostics absent/present. Retain original
block-end CSV from [setar_play](../../apps/setar_play.cpp), plus analysis rules;
it cannot reconstruct unrecorded sample-level spikes. Propose a bounded
higher-cadence capture when that question matters, preserving extrema and
persistent fault counters.

Independent reconstruction must reproduce displayed totals, signs and residuals
from the retained records. Current `engine.lossless_energy_ledger`,
`engine.lossy_energy_ledger`, `acoustics.observation_invariance` and
`realtime.allocation_hook` have limited engine scope; `ui.plot_schema` and
`realtime.telemetry_snapshot` are planned. Reject racing reads, a stale graph
presented as current, or a contained fault reported as valid physics.
`ENG-P15-T02` / `REQ-UXI-002` own the panel contract; its ownership prerequisite
is `REQ-RTS-003`. Hand missing publication/freshness mechanisms to P13 and
the diagnostic failure fixtures to P16.


## P15-T04: specify graphs, animations, and screenshots

Build a visual contract for common plots: axes and units, range, model/sample
identity, method, tolerance bands where justified, legend, and color-independent
identification. A mode-shape visualization must distinguish undeformed
geometry, deformation magnification, loading/support conditions, and whether
the shape is simulated or measured. Numerical animations must not invent
unobserved spatial detail.

Create real screenshots through a reproducible capture path from a known build
and fixture. Store capture configuration, dimensions, source revision, and
whether the target is a headless HTML report, standalone UI, or plugin editor.
Do not label a Figma-style concept drawing an implemented screenshot. Include
usable alt text and textual numerical summaries so the essential result remains
available without inspecting pixel colors.

Test interactive plots with boundary and failure data, not only elegant nominal
curves. A nearly silent signal, nonfinite run rejection, missing external asset,
contact solver fault, and huge parameter scale all need understandable UI states.

### Execution recipe

**Question and sources.** Does a graph expose the quantity actually computed,
including missing resolution and uncertainty, rather than imply unobserved
instrument motion? Use [visual QA](../ux/03-visual-testing-and-screenshots.md),
[graph contracts](../ux/05-live-engineering-graphs-and-screenshot-contract.md)
and `R-METROLOGY-JCGM-100` sections 4, 5 and 7. For a future measured mode-shape
comparison, `R-SETAR-MANSOUR-SCAVONE-2012` sections 3.1–3.4 and 4.1–4.2 provide
the apparatus/support route in the [source atlas](../research/source-atlas.md);
the published study does not provide this project's spatial dataset.

Start with one reproducible waveform/energy report. Inspect
[analyze_render.py](../../tools/analyze_render.py): `waveform_svg` renders
min/max envelopes of fixed-gain full-scale output, while `energy_svg` uses
block-end modal energy and a declared logarithmic floor. The older
[generate_report.py](../../tools/generate_report.py) instead starts with a
displacement observation. Preserve those different observables and gains;
neither label can be changed to calibrated microphone pressure. A string
animation needs recorded displacement coordinates and interpolation assumptions;
a body animation additionally needs spatial mode shapes absent from
`BodyMode`'s bridge participation and observation weights. Leave it unavailable
until an admissible dataset exists.

Freeze input hashes, revision, analysis/window settings, viewport, fonts, scale,
theme, locale and represented sample time. Compare plotted values and extrema
with an independently calculated table before capturing pixels. Include
zero/small signals, logarithmic-floor annotations, missing assets, stale data,
rejected nonfinite data and recorded solver faults. Derive any comparison
tolerance from numeric rounding or characterized rasterizer variation; an
unexplained threshold remains proposed. Preserve raw data, original captures,
semantic assertions and separate annotated/difference images.

[tools/capture_report.py](../../tools/capture_report.py) captures a generated
HTML report when a browser is available; it does not capture a JUCE host.
A native capture needs the actual built editor and host identity. The current
560 × 250 editor and the qualification validator's minimum screenshot
dimensions are different contracts: capture sufficient real host context,
never enlarge or pad pixels to manufacture a passing artifact.

An independent reviewer verifies source values, axes, evidence labels and capture
origin. Existing `integration.report` and `integration.setar_play` check report
pipelines; `ui.plot_schema` and `ui.screenshot_regression` remain planned.
Reject fabricated mode detail, hidden peaks, relabeled synthetic data and
baseline updates that conceal a defect. `ENG-P15-T02` / `REQ-UXI-002` receive
accepted/rejected/blocked visual records for P15-T06 and P16-T05.


## P15-T05: test access, language, and input devices

Support keyboard focus, screen-reader labels where applicable, high-contrast
readability, scalable text, and meaningful indicators that do not depend on
color alone. Check both left-to-right and right-to-left presentation where
Persian text is included. Technical parameter identifiers should remain stable
even when visible translations change.

Use practitioner review to test whether terms and controls represent intended
gestures. Consider expressive controllers and accessibility alternatives, not
only mouse dragging. Record where a requested gesture cannot yet be represented
by the engine rather than designing a decorative control that does nothing.

### Execution recipe

**Question and primary routes.** Can different input and access routes request
the same physical action and understand its result? Read
[accessibility/localization](../ux/04-accessibility-localization-and-feedback.md),
`R-SDK-JUCE-AUDIOPROCESSOR` and the associated official
[AccessibilityHandler API](https://docs.juce.com/master/classjuce_1_1AccessibilityHandler.html)
for roles, names, values and actions. Inspect the matching version before coding;
the readable `master` page does not prove 8.0.10 behavior. Use the existing
[WCAG 2.2 route](https://www.w3.org/TR/WCAG22/) for criteria 1.4.3, 1.4.11,
2.1.1, 2.4.7 and 4.1.2 as design/test references, not automatic native-plugin
conformance. `R-SETAR-TABASSIAN` and `R-ETHICS-OHA` guide contextual language
and authorized feedback; no translated phrase is practitioner-approved here.

Build a task-by-route table for selection, numeric edit/cancel, stronger attack,
release, panic and finding a fault. Use mouse, keyboard-only, actual supported
assistive technology, and each declared controller. Freeze build, host, OS,
device/driver, mappings, font, scale and locale. Compare canonical events in
SI units/sample offsets across routes while holding the initial physical state
and observation gain fixed. Controller pressure stays dimensionless unless
a calibration gives its force relationship; device latency needs a recorded
clock/loopback method and uncertainty, separate from mapper lead time.

Inspect [SetarEditor.cpp](../../plugin/Source/SetarEditor.cpp) and
[plugin/CMakeLists.txt](../../plugin/CMakeLists.txt): labels and
`EDITOR_WANTS_KEYBOARD_FOCUS FALSE` are source facts, not evidence of keyboard
or screen-reader success in a DAW. Exercise focus restoration after close/reopen,
host-intercepted keys, enlarged text, mixed Persian/Latin strings, minus signs,
units and decimal entry. Keep `outputGainDb` and other stored IDs unchanged
under translation. Check text/table alternatives to plots and that repeated
telemetry announcements do not prevent recovery. Unsupported MPE/pitch-bend
routes must remain explicitly unsupported.

Retain accessible-tree/value/action records where available, actual captures,
controller calibration records, input/event comparisons and scoped human
feedback with permission-safe identifiers. A separate access reviewer checks
essential task completion; no available reviewer or host means blocked coverage,
while synthetic semantic fixtures can continue. `score.midi_mapper` is an
existing mapping test, not an accessibility test. Planned
`ui.accessibility_keyboard`, `ui.control_semantics` and `ui.screenshot_regression`
serve `ENG-P15-T01` / `REQ-UXI-001` and `ENG-P15-T02` / `REQ-UXI-002`.
Reject focus traps, ambiguous units, silently truncated input and inaccessible
recovery; hand defects and untested combinations to P15-T06/P16.


## P15-T06: qualify interaction and presentation

Perform scripted UI reviews and usability sessions against the same build.
Record task, participant role where consent permits, expected outcome, actual
observation, and revision. Include low-vision/keyboard tests, repeated editor
lifecycles, invalid parameter edits, error recovery, and many-instance behavior.
Capture actual screenshots with labels identifying their evidence status.

Acceptance requires that controls produce documented engine events, diagnostics
do not compromise callback bounds, screenshots are traceable, and users can
recover from invalid operations without corrupting calibrated data. Beautiful
screenshots alone do not pass this phase.

> Implement one musician control panel and one engineering diagnostic panel over
> the same stable engine API. For every control, list the event, unit, physical
> meaning, range, and state-change policy. For every graph, name the underlying
> variable, provenance, rendering transform, and evidence level. Test editor
> lifecycles, failure states, accessibility, and callback isolation. Provide
> actual captured evidence and keep conceptual wireframes explicitly separate.

### Execution recipe

**Question and research route.** Can a user accomplish a musical intention,
recognize the active physical setup and recover from failure on a frozen
candidate? Start with “Evaluation tasks” in the
[musician-view chapter](../ux/01-musician-view-and-interaction.md),
`R-SETAR-TABASSIAN` pages 1–2, and `R-ETHICS-OHA`'s preparation/communication
guidance through the [elicitation protocol](../ethnomusicology/elicitation-and-terminology.md).
Search only to resolve a specific task/term or missing access method. These
sources help formulate neutral tasks; they supply neither project usability
scores nor evidence of acoustic fidelity.

Prepare scripts before a session: make a stronger attack at unchanged observation
gain; identify which physical strings should continue ringing; distinguish a
selection requiring re-preparation from a panel change; cancel an invalid edit;
and discover/recover from stale diagnostics or a reported fault. Record the
expected canonical event/state consequence, not merely the expected button.
Separate first-use discovery from learned execution. Freeze source/binary,
configuration, sample rate, block pattern, controller and audio chain; otherwise
latency, loudness, training and host focus can confound a UI comparison.

First execute deterministic developer fixtures over the proposed harness, then
authorized practitioner/access reviews when available. Declare categorical
failure criteria before collecting results: wrong string/gesture, unintended
reset, loss of the prior accepted setup, inability to reach recovery, or concealed
fault. Any time/completion-rate target needs a proposed task-specific rationale;
do not invent a universal usability percentage. Preserve interventions and
abandoned trials. An observation of “too short” requires a discriminating replay
of excitation, damping and output mapping before changing physical loss.

Retain task version, participant role/access history where permitted, consent
linkage, actual observations, event/physical traces, genuine screenshots,
assistance given, and accepted/rejected/inconclusive/blocked decisions. Independent
review must connect each apparent success to its event/state assertion and
recheck revised tasks on the same candidate. No participant or built host means
those gates remain blocked, not replaced by a developer opinion.

The crosswalk assigns `ENG-P15-T02` / `REQ-UXI-002`; semantic results consume
P15-T01/T02's `REQ-UXI-001` contract. Present `integration.setar_play` and
`realtime.allocation_hook` are limited pipeline/engine checks, not usability
coverage; `ui.screenshot_regression` is planned. Hand traceable UI evidence and
open failures to P16, and separately preregister acoustic/playability claims in
P17. A usable synthetic prototype has not thereby become a validated setar.

### Phase handoff checklist

- Keep the original specification status and distinguish the custom editor source,
  captured HTML reports, proposed wireframe, and still-unrun native host/access
  cases.
- Supply the control/event dictionary, calibration/provenance sidecars, diagnostic
  definitions and generation/ownership contract; mark missing physical mechanisms
  and reset transitions explicitly.
- Supply reproducible fixtures, raw numeric records, semantic/access assertions,
  authentic captures and scoped independent-review dispositions. Retain negative
  examples and permission/access restrictions.
- Pass a bounded candidate and unresolved tasks to P16. Practitioner, specimen,
  continuous-retuning and full telemetry-publication gates stay open until their
  own evidence exists; adding these recipes closes none of them.
