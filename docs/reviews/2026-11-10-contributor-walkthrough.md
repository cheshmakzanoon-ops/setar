# Contributor walkthrough — 2026-11-10

## Result and review boundary

The three bounded routes below are usable for a contributor who has not seen the
original discussion. Each reaches an exact scientific task, its engineering work
order, an inspectable source route, a calibrated acquisition procedure or declared
synthetic fixture, a finding disposition, current code fields, independent checks,
and a downstream owner. They also produce useful blocked handoffs. This is a
finding about **documented execution readiness**, not a passed contact, body,
identification, or fidelity gate.

One concrete admission omission was reported to the coordinator and corrected
during this review: the normal-contact recipe originally stated only the
mathematical exponent domain, while the current engine admits finite exponents
in `[1,4]`. The corrected P05 recipe and acquisition mapping were rechecked against
the source. No remaining must-fix error was found in these selected routes.
The practical blockers described below remain real work.

The inspected source base is `b4ae94e8cb1e77710a22d85eb707977d65f6c5a4`, with
concurrent documentation changes. This review read
[AGENTS.md](../../AGENTS.md), [README](../../README.md),
[START_HERE](../START_HERE.md), the
[execution guide](../roadmap-execution-guide.md),
[source atlas](../research/source-atlas.md),
[research workflow](../research/research-to-model-workflow.md),
[evidence ledger](../research/evidence-ledger.md), and
[documentation campaign ledger](2026-11-10-documentation-expansion.md), then
followed the relevant phase, acquisition, example, template, source, and test files.

Checks performed here were read-only source and JSON inspection, targeted path
existence checks, and comparison of named CTests with their registrations and
assertions. No build, DSP execution, estimator implementation, acquisition,
participant contact, plugin work, or scientific promotion was performed. The
worked arithmetic below identifies supplied synthetic inputs; its independent
mathematical audit belongs to the separate research review. Publication access
statements are explicitly inherited from the dated ledger/atlas; this walkthrough
did not retrieve those papers again or upgrade their access status.

## Navigation and evidence checks

The [crosswalk](../../roadmap/phase-task-crosswalk.json) and
[engineering register](../../roadmap/engineering-tasks.json) give these routes:

| Bounded question | Scientific tasks followed | Exact receiving work orders | Result a contributor can leave now |
| --- | --- | --- | --- |
| Does a bright attack justify a different nail law? | P05-T02 and P05-T06, using the P05-T01 geometry contract | `ENG-P05-T02`; P05-T06 maps to `ENG-P05-T01` and `ENG-P05-T03` | Synthetic normal-law fixture contract plus a blocked empirical identification packet |
| Can signed body-response data become a reduced bridge model? | P08-T04, P08-T05 and the loading boundary in P08-T06 | `ENG-P08-T02`, `ENG-P08-T03`; P08-T06 also maps to `ENG-P13-T02` | Port/normalization decision, synthetic comparator design, and a blocked measured-asset handoff |
| Can observed string decay identify all loss coefficients? | P12-T01 and P12-T03, with P12-T04 partitioning | `ENG-P12-T01`; P12-T04 maps to `ENG-P12-T02` | Reduced-parameter decision and independent recovery/negative-control design, with empirical fitting blocked |

This mapping matters in practice: P08-T04 is not `ENG-P08-T04`, and P05-T06 is
not `ENG-P05-T06`. No such guessed work-order names are needed to follow the
recipes. The register's current statuses also remain useful: `ENG-P05-T02` is
`implemented_unverified`, `ENG-P05-T03` and `ENG-P08-T03` are `in_progress`, and
the inspected measurement/identification work orders are
`specified_not_implemented`.

Direct inspection of the
[campaign template](../../data/measurement-campaign-template.json) found 17
`missing` channels, zero raw attachments, null specimen/session identifiers,
`physical_admissibility: not_evaluated`, and
`human_measurement_review: not_performed`. These are static file observations,
not a fresh validator run. Inspection of
[the validator](../../tools/validate_measurement_campaign.py) confirms that
manifest/file checks do not perform calibration, identify parameters, establish
holdout independence, or load an engine asset. Acquisition 13 explains this
boundary and permits a restricted mechanism study without pretending all 17
packets are complete.

