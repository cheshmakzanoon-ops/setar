# Acoustics 01 — Cavity physics and structural–acoustic coupling

## Purpose and implementation status

This proposed, unimplemented P10 task connects the elastic instrument to its internal air and openings. Inputs are inner geometry, soundhole passages, structural surface motion, material and air properties, and a declared frequency range. Outputs are a coupled reference model, interface matrices, energy tests, cavity/structure mode interpretation, and a reduced-loading candidate.

A cavity resonance is not simply another body-filter peak. Internal pressure loads the soundboard and bowl; their motion changes cavity volume and excites the air. Openings connect internal and external fields. Depending on frequency and geometry, cavity and structural motions can hybridize. The task is to establish when a lumped approximation is adequate and when spatial acoustic resolution is required.

## Linear acoustic assumptions

Begin with small pressure and particle-velocity perturbations around a stationary ambient state. Let \(p\) be acoustic pressure in Pa, \(\mathbf u_f\) particle velocity in m/s, \(\rho_0\) ambient density, and \(c\) sound speed. The lossless first-order equations are

\[
\frac{1}{\rho_0c^2}\dot p+\nabla\cdot\mathbf u_f=0,
\qquad \rho_0\dot{\mathbf u}_f+\nabla p=0.
\]

State whether \(\rho_0\) and \(c\) are uniform. Values depend on environmental conditions; use measured conditions or an explicitly labeled assumption. Do not insert convenient constants and later describe them as laboratory measurements.

This model omits mean flow, nonlinear compressibility, and detailed thermoviscous boundary layers. Those omissions may be acceptable over a defined operating range, but aperture losses and narrow passages require separate scrutiny. A linear acoustic model should not be stretched into a claim about every extreme excitation.

## Normals and work signs

At a solid–fluid interface, define \(\mathbf n_s\) pointing outward from solid into the fluid. The outward normal of the adjacent fluid domain is \(\mathbf n_f=-\mathbf n_s\). If structural velocity is \(\mathbf v_s\), continuity gives

\[
\mathbf u_f\cdot\mathbf n_f=-\mathbf v_s\cdot\mathbf n_s.
\]

Fluid pressure exerts traction \(-p\mathbf n_s\) on the solid. Structural interface power is therefore

\[
P_s=-\int_\Gamma p\,\mathbf v_s\cdot\mathbf n_s\,dS.
\]

The acoustic energy equation gives the opposite interface power. This cancellation is the decisive sign test. A diagram with arrows is useful, but the algebraic work ledger is authoritative.

Do not reuse a normal field from an exterior BEM boundary without checking which domain it leaves. Inner and outer soundboard faces have different adjacent fluid domains. A shell midsurface needs a documented mapping to both physical faces.

## Physical acoustic energy

For the first-order field,

\[
E_f=\frac12\int_{\Omega_f}
\left(\frac{p^2}{\rho_0c^2}+\rho_0|\mathbf u_f|^2\right)dV.
\]

Both terms integrate to joules. In the lossless unforced case,

\[
\dot E_f=-\int_{\partial\Omega_f}p\,\mathbf u_f\cdot\mathbf n_f\,dS.
\]

This form makes loading and radiation work explicit. Use it to verify closed-cavity exchange and aperture power.

A pressure-only second-order formulation has a convenient quadratic invariant involving pressure derivatives and gradients, but that invariant is not automatically the physical energy in joules. Do not compare it numerically with structural mechanical energy without deriving the necessary reconstruction or scaling.

## Pressure FEM derivation

Eliminating particle velocity gives

\[
\frac{1}{\rho_0c^2}\ddot p-\nabla\cdot\left(\frac{1}{\rho_0}\nabla p\right)=0.
\]

With pressure basis \(N_p\),

\[
M_p=\int_{\Omega_f}\frac{N_p^TN_p}{\rho_0c^2}dV,
\qquad
K_p=\int_{\Omega_f}\frac{(\nabla N_p)^T\nabla N_p}{\rho_0}dV.
\]

The interface natural derivative follows from momentum:
\((1/\rho_0)\partial p/\partial n_f=-\dot{\mathbf u}_f\cdot\mathbf n_f
=\ddot{\mathbf u}_s\cdot\mathbf n_s\).

Define

\[
C_{sp}=\int_\Gamma N_s^T\mathbf n_sN_p\,dS.
\]

Then one consistent semi-discrete pair is

\[
M_s\ddot q+C_s\dot q+K_sq+C_{sp}p=Bf,
\]
\[
M_p\ddot p+K_pp=C_{sp}^T\ddot q,
\]

with additional aperture and loss terms as required. The apparent asymmetry of acceleration/pressure coupling is a consequence of variable choice, not permission to ignore energy. Verify it against a first-order formulation or a derived coupled energy identity.

## Interface quadrature and nonmatching meshes

Structural and acoustic meshes need not match. Their interface integration must still preserve work. Compute a shared geometric overlap or mortar-like coupling and use its transpose in the conjugate equation. Independently interpolating pressure to structure and velocity to fluid can violate reciprocity and inject energy.

