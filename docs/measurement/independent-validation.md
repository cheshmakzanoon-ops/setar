# Independent validation and leakage-resistant partitions

## What independence must exclude

Independent validation is not merely evaluating files that were not passed to
the final optimizer. Data also influence model architecture, parameter bounds,
manual listening adjustments, feature selection, mode-count choices, thresholds,
and user-interface mappings. If a developer repeatedly listens to a supposedly
held-out phrase while adjusting the model, that phrase is development evidence.

The project can use machine-learning terminology such as training, validation,
and test even when no neural model exists, but the roles must be explicit.
Calibration data estimate parameters. Development-validation data guide choices.
Independent-test data evaluate a frozen claim. Reusing test evidence for a
revision is allowed if its role is changed and the next independent claim uses
new evidence.

## Partition by the unit of generalization

Randomly splitting notes from the same recording often leaks specimen state,
room response, player technique, tuning, and microphone placement across sets.
Choose groups according to the claim. A specimen-generalization claim needs
held-out specimens. A player-generalization claim needs held-out players. A
gesture-generalization claim needs gesture families or meaningful variants that
were not used to design the mapping.

Use a hierarchy of linked groups: raw session, instrument configuration, specimen,
player, gesture family, and source material. Segments from one raw event and
processed versions of the same recording must remain together. If a phrase is
re-recorded under nearly identical conditions, decide whether its shared musical
structure creates leakage for the intended task and record the policy.

There may be insufficient data to hold out every axis simultaneously. State which
axis is genuinely independent and which remains shared. A same-specimen,
new-player test can support a limited player-transfer claim without supporting
unseen-instrument generalization. Do not hide the limitation behind a single
headline score.

## Partition manifest

Create the split before iterative calibration and store immutable group
assignments. Record the rationale, randomization seed where applicable, balance
constraints, and data access policy. Independent reviewers or protected storage
can help keep the final set unseen, but a human process is also needed to prevent
informal sharing of examples.

```yaml
partition_id: PARTITION-PENDING
status: proposed
claim_axis: null
group_keys:
  - specimen_id
  - participant_id
  - session_id
  - gesture_family_id
calibration_groups: []
development_groups: []
independent_test_groups: []
duplicate_policy: Same raw event and all derivatives remain in one group.
freeze_model_id: null
test_access_log: []
```

The empty lists are not a completed split. The validator should reject an
independent-validation claim until actual group membership, rights, and model
freeze are present.

## Physical validation ladder

Begin with mathematical verification on synthetic problems: conservation,
passivity, stability, convergence, known frequencies, and known transfer
responses. These tests establish implementation properties, not setar fidelity.
Next use subsystem measurements, such as string partials, bridge mobility, and
contact timing. Then test coupled behavior and complete acoustic output.

For strings, use held-out lengths, tunings, amplitudes, and damping conditions
within a declared safe domain. For body models, hold out excitation ports,
response points, or configurations that expose the predicted spatial structure.
For radiation, hold out directions and listening positions. For performance
control, hold out players or gesture combinations. Each test should be chosen
because it challenges a mechanism rather than because it is easy to match.

Do not let a successful downstream acoustic metric erase a failed upstream
physical test. Compensating errors can create a plausible spectrum. If the body
filter hides incorrect string dispersion, the model may fail under another
tuning or articulation. Report both levels and investigate the compensation.

## Metrics with interpretable scope

Use a compact set of metrics tied to the hypothesis. Partial-frequency error in
cents, decay-rate error, complex mobility residual, modal subspace agreement,
onset-time error, cross-string energy transfer, and directional pressure error
can each answer a specific question. A single generic spectral distance may be
useful but should not replace these mechanism-level measures.

Define time windows, frequency bands, normalization, and noise-floor handling in
advance. Report distributions across trials and groups rather than only an
overall mean. A mean can hide a catastrophic failure in one important articulation.
Show worst relevant cases and the conditions under which they occur.

When metrics have different units, do not combine them into an arbitrary weighted
score without explaining the tradeoff. A physically invalid model should not
receive an overall pass because another metric is excellent. Use hard safety
and admissibility requirements, then separate quality criteria within the valid
domain.

## Perceptual and player validation

Physical agreement and musical credibility are complementary. A model may match
selected mechanical measurements yet omit a gesture that players consider
essential. A pleasing synthesized sound may be musically useful without matching
the target specimen. Ask questions that preserve these distinctions.

Use player-reviewed tasks covering isolated articulation, transitions, sustained
patterns, sympathetic response, tuning changes, and meaningful phrases. Separate
listening judgments from active playing judgments. A keyboard performer evaluating
MIDI response is not testing the same interaction as a setar player evaluating
a physical gesture controller. Describe the interface and participant expertise.

Blind stimulus identities where feasible and counterbalance order. Include
repeated items to estimate within-listener consistency. Avoid leading wording
such as asking how “authentic” a labeled advanced model sounds. Ask a precise
question, provide a clear scale, and preserve free comments for failure analysis.
Expert disagreement is data, not a problem to average away without examination.

## Reference matching without unfair processing

For tightly controlled comparisons, align the physical state and excitation as
closely as the experiment permits. When exact gesture matching is impossible,
compare distributions or conditional features rather than pretending two takes
are identical. Use measured excitation descriptors to explain differences.

Apply documented, symmetric listening preparation. Loudness matching may be
appropriate for timbral comparisons but removes absolute level as a tested
property. Time alignment may remove latency or release-time differences. State
which degrees of freedom were normalized away and do not claim accuracy for
them from that comparison.

Do not tune reverberation, equalization, or stereo effects separately to favor the
model. If a production listening mode is evaluated, disclose its processing and
also retain a dry or mechanically interpretable comparison where useful.

## Statistical analysis

Choose the sampling unit correctly. Many notes from one specimen or many ratings
from one listener are correlated observations. Use group-aware confidence
intervals, resampling, or hierarchical models as appropriate. Report sample
coverage and exclusions. A narrow interval based on pseudoreplication is not
strong evidence.

Predefine primary outcomes and handle exploratory analyses separately. If many
metrics, parameter variants, and subgroups are examined, acknowledge selection
and multiplicity. Do not promote the one favorable comparison to the headline
while hiding the broader search. Exploratory findings can motivate the next
experiment without being presented as confirmatory results.

Use equivalence or noninferiority concepts only with justified margins and a
design capable of supporting them. Failure to detect a difference is not proof
of perceptual identity. A small panel may simply lack power or coverage. Report
inconclusive results honestly and avoid guarantees of exact replication.

## Failure analysis

For each failure, classify whether it arises from implementation error, invalid
parameter import, calibration uncertainty, missing mechanism, insufficient
coverage, or mismatch between the stated claim and the test. Preserve the raw
evidence and model version. A failed independent test is valuable because it
limits the claim and guides the next intervention.

Do not alter acceptance thresholds after seeing the failure. If the original
threshold was poorly chosen, document that methodological error and design a new
study. The old result remains a result under its original rule. Similarly, do
not silently exclude a difficult articulation because it exposes model weakness.

## Release evidence

The validation report states exactly which model version, parameter package,
platform, and output mode were tested; which specimens, players, gestures, and
conditions were covered; what remained independent; what passed; what failed;
and what is unknown. Link the analysis packet and rights-cleared examples rather
than relying on a promotional audio clip.

The gate permits only the scoped claim supported by the evidence. A validated
low-amplitude open-string behavior can coexist with experimental ornamentation.
A useful artistic instrument can be released without claiming a specimen replica.
The next action is determined by the most consequential unresolved behavior,
not by the desire to replace a cautious label with a stronger one.
