# Finite fingernail contact and physically meaningful release

## Replace a trigger with an interaction

A fingernail pluck is not fully described by an impulse amplitude. During loading,
the string moves against a finite surface, reflects waves from its boundaries,
may slide, and eventually separates. Its displacement and velocity at release
depend on that history. A triangular initial displacement remains a valuable
linear benchmark, but it is a different experiment from a finite moving nail.

The proposed interaction baseline uses a prescribed or dynamically driven nail
surface and two transverse string fields. Its purpose is to predict a release
state from a contact trajectory and material assumptions. This is a design
contract. The repository's actual implemented capabilities must be checked before
claiming the feature exists.

Prerequisites are the verified linear string, two-axis frame conventions,
power-consistent interpolation, and an offline nonlinear reference. Inputs are
nail geometry, a motion or actuator command, contact parameters with provenance,
the current string state, and a prepared numerical domain. Outputs are forces,
contact state, actuator work, dissipation, and the continued string response.

## Define a signed gap once

Let `g` be positive when the string and nail are separated, zero at contact, and
negative when a compliant or penalty model permits overlap. Let the local normal
`n` point in the direction that increases `g` for a displacement of the string.
The compression coordinate is `eta = max(0, -g)`. With a simple power-law contact,

\[
\Phi(\eta)=\frac{K}{\alpha+1}\eta^{\alpha+1},
\qquad F_n=K\eta^\alpha.
\]

The elastic force on the string is `+F_n n`, and the opposite reaction acts on
the nail. `K` has units `N/m^alpha`; it is not always a spring constant in `N/m`.
The proposed baseline requires `K > 0` and `alpha >= 1`, with zero elastic force
under separation. Other exponents need their own regularity and solver analysis.
The exponent and coefficient are model parameters to identify or deliberately
synthesize. They must not be inferred from the word fingernail alone.

This force is the negative derivative of the potential with respect to gap.
In the continuous frictionless closed system, that pairing permits a consistent
energy balance. It does not prove that a chosen numerical force evaluation is
stable. The discrete implementation must use the numerical framework's exact
force, state, and work definitions.

## Point contact versus a finite patch

A single point is a reduction. A finite nail surface can contact a short string
region, and that region can move during the stroke. There are at least two useful
model levels: a lumped contact acting through a normalized spatial kernel, and a
set of quadrature contacts with distinct gaps along a resolved surface. The
first is cheaper but cannot represent every contact-shape change of the second.

For a lumped contact, the kernel's physical width is specified in metres. It must
not silently become narrower as the grid is refined by always occupying one
node. Normalize interpolation and force distribution consistently so a force in
newtons remains the same physical force at different resolutions. The exact
mass or quadrature weighting belongs in the port derivation.

For a resolved patch, each quadrature element contributes potential, force, and
work under a defined measure. Increasing quadrature density should converge to
the same surface interaction rather than multiply stiffness. Check this before
fitting a contact coefficient. A parameter that compensates for the number of
quadrature points is not a material parameter.

## Nail motion and driver choice

A prescribed nail trajectory is an ideal actuator. It can supply or absorb work
as required to follow its path. That is useful for repeatable experiments, but it
is not a complete model of a human finger. Record its work and ensure the
trajectory is within the intended interaction domain. A rigid prescribed surface
can generate large forces if commanded through an immovable string.

A dynamic nail model instead gives a mass or inertial representation, compliance,
and an actuator that drives it. This can represent the string reacting back on
the finger, but it introduces more parameters and possible numerical stiffness.
Promote to it only when the prescribed-trajectory model fails a relevant
observable or when feedback to the performer is itself the research question.

The driver should define position, velocity, orientation, and any higher-order
continuity needed by the chosen integration. A sequence of discontinuous position
commands is not equivalent to a smooth hand motion. If a controller is sampled
coarsely, reconstruct a bounded trajectory with documented interpolation rather
than differentiating noisy positions into unbounded velocities.

## Moving interpolation carries work

If a contact samples the string through a time-dependent interpolation row
`I(t)`, its sampled displacement is `I(t) u(t)`. Its derivative includes both
`I(t) u_dot(t)` and `I_dot(t) u(t)`. The second term matters when the contact
location moves along the string. Omitting it can make a sliding actuator perform
unrecorded work.

Likewise, a rotating normal contributes geometry terms to the gap derivative.
Compute the relative normal velocity from the derivative of the actual gap
function, not from an unrelated projection chosen for convenience. If the nail
orientation is dynamically represented, the reaction torque and rotational work
must be included. If orientation is prescribed, the external control-work
ledger must include its effect.

