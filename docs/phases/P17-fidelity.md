# P17 — Evaluate physical, acoustic, perceptual, and expressive fidelity

**Status:** specification. **Prerequisites:** P16 plus independent specimen and
performer evidence. **Exit evidence:** a predeclared, domain-limited fidelity
assessment with uncertainty, ablations, and practitioner interpretation.

## Define what “high fidelity” means before listening

The goal of competitive physical-modeling quality is important, but it is not
a numerical unit and cannot be certified by matching a named commercial
product's brand. This phase converts the aspiration into falsifiable questions:
does the model predict independent observations, behave naturally under new
gestures, remain stable across expression, and convince listeners under
controlled comparison? No one test answers all four.

Read [validation](../validation/README.md),
[measurement](../measurement/README.md),
[performance](../performance/README.md), and
[acoustic radiation](../acoustics/README.md). The target is an identified setar
class/specimen and declared performance domain, not an undefined “perfect
setar” across every builder, player, environment, and musical practice.

### Entry conditions and research map

Planning, synthetic analysis checks, and protocol review can proceed now. An
empirical claim still requires permitted independent measurements, calibrated
observation, a frozen candidate, and the applicable P16 evidence. The work-order
dependencies are `ENG-P16-T01` and `ENG-P12-T02` for `ENG-P17-T01`, then
`ENG-P17-T01` and `ENG-P16-T02` for `ENG-P17-T02`. Record each dependency's actual
evidence or blocker; a written protocol does not satisfy it. This chapter neither
contacts participants nor records a completed listening or playing study.

| Decision | Start with this existing research route | Evidence boundary |
| --- | --- | --- |
| Meaningful acoustic difference | `R-METROLOGY-JCGM-100`, JCGM 100:2008 §§4–5 and §7; [acoustic metrics](../validation/03-acoustic-metrics.md) | An uncertainty method supplies no setar coefficient or universal perceptual threshold. |
| Support and sensing confounds | `R-SETAR-MANSOUR-SCAVONE-2012`, §§3.1–3.4 and §§4.1–4.2; [acquisition campaign](../measurement/13-acquisition-campaign-execution.md) | Published apparatus comparisons motivate a project loading/support comparison, not a correction factor. |
| Listening-method choice | `R-EVAL-ITU-BS1116-3`, `R-EVAL-ITU-BS1534-3`; [listening experiments](../validation/06-listening-experiments.md) | Pin the exact recommendation and record actual access before claiming method conformance. |
| Musical action and interpretation | `R-SETAR-TABASSIAN`, handout pp. 1–2; [elicitation protocol](../ethnomusicology/elicitation-and-terminology.md) | A practitioner-authored description motivates questions; it does not replace the future participants' demonstrations. |

Use the [source atlas](../research/source-atlas.md) for primary links and access
limits, and the [research workflow](../research/research-to-model-workflow.md)
for finding records. The four `fidelity.*` acceptance IDs in this phase are
**planned**, with no current executable mapping in the
[coverage registry](../../roadmap/executable-test-coverage.json). Current scores
under [data/scores](../../data/scores/README.md) and the
[offline player](../engineering/12-offline-player-and-analysis.md) contain
synthetic inputs and outputs. They can exercise a pipeline, not replace the
specimen, listener, or practitioner evidence.

## P17-T01: freeze claims and test partitions

Write the specific claims before opening the final held-out test set. Examples
include partial-frequency prediction on untrained finger positions, bridge
mobility under changed string tension, measured attack evolution under a
controlled stroke, or musician control naturalness in a defined gesture set.
Each claim needs observations, nuisance controls, comparison method, and a
decision rule that reflects uncertainty.

Ensure reference measurements and model outputs share the intended condition.
A changing nail, instrument humidity, microphone placement, or player support
may affect the result. Record those conditions rather than explaining any
disagreement as “human variability.” If the two conditions cannot be matched,
narrow the claim or model the condition difference explicitly.

