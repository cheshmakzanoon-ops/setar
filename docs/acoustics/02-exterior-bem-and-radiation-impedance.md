# Acoustics 02 — Exterior BEM and radiation impedance

## Objective and proposed status

This unimplemented P10 workflow computes how vibrating surfaces and openings load the instrument and produce pressure in the surrounding air. Inputs are exterior geometry, normal velocity basis functions, air properties, openings, and the intended observation region. Outputs are radiation impedance, pressure transfer functions, directivity, power checks, and convergence evidence.

Two products must remain distinct. Radiation loading feeds back into structural and cavity motion. Radiation observation maps that motion to pressure at a microphone or in the far field. A pressure transfer at one point is not a substitute for the load impedance distributed over the surface.

## Harmonic convention and outgoing waves

Use \(e^{i\omega t}\) in this volume. In a homogeneous unbounded fluid,

\[
\nabla^2p+k^2p=0,\qquad k=\omega/c,
\]

and an outgoing free-space Green function is

\[
G_k(\mathbf x,\mathbf y)=
\frac{e^{-ik|\mathbf x-\mathbf y|}}{4\pi|\mathbf x-\mathbf y|}.
\]

Some libraries use the opposite time convention and therefore the conjugate spatial exponential. Read their definitions and convert consistently. Mixing conventions changes reactive signs and propagation phase while leaving many magnitude plots unchanged.

On a surface normal pointing from the solid into exterior fluid,

\[
\frac{\partial p}{\partial n_s}=-i\omega\rho_0v_n.
\]

Define whether the solver’s boundary normal is this solid normal or the outward normal of the exterior fluid domain, which points oppositely at the obstacle. Do not repair a negative radiation resistance by flipping signs experimentally until plots look plausible; derive the convention and verify an analytic fixture.

## Why boundary elements are attractive

For homogeneous exterior acoustics, BEM discretizes the radiating boundary rather than a large air volume. The outgoing Green function encodes unbounded propagation. This avoids a finite exterior boundary and its reflection error, but introduces dense or hierarchically accelerated operators, singular quadrature, and integral-equation conditioning questions.

BEM is not automatically more accurate than FEM or FDTD. It is attractive when surface geometry is available and the exterior medium is suitably modeled by a Green function. Heterogeneous air, complex nearby rooms, or strongly nonlinear propagation can favor other approaches.

Keep the first reference problem small enough for an independently inspectable dense solve. Add fast multipole or hierarchical acceleration only after the dense formulation and quadrature pass benchmarks. Otherwise approximation error and formulation error become difficult to separate.

## Operator spaces and discretization

Choose pressure and normal-derivative approximation spaces according to the integral formulation. Continuous and discontinuous bases are not interchangeable details. Weak and strong forms involve mass matrices and dual spaces; multiplying discretized operators as ordinary arrays can be wrong if the basis mappings are ignored.

The Bempp Handbook’s “Boundary Operators” section documents domains, ranges, dual spaces, weak forms, and Helmholtz operators: https://bempp.com/handbook/api/boundary_operators.html. Its “Potential Operators” section describes evaluation away from the boundary: https://bempp.com/handbook/api/potential_operators.html. Treat these as concrete API/formulation references, not an assertion that a ready setar workflow exists.

Preserve surface orientation, element order, and physical groups. Closed-surface normal consistency should be tested before operator assembly. An isolated flipped triangle can corrupt loading and produce local singular behavior without an obvious silhouette defect.

## Nonuniqueness and conditioning

Some exterior boundary-integral formulations develop fictitious interior-resonance difficulties. A frequency sweep may then show spikes that are numerical formulation artifacts, not instrument resonances. Use a formulation designed to avoid the relevant nonuniqueness, such as an appropriate combined formulation, and verify it near known interior eigenfrequencies of a test shape.

Do not identify a resonance solely because a matrix condition number increases. Physical resonances, integral-formulation irregularities, mesh errors, and poor preconditioning can all affect conditioning. Compare independent formulations and analytic benchmarks.

If a combined formulation introduces a coupling parameter, document its dimensions and frequency scaling. Copying a numerical constant from a differently normalized example can change the formulation substantially.

## Singular and near-singular integration

Self and neighboring panels require appropriate singular quadrature. A microphone very near the surface requires accurate near-singular evaluation even if the boundary solution is converged. Increasing the number of surface elements without changing evaluation quadrature may not fix a close-field error.

Separate boundary-solve convergence from observation-evaluation convergence. Move a synthetic observer progressively closer to a sphere and compare with the analytic field. This exposes quadrature problems that far-field checks miss.

At edges and openings, pressure or derivative behavior can be less smooth. Refine locally and choose compatible basis spaces. Do not smooth real soundhole geometry merely to make a solver converge without reporting the altered physical problem.

## Radiation impedance as a work map

Expand normal velocity using basis functions \(w_j(\mathbf x)\):

\[
v_n(\mathbf x)=\sum_j w_j(\mathbf x)\alpha_j.
\]

Choose \(\alpha_j\) with declared units, for example m/s when \(w_j\) is dimensionless. Define generalized fluid effort

