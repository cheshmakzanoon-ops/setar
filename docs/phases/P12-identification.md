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

> Design and execute a reproducible identification experiment. State the
> physical/nuisance parameter split, observables, weighting, priors, constraints,
> and groupwise holdouts before fitting. Demonstrate synthetic recovery and
> derivative checks where applicable. Report competing solutions and failed
> fits, not just the optimum. Deliver a versioned calibrated asset only when
> independent predictions support its stated domain; otherwise deliver the
> next measurement or model-reduction decision.
