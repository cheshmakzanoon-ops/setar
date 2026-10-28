# Convergence studies, reproducible fixtures, and cross-platform tolerances

## What convergence evidence must establish

A solver converges when its approximation approaches the stated problem as the
relevant discretization or solve tolerances are refined under appropriate
assumptions. Comparing two nearby parameter settings is not enough. A useful
study identifies which error source is changing and which are held small or
fixed.

For this project, separate at least spatial discretization, temporal integration,
modal truncation, contact regularization, interpolation, nonlinear-solve tolerance,
floating-point rounding, and output sample-rate conversion. Several can dominate
at different stages of the same gesture.

Inputs are a versioned fixture, an independent reference, a refinement plan, and
declared metrics. Outputs are measured errors, observed rates, failure cases,
and a supported operating region. Numerical budgets are proposed engineering
requirements unless derived mathematically or established empirically.

## Spatial refinement

Hold the physical model and forcing fixed. Reduce h or increase finite-element
resolution while making time error sufficiently small. For an explicit stiff
string, the time step may need to shrink with spatial refinement to remain
stable. That coupled change must be accounted for when interpreting the result.

Compare at common physical locations using a defined observation map. Directly
subtracting arrays of different lengths does not compare physical fields.
For field norms, integrate or project onto a common sufficiently accurate
representation. Preserve the original physical domain and boundary conditions.

Use smooth data for formal-order tests. A point force, sharp initial kink, or
contact event can reduce regularity and observed order. Such cases remain
important, but their convergence study should emphasize appropriate physical
observables rather than demand a smooth-solution rate that the problem cannot
support.

## Temporal refinement

Freeze the spatial system and compare several decreasing k values against its
exact modal or high-accuracy semidiscrete solution. This isolates time integration
from spatial approximation. Use identical physical output times and a consistent
forcing trajectory.

For the explicit backward-gradient-loss scheme, test \(\sigma_1=0\) and
\(\sigma_1>0\) separately. The latter generally has first-order temporal error.
A second-order fit obtained only with zero gradient loss does not establish
second order for the full model.

For nonlinear methods, tighten the algebraic solve enough that it does not limit
the observed rate. If reducing k also changes the number of permitted Newton
iterations or contact substeps, record those changes. An apparent improvement
may come from a different solver tolerance rather than the integrator.

## Observed order and asymptotic regime

For errors E(h) and E(h/r), a common estimate is
\[
p_{\mathrm{obs}}=
\frac{\log[E(h)/E(h/r)]}{\log r}.
\tag{G1}
\]
Use it only when both errors are meaningful, measured in the same norm, and
above the reference and rounding floor. If either error is zero due to a
special exact case, the ratio is not a useful order estimate.

Use at least several refinement levels and inspect the trend. Coarse grids may
be outside the asymptotic regime, while very fine grids can hit floating-point
or reference-solver limits. A fitted slope across all levels can hide both
effects. Report the raw error table and the subset used for any slope estimate.

Richardson extrapolation assumes a sufficiently regular error expansion and
consistent refinement. Do not use it blindly across contact topology changes,
mesh snapping events, or a transition from underdamped to overdamped numerical
roots. Those changes can invalidate the assumed expansion.

## Contact stiffness and penalty convergence

Increasing penalty stiffness is not the same as refining h or k. It changes
the model unless the intent is an approximation to a rigid obstacle. In a
compliant-contact model, the stiffness is a physical parameter; convergence
means solving that parameterized model accurately.

For a rigid-obstacle limit, study penetration, impulse, rebound, and energy as
penalty stiffness increases while temporal and spatial errors are controlled.
The limit may require increasingly demanding resolution. A method can remain
stable while force peaks become inaccurate or contact timing stagnates.

Keep physical contact width fixed under spatial refinement. If a distributed
contact is represented by more quadrature points, scale its potential and
force weights consistently. Otherwise the study changes total stiffness with
resolution and cannot be interpreted as convergence.

## Solver-tolerance refinement

At fixed h and k, tighten nonlinear residual and linear-solve tolerances.
Compare accepted states, contact forces, and energy defects. Identify the point
where further tightening no longer changes physical observables materially.
This distinguishes algebraic error from integration error.

Record iteration count and rejected steps as part of the tolerance study.
An aggressively tight tolerance can be numerically inappropriate near
floating-point limits, leading to stagnation without improving the physical
solution. The right response is a scaled tolerance analysis, not an unbounded
iteration loop.

