# Experiment record

This record applies to numerical, bench, acoustic, and participant experiments.
Mark its status as proposed until acquisition or execution has occurred. Use
explicit nulls or “not acquired” for missing observations; do not populate a
template with plausible-looking measurements.

## Hypothesis and decision

- Experiment identifier:
- Related task, phase, and source claims:
- Physical or software proposition:
- Competing explanations:
- Observable that distinguishes them:
- Decision for each plausible outcome:
- What the experiment cannot decide:

A test is useful when an unfavorable result can change the model or narrow its
claim. State nuisance effects that could mimic the expected difference before
collecting the main data.

## Object and state

- Evidence kind: synthetic, published reanalysis, measured, or mixed by field:
- Model revision or specimen/configuration identifiers:
- Parameters, units, signs, coordinate frames, and uncertainty:
- Support, preload, tuning, environment, and contact state where relevant:
- Participant/task identifiers and authorized recording scope where relevant:
- Instrument handling approval and prohibited interventions:

Separate intended settings from observed state. A nominal note name does not
establish tension, and a sample-rate header does not independently establish
clock accuracy. For synthetic fixtures, name the generator and known answer.

## Acquisition or execution chain

- Raw inputs and checksums:
- Sensors, channels, calibration identifiers, gains, polarity, and bandwidth:
- Time bases, synchronization, latency, and drift model:
- Software, compiler, build mode, hardware, and numerical settings:
- Exact commands or operator actions:
- Output location and overwrite policy:

Keep raw data immutable. The current renderer refuses to overwrite its known
output files; use a fresh experiment directory rather than deleting broadly.
Record each derived transformation and its parameters so a second analyst can
reconstruct the result.

## Design and quality controls

- Controlled factors and their allowed domain:
- Repetition and grouping units:
- Randomization or counterbalancing:
- Calibration/reference checks before and after relevant changes:
- Loading, fixture, room, or instrumentation-disturbance controls:
- Inclusion/exclusion rules and quality flags:
- Stop conditions:

Repeated strikes on one unchanged setup do not estimate remounting, player, or
specimen variation. State which level of variation the design actually samples.
Retain rejected trials and reasons where the rights and storage policy permit.

## Analysis fixed before the final test

- Measurement equation and estimator:
- Windows, filters, frequency bands, normalization, and delay corrections:
- Fit objective, constraints, initializations, and model-order policy:
- Uncertainty budget and covariance assumptions:
- Calibration/development/independent-test partitions:
- Primary acceptance quantities and justified tolerances:
- Exploratory analyses kept separate:

Normalization removes information. If loudness or onset is aligned away, do not
use the same comparison to claim accurate absolute level or release timing.
Independent test data cannot also select the best model without changing their
role to development evidence.

## Results and interpretation

- Commands/actions actually completed:
- Observed outputs and uncertainty:
- Failed or missing channels, trials, and checks:
- Comparison with each competing explanation:
- Result: supports scoped claim, contradicts it, inconclusive, or invalid:
- Alternative causes not ruled out:

Attach real artifacts, not anticipated plots. State whether a disagreement is
larger than measurement uncertainty and whether model discrepancy is distinct
from acquisition error. Do not inflate uncertainty to make an inadequate model
appear validated.

## Reproduction and release

- Reproduction commands and dependency manifest:
- Independent reviewer result:
- Rights decision for raw data, derivatives, and public examples:
- Restricted assets omitted from the repository:
- Claim wording permitted by the evidence:
- Next discriminating experiment:

The packet is complete when another qualified person can reproduce the analysis,
understand its limits, and determine which uses of the artifacts are authorized.
