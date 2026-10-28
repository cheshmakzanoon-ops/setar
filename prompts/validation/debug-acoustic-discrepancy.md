# Diagnose an acoustic discrepancy without fitting it away

## Diagnostic task

Investigate `[specific mismatch]` in `[candidate revision and fixture]` relative
to `[reference identity]`. Examples include an incorrect early decay, missing
sympathetic beat, bright attack, pitch drift, or receiver-dependent spectral
error. This request authorizes diagnosis and relevant non-mutating experiments,
not an unrequested model rewrite or new data collection. If implementation is
also authorized, isolate diagnosis before making the smallest justified change.

Preserve the failing render, input events, parameters, model assets, reference,
analysis configuration, and logs. State whether the reference is analytic,
synthetic, published, or measured. If measured, verify specimen, gesture,
receiver, permission, and conditioning. Do not begin by adjusting an equalizer
or normalization until the mismatch looks smaller.

## Localize the layer

Reproduce the same canonical input in the headless core and, if relevant, the
plugin harness. Compare event timing and configuration identity before comparing
sound. Determine whether the mismatch already exists in internal string state,
bridge force/velocity, body response, radiation, or only the final audio path.
A microphone discrepancy does not automatically implicate string damping.

Check units and sign conventions, sample rate, discretization, coefficients,
precision, denormal policy, mode truncation, and contact residuals. Verify that
the report is plotting the intended channel and observable. Confirm that the
extractor returns correct results on a synthetic fixture resembling this case.
Rule out analysis artifacts before inventing a physical explanation.

## Competing hypotheses

Create a short hypothesis table. Each row names a mechanism, its predicted
signature, an intervention that distinguishes it, and possible confounds. For
excessive brightness, candidates may include release duration, pluck position,
mode weighting, body mobility, receiver direction, or observation calibration.
For a long tail, distinguish weak loss, energy transfer, beating, noise-floor
bias, and a stuck event lifetime.

Choose interventions that change one relevant factor at a time. Tighten the
numerical reference to test discretization. Dampen a companion string in a
synthetic control to test coupling. Change receiver view while holding mechanical
state fixed to test radiation. Shift contact timing relative to the sample grid
to test event/discretization interaction. Explain which outcomes would reject
each hypothesis before running the experiment.

Do not treat every parameter direction as independently identifiable. If several
parameter combinations produce the same observation, report the ambiguity and
propose another measurement. More optimization iterations cannot resolve a
structurally uninformative experiment.

## Quantify the mismatch

Use the relevant feature with units and uncertainty, plus at least one independent
view. A decay problem needs fit-window/noise-floor sensitivity; a pitch problem
needs partial assignment and time dependence; an attack problem needs alignment
and input-timing uncertainty. Preserve unnormalized output when dynamic response
is implicated. Show the failing case and nearby controls.

Check the discrepancy across block partitions and supported numerical settings.
If it changes with host block size, investigate scheduling or smoothing before
claiming new instrument physics. If it appears only on one platform, compare
flags, library behavior, and exact event order; do not demand bit-identical
transcendentals without a justified contract.

## Deliverable

Return a reproducible case, hypothesis table, experiment results, eliminated
causes, remaining uncertainty, and a recommended smallest next action. Attach
numeric data and actual plots with provenance. Mark synthetic interventions
synthetic and absent measurements absent. Do not create invented measured curves
to illustrate a preferred explanation.

If a fix is authorized and evidence identifies the cause, add a regression that
fails before the fix and checks the physical or semantic contract afterward.
Run affected cross-platform tests, preserve failure artifacts, and compare the
changed model against the independent reference. State whether the change fixes
software, improves a model within its domain, or merely provides a protective
fallback. Those are different outcomes and must remain different in the report.