## Walkthrough 1: P05 nail-law decision

### Question and exact source route

The bounded hypothetical question is: **if an attack has greater high-frequency
energy than the current model predicts, does that observation support changing
`contacts.nail.stiffness`?** No such measured discrepancy is claimed here.

[P05-T06](../phases/P05-excitation.md),
[worked Example D](../research/model-decision-examples.md), and
[acquisition packet `excitation.nail_flesh`](../measurement/13-acquisition-campaign-execution.md)
lead to the same answer: audio brightness alone cannot distinguish stiffness,
contact position/width, motion, friction, body response, observation, or previous
string state. This is a concrete reason to specify a discriminating experiment.

The source trail is usable without inventing a source's scope. The atlas directs
the contributor to `R-SETAR-TABASSIAN`, pages 1–2, for performer-described actions,
and `R-CONTACT-BILBAO-2014`, versioned preprint, for potential/work methods.
P05 gives §2/§4 reading targets; the atlas does not claim a complete derivation
review. Example D additionally records a failed fresh preprint retrieval. A
contributor can retain both access records and request the exact version instead
of reporting unread methods as inspected. Neither source supplies this
specimen's nail coefficients.

### Concrete inputs and operations

The [contact/motion protocol](../measurement/pluck-contact-and-motion.md) and
acquisition chapter make the proposed empirical work actionable:

1. Identify the specimen, setup, string, support, permitted gesture, nail region,
   and initial resting/ringing state. Keep natural playing and controlled surrogate
   trials distinguishable.
2. Retain force calibration, string and nail motion, instrument-frame projections,
   original timestamps, clock offset/drift, and uncertainty. A camera rate or
   a visible release frame alone does not establish time accuracy. Pilot whether
   sensing changes the action.
3. Vary position, speed, and direction in controlled contrasts while holding the
   other mechanism and observation quantities fixed or carrying their uncertainty.
   Annotate engagement/release as intervals and retain a second annotation check.
4. Compare force–gap behavior and released displacement/velocity before comparing
   a declared brightness metric. Acquisition 13 distinguishes string work,
   prescribed-surface work, stored potential, and dissipation, so a contributor
   is not instructed to call every force–motion integral “injected energy.”
5. Reserve whole trajectories/conditions and an independent session where feasible;
   record exclusions and failed takes. Determine force, time, frequency, and
   prediction budgets before decisive data inspection. The documents appropriately
   leave these apparatus-specific budgets to the pilot instead of inventing a
   universal safe force or camera rate.

For work that can begin without a specimen, Example D supplies the declared
synthetic pair `delta_ref = 0.0005 m`, `F_ref = 0.05 N`, with `alpha = 1.5`,
and its stated `K = 4472.13595499958 N/m^1.5` and stored potential `1e-5 J`.
P05-T02 supplies the separated/contact/unloading cases and independent
mass/obstacle energy quadrature. That is enough to specify a bounded numerical
fixture. It is not an empirical nail measurement or an executed fixture in this
review.

### Current field mapping and its limits

The actual target is [config.hpp](../../include/setar/instrument/config.hpp)
`ContactConfig::nail`, containing `ContactLaw::{stiffness, exponent,
damping_n_s_per_m}`. The corrected recipe and acquisition mapping now match
[config.cpp](../../src/instrument/config.cpp): finite positive stiffness,
finite exponent in `[1,4]`, and finite nonnegative damping. A physically supported
fit outside that implementation range needs an explicit extension decision;
clipping it changes the model and invalidates the original fit claim.

[GestureEvent](../../include/setar/instrument/gesture.hpp) supplies
`position_m`, `angle_rad`, `speed_m_per_s`, `gap_m`, `force_n`, and `travel_m`.
Inspection of `Instrument::Impl::apply`, `assemble`, and the nail state transition
in [instrument.cpp](../../src/instrument/instrument.cpp) confirms the documented
straight prescribed point trajectory. Its `force_n` is a release threshold,
not a measured force-history input. `valid_x` currently requires
`0 < position_m < bridge_position_m`; the event cannot target the afterlength.
The direction is `(sin(angle_rad), cos(angle_rad))` in vertical/horizontal order.
Release removes the contact, and the remaining contact potential is subsequently
booked according to the documented release policy.

