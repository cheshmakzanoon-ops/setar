# P05 — Model fingernail excitation and physical release

**Status:** specification. **Prerequisites:** P04 and verified contact numerics.
**Exit evidence:** bounded contact behavior, correct work transfer, reproducible
release, and gesture-dependent predictions evaluated against relevant evidence.

## What makes this a setar task

The setar's right-hand nail interaction is central to its attack and articulation.
A generic random burst into a delay line is not a measured model of that contact.
At the same time, prescribing an arbitrary elaborate nail geometry without data
does not make the result accurate. Begin with a limited model whose parameters,
observables, and failure modes can be tested.

Read [contacts](../contacts/README.md), [performance](../performance/README.md),
and [measurement](../measurement/README.md). Consult practitioner descriptions
and controlled recordings to establish stroke direction, contact sequence, and
which strings are intentionally contacted. Do not infer the entire gesture from
the audio waveform alone.

## P05-T01: define the contact geometry and kinematics

Specify the string coordinate, nail trajectory, normal direction, contact gap,
and local tangential motion. Decide whether the nail trajectory is prescribed,
driven by a compliant finger model, or measured as an input for a controlled
experiment. Those are different boundary conditions and should be selectable
test configurations rather than mixed implicitly.

Declare how a finite contact region is represented. A point-contact limit is
useful for verification, but the contact width may affect high-frequency
excitation. If an interpolation kernel spreads force across a grid, distinguish
numerical support width from a physical contact patch. Verify that observing
velocity and distributing force are adjoint operations under the chosen inner
product.

Define release geometrically or mechanically. A note-on envelope is not a
physical release rule. The transition must avoid injecting an arbitrary
discontinuity into string state. Store enough contact state to replay the event
deterministically and to distinguish a nail passing over a ringing string from
exciting a silent one.

## P05-T02: establish a passive normal contact reference

Choose a nonpenetration or compliant penalty model with an explicit stored
potential and, if used, dissipative term. Derive force from the potential with
the declared gap sign. Test no-contact, first contact, increasing compression,
unloading, and separation. Penalty stiffness is not merely a brightness knob;
it affects penetration, numerical stiffness, and solver requirements.

Implement the reference solver following the verified numerical contact chapter.
Check the discrete work balance and the small-contact/rigid-contact limits
appropriate to that scheme. Separate numerical convergence from the physical
choice of penalty law. A solver that stays stable by excessive numerical damping
may erase the very attack behavior being investigated.

Include deliberately difficult trajectories and rejected parameter combinations.
The reference run must report residuals and convergence state. The real-time
containment policy is a later engineering requirement; it must not redefine a
failed solve as a normal attack.

## P05-T03: introduce friction only with defined scope

If tangential friction is needed, state whether it represents nail/string
sliding, a simplified effective contact, or another mechanism. Specify static
and sliding regimes, regularization, and the corresponding dissipation. Do not
reuse bowed-string friction laws without evaluating whether their assumptions
apply to a brief nail interaction.

Verify the sign of frictional power and behavior near zero relative speed.
Discontinuities or regularizations can create stiffness and affect attack
spectra. Use energy accounting and trajectory-controlled experiments to isolate
those effects. A parameter sweep should show continuous physical transitions
where the underlying model predicts them, not discontinuities caused by a
branch condition or solver tolerance.

## P05-T04: validate the excitation spectrum

Start with an ideal triangular-displacement pluck as an analytic test of string
observation and spatial excitation. Expected harmonic cancellations depend on
pluck position. Use that limiting case to reveal indexing, normalization, and
boundary errors before interpreting a nail simulation. Do not label the
triangular fixture a realistic fingernail model.

Then compare physical contact outcomes across pluck positions, depths, speeds,
directions, and initial ringing states. Track delivered work, initial modal
distribution, attack time, and high-frequency content. Keep microphone/body
effects out of the isolated-string test. Once coupled, examine whether the same
contact parameters predict held-out attacks rather than refitting each note.

## P05-T05: multiple strings and directional strokes

A sweep may contact several strings at distinct instants and depths. Derive
those times from a trajectory/geometry model or a declared performance
abstraction. Do not fire identical independent note-ons at the same sample and
call that a strum. The shared physical body continues to couple strings even
when only one is directly touched.

Test an inward stroke, outward stroke, a missed string, a partial sweep, and a
repeated pluck while a previous vibration remains. Verify that string state is
not reset on every contact. A contact may add or remove energy depending on
relative motion; the work ledger must explain the outcome. Randomization, if
later added, must represent a documented distribution of gesture variability
and be reproducible from a seed.

## P05-T06: data and acceptance

Use synchronized gesture/force observations where feasible, with clear limits
on what the measurement can resolve. Avoid inferring nail stiffness from an
audio spectrum while simultaneously changing body damping and microphone EQ.
That inverse problem may be underdetermined. Use separate experiments and
sensitivity analysis to choose identifiable parameters.

Accept the component when contact mechanics, work transfer, release, repeated
interaction, and directional behavior pass their tests within a declared domain.
Authenticity needs practitioner and measurement evidence; mathematical contact
verification alone is insufficient.

> Implement the specified nail-contact experiment without resetting the string
> or substituting prerecorded attacks. State the gap/force sign, contact law,
> trajectory, solver, and release rule. Show work and energy before, during,
> and after contact; include no-contact and failed-solve cases. Compare spectra
> at several physical pluck positions and label all synthetic assumptions.
> Do not add uncontrolled humanization to make an invalid model sound natural.

Handoff contact-state and gesture interfaces to P06 and P11. Preserve isolated
tests so later body coupling cannot hide an excitation regression.
