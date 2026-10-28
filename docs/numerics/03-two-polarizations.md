# Two transverse polarizations and directional mechanics

## Why two planes are not two unrelated instruments

An acoustic string can move in two transverse directions. A one-plane model is
valuable for verification, but it cannot represent arbitrary pluck direction,
elliptical motion, directional bridge response, or contact geometry that couples
normal and tangential motion. Adding a second displacement array is only the
first step. The two planes share string identity, geometry, excitation, and often
nonlinear tension.

This chapter defines a two-polarization numerical contract while keeping
longitudinal and torsional motion optional evidence-driven extensions. Its inputs
are a calibrated transverse frame, per-direction physical operators, common
material and geometric parameters where appropriate, and directional port maps.
Outputs are paired states, energy and covariance tests, and a coherent contact
interface. No claim of measured setar polarization behavior is made without
instrument data.

## Isotropic baseline

Stack nodal coordinates as
\[
\mathbf q=\begin{bmatrix}\mathbf y\\\mathbf z\end{bmatrix},
\qquad
M_2=\begin{bmatrix}M&0\\0&M\end{bmatrix},
\quad
K_2=\begin{bmatrix}K&0\\0&K\end{bmatrix}.
\]
Use the corresponding block damping matrix for the simplest isotropic baseline.
The mechanical energy is the sum of the two plane energies. In the linear
uncoupled interior, either plane may be updated separately for efficiency.
They still meet at directional contacts and at the bridge.

For a homogeneous circular wire with symmetric ideal boundaries, this baseline
has degenerate modal pairs. A fixed linear polarization remains fixed in the
absence of symmetry-breaking forces or nonlinear effects. A rotating coordinate
system should not create a beat frequency. If the solver produces a split pair
solely because one array is updated first, that is a numerical defect.

Do not introduce an arbitrary detuning between planes to obtain a richer sound.
If a split is desired as a physical model, represent its cause or label it as an
effective measured anisotropy with provenance and uncertainty.

## Anisotropy and cross coupling

A general linear transverse constitutive model can use symmetric \(2\times2\)
tension, bending, or damping matrices. For example, a constant positive
semidefinite bending matrix \(B_t\) gives potential
\[
V_b=\frac12\int_0^L\mathbf r_{xx}^TB_t\mathbf r_{xx}\,dx.
\]
Its symmetry guarantees that force derives from this quadratic potential.
Off-diagonal terms couple y and z. The matrix eigenvectors define principal
bending directions; their orientation is a physical parameter, not a plotting
choice.

A damping matrix should be positive semidefinite in the same coordinate system
used for velocity. Assigning negative off-diagonal entries is not inherently
wrong, but the full quadratic form must remain nonnegative. Checking only that
diagonal entries are positive is insufficient. For a two-by-two symmetric
matrix, verify nonnegative diagonal entries and determinant, with care near
semidefinite cases.

Bridge coupling may be the dominant source of directional asymmetry even if
the string interior is isotropic. Avoid fitting both an arbitrary string
anisotropy and arbitrary bridge anisotropy to the same small dataset without an
identifiability study.

## Directional excitation

Let a point force magnitude F act in the unit transverse direction
\(\mathbf n=(n_y,n_z)^T\). With scalar spatial interpolation I, the generalized
force is
\[
\mathbf F_q=
\begin{bmatrix}n_y I^T\\n_z I^T\end{bmatrix}F,
\]
and the conjugate velocity is
\[
v_p=n_y I\dot{\mathbf y}+n_z I\dot{\mathbf z}.
\]
Their product is the mechanical input power. Normalize the direction vector
only after validating that its norm is nonzero. An almost-zero direction is an
invalid control input, not a reason to amplify floating-point noise.

If a plucking nail has its own dynamics, its force direction may change as
contact geometry evolves. A prescribed rotating direction can do geometric work.
Use a mechanically derived relative coordinate or include the controller's work.
Do not update the normal after solving contact and then apply the old scalar
force along the new direction.

## Contact normal and tangential coordinates

For a locally planar contact, define a normal and an orthogonal tangent in the
transverse plane. The relative normal displacement determines separation and
compression; relative tangential velocity enters friction. Their generalized
forces must use the transpose of the same geometric map.