Physical patch width, tangential nail friction, a dynamic nail mass, arbitrary
measured-trajectory playback, and geometric slip-off are explicitly proposed
extensions. The work order's `src/contact/nail.cpp` and
`include/setar/contact/nail.hpp` paths are absent; the implementation-evidence
paths correctly lead to the existing instrument engine. A new contributor need
not create a duplicate contact engine to satisfy a proposed filename.

### Independent checks and retained decision

The following are existing test definitions inspected here, not fresh results:

| Check | Actual assertion scope | Additional evidence still required |
| --- | --- | --- |
| `solver.discrete_gradient_identity` | Test-side potential/force expressions check discrete work across separated, entering, leaving, and small-increment cases | Independent time-domain contact reference and the selected physical law's validity |
| `solver.newton_scalar_oracle` | Scalar Newton roots are compared with a test-side bisection solution | Full instrument/contact-reference convergence for the proposed change |
| `contact.nail_passivity` | Synthetic prescribed-stroke work, stored energy, release dissipation, and force-threshold behavior | Calibrated force/motion and observed human/surrogate release behavior |
| `contact.release_convergence` | Synthetic release displacement at 48/96/192 kHz is compared with a quasi-static point-compliance limit and a declared discrepancy budget | Measured release states, finite-patch/friction behavior, and an independent full contact reference |

The definitions are in
[numerics_tests.cpp](../../tests/instrument/numerics_tests.cpp) and
[engine_tests.cpp](../../tests/instrument/engine_tests.cpp), with registrations
in [CMakeLists.txt](../../CMakeLists.txt). P05 honestly leaves
`contact.strike_speed_envelope` and practitioner acceptance work planned.

The usable retained finding is **blocked for empirical stiffness promotion**,
with the brightness-only inference rejected and the physical cause unresolved.
Retain the competing explanations, missing channel list, proposed controls,
source-access limits, exact current fields, and independently specified synthetic
fixture. `ENG-P05-T01` receives the minimum calibrated acquisition request;
`ENG-P05-T02` receives a law/geometry change only if the resulting evidence supports
it; P05-T06's replay/held-out work goes through `ENG-P05-T03`, with parameter
ambiguity handed to `ENG-P12-T01`. Gate G-07 remains open.

## Walkthrough 2: P08 body FRF and reduction

### Question and exact source route

The bounded question is: **does a negative cross-mobility require rejection, and
what would let a calibrated response become an installable body model?**
[P08-T04/T05](../phases/P08-body-model.md),
[worked Example B](../research/model-decision-examples.md), and the
[`bridge.multi_axis_mobility` packet](../measurement/13-acquisition-campaign-execution.md)
provide a consistent route.

The atlas identifies `R-SETAR-MANSOUR-SCAVONE-2012` §3.1–§3.4/§4.1–§4.2 for
support and sensor-loading comparisons, and `R-SETAR-MANSOUR-2011` §3 for
the restrained/preloaded apparatus. It preserves `R-SETAR-MANSOUR-2015`,
DOI `10.1115/1.4030863`, as an access-limited acquisition lead. The older
conference article is not silently substituted for the unread journal article.
P08 separately points to primary reduction/passivity methods, with inspection
required before a method is adopted. A frequency table from any of these routes
does not supply this specimen's calibrated residues, damping, or normalization.

### Concrete inputs and operations

The [FRF protocol](../measurement/frf-and-modal-protocol.md) supplies the missing
operational detail behind “measure bridge response.” A contributor can specify
force entering each body port in N and conjugate velocity in m/s, register the
axes, calibrate the complete sensor chain and relative delay, and retain original
force/response traces, support/preload identities, force spectra, complex FRFs,
coherence, repeats, and rejected impacts. The protocol gives conversion rules
for accelerance/receptance and warns about DC and low-frequency noise. It also
states what sensor-loading and amplitude pilots must resolve.

P08-T04 then requires common-pole complex fitting with calibrated phase, a
time-domain decay comparison, and an explicit effective-loss interpretation.
P08-T05 requires consistent normalization, signed input/output maps, full-port
passivity reasoning, retained-band/residual treatment, and a held-out port or
condition. Correcting a cross-response sign by taking its absolute value is a
rejected operation, even if the resulting matrix still happens to be passive.

