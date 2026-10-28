# Time integration: stability, phase, dissipation, and nonlinear work

## Select an equation, not a method label

Names such as Newmark, midpoint, generalized alpha, and symplectic do not fully
specify an implementation. Parameter conventions, force evaluation, constraint
treatment, initialization, and nonlinear solve tolerances determine the actual
algorithm. This chapter compares candidate references under a common mechanical
system,
\[
M\ddot{\mathbf q}+C\dot{\mathbf q}+\nabla V(\mathbf q)=\mathbf F.
\]
Use fixed positive mass, nonnegative physical damping, and a stated potential
unless a subsection explicitly relaxes those assumptions.

Inputs are prepared operators, states, force/control trajectories, and a step.
Outputs are a proposed next state, solver diagnostics, and a discrete work
residual. A failed proposal must not be committed as an ordinary state.

## Central differences

For a lossless linear oscillator \(\ddot q+\Omega^2q=0\), central differences
give
\[
q_{n+1}-2q_n+q_{n-1}+k^2\Omega^2q_n=0.
\]
The numerical phase per step satisfies
\[
\theta=2\arcsin(\Omega k/2),\qquad \Omega k<2.
\tag{T1}
\]
The scheme is conditionally stable and has a modified half-step energy.
Its temporal phase frequency is higher than the continuous oscillator frequency.
That error can partially cancel a spatial discretization's lower eigenfrequency,
but cancellation at one configuration is not a general accuracy guarantee.

For a matrix system, the highest generalized frequency controls the linear
lossless criterion. Damping time placement changes the result. The exact
backward-gradient-loss string scheme is specified in its own chapter, including
its effective mass and first-order loss perturbation. Do not summarize it simply
as “standard central differences.”

## Implicit midpoint as a linear energy reference

The first-order midpoint equations are
\[
\Delta\mathbf q=k\overline{\mathbf v},\qquad
M\Delta\mathbf v/k=-K\overline{\mathbf q}
-C\overline{\mathbf v}+\overline{\mathbf F}.
\tag{T2}
\]
For the fixed linear system, they exactly satisfy the adjacent-state mechanical
energy balance in real arithmetic. In a lossless oscillator,
\[
\theta=2\arctan(\Omega k/2).
\tag{T3}
\]
The amplification magnitude is one for every finite k, but the phase frequency
is lower than the physical one. As k grows, numerical phase approaches a finite
per-step limit rather than resolving arbitrarily high frequencies correctly.
Unconditional linear stability does not remove the accuracy requirement.

For quadratic energy, a synchronized midpoint coupling is particularly useful
as a reference because internal port work cancels directly. Its factorization
may be prepared outside real time for fixed matrices. Nonlinear contact still
requires a solve and a compatible potential discretization.

## Ordinary midpoint versus discrete gradients

For nonlinear V, evaluating \(\nabla V\) at the mean coordinate generally fails
to satisfy
\[
\nabla V(\overline{\mathbf q})^T\Delta\mathbf q=\Delta V.
\]
The discrepancy vanishes for quadratic V but not for a generic quartic or
one-sided contact potential. A discrete-gradient method replaces the gradient
with a two-point map satisfying that identity exactly.

One must distinguish energy conservation, symplectic structure, momentum
conservation, and phase accuracy. A method may preserve one property but not
another. Exact energy conservation alone can still coexist with phase error or
an inaccurate invariant distribution. The project uses energy as a powerful
diagnostic and stability tool, not as a substitute for convergence.

For a discrete-gradient solve with nonzero residual \(\mathbf r\) in force units,
the work defect contains \(\mathbf r^T\Delta\mathbf q\). Therefore solver
tolerances can be translated into an energy-defect bound. This is more useful
than an unexplained fixed iteration count.

## Newmark family with explicit convention

The Newmark update is
\[
\mathbf q_{n+1}=\mathbf q_n+k\mathbf v_n+
k^2[(1/2-\beta)\mathbf a_n+\beta\mathbf a_{n+1}],
\]
\[
\mathbf v_{n+1}=\mathbf v_n+
k[(1-\gamma)\mathbf a_n+\gamma\mathbf a_{n+1}].
\tag{T4}
\]
The acceleration at the new endpoint is found from equilibrium at that endpoint,
unless a modified equilibrium evaluation is explicitly specified.

For the average-acceleration choice \(\gamma=1/2,\beta=1/4\), a fixed linear
system with consistently initialized endpoint acceleration is equivalent to
trapezoidal/midpoint evolution with the appropriate averaged load. It is
second order and unconditionally stable for the standard autonomous linear
positive-mass, nonnegative-stiffness problem. It introduces no amplitude decay
for an undamped oscillator, but has the phase distortion in T3.

For \(\gamma=1/2,\beta=1/6\), the linear-acceleration variant is not the same
method and has a finite linear stability range. For an undamped scalar oscillator
the condition is \(\Omega k\le\sqrt{12}\), with strict margin preferred.
Do not repeat a blanket claim that all implicit Newmark variants are
unconditionally stable.

For the specific line \(\gamma=1/2,\beta\ge1/4\), the standard linear
unconditional-stability result applies. Other parameter choices should be
verified by deriving their amplification matrix instead of importing a condition
from a different convention. Nonlinear constraints and inconsistent initial
accelerations require separate analysis.

