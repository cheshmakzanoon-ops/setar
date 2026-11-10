# P12 — Identify parameters without overfitting the instrument

**Status:** specification. **Prerequisites:** P11 and an authorized measurement
corpus. **Exit evidence:** an identifiable parameterization, reproducible fitting
procedure, quantified uncertainty, and predictions on untouched validation groups.

## Calibration is not proof by resemblance

A flexible model can match one recording for the wrong reasons. Damping, pluck
position, microphone response, body modes, and gain can compensate for each
other. This phase establishes what the observations actually identify and which
claims remain uncertain. Read [research identifiability](../research/README.md),
[measurement](../measurement/README.md), and
[validation](../validation/README.md) before selecting an optimizer.

The mathematical model must already have independent verification. Fitting a
solver's numerical dispersion to measured inharmonicity is not physical
calibration. Likewise, reducing an objective function does not repair incorrect
units, a wrong force sign, or an unlicensed recording.

## Entry packet and source map

Separate permission to test an estimator on synthetic truth from evidence needed
to calibrate a specimen. The current prototype has no measured calibration corpus,
and the [specimen template](../../data/specimen-template.json) and
[campaign template](../../data/measurement-campaign-template.json) are blank
contracts. Record unknowns as null. Independent synthetic work may proceed while
acquisition is blocked; empirical phase acceptance may not.

