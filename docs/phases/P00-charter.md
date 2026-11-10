# P00 — Define the instrument, claims, and scientific contract

**Status:** specification. **Prerequisites:** none. **Exit evidence:** reviewed
scope, requirement register, claim vocabulary, and risk ownership. This phase has
no delivery date and is revisited when the target changes.

## Why this phase precedes equations

An instrument name does not define a complete mathematical problem. “Setar”
includes different makers, materials, strings, fret arrangements, tunings, player
techniques, and recording conditions. A goal of “perfect sound” cannot tell an
engineer whether an amplitude discrepancy comes from a model error, a microphone
position, a tuning difference, or a different nail gesture. Scope must identify
which of those variables are controlled, varied, or outside the initial claim.

Read [orientation](../START_HERE.md), the [quality contract](../governance/quality-contract.md),
and the [ethnomusicology index](../ethnomusicology/README.md). Treat the user goal
of commercial-class physical-modeling quality as a design ambition. Translate
that ambition into specific, falsifiable requirements without inventing numerical
tolerances that have no perceptual or measurement basis.

### Working entry and source routing

Start from the current [prototype capability register](../engineering/11-playable-prototype.md),
the [task crosswalk](../../roadmap/phase-task-crosswalk.json), and the
[source atlas](../research/source-atlas.md). The recipes below specify future
decisions and evidence; they do not report new source inspection, measurement,
practitioner review, or phase completion. The present synthetic preset is a
usable verification fixture while specimen-dependent claims remain blocked.
Keep a source-location column alongside every charter assertion: instrument
description routes to the setar/practitioner sources, uncertainty to metrology,
state and work to numerical methods, and host behavior to the pinned SDK.
These routes answer different questions and cannot substitute for one another.
Use the [research-to-model workflow](../research/research-to-model-workflow.md)
for the common finding format; P00 adds the decisions that determine what may
later enter an asset, test, or public claim.

## P00-T01: define the reference domains

Separate three domains in the charter. The physical domain lists specimens,
strings, environmental conditions, support conditions, and player contact. The
musical domain lists tunings, gestures, dynamics, tempo-dependent behavior,
ornaments, and allowed simultaneous sounds. The software domain lists host
formats, platforms, sample rates, block sizes, offline rendering, state recall,
and interface accessibility.

For each domain, write an initial inclusion list, exclusion list, and uncertainty
list. An inclusion means the project intends to collect enough evidence to make
a claim; it does not mean the implementation already works. An exclusion means
the release must not advertise the omitted behavior. An uncertainty means an
experiment or decision is needed before assigning either status.

Do not classify instrument structure from a stock photograph. Record the source
of construction facts and the person who will verify them against the selected
specimen. If no specimen is yet available, state that the initial executable work
is synthetic verification. Avoid inventing a maker/model identity to make the
charter appear complete.

### Execution recipe