Pair interpolation with its adjoint force injection under the chosen mass or
quadrature metric. A test should verify equality between generalized force work
and contact-port work for arbitrary small states. This algebraic test catches
errors before they appear as subtle attack differences in full audio.

## Release is separation, not a forced envelope

In the geometric model, release occurs when the surface no longer supports a
compressive interaction and the evolving trajectory separates. The string then
continues from its current displacement and velocity. Do not replace that state
with a predesigned waveform at the release timestamp. Doing so discards the
history the contact solver just computed.

A state machine may label approach, loaded contact, sliding, separation, and
recontact for diagnostics. Those labels should reflect the solved geometry and
forces. A user command can request the nail to move away, but an arbitrary
fixed-duration gate should not declare release while the geometry still
penetrates. If a simplified release scheduler is used, label it as a reduced
model and compare it with the geometric reference.

Recontact is physically possible in a finite-trajectory model. Suppressing it
with a long cooldown can improve numerical behavior while deleting real
interaction. First determine whether observed repeated contact is physical or
numerical chatter. Diagnostic hysteresis may prevent event-log flicker, but it
must not silently alter forces unless that regularization is part of the model.

A discrete-gradient contact force represents work across a whole timestep. It
can be nonzero even when the final gap is positive, because contact occurred
during that interval. Do not zero the step force solely from the final endpoint's
separation; doing so can destroy the discrete work identity. Use the selected
scheme's contact definition and timestep refinement to assess release-time
accuracy, while keeping endpoint contact labels distinct from interval work.

## Damping and nonadhesion

A simple deliberately dissipative candidate adds a compression-only normal
damping force proportional to `eta^beta * max(-g_dot, 0)`. Its relative power is
nonpositive during compression, and it vanishes during separation. This is one
model choice, not a universal material law. Other viscoelastic laws can include
unloading dissipation but need careful unilateral treatment to avoid unintended
attractive forces.

For this candidate use a nonnegative damping coefficient and `beta > 0`, and
define the damping force as zero when compression is zero. Those restrictions
avoid an undefined or singular zero-compression power and make the stated
separation limit explicit. Their discrete realization still needs the combined
scheme's work test.

If a damping formula produces a tensile normal force when the surfaces should
be separated, do not silently clip it and retain an energy proof derived for the
unclipped law. Either derive the unilateral law explicitly or use a model whose
admissibility is clear. State whether adhesion is intentionally excluded.

Contact stiffness should not be increased without limit to approximate rigidity
while keeping the same numerical settings. Penetration error, nonlinear solver
conditioning, timestep accuracy, and high-frequency content all change. Report
the tradeoff and the convergence evidence for the selected range.

## Verification sequence

Start with two lumped bodies and a fixed normal. Verify force direction,
separation, potential recovery, and the work ledger. Then couple one contact to a
small string reference with known mass and stiffness matrices. Compare a
contact-free trajectory, a gentle loading/release trajectory, and a strongly
compressed but admissible trajectory.

Add finite patch width and demonstrate spatial/quadrature convergence. Then move
the contact along the string and verify the extra control work. Finally rotate
the surface and compare with a transformed equivalent fixture. Each addition
has a separate failing case so a defect can be localized.

For repeated plucks, initialize the string with controlled phase and amplitude
before the nail arrives. Verify that the contact result changes appropriately
with the existing motion and agrees with the reference. A production path that
restarts the string will fail this experiment even if isolated attacks sound
plausible.

## Evidence gate and next action

Acquire contact observations through the shared [pluck, contact, and motion
protocol](../measurement/pluck-contact-and-motion.md). Its synchronization,
annotation, and uncertainty fields are the inputs to a parameter fit. An inferred
force from a motion model must remain distinct from a calibrated direct force
measurement.

Required evidence includes bounded penetration under the chosen compliant model,
convergence of release time and release state, a closed energy/work ledger within
declared numerical tolerances, and deterministic failure behavior. Parameter
records must identify which values are measured, inferred, published, or
synthetic. A screenshot of a moving nail does not satisfy these requirements.

The model's acoustic validity remains open until observed contact and released
motion are compared with a real setar interaction. The next chapter adds friction
and flesh transitions only after the frictionless geometry and energy tests pass.
If the baseline is unstable, no amount of friction tuning or output limiting is
an acceptable substitute for correcting it.