| Research need | Primary source and reading route | Interpretation limit |
| --- | --- | --- |
| Correlated input and observation uncertainty | `R-METROLOGY-JCGM-100`, [JCGM 100:2008](https://www.bipm.org/documents/20126/2071204/JCGM_100_2008_E.pdf), §§4, 5 and 7 | Metrology method, not specimen error bars or proof of unbiased acquisition |
| Parameter compensation and profiles | `R-IDN-RAUE-2009`, [DOI](https://doi.org/10.1093/bioinformatics/btp358), §2.3 reading lead | The atlas records abstract/§2.3 excerpts and failed direct retrieval; obtain uninspected methods before claiming them reproduced |
| Support/sensor confounding | `R-SETAR-MANSOUR-SCAVONE-2012`, [author-hosted paper](https://caml.music.mcgill.ca/lib/exe/fetch.php?media=publications:nantes2012_mansour.pdf), §§3.1–3.4 and 4.1–4.2 | Paired-apparatus method; project loading/support error still needs measurement |
| String equation and loss assumptions | `R-NUM-BILBAO-SMITH-2005`, [author manuscript](https://www.pure.ed.ac.uk/ws/portalfiles/portal/11222313/acustica_final.pdf), §II, then scheme/appendix as needed | Numerical method, not setar string coefficients or a proof for the current modal-contact solver |

Use the [source atlas](../research/source-atlas.md) to preserve access limits and
the [research-to-model workflow](../research/research-to-model-workflow.md) to join
findings to parameters. Each task's evidence packet needs the
[finding record](../../templates/research-finding.md); any accepted model change
also needs the [promotion record](../../templates/parameter-promotion.md).

## P12-T01: define the parameter and nuisance model

List each parameter's meaning, unit, admissible range, source, uncertainty,
scope, and expected observables. Separate global specimen properties from
session-specific setup, gesture variables, and observation-chain nuisance
parameters. A material modulus should not vary independently for every note
merely because that makes the fit easier.

Choose a reduced parameterization supported by the data. Include correlations
and invariances; for example, several combinations of geometric and elastic
properties may produce similar resonances. Explain which measurements break
those ambiguities. If no measurement can distinguish them, report the family
of solutions or an effective parameter rather than a falsely precise physical
constant.

Use transformations that enforce legitimate domains without concealing errors.
A log parameter can enforce positive loss, but it does not establish that the
chosen loss mechanism is correct. Invalid model configurations must remain
visible to the fitting process and its diagnostics.

### Execution recipe

**Question and research.** Which parameter combinations can the proposed channels
distinguish, and which merely absorb boundary or observation error? Read
`R-IDN-RAUE-2009` with its access limitation above, `R-METROLOGY-JCGM-100` §5, and
[parameter identifiability](../research/parameter-identifiability.md). Search
`profile likelihood practical identifiability correlated observations` and
`string decay wavenumber frequency squared degeneracy`. The measured route combines
independent length/linear-mass information, multi-partial motion and bridge
force/velocity, with support and sensor response in the acquisition manifest.
More samples from one microphone cannot by themselves remove an invariance.

1. Write `y=h(theta,u,z)+delta+epsilon`. Tabulate each physical/effective parameter,
   controlled input and nuisance quantity with unit, source, uncertainty, sharing
   level and allowed domain. Distinguish specimen geometry from session tuning,
   take excitation, support compliance, clock/gain and microphone transfer. Give
   every observed feature its unit and covariance group; carry common scale and
   calibration errors across channels rather than fixing them as exact.
2. Establish the exact loss counterexample before fitting. Define the current
   modal amplitude-decay law as `sigma_n=a0+a2*k_n^2+b2*omega_n^2`, where `a0` is
   in 1/s, `a2` in m²/s and `b2` in s. For a flexible string at fixed
   `c²=T/mu`, `omega_n²=c²*k_n²`; observations identify `a2+c²*b2`, not `a2` and
   `b2` independently. Nonnegative bounds can select a point on that family but
   cannot identify its members. Near-flexible strings give a poorly conditioned
   version; changing length alone at unchanged c does not resolve the exact case.
3. Compute scaled sensitivities at multiple admissible points, perturbation sizes
   and boundary hypotheses. Inspect singular directions and conditional profiles,
   re-optimizing nuisance parameters. Test whether independently observed stiffness
   or altered tension supplies resolvable information; sharing loss coefficients
   across tensions is itself a hypothesis. Separate structural ambiguity from
   insufficient signal/noise and from solver or feature-assignment discontinuity.

**Translate and retain.** Map `a0/a2/b2` to `StringLossLaw` fields
`decay_constant_per_s`, `decay_wavenumber_squared_m2_per_s`,
`decay_frequency_squared_s` in [config.hpp](../../include/setar/instrument/config.hpp).
Its modal span uses `total_length_m`, while the speaking length is
`bridge_position_m`. Retain parameter/covariance tables, sensitivity scaling,
SVD/profiles, alternative boundary/observation models and reduced-parameter
decisions. `tools/calibration/sensitivity.py` and `experiments/identifiability.md`
are proposed work-order paths, not existing estimators.

**Acceptance and handoff.** `ENG-P12-T01`/`REQ-IDN-001` requires planned
`identify.synthetic_recovery` and `identify.sloppy_direction`. Existing
`preset.tension_inverse` and `engine.config_validation` check arithmetic/admission,
not identification. Accept an identifiable combination or an explicitly bounded
effective model; reject confidence manufactured by bounds or hidden gains. Hand
accepted/rejected/inconclusive/blocked parameters and the smallest ambiguity-breaking
measurement to P12-T02/P12-T03. Physical admissibility remains a separate gate.

## P12-T02: define the objective before inspecting results

Select observations according to mechanisms: complex FRFs for coupling,
partial frequencies for dispersion, decay envelopes for loss, contact timing
for excitation, and spatial response for radiation. Specify preprocessing,
windowing, alignment, normalization, and uncertainty weighting before optimizing.
Do not tune those choices separately for each candidate until a favored model
wins.

Include multiple objectives or constraints where a single scalar hides important
failures. A low waveform error may reward compensating phase/gain changes;
a spectral average may hide an incorrect attack or unstable long tail. Report
component metrics alongside any aggregate objective. Use units or clearly
defined nondimensional residuals so weights are interpretable.

Keep physical constraints and scientific evidence distinct. Reciprocity,
positivity, and geometry bounds are admission requirements. A good fit cannot
waive them. Conversely, admissibility alone does not make a parameter set
specimen-correct.

### Execution recipe

**Question and research.** Will the objective penalize the actual physical failure,
with a defensible uncertainty model, before any candidate fit is seen? Read
`R-METROLOGY-JCGM-100` §§4–5 and the observable/profile reasoning of
`R-IDN-RAUE-2009` at the available scope, then
[calibration objective design](../validation/05-identifiability-and-calibration.md)
and [acoustic metrics](../validation/03-acoustic-metrics.md). Search
`complex FRF residual covariance correlated frequency bins` and `amplitude decay
estimator window bias`. Acquire repeated calibrated channels through
`bridge.multi_axis_mobility`, `strings.stiffness_losses` and
`observation.calibrated_mics`; derive covariance from the measurement model and
repeats, not from an optimizer's desired answer.

1. Freeze the observation vector, units, extraction code, valid band, noise-floor
   policy, windows, mode matching, time alignment, exclusions and normalization.
   Use m/(N·s) for velocity/force mobility, Hz or declared cents residuals for
   frequency, 1/s for amplitude decay, s for contact timing and Pa for calibrated
   pressure. Keep amplitude decay distinct from energy decay and body damping
   ratio; they cannot share a raw numeric residual.
2. Write residual blocks and their covariance. For meaningful phase, stack real
   and imaginary FRF components with shared calibration/time errors accounted for.
   A proposed `r^T Sigma^-1 r` objective requires justified covariance and rank
   handling; densely sampled bins or overlapping windows are not independent
   repeats. Preserve physical and perceptual weights separately. If a probability
   model is unjustified, call the quantity a weighted discrepancy rather than a
   calibrated likelihood.
3. Declare train-only selection of regularization, nuisance gain/timing treatment,
   component acceptance budgets and invalid-candidate behavior. Inspect a synthetic
   wrong-onset or wrong-cross-port candidate to ensure an average cannot conceal
   it. Derive tolerances from acquisition uncertainty, independent forward error
   and intended use; record proposed thresholds and their rationale before fitting.

**Translate and retain.** Current `BodyMode.radiation_weight` in
[config.hpp](../../include/setar/instrument/config.hpp) maps acceleration to
observation; it is not acoustic feedback. Fix its role and
`ProcessSpec.output_gain_per_pa` when separating observation gain from mechanics.
Retain the objective specification/hash, units/covariance, extraction examples,
component plots and alternative objectives rejected. `tools/calibration/fit.py`
is proposed; the current renderer is only a possible forward evaluator.

**Acceptance and handoff.** `ENG-P12-T02`/`REQ-IDN-002` owns planned
`identify.heldout_reproduction` and `identify.leakage_guard`. Present
`acoustics.observation_invariance` checks a synthetic observation/mechanics
separation, not residual statistics. An independent reviewer must reproduce the
objective from frozen records and see invalid physical candidates rejected
regardless of score. Record acceptance, rejection or missing-covariance blockage,
then hand the frozen specification to P12-T04/P12-T05.

## P12-T03: test the inverse problem on synthetic truth

Generate a controlled synthetic corpus with known parameters using a declared
model and noise/observation chain. First test a simple identifiable case, then
add the suspected ambiguities. Use an independent implementation or deliberately
different discretization where feasible to avoid an unrealistically easy
inverse crime.

Check parameter recovery, prediction recovery, sensitivity to starting values,
and confidence coverage when such intervals are claimed. Distinguish numerical
failure, local minima, and structural nonidentifiability. A fit can predict the
observations well while recovering the wrong physical parameters; that is an
important result, not a reason to discard the test.

Verify derivatives independently. For smooth components, compare automatic or
analytic derivatives with well-scaled finite differences. For contact switching
or event discontinuities, identify where derivatives are undefined or unstable.
Do not treat a differentiable surrogate's gradient as the exact gradient of
the original nonsmooth physical system without evidence.

### Execution recipe

**Question and research.** Can the analysis recover quantities it claims to know
and explicitly fail on known ambiguities before it touches specimen data? Use
`R-NUM-BILBAO-SMITH-2005` §II for equation assumptions and `R-IDN-RAUE-2009` as a
profile-method lead within its recorded access. Read the
[worked synthetic stiffness example](../research/research-to-model-workflow.md)
and [identification experiments](../strings/07-identification-experiments.md).
Search `synthetic parameter recovery inverse crime boundary mismatch` when choosing
an independent generator. Measurement routes supply later noise, missing-channel
and calibration models; until acquired, chosen noise levels remain synthetic.

1. Generate an analytic simply-supported string fixture with declared L, mu, T,
   EI and loss, using `omega_n²=(T/mu)*k_n²+(EI/mu)*k_n⁴`. Preserve undamped versus
   damped frequency conventions. Independently recover T/EI from several partials
   with L/mu known, reserving partials for prediction; a withheld synthetic partial
   is a pipeline check, not an independent instrument session.
2. Add deliberately inadequate cases: only one frequency while T/mu/L vary;
   `EI=0` with both `a2` and `b2` free at fixed wave speed; unknown excitation
   scale times unknown observation gain. Require detection of the corresponding
   families even when residuals approach zero. Add wrong mode numbering,
   termination compliance and omitted observation response as model-mismatch
   negative controls; a plausible fit must not falsely pass physical recovery.
3. Repeat over specified noise seeds, multiple starts and declared missing
   channels. Retain all outcomes, including solver failures and boundary hits.
   Compare analytic/implemented derivatives over scaled step sizes and check the
   predicted objective change against a fresh forward solve. For contact switching,
   document topology changes and one-sided behavior; a smooth surrogate must be
   evaluated again with the actual contact law.

**Translate and retain.** Use current isolated `StringParameters` in
[reference.hpp](../../include/setar/reference.hpp) only with its full simply-supported
`length_m`; the instrument's bridge/afterlength geometry is a different forward
problem. Current `solver.modal_exact_free_response` and
`solver.modal_forced_convergence` verify forward fixtures, not an inverse package.
Proposed `tools/calibration/sensitivity.py` and `experiments/identifiability.md`
should retain generator/analysis versions, truths, observation covariance, noise
seeds, recovery/prediction errors, profiles and coverage results if intervals are
claimed. Set accuracy criteria from analytic error and declared noise; a finite
interval by itself is not success.

**Acceptance and handoff.** Under `ENG-P12-T01`/`REQ-IDN-001`, planned
`identify.synthetic_recovery` and `identify.sloppy_direction` must both succeed:
recover the identifiable case and recognize the negative controls. Reject a
same-code roundtrip as the sole oracle or unsupported confidence coverage.
Classify failure as analysis, optimization, ambiguity or model discrepancy; hand
the accepted synthetic envelope and unresolved cases to P12-T04/P12-T05 without
changing the prototype's synthetic status.

## P12-T04: design independent partitions

Partition by meaningful acquisition groups: specimen, session, gesture family,
performer, setup, or observation position as appropriate to the claim. Randomly
splitting adjacent samples from one recording does not provide independent
validation. Define what generalization is being tested and preserve the held-out
groups from fitting and model-selection decisions.

Maintain a final evaluation set with access governance suitable to the project.
If developers repeatedly inspect it and tune the model, it has become part of
development evidence. Record that transition and acquire a new independent test
where necessary. There is no benefit in keeping the label “held out” after its
independence has been lost.

### Execution recipe

**Question and research.** What new condition does prediction generalize to, and
can any information from that condition influence fitting or selection? Read
`R-METROLOGY-JCGM-100` §5 for shared-error consequences and
`R-IDN-RAUE-2009` for the dependence of identification on observations, without
claiming unavailable full methods reviewed. Use
[corpus provenance](../validation/02-reference-corpus-provenance.md) and
[independent validation](../measurement/independent-validation.md). Search
`grouped validation repeated measurement session leakage calibration covariance`.
The empirical route is the `inference.heldout_split` acquisition packet joined to
raw session/configuration/performer/observation IDs, not arbitrary audio windows.

1. State the generalization claim first. Another take in the same setup tests
   repeatability; a new session, pluck family, microphone position or specimen
   tests a different domain. Select grouping at that level and record which
   nuisance/calibration quantities remain shared. Correlated calibration across
   sessions still limits independence even when filenames differ.
2. Build a lineage graph before assigning development, selection and final groups.
   Keep repeated takes, overlapping crops, resampled versions, channel exports,
   derived FRFs and annotations from a common acquisition in compatible groups.
   Attach raw hashes and configuration/permission references. Preserve exclusions
   and the reason a missing group cannot support the claim; do not fabricate a
   convenient session to balance partitions.
3. Freeze a split manifest and fit-facing views. Learn normalization, filters,
   feature selection, nuisance estimates, early stopping and hyperparameters only
   from permitted development data under the preregistered procedure. Keep final
   predictions separate until model/objective freeze. If inspection changes the
   model, mark the inspected group development and request new independent evidence
   or narrow the claim.
4. Design a sentinel test: replace only final-group values with a conspicuous
   synthetic perturbation and require unchanged fitted parameters, learned
   preprocessing and stopping decisions. The final evaluation result should
   change. Also attempt to insert another derived copy of a training raw asset
   into the final group and require lineage rejection.

**Translate and retain.** Current
[measurement campaign validator](../../tools/validate_measurement_campaign.py)
checks manifest/file integrity within its scope; `data.acquisition_manifest_integrity`
does not prove split independence. `data/splits/<split-id>.json` and
`tests/calibration/holdout_tests.cpp` are proposed paths. Retain the split/hash
manifest, dependency graph, access/inspection log, sentinel results and selection
history, with covariance and participant identities handled under the data contract.

**Acceptance and handoff.** `ENG-P12-T02`/`REQ-IDN-002` requires planned
`identify.leakage_guard` and `identify.heldout_reproduction`; neither is an existing
identification CTest. Reject leaked preprocessing or a claim broader than its
held-out unit. Accept a synthetic guard test separately from blocked empirical
partitioning, and hand the immutable fit/evaluation interfaces to P12-T05 and P17.

## P12-T05: run staged calibration experiments

Fit or measure isolated mechanisms first where practical. Use string tests to
constrain dispersion/loss, structural tests to constrain body response, and
controlled gestures to investigate excitation. Then jointly refine a limited
set of parameters while observing whether previously verified mechanisms remain
consistent.

Preserve every run's input hashes, code revision, configuration, optimizer,
seed, termination reason, and candidate parameter set. Do not keep only the
best result. Failed or competing fits reveal ambiguity and implementation
defects. Expensive sweeps should be restartable from explicit checkpoints,
without hidden mutable global state.

Compare baseline, candidate, and ablated models on the same partitions. A more
complex model should justify its extra parameters through predictive improvement
or newly explained behavior, not merely a lower training residual. Quantify
the uncertainty of the comparison and avoid selection based on one favorable
note.

### Execution recipe

**Question and research.** Does one limited parameterization predict linked string,
body, contact and observation measurements without one subsystem concealing
another's error? Read `R-SETAR-MANSOUR-SCAVONE-2012` §§3.1–3.4/4.1–4.2 for apparatus
confounding, `R-METROLOGY-JCGM-100` §5 for propagation, and
[string identification](../strings/07-identification-experiments.md) plus
[acquisition reduction sequence](../measurement/13-acquisition-campaign-execution.md).
Search `string decay boundary loss bridge mobility sensor loading identification`.
Acquire calibrated mechanisms in the defined packets; published modal frequencies
alone cannot supply masses, port residues or acoustic radiation weights.

1. Admit geometry, mass/linear-density, support, tuning and calibration records,
   carrying their covariance. Use controlled low-amplitude string trials to
   constrain T/EI and identifiable loss combinations; compare alternative
   termination assumptions and two-polarization beating before assigning all
   envelope change to material damping. Preserve the `a2`/`b2` ambiguity if the
   available dispersion/tension changes do not resolve it.
2. Fit structural/bridge response to calibrated complex multiport observations,
   including cross responses and documented support/preload. Check modal mass,
   normalization, residue signs and passivity independently. Keep body, support
   and acoustic loading hypotheses distinct; an effective installed loss must
   not be counted again as a separate string/radiation mechanism.
3. Constrain excitation from synchronized trajectory/contact information with
   structure fixed or uncertainty propagated. Compare onset and release motion,
   not only a spectrum. Calibrate the observation chain using its own channel,
   location and gain evidence; microphone response must not become arbitrary
   per-note body coefficients. Then jointly refine only parameters P12-T01 found
   supported, checking that earlier observables remain within their budgets.
4. Run the frozen objective on development groups with multiple starts. Compare
   baseline, candidate and mechanism ablation on identical inputs and permitted
   selection groups. Inspect residuals by string, amplitude, support, direction
   and time; profile disputed parameters and test gradient reliability near
   contact events. Evaluate the locked candidate on final groups once, with no
   post-evaluation gain or window adjustment.

**Translate and retain.** Current forward targets are `StringConfig`, `BridgeConfig`,
`ContactConfig` and `BodyMode` in [config.hpp](../../include/setar/instrument/config.hpp),
with `Instrument::prepare()`/`process()` in
[instrument.cpp](../../src/instrument/instrument.cpp). Offline fitting in
`tools/calibration/fit.py` is proposed; no optimizer belongs in `process()`.
Retain raw/derived hashes, every start/seed, solver resolution, objective version,
termination status, checkpoints, alternative fits, parameter covariance and
per-mechanism prediction plots. Reproduce an earlier stage independently after
joint refinement rather than accepting the joint score alone.

**Acceptance and handoff.** `ENG-P12-T02`/`REQ-IDN-002` owns planned
`identify.heldout_reproduction` and `identify.leakage_guard`. Present
`engine.lossy_energy_ledger` and `body.mobility_passivity` cover synthetic forward
properties, not calibration. Reject inadmissible fits or compensating systematic
residuals; use predeclared uncertainty/error budgets for a scoped decision. If
all admissible models fail, retain an inconclusive/missing-mechanism result and
send a discriminating measurement to P04–P10. Only a supported candidate proceeds
to P12-T06; unavailable measurements keep that promotion blocked.

## P12-T06: accept a calibrated asset, not a magic preset

Package the accepted parameters with specimen/session scope, data provenance,
fit procedure, uncertainty, validation results, and limitations. Separate
physical parameters from artistic presentation controls. A musician may prefer
an altered timbre; that choice should not overwrite the reference calibration
or its evidence record.

Acceptance requires usable prediction on declared independent groups and no
unresolved admission failures. If parameters remain nonidentifiable, choose
between additional measurements, a reduced model, or a narrower claim. Do not
publish extra decimal places as a substitute for resolving uncertainty.

### Execution recipe

**Question and research.** Can another contributor reproduce the accepted
predictions and install the same physical meanings without losing uncertainty or
overstating evidence? Read `R-METROLOGY-JCGM-100` §7 for reporting and revisit
`R-IDN-RAUE-2009`'s available identifiability argument, then the
[data contract](../measurement/data-contracts-and-provenance.md) and
[parameter-promotion template](../../templates/parameter-promotion.md). Search
`parameter covariance model calibration reproducible prediction scope` only to
resolve a specific reporting gap. The measured route is the authorized raw corpus
and untouched-group report, with reviewer traceability; neither a literature
table nor an optimizer file substitutes for that packet.

1. Prepare a versioned candidate manifest containing specimen/setup scope,
   calibration/source/raw IDs, analysis and forward revisions, objective/split
   hashes, fitted/effective/nuisance/fixed quantities, covariance, unidentified
   directions, admissibility results and prediction limits. Record accepted,
   rejected, inconclusive and blocked conditions. Keep artistic gain/timbre choices
   outside the reference calibration and preserve the original synthetic fixture.
2. Audit every field mapping independently. Map speaking and total length
   explicitly; retain loss in 1/s, m²/s and s rather than replacing it with body
   `damping_ratio`. For body modes retain frequency convention, modal mass,
   participation normalization and observation units. A frequency list alone is
   incomplete. Check `radiation_weight`'s acceleration-to-pressure meaning and
   ensure no observation calibration is mistaken for acoustic feedback.
3. Reconstruct candidate predictions from permissible raw/derived records and
   verify declared tolerances without refitting. Re-run units/admissibility and
   independent forward checks for the affected mechanism. Inspect both prediction
   uncertainty and parameter ambiguity; a narrow predictive domain can be accepted
   while microscopic coefficients remain effective. Reject a broader physical
   claim that depends only on positivity, prior bounds or a low training loss.
4. Version incompatible units, geometry, normalization, event/fret mapping or
   valid-domain changes. Provide old/new IDs, affected reports/presets, migration
   or explicit rejection, and rollback. Prepare/import outside the callback and
   specify the installation/reset policy before claiming state continuity.

**Translate and retain.** Current `InstrumentConfig::provenance` is coarse;
per-quantity mixed evidence requires a sidecar, proposed here as
`data/calibration/<asset-id>/manifest.json`. This path and a general calibrated
asset importer are not implemented. `Instrument::prepare()` validates a supplied
configuration; it does not reconstruct this scientific evidence. Keep the complete
promotion packet, reproducible commands, independent reviewer decision and
limitation statement, without restricted raw participant data in public assets.

**Acceptance and handoff.** The exact crosswalk is `ENG-P12-T02`/`REQ-IDN-002`
and `ENG-P13-T01`/`REQ-RTS-002`. Planned `identify.heldout_reproduction`,
`identify.leakage_guard`, `realtime.reference_parity` and `realtime.multirate_alias`
remain required in their scopes. Present `engine.config_validation`,
`preset.config_admissibility` and `data.acquisition_manifest_integrity` cannot
certify calibration or reference/runtime parity. Hand an accepted scoped packet
to P13 with an unchanged reference and error budget; otherwise deliver the reduced
model or next measurement decision and leave the calibrated-asset gate open.

> Design and execute a reproducible identification experiment. State the
> physical/nuisance parameter split, observables, weighting, priors, constraints,
> and groupwise holdouts before fitting. Demonstrate synthetic recovery and
> derivative checks where applicable. Report competing solutions and failed
> fits, not just the optimum. Deliver a versioned calibrated asset only when
> independent predictions support its stated domain; otherwise deliver the
> next measurement or model-reduction decision.

## Exit and handoff check

- The observation equation, units, covariance, parameter sharing and nuisance
  treatment were fixed before fitting; source access and model discrepancy remain
  visible in the finding records.
- Synthetic recovery includes deliberately unidentifiable and mismatched cases;
  physical positivity/admissibility is never reported as identification.
- Raw lineage, preprocessing, selection and inspection histories support the
  claimed held-out groups; any reused final data are relabeled development.
- The candidate explains independent predictions with the declared uncertainty
  and mechanism budgets, or the result names a reduced claim/new measurement.
- P13 receives versioned parameters, correlated uncertainty, validity envelope,
  independent reference procedure, failure cases and rollback. P17 receives the
  untouched evaluation plan and participant/data restrictions. No present fixture
  or completed template closes missing empirical evidence.
