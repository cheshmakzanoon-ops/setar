# Validating repeated gestures, ringing, and coupled playability

## Isolated notes are an insufficient test set

A physically modeled instrument is stateful. The result of a gesture depends on
what is already moving and which contacts remain. A library of convincing
isolated plucks can therefore coexist with unrealistic repeated strokes,
incorrect note-off behavior, or broken sympathetic ringing. This chapter defines
a sequence-level test program that exposes those failures.

Inputs are verified numerical components, a deterministic event interpreter,
declared controller mappings, and reference conditions with provenance. Outputs
are regression fixtures, mechanical and acoustic metrics, practitioner feedback,
and a capability statement limited to the tested domain. The program must not
claim universal high fidelity from one appealing phrase.

Separate verification, validation, and usability. Verification checks agreement
with equations and event contracts. Validation compares predictions with
independent observations. Usability asks whether a performer can intentionally
control the result. A system can pass one category and fail another; report them
separately.

## Repeated-pluck phase experiments

Begin with one synthetic string and a reproducible finite-contact path. Apply a
first pluck, then a second at several controlled phases of the continuing motion.
Measure supplied work, release state, peak displacement, and subsequent spectrum.
Compare with an independent reference and with an intentionally reset-state
control. The production path should follow the continuous-state reference.

Do not require the second pluck to add the same energy every time. A driven
contact can add or remove energy depending on relative motion. The invariant is
correct work accounting and reference agreement, not a fixed amplitude increment.
An amplitude-normalized test can hide precisely the phase dependence under study.

Vary spacing over several physically meaningful regimes: a new stroke before
the previous contact has ended, after release while strong motion remains, and
after substantial decay. These are not project deadlines; they are controlled
gesture intervals. The first regime may be invalid for a single-nail profile and
should then be rejected explicitly rather than spawning an extra nail.

## Ringing after note release

Create a fixture where a controller note ends but no physical damping gesture
is requested. The targeted string and coupled body should continue from their
state. Then repeat with an explicit damper and verify the difference in energy
flow. This pair catches accidental coupling between note ownership and physical
state lifetime.

Release one note while another owns a contact on the same string. Verify that
the surviving contact remains. Reuse the controller channel for a new note and
ensure expression changes affect the intended owner. These tests are essential
for MPE and equally useful for ordinary MIDI repeated notes.

A computational sleep optimization must preserve the possibility of later
coupling. A quiet string can receive energy from the bridge. Freezing it
indefinitely because its own output fell below a threshold removes sympathetic
response. Any dormant-state reduction needs a wake-up rule and an error bound
that include incoming port work, not merely local amplitude.

## Four-string and paired-course regressions

With coupling disabled, excite each slot separately and verify isolation. With
passive coupling enabled, excite one slot and observe the others. Total energy
must obey the combined ledger while individual string energies can rise. Do not
mistake energy transfer for instability or demand that every component decay
monotonically.

For the paired bass/drone course, test a shared sweep, individual slot targeting,
small controlled detuning, and independent damping. The two members should not
be identical unless the synthetic fixture intentionally makes their parameters
and excitation symmetric. Array order should not determine which one receives
the stronger pluck.

Separate direct nail excitation from bridge-mediated transfer in diagnostics.
An unplucked member can respond because of body motion, but a broad nail may
also contact it directly. Without contact traces and port forces, the two
mechanisms can be confused in a microphone recording.

## Left-hand sequence tests

Engage a lower finger, add a higher finger, pluck, remove the higher finger, and
then release the lower finger. The expected behavior follows the active contact
geometry, not the most recently received note number. Compare the full-domain
reference with any reduced speaking-segment implementation.

Test finger engagement during ringing, partial damping, and release without a
new right-hand excitation. Track energy in the nut-side and bridge-side segments
where the representation permits it. A state reset can produce a clean-sounding
transition while failing the physical continuity requirement.

For slides or pitch inflections, record the actual mapped mechanism. A tension
trajectory, moving finger, and output pitch effect should be evaluated as
different modes. Do not pool them in one listening set and then claim that all
demonstrate physical left-hand modeling.

## Alternating sequences and riz

Use the practitioner-reviewed gesture record to generate the intended alternating
sequence. Preserve its accent structure, selected strings, and left-hand
relationships. Compare a periodic synthetic exercise with a reviewed musical
example, but do not assume that random timing perturbation is the missing
ingredient if the result sounds mechanical.

Analyze stroke-to-stroke work, contact duration, release orientation, and
inter-onset timing. Correlations can matter: a stronger stroke may alter the
state encountered by the next one. Independent random perturbations of each
parameter can destroy such relationships. A variation model needs measured or
explicitly artistic justification.

Render the same sequence under several block partitions and sample rates within
the supported domain. Sample-rate changes require prepared-state validation;
they are not merely an output-file setting. Separate numerical convergence from
host scheduling invariance so a failure has a clear owner.

## Perceptual and practitioner evaluation

Choose questions tied to the current claim. Can a performer control the intended
dynamic range? Are forward and reverse gestures distinguishable in the expected
way? Does the paired course respond credibly? Does a stopping transition preserve
the intended phrase? These questions are more actionable than a single global
authenticity score.

Use level-matched comparisons when judging timbre, but retain physically scaled
versions for dynamics and work analysis. Document listening equipment, playback
level procedure, room conditions if known, and whether examples were randomized
or blinded. Unknown conditions remain unknown rather than being filled in with
typical values.

Do not coach participants toward the engineering hypothesis. Present neutral
choices and allow free description. Preserve disagreement and uncertainty. A
small practitioner panel can provide rich qualitative evidence, but it does not
establish population-wide preference or universal cultural validity.

## Regression metrics and thresholds

Use a small set of interpretable metrics: event-frame agreement, contact onset
and release differences, energy residual, modal-frequency error, decay error,
and selected spectral or perceptual features. Define thresholds before reviewing
the candidate change where possible. One aggregate score should not allow a
large timing error to be canceled by a small spectral improvement.

Thresholds need context. A synthetic solver comparison can use tight numerical
tolerances; a measured manual gesture has experimental variability. Do not apply
the same tolerance to both and call one a failure of physics. Conversely, broad
measurement variation should not excuse a deterministic scheduler moving events
by an entire host block.

Keep a failure corpus. Include formerly unstable contact cases, ownership bugs,
capacity overflow, and near-threshold buzz. A regression suite composed only of
successful musical examples is unlikely to catch the edge cases that matter
most in live use.

## Execution budget and physical fidelity

Measure worst-case supported sequences, not only idle or isolated-note cost.
Rapid alternation, overlapping fingers, multiple active contacts, and long
ringing can exercise different paths. Report the platform, build, sample rate,
prepared configuration, and event density. A configured cross-platform workflow
is not evidence that each platform has been exercised.

When optimizing, compare mechanical diagnostics before and after the change.
An approximation that saves CPU by clearing small string states may alter
sympathetic response. A reduced contact solve may change release timing. State
the resulting error and valid domain rather than describing optimization as
semantically free.

## Acceptance and capability statement

The work passes when the sequence corpus is reproducible, invariant tests pass,
held-out observations are reported honestly, and practitioner feedback maps to
specific capabilities or limitations. The capability statement should distinguish
implemented, numerically verified, acoustically validated, and practitioner-
reviewed behavior.

If only a synthetic four-string scaffold exists, the correct statement is that
the scaffold supports the tested state and scheduling properties. It is not a
finished setar. The next action is the smallest missing experiment or mechanism
identified by the evidence, not a broad claim of completion or an arbitrary
increase in preset complexity.
