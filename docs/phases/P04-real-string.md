# P04 — Add stiffness, loss, polarization, and justified nonlinearities

**Status:** specification. **Prerequisite:** P03. **Exit evidence:** each added
mechanism has independent verification, admissible parameters, and an explicit
validity range. “More complex” is not an acceptance criterion.

## Reading and physical scope

Read the [string volume](../strings/README.md) and the exact
[numerical schemes](../numerics/README.md) before implementation. Use verified
primary research for physical mechanisms and parameter-identification methods.
Separate a property of a particular string from a convenient approximation
used by a model.

The first stiff-string reference uses bending rigidity EI and linear density
mu. For simply supported endpoints, its partial angular frequencies satisfy
omega_n squared = (T/mu)(n pi/L) squared + (EI/mu)(n pi/L) to the fourth power.
This analytic case is a test fixture, not proof that actual nut, fret, and
bridge boundaries are simply supported.

## P04-T01: stiffness and dispersion

Add bending stiffness without changing the existing flexible-string limit.
Derive the spatial operator and the required boundary terms. Verify units of
Young's modulus, area moment, and bending rigidity; wire diameter enters the
area moment strongly, so a diameter/radius mistake causes a large error.
Record how diameter and linear density are measured or specified.

Test zero-stiffness convergence to P03, exact stiff-mode frequencies, and boundary
conditions. Compare multiple resolutions across the usable partial band. Do
not compensate numerical dispersion by fitting a different physical stiffness
unless the approximation and compensation are explicitly separated. Otherwise
an inferred wire material becomes a hidden solver-error parameter.

If an all-pass or waveguide approximation is explored, compare its phase/group
delay to the same continuum target and preserve the reference model for
validation. That alternative is a separate decision, not an undocumented swap
inside the same class name.

## P04-T02: physical and empirical loss models

Classify losses by mechanism and evidence: air interaction, internal material
loss, termination loss, bridge/body energy transfer, and finger contact should
not all be folded into one exponential unless the limited model is declared.
Some loss mechanisms can be represented with causal frequency-dependent
operators or fitted modal decay rates. Identify which representation is being
used and how it will couple to the body later.

For the chosen damped PDE, derive the loss discretization together with the
wave update. A backward-time frequency-dependent loss term can change the
overall temporal order even when the wave terms are centered. Use the bound
and energy expression belonging to that exact scheme. Do not cite a centered
scheme's second-order accuracy for a different implementation.

Test nonnegative dissipation, analytic modal decay where available, the lossless
limit, and behavior at the parameter-domain edges. An experimentally fitted
negative damping coefficient must be rejected or interpreted as a failed
identification, not used to simulate an active string. Keep termination loss
separate enough to avoid counting it twice when body coupling is enabled.

## P04-T03: two transverse polarizations

Introduce two transverse displacement components with a declared physical basis.
For an ideal isotropic string with identical boundaries, rotating the basis
should not change observable energy or equivalent motion. Use that symmetry as
an independent test. Actual bridge/fret geometry can make the two directions
respond differently; document where the anisotropy enters.

Excite each direction separately and together. Verify that a change in pluck
direction maps into physically meaningful components rather than arbitrary
left/right stereo panning. Radiation is a later observation problem. Couple
the components only through explicitly modeled mechanisms; otherwise apparent
beating may be an accidental mismatch in duplicated parameter arrays.

Prepare the interface for directional bridge admittance. Force/velocity vectors
must use the same coordinate frame as the body ports. Test frame transforms
with orthogonal matrices and verify that dot-product power is invariant.

## P04-T04: nonlinear tension and longitudinal effects

Decide what observation requires additional nonlinearity: pitch rise at strong
excitation, amplitude-dependent frequency, polarization exchange, or another
measured effect. Write the associated assumptions about longitudinal motion,
quasi-static stretching, tension, and boundary work. Do not add nonlinear terms
only because high-fidelity instruments are expected to be complicated.

Derive the stored energy and gradient of the selected extension. Verify the
small-amplitude limit against the linear model. Use controlled amplitude sweeps
to compare predicted frequency shift and energy behavior. Distinguish an actual
amplitude-dependent frequency shift from estimator bias caused by a changing
spectrum or short observation window.

A nonlinear solver needs a bounded, observable convergence policy. Record
residual definitions, scaling, iteration limits, and failure behavior. The
offline reference may use robust implicit methods that are too expensive for
the eventual callback; that is acceptable if the interface and comparison plan
are explicit. Never silently continue with an unconverged iterate.

## P04-T05: decide whether torsion belongs in scope

Torsional motion may matter for particular contacts or strings, but adding it
requires excitation, boundary, coupling, and measurement evidence. Define a
test that can distinguish its contribution from missing damping or contact
geometry. If there is no identifiable observable in the current domain, record
it as a research extension instead of adding uncalibrated degrees of freedom.

For any accepted torsional model, verify rotational inertia units, torque/angular
velocity power, wave speed, termination conditions, and coupling to nail/fret
friction. Do not connect a torque directly to a translational force port.

## P04-T06: identify parameters without confounding mechanisms

Use the measurement plans to isolate string properties before full-body fitting.
Record mounting compliance and environmental conditions. Fit partial frequencies
and decay envelopes with uncertainty and inspect residual patterns. A fit that
works for one note but predicts impossible material values or fails at another
length is not a verified physical explanation.

Run sensitivity experiments: vary one admissible parameter around its estimate,
observe which metrics change, and identify combinations that the current data
cannot distinguish. Report those identifiability limits to P12. Store parameter
uncertainty and provenance with the preset/model asset rather than as a detached
spreadsheet that cannot be traced to the engine.

## Completion and implementation prompt

The component gate requires limiting cases, independent mode/decay tests,
energy/dissipation evidence, frame-invariance tests, convergence studies, and
parameter-domain rejection. Every extension must improve a stated prediction
or enable a later verified interaction; unused complexity should remain optional.

> Add only the specified physical extension to the verified string baseline.
> Explain the observation motivating it, derive its units and energy/work
> contribution, and identify the numerical scheme used. Preserve the previous
> limiting case. Add tests that fail if the extension is accidentally doubled,
> omitted, or has its sign reversed. Report identifiability limits and do not
> adjust body or output EQ to conceal a string-model discrepancy.

Handoff directional string ports to P05/P06 and P09. Handoff admissible parameter
ranges and unresolved identification questions to P12.
