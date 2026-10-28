# Boundary closures, interpolation, and joint mechanical coupling

## Why the boundary is part of the algorithm

An interior stencil does not define a string model. The endpoint closure chooses
how force, moment, translation, and rotation interact with the nut, bridge, and
supports. It also determines the operator spectrum used by a stability proof.
This chapter's objective is to make boundary and interpolation decisions as
reviewable as the interior recurrence.

Prerequisites are the continuous boundary-work expression and the discrete
operator chapter. Inputs are geometry, endpoint mechanical assumptions, contact
locations, spatial weights, and the selected time representation. Outputs are
consistent state constraints, observation maps, force maps, and coupled test
fixtures. The reference implementation should use small matrices even when the
production implementation uses specialized stencils.

## Simply supported and clamped are different models

For the baseline bending string, a simply supported endpoint has zero displacement
and zero bending moment. A clamped endpoint has zero displacement and zero slope.
Both give zero power for a fixed support, but their modal spectra differ. Testing
only energy conservation cannot tell which was implemented.

For the compatible simply supported discretization, form \(S=-D_{xx}\) with
Dirichlet displacement endpoints and use \(S^2\) for bending. The corresponding
ghost condition is an odd extension, such as \(u_{-1}=-u_1\) at the left end.
For a clamped closure, derive a different operator from its weak form or a
summation-by-parts construction. Do not set slope zero by overwriting one interior
sample after each update; that removes or adds energy without a defined reaction.

A physically detailed bridge may be neither perfectly pinned nor perfectly
clamped. Introduce endpoint compliance as an explicit mechanical component.
Calibrate it against measurements only after its force and moment conventions
are fixed. A frequency-dependent empirical termination is acceptable as a reduced
physical model only if its causality, passivity, and parameter provenance are
examined.

## Prescribed endpoint motion by lifting

When endpoint displacement is nonzero, write
\[
u(x,t)=w(x,t)+\ell(x,t),
\]
where \(\ell\) satisfies the prescribed boundary motion and w has homogeneous
essential boundaries. For a tension-only string, a linear spatial lift may be
sufficient. For bending boundaries, choose a lift satisfying the full prescribed
conditions. Substitute the lift into the PDE; its acceleration, damping,
tension, and bending terms become known forcing.

This procedure avoids pretending the homogeneous operator still applies unchanged.
The prescribed support does work, and that work must agree with the reaction
force times support velocity. A lift chosen merely for convenience may have
nonzero curvature; include its bending contribution.

If a bridge is dynamic rather than prescribed, its motion is unknown and must
be solved with the string. The lifting approach can still help assemble the
coupled matrix, but it does not justify treating bridge motion as an external
recording or delayed control signal.

## Interpolation is an observation operator

For a contact at \(x_p\) between nodes j and j+1, linear interpolation gives
\[
u_p=(1-\theta)u_j+\theta u_{j+1},\qquad
\theta=(x_p-jh)/h,\quad0\le\theta\le1.
\]
Call its row \(I_p\). The generalized nodal force from a point force F is
\[
\mathbf F=I_p^TF.
\tag{B1}
\]
The acceleration contribution is \(M^{-1}I_p^TF\). For uniform nodal mass,
this includes division by \(\mu h\).

Higher-order interpolation can reduce observation error, but negative weights
may appear. Negative weights do not by themselves violate passivity when the
transpose injection is used, although they can change contact localization,
force overshoot, and convergence. Choose interpolation order from an accuracy
study, not from the assumption that more neighboring nodes are always better.

Near an endpoint, do not silently change from a high-order interpolation to
nearest-node injection. Either use a defined one-sided compatible map, include
the constrained endpoint reaction, or restrict the supported contact region.
The observation and injection maps must change together.

## Static work identity and moving-map correction

For fixed \(I_p\),
\[
F(I_p\dot{\mathbf q})=(I_p^TF)^T\dot{\mathbf q}.
\tag{B2}
\]
This equality should be tested directly using unrelated deterministic states and
forces. A force conservation test alone is insufficient: two maps can have the
same sum of weights and still disagree in power.

If \(I_p=I_p(t)\), then the contact coordinate velocity is
\[
\frac{d}{dt}(I_p\mathbf q)
=I_p\dot{\mathbf q}+\dot I_p\mathbf q.
\tag{B3}
\]
The first term is local material velocity; the second comes from motion of the
observation point. The difference is essential for a sliding finger or moving
constraint. It can exchange energy with the controller even if local string
velocity is zero. The retuning and moving-domain chapter develops a discrete
work split for this situation.

