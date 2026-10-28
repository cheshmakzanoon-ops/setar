# Contact potentials, exact work, and nonlinear solve guarantees

## Contact is a mechanics problem before it is a solver problem

A fret, nail, fingertip, or bridge surface can be represented by several different
models. A compliant contact stores deformation energy. A penalty approximation
to a rigid obstacle allows artificial penetration that should decrease in a
controlled limit. An ideal unilateral constraint imposes nonpenetration and
requires complementarity or impact mechanics. These alternatives cannot be
interchanged by merely making one coefficient very large.

The reference proposed here uses a conservative one-sided potential plus explicitly
separate dissipative terms. Its purpose is a verifiable baseline for pluck release,
fret contact, and stopping mechanics. It does not establish the constitutive law
of a measured setar nail or tied fret.

Primary source context is Bilbao, Torin, and Chatziioannou, *Numerical Modeling of
Collisions in Musical Instruments*, <https://doi.org/10.3813/AAA.918813>, author
manuscript <https://arxiv.org/pdf/1405.2589>; and Bilbao and Torin,
*Numerical Simulation of String/Barrier Collisions: The Fretboard*, equations
(5), (14), and section 3.6,
<https://www.pure.ed.ac.uk/ws/portalfiles/portal/17485791/dafx14_final.pdf>.
The scalar residual and bracketing procedure below are derived for the midpoint
reference defined here, not copied from a different three-level scheme.

## Potential, force, and dimensions

Let signed compression \(s=-g\), where positive gap means separation. Use
\[
\Phi(s)=\frac{K_c}{\alpha+1}[s]_+^{\alpha+1},
\qquad [s]_+=\max(0,s),\quad K_c\ge0,\quad\alpha\ge1.
\tag{Q1}
\]
The compressive force magnitude is
\[
F_c(s)=\Phi'(s)=K_c[s]_+^\alpha.
\tag{Q2}
\]
For a lumped contact, \(\Phi\) is in joules and \(K_c\) has units N/m^\(\alpha\).
For a potential density integrated over length, its coefficient instead has units
N/m^\(\alpha+1\). A distributed barrier discretization needs quadrature weights;
copying a lumped stiffness into every grid node changes total stiffness with
mesh refinement.

If \(s=b-G\mathbf q\), with \(G\mathbf q\) the string's normal displacement,
the generalized contact force on the string is \(G^TF_c(s)\). Its sign follows
from \(-\nabla_{\mathbf q}\Phi\), not from a remembered “collision minus sign.”
If the surface is dynamic, include its opposite force and displacement in the
same relative coordinate.

## Two-point contact force

For two compression states \(a,b\), define
\[
\overline F_c(a,b)=
\begin{cases}
[\Phi(b)-\Phi(a)]/(b-a),&b\ne a,\\
\Phi'(a),&b=a.
\end{cases}
\tag{Q3}
\]
Then
\(\overline F_c(a,b)(b-a)=\Phi(b)-\Phi(a)\) exactly in real arithmetic.
This is the discrete chain rule needed for exact mechanical work. It is not the
same as evaluating \(F_c\) at \((a+b)/2\), except for special potentials.

A step can straddle contact onset or release. Q3 remains meaningful even when
one endpoint is separated and the other compressed. Its positive force can act
over a step that ends separated, representing integrated contact work. Do not
zero that force merely because the final gap is positive; doing so breaks the
potential identity. If the step is too coarse to resolve contact timing accurately,
refine the temporal discretization rather than rewriting its force afterward.

## Stable evaluation near equal endpoints

Direct subtraction of nearly equal potential values loses precision. There are
several reliable special cases:

1. If both endpoints are nonpositive, the result is exactly zero.
2. If one is positive and the other nonpositive, use the single nonzero potential
   divided by the signed endpoint difference. No near-equal large values cancel.
3. If both are positive and the exponent produces a small integer polynomial,
   use the exact divided-difference polynomial.
4. For positive fractional powers, scale by the larger endpoint and evaluate
   a ratio with log1p/expm1 or a verified local series.

For \(m=\max(a,b)>0\), \(\ell=\min(a,b)>0\), \(r=\ell/m\),
\[
\overline F_c
=\frac{K_c m^\alpha}{\alpha+1}
\frac{1-r^{\alpha+1}}{1-r}.
\tag{Q4}
\]
The ratio tends to \(\alpha+1\) as \(r\to1\). A branch threshold for switching
to a series is a numerical design choice to be tested, not a physical parameter.
Evaluate the ratio in a way that does not overflow intermediate powers when the
final scaled force is representable. If the requested parameter envelope itself
overflows, reject it before audio processing.

An integral identity provides an independent reference:
\[
\overline F_c(a,b)=
\int_0^1 K_c[a+\theta(b-a)]_+^\alpha\,d\theta.
\tag{Q5}
\]
High-accuracy quadrature of Q5 can validate the optimized formula outside the
real-time path. It must not share the same cancellation-prone implementation.

## Scalar implicit contact and uniqueness

Consider one free mass \(m>0\), displacement \(q\), velocity \(v\), and a fixed
barrier at \(q=0\). Positive q means separation, so \(s=-q\). The midpoint
reference step with Q3 is
\[
v_1=v_0+\frac{k}{m}\overline F_c(s_0,s_1),\qquad
q_1=q_0+\frac{k}{2}(v_1+v_0).
\]
Eliminating q and v gives
\[
R(s_1)=s_1-s_0+kv_0+\gamma\overline F_c(s_0,s_1)=0,
\qquad\gamma=k^2/(2m).
\tag{Q6}
\]

