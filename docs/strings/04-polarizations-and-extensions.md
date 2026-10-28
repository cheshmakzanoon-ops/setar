# Two transverse polarizations and evidence-triggered extensions

## Why a scalar string is a limited starting point

A physical string can move in two transverse directions. A pluck orientation
determines how energy enters those directions, and the bridge can respond
differently to them. A scalar model can be a valid verification fixture, but it
cannot represent arbitrary nail trajectories, orientation-dependent coupling, or
conversion between transverse motions without an explicit reduction argument.
This chapter makes the two-polarization model the proposed interaction baseline
while keeping more expensive degrees of freedom conditional on evidence.

Use `y(x,t)` across the string array in the approximate soundboard plane and
`z(x,t)` away from the soundboard. The local tangent of a curved or deflected
equilibrium need not align exactly with the global `x` axis. At small slopes, the
global frame can be a useful approximation. At larger static deflection or near
contacts, define a local orthonormal frame and transform forces and velocities
consistently. Rotating force but not velocity changes computed power.

For an isotropic uniform string with symmetric ideal boundaries, the two linear
transverse equations have identical coefficients and are independent. Their
frequencies are degenerate. This is a property of that idealization, not a demand
that a real instrument exhibit perfectly coincident polarized modes. Boundary
compliance, contact, construction asymmetry, and geometric nonlinearity can break
the degeneracy.

## Rotation is the first nontrivial test

In an isotropic fixture with rotationally symmetric boundaries, rotate the pluck
direction by a known angle while preserving total excitation work. The total
mechanical response should rotate accordingly. A sensor projected onto one axis
will change, but rotationally invariant quantities should not. This test detects
unequal numerical treatment of the axes, accidental gain differences, and
incorrect vector normalization.

A second test chooses two different bases spanning the same transverse plane.
Represent the same initial state and force in each basis, transform the output
back, and compare. Basis dependence in an isotropic configuration is a defect.
Do not confuse it with real anisotropy introduced by an explicitly directional
bridge. The fixture must separate those two cases.

The initial pluck direction is not simply a stereo pan control. It affects the
mechanical force delivered to body ports, which then affects radiation. Panning
a scalar output can mimic an output-image change while leaving the mechanism
unchanged. Label such an effect as an output effect if it is offered; do not use
it as evidence that polarized string motion exists.

## Bridge coupling as a vector relation

A linear bridge model may relate a vector of forces to a vector of velocities
through a mobility matrix. The matrix can include cross-axis and cross-string
terms. Reciprocity and passivity are physically useful constraints when their
assumptions apply. The exact matrix dimensions depend on which translations and
rotations are retained; a two-axis point port is not automatically a complete
bridge description.

For a passive frequency-domain mobility under a consistent convention, its
Hermitian real part should be positive semidefinite on the relevant force space.
Checking only the diagonal elements is insufficient: cross terms can create an
active combination even when each individual port appears dissipative. The body
volume owns the detailed model. The string volume's obligation is to pass forces
and velocities in the agreed frame with the agreed signs and units.

When measuring only one direction, mark the other matrix elements unknown.
Setting them to zero is a synthetic reduction, not a measurement of zero.
Perform sensitivity tests to decide whether that reduction is adequate for the
current claim. An audible orientation difference does not by itself identify
which matrix element caused it, because the nail and observer also introduce
directional effects.

## Apparent doublets and their alternatives

Two nearby spectral peaks can be consistent with polarized modes, but other
mechanisms produce doublets: neighboring strings, body coupling, amplitude
modulation, changing tension, or analysis leakage. Design experiments that isolate
candidate causes. Damping other strings, changing pluck orientation, measuring
motion along two axes, and repeating at low amplitude can help, provided the
interventions and their own effects are documented.

A mono microphone cannot uniquely reconstruct two-dimensional string motion.
It observes a radiated mixture influenced by the body and room. If an estimator
claims a polarization angle from mono audio, treat that as model-dependent
inference and test it on synthetic data with known ground truth before applying
it to an instrument. Even synthetic success may rely on conditions absent in
real recordings.

Use non-contact displacement measurement when feasible, or document sensor mass
and attachment effects if contact sensing is necessary. A sensor mounted on a
light string may change precisely the modes being studied. A beautiful measured
trace is not necessarily an undisturbed trace.

## When longitudinal motion becomes justified

Transverse motion can change axial strain. A simple quasi-static tension
correction captures some effects without resolving longitudinal waves. A fully
dynamic longitudinal degree of freedom becomes useful when measured or reference
behavior requires finite-speed axial propagation, identifiable longitudinal
resonances, or coupling that a quasi-static reduction cannot predict.