Test constant pressure on a closed surface: the resultant force should vanish for a geometrically closed body, and moments should be consistent. Test a uniform piston patch: integrated volume flow should equal area times normal velocity. These simple checks reveal missing area weights and reversed normals.

Measure interpolation error separately from each domain’s discretization error. Refining only the fluid mesh may not improve a coarse interface map. Store the coupling matrix and its geometry/quadrature hash as an auditable asset.

## Closed cavity benchmarks

Use a rigid rectangular cavity as a first spatial benchmark. Its separable modes provide independent frequencies under known dimensions and rigid boundaries. Include the spatially constant pressure mode and explain its treatment.

A constant pressure offset is not always a disposable gauge. In a sealed compressible cavity, net wall displacement can change mean pressure and store energy. In a purely harmonic nonzero-frequency formulation or a pressure-reference convention, the treatment differs. Specify the physical constraint rather than deleting a singular vector reflexively.

Next couple a piston or elastic plate to the cavity. Compare with a low-dimensional mass–spring–compliance model in the long-wavelength limit. Check both coupled frequencies and energy exchange; matching one resonance can hide a sign error elsewhere.

## Openings and lumped limits

At sufficiently low frequency, a cavity may be approximated by an acoustic compliance \(C_a=V/(\rho_0c^2)\) relating pressure to displaced volume. An opening can contribute acoustic inertance and resistance. Its effective length includes end effects that depend on geometry and nearby boundaries.

This is an approximation requiring validation. Multiple soundholes interact through both internal and external fields; their inertances are not always independent parallel tubes. A thin plate with several small holes differs from one equivalent circular hole even when total area matches.

Keep geometric passage length separate from empirical end correction. If an end correction is fitted, label it effective and state the configuration. Do not count the same exterior added mass once in the correction and again in a coupled radiation impedance.

## Thermoviscous and aperture loss

Boundary-layer thickness scales with frequency and fluid properties. Compare it with passage dimensions before assuming lossless flow. Narrow holes may require a thermoviscous reference, an effective boundary condition, or an experimentally identified passive impedance.

A resistance must dissipate nonnegative power. Frequency-dependent impedance requires a causal realization before time-domain use. A fitted negative resistance is a diagnostic, not a legitimate correction for a poor resonance match.

If nonlinear aperture flow becomes relevant, define the amplitude regime and derive or identify a nonlinear loss law. Do not add a generic saturator to microphone output and call it cavity nonlinearity.

## Hybrid modes and interpretation

Compare uncoupled structural modes, rigid-wall cavity modes, and coupled modes. Track how frequencies and energy partitions change as coupling is introduced. This identifies hybridization without pretending that each measured peak has one pure label.

Energy participation in structure and fluid is more informative than visual resemblance alone. A pressure-dominated mode may have weak bridge participation; a structural mode may radiate weakly despite strong displacement.

Do not label every low-frequency pressure peak a Helmholtz resonance. Verify the spatial pressure pattern, opening flow, and scaling with volume or aperture parameters in controlled models. The name should follow mechanism.

## Mesh and frequency convergence

Refine cavity volume elements, opening geometry, interface quadrature, and polynomial order separately. Acoustic wavelength is a starting resolution scale, not a universal elements-per-wavelength certificate. Dispersion depends on element type and order.

Small openings and curved boundaries can dominate error independently of wavelength. Use local refinement and compare volume flow, pressure, radiation loading, and coupled mobility.

Resolve narrow resonances with adaptive frequency sampling. A fixed coarse grid can miss a coupled pole and falsely suggest a mode disappeared. Track complex response and energy, not only peak locations.

## Coupling task sequence

1. Validate cavity topology and opening connectivity.
2. Freeze air properties and normal conventions.
3. Verify rigid-cavity modes.
4. Verify piston volume-flow and pressure-force units.
5. Assemble the work-conjugate interface matrix.
6. Test a closed coupled lossless fixture.
7. Add passive losses individually and verify energy decrease.
8. Add openings with explicit exterior termination.
9. Refine meshes and interface quadrature.
10. Compare coupled mechanical and acoustic observables.
11. Identify a justified reduced cavity representation.
12. Export matrices, conventions, and uncertainty.

## Acceptance and reading

Require interface-work cancellation, unit tests, closed/open benchmark agreement, converged coupled responses, and a stated treatment of mean pressure. A cavity animation is not sufficient evidence.

COMSOL’s official Acoustic–Structure Boundary documentation provides a primary implementation reference for pressure loading and structural acceleration coupling: https://doc.comsol.com/6.3/doc/com.comsol.help.aco/aco_ug_multiphysics_couplings.14.03.html. Its normal convention must be translated explicitly to this chapter; software terminology such as inward acceleration must not be copied without checking signs.

The next task supplies exterior radiation. Internal air cannot be terminated by an arbitrary zero-pressure opening and then claimed to include realistic radiation loading. That boundary is a useful comparison case, not a universal physical endpoint.