For an independent synthetic starting point, Example B declares a single mode:
`m = 0.02 kg`, `f = 300 Hz`, `zeta = 0.01`, participation `(1,-0.5)` at two
vertical bridge ports. Its analytic transfer, null force direction, normalization
transformation, and incorrect-sign control give a finite task that does not need
the unavailable journal paper or a real FRF. This review checked that the current
API can express those quantities; it did not execute that fixture.

### Current field mapping and the importer boundary

The example maps directly to `BodyMode::{frequency_hz, damping_ratio,
modal_mass_kg, bridge_participation}` in
[config.hpp](../../include/setar/instrument/config.hpp). The two ports are
`bridge_participation[0][0]` and `[1][0]`; the first index is a physical string,
and the second is vertical polarization. All eight translational bridge ports
exist. `bridge_mobility()` in
[body_model.cpp](../../src/instrument/body_model.cpp) evaluates the corresponding
continuous modal mobility with signed products of the participation entries.

The documentation distinguishes second-order modal numerators from first-order
pole residues and preserves mass/shape scaling across every map. It also identifies
the actual representation limit: real modes, scalar underdamped modal losses,
and eight translational ports. General complex modes, off-diagonal damping,
rotational ports, and arbitrary residual realizations require an extension.
`radiation_weight` is an observation map; it cannot provide the missing mechanical
feedback or measured pressure calibration.

The proposed `src/body/reduction.cpp`, `tests/body/reduced_model_tests.cpp`, and
`experiments/bridge-mobility.md` paths are absent in the inspected tree.
`Instrument::prepare()` accepts an in-memory configuration; it does not consume
the campaign manifest. [setar_play.cpp](../../apps/setar_play.cpp) still constructs
`synthetic_setar_config()`. Thus “deliver a candidate and field map for review” is
an executable organizational handoff, while “load this measured manifest into the
player” would currently be an invented operation. P08-T06 and acquisition 13
state that boundary clearly, including the danger of treating an empty body as
a failed-load fallback: its current meaning is grounded bridge contacts.

### Independent checks and retained decision

Inspection of [body_tests.cpp](../../tests/instrument/body_tests.cpp) confirms
that `body.mobility_reciprocity` compares swapped synthetic ports and includes
an independent analytic plate-modal sum in an uncoupled limit.
`body.mobility_passivity` checks metadata, all driving-point real parts, and the
full 8×8 Hermitian matrix at 2,001 frequencies from 10 Hz to 20 kHz. The
[coverage registry](../../roadmap/executable-test-coverage.json) correctly retains
partial coverage. These definitions do not prove measured-data passivity,
continuous-frequency passivity for arbitrary imports, or the quality of a
reduction that has not been implemented.

The planned `body.reduction_heldout_frf`, `body.reduction_passivity`, and asset
handoff checks therefore remain distinct. A useful retained packet can accept
the signed-port interpretation for the declared analytic fixture, reject a
frequency-only body import, and classify specimen reduction as **blocked** by
missing calibrated FRFs and the importer. The packet names normalization,
loss ownership, allowable representation, comparator, residual treatment,
held-out plan, and exact rejection cases.

`ENG-P08-T02` receives the acquisition/response-identification work and
`ENG-P08-T03` the reduction decision. P09 receives only a justified passive port
model; P10 receives a separate motion/observation contract; P13 receives the
bounded loading/publication proposal. An eventual measured-FRF response finding
can have a restricted accepted scope while the FEM/wood interpretation and
`REQ-BOD-001` remain open. The register's dependency on `ENG-P08-T01` still prevents
claiming the complete work order or phase has passed merely because a response
finding is useful. Gates G-11, G-12, and G-18 remain unresolved in this review.

## Walkthrough 3: P12 loss identification and holdouts

### Question and exact source route

The bounded question is: **at fixed wave speed in the flexible-string limit,
can decay observations determine both wavenumber-squared and
frequency-squared loss coefficients?** [P12-T01/T03](../phases/P12-identification.md),
[Example A](../research/model-decision-examples.md), and acquisition packet
`strings.stiffness_losses` make the nonidentifiable combination explicit:

