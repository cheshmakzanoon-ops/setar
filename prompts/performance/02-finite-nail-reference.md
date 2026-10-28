# Work brief: a finite nail-contact reference with an auditable work ledger

## Prompt to give the contributor

Build or review one offline reference experiment for finite nail contact on a
string. Read `AGENTS.md`, the orientation, the relevant numerical energy chapters,
`docs/contacts/01-finite-nail-contact.md`, and
`docs/contacts/02-friction-flesh-direction.md`. Read the exact primary-source
sections linked there when using their method. Do not copy a published update
without reconstructing its notation, units, assumptions, and boundary conditions.

Your goal is not a finished instrument. It is a falsifiable demonstration that
the selected contact law, geometry, and time integrator exchange and dissipate
energy consistently and converge in release behavior. Use C++20 for the core
reference where the repository expects it; offline analysis may use the existing
analysis tools. Keep JUCE out of the reference and do not add recorded-note
playback as synthesis.

## Scope the model before coding

Choose a frictionless normal contact first unless the frictionless reference
already passes. Define signed gap positive under separation, normal direction,
compression coordinate, potential, units, and whether the nail is prescribed or
dynamic. State whether a finite patch is a lumped kernel or resolved quadrature.
All unmeasured material and geometry values must be labeled synthetic.

Write the continuous work balance and the exact discrete balance for the chosen
scheme. The explicit string FDTD stability bound does not automatically cover
your contact update. If using an implicit midpoint/discrete-gradient reference,
derive it as a combined mass-matrix system. Do not inject its force into a
different explicit update and inherit the reference's stability claim.

Specify interpolation and adjoint force injection under the actual mass or
quadrature metric. If the contact moves along the string, include the geometry
terms in relative velocity and actuator work. If its frame rotates, account for
the corresponding kinematics and any torque work. A moving visualization is not
a substitute for this derivation.

## Start with a smaller independent problem

Construct a two-body or small-matrix contact fixture whose equations can be
checked independently. Verify force direction, separation, potential recovery,
and energy balance before coupling a long string. Use analytic limits or a
separately implemented high-accuracy solver as a reference; rerunning the same
code at a smaller timestep is useful but not fully independent.

Then couple one contact to a short verified string representation. Test a path
that never touches, a gentle load-and-release path, and a strongly compressed
but admissible path. Preserve gap, normal force, relative velocity, contact
potential, actuator work, dissipation, and solver residuals at sufficient
resolution to inspect one event.

Do not use an output limiter, amplitude normalization, or state clipping to make
the result appear stable. If a physical or numerical domain limit is exceeded,
stop or reject the case under a documented policy and retain the diagnostic.

## Add finite width and motion deliberately

Define patch width in metres and normalize its interpolation/injection so grid
refinement does not change total force. For resolved quadrature, scale potential
and force by the selected measure so adding contact points does not multiply
stiffness. Demonstrate convergence with spatial and quadrature refinement.

Move the contact along the string and verify the additional control work. Rotate
the contact frame in an isotropic fixture and check the expected transformed
response. Then test release and possible recontact without an arbitrary cooldown
that changes the mechanics. Diagnostic event hysteresis must not alter forces
unless it is an explicit regularization under test.

If adding friction, start with a law whose relative power is nonpositive and
verify that property in an isolated sliding fixture. A stick-state extension
requires its own stored-energy and separation rules. Do not add surface noise
until deterministic mechanics and aliasing are understood.

## Numerical failure and bounded behavior

Record iteration counts, residual norms, conditioning concerns, and the exact
termination criterion. An iteration cap bounds execution but does not make an
unconverged force correct. Exercise at least one deliberate nonconvergence or
out-of-domain case and demonstrate the reported outcome.

For an eventual real-time reduction, measure the approximation against this
reference over a declared domain. Do not claim real-time suitability from one
average timing result. The current task can remain offline if that is the honest
state of the evidence.

## Deliverables and exit gate

Deliver the derivation, synthetic manifest, reference implementation or review
findings, independent comparison, refinement plots, work ledger, and a concise
failure report. Identify the exact source revision and commands used. Rendered
audio may accompany the report, but it does not replace the mechanical evidence.

Acceptance requires correct force signs and units, no unexplained energy,
convergent release time and state over the tested domain, and visible failure
behavior. The result supports a contact solver claim only. Setar nail parameters,
practitioner gesture validity, and full-instrument fidelity remain separate
experiments unless this task actually acquires and evaluates the corresponding
evidence with permission.
