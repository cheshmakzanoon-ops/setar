# P08 — Verify the elastic body and its reduced representation

**Status:** specification. **Prerequisites:** P07. **Exit evidence:** a converged,
traceable structural model and a reduced representation whose errors are bounded
for the intended ports and frequency range.

## Define the structural model before the mesh

Choose the continuum assumptions explicitly: shell, solid, beam, or a mixed
description; small or large displacement; prestressed or unstressed equilibrium;
linear or nonlinear constitutive behavior. The choice follows thickness, joint
behavior, loading, and observations rather than which element type is easiest
to click in a solver. Read [body modeling](../body/README.md),
[geometry](../geometry/README.md), and [numerics](../numerics/README.md).

Keep the structural and acoustic responsibilities distinct at this stage. A
structural displacement field is not a microphone signal. Its validity is
assessed through structural observations and work before P10 adds cavity and
radiation behavior.

## P08-T01: verify the element and assembly pipeline

Use independent elementary problems before the instrument geometry. Check rigid
translations and rotations, constant-strain or bending patch behavior where
appropriate, mass totals, and known beam/plate frequencies within each element's
assumptions. State the constraints and analytical reference; do not compare two
solvers that share the same undocumented boundary mistake.

Audit element orientation, numerical quadrature, thickness interpretation,
constitutive rotations, and assembly indexing. Check matrix symmetry and the
expected nullspace. A free body has rigid-body modes; eliminating them by adding
arbitrary weak springs changes the problem. If stabilization is necessary,
quantify its effect and keep it out of claimed physical damping.

Exercise deliberately invalid elements and materials. Negative Jacobians,
degenerate cells, nonpositive density, and inadmissible elasticity should produce
specific diagnostics before a modal solution is accepted. A solver's successful
exit code is insufficient evidence of an admissible model.

## P08-T02: establish equilibrium and supports

Represent gravity, string loads, contact preload, and mounting conditions only
where they belong to the intended experiment. Reproduce the supports used in
the measurement session. A freely suspended modal test, a player-held instrument,
and a bench-mounted object have different boundary conditions.

If a prestressed analysis is needed, converge the static equilibrium first and
verify reaction forces and deformation. Linearize about that state using a
documented tangent operator. Do not add a geometric-stiffness correction with
an unexplained sign. Check that reducing the load approaches the corresponding
unstressed model and that the selected equilibrium is physically meaningful.

## P08-T03: converge the observables that matter

Refine mesh and approximation order while holding the physical model fixed.
Compare not only natural frequencies but mode shapes, port residues, and bridge
mobility. A frequency can converge while a local force/velocity response near a
small footprint remains inaccurate. Treat the bridge loading patch as a
physical region, not a single convenient mesh node whose meaning changes under
refinement.

Track mode correspondence using appropriate shape/mass metrics rather than
frequency sorting alone. Near-degenerate modes may rotate within a subspace;
evaluate the subspace and its port response instead of declaring arbitrary
eigenvector differences a failure. Preserve sign-invariant comparison where
eigenvector sign is physically arbitrary.

Document the selected band and residual effect of omitted higher modes.
Convergence within that band does not justify unlimited sample-rate output.
Check static compliance and low-frequency response as well as resonance peaks;
a truncated model can reproduce visible peaks yet have incorrect off-resonance
behavior.

## P08-T04: identify and constrain losses

Introduce damping through a representation with stated assumptions and evidence.
Modal decay coefficients, proportional damping, and frequency-dependent
constitutive loss are not interchangeable. Compare their predicted decays and
frequency response over the intended amplitude and environment range.

Keep structural dissipation separate from acoustic radiation and joint losses
until their combined identification is defensible. Fitting total measured decay
as structural damping and then adding a radiation model can double-count energy
loss. Record what each coefficient includes and which conditions it was fitted
under. Reject negative dissipative parameters unless a genuinely active physical
subsystem has been explicitly modeled, which is not the default here.

## P08-T05: derive a port-preserving reduced model

Choose physical force/velocity ports before reduction. Normalize modes with the
correct mass convention and retain the corresponding force projection and
velocity observation. For a second-order structural model, the input and output
maps must preserve their power pairing. Arbitrary rescaling of one map changes
coupling even if isolated frequency plots look unchanged.

Compare the reduced model with the full model on impulses, sweeps, static or
quasi-static compliance where meaningful, and multiport reciprocity. Evaluate
truncation error at each intended bridge/cavity port, not just a global norm.
Any residual flexibility or high-frequency correction must have its own
stability/passivity argument and tests.

Store reduction provenance: full-model identity, basis, normalization, retained
band, damping treatment, input/output maps, and validation cases. A list of
frequencies without masses/residues and port definitions is not an installable
body model.

## P08-T06: define safe asset loading

Design the reduced-model asset schema for offline validation and bounded runtime
use. Check dimensions, units, finite values, symmetry where required, mode counts,
energy-related positivity, version, and hashes before installation. Preparation
must leave the current valid model unchanged if a new asset fails validation.

Treat malformed or incompatible files as data errors with actionable messages.
Never deserialize arbitrary executable content. Keep expensive eigenanalysis,
mesh loading, and matrix factorization outside the audio callback. Document the
memory/cost envelope of the accepted reduced representation for P13.

## Exit review

Require separate reviews of physical assumptions, numerical convergence, and
reduction accuracy. Attach actual mesh/shape plots with units, deformation scale,
boundary markings, and synthetic/measured labels. A dramatic modal animation
does not establish correct material properties.

> Construct and verify the specified body model without importing quarantined
> elastic constants. Demonstrate element-level checks, boundary equilibrium,
> mesh convergence, mode/subspace matching, and bridge-port response. Derive a
> reduced asset with explicit mass normalization and power-paired maps. Compare
> full and reduced responses, disclose omitted physics, and reject invalid
> assets before they can reach real-time processing.

Hand the validated port contract to P09 and the surface-motion representation
to P10. Preserve full-model fixtures so later optimization remains testable.
