# Multi-string sweeps and paired-course encounters

## One gesture, several interacting objects

A sweep is a path through the instrument's string geometry. It is not merely a
chord whose note-ons have arbitrary delays. A single nail can encounter several
strings, carry altered velocity or force after one encounter, and interact with
strings that are already moving. The paired bass/drone course makes this
distinction especially important: the two members are close in playing geometry
but remain separate states.

The proposed high-fidelity reference uses one nail geometry and four physical
strings. A reduced real-time implementation may schedule bounded contact
segments from that geometry, but it must retain the physical meaning of encounter
order, target eligibility, and energy. The reduction is compared with the
reference rather than declared equivalent by design.

Inputs include the prepared string-array geometry, per-slot equilibrium heights,
nail surface and motion, course eligibility, and current string states. Outputs
include encounter intervals, per-slot forces, total actuator work, and a trace
linking all encounters to the parent gesture. The gesture has one identity even
when it creates several contact IDs.

## Geometry establishes order

Store each string's local position in the plucking region, not only its course
number. A straight path across a flat array is a useful synthetic fixture, but a
real hand trajectory and string array may be curved. Candidate intersections
must account for the nail's extent and orientation. A point moving between string
centres can miss a contact that a finite surface would make.

Reverse the path to obtain a reverse sweep only when that operation matches the
chosen physical gesture. Reversing a list of note events is a scheduling
approximation. It does not reverse nail curvature, flesh exposure, velocity
history, or feedback from preceding contacts. The approximation may be adequate
for a restricted controller mode, but its limits need evidence.

Tie-breaking for nearly simultaneous encounters must be deterministic. If the
reference solves them together, the real-time model should not create a large
order-dependent difference by handling one string fully before the other. Use a
coupled solve or demonstrate that the chosen splitting error is below the
declared tolerance over the supported geometry.

## Prescribed versus reactive sweeps

With a prescribed trajectory, the actuator supplies whatever work the interaction
requires within the model. Contact with the first string does not slow the nail
unless the trajectory policy explicitly changes. This is useful for controlled
comparisons because it isolates geometry and string state.

With a dynamic finger/nail representation, preceding contacts change the nail
state and can alter later encounters. The driver must supply force or impedance
rather than an inconsistent simultaneous position constraint. This richer model
needs additional identification and may remain an offline reference until its
benefit is demonstrated.

Do not combine the two models accidentally by prescribing position while also
integrating a free nail velocity that is used in friction. That creates two
incompatible contact velocities. Every force and work calculation must use the
same kinematic definition.

## Eligibility and selection are separate from collision

A slot mask can state which strings the player intends to address. Geometry then
determines whether contact actually occurs. A mask should not force an encounter
through an impossible path. Conversely, a physical path may contact an unintended
string. Whether the simulator allows that accidental contact is a declared mode
choice, not an invisible exception.

For research, preserve physical incidental contacts. For a keyboard-oriented
performance adapter, a constrained path may avoid them intentionally. Label the
latter as assisted targeting. It can improve usability without pretending to
reproduce every consequence of an unconstrained hand trajectory.

A paired-course command should resolve to both eligible member slots but should
not prescribe equal impulse, equal release time, or equal amplitude. If a user
wants only the bass or only the drone, the adapter needs an explicit slot-level
selection or a verified path that selects it. Do not silently reroute an
unplayable request to a different physical string without reporting the policy.

## Timing at and between samples

Encounter time can fall between output samples. The numerical solver determines
how contact onset is represented at its timestep; the host event scheduler
determines when the parent trajectory becomes active. These are different
timelines. A sample-accurate parent event does not imply infinitely precise
collision timing.

Test timestep refinement of encounter order, contact duration, release state,
and radiated attack. If a small timestep change swaps two nearly simultaneous
encounters and changes the result substantially, the model needs a coupled
treatment or a documented uncertainty region. Do not hide this sensitivity by
rounding all encounters to a fixed strum grid.

A reduced scheduler can use deterministic sub-event offsets derived outside the
callback from a prepared path. Record the derivation and its limitations. If the
string's existing motion can change encounter time materially, a fixed offset
cannot capture that effect. The reduction's valid domain should exclude or bound
such cases rather than silently ignore them.

## Capacity and bounded work

Prepare a bounded candidate list. Four strings do not automatically mean four
contact points: a finite surface or fret model can create more quadrature
contacts. Define maximum active contacts, maximum candidate tests, and solver
work for overlapping sweeps. Reject or simplify an over-capacity command through
a documented policy before it destabilizes rendering.

Overlapping parent gestures require a clear ownership model. A physical hand may
not execute two contradictory nail trajectories simultaneously, but a controller
can request them. The interpreter may queue, replace, merge under a defined
rule, or reject. It must not create unlimited virtual nails in a mode advertised
as one physical performer unless that is an explicit creative extension.

Telemetry should report capacity rejection without allocating a diagnostic
string in the callback. Use bounded codes and counters; render readable messages
outside the real-time thread. A missing contact due to capacity should not be
indistinguishable from a geometric miss.

## Energy accounting across encounters

Sum work over all contacts using their actual relative velocities. The same nail
reaction can transfer energy among strings through a dynamic contact body. The
instrument body can also exchange energy through bridge coupling. Separate those
paths in diagnostics so a sympathetic response is not misattributed to a direct
pluck.

When the nail is prescribed, total string-plus-body energy can increase by the
actuator's work. When the nail is free and all external driving is disabled, the
closed mechanical system should obey its conservation/dissipation law. These are
complementary tests. Demanding monotonic string energy in the driven test would
misclassify valid excitation as instability.

A reduced impulse sequence should be compared at equal total supplied work or
another explicitly selected condition, not merely equal arbitrary velocity
numbers. Otherwise a perceived timbre improvement may only be a loudness
difference. Preserve unnormalized outputs for physical analysis even when
loudness-matched versions are used in listening tests.

## A discriminating test matrix

Begin with four identical synthetic strings at equally spaced positions and no
body coupling. Verify encounter order under forward and reverse paths. Then
change only one height or spacing and predict the order change. Add a paired
course with distinct density and tension while retaining the same geometric
path. Confirm that its members can receive different work.

Next initialize one string with known motion before the sweep. Compare results
across phases and against a zero-state control. Then enable a passive body and
observe indirect energy transfer after the nail has left. Finally compare a
prescribed and a dynamic nail under a shared driver definition, documenting
which comparison quantities can be held equal.

For deterministic scheduling, render the same parent event stream under several
host block partitions. Encounter behavior should agree within the engine's
declared determinism contract. A block boundary must not restart the path or
reseed variation. Include a sweep whose first contact occurs just before one
boundary and whose last contact occurs after another.

## Acceptance and handoff

The deliverables are prepared geometry, a bounded candidate strategy, parent and
contact ownership rules, contact traces, work ledgers, and reference comparisons.
Acceptance requires four persistent states, independent paired-course behavior,
deterministic ordering, correct response to existing motion, and explicit
over-capacity behavior.

Practitioner validation should ask whether the selectable stroke paths and
course behavior are useful and credible, while preserving individual feedback
and disagreement. It should not ask listeners to certify internal mechanics they
cannot observe. The next work package adds left-hand stopping, where the same
string may be simultaneously plucked, fretted, and damped; that combined state
must remain one physical system rather than several unrelated note voices.
