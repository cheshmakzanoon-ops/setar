# Independently review a validation claim

## Review request

Review `[claim ID and report]` against `[model revision]`, `[corpus/split
manifest]`, and `[threshold/protocol revision]`. This is an evidence review, not
authorization to change the model, refit parameters, collect new participant
data, publish a report, or modify repository settings. Read the relevant
validation chapters and inspect the actual artifacts. Preserve the original
record even if its interpretation is wrong.

First restate the claim in a falsifiable form. Identify the specimen/population,
gesture domain, tuning, receiver, observable, conditioning, and intended use.
Classify it as numerical verification, empirical validation, perceptual evidence,
product qualification, or usability evidence. If the report mixes these, separate
the claims before judging them.

## Evidence audit

Trace the result backward from summary to numeric table, analysis configuration,
candidate output, raw reference, and permission/provenance record. Verify hashes
or equivalent immutable identity. Check whether the binary, fitted assets,
parameter mapping, and corpus are the versions named by the claim. A nearby
successful run does not support a different candidate.

Inspect the oracle. Determine what is independent and what code or assumptions
are shared. Check synthetic extractor validation, convergence, physical units,
sign conventions, and error floors. If the reported number comes from a graph
without underlying data, classify reproducibility as incomplete rather than
estimating precise values from pixels.

Inspect the split. Determine whether takes, sessions, players, instruments, and
derivatives were grouped consistently. Look for final-test examples used in
model selection, manual tuning, feature-weight selection, or favorable stimulus
selection. Treat repeated examination as potential leakage even if no training
script accessed the files.

Inspect uncertainty. Separate measurement, input, parameter, numerical, and
extraction contributions. Check correlated residuals and repeated observations.
Do not accept an optimizer's local covariance as the complete experiment's
uncertainty. Look for fitted parameters at bounds, weakly identified directions,
and nuisance transformations absorbing missing physics.

## Perceptual and visual claims

For listening results, compare the performed study with preregistration. Check
stimulus hashes, level matching, playback chain, randomization, listener groups,
exclusions, sample-size rationale, stopping, multiplicity, and analysis. Determine
whether the result concerns discrimination, reference resemblance, preference,
or playability. A nonsignificant ABX result does not establish equivalence.

For screenshots, determine whether each image is a wireframe, component snapshot,
native application/plugin capture, or offline report. Require build/context and
fixture provenance. Check numeric and semantic assertions separately from image
similarity. A polished diagram or generated mockup cannot demonstrate implemented
controls or measured acoustics.

For cross-platform or host claims, inspect observed logs. A configured matrix,
skipped job, or headless pass cannot support unexecuted Windows/macOS or plugin
format claims. Distinguish generic validator coverage from manual DAW workflows.

## Findings format

Report each finding with severity, exact claim affected, evidence location,
consequence, competing interpretation, and smallest corrective action. Separate
confirmed defects from hypotheses and missing evidence. Do not describe an absent
measurement as a failed physical model; it is an unsupported claim until tested.
Do not accept a broad claim merely because some narrower result is sound.

Conclude with one of: supported within the stated domain; supported only after
narrowing the wording; inconclusive because evidence is missing; contradicted
by identified evidence; or invalid because the protocol/oracle cannot answer the
question. Explain the choice with concrete observations.

## Handoff

Provide a claim-by-claim table, reproducibility status, uncertainty concerns,
leakage assessment, and proposed next experiments. If a threshold needs revision,
recommend a new version and rerun rather than retroactively moving the boundary.
Do not rewrite the report as though a proposed correction has already happened.
The review succeeds when the next engineer knows exactly what evidence would
change the conclusion.
