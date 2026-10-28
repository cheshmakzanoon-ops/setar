# P06 — Frets, stopping, damping, and changing speaking length

**Status:** specification. **Prerequisites:** P04 and P05. **Exit evidence:**
contact and stopping mechanisms preserve the declared mechanical balance and
predict a defined set of left-hand gestures without resetting the instrument.

## Establish the physical question

A fret is a contact constraint, not simply a command to shorten a delay line.
The string can remain in contact, lose contact, touch more than one support, or
transfer energy into a finger. Behind-fret segments can also retain motion.
Which effects matter depends on geometry, action, tension, displacement, and
the player's gesture. This phase determines which mechanisms are necessary for
the target specimen and how their limits are represented.

Read [contact mechanics](../contacts/README.md),
[performance](../performance/README.md), and
[numerical boundary treatment](../numerics/README.md). The reference problem
must identify whether the fret is modeled as rigid, compliant, lossy, or moving.
Do not introduce all four options before the simplest relevant case is verified.

## P06-T01: inventory stopping geometry

Build a specimen-referenced coordinate description of nut, bridge, tied frets,
finger contact region, and string rest position. Include string radius and
whether a reported height is measured to its center or surface. Record the
uncertainty and loading state of each measurement. A fret position measured on
an unstrung neck need not equal the effective contact point under playing load.

Specify which string segments are retained in the model. If the finger damps a
segment that is omitted, explain the reduced boundary impedance that substitutes
for it. An ideal fixed endpoint is a useful limiting case; it is not evidence
that the omitted segment has no audible effect. Keep a richer offline model to
test the simplification under the most demanding intended gestures.

For tied frets, distinguish the nominal fret center, contact crown, and variable
contact width. Include the mounting condition in the uncertainty record rather
than assigning a universal setar fret geometry. Avoid equating a visual fret
number with one immutable pitch across tunings, specimens, and practices.

## P06-T02: verify a stationary unilateral support

Start with one fixed obstacle and a single string. Test an initial state that
never touches it, a small isolated collision, sustained contact, and repeated
collisions. With a lossless obstacle/contact formulation, the energy balance
must account for reversible contact storage. If loss is present, distinguish
contact dissipation from bulk string damping.

Compare contact timing and impulse under step refinement. Plot penetration or
constraint violation in physical units; a visually smooth displacement trace can
hide an unacceptable overlap. If numerical regularization is used, show how
its parameter affects timing and spectral content. Do not reduce the step until
the waveform looks plausible and omit the convergence study.

Add simultaneous contacts only after the single-contact tests pass. Include a
case where contact activates at neighboring frets and a case where the finger
prevents a rebound. The active-set or nonlinear iteration policy must be bounded
for a production implementation and inspectable in the reference implementation.

## P06-T03: represent finger compliance and losses

A damping finger is not necessarily a fixed endpoint. Define whether the model
represents transverse impedance, normal contact, tangential loss, or a lumped
effective combination. Give each parameter a measurable role and unit. A
frequency-independent dashpot may be an initial approximation; assess its
validity against the measured decay and contact-duration range before adding
more elements.

Use prescribed finger motion as an external work input. If the finger model has
its own mass and compliance, include that stored energy. Verify that reducing
the damping coefficient approaches the corresponding lossless model, and that
removing the finger restores the uncontacted response without a hidden state
reset. A negative damping estimate is a diagnostic failure, not a permissible
tone-shaping adjustment.

Separate intentional left-hand muting from emergency numerical containment.
Musical damping should act through a documented physical or performance model.
A safety mute after nonfinite state must remain visible in diagnostics and must
not be counted as a successful damping gesture.

## P06-T04: move a contact without unexplained energy

Sliding or repositioning a finger changes the constraint and may do mechanical
work. Establish the difference between moving a physical contact and instantly
selecting a new numerical domain. If grid interpolation, remeshing, or modal
projection is required, test displacement, velocity, work, and interpolation
error across the transition. A crossfade can hide a click while still changing
the physics; label any such production approximation explicitly.

Exercise slow and rapid motion, motion during silence, and motion during strong
ringing. Compare different host block partitions with identical sample-stamped
events. A gesture should not change merely because a DAW uses a different buffer
size. Include endpoints, direction reversal, and an invalid position outside the
physical domain. Reject or safely constrain impossible geometry according to a
published input contract, not an undocumented clamp.

## P06-T05: investigate buzz and residual segments

Contact chatter can be a physical phenomenon, numerical instability, or both.
Identify it through step refinement, work balance, and controlled variation of
action and amplitude. Do not declare every high-frequency transient authentic
fret noise. Compare contact force histories, spectral changes, and measured
onset conditions rather than only one pleasing recording.

Evaluate whether behind-fret and nut-side motion matters to the target domain.
Perform an ablation: retain the segment, replace it with an impedance, then
remove its dynamics under an explicitly ideal boundary. Compare controlled
responses and perception with the same main-string parameters. Select the
simplest representation that satisfies the declared error budget, keeping the
ablation artifacts so future extensions can revisit the decision.

## P06-T06: qualify left-hand gestures

Build a gesture matrix with practitioner input: stopped notes, release to an
open string, damping, slides where relevant, repeated articulation, and contact
on a previously ringing string. Specify which actions are in scope rather than
pretending every named ornament is one standard MIDI sequence. Record the
physical control trajectory separately from its eventual keyboard mapping.

Acceptance needs both mechanical verification and instrument-specific evidence.
Show that failure cases are detectable, contact state survives block boundaries,
and changing the fingering does not silently erase unrelated strings. Hand off
event definitions, contact state ownership, and admissible geometry to P11.

> Implement one stopped-string experiment with an explicit fret/finger geometry.
> Document the gap convention, retained segments, moving-boundary work, and
> numerical constraints. Test no contact, sustained contact, release, and motion
> during ringing. Compare step refinement and event block partitions. Report
> unexplained energy or chatter as a defect, not as realism. Do not invent a
> specimen's fret positions or infer a universal tuning from equal temperament.