**Question and research route.** What exact object, setup and playing task could
the first fidelity claim describe, and what changes would invalidate it? Work
under `ENG-P00-T01` and `REQ-GOV-001`. Read
[reference-instrument selection](../measurement/reference-instrument.md) and
[elicitation task design](../ethnomusicology/elicitation-and-terminology.md).
Use `R-SETAR-TABASSIAN`, [pages 1–2](https://www.atlasensemble.nl/assets/files/instruments/Setar/Setar%20by%20Kiya%20Tabassian.pdf),
to identify candidate course, tuning and stroke questions;
`R-SETAR-MANSOUR-SCAVONE-2012`, [§3.1–§3.4 and §4.1–§4.2](https://caml.music.mcgill.ca/lib/exe/fetch.php?media=publications:nantes2012_mansour.pdf),
to identify support/sensor confounders. These sources guide questions, not this
specimen's constants. Search `"Iranian setar" fingernail tuning` and
`"سه‌تار" پرده بندی کوک` only to resolve named gaps; record inspected locations,
access failures and conflicting descriptions.

1. Choose explicitly between one specimen, a specimen family, and a defined
   player's task domain. Draft one row per intended claim with specimen/setup
   IDs, physical string index and course, intended tuning, measured pitch if
   available, gesture/start state, support, environment, observation and exclusion.
   Keep unavailable values null in the
   [specimen template](../../data/specimen-template.json); an author photograph
   or historical tuning description cannot fill an unmeasured field.
2. For each inclusion, name the observation that would establish it. Route
   speaking length/action to calibrated length measurements in metres; string
   mass to a calibrated balance and measured length; gesture timing to
   synchronized motion/audio; bridge response to force in N and velocity in m/s;
   acoustic output to calibrated pressure in Pa with microphone position in
   metres. Specify temperature/humidity records and repeated setups. A changed
   support, sensor, fret or tuning creates a distinguishable configuration.
3. Require the specimen-to-string transform before interpreting positions:
   the [geometry frame](../geometry/01-specimen-and-coordinate-contract.md) and
   the nut-origin local frame in
   [config.hpp](../../include/setar/instrument/config.hpp) are not interchangeable.
   `StringConfig.bridge_position_m` is the speaking length;
   `total_length_m` extends to the tail. Record both contact landmarks and their
   uncertainty. Keep string indices separate from performance course groupings.
4. Add a software row naming the engine/binary revision, platform, rate in Hz,
   block size in frames, event/capacity envelope and observation mode. Keep a
   headless numerical claim distinct from a named host/version claim; unavailable
   host or hardware evidence remains an explicit exclusion or open question.

**Decision and handoff.** Retain the domain matrix, permission references,
source excerpts as short paraphrases with exact locations, raw-record requests,
derived configuration map, disagreements and proposed observation plots. The
work-order destinations `data/specimens/<specimen-id>/charter.json` and
`docs/governance/claims.md` are **proposed**, not existing specimen assets. A
second reviewer must reconstruct the intended comparison and reject a family-wide
claim supported by one object or a clamped-body result advertised as player-held
behavior. Mark each row accepted for scope, rejected, inconclusive, or blocked;
missing access blocks its empirical claim but permits synthetic fixtures.
`governance.claim_trace` is planned, not a present CTest. Existing
`preset.fret_positions` and `preset.tension_inverse` exercise synthetic preset
calculations only. Hand the acquisition rows to `ENG-P01-T02` and gesture
ambiguities to P11/`REQ-PRF-002`; preserve
[setar_preset.cpp](../../src/instrument/setar_preset.cpp) as synthetic until a
separate, reviewed parameter promotion supports a new configuration.

## P00-T02: establish the claim ladder

Adopt the quality contract's Q0–Q4 labels. Define how a result advances through
proposal, executable reference, verified component, independently validated
component, and integrated qualification. Require a separate statement of domain
at each transition. A model that passes a lossless single-mode test has not
demonstrated a lossy four-string instrument.

Use negative claims to expose ambiguity. Examples include “this plot does not
validate fingernail excitation,” “this test does not run inside a DAW,” and “these
frequencies were transcribed from a paper, not remeasured.” Such statements
should clarify a specific boundary rather than fill every paragraph with generic
disclaimers. Place the limitation beside the result that could otherwise mislead.

Acceptance for this task is an example evidence packet at each proposed level,
including a deliberately insufficient packet and the reason it fails. Reviewers
should agree on classification without knowing who produced the work.

### Execution recipe

**Question and sources.** Which inference does a particular result support, and
what missing observation prevents promotion? Use `ENG-P00-T02` to define the review
policy for `REQ-GOV-001`. Read the
[evidence contract](../validation/00-evidence-contract.md) and
[test taxonomy](../validation/01-test-taxonomy-error-budgets.md). For the meaning
of an experimental interval, inspect `R-METROLOGY-JCGM-100`,
[§4, §5 and §7](https://www.bipm.org/documents/20126/2071204/JCGM_100_2008_E.pdf).
Use search terms `JCGM 100 correlated input quantities uncertainty reporting`
when a specific reporting convention remains unclear. This is a method source,
not evidence that any project measurement has occurred.

Build a packet example for each row below. Label absent evidence as a proposed
packet; do not manufacture measurements or signatures to complete the examples.

| Claimed level | Minimum decisive addition | Counterexample that must stop promotion |
| --- | --- | --- |
| Q0 | Equation/interface or experiment, domain, source locations and falsifier | A drawing presented as an executing solver |
| Q1 | Identifiable executable, synthetic input manifest, units, run outputs and independent expected result | A render without its configuration or comparator |
| Q2 | Independent derivation/reference, refinement/error plots, invariants and rejected-input results across the stated domain | One stable waveform used to certify all contact states |
| Q3 | Calibrated raw observations, preprocessing lineage, uncertainty and predictions on data unavailable during fitting | Reusing fitted partials as independent validation, or claiming radiation from frequencies alone |
| Q4 | Integrated physical, gesture, deadline, host/platform and practitioner evidence for a frozen release domain | A headless pass substituted for a host run or playing evaluation |

Select a real current test for the Q1/Q2 illustration, such as
`solver.modal_exact_free_response` in
[numerics_tests.cpp](../../tests/instrument/numerics_tests.cpp), and read its
assertions: its oscillator comparison says nothing about nail coefficients or
measured instrument identity. Record source commit, exact test name, synthetic
initial state, rate and output identity; link an existing run only if inspected.
Keep numerical displacement error in metres or its declared normalization
separate from calibration uncertainty and model discrepancy. For Q3 planning,
freeze development/holdout partitions by session/setup before fitting; correlated
windows from one take do not create independent specimens.

Have reviewers classify packets sufficient for a stated scope and deliberately
insufficient packets without author identity. Resolve disagreement by identifying the missing
observable, calibration, comparator or source passage. Retain raw/derived manifests,
claim text, required plots, level requested, level supported, reviewer reasoning,
and accepted/rejected/inconclusive/blocked disposition. Missing calibration or a
reused holdout blocks that promotion; it does not erase narrower numerical
evidence. `governance.claim_trace` remains a planned acceptance name, and the
[coverage registry](../../roadmap/executable-test-coverage.json) is a map of
partial checks, not permission for blanket Q2–Q4 labels. Hand the reviewed rules
to P01's source ledger, P02's oracle work and P17's claim decisions. The proposed
`docs/governance/claim-gates.md` must preserve these examples and rejected claims
when an authorized implementation task creates it.

## P00-T03: specify the physical-model constraint

The synthesis source is physical state, excitation, coupling, and observation.
Measured model parameters are permitted and usually essential. Reference
recordings are permitted for analysis and independent validation subject to
permissions. Replaying recorded instrument notes is not the engine. If an
impulse response, fitted filter, or reduced model is introduced, identify what
physical quantity it represents, how it was obtained, its validity range, and
whether it participates in feedback or merely observation.

An impulse response used only as a post-filter does not reproduce the reciprocal
string–body interaction. A measured modal model can be physically meaningful
without solving a spatial mesh at every audio sample. Conversely, a huge
finite-element mesh can be inaccurate if its materials and boundary conditions
are wrong. This project evaluates derivation and evidence, not the apparent
complexity of the algorithm.

### Execution recipe

**Question and sources.** Can every audible contribution be traced to declared
physical state and excitation, and does every claimed interaction exchange the
required work? Use `ENG-P00-T01`, `ENG-P00-T02` and `REQ-GOV-001`, with downstream
`REQ-CPL-001` and `REQ-ACO-002`. Read
[physical loading versus observation](../decisions/ADR-0006-physical-loading-and-observation.md)
and the [energy-port contract](../mathematics/03-energy-and-passive-ports.md).
`R-NUM-BILBAO-SMITH-2005`, [§II–§III and appendix](https://www.pure.ed.ac.uk/ws/portalfiles/portal/11222313/acustica_final.pdf),
provides an energy/discretization reading route, not proof for this solver.
`R-ACO-DELEON-2008`, [§2.1–§2.2 and §3.1.2](https://caml.music.mcgill.ca/lib/exe/fetch.php?media=publications:ma_deleon_2008.pdf),
is a route for acoustic boundary conventions; the atlas records only partial
inspection. Read the relevant sections before adopting a method. Search
`multiport mobility passive realization` or `structure acoustic observation
loading` for a specific unresolved transfer definition.

1. Trace a proposed note from `GestureEvent` through
   [Instrument::process](../../src/instrument/instrument.cpp) to observation.
   Inventory persistent states, prescribed motions, stored potentials, losses,
   loading ports and output-only operations. Attach equation, units, sign/frame,
   time level, source/finding ID and validity band to each edge. Force/velocity
   pairs use N and m/s; acoustic pressure/volume flow use Pa and m³/s. Their
   products represent power; a microphone transfer is a different quantity.
2. Classify each imported asset before accepting it: measured physical
   parameters, reduced mechanical/acoustic model, observation filter, or reference
   recording. For a loading impulse response require its complex transfer between
   power-conjugate ports, normalization, causal/passive realization and boundary conditions.
   For a pressure observation require calibration, location, phase and causal
   stability without imposing a driving-point passivity test on a nonconjugate
   transfer. Recorded note playback fails the engine constraint regardless of
   sound quality; authorized reference recordings remain analysis evidence.
3. Audit the current representation explicitly. In
   [config.hpp](../../include/setar/instrument/config.hpp), `BodyMode` needs
   modal mass and signed `bridge_participation` as well as frequency/damping.
   `radiation_weight` has units Pa·s²/m and maps acceleration to output; it does
   not implement acoustic feedback. Frequency lists cannot fill those missing
   quantities. The coarse `Provenance` label also cannot replace per-field
   uncertainty and source lineage in a versioned sidecar.

Retain the state/port inventory, asset and raw-source hashes, conversions,
derivation, response/work plots and the decision for every proposed contribution.
An independent reviewer checks limiting cases: remove shared participation and
unstruck strings must receive no transferred motion; change output weights and
mechanical state must remain unchanged for the current one-way observation.
Existing `coupling.uncoupled_limit` and `acoustics.observation_invariance` in
[engine_tests.cpp](../../tests/instrument/engine_tests.cpp) exercise those
synthetic cases, not measured radiation. Reject unexplained sample mixing,
untraceable coefficients or a post-filter presented as reciprocal loading; mark
missing FRFs/calibration blocked rather than fitting around them. Handoff accepted
definitions and unresolved alternatives to P08–P10 and P12; retain the planned
`governance.claim_trace` gate for the corresponding claim.

## P00-T04: define interface and responsibility boundaries

Assign ownership for physical model specification, numerical implementation,
measurement, real-time integration, plugin behavior, interfaces, and validation.
The same person may fill several roles, but each change must identify the
perspectives that reviewed it. Keep model input/outputs separate from GUI units
and user-facing macro controls. A slider value is not a physical parameter until
its mapping is defined.

Write an architecture decision that the core is C++20 and independent of JUCE.
The offline reference renderer and plugin must share defined physical interfaces,
not hidden duplicate implementations. Data import belongs away from the audio
callback. Tooling may use Python for analysis and diagrams; those scripts must
not silently become the deployed sound generator.

### Execution recipe

**Question and route.** Who can change each physical meaning or mutable state,
and who independently checks that change before another subsystem consumes it?
Use `ENG-P00-T02`, `ENG-P02-T01`, `REQ-GOV-001`, `REQ-LAB-001` and `REQ-LAB-002`.
Read [ADR-0002](../decisions/ADR-0002-cpp-core-and-juce-boundary.md),
[state ownership](../architecture/02-physical-ports-and-state-ownership.md) and
[asset boundaries](../architecture/03-evidence-assets-and-scheduling-boundaries.md),
checking their proposed architecture against the current API. For host semantics,
use `R-SDK-JUCE-AUDIOPROCESSOR`, the
[official lifecycle/state methods](https://docs.juce.com/master/classjuce_1_1AudioProcessor.html),
then inspect the matching pinned checkout before implementation; `master` is not
a version guarantee. Search `AudioProcessor prepareToPlay processBlock
getStateInformation` for the specific unresolved boundary, not generic advice.

Create the following ownership rows with named assignee, backup, reviewer and
unresolved decision; a vacant row is visibly unassigned, never presumed covered.

| Boundary | Responsible perspective and current anchor | Required review question |
| --- | --- | --- |
| Physical meaning | Model owner; `StringConfig`, `BodyMode`, `ContactLaw` in [config.hpp](../../include/setar/instrument/config.hpp) | Are SI units, axes, energy and validity explicit? |
| Evidence to coefficients | Measurement/asset owner; [specimen template](../../data/specimen-template.json), proposed import/sidecar pipeline | Can calibration, covariance, permissions and every conversion be reconstructed? |
| Coupled live state | Numerical owner; `Instrument` in [instrument.hpp](../../include/setar/instrument/instrument.hpp) | Is force/work assembled consistently, with bounded and observable failure? |
| Gesture meaning and timing | Performance owner plus practitioner reviewer; [gesture.hpp](../../include/setar/instrument/gesture.hpp), [performance.cpp](../../src/instrument/performance.cpp) | Do physical string, position, sample offset and sequence retain their meaning? |
| Host and presentation | Adapter/UI owner; [SetarProcessor.cpp](../../plugin/Source/SetarProcessor.cpp) | Does conversion preserve the same engine contract and separate display units? |
| Acceptance | Independent numerical/acoustic/software reviewers as applicable | Does the packet support this claim without borrowing another gate's authority? |

Walk one hypothetical calibrated length and one equal-time gesture pair through
the rows. Record the raw unit/frame, conversion to metres, uncertainty, config
identity, event ordering and observer output; contrast against a deliberately
millimetre-labelled-as-metre input and reversed event sequence. No new specimen
value is needed for this synthetic contract review. Capture allowed thread,
ownership/lifetime, failure result and downstream invalidation for every crossing.
The current `prepare()` may allocate/throw off-thread; `process()`, `reset()`,
`snapshot()` and `ledger()` require a single owner. A snapshot method is not
permission for concurrent GUI reads. General immutable asset publication remains
proposed; retain its destination under `REQ-RTS-003`.

Keep the reviewed matrix, typed example traces, source/version references and
rejected ambiguity list as the handoff. Existing `engine.event_ordering`,
`realtime.block_partition` and `realtime.allocation_hook` check particular
synthetic API paths; they do not establish host qualification. Planned
`verification.oracle_independence`, `ci.matrix_coverage` and
`plugin.core_headless_link` are not substitute run records. Accept only boundaries
whose producer and consumer reviewers agree on physical/time/lifetime meaning;
reject dual writers, hidden duplicate physics or unspecified unit conversion.
Route blocked assignments to the charter owner, lab checks to `ENG-P02-T01`,
publication work to `ENG-P13-T02`, and adapter evidence to `ENG-P14-T01`.

## P00-T05: define measurable ambitions without premature targets

For pitch, specify whether error is measured in cents relative to an analytic
string, an experimental partial, or a performer-selected tuning. For decay,
define the window, partial tracking, noise threshold, and environmental controls.
For responsiveness, define event-to-output behavior separately from audio-driver
round-trip latency. For CPU budget, identify a hardware class and worst-case
state before selecting a numerical limit. For realism, specify the task a
listener performs and the population to whom the conclusion applies.

If a target cannot yet be justified, mark it proposed and give the experiment
that will set it. Do not create a table of impressive percentages that no one
can measure. A future revision must preserve the old target and explain why
the new one better serves the claim, rather than overwrite a failed criterion.

### Execution recipe

**Question and sources.** Is the proposed acceptance band capable of resolving
the physical or audible difference the claim concerns? Work under `ENG-P00-T02`
and `REQ-GOV-001`; downstream consumers are `REQ-RTS-001`, `REQ-RTS-002`,
`REQ-FID-001` and `REQ-FID-002`. Read
[error-budget policy](../validation/01-test-taxonomy-error-budgets.md) and
[measurement uncertainty](../measurement/uncertainty-and-calibration.md).
Use `R-METROLOGY-JCGM-100`,
[§4–§5 and §7](https://www.bipm.org/documents/20126/2071204/JCGM_100_2008_E.pdf),
for measurement functions/correlation/reporting. For listening-question design
read the appropriate pinned edition of `R-EVAL-ITU-BS1116-3`
([small impairments](https://www.itu.int/rec/R-REC-BS.1116-3-201502-I/en)) or
`R-EVAL-ITU-BS1534-3`
([intermediate quality](https://www.itu.int/rec/R-REC-BS.1534-3-201510-I/en));
the atlas records incomplete access to the latter's PDF. Search `pitch
discrimination plucked string attack duration` or `partial decay estimator
uncertainty` only after specifying the stimulus and observable. No standard
supplies a universal setar accuracy or audibility threshold.

For one pilot observable, write a budget with the following separate rows before
examining candidate performance. Do not add the rows in quadrature without a
justified joint uncertainty model.

| Budget | Comparator and controlled experiment | Retained result and stop condition |
| --- | --- | --- |
| Numerical | Analytic or independently converged solution with fixed physical inputs; refine step/mode count without refitting | Error versus resolution and rounding/extraction floor; stop a physical comparison that numerical error cannot resolve |
| Measurement | Calibrated raw channels, repeated setup/session and explicit feature estimator | Input uncertainties, covariance, calibration/drift and window sensitivity; mark intervals unresolved when noise/support variation dominates |
| Model discrepancy | Held-out configuration with the same declared observation operator | Residuals by gesture, frequency, amplitude and position; reject compensation by arbitrary gain, damping or EQ |
| Audibility/playability | Preregistered masked task, level/playback controls and relevant listener/player population | Effect/interval and exclusions; an inconclusive or underpowered result cannot establish equivalence |

Define pitch as `1200 log2(f_candidate/f_reference)` cents, stating whether each
frequency is undamped, a tracked partial or a performer's target. Define amplitude
decay as `A(t)=A0 exp(-sigma*t)` with `sigma` in s⁻¹, fitted interval and noise
floor; do not confuse it with modal damping ratio. Retain waveform/partial data,
window choices and estimator checks on synthetic signals with known answers.
In the current engine, `StringLossLaw` coefficients have units s⁻¹, m²/s and s;
their uncertainty belongs in the parameter record, not in an unexplained scalar
"accuracy" score.

For responsiveness, define the event timestamp and measurable response marker
separately from driver round-trip latency. For execution, derive the block
deadline `B/f_s` seconds, then register any proposed headroom and miss policy
against named hardware, build, capacities and worst-case gestures. Existing
`engine.open_string_pitch` in [engine_tests.cpp](../../tests/instrument/engine_tests.cpp)
has synthetic truncation/discrete-model checks; `realtime.max_work` is not a
representative-hardware deadline distribution. Neither sets a perceptual budget.

Retain versioned thresholds, provenance of each rationale, raw/derived manifests,
error curves, uncertainty tables and competing explanations. An independent
reviewer accepts a budget only if its comparator and experimental resolution
support the claimed decision; otherwise record rejected, inconclusive or blocked
with the smallest resolving experiment. Keep `governance.claim_trace`,
`realtime.reference_parity` and `fidelity.heldout_acoustic` as planned gates where
no complete execution exists. Hand numerical budgets to P02/P13, measurement
pilots to P01/P12, and audibility/playability questions to P17. Preserve failed
criteria and revisions rather than widening a band retrospectively.

## Outputs and tests

Produce a charter, claim examples, requirement identifiers, ownership map, and
initial risk register. Review the charter against three counterexamples: a
generic guitar-like model with a setar label, a physically sophisticated model
with no measurement validation, and a sample player that sounds excellent but
violates the synthesis constraint. The charter must classify all three correctly.

The documentation auditor checks links and phase dependencies, not the
substantive quality of this work. A human review must establish that the scope
is bounded and internally coherent. Record unresolved choices explicitly.

## Developer prompt

> Read P00 and the quality contract. Draft a scope amendment for the specific
> requested capability. Identify the physical, musical, and software domains it
> changes; list evidence already present and evidence still missing. Propose
> falsifiable requirements without claiming unmeasured accuracy. Do not implement
> DSP or alter licensing, CI permissions, or release claims. Return the amended
> charter, a requirement-to-evidence table, and the smallest next experiment.

## Exit gate and handoff

Advance when the owner/reviewers can explain the intended instrument, the
permitted model/data practices, the claim ladder, and what the first release will
not claim. Handoff the unresolved measurements to P01 and the executable
verification requirements to P02. Keep P00 open to evidence-driven refinement.

Before that handoff, check that the packet contains:

- A specimen/domain row for every proposed claim, with unknowns and exclusions
  visible and no synthetic field promoted by association with one measurement.
- Source IDs and exact inspected locations, access limits, raw/derived lineage,
  and permission references for the observations needed next.
- A state/port/observation classification and assigned producer, consumer and
  independent reviewer for each consequential interface.
- Separate numerical, measurement, model-discrepancy and audibility decisions,
  with proposed thresholds distinguished from justified ones.
- Accepted, rejected, inconclusive and blocked findings; exact current test scope;
  planned acceptance names; and the next work order for every open dependency.

Passing this document review authorizes no new specimen-fidelity statement by
itself. Independent synthetic verification may continue through P02 while access,
calibration and practitioner evidence are acquired through their own gates.