The primary source is Nathan Newmark, *A Method of Computation for Structural
Dynamics*, <https://doi.org/10.1061/JMCEA3.0000098>. The equations and selected
claims here are stated explicitly so they can be checked independently of a
software package's description.

## Generalized alpha without convention ambiguity

Use the forward-weight convention
\[
\mathbf q_{n+\alpha_f}=(1-\alpha_f)\mathbf q_n+\alpha_f\mathbf q_{n+1},
\]
and the corresponding velocity average, while acceleration uses \(\alpha_m\).
Evaluate the linear equilibrium as
\[
M\mathbf a_{n+\alpha_m}+C\mathbf v_{n+\alpha_f}
+K\mathbf q_{n+\alpha_f}
=\mathbf F_{n+\alpha_f}.
\tag{T5}
\]
The kinematic update remains T4. In this convention, choose a target
high-frequency spectral radius \(0\le\rho_\infty\le1\) and set
\[
\alpha_m=\frac{2-\rho_\infty}{1+\rho_\infty},\qquad
\alpha_f=\frac1{1+\rho_\infty},
\]
\[
\gamma=\frac12+\alpha_m-\alpha_f,\qquad
\beta=\frac14(1+\alpha_m-\alpha_f)^2.
\tag{T6}
\]

The PETSc primary documentation uses this convention:
<https://petsc.org/main/manualpages/TS/TSAlpha2SetRadius/> and
<https://petsc.org/main/manualpages/TS/TSAlpha2SetParams/>.
The original generalized-alpha paper is Chung and Hulbert, *A Time Integration
Algorithm for Structural Dynamics With Improved Numerical Dissipation*,
<https://doi.org/10.1115/1.2900803>.

Some publications instead use weights at \(n+1-\alpha\); their numerical
parameters are complements of these. Never copy alpha values without the
evaluation equation. For small \(\rho_\infty\), \(\alpha_m\) can exceed one,
so the acceleration expression is an extrapolative weighted value rather than
a convex interpolation. That is intentional in this parameterization.

Generalized alpha deliberately damps high numerical frequencies in the standard
linear analysis. This can be useful for a structural reference, but the
algorithmic loss must be separated from physical damping. A fitted decay curve
can otherwise absorb solver loss into material parameters. The linear stability
result does not guarantee energy monotonicity for arbitrary nonlinear contact,
moving constraints, or incomplete iterative solves.

## Exact modal propagation

For an uncoupled constant-coefficient mode, the state transition can be evaluated
from a two-by-two matrix exponential. In the underdamped case the homogeneous
poles are
\[
r_\pm=e^{-dk}e^{\pm i\sqrt{\Omega^2-d^2}\,k}.
\]
This removes homogeneous time-discretization error for that mode. It does not
prevent aliasing if sampled oscillation exceeds the represented frequency range.
It also does not make an arbitrary nonlinear force exact.

For piecewise-constant force, integrate the input over the step using the exact
state-transition integral. For a force impulse, use the momentum jump. For a
linearly varying force, use the corresponding integral. These are distinct input
models. A recurrence that simply adds \(k^2F\) after exact homogeneous propagation
does not automatically implement any of them correctly.

Near zero frequency or critical damping, evaluate limits robustly. Functions
such as sinc-like ratios or matrix-function methods can avoid cancellation.
Reject a mode solver whose accuracy depends on dividing by a nearly zero
damped frequency without a limiting branch.

## Explicit Runge–Kutta as a comparison, not a free stability guarantee

Classical fourth-order Runge–Kutta applied to a linear oscillator has stability
polynomial \(R(z)=1+z+z^2/2+z^3/6+z^4/24\). On the imaginary axis,
\[
|R(ix)|^2=1-x^6/72+x^8/576.
\]
This yields a bounded linear interval \(|x|\le\sqrt8\), but the method does
not conserve oscillator energy exactly. Its higher formal order does not remove
the need to resolve contact events or obey its stability region.

An adaptive reference solver can be useful outside real time, with independently
controlled local error and convergence checks. Its accepted internal steps and
interpolation to output times must be recorded. The production callback should
not inherit an unbounded adaptive loop.

## Initial equilibrium and static preload

A string pressed against a fret or loaded by a finger can begin from a static
equilibrium. Solve that equilibrium with the same forces and constraints used
in dynamics, then initialize velocity and acceleration consistently. Starting
from an arbitrary deformed shape with zero acceleration creates a spurious
transient unrelated to the intended pluck.

For a reference intended to model release of stored deformation, the static
preload work is part of initial energy preparation. Preserve it in metadata.
Do not confuse a static initialization solve with several undocumented damped
audio steps that happen to settle near equilibrium.

## Decision procedure and rejection gates

First choose required physical bandwidth and nonlinear behavior. Next select a
transparent reference method and derive its energy and phase behavior. Run
temporal refinement on a fixed spatial system. Then evaluate candidate production
methods using error versus cost across the actual supported envelope.

Reject a method choice justified solely by the word “implicit,” “symplectic,”
or “fourth order.” Reject fitted physical damping that changes drastically when
the numerical step changes. Reject nonlinear results whose accuracy is limited
by solver tolerance but reported as integrator order.

Every candidate needs a separate force-time contract, initialization contract,
failed-step policy, and work ledger. Continue with dispersion and dissipation
metrics to turn these method comparisons into quantitative evidence.