Keep final evaluation data independent of architecture and parameter choices.
If evaluators inspect a held-out result and then tune the model repeatedly,
it has become development data. Record the exposure and establish a new
evaluation partition before making a fresh generalization claim.

### Execution recipe

**Question and route.** Which specified observations should this frozen candidate
predict without further tuning, and which separate sound-identity or playing
claims would those observations support? Under the
[crosswalk](../../roadmap/phase-task-crosswalk.json), this task belongs to
`ENG-P17-T01` / `REQ-FID-001`; perceptual and expressive endpoints are handed to
`ENG-P17-T02` / `REQ-FID-002`. Read
[grouped partitions](../validation/02-reference-corpus-provenance.md#grouped-partitions)
and [final preregistration](../validation/09-final-fidelity-acceptance.md#experimental-object-and-preregistration).
For correlated uncertainty, retrieve `R-METROLOGY-JCGM-100` through its
[official record](https://www.bipm.org/en/doi/10.59161/jcgm100-2008e) and inspect
§§4–5; the atlas records inspection of the contents and §4.1, not every section.
A targeted query is `JCGM 100:2008 correlated input quantities`; stop when the
measurement equation and remaining calibration needs are explicit.

1. Write one row per physical, acoustic, perceptual, and expressive claim:
   specimen/setup, gesture population, observable/unit, intended generalization,
   independent comparator, smallest consequential difference, uncertainty model,
   allowed processing, exclusion/stopping rule, and accepted/rejected/inconclusive
   decision. Mark unsupported thresholds proposed, with their rationale. A
   preference hypothesis cannot stand in for an acoustic prediction.
2. Freeze architecture, parameters, analysis revision, reference/calibration
   versions and all accessible development data. Split by the unit needed for
   the claim: whole sessions for session transfer, whole players for player
   transfer, whole specimens for specimen transfer. Keep one take's windows,
   microphones, resamplings, and other derivatives together. Record crossed
   player/session/setup relationships; one specimen cannot support an
   instrument-population inference. Reserve a final partition distinct from
   data used for model selection.
3. Record string installation/history, tuning before/after, fret/action geometry,
   nail condition, player support, humidity/temperature, excitation uncertainty,
   microphone position/orientation and room. Use metres, newtons, seconds,
   pascals, and explicit axes; clock delay is calibrated separately from onset
   alignment. Unmatched conditions restrict the claim rather than disappearing
   into a residual term called human variability.
4. Have an independent reviewer audit group membership and access logs before
   unsealing. Plan negative checks with duplicate derivatives and swapped
   condition labels so the analysis refuses invalid provenance. If holdout
   inspection guides a revision, record the exposure and obtain a fresh holdout
   or report only exploratory results.

Retain immutable raw/derived hashes, split/group tables, calibration covariance,
source locations actually read, preregistration revisions and the access decision.
The work order proposes `experiments/heldout-acoustic-prereg.md` and
`tools/fidelity/compare.py`; neither is current tooling. Existing
`integration.setar_play` in [tools/test_setar_play.py](../../tools/test_setar_play.py)
checks synthetic render/report integrity, not corpus independence. Final acceptance
requires planned `fidelity.heldout_acoustic` and `fidelity.metric_reliability`;
missing permissions/calibration produce **blocked**, leakage invalidates the
evaluation, and a clean manifest only admits T02–T03 to evaluation.

## P17-T02: score mechanisms separately

Compare string partial frequencies, inharmonicity, onset trajectory, decay
behavior, contact timing, bridge transfer, body mobility, sound pressure,
directivity, and gesture-dependent response using measures appropriate to each.
Do not collapse them prematurely into one “accuracy percentage.” A wrong
mechanism may be obscured by compensating microphone equalization or output
gain while a global metric improves.

Use complex frequency response and spatial observations where available, not
only magnitude spectra. Record level alignment when applied. Align temporal
events according to the preregistered rule; freely shifting every event to
maximize similarity can hide attack-timing defects. Visualize distributions
across notes and gestures, not only the best example.

Report uncertainty and sensitivity. A difference smaller than microphone
calibration uncertainty may not support a strong conclusion; a difference
larger than combined uncertainty may indicate missing physics, wrong input
conditions, or a measurement issue. Investigate rather than reflexively
fitting another filter.

### Execution recipe

**Question and route.** Which mechanism explains the discrepancy, with what
uncertainty, under matched input and observation? Execute `ENG-P17-T01` /
`REQ-FID-001` using [acoustic metrics](../validation/03-acoustic-metrics.md) and
[gesture/radiation validation](../validation/04-gesture-and-radiation-validation.md).
Use `R-METROLOGY-JCGM-100` §§5 and 7 for covariance/reporting and
`R-SETAR-MANSOUR-SCAVONE-2012`
([author paper](https://caml.music.mcgill.ca/lib/exe/fetch.php?media=publications:nantes2012_mansour.pdf),
§§3.1–3.4, §§4.1–4.2) for support/loading controls. Search the latter source for
`support`, `excitation`, and `accelerometer`; use its apparatus logic, not its
specimen's response as this project's truth.

Freeze estimator windows/bands, calibrated channel signs, noise floors and event
alignment before comparing the held-out takes. Obtain synchronized mechanical
and pressure channels through the acquisition campaign where the claim requires
them. An unmeasured input force or an unresolved partial remains explicitly
unknown. Use the following distinct outcomes rather than a weighted realism score.

| Mechanism | Measurement and comparison to retain | Diagnostic failure |
| --- | --- | --- |
| Pitch and dispersion | Partial trajectories in Hz and `1200 log2(f_model/f_reference)` cents, with mode assignment and uncertainty | Retuning each note or swapping close modes can conceal wrong stiffness/termination. |
| Loss and transfer | Resolved amplitude decay rates in s⁻¹, early/late windows, beating and noise-floor censoring | A quiet microphone tail cannot identify string loss independently of body transfer or radiation. |
| Nail/fret transitions | Synchronized force in N, motion in m, onset/release in s, transient spectra and work in J where observable | Free per-event shifts or time warping erase timing defects. |
| Bridge/body | Complex `Y_ij = v_i/F_j` in m/(N·s), coherence, phase/delay convention, diagonal/cross-port response | Magnitude peaks alone do not identify modal masses or signed residues. |
| Radiation | Calibrated pressure in Pa at recorded receiver coordinates/angles, common-scale spectra and directivity | A single microphone gain fit cannot establish spatial radiation. |
| Dynamics/history | Pressure/partial-level/contact changes versus measured or declared input and residual state | Per-note normalization deletes the dynamic-response claim. |

Validate extractors with independent analytic tones, close beating modes,
known decay/drift, calibrated noise and shifted events before using empirical
results. These are proposed metric-reliability fixtures, not existing CTests.
Report paired differences, grouped intervals, individual failures and sensitivity
to preregistered analysis choices. Propagate shared calibration errors; many
windows do not reduce them. A wide uncertainty interval is inconclusive, not an
automatic pass. Derive a decision margin from the claim, measurement resolution
and practical consequence before opening results.

Translate a supported diagnosis to current [config.hpp](../../include/setar/instrument/config.hpp):
`StringConfig.bridge_position_m` is speaking length while `total_length_m` includes
afterlength; `StringLossLaw` coefficients have units s⁻¹, m²/s and s and are not
`BodyMode.damping_ratio`. `BodyMode.radiation_weight` maps acceleration to
observation, never acoustic feedback. Inspect `Instrument::snapshot()`/`ledger()`
in [instrument.hpp](../../include/setar/instrument/instrument.hpp) and
`bridge_mobility` in [body_model.cpp](../../src/instrument/body_model.cpp).
The present [analyzer](../../tools/analyze_render.py) estimates a restricted
nominal-pitch window and plots synthetic renders; it is not this comparator.

Retain raw/derived manifests, metric tables, common-axis plots, alternative
mechanisms and exact field/equation recommendations. Current
`engine.open_string_pitch`, `contact.release_convergence` and
`acoustics.observation_invariance` in
[engine_tests.cpp](../../tests/instrument/engine_tests.cpp) supply narrow synthetic
checks. Planned `fidelity.metric_reliability` and `fidelity.heldout_acoustic`
require the independent evidence above. Reject unsupported metric extraction;
route mechanical residuals to the relevant P04–P09 owner and pressure-only
residuals to P10 before requesting a new fit from P12.

## P17-T03: evaluate ablations and generalization

Remove or simplify one mechanism at a time—string stiffness, two-polarization
motion, nail compliance, fret damping, body feedback, cavity loading, spatial
radiation—and compare against the full candidate on held-out tasks. The
question is whether that mechanism explains additional observations or
expressive behavior. Increased computation without demonstrable explanatory
value is not automatically progress.

Test parameter and environment changes not used during calibration: supported
tunings, stopped positions, dynamics, repeated excitation, microphone positions,
and physical strings with different properties. Study where the model fails.
The resulting validity boundary is a useful engineering result, even when it
reduces the scope of an initial release.

Check interactions. A contact model that works with a fixed string may fail when
coupled to a compliant bridge; a body reduction that matches an impulse may fail
under continuous gesture-driven feedback. Preserve those counterexamples as
regression tests and update the relevant earlier phase evidence.

### Execution recipe

**Question and route.** Does a specified mechanism improve prediction outside its
calibration conditions enough to justify its physical and computational role?
This is `ENG-P17-T01` / `REQ-FID-001`. Use
[failure hypotheses](../validation/04-gesture-and-radiation-validation.md#separating-failure-hypotheses)
and [sequence validation](../performance/05-performance-validation.md).
`R-NUM-BILBAO-SMITH-2005`
([author manuscript](https://www.pure.ed.ac.uk/ws/portalfiles/portal/11222313/acustica_final.pdf),
§II and the §III/appendix reading route) supplies energy/discretization reasoning,
not proof for this modal-contact implementation. Search within that route for
`boundary`, `loss`, and `energy` when defining a physically admissible ablation.
Use `R-SETAR-MANSOUR-SCAVONE-2012` for a paired support/loading experiment if an
apparent mechanism benefit changes with apparatus.

1. Register each full/ablated pair, the predicted change, unchanged physical
   inputs, numerical resolution and independent observable. First compare with
   remaining parameters fixed; a separately declared recalibrated comparison
   asks whether a simpler model can compensate using training data. Never refit
   on the final holdout or mix these two interpretations.
2. Distinguish available configuration interventions from proposed fixtures.
   Current `StringConfig.bending_stiffness_n_m2 = 0` removes bending stiffness;
   `BodyDesign.cavity.enabled` in
   [body_model.hpp](../../include/setar/instrument/body_model.hpp) controls the
   synthetic cavity. Zeroing one string's `BodyMode.bridge_participation` has an
   existing isolation precedent in
   [engine_tests.cpp](../../tests/instrument/engine_tests.cpp). Check changed
   poles, port work and initial state rather than assuming a switch affects only
   output. There is no general implemented toggle for every listed mechanism.
   Proposed single-polarization or rigid-nail comparators need their own equations
   and verification; an arbitrarily huge contact stiffness is not an oracle.
3. Keep mechanical and observation interventions separate. The current
   `setar_play --no-body` grounds the bridge and produces silence because only
   body modes radiate; it is not a fair audible “body feedback removed” condition.
   Setting `radiation_weight` to zero removes observation, not mechanical loading.
   A feedback ablation with a comparable observation requires a separately
   specified fixture and cannot be claimed from either operation.
4. Compare paired effect distributions on complete held-out tunings, stopped
   positions, dynamics, re-strike intervals and receiver views; reserve different
   strings/setups/specimens only when the claim extends to them. Test selected
   interactions, such as contact with bridge compliance and cavity with body
   reduction, under a preregistered factorial contrast. Log support, environment,
   tuning drift and input variability. Check each candidate's convergence and
   admissibility so numerical failure is not mistaken for physical evidence.

Retain full variant manifests, hashes, metric deltas with grouped uncertainty,
runtime context, counterexamples and the domain in which improvement disappears.
Current `coupling.cross_string_transfer`, `coupling.uncoupled_limit`,
`performance.restrike_memory` and `body.cavity_coupling_oracle`
([body_tests.cpp](../../tests/instrument/body_tests.cpp)) check synthetic mechanism
behavior, not its measured or audible importance. Planned
`fidelity.heldout_acoustic`/`fidelity.metric_reliability` remain the empirical
gate. A negligible effect with narrow uncertainty supports simplification only
within the registered margin/domain; broad uncertainty is inconclusive. Invalid
variants are excluded as invalid with reasons. Return failed physical hypotheses
to P04–P10, unresolved identification to P12, and justified reduction tradeoffs
to P13; pass interpretable audible contrasts to T04.

## P17-T04: design controlled listening experiments

Predefine the listening question: discrimination, preference, similarity,
expressive responsiveness, or identification of a specific artifact. Use
appropriate procedures, controlled levels, randomized presentation, balanced
orders, and honest handling of the observation environment. A listener's
inability to distinguish two clips in one test is not proof of mathematical
or perceptual equivalence everywhere.

Distinguish blind comparison of recorded outputs from live expressive evaluation.
The latter involves response to gesture, predictability, and player adaptation.
Both matter for an instrument, but their outcome measures and confounders differ.
Record performer familiarity, control mapping, latency, and instrument experience
where relevant and permitted by consent.

Do not claim compliance with a listening standard unless its actual conditions
and analysis were followed. References such as ITU-R methodologies inform
design choices but do not automatically validate a small informal session.
Keep raw response handling, statistical analysis, exclusions, and uncertainty
specified before drawing conclusions.

### Execution recipe

**Question and source choice.** For `ENG-P17-T02` / `REQ-FID-002`, select one
primary hypothesis that changes an engineering decision: whether held-out
fret-release artifacts are discriminable under the stated playback conditions;
whether the full nail model matches reference attacks better than its verified
ablation; or whether listeners prefer a rendering. Identify which conclusion
each permits. Preference alone does not establish reference similarity, and
comparison of two synthetic variants alone does not establish setar fidelity.

Read [listening experiments](../validation/06-listening-experiments.md) and the
atlas cards `R-EVAL-ITU-BS1116-3` and `R-EVAL-ITU-BS1534-3`. The pinned sources
are [ITU-R BS.1116-3 (2015)](https://www.itu.int/rec/R-REC-BS.1116-3-201502-I/en),
small-impairment assessment, and
[ITU-R BS.1534-3 (2015)](https://www.itu.int/rec/R-REC-BS.1534-3-201510-I/en),
intermediate-quality assessment/MUSHRA. They are different methods; ABX is not
automatically either. Retrieve the chosen edition and inspect its Annex 1
method, listeners, material, analysis and reporting before preregistration;
search `BS.1116-3 Annex 1` or `BS.1534-3 Annex 1` at the official route. The atlas
records retrieval of the BS.1116 PDF but a timeout for the BS.1534 PDF, not full
method review of both. If the needed text remains unavailable, mark conformance
review blocked and describe the proposed adapted design honestly. Record every
deviation; neither title confers certification.

1. Freeze primary endpoint, listener/stimulus populations, hypotheses, practical
   effect or equivalence margin, analysis, exclusions, multiplicity and stopping.
   Specify trained-listener and setar-practitioner roles separately. Use
   development-only pilot variance or explicit uncertainty scenarios to simulate
   power/interval precision with both listener and stimulus clustering. Justify
   participant and item counts from the decision-relevant effect, fatigue and
   expected missingness; no universal number or repeated-clip count supplies
   adequate power. Retain sensitivity to the assumed variance. This step plans
   authorized future collection and does not contact participants.
2. Construct permitted held-out stimuli with matched physical context, room,
   encoding, rate, duration and playback path. Declare timbre-level matching
   method/window and store its gains plus unadjusted files; retain calibrated
   level for a separate dynamics endpoint. Inspect filenames, metadata, silence,
   noise floors and loading cues for unintentional identity leakage. Preserve
   real attack differences when they are the target. Use training examples
   outside the final set, neutral instructions, randomized/counterbalanced orders
   and protocol-appropriate reference/control conditions.
3. Before unblinding, an independent reviewer replays assignments from the seed,
   verifies masking and playback calibration, and exercises the analysis on
   synthetic responses with known effects and null cases. Predefine how a failed
   control or equipment check invalidates an inference; never remove unfavorable
   responses ad hoc. Keep response dependence in the analysis rather than treating
   every trial as an independent listener.

Retain protocol/source versions, stimulus/build hashes, gains, assignment log,
equipment/level record, consent-permitted responses, exclusions, effect estimates,
uncertainty and failures. **Nonsignificant discrimination does not establish
indistinguishability.** Report compatible effect sizes; an equivalence conclusion
requires a justified preregistered bound and interval/design supporting that
bounded claim. Wide intervals or ineffective controls yield inconclusive results.

The current [player](../../apps/setar_play.cpp) and `integration.setar_play`
support synthetic stimulus plumbing only. The work order's
`experiments/practitioner-blind-study.md` and
`data/evaluations/<study-id>/manifest.json` are **proposed** paths; planned
`fidelity.blind_listening` is not a present CTest. Pass accepted, rejected or
inconclusive perceptual findings to T06 with mechanism hypotheses for P05/P06/P10.
Live control remains the separate T05 study.

## P17-T05: run practitioner-centered expressive trials

Ask qualified setar practitioners to perform a bounded set of musical actions
and assess agency, damping, repeated plucks, pitch behavior, gesture transitions,
and physical consistency. Include space for unexpected techniques rather than
forcing every response into an imported keyboard-synth vocabulary.

Treat differences among musicians as informative. The model may fit one setup
or school of technique while failing another. Document the range and avoid
declaring universal cultural authenticity from one participant. With consent,
link structured feedback to engine event logs and observation conditions so
the engineering team can reproduce actionable issues.

Evaluate whether the UI makes valid physical behavior accessible. A sound engine
may have an adequate nail model but expose it through controls too coarse to
play. Conversely, a cleverly mapped interface may conceal an incomplete
mechanical model. Keep both claims separate during interpretation.

### Execution recipe

**Question and route.** Can the stated controller/build let a practitioner
intentionally produce the declared setar contrasts—continuing resonance versus
damping, repeated strokes, controlled stopping and release—without losing
agency? `ENG-P17-T02` / `REQ-FID-002` owns this evaluation; the reviewed gesture
grammar is an upstream `REQ-PRF-002` dependency, not a claim supplied by playback.
Read [performance validation](../performance/05-performance-validation.md) and
[elicitation](../ethnomusicology/elicitation-and-terminology.md).
`R-SETAR-TABASSIAN`
([primary handout](https://www.atlasensemble.nl/assets/files/instruments/Setar/Setar%20by%20Kiya%20Tabassian.pdf),
pp. 1–2) motivates fingernail, tuning, course and sympathetic-response questions.
Target `setar fingernail damping paired course` when following that source's
terminology. For fretting, `R-FRETTING-POORHAYDARI-2023`, “Methodology and Initial
Tests” [7]–[8], distinguishes reported practice from a proposed tuning model.
Neither supplies a universal task vocabulary or measured controller mapping.

1. Prepare a future consent-controlled task matrix, with eligibility/background,
   prior prototype exposure and represented practices. Keep formative advisors
   distinct from independent evaluators. For each task record starting ringing
   state, tuning, selected strings, allowed controls, intended audible contrast,
   and practitioner-defined success. Pair a controlled action with a short
   phrase and permit unexpected techniques. Document unavailable controller
   actions as scope limits, not performer errors. No recruitment is performed
   by this documentation task.
2. Pin the actual executable, host, device, controller mapping, sample rate/block
   settings and measured end-to-end latency/jitter. Verify the playable path
   before scheduling an evaluation: the current optional plugin has no recorded
   hosted qualification. Counterbalance mappings/variants where meaningful,
   record familiarization and breaks, and distinguish first-use discoverability
   from skilled control. Visible controls or facilitator knowledge can prevent
   blinding; disclose that rather than pooling these outcomes with T04.
3. Capture a synchronized chain: requested action → controller event → physical
   `GestureEvent` → contact/bridge state → sound → participant explanation.
   Use seconds/sample indices, metres from the nut, radians and newtons where
   the signals actually represent them. A MIDI value is not a measured force.
   Current [performance.cpp](../../src/instrument/performance.cpp) implements
   `parse_score`/`schedule`; [gesture.hpp](../../include/setar/instrument/gesture.hpp)
   defines `NailStroke`, `FingerPress`, `FingerMove` and `FingerRelease` on
   persistent strings. `Instrument::snapshot()` and `ProcessSummary` expose
   contact/fault diagnostics; block-end telemetry cannot establish precise
   sub-block release timing without proposed finer offline capture.
4. Analyze task completion, timing/response consistency, intended dynamic range,
   recovery, fatigue and agency separately. Quantitative comparisons need a
   decision-relevant effect and precision/power design with repeated actions
   nested within players; qualitative inquiry needs justified coverage of
   contexts and explicit limits. Preserve divergent accounts and reviewed
   translations. Have practitioners check interpretations attributed to them;
   label the engineering team's causal diagnosis separately.

Retain anonymized task/event manifests, permitted recordings, latency records,
coding/disagreement tables and reproduced failure sequences. Current
`performance.persistent_state`/`performance.restrike_memory` in
[engine_tests.cpp](../../tests/instrument/engine_tests.cpp), `score.midi_mapper`
in [performance_tests.cpp](../../tests/instrument/performance_tests.cpp), and
`contact.gesture_replay` in
[realtime_tests.cpp](../../tests/instrument/realtime_tests.cpp) establish synthetic
state/mapping/replay properties, not practitioner approval. Planned
`fidelity.performer_coverage` remains open. Stop an invalid or unreliable playable
condition; classify missing access as blocked and ambiguous feedback as
inconclusive. Return mapping issues to P11/P15, latency to P13–P16, physical
transition failures to P05/P06, and scoped expressive findings to T06.

## P17-T06: publish the evidence and limitations

Produce a result organized by claims, methods, independent data groups,
observed differences, uncertainty, competing explanations, and acceptance
decision. Include representative successes, worst cases, ablations, and
failure recordings with appropriate rights. Preserve listener analysis and
practitioner feedback boundaries.

An aspiration to Pianoteq-level responsiveness and fidelity can guide which
tests to prioritize; it is not a legal, technical, or marketing certification.
Do not imply access to third-party proprietary implementation or data. The
final decision may be to gather a new specimen, revise physics, improve control,
or narrow the product claim. None requires a calendar deadline.

### Execution recipe

**Question and route.** Which exact claims survive independent review, and what
must change when one fails? The crosswalk assigns this task to both
`ENG-P17-T01` / `REQ-FID-001` and `ENG-P17-T02` / `REQ-FID-002`. Use the
[final fidelity dossier](../validation/09-final-fidelity-acceptance.md#required-final-evidence-bundle)
and [quality contract](../governance/quality-contract.md) to keep Q1/Q2 synthetic
or numerical evidence separate from Q3 measured-component and Q4 integrated
qualification. Read `R-METROLOGY-JCGM-100` §7 for reporting uncertainty and the
reporting part of the exact `R-EVAL-ITU-BS1116-3` or `R-EVAL-ITU-BS1534-3` edition
actually used. Search only the unresolved claim's source route, such as
`BS.1534-3 Annex 1 reporting`; do not retrofit a favorable method after outcomes
are known. The atlas's access limitations remain in the dossier until a documented
new inspection resolves them.

1. Assemble the proposed `fidelity-study/<approved-study-id>/` packet described
   in the final protocol: frozen claims, specimen/setup and split manifests,
   immutable raw references and transformations, candidate/build/assets,
   acoustic and port-work results, authorized listener/player analyses,
   qualification records and claim decision. This layout is a proposed output,
   not an existing measured dataset. Include exact commands, source locations,
   hashes, units, calibration covariance, analysis versions and excluded/failed
   cases so another reviewer can reconstruct each table.
2. Have an independent reviewer trace each claim from its preregistered
   observation and comparator through grouping, processing and uncertainty to
   its decision. Inspect representative distributions and the worst cases, not
   only selected clips. Check ablations and whether newly exposed data informed
   model changes. An acoustic pass cannot offset a failed expressive endpoint;
   a listener preference cannot repair wrong mechanical port work. Report
   exploratory follow-ups as such.
3. Assign **accepted within the stated domain**, **rejected**, **inconclusive**,
   or **blocked**, with reasons. Acceptance needs the predeclared meaningful
   margin and sufficient uncertainty resolution; an imprecise null result is
   inconclusive. Invalid calibration, leakage or broken controls invalidates
   that evaluation before its apparent score is interpreted. Preserve both the
   original result and any correction. Record the smallest discriminating next
   action: new support/calibration acquisition, missing mechanical channel,
   revised contact/coupling/radiation model, improved mapping, additional planned
   study precision, or a narrower claim.

Attach the actual scope of current `integration.setar_play`,
`body.mobility_reciprocity`, `body.mobility_passivity`, and
`realtime.block_partition` checks using their runners in
[tools/test_setar_play.py](../../tools/test_setar_play.py),
[body_tests.cpp](../../tests/instrument/body_tests.cpp) and
[realtime_tests.cpp](../../tests/instrument/realtime_tests.cpp). Their current
scope includes synthetic render integrity, sampled modal-body symmetry/passivity,
and within-build block-partition invariance, not measured fidelity or host latency.
Record any later executions with source/binary identity; listing a CTest is not
running it.
The four planned acceptance IDs—`fidelity.heldout_acoustic`,
`fidelity.metric_reliability`, `fidelity.blind_listening`,
`fidelity.performer_coverage`—need their own executed evidence, not a synthetic
coverage substitution. Supported findings may recommend changes to current
`InstrumentConfig`, `BodyMode`, `GestureEvent` or the observation chain, but
publishing a report alone changes none of their scientific provenance.

**Exit/handoff checklist:** the domain and exclusions are explicit; all required
claims have decisions and uncertainty; rights/consent govern each distributable
asset; practitioner statements and engineering inferences are distinguishable;
failure routes have owners and retained counterexamples; exact binaries and
their platform/host limits are identified. Send the reviewable dossier to
`ENG-P18-T01` for rights/claim review and `ENG-P18-T02` for packaging, and send
failed mechanism claims back to their earlier owners. Public distribution still
requires those gates. Missing evidence leaves this phase's scientific gate open
while allowing independent synthetic regression work to continue.

> Predeclare one physical, one acoustic, one perceptual, and one expressive
> claim. Construct groupwise held-out tests with the correct specimen/setup
> controls. Compare mechanisms and ablations; present distributions and
> uncertainty, not handpicked clips. Run a listening procedure appropriate
> to its stated question and collect scoped practitioner evidence. Report
> failures and limits explicitly. Do not equate lack of ABX significance,
> a beautiful waveform, or market comparisons with 1:1 recreation.
