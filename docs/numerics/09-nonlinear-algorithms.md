# Nonlinear solver engineering and bounded failure handling

## Solver responsibility

A nonlinear integrator is not implemented merely because Newton's method has
been placed inside the sample loop. The solver must find the correct branch,
meet a physically interpretable residual criterion, remain within a bounded
operating envelope, and distinguish an accepted step from a failed proposal.
The integrator's energy argument usually assumes the nonlinear equations are
solved exactly; finite tolerance is part of the numerical error budget.

This chapter converts the contact and discrete-gradient equations into an
implementation plan. Inputs are an immutable prepared mechanical system, the
current accepted state, control data for the step, and a finite solver budget.
Outputs are either a fully diagnosed accepted state or a structured failure.
There is no third category of silently accepted “close enough” state.

## Displacement-increment residual

For the synchronized midpoint reference, let
\(\Delta\mathbf q=\mathbf q_1-\mathbf q_0\), and use
\(\mathbf v_1=2\Delta\mathbf q/k-\mathbf v_0\).
With linear stiffness K, damping C, and a nonlinear discrete gradient g, the
force residual is
\[
\mathbf R(\Delta\mathbf q)=
\left(\frac{2M}{k^2}+\frac Ck+\frac K2\right)\Delta\mathbf q
+K\mathbf q_0+
\mathbf g(\mathbf q_0,\mathbf q_0+\Delta\mathbf q)
-\frac{2M\mathbf v_0}{k}-\overline{\mathbf F}.
\tag{S1}
\]
For translational coordinates, every entry is a force. Mixed rotation coordinates
carry corresponding moments; use coordinate-aware scaling in convergence norms.

Define the positive linear matrix
\[
A_0=2M/k^2+C/k+K/2.
\]
For fixed linear gap maps and separable convex contact potentials, the contact
Jacobian contribution has form \(G^TDG\), where D is diagonal and nonnegative.
Then \(A_0+G^TDG\) is symmetric positive definite. This structure can support
a robust Newton solve or a low-rank reduction.

Do not generalize that symmetry to every discrete gradient. The quartic
stretching two-point gradient can have a nonsymmetric derivative with respect
to its second endpoint. Moving maps, friction, and nonlinear normals also alter
Jacobian structure. Choose the linear solver from the actual derivative.

## Independent residual and Jacobian implementations

Implement the residual in a clear reference path before optimizing its derivative.
Derive each analytic Jacobian term and check directional derivatives:
\[
\frac{R(x+\epsilon d)-R(x-\epsilon d)}{2\epsilon}
\approx J(x)d.
\]
Sweep epsilon over a range of scales. A single epsilon can accidentally hide
truncation error or roundoff. At a nonsmooth activation point, use appropriate
one-sided or semismooth checks rather than demanding a nonexistent classical
second derivative.

Automatic differentiation can help construct a reference derivative, but it does
not validate the physical residual. A perfectly differentiated wrong force sign
remains wrong. Compare the energy and virtual-work identities independently.

Do not reuse the production Jacobian to generate the expected residual correction
in every test. Include finite-difference directional checks, analytic scalar
fixtures, and matrix-assembled small systems. Independent structure matters more
than a large number of tests that all repeat one mistake.

## Predictor design

A simple predictor uses the free linear step without contact, followed by a
bounded projection into the solver's admissible numerical region if required.
The projection is only an initial guess; it must not alter the accepted physical
state or be counted as an actual collision response.

Reusing the previous step's contact force can improve convergence for sustained
contact. It can be a poor guess at onset, release, or abrupt geometric changes.
Track which predictor was used and compare iteration distributions. Do not make
correctness depend on a lucky warm start.

For a quasistatic initial preload, solve the static contact problem separately.
Use that solution to initialize the dynamic state and its stored energy. A
dynamic predictor should not be forced to discover a large missing static
equilibrium during the first audible sample.

## Newton and globalization

At iteration j, solve
\[
J_j\Delta x_j=-R_j.
\]
Check factorization status, finite entries, and linear-solve backward error.
An inaccurate linear correction can cause nonlinear stagnation even when the
outer method is appropriate.

For a general smooth residual, a line search may use a scaled merit function
\(\frac12\|W_RR\|^2\). Require a specified sufficient decrease and enforce
finite iteration bounds. The derivative of this merit function is not generally
the same as the physical potential gradient. If using an incremental convex
potential instead, prove its existence for the particular residual.

For a scalar monotone contact residual, safeguarded Newton with a verified
bracket is preferable to a generic multidimensional line search. Newton proposals
outside the bracket are replaced by bisection. The bracket is an invariant that
supports a deterministic error bound.