\[
F_i=\int_\Gamma w_i(\mathbf x)p(\mathbf x)\,dS.
\]

Then \(F=Z_{\mathrm{rad}}\alpha\), and the mean power entering the fluid for peak phasors is

\[
\overline P=\tfrac12\operatorname{Re}(\alpha^HF)
=\tfrac12\alpha^H
\frac{Z_{\mathrm{rad}}+Z_{\mathrm{rad}}^H}{2}\alpha.
\]

The structure receives the opposite effort. With this projection, radiation resistance is the Hermitian part of the impedance. It must be positive semidefinite for a passive exterior problem within numerical tolerance. Reactive loading can have either sign according to frequency and convention; it does not itself represent dissipated power.

Do not use an unweighted nodal pressure vector as generalized force. Surface integration and basis scaling are essential. If coefficients represent volume velocity instead, impedance units and projection weights change.

## Analytic pulsating-sphere test

For a sphere of radius \(a\) with uniform outward radial velocity \(v_0\), the surface specific impedance under this convention is

\[
Z_{\mathrm{spec}}=\rho_0c\frac{ika}{1+ika}.
\]

Its real part is nonnegative and tends to \(\rho_0c\) as \(ka\) becomes large. At small \(ka\), reactive behavior dominates and resistance is small. The volume-velocity impedance divides by \(4\pi a^2\).

Use this fixture to test pressure amplitude, phase, surface power, far-field power, and units. Evaluate several values of \(ka\), including a low-frequency case where cancellation and conditioning are challenging. Do not test only one frequency where a sign error can be hidden by magnitude comparison.

For a sphere, pressure decays as \(1/r\) with outgoing phase. Compare two radii and directions. An implementation that reproduces surface impedance but gives the wrong propagation delay still fails the observation contract.

## Power balance and reciprocity

Integrate outward intensity over a surrounding sphere and compare with power supplied at the radiating boundary. Include fluid absorption if modeled. This is stronger than checking one microphone.

Reciprocity should appear in consistently normalized source/receiver or surface-port maps. A raw matrix may need mass-weighting before symmetry is meaningful. Derive the discrete bilinear work relation rather than demanding ordinary symmetry of every operator array.

Test simultaneous surface patterns, not just one mode at a time. The radiation-resistance matrix contains interference terms. Two individually radiating patterns can combine into a weakly radiating pattern through cancellation; treating their powers independently misses that effect.

## Cavity openings and exterior coupling

Soundholes are not simply extra vibrating solid panels. They connect fluid domains and require continuity or an aperture representation. Their volume flow and pressure difference belong to an acoustic port with power \(pQ\).

If the cavity FEM and exterior BEM use different aperture meshes, preserve the pressure/flow work pairing. Verify a simple duct or cavity-opening fixture before using the irregular instrument.

Avoid double counting end corrections. A lumped aperture model calibrated with exterior added mass should not retain the same correction when a full exterior field supplies that added mass explicitly.

## Baffled approximations and direct string radiation

A Rayleigh-type baffled-plane approximation can be useful for a planar radiator, but a setar is not an infinite baffle. Its bowl, neck, and openings can scatter and cancel radiation. Compare the approximation against closed-surface BEM in the band where it is proposed.

Direct string radiation may be small relative to body radiation under some conditions, but the omission should be tested or bounded. Do not silently include it in a fitted microphone transfer while claiming the transfer represents body radiation only.

Separate each source contribution for diagnostics, then combine complex pressures before calculating magnitude or power. Adding magnitudes destroys interference.

## Frequency and spatial sampling

Use structural modes, reduced surface patterns, or aperture-flow basis functions as radiation sources. The basis must span relevant surface motion; a few point monopoles chosen visually may not.

Refine the frequency grid where impedance or observation transfer varies rapidly. Retain phase and delay information. For far-field directivity, use an angular quadrature with documented weights and sufficient order for the maximum spatial complexity.

Convergence must include surface mesh, basis order, quadrature, linear solve, frequency sampling, and angular sampling. Report each separately where feasible. One global error number can conceal a close-field failure or a narrowband nonpassivity defect.

## Procedure and gate

1. Verify geometry, normals, and harmonic convention.
2. Solve pulsating-sphere and scattering benchmarks.
3. Audit weak/strong-form basis mappings.
4. Verify nonuniqueness handling near test interior resonances.
5. Construct work-conjugate surface ports.
6. Compute radiation loading and observation separately.
7. Check resistance positivity and reciprocity.
8. Check boundary-to-far-field power balance.
9. Couple apertures and compare with cavity benchmarks.
10. Refine all relevant discretizations.
11. Export complex matrices with units and conventions.
12. Fit a passive runtime load and a causal observation map.

Acceptance requires analytic agreement, converged power balance, passive loading, and a documented observation region. A visually convincing directivity plot is not sufficient.

The next chapter supplies an independent time-domain exterior route. Agreement between BEM and FDTD on the same simplified fixture helps locate formulation and boundary errors, but agreement between two implementations sharing the same mistaken geometry is not specimen validation.
