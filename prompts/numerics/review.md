# Review a numerical change as mathematics, code, and evidence

Use this prompt for a read-only technical review unless implementation changes
are explicitly requested. The reviewer should identify concrete defects and
missing evidence, not rewrite the entire architecture.

## Task prompt

Review [change or source paths] in the setar repository. Read AGENTS.md and the
relevant mathematical and numerical chapters. Inspect the actual diff and its
tests. Preserve all files; do not implement fixes, change fixtures, regenerate
goldens, commit, or push during this review.

State the numerical problem the change claims to solve. Identify the physical
PDE or finite-dimensional equations, boundary conditions, state variables, units,
time levels, forcing semantics, and supported parameter envelope. If those are
not recoverable from the code and documentation, treat the missing contract as
a finding rather than guessing the intended model.

## Equation and energy audit

Derive the implemented update from its source operations, including endpoint
rows and parameter conversions. Compare it with the documented scheme term by
term. Check signs, powers of h and k, linear versus volumetric density, angular
frequency versus hertz, and lumped force versus force density.

For an explicit stiff-string change, verify the exact damping time placement.
The backward \(\sigma_1\) scheme requires the effective mass and grid criterion
in docs/numerics/01-stiff-string-fdtd.md. A centered \(\sigma_1\) scheme is a
different implicit update. Flag any stability or accuracy claim copied across
those variants.

Recompute the discrete energy and its positivity condition. Verify the force's
conjugate velocity time level. Check whether boundary reactions and moving
parameter work are included. A diagnostic that omits one subsystem's energy
cannot establish passivity of the complete model.

For contacts, inspect the potential, gap convention, discrete gradient, normal
direction, and derivative. Check the equal-endpoint limit and the onset/release
branches. Determine whether the nonlinear residual is scalar monotone, coupled
convex, or more general; do not accept a scalar convergence argument for a
multi-contact system.

## Linear algebra and coupling audit

Check mass positivity, stiffness and damping symmetry or weighted self-adjointness,
expected nullspaces, and coordinate normalization. Verify a generalized
eigenproblem is solved in the correct form. For repeated eigenvalues, ensure
tests compare appropriate subspaces rather than arbitrary eigenvector signs.

Verify observation and injection maps are exact adjoints in the declared
quadrature. Inspect shared-force signs and coordinate transforms. Look for
undocumented sample delays, sequential updates that use mixed time levels,
or body processing that is described as bidirectional while actually functioning
only as an output filter.

For a moving contact, locate the geometric work term associated with changing
interpolation or normal. For retuning, locate parameter work and stability
revalidation. For remeshing or changing modal bases, locate the state projection
and its energy diagnostic.

## C++ and real-time audit

Inspect array ownership, minimal sizes, boundary indexing, overflow-safe dimension
construction, and state commit order. Check for allocations, locks, I/O,
unbounded loops, or exception paths in the callback. Confirm preparation data
cannot be partially mutated by another thread.

Inspect actual compiler flags and nonfinite checks. A finite-math optimization
assumption can invalidate a source-level safety check. Check denormal policy,
fused-operation differences, reduction order, and the distinction between strict
reference and production builds.

Do not assume all SIMD differences are harmless rounding, or all cross-platform
differences are bugs. Use the physical error budgets and determine which
operation changed. Event ordering and accepted/rejected step status should not
be dismissed as ordinary floating-point variation.

## Evidence audit

Determine which tests were actually run. Distinguish analytic references,
convergence studies, production-generated goldens, and empirical recordings.
Check that expected values are independent of the implementation under review.
Inspect negative tests for wrong signs, violated stability bounds, incomplete
solves, and nonfinite input.

Review spatial and temporal refinement separately. Check smoothness assumptions
of initial conditions, especially sharp plucks in stiff strings. A finite modal
projection of a triangular shape has finite energy, but its continuum bending
energy need not converge with modal count.

Evaluate tolerance provenance. Are thresholds derived, proposed, measured, or
merely increased until tests pass? Check absolute-plus-relative comparisons near
zeros and node observations. Ensure waveform alignment or normalization does
not hide timing or force-scale defects.

## Required review output

Lead with actionable findings ordered by consequence. For each finding, give the
actual source location, triggering condition, mathematical or runtime mechanism,
expected consequence, and a minimal test that would demonstrate it. Separate
confirmed defects from risks and missing evidence.

If no defect is established, say that clearly while listing untested regimes
and residual uncertainty. Do not claim a proof of instrument fidelity from
code review. Provide a short acceptance recommendation tied to the relevant
master phase gate in docs/ROADMAP.md.
