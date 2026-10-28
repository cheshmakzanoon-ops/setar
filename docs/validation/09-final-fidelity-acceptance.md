# Final fidelity qualification: a falsifiable, specimen-bounded claim

**Status: experimental protocol, not an executed study.** “Perfect recreation”
and “Pianoteq-level quality” are creative aspirations, not measurable
certifications or third-party endorsements. This project can make defensible
claims of predicted motion, acoustic identity and expressive playability for a
specified setar, setup and test envelope only after independent data support
them. No existing synthetic render, graph or documentation word count satisfies
this gate.

## Three distinct claims that must be evaluated

1. **Mechanistic correctness:** the chosen equations and discrete solver model
   intended physical effects consistently: string stiffness/tension/loss,
   finite nail/flesh and finger/fret contact, moving constraints, bridge/body
   loading, acoustic propagation and radiation. The evidence includes energy,
   convergence and physically meaningful port variables—not just sound.
2. **Acoustic correspondence:** for held-out reference conditions, predicted
   calibrated pressure, FRFs and time-frequency trajectories agree with actual
   measurements within a preregistered error budget incorporating noise and
   modeling uncertainty. Good pitch alone is insufficient.
3. **Musical expressiveness and listener impression:** musicians can perform
   meaningful setar gestures and independent listeners evaluate sound identity
   under controlled blind conditions. A single striking demo has little power
   to establish generality across dynamics or performers.

Treat the three outcomes separately. A mechanistically faithful model may be
poorly calibrated; a fitted equalization layer may be perceptually helpful but
does not prove bridge mechanics; a human study can be inconclusive despite a
strong acoustic measurement. Each accepted statement names its claim level
Q0–Q4 from the [quality contract](../governance/quality-contract.md).

## Experimental object and preregistration

Before any evaluation, identify the specimen(s), string installation, tuning,
fret positions, materials/bridge calibration, instrument support, temperature/
humidity, player, performer technique, microphone/room geometry and model
version. Freeze:

- The calibration train/validation/held-out split by specimen, session,
  gesture and observation location. Avoid splitting near-duplicate frames from
  the same pluck across train and holdout.
- Independent reference/sensor calibration, uncertainty/noise floor, timing
  alignment, audio gain rule, time-frequency analysis windows and frequency
  bandwidth. Matching the test microphone by optimizing gain after hearing
  each target result violates the holdout.
- Required tests and exclusions: open/fretted strings, dynamic pluck angles and
  velocities, re-strikes with residual state, slides/ornaments, damping,
  sympathetic transfer, tuning changes, different observation angles and
  representative phrases as authorized by practitioners.
- Reported effect metrics, minimum relevant differences if supported by
  expert/literature evidence, statistical analysis, groupings and how a failed
  metric influences the claim. Include planned qualitative failure reporting.
- Stopping, exclusion, data-retention and consent-withdrawal rules that do
  not depend on whether the model happens to sound good.

Register the plan and raw input hashes *before* running final comparisons.
Changing the holdout after a failure yields a new study/version, not a pass.

## Acoustic measurement and metric inventory

| Aspect | Required observables | Failure that a superficial comparison misses |
| --- | --- | --- |
| Pitch/intonation | Fundamental and individual partial trajectories, stopped-string intonation, note transitions | Correct static f0 but wrong attack chirp or fret bending |
| Stiffness/inharmonicity | Frequency-dependent partial offsets and amplitude dependence | Tuned harmonic oscillator masking bending dispersion |
| Decay/loss | Partial-specific time constants and frequency dependence, quiet tail noise floor | One common amplitude envelope hiding modal/body damping errors |
| Attack/nail release | Force/velocity/penetration traces where available; attack time, spectral flux and transient phase | Similar FFT magnitude but wrong contact motion or onset timing |
| Sympathetic behavior | Cross-string mechanical state and bridge FRFs under one-string drive, beating | Mixed copied audio pretending to be energy transfer |
| Fretting and damping | Contact times, damping work, afterlength and release continuity | State reset that yields clean MIDI note but deletes physical memory |
| Body/bridge | Complex multiport mobility, modal residues/damping, measured direct bridge response | Correct resonant peaks but wrong phase, port sign or spatial coupling |
| Cavity/radiation | Pressure at calibrated angles/distances, directivity and room separation | One microphone EQ that fails away from that position |
| Dynamic expressivity | Timbre/level/response trajectories across pluck speed, nail angle, fretting force and ornaments | Model tuned to one loudness/note only |
| Robustness | Unseen setups, players, temperature/humidity, tuning and block sizes within claimed range | Overfitting an individual recording or one machine |

Calculate acoustic metrics from matched physical conditions. In addition to
absolute errors, report the uncertainty of each target and systematic
calibration biases. If the measured observation is below the noise floor, do
not treat its phase or decay as a reliable target. Use held-out reference
pressure at consistent microphone gain rather than normalizing each model
render to its own loudness peak after the fact.

For a complex FRF \(H(f)\), evaluate magnitude *and* phase (with an explicitly
defined unwrap/delay alignment), coherence, mode shape/residue and significant
cross-port terms. For sound pressure, report the microphone/room chain, sample
clock, time window and signal-to-noise. For attack, align by a physically
defined onset or separately report alignment error; do not optimize away the
timing failure before measuring it.

