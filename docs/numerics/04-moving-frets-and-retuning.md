# Moving frets, retuning, changing domains, and state transfer

## Treat controls as physical operations

A fretting gesture, tuning change, bend, or slide alters the mechanical problem.
Its numerical implementation must say what moved, what parameter changed, what
state persisted, and where work entered. A sequence of plausible pitches is not
enough: the physical model is also expected to retain residual string motion,
sympathetic response, contact memory, and afterlength behavior.

The preferred reference starts with a fixed full string domain and evolving
contacts. This is not a universal production mandate, but it avoids a common
mistake: shortening an allocated string on every fret event and silently deleting
the motion behind the fret. A shortened-domain model is a legitimate reduction
only after its omitted mechanics and state-transfer error are quantified.

Inputs include the control trajectory in physical units, a contact geometry
description, the chosen parameter semantics, and the current physical state.
Outputs include a new state, explicit parameter work, projection diagnostics,
and a validity decision. These operations belong in the engine contract rather
than in a JUCE parameter callback.

## Tension changes at fixed domain

With tension-dependent potential
\[
V_T(\mathbf q,T)=\frac12T\,\mathbf q^TA_s\mathbf q,
\]
continuous external work is
\(\dot T\,\mathbf q^TA_s\mathbf q/2\). For a discrete step, let
\(s_i=\mathbf q_i^TA_s\mathbf q_i\), and use arithmetic endpoint means.
The exact product identity gives
\[
\Delta V_T
=\frac12\overline T(s_1-s_0)
+\frac12\overline s(T_1-T_0).
\tag{R1}
\]
The first term is mechanical potential change due to state motion, represented
by the force using \(\overline T A_s\overline{\mathbf q}\). The second is
parameter work by the tuning mechanism. It can be positive or negative.

This two-point accounting is preferable to evaluating \(\dot T\) from noisy
finite differences after the fact. It also reveals that a parameter change at
fixed displacement changes stored energy even when the numerical state arrays
are untouched. “State continuity” and “energy continuity” are different claims.

For the explicit FDTD scheme, changing tension also changes the admissible grid
condition and the modified discrete energy. A bound checked only at initialization
is insufficient. A conservative parameter envelope can fix one grid valid for
all supported tensions; otherwise a controlled reconfiguration path is needed.
Interpolating coefficients without checking the entire trajectory is not enough.

## What a pitch control means

Several possible controls must remain distinguishable:

- Tuning changes equilibrium axial tension and usually affects all modes through
  the string dispersion relation.
- A stopped note changes contact constraints and the effective speaking region.
- A bend changes geometry and may change tension, length, and contact state.
- A reduced modal pitch parameter changes selected eigenvalues according to an
  approximation whose physical correspondence must be documented.
- A final audio resampler changes playback pitch, not the underlying mechanics.

A physical-model engine should not silently substitute the last operation for
the first three. A limited reduced model may expose a frequency-scale parameter,
but its label and validation must not imply a mechanically modeled bend.

If tension is inferred from a desired ideal fundamental using
\(T=\mu(2Lf_1)^2\), remember that this formula neglects stiffness and compliant
boundaries. For a stiff simply supported string, solve the appropriate frequency
relation; check that the resulting T is nonnegative. A target below the pure
bending fundamental cannot be reached by reducing tension within that model.

## Moving interpolation and exact geometric work

For signed compression \(s=b-G\mathbf q\), where b is the surface position and
G maps string displacement to the contact direction,
\[
\Delta s=\Delta b-\overline G\,\Delta\mathbf q
-(\Delta G)\overline{\mathbf q}.
\tag{R2}
\]
This is an exact bilinear identity for endpoint averages. Let \(\overline F\)
be the two-point potential force, so
\(\Delta\Phi=\overline F\,\Delta s\). Apply generalized string force
\(\overline G^T\overline F\). The sum of string work and contact potential
change is
\[
W_{\mathrm{control}}
=\overline F[\Delta b-(\Delta G)\overline{\mathbf q}].
\tag{R3}
\]

R3 accounts for externally moved surface and contact location. If the other
surface is a dynamic object, its corresponding state and force should be included
instead of classifying all its motion as external control. For vector contacts,
apply the identity with the correct force/displacement pairing.

Changing a normal is also a change of G. The exact bilinear identity is helpful
for a prescribed map, but a geometry-dependent normal may require the full
nonlinear gap gradient. Do not use R3 to avoid differentiating the actual geometry.

## Crossing spatial cells

Linear interpolation weights are continuous as a point moves across a cell
boundary, but their derivatives are piecewise constant and can jump. This can
affect high-frequency contact behavior. Higher-order maps may improve smoothness,
but only if their endpoint treatment and transpose injection remain compatible.