For multiple coupled contacts, independent scalar brackets are not enough
because other coordinates change each residual. A componentwise fallback needs
a convergence proof or a sufficiently tested restricted operating envelope.
Do not call one sweep of contact updates a converged simultaneous solution.

## Stopping criteria in physical terms

Use both residual and update tests. A small update can occur because the
Jacobian is ill conditioned, a line search is stalled, or finite precision prevents
progress. It does not establish equilibrium. A small residual alone can also be
misleading when badly scaled coordinates conceal a large physical displacement.

For a nondimensional residual \(\widehat R\), define the scaling explicitly.
A proposed criterion may combine absolute and relative tolerances:
\[
\|\widehat R\|_\infty
\le\epsilon_{\mathrm{abs}}+
\epsilon_{\mathrm{rel}}\widehat F_{\mathrm{scale}}.
\]
The values are design budgets, not universal constants. Choose them from an
error study that relates solver error to contact force, penetration, and total
work. Preserve the values in test metadata.

The unbalanced work of a force residual is
\[
\delta W_R=R^T\Delta q.
\tag{S2}
\]
Its absolute value can be bounded by compatible dual norms. This provides a
direct connection between algebraic tolerance and the energy ledger. Also
measure the actual discrete energy defect, because implementation errors can
invalidate a bound based on an incorrectly assembled residual.

## Active contacts and changing sets

An active-set method guesses which constraints are active, solves the associated
system, and checks consistency. It must verify both primal gap conditions and
dual force conditions. Cycling between active sets is a failure mode, not a
reason to accept the last iterate silently.

A smooth penalty potential avoids an exact binary active-set constraint, but
its derivative still changes at contact activation. A force evaluation with
unilateral powers must use consistent branches for potential, force, and
derivative. Branch disagreement near zero can create energy defects concentrated
at contact onset.

For a discrete gradient, force can be nonzero over a step that crosses release.
An endpoint-based active-set heuristic must respect that temporal interpretation.
If it simply discards every force whose final gap is positive, it destroys
the discrete work identity.

## Bounded runtime policy

The production solver must have explicit maximum dimensions, iterations, line-
search reductions, bracket expansions, and optional substeps. These bounds are
part of the supported physical envelope. A reference solver outside real time
can use a more expensive fallback to diagnose the failing case.

If substepping is used, prepare its data and coefficient variants ahead of time,
and bound the number of substeps. Apply external controls and power accounting
consistently across them. A substep path must not change the physical force
trajectory without disclosure.

Do not launch a background repair that modifies the audio state nondeterministically.
Instead, retain the last accepted state or invoke the documented fault-containment
policy while recording a compact diagnostic event. Whether the product fades,
mutes, or disables a voice is an application decision; none of those outcomes
counts as a valid physical render for verification.

## Diagnostic record

A failed-step record should include model and fixture identifiers, physical
parameters, previous accepted state reference, event offset, contact locations,
residual norm, update norm, linear backward error, iteration counts, bracket
status, finiteness flags, and the reason for rejection. Keep the callback record
bounded and allocation-free; serialize detailed data elsewhere.

Do not log an entire unbounded state history from the callback. Use a prepared
diagnostic ring or reproducible event seed and a controlled snapshot mechanism.
The objective is to reproduce the failure in the reference harness, not to
perform forensic file I/O during audio processing.

## Stress-test matrix

Include low and high supported contact stiffness, small and large supported
approach speeds, nearly grazing contact, sustained compression, overlapping
contacts, contacts near boundaries, and changing geometric maps. Vary internal
sample frequency independently from host block size.

Test cases just outside the supported envelope should fail cleanly at validation
or step acceptance. They must not produce an apparently successful but clipped
waveform. Deliberately force too few Newton iterations and verify that the test
detects incomplete convergence through residual and energy channels.

Compare a production solver with a stricter reference on the same mechanical
step. If they disagree, classify the cause: residual formulation, derivative,
linear solve, convergence tolerance, predictor branch, or state commit. A raw
waveform difference alone is not enough to identify the failure.

## Sources, limitations, and next action

The potential-based motivation is supported by Bilbao, Torin, and Chatziioannou,
*Numerical Modeling of Collisions in Musical Instruments*,
<https://doi.org/10.3813/AAA.918813>, and Bilbao and Torin's explicit discussion
of nonlinear equations and solver convergence in sections 3.6 and 5 of
<https://www.pure.ed.ac.uk/ws/portalfiles/portal/17485791/dafx14_final.pdf>.
The exact residual S1 and the production policy here are repository specifications.

Reject any solver claim that confuses unique mathematical roots with guaranteed
Newton convergence, or average iteration count with bounded real-time cost.
Continue with floating-point and runtime controls so the tested numerical method
survives its C++ implementation.
