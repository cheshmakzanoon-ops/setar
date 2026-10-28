# Body 05 — Eigenmodes, normalization, and tracking

## Purpose

This proposed, unimplemented P08 task turns verified structural matrices into an interpretable basis. Inputs include equilibrium, constraints, ports, mass, stiffness, and mesh evidence. Outputs include eigenpairs, residuals, normalization, tracked mode families, and reconstruction tests.

Twenty returned frequencies are not a complete modal model. The vectors need physical scaling, modes must correspond across meshes and parameter changes, and the selected basis must represent the loading and observations.

## Eigenproblem and rigid motions

\[
K\phi_j=\lambda_jM\phi_j,\quad
\omega_j=\sqrt{\lambda_j},\quad f_j=\omega_j/(2\pi).
\]

Use a generalized Hermitian solver only when matrices actually have the required structure. Nonsymmetric tangents, nonconservative loads, and some mixed formulations require another problem class.

SLEPc’s official EPS manual describes generalized problem types and residual-based accuracy: https://slepc.upv.es/release/documentation/manual/eps.html. Its EPSComputeError reference specifies available error measures: https://slepc.upv.es/release/manualpages/EPS/EPSComputeError.html. These establish documented capabilities, not installation status.

Never take the absolute value of a negative eigenvalue to create a frequency. Negative stiffness directions may indicate unstable equilibrium or invalid assembly. Small negative rigid-body values require a scale-aware roundoff explanation.

Identify free-body nullspace geometrically. The six returned low vectors need not align with separate translations and rotations because any basis of that subspace is valid. Soft-support modes are not automatically rigid modes to discard. Inspect their deformation and support sensitivity.

## Mass normalization and units

Choose \(\phi_j^TM\phi_j=1\), then check

\[
\Phi^TM\Phi=I,\qquad\Phi^TK\Phi=\Omega^2.
\]

For translational coordinates, mass-normalized shape entries have units kg\(^{-1/2}\), and modal coordinates m·kg\(^{1/2}\). Other normalization choices are valid only if coupling and observation maps transform consistently.

A unit-maximum-displacement plot must not overwrite the computational basis. Rescaling a mode requires inverse rescaling of its coordinate and consistent adjustment of participation coefficients. Otherwise the apparent harmless plotting operation changes physical coupling.

Use a deterministic sign convention based on a well-observed component. Avoid a point near a modal node, where tiny noise flips signs. Signs do not alter a mode physically, but inconsistent signs between bridge and radiation exports do alter predicted sound.

## Residual and orthogonality tests

Compute \(r_j=K\phi_j-\lambda_jM\phi_j\). Report absolute and scaled residuals with explicit norms. A scale involving only \(|\lambda_j|\) fails near zero; use a suitable rigid-mode metric.

Recompute residuals independently after export/reimport. Orthogonality alone is insufficient: wrong vectors can be orthogonal. Nearly repeated modes may need subspace reorthogonalization rather than rejection.

Maintain fixtures with exact distinct, repeated, and nearly repeated eigenvalues. Include an anisotropic FE benchmark. A diagonal matrix with separated values does not test tracking or geometric observation maps adequately.

## Tracking modes

Frequency order is not identity. Modes cross or veer as geometry or material changes. Map vectors to a shared observation/integration space and compare

\[
\mathrm{MAC}_W(a,b)=
\frac{|a^HWb|^2}{(a^HWa)(b^HWb)}.
\]

Specify weighting \(W\). A mass-weighted FE comparison is different from an unweighted sensor comparison. Sparse observation can make physically different modes appear similar.

Combine shape correlation, frequency proximity, and port participation in a gated global assignment. Do not greedily choose the nearest frequency. Retain ambiguous matches in the report.

For repeated clusters, compare invariant subspaces through principal angles or singular values. Individual vectors may rotate within a correct subspace. A poor pairwise MAC does not necessarily imply a poor clustered response.

Track changes in strain-energy distribution as supporting evidence. A mode that shifts from neck-dominated to soundboard-dominated may undergo genuine veering rather than a bookkeeping error. Do not force a single label through an ambiguous transition without reporting it.

## Comparison with measured shapes

Project FE modes through the sensor operator. A laser vibrometer observes motion along its beam; an accelerometer observes its calibrated axes and adds mass. Full three-dimensional FE displacement cannot be compared directly with a scalar sensor channel.

Separate shape similarity from amplitude calibration. High MAC does not establish modal mass, residue magnitude, damping, or radiation. Sensor locations near nodes provide poor scaling information even if useful for identifying nodal structure.

Design sensors for observability using candidate modes, but retain independent validation locations. Otherwise the measurement design can preferentially confirm the assumed basis and miss unmodeled behavior.

## Damped modes

Diagonal modal damping is a choice. Proportional damping permits independent coordinates; nonproportional joint or radiation loss can couple them. The quadratic eigenproblem

\[
(s^2M+sC+K)\psi=0
\]

produces complex poles and potentially complex modes. Left/right vectors and residues require consistent normalization. Do not insert damped frequencies into an undamped residue formula casually.

Frequency-dependent material or radiation loads create a nonlinear eigenproblem. Internal-variable state-space realizations can convert it into an enlarged linear problem. Direct nonlinear eigensolvers are another option. Either path needs its own residual interpretation and convergence tests.

## Completeness and reduction band

Select modes by port and observation contribution as well as frequency. An above-band mode can supply static flexibility. A weak bridge mode may radiate strongly under another excitation. A low frequency alone does not guarantee importance.

Compare modal reconstruction with full forced response, including antiresonances and cross-mobility. Relative error diverges near true zeros; use absolute and energy-related metrics too.

One spectral search can miss clustered modes. Use overlapping windows, count checks where available, and independently checked residuals. Report the requested interval, returned interval, and unresolved gaps.

## Procedure and acceptance

1. Verify symmetry, mass positivity, constraints, and equilibrium.
2. Identify rigid/support subspaces.
3. Select the correct eigenproblem and spectral search.
4. Solve below the mesh-error budget.
5. Normalize and check orthogonality.
6. Export modes and projected ports.
7. Reimport and audit residuals.
8. Track families across refinement and uncertainty samples.
9. Compare reconstructed and full responses.
10. Publish normalization and cluster ambiguity.

Acceptance requires basis-invariant response agreement, not identical vector signs. Another solver should reproduce physical outputs within tolerance even if repeated-mode bases differ.

The identification task receives computational normalization, not visual scaling. The runtime task receives all coupling maps and residual corrections. A list of frequencies plus arbitrary decays is insufficient to preserve physical loading.