## Joint midpoint solve with a linear body

For each component, midpoint stepping can be written as a linear response to a
shared port force. Eliminating velocity in
\[
\Delta\mathbf q=k\overline{\mathbf v},\qquad
M\Delta\mathbf v/k=-K\overline{\mathbf q}
-C\overline{\mathbf v}+B_p\mathbf f
\]
gives
\[
A_q\Delta\mathbf q
=2M\mathbf v_0-kK\mathbf q_0+kB_p\mathbf f,
\quad
A_q=2M/k+C+kK/2.
\tag{B4}
\]
Here \(\mathbf f\) is the midpoint force. With symmetric positive mass and
nonnegative stiffness and damping, \(A_q\) is positive definite.

The midpoint port velocity is \(B_p^T\Delta\mathbf q/k\). Thus the velocity
response to force is
\[
Y_p=B_p^TA_q^{-1}B_p.
\tag{B5}
\]
The factors of k in B4 cancel in this velocity response. Check this explicitly;
a missing k changes mobility units and coupling strength. \(Y_p\) has units
velocity per force for a translational port.

For two connected components with opposite force signs, write their free
midpoint velocities \(\mathbf v_s^{\,0},\mathbf v_b^{\,0}\). Enforcing equality
gives
\[
(Y_s+Y_b)\mathbf f
=\mathbf v_b^{\,0}-\mathbf v_s^{\,0}.
\tag{B6}
\]
This small Schur-complement solve is a useful reference. Check rank: redundant
constraints or mechanically blocked directions can make a port matrix singular.
Do not add an arbitrary regularizer without identifying the resulting compliance.

## Heterogeneous time schemes

The explicit three-level string uses a half-step energy and centered force
velocity. A midpoint modal body uses integer-state energy and an adjacent-state
velocity mean. They can be coupled, but the joint discretization must be derived.
Connecting them with whatever samples are most convenient does not inherit
either standalone energy proof.

A safe development strategy is to use the same midpoint time representation for
both components in the first coupled reference. Then evaluate optimized or
multirate alternatives against that reference. If a scheme introduces a delay,
its stored delay energy or passivity condition must be part of the analysis.
Do not call a zero-order-held reaction force “instantaneous.”

For multirate integration, interface work should be accumulated at the substep
level and matched across rates. Averaging force and velocity separately does
not generally preserve the average of their product. A power mismatch can remain
even when average force and average velocity each look correct.

## Discrete boundary diagnostics

Measure the following quantities independently:

- Endpoint displacement and slope residuals for the intended boundary type.
- Generalized reaction force and moment.
- Work at each endpoint, with its orientation sign.
- Individual subsystem energy changes and internal port cancellation.
- Modal frequency changes caused by endpoint compliance.

A fixed-end energy test cannot validate reaction sign because endpoint velocity
is zero. Add a moving-boundary fixture with a known smooth displacement. Similarly,
a point displacement observation does not validate a moment port; use a test
with nonzero angular velocity.

Test reciprocity of a passive linear structure by swapping excitation and
observation ports expressed in the same conjugate units. The frequency response
matrix should be symmetric for the assumed reciprocal system. A nonreciprocal
result can reveal indexing, transposition, or frame errors, although certain
physical models genuinely require nonsymmetric terms and must be identified.

## Implementation procedure and rejection gates

Prepare boundary and interpolation maps outside real time. Validate support
indices, partition of unity where appropriate, coordinate transforms, and the
work identity. Precompute factorizations for fixed matrices. During a step,
assemble the common force unknown, solve the joint port problem, reconstruct
states, and calculate an independent energy residual before committing diagnostics.

Reject a model whose boundary force is extracted from an interior stencil without
deriving its physical traction. Reject endpoint overwrites after the update,
unexplained force delays, and separate interpolation/injection approximations.
Reject a coupling test that compares only the final audio gain.

The relevant primary continuum and discrete-contact source is Bilbao and Torin,
sections 2–3 of *Numerical Simulation of String/Barrier Collisions: The Fretboard*,
<https://www.pure.ed.ac.uk/ws/portalfiles/portal/17485791/dafx14_final.pdf>.
The matrix elimination in B4–B6 is a repository derivation for the specified
midpoint equations. Continue with two polarizations before general directional
contact is enabled.