For convex differentiable \(\Phi\), its divided difference is nondecreasing
in its second argument. Where differentiable,
\[
\partial_b\overline F_c(a,b)
=\frac{\Phi'(b)(b-a)-[\Phi(b)-\Phi(a)]}{(b-a)^2}\ge0.
\tag{Q7}
\]
At equal smooth endpoints, the limit is \(\Phi''(a)/2\). Thus
\(R'(s_1)\ge1\) where defined, and R is strictly increasing everywhere in the
continuous sense. It crosses zero because the linear term dominates toward
negative infinity and the positive terms grow toward positive infinity.
This establishes a unique scalar root for this model.

It does not establish global convergence of undamped Newton iteration. Newton
can overshoot, encounter a poorly evaluated derivative, or exhaust a fixed
iteration budget. Existence, uniqueness, convergence of an algorithm, and
acceptable runtime are separate claims.

## Safeguarded scalar algorithm

Compute the unconstrained prediction \(s_{\mathrm{free}}=s_0-kv_0\), and
\(F_u=\overline F_c(s_0,s_{\mathrm{free}})\). Since \(F_u\ge0\), an upper
endpoint is \(u=s_{\mathrm{free}}\). A finite lower endpoint is
\(\ell=s_{\mathrm{free}}-\gamma F_u\). Monotonicity gives
\(R(\ell)=\gamma[\overline F_c(s_0,\ell)-F_u]\le0\), while
\(R(u)=\gamma F_u\ge0\). Thus this scalar model has an analytic bracket,
provided its evaluation remains finite. If \(F_u=0\), the free prediction is
already the exact root. Other models may require bounded bracket expansion,
but no unbounded search belongs in the callback.

Then iterate within the bracket. Propose a Newton step only if the derivative
is finite and positive and the proposal remains strictly inside the bracket.
Otherwise use its midpoint. Evaluate R, replace the matching bracket endpoint,
and terminate only when both a scaled residual criterion and a coordinate or
bracket-width criterion are satisfied. Retain the full bracket invariant after
every iteration.

Bisection gives a predictable reduction in bracket width: after j bisections,
the width is the initial width divided by \(2^j\). This is an iteration-count
bound, not a claim that every real-time configuration is affordable. Select the
admissible parameter envelope and numerical tolerance jointly with a measured
runtime budget. Report when a supported gesture exceeds the budget.

## Multiple contacts are not independent scalar problems

Several contacts coupled through the same string share an effective compliance.
After eliminating a linear mechanical update, a common residual structure is
\[
\mathbf R(\mathbf s)=\mathbf s-\mathbf s_{\mathrm{free}}
+W_c\,\overline{\mathbf F}(\mathbf s_0,\mathbf s),
\tag{Q8}
\]
where \(W_c\) is symmetric positive semidefinite for a compatible collocated
mechanical solve. Its off-diagonal entries represent interaction through the
string or body. Solving each contact once independently ignores these terms.
Repeated local solves become an iterative method whose convergence needs analysis.

The Jacobian \(I+W_cD\), with diagonal nonnegative D, is generally not symmetric
in ordinary coordinates even when \(W_c\) is symmetric. Do not feed it to a
Cholesky routine as though it were. A transformed formulation, full mechanical
coordinate solve, or suitable general factorization may restore an appropriate
matrix structure. Handle zero derivatives without dividing by their square root.

For a fixed set of convex potentials, an implicit mechanical-coordinate residual
can often be related to a convex incremental potential. Prove that property for
the chosen update before invoking uniqueness. Friction, moving normals, nonlinear
geometry, and nonconvex constitutive laws can invalidate the simple argument.

## Dissipation, friction, and nonadhesion

A dissipative force should satisfy nonpositive mechanical work at relative
velocity. A dashpot acting only in compression can be defined with a nonnegative
state-dependent coefficient and a carefully chosen discrete velocity. However,
naively adding \(d\,\dot s\) to Q2 can produce a tensile total force during
separation. Clipping the total force afterward alters the energy proof.
Specify a nonadhesive law and derive its discrete work, or explicitly model
adhesion if evidence supports it.

Tangential friction introduces another velocity and another power channel.
For friction force \(\mathbf F_t\), require
\(\mathbf F_t^T\mathbf v_{\mathrm{rel},t}\le0\) in the relevant reference
frame. Static sticking involves a constraint or internal state; a smooth sign
approximation is not automatically a faithful model of stick-slip. Test friction
separately from normal contact before combining them.

## Acceptance evidence

The scalar suite must include separation, initial compression, approach, rebound,
grazing onset, a step crossing contact onset, a step crossing release, near-equal
endpoints, and very small forces. Compare force evaluation against Q5, compare
roots against an independent bracketed reference, and verify energy change.
Use several power exponents, including the nonsmooth-curvature case \(\alpha=1\).

The coupled suite must include two contacts with overlapping interpolation
support, contacts near a boundary, a compliant opposing object, and a deliberately
ill-conditioned but admissible contact configuration. Record root residual,
linear-solve residual, energy defect, maximum penetration, iteration count,
and rejected-step count. A single “converged” boolean is insufficient evidence.

Reject an implementation that uses unbounded iteration, masks nonfinite values,
clips a failed energy, or treats a solver fallback as a successful physical
render. The next action is to implement the reference algorithm outside real time,
then establish the finite operating envelope for an optimized solver.