Use multiple independent settings. An average over 100 time windows of the
same contact event is not a sample of 100 independent performances. When
analyzing multiple observations from one specimen/player, use appropriate
within-event or hierarchical uncertainty instead of independent-sample tests.

## Blind auditory experiment: execution procedure

1. Define the exact discrimination or equivalence/preference question. These
   are distinct designs; a non-significant discrimination test does **not**
   establish equivalence without power and equivalence bounds.
2. Select trained/untrained listener groups and practicing setar players as
   appropriate. Make the sampling and recruitment/consent process explicit;
   do not assume a Western guitar-player group represents setar expertise.
3. Reserve test notes/phrases and performers not used for calibration. Prepare
   matched recordings with controlled room, gain, duration and encoding, or
   disclose unavoidable differences and their likely confounding effects.
4. Counterbalance stimulus order and condition assignments, generate a
   replayable randomization manifest and keep identities masked from listeners
   and experimenters where possible.
5. Verify equipment/headphones, level consistency and attention/familiarity
   instructions. Include training examples *outside* the held-out corpus.
6. Collect trial choices, confidence and optional qualitative descriptions;
   record exclusions/dropouts under the preregistered rules.
7. Analyze participant-level or hierarchical results with uncertainty/effect
   sizes and correction for planned multiple comparisons; include negative
   cases and possible confounds, not just the best examples.
8. Retain anonymized consent-permitted data/analysis code and publish only
   legally distributable excerpts. Do not copy restricted musician recordings
   into a public test asset.

The study may legitimately conclude “results inconclusive for this sample,”
“under these conditions listeners preferred model A,” or “audible distinction
remained in fast fret-release gestures.” Those conclusions give engineers a
more useful next task than unsupported “perfectly indistinguishable” language.

## Playability study: physical action and human control

Blind listening cannot certify playability. Arrange practitioner testing for
repeated nail strokes, idiomatic tuning/intonation, finger motion, slides,
trills/ornaments, string selection, damping and phrase dynamics. Define what
the input controller is capable of expressing and label any fallback gestures
as adaptations. Test latency **as experienced in the actual host and audio
interface**, not as a simulation of note output with zero device delay.

Use annotated observations: requested action, mapped event, simulated contact
and bridge response, audible result, performer assessment and error category.
Consider response consistency, unintended artifacts, expressive range and
mental/physical demand to control it. Do not infer “authentic traditional
playability” from the developer's own preferences or generic MIDI keyboard
performance alone.

## Decision matrix and claim language

| Evidence state | Allowed wording | Disallowed leap |
| --- | --- | --- |
| Q1 synthetic | “Independent synthetic string fixtures behave as specified.” | “Physically recreated setar sound.” |
| Q2 numerical | “This coupled model passes the declared numerical energy and convergence tests.” | “Measured accurate tone under actual plucking.” |
| Q3 measured component | “For these reference specimen/conditions, bridge FRF/pressure/contact predictions met the preregistered uncertainty-based limits.” | “All setars sound the same / perfect all-note emulation.” |
| Q4 product qualified | “The listed builds/hosts passed specified real-time, acoustic and practitioner/participant tests for the published envelope.” | “Universal Pianoteq parity” or an unsourced 1:1 indistinguishability claim. |

If a key interaction fails, release claims must narrow honestly; don't assign a
weighted average that lets strong pitch fidelity offset wrong nail contact or
bridge energy. If results match a single specimen but not other setars, state
that limitation and treat the generalized instrument preset as a different
research question.

## Required final evidence bundle

```
fidelity-study/<approved-study-id>/
  preregistration.md           # frozen hypotheses and decisions before analysis
  specimen-setup-manifest.json # identity, setup/environment and rights
  data-splits.json             # train/validation/held-out hashes, no leakage
  measurements/                # access-controlled when required
  model-build-manifest.json    # source/solver/asset versions and parameter provenance
  acoustic-results.json       # individual metric values, uncertainty and status
  port-work-results.csv       # independent physical/numerical observables
  listening-study/            # consent-controlled anonymized data and analysis
  performer-study/            # action coverage and qualitative failure taxonomy
  screenshots/                # real GUI and plots with capture provenance
  qualification-matrix.json   # named OS/host/hardware runs
  claim-decision.md           # accepted scope, failed gates and reviewer roles
```

This layout is proposed; no such measured study is currently asserted. Keep
private raw recordings and participant data outside the public repository
unless redistribution is expressly authorized. Hashes and summaries can be
public when they do not breach consent, contracts or sensitive information.

**Handoff and authority:** `ENG-P17-T01/T02` depend on verified numerical methods,
valid calibration, tested plugin interaction and independent measurements.
Final release packaging adds the separate rights and installability gates
`ENG-P18-T01/T02`. Consult the full
[requirements register](../../roadmap/requirements.json),
[specimen acquisition matrix](../measurement/specimen-completeness-matrix.md),
and [verification tests](../engineering/verification-matrix.md).