If a reduced contact solver and a full-coordinate solver disagree, compare their
residuals in the original mechanical coordinates. A small reduced residual does
not prove correct reconstruction if the elimination or force map is wrong.

## Golden data are regression evidence

A golden dataset records expected behavior of a particular accepted implementation
and configuration. It can catch accidental changes, but it cannot prove the
original behavior was physically or numerically correct. Promote a fixture to
golden status only after independent analytic or convergence evidence exists.

Store physical parameters, units, initial state definition, boundary and contact
configuration, event sequence, integrator variant, mesh or modal basis,
precision, compiler flags, and reference provenance. Include a content hash
and schema version so accidental corruption and incompatible interpretation are
detectable.

Do not store only a normalized WAV. Preserve physical state summaries, energy
and work traces, contact impulses, and analysis metadata. A normalized waveform
can hide a force-scale regression, and audio quantization can hide a mechanical
state defect.

## Synthetic versus measured datasets

Mark synthetic fixtures as synthetic in their metadata and human-readable
description. Their purpose is controlled verification, not evidence of a real
instrument. Measured reference data require permissions, instrument and setup
provenance, calibration details, and uncertainty.

Do not merge measured and synthetic values in one table without explicit labels.
Do not tune a solver tolerance against a measurement whose noise floor dominates
the relevant difference. Numerical correctness should first be established on
clean mathematical fixtures.

If a measurement is used for parameter identification, preserve a separate
validation set when feasible. A model can fit its calibration examples while
failing at another pluck location, dynamic level, tuning, or articulation.
Numerical regression and empirical generalization are different tests.

## Bitwise and metric comparisons

Bitwise comparison is useful within a deliberately controlled environment:
same implementation, compiler configuration, instruction behavior, operation
ordering, and math library assumptions. It is not a universal requirement for
all platforms.

Across platforms, use physically interpretable metric tolerances while retaining
strict checks for invariants such as finite states, event ordering, dimensions,
and accepted/rejected status. Allowing a small frequency difference is different
from allowing one platform to take a different contact branch and silently mute.

Record whether fused multiply-add, SIMD reduction order, denormal handling, and
transcendental evaluation differ. Do not use a broad “platform tolerance” to
hide a race, integer overflow, or missing boundary term.

A useful comparison hierarchy is:

1. Exact metadata and event identity.
2. Exact discrete choices that should not depend on rounding, such as mesh count.
3. Algebraic and energy residual budgets.
4. State and port-response error budgets.
5. Frequency, decay, impulse, and transient metrics.
6. Perceptual tests only after the preceding numerical checks pass.

## Choosing tolerances without arbitrary inflation

Start from the expected error source: discretization order, linear conditioning,
nonlinear residual, or floating-point rounding. Use analytic fixtures to estimate
its scale. Then choose a proposed budget that is strict enough to detect relevant
regressions and loose enough to reflect known numerical limits.

Use absolute-plus-relative comparisons with a stated physical floor. Relative
error alone is unsuitable near zero crossings and mode nodes. Absolute error
alone can be inappropriate across widely different physical scales.

For a waveform, report both a normed error and a maximum error. For a mode,
report frequency and decay separately. For contact, report penetration,
impulse, peak force, and energy defect. No single scalar captures every failure.

When a test fails, investigate before changing the threshold. A threshold change
must include a reason, evidence that the old budget was inappropriate, and an
assessment of what regressions the new budget might no longer detect.

## Reproducible experiment packaging

Each experiment should be runnable from physical fixture metadata without a
plugin host or interactive UI. Define sample count, internal frequency, event
offsets, and output observations deterministically. A seed alone is not enough
if a randomized fixture depends on an unspecified generator algorithm.

Store compact raw numeric outputs and a machine-readable summary. Include
failed configurations in the report. Do not regenerate golden results
automatically whenever a test fails; that converts regression testing into
approval of every change.

Separate analysis code from synthesis code where possible. If both compute a
modal frequency using the same mistaken helper, agreement is not independent.
Use exact formulas, a different representation, or independently derived
calculations for expected values.

## Rejection gates and next action

Reject a convergence claim without raw refinement data, a golden dataset without
provenance, or a cross-platform claim without actual runs. Reject comparison
rules that normalize away force scale or align away every timing error.

The mathematical basis for the fixtures is documented in the preceding chapters.
Compiler and floating-point variability should be interpreted using the primary
GCC and Clang documentation linked in the runtime chapter. Continue with the
reference algorithm blueprints to turn the specification into small executable
tasks with clear handoff evidence.