Test a contact trajectory that repeatedly crosses nodes in both directions.
Compare the trajectory's exact physical location with the interpolated location.
Record force, work, gap, and solver iterations near each crossing. A click that
occurs at grid crossings rather than at physical events is strong evidence of a
numerical artifact.

Do not snap fret locations to the nearest node and then claim continuous
intonation control. A static snapped model can be a development fixture, but its
pitch quantization and contact error must be visible. The same caution applies
to switching interpolation support discontinuously for performance.

## Remeshing is a state transformation

Suppose a configuration change requires a new grid. The old and new states
represent functions in different approximation spaces. Copying sample values by
index does not preserve the physical field. Nearest-neighbor resampling can
change slope and curvature energies substantially, particularly for a stiff
string.

For fixed physical mass and domain, an \(L^2\) projection of velocity onto the new
space can be formed by
\[
M_{\mathrm{new}}\mathbf v_{\mathrm{new}}
=B_{\mathrm{cross}}\mathbf v_{\mathrm{old}},
\tag{R4}
\]
where \(B_{\mathrm{cross}}\) integrates products of old and new basis functions
with physical density. An exact orthogonal projection does not increase the
corresponding kinetic norm. It does not automatically control bending or tension
energy in the displacement field.

Project displacement using a norm appropriate to the physical question, possibly
an energy norm with boundary constraints. If a projection changes the physical
domain, mass, or equilibrium, the two energies do not even live on the same
unchanged model. Derive the transformation and its work interpretation explicitly.

Compute energy before and after projection and record the difference separately.
Do not globally rescale the new state to match the old energy: that can preserve
one scalar while corrupting phase, momentum, local contact conditions, and the
distribution between modes. An energy-aware projection is an optimization problem
with stated constraints, not a post-hoc normalization.

## Topology and constraint activation

Activating an ideal rigid constraint on an incompatible velocity can represent
an impact. A velocity projection may dissipate energy according to an assumed
impact law. It should not be described as lossless unless its compatibility and
conservation conditions are met. A compliant penalty contact avoids an
instantaneous constraint switch, but requires its own stiffness and penetration
study.

Releasing a contact should not reset the string displacement or velocity.
Stored contact energy and opposing-object motion determine release behavior.
If a reduced contact model removes an internal state on release, document where
its remaining energy goes and test the limit.

Changing a modal basis has the same state-transfer issue as remeshing. Project
both displacement and velocity into the new basis, preserve rigid or constrained
components as appropriate, and account for discarded energy. Updating eigenvalues
while leaving coefficients unchanged may be intentional parameter work, but
changing eigenvectors without transforming coordinates is generally a different
physical state.

## Runtime architecture

Prepare candidate configurations and projection maps outside the audio callback.
Validate their physical range, matrix properties, and stability margin. Transfer
an immutable prepared object through a real-time-safe mechanism. Apply the
transition at a documented sample boundary and record its event identity.

The callback must not allocate a new grid, factor a large matrix, or run an
unbounded geometry search because a host automation value changed. If a requested
transition is unsupported, report that condition and use an explicit product
policy; do not silently accept an invalid grid.

Control smoothing may be useful for interpolation quality or to model finite
actuator motion. Its bandwidth and physical meaning should be explicit. A smoother
does not replace work accounting or a stability check, and an emergency
coefficient limiter must not silently rewrite an intended performance.

## Verification procedure

Use a synthetic string with nonzero displacement and velocity. Change tension
while holding the state fixed and verify R1. Then integrate with a smooth tension
trajectory and verify total work. Repeat with both increasing and decreasing
tension, and with a modal state whose slope energy is analytically known.

For moving contacts, test a stationary string under a moving observation point:
the material velocity is zero but the geometric term need not be. Test a moving
surface with fixed observation map, then both together. Check R2 algebraically
on unrelated endpoint data before testing a full nonlinear solve.

For remeshing, project a low smooth mode, a multimode state, and a localized
smooth disturbance. Compare displacement, velocity, strain energy, and physical
observations before and after. Repeat old-to-new-to-old to expose nonreversible
loss. A small displacement error alone does not establish a good stiff-string
transfer because curvature energy weights short wavelengths strongly.

## Rejection gates and source boundary

Reject unexplained energy jumps, state resets at fret changes, unvalidated
coefficient envelopes, and projections that conceal their loss. Reject a
supposed continuous slide that is implemented as a sequence of unrelated
restarted notes.

Bilbao and Torin explicitly discuss moving finger distributions and their supplied
power in section 2.4 of *Numerical Simulation of String/Barrier Collisions: The
Fretboard*, <https://www.pure.ed.ac.uk/ws/portalfiles/portal/17485791/dafx14_final.pdf>.
R1–R4 are repository derivations for the conventions above. The next step is
modal reduction, where state coordinates and truncation require the same care.