When the normal varies with position, the potential gradient includes the
derivative of the gap function. In general it is not enough to compute a gap
using one geometry and a force direction using another. The gradient of the
implemented gap is the reference for force direction. Check it with directional
finite differences outside the callback, with a step sweep to separate truncation
from rounding error.

For a curved fret or nail, use signed distance only within a region where its
normal is well defined. Corners, self-intersections, and discontinuous nearest-
point choices need an explicit contact model. A normal that jumps between two
facets can create an abrupt state-dependent force direction even when penetration
is continuous.

## Shared nonlinear stretching

For the global-tension model,
\[
\Delta T=\frac{EA}{2L}
\left(\|\partial_x y\|_{L^2}^2+\|\partial_x z\|_{L^2}^2\right).
\]
Both planes use the same \(\Delta T\). Its quartic potential is a function of
the combined slope norm. The exact discrete gradient in the nonlinear-stretching
chapter applies to the stacked coordinate vector.

This coupling creates energy exchange possibilities through the common tension
and through external directional loads, but it does not reproduce every mechanism
in a three-dimensional extensible string. In particular, eliminating longitudinal
motion removes its traveling-wave states. State the reduction when comparing
sidebands or combination frequencies with measurements.

## Modal and polarization diagnostics

A scalar FFT of one output can conceal energy moving into the other plane.
Record both directions at at least one declared physical point, plus total
energy and bridge power. For a nearly monochromatic motion, inspect the
two-dimensional trajectory and its principal axes, but do not reduce broadband
motion to one ellipse without explaining the analysis window.

To distinguish true modal splitting from numerical error, compare the two-plane
linear operator's eigenvalues with the time-domain response. Rotate the entire
fixture, including its bridge and excitation maps, and verify that eigenvalues
are invariant. Rotating only the plotted output is not the same experiment.

A proposed polarization metric might include energy ratio between planes,
cross-spectral coherence, principal-axis orientation, and precession rate.
These are analysis choices; their acceptable errors require a declared budget
and empirical relevance. Do not present a visually appealing trajectory as a
validated physical behavior.

## Data layout and synchronization

Structure-of-arrays storage often suits vectorized stencil updates, whereas
small interleaved vectors may suit local geometry calculations. The layout
choice is an implementation matter, provided both planes observe the same
physical time. A solver must not use the newly updated y state with the old z
state when computing a shared nonlinear tension unless that partitioned method
has been intentionally derived.

Precompute direction and interpolation maps for stationary contacts. For moving
contacts, update them in a staged immutable control snapshot and solve with a
consistent snapshot. A mid-callback control race can create a force whose y and z
components belong to different geometries.

Keep diagnostics in double precision even if a later production state uses a
different precision, subject to profiling and explicit error tests. This does not
repair an inconsistent solver; it makes the inconsistency easier to observe.

## Required test sequence

1. Run the one-plane reference in y with z exactly zero.
2. Repeat in z and compare after exchanging coordinate labels.
3. Excite a fixed oblique direction in the isotropic linear model.
4. Rotate that fixture by several nontrivial angles and test covariance.
5. Add a known symmetric anisotropy and compare generalized eigenvalues.
6. Add a directional bridge port and verify total power cancellation.
7. Add shared stretching and verify the small-amplitude linear limit.
8. Add a stationary normal contact, then tangential loss if justified.
9. Add moving geometry only after its work channel is defined.

Negative tests should deliberately swap one force component's sign and deliberately
update planes asynchronously. The energy and covariance checks must detect both.
If they do not, the tests are not exercising the intended coupling.

## Rejection gates and source boundary

Reject independent unexplained plane detuning, inconsistent normals, duplicated
energy accounting, and artificial random excitation added to conceal a missing
coupling mechanism. Reject claims that a two-plane string automatically reproduces
all longitudinal, torsional, or body-mediated effects.

Bilbao's *Conservative Numerical Methods for Nonlinear String Dynamics: Non-planar
Vibration*, <https://arxiv.org/abs/1905.09235>, is primary background for
conservation considerations in nonplanar motion. The block-matrix and port
formulations here are derived from the stated energy. The next action is to
establish state and work handling for retuning and moving frets.