`sigma_n = a0 + a2*k_n^2 + b2*omega_n^2`, with
`omega_n^2 = c^2*k_n^2`, identifies `a2 + c^2*b2` rather than its two members.
Changing length at unchanged wave speed does not fix that exact dependence.
Nonnegative bounds do not constitute independent information.

The source route points to JCGM 100:2008 §§4, 5, and 7 for uncertainty, and
`R-IDN-RAUE-2009`, DOI `10.1093/bioinformatics/btp358`, for profile-method reading.
The atlas records abstract/§2.3 excerpts and unsuccessful direct article access.
A contributor can derive and test the displayed algebraic ambiguity without
claiming to have reproduced an unread profile-likelihood procedure. That is a
useful bounded stop to the source search.

### Concrete inputs and operations

The recipe asks for a parameter/nuisance table with units, sharing level,
covariance, admissible domains, and acquisition identities before selecting an
optimizer. It then specifies scaled sensitivities at several perturbation sizes,
alternative boundary/observation hypotheses, conditional profiles where justified,
and the smallest added observable capable of resolving the ambiguity.

The [workflow's synthetic stiffness example](../research/research-to-model-workflow.md)
supplies an identifiable control: `L = 0.6500 m`, `mu = 0.000400 kg/m`,
`EI = 0.000200 N*m²`, and undamped `f1 = 220 Hz`, with listed fit and reserved
partials. P12-T03 directs a separate analytic generator and recovery analysis,
then requires single-frequency, flexible-loss, unknown-gain, wrong-mode, and
boundary-mismatch negative controls. Noise levels and covariance remain declared
synthetic choices until measured. This is sufficient to specify the first
estimator implementation without inventing a corpus.

For empirical work, the route requires independently constrained length/density,
multi-partial motion, bridge response and support/sensor records. The data must
distinguish actual amplitude decay, beating, observation cancellation, and
body-mediated loss before a coefficient is called intrinsic string damping.
When the observations support only an effective combination, that reduced model
is a legitimate finding; the microscopic decomposition remains unresolved.

### Current field mapping and independent tests

`a0`, `a2`, and `b2` map exactly to
`StringLossLaw::{decay_constant_per_s, decay_wavenumber_squared_m2_per_s,
decay_frequency_squared_s}` with units `1/s`, `m²/s`, and `s`.
[instrument.cpp](../../src/instrument/instrument.cpp) computes its modal wavenumber
using `total_length_m`; `bridge_position_m` is the speaking span. The standalone
`StringParameters.length_m` in [reference.hpp](../../include/setar/reference.hpp)
is the complete simply-supported span. The guide makes this boundary distinction
explicit, so a contributor can avoid treating measured coupled speaking-segment
partials as the uncoupled nut-to-tail basis frequencies. Preparation's loss and
frequency truncation must be recorded in a future fit's forward-model identity.

The proposed `tools/calibration/sensitivity.py`, `tools/calibration/fit.py`,
`experiments/identifiability.md`, and `tests/calibration/holdout_tests.cpp` are
absent. No ready-made identification command is promised. Existing
`preset.tension_inverse` in
[preset_tests.cpp](../../tests/instrument/preset_tests.cpp) checks the tension
helper against a separately coded forward frequency relation and invalid inputs;
it does not recover unknown tension/stiffness from noisy observations.
`engine.config_validation` checks admission, not identifiability. The phase
correctly leaves `identify.synthetic_recovery`, `identify.sloppy_direction`,
`identify.leakage_guard`, and `identify.heldout_reproduction` planned.

P12-T04 supplies a concrete independent guard design: perturb only final-group
values and require fitted parameters, learned preprocessing, and stopping
decisions to remain unchanged while final evaluation changes. It also requires
lineage rejection for a derived copy of a training raw asset. Both are future
tests, not properties proved by a split filename or the campaign validator.

### Retained decision and next owner

The bounded loss decision is to retain the identifiable coefficient combination
and explicitly preserve the unresolved decomposition. A physical calibration
promotion is **blocked** by the absent corpus; a synthetic recovery implementation
can proceed under `ENG-P12-T01`/`REQ-IDN-001` with the positive and negative
controls above. The handoff must carry parameter units, sensitivity scaling,
generator/analysis independence, noise assumptions, proposed error criteria,
rejected interpretations, and the additional force/motion/tension information
needed for any stronger claim.

`ENG-P12-T02` receives the frozen objective, lineage-aware split design, and
eventual fitting plan. P12-T06 routes an actually supported asset to
`ENG-P13-T01`, with P17 receiving a still-independent evaluation plan. Nothing
here justifies marking a mixed synthetic/measured `InstrumentConfig` wholly
`Measured`; the per-quantity sidecar and package-label policy remain required.
Gate G-16 and empirical calibration remain open.

## Blocked-handoff tests

The [finding template](../../templates/research-finding.md) has fields for missing
access, exact next owner, current/proposed symbols, disposition, and permitted
independent work. The [promotion template](../../templates/parameter-promotion.md)
keeps a pending or rejected candidate separate from a new installed model.
Applying those fields gives the following usable outcomes without fabricated
sensor files or citations:

| Missing input | Concrete blocked packet | Work that may proceed independently | Claim that stays open |
| --- | --- | --- | --- |
| Full text of the 2015 Mansour article or uninspected Raue methods | Record the exact DOI/version, inspected scope, historical retrieval outcome, needed apparatus/method sections, and any separately needed raw/calibration assets; assign an acquisition owner and keep the request unsent until authorized | Analytic two-port fixture, unit/normalization checks, and direct derivation of the flexible-loss invariance | Reproduction of unread methods and any paper-derived parameter import |
| Calibrated nail force, string motion, or complex bridge FRFs | Leave raw/calibration/specimen IDs null; name the question, minimum channels, axes, synchronization, support/loading pilot, uncertainty target, held-out unit, and receiving work order | Normal-law energy-oracle design, known-pole FRF processing controls, and synthetic identification/negative-control design | Measured contact law, specimen body response, or empirical parameter identification |
| Measured-body/calibrated-asset importer | Deliver a candidate field map, units, normalization, valid domain, provenance sidecar, supported-representation check, rejection policy, installation/reset policy, and independent round-trip plan | Validate a separately constructed synthetic in-memory fixture and design the offline conversion contract in a later implementation task | A loaded measured asset, transactional live publication, or runtime qualification |

These handoffs are usable because they say which input changes the decision.
They do not merely say “needs more data.” In this review they are worked task
outcomes and future assignments, not completed acquisitions or implemented
fixtures. The public manifest remains blank.

## Corrections confirmed and practical blockers remaining

**Corrected and rechecked:** P05-T02 and acquisition 13 now distinguish the
normal-law mathematical family, current `[1,4]` exponent admission, and empirical
support. They preserve an out-of-range fitted candidate for an explicit extension
decision and prohibit silent clipping. This resolves the only concrete mismatch
reported by this walkthrough; the worked `alpha = 1.5` example was already within
the current range.

**Minor navigation opportunity:** the P05 current-event mapping could repeat the
present `0 < position_m < bridge_position_m` admission restriction beside the
field list. The linked implementation exposes it and no inspected worked example
violates it. This is not a reason to alter the physical span definitions or add
afterlength gestures implicitly.

The remaining blockers are substantive:

- No identified specimen, permission packet, or calibrated raw records were
  supplied. A real operator must assign the pilot's instrument, apparatus,
  reviewer, permitted interventions, and uncertainty/acceptance budgets.
- The independent full contact reference, specimen FRF/reduction evidence,
  calibration estimators, and leakage tests are still implementation or
  experiment work. Existing synthetic forward tests cannot stand in for them.
- A general measured-asset importer, mixed-evidence package policy, and live
  publication/state-transition qualification remain proposed. The current
  in-memory API and template validators have narrower responsibilities.
- A restricted measured response may eventually support a useful finding while
  FEM/material gates remain open. Its reviewer must record that restricted scope
  and preserve register dependencies rather than treating an accepted finding as
  a completed phase.

The inspected documents expose these limits and provide the next bounded task
for each. This review changes no scientific status, requirement status, CTest
result, parameter, or runtime behavior.
