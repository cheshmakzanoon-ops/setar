# Diagnose a numerical failure without hiding its cause

Use this prompt when a render becomes unstable, clicks during a gesture, changes
across platforms, fails convergence, or disagrees with an analytic reference.
Diagnosis does not authorize implementation changes unless the task says so.

## Task prompt

Diagnose [observed failure] in [component or fixture]. Read the relevant source,
tests, AGENTS.md, and numerical contract. Do not clip, normalize, add noise,
change physical damping, or relax tolerances merely to make the symptom disappear.
Do not claim a root cause until a controlled test distinguishes it from plausible
alternatives.

First establish the smallest reproducible fixture. Record physical parameters,
units, initial conditions, boundary and contact configuration, event sequence,
internal sample frequency, host block partition, precision, compiler flags,
and the exact first rejected or divergent step. Preserve the failing fixture
before modifying any candidate.

## Classify the symptom

If energy grows, determine whether external force, moving geometry, retuning,
or state projection supplied the increase. Compute the full work ledger before
calling every increase unstable. If the balance residual grows without accounted
work, inspect the first subsystem and time level where cancellation fails.

If the waveform clicks at grid crossings, isolate interpolation support and
moving-map work. If pitch is wrong but the system is stable, compare continuum,
semidiscrete, and fully discrete modal frequencies. If decay is wrong, separate
physical loss from algorithmic damping and output filtering.

If platforms disagree, compare prepared coefficients and discrete choices
before comparing audio. Check compiler floating-point assumptions, fused
operations, denormal mode, reduction order, math-library evaluation, event
scheduling, and data races. Do not label all discrepancies “floating point”
without locating their origin.

## Controlled isolation sequence

Run the component with zero external input and a smooth known state. Disable
nonlinearities only through an explicit diagnostic configuration, preserving
the original failure case. Compare the resulting linear problem with an analytic
mode or small matrix reference.

Check units and coefficient powers before tuning. Verify actual h equals L/N,
the selected grid satisfies the scheme-specific strict condition, and endpoint
rows match the intended boundary. For the backward-gradient-loss FDTD scheme,
evaluate its effective energy matrix, not the centered-loss energy.

Reintroduce one interaction at a time: ordinary loss, gradient loss, bridge
coupling, scalar contact, multiple contacts, shared stretching, and moving
controls. The order can be adapted to the failure, but keep each test's hypothesis
explicit. A broad parameter sweep is useful only after its diagnostic outputs
can distinguish mechanisms.

## Nonlinear-solver diagnosis

Evaluate the residual independently at the reported solution. Compare analytic
Jacobian products with directional differences away from nonsmooth points.
Check scaling, factorization residual, predictor, line search, bracket invariants,
and stopping conditions.

For a scalar convex contact, compare with the analytic bracket and a strict
bisection reference. For coupled contacts, compare the eliminated residual with
the original mechanical-coordinate equations. A reconstruction sign error can
leave the reduced solver apparently converged.

Measure the unbalanced residual work \(R^T\Delta q\) and compare it with the
observed energy defect. If they disagree, inspect the energy implementation,
force time level, or state commit rather than simply tightening Newton tolerance.

Check onset, release, and near-equal potential endpoints separately. Direct
subtraction of nearly equal potentials can fail even when the outer solver is
mathematically well posed. Verify stable divided-difference branches with an
independent integral or high-precision reference.

## State-transition diagnosis

For retuning, compare energy before and after the parameter change with the
exact product-rule work split. For moving interpolation, evaluate the contribution
from the map change. For remeshing, compare physical displacement, velocity,
curvature energy, and discarded modes before and after projection.

If a note event resets a persistent string, identify the removed energy and
mechanical memory. Do not hide the reset with an output crossfade and call the
underlying model continuous. If a product-level crossfade is intentionally used,
evaluate it as a separate approximation.

Verify host-block invariance by processing the same event sequence with several
partitions. A different result can indicate stale controls or event-offset errors
rather than a physical instability.

## Required diagnostic output

Return the minimal reproducer, actual commands and observed results, the earliest
confirmed invariant violation, and the most likely cause with supporting evidence.
List alternatives that were tested and rejected, and uncertainties that remain.
Do not fabricate a successful test when a tool or platform was unavailable.

Recommend the smallest fix and the regression tests it would require, but do not
implement it unless authorized. If implementation is authorized, preserve the
original reproducer and rerun both the focused test and the relevant numerical
suite after the change. Report whether the failure is solved within the supported
envelope, rather than declaring the whole instrument validated.