Do not add longitudinal state merely because a continuum mechanics textbook
contains it. State the observable discrepancy first. Examples include a
repeatable spectral feature that moves with the expected axial-wave scaling, or
an amplitude-dependent transient that remains unexplained after contact and
measurement artifacts are controlled. Then specify the measurement needed to
distinguish the dynamic axial model from the simpler alternative.

The longitudinal model needs additional parameters, boundary conditions, and
coupling terms. Its wave speed can impose a different numerical stiffness than
transverse motion. The transverse solver's time-step bound cannot be reused.
An implicit update may remove one explicit stability restriction but still
requires accuracy, nonlinear convergence, and energy analysis. Cost must be
measured in worst-case prepared configurations, not extrapolated from one string
at one tension.

## When torsion becomes justified

A tangential contact force applied away from the string centre can exert torque.
Torsional motion may then affect surface relative velocity and friction. A
torsional model requires angular displacement, rotational inertia per length,
torsional rigidity or an identified effective law, torque boundary conditions,
and torque work. Its power port is torque times angular velocity, not force
times translational velocity.

Whether this matters for the target setar is an empirical question. Look for a
failure of the translational contact model under controlled changes in contact
orientation, string construction, or surface conditions. Avoid assigning every
high-frequency attack discrepancy to torsion. Nail roughness, finite contact
width, fret collisions, sensor bandwidth, and numerical aliasing are competing
explanations.

If torsion is retained, couple surface velocity consistently. A radius or lever
arm transforms angular velocity into tangential surface velocity; it also
transforms friction force into torque. Using different lever arms in those two
places violates power consistency. Test the isolated transformation before
embedding it in a full instrument. For composite strings, the effective torsional
parameters may not follow a homogeneous-cylinder formula.

## Geometric and constitutive anisotropy

An isotropic circular string is a useful model class. If the actual construction
is asymmetric or composite, two bending rigidities and coupling terms may be
needed. Begin with observed evidence, not a speculative collection of parameters.
Unidentifiable anisotropy can fit almost any small spectral discrepancy while
making predictions unreliable.

Constitutive anisotropy and boundary anisotropy should be tested separately.
Rotating a specimen relative to a controlled fixture can be informative if safe
and feasible. Reinstalling a string, however, can also change tension, seating,
and wear. The experimental design must record these changes rather than treating
rotation as the only intervention.

Parameter uncertainty should propagate into predictions of polarization mixing.
If a tiny uncertain cross term determines the entire output orientation, report
that fragility. A user-facing control should not imply precise control of an
unidentified physical mechanism.

## A hierarchy with explicit promotion rules

The proposed hierarchy begins with a scalar verification fixture, then two
transverse fields, then nonlinear geometric tension if needed, then dynamic axial
or torsional state only when a falsifiable discrepancy warrants it. Promotion is
not irreversible. A more elaborate model that cannot be identified or executed
within the required budget may remain an offline research model.

For each promotion, preserve the simpler model as an ablation. Compare the same
inputs, output definitions, and parameter provenance. More degrees of freedom
should improve a withheld observable, not merely lower training error. Record
whether the improvement survives uncertainty in the measurements and whether it
matters perceptually under the intended listening conditions.

The real-time engine may use a reduced model derived from the richer reference.
That reduction needs its own domain of validity. A reduction valid for isolated
soft plucks may fail during repeated capture, aggressive stopping, or multi-string
sweeps. The validation set must include the interaction classes for which the
reduction is advertised.

## Work package and acceptance evidence

Inputs are the validated scalar model, coordinate transforms, bridge-port
contract, and selected excitation trajectories. Outputs are two-axis state,
vector force interpolation, rotational-invariance tests, and a report comparing
observed directional behavior with the model. No JUCE type is required in this
layer; controller and host conventions are resolved before physical forces reach
it.

Acceptance for two transverse fields requires independent-axis behavior in an
uncoupled fixture, rotation covariance in an isotropic fixture, and correct work
under transformed ports. Acceptance for an extension additionally requires an
identified discrepancy, identifiable parameters, independent numerical tests,
and withheld evidence of improved prediction. A richer state vector without that
evidence is a proposed experiment, not an established fidelity improvement.

If the tests fail, reduce to the smallest failing configuration. A basis-transform
error should not be compensated by a fitted bridge coefficient. A missing torque
unit should not be hidden by rescaling a friction coefficient. Only after those
errors are resolved should the team proceed to the nonlinear tension and contact
chapters.
