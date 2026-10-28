# P03 — Verify the linear string before adding realism

**Status:** specification beyond the limited modal bootstrap. **Prerequisite:**
P02. **Exit evidence:** independently verified string modes, boundaries, energy,
forcing, and convergence over a declared numerical domain.

## Objective

Implement a transparent mathematical reference for one string. This phase is
not a tone-design exercise. If a simple string is numerically wrong, body
resonances and nail noise can make the output more attractive while obscuring
the defect. Establish correctness where analytic solutions are available.

Read [mathematics](../mathematics/README.md), [numerics](../numerics/README.md),
and [strings](../strings/README.md). Use the exact scheme derivation in the
numerics volume; do not combine a stencil from one paper with a stability bound
from another.

## P03-T01: declare the continuum problem

Use a coordinate x along a string of length L, transverse displacement y in
metres, tension T in newtons, and linear density mu in kilograms per metre.
Begin with constant positive tension/density and small transverse motion. For
the flexible lossless reference, mu y_tt = T y_xx. Document the endpoints,
initial displacement, initial velocity, and any force distribution.

The first boundary fixture is fixed displacement at both ends. The corresponding
normal modes and frequencies are analytic. A later stiff-string fixture requires
additional boundary conditions; fixed displacement alone is insufficient for
a fourth-order spatial equation. Do not silently call a simply supported model
“clamped,” because that changes eigenfunctions and partial frequencies.

Define the output variable. The displacement of one point, endpoint force,
velocity at a bridge, and radiated pressure are not interchangeable. This phase
may observe displacement or velocity, but must not label it sound pressure
without an acoustic observation model.

## P03-T02: implement at least two independent reference views

A modal solution and a spatial finite-difference solution can cross-check one
another in a shared linear case. Their errors and assumptions differ: modal
truncation removes high-frequency components, while a finite grid changes wave
propagation and imposes a resolution limit. Do not interpret agreement between
two similarly truncated representations as proof at frequencies neither resolves.

Start with one exact mode and a known amplitude. Compare time-domain phase,
frequency, energy, and spatial shape. Then superpose a few modes with deliberately
different amplitudes and phases. Verify linearity by comparing the response to a
sum of excitations with the sum of individual responses. Floating-point
differences require a justified tolerance, not exact equality of every sample.

Keep reference computations separate enough to avoid repeating the same indexing
mistake. An expected modal frequency should come from the continuum formula,
not from the model's internal stored frequency returned through an accessor.
An output test must exercise the actual output interpolation or observation path.

## P03-T03: establish boundary and force normalization

Verify endpoint displacement zeros and the sign of reflected waves. Test a
force at a selected position using a distribution whose discrete normalization
represents newtons correctly. A point force on a grid typically requires
considering the cell measure; inserting the same number into an acceleration
array is not a unit-consistent force model.

Use a smooth spatial forcing profile before a sharp point excitation so the
expected convergence is not dominated by a singularity. Compare work computed
from force times the collocated velocity with the state-energy change. An
interpolation operator used to observe contact velocity needs the adjoint
distribution operator for consistent work transfer. Test fractional positions,
not only exact grid nodes.

Exercise positions near both boundaries, equal and opposite force pairs, and
forces with zero integrated spatial sum. Boundary-force sign errors often remain
hidden when every plot uses absolute magnitudes.

## P03-T04: derive and test discrete energy

Write the continuum energy for the selected model and derive the numerical
energy for the exact time update. A convenient sum of squared samples is not
automatically that energy. For time-staggered schemes, the energy may involve
states at adjacent times; use the expression established by the numerical
analysis, including positivity restrictions.

Run unforced lossless fixtures over many cycles and inspect drift and bounded
oscillation in the appropriate energy. Repeat at several amplitudes, grid sizes,
sample rates, and initial mode combinations. Include a case close to, but inside,
the stated stability boundary and a deliberately invalid case that preparation
rejects. Do not intentionally emit unbounded audio to speakers during a
stability investigation; use offline state analysis.

Separate accumulation error in the diagnostic sum from error in the solver.
If necessary, compare compensated or higher-precision diagnostic accumulation
without changing the integration. Report absolute energy residual and a
well-conditioned normalization relative to initial energy or delivered work.

## P03-T05: establish convergence and dispersion

Choose a physical problem and vary spatial and temporal resolution in a way that
respects stability. Measure frequency and phase error over a stated duration,
spatial error at aligned physical coordinates, and energy behavior. A successful
run at one resolution does not establish convergence. If changing grid spacing
also changes the physical string length, the comparison is invalid.

When an explicit stability relation links step sizes, explain what quantity is
being held fixed and what order of accuracy the experiment can reveal. Do not
fit a slope across a pre-asymptotic region or round-off plateau and call it the
scheme order. Plot the residuals and identify the usable convergence regime.
Non-smooth initial displacement can lower observed convergence; distinguish
that effect from an implementation error using a smooth fixture.

Compute a numerical-dispersion map showing error by partial number or wavelength.
Use it to define a usable frequency band, not only a Nyquist limit. A scheme can
be stable while badly dispersive near the grid limit. Later real-time reduction
must compare against the established reference band.

## P03-T06: document failure boundaries

Reject nonpositive lengths/densities, invalid tension, nonfinite inputs, unsupported
sample rates, and insufficient resolution. Preserve the previous prepared state
if a replacement configuration fails validation. Document whether changing a
parameter is allowed during processing or requires preparation. Thread ownership
of the state must be explicit even in the offline prototype.

Add tests for reset, repeated prepare, segmentation into different block sizes,
and deterministic forcing schedules. These tests ensure the mathematical
experiment is not changed by software buffering. They become essential when
MIDI and host callbacks arrive.

## Evidence and prompt

Required artifacts include equations with assumptions, scheme specification,
analytic comparisons, boundary/work tests, convergence plots, dispersion maps,
invalid-input results, and exact commands. All fixtures are synthetic.

> Implement the specified linear-string case only. State the continuum and
> discrete boundary conditions, SI units, and supported parameter domain. Derive
> the relevant energy and stability conditions or cite the exact verified scheme
> derivation. Add independent modal, forcing-work, boundary, and convergence
> tests. Do not add a body filter, aesthetic EQ, randomized variation, or a claim
> of setar realism. Report both successful and rejected configurations.

Advance to P04 only when the new physics can be compared against a reliable
linear baseline. Preserve the P03 fixtures permanently as limiting-case tests.
