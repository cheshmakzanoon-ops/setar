# A finite-element reference for strings and coupled structures

## Role in the project

Finite elements are useful when nonuniform geometry, mixed boundary conditions,
or structural coupling make a uniform string grid inconvenient. They also provide
an independent spatial reference for checking finite differences. A second
implementation is genuinely informative only when it is derived from the weak
form and does not copy the same boundary stencil.

This chapter gives a one-dimensional Euler–Bernoulli string/beam reference.
It does not prescribe a full three-dimensional setar body mesh. Body-specific
material anisotropy, shells, joints, and acoustic radiation belong to the body
and research documentation. The same mass, stiffness, damping, and port contracts
apply when those structures are reduced.

Inputs are a mesh, physical coefficients, essential and natural boundaries, and
loading maps. Outputs are assembled matrices, a convergence study, and a reduced
model if desired. The reference can be built outside real time; only prepared
operators or reduced coefficients should enter the audio callback.

## Weak form with boundary power intact

Let \(\psi(x)\) be an admissible test function satisfying homogeneous essential
boundary conditions. Multiply the continuum PDE by \(\psi\) and integrate by
parts to obtain
\[
\int_0^L\mu u_{tt}\psi\,dx
+\int_0^L2\mu\sigma_0u_t\psi\,dx
+\int_0^L2\mu\sigma_1u_{tx}\psi_x\,dx
+\int_0^LT u_x\psi_x\,dx
+\int_0^LEIu_{xx}\psi_{xx}\,dx
=\int_0^Lf\psi\,dx+[\tau\psi+m\psi_x]_0^L,
\tag{E1}
\]
with \(\tau=T u_x-EIu_{xxx}+2\mu\sigma_1u_{tx}\) and \(m=EIu_{xx}\).
Choosing \(\psi=u_t\) recovers the continuous power balance.

The bending term requires an \(H^2\)-conforming trial space in a direct
displacement formulation. Ordinary piecewise-linear \(C^0\) functions do not
possess the required classical square-integrable second derivative across element
interfaces. A mixed formulation or a consistent discontinuous method can use
different spaces, but it is a different derivation. Do not insert piecewise-linear
shape functions into the bending energy and ignore their interface jumps.

## Cubic Hermite element

For an element of length \(\ell\), use local coordinate \(\xi\in[0,1]\) and
degrees of freedom
\(\mathbf q_e=(u_1,\theta_1,u_2,\theta_2)^T\), where
\(\theta=u_x\) is the small slope. The shape functions are
\[
N_1=1-3\xi^2+2\xi^3,\quad
N_2=\ell(\xi-2\xi^2+\xi^3),
\]
\[
N_3=3\xi^2-2\xi^3,\quad
N_4=\ell(-\xi^2+\xi^3).
\tag{E2}
\]
Then \(u=N\mathbf q_e\). The factors of \(\ell\) in rotational shape functions
are essential; all four terms must have displacement units. Check the endpoint
value and derivative interpolation conditions directly.

For constant element coefficients, exact integration gives
\[
K_b=\frac{EI}{\ell^3}
\begin{bmatrix}
12&6\ell&-12&6\ell\\
6\ell&4\ell^2&-6\ell&2\ell^2\\
-12&-6\ell&12&-6\ell\\
6\ell&2\ell^2&-6\ell&4\ell^2
\end{bmatrix},
\tag{E3}
\]
\[
K_T=\frac{T}{30\ell}
\begin{bmatrix}
36&3\ell&-36&3\ell\\
3\ell&4\ell^2&-3\ell&-\ell^2\\
-36&-3\ell&36&-3\ell\\
3\ell&-\ell^2&-3\ell&4\ell^2
\end{bmatrix},
\tag{E4}
\]
\[
M_e=\frac{\mu\ell}{420}
\begin{bmatrix}
156&22\ell&54&-13\ell\\
22\ell&4\ell^2&13\ell&-3\ell^2\\
54&13\ell&156&-22\ell\\
-13\ell&-3\ell^2&-22\ell&4\ell^2
\end{bmatrix}.
\tag{E5}
\]

These matrices are derived by integrating \(N^TN\), \(N_x^TN_x\), and
\(N_{xx}^TN_{xx}\), respectively. Implement numerical quadrature independently
and compare it with E3–E5. That comparison catches transposed rotations and
incorrect element-length powers more effectively than copying a table into both
the implementation and its expected result.

The gradient damping matrix is
\[
C_{1,e}=2\mu\sigma_1\int_0^\ell N_x^TN_x\,dx,
\]
and ordinary damping is \(C_{0,e}=2\sigma_0M_e\).
Although \(C_{1,e}\) is proportional to \(K_T\) for nonzero constant T, assemble
it through the integral so zero-tension tests do not divide by T.

## Assembly and boundary elimination

Map element translations and slopes into a global coordinate vector. Assemble
by addition using a consistent local orientation. Reversing an element changes
the interpretation of its slope degrees of freedom; either orient every element
consistently or apply the corresponding transformation matrix.

Apply essential boundaries by eliminating or constraining the correct degrees
of freedom. A simply supported end fixes translation but leaves rotation free,
with natural zero moment. A clamped end fixes both. A prescribed nonzero
coordinate requires moving its contributions into the load vector with all
inertial and damping terms included.

Do not impose essential boundaries by replacing only a stiffness row while leaving
the mass row active. That can create a spurious constrained oscillator. Row and
column operations must preserve the intended reduced system or a properly
formulated constraint system.

For a free-free bending element, rigid translation and rigid rotation are
stiffness null modes. A tension term about a straight reference changes the
interpretation of global rotation under fixed axial constraints; select patch
tests consistent with the physical model. Do not demand a nullspace that the
chosen prestressed reference does not possess.

## Consistent versus lumped mass

The consistent mass matrix follows directly from kinetic energy. Mass lumping
can reduce solve cost, but changes eigenfrequencies and mode shapes, especially
near the resolution limit. Some naive row-sum lumping procedures behave poorly
for mixed displacement/rotation coordinates. A positive diagonal mass is not
automatically a faithful approximation.

Keep the consistent-mass system as a reference. If lumping is proposed, identify
the exact method, verify positivity, compare modal and transient errors, and
rederive any explicit stability condition from the resulting generalized
eigenvalues. Do not use a finite-difference CFL expression for a finite-element
mesh with a different mass representation.

For central differences without the backward-loss complication, a spectral
criterion involves the largest eigenvalue of \(M^{-1}K\), with strict
\(k\omega_{\max}<2\) for the lossless linear case. Nonuniform small elements
can dominate \(\omega_{\max}\). A mesh that is mostly coarse may still have a
very restrictive explicit step because of one highly refined region.

## Loads and contact ports

A point force at local coordinate \(\xi_p\) produces generalized load
\(N(\xi_p)^TF\). The corresponding contact displacement is
\(N(\xi_p)\mathbf q_e\). Their work identity is exact for the finite-element
coordinates. A force applied to a slope degree of freedom has moment units;
do not treat every entry of the element load vector as a translational force.

A distributed load uses \(\int N^Tf\,dx\). If a contact potential density is
integrated with quadrature, use the same quadrature consistently for potential,
force, and discrete gradient. Different quadrature orders can produce a force
that is not the derivative of the reported potential.

For a contact crossing an element boundary, the Hermite field is continuous in
displacement and slope for a conforming assembly. Its higher derivatives may
jump. The contact geometry and force law determine which regularity is relevant.
Test crossings explicitly rather than assuming continuity of u guarantees a
smooth contact force.

## Spatial convergence and patch tests

First verify constant and linear fields are interpolated exactly by E2.
Verify a cubic field and its derivatives within one element. Then perform an
energy comparison between analytic integrals and assembled quadratic forms.
These are element tests, not full dynamic validations.

For a simply supported constant-coefficient string, compare computed eigenvalues
with the analytic stiff-string spectrum. Refine the mesh systematically and
report errors for several modes as a function of element count. Do not compare
the highest index mode across meshes as though it represented the same physical
frequency; use a fixed set of physical modes.

Next compare a smooth transient with a modal solution and then with the
finite-difference reference. Use sufficiently accurate time integration that
spatial errors are visible. If temporal error dominates, mesh refinement can
appear to stagnate for reasons unrelated to the element.

For nonuniform meshes, refine the transition region and test energy and modal
convergence. Abrupt element-size changes can alter high-frequency behavior.
Check that a local refinement improves the intended observable rather than
merely increasing the largest numerical frequency.

## Solver and model-reduction handoff

The assembled matrices should carry metadata for mesh coordinates, material
regions, constraints, coordinate units, and port locations. Before reduction,
check symmetry, mass positivity, stiffness nullspace, and damping positivity.
Use a generalized eigenproblem, not an ordinary eigendecomposition of a
non-symmetric normalized matrix.

Export modal shapes at mechanical ports together with frequencies and damping.
Do not export only a frequency list and later invent coupling gains. Preserve
the normalization convention so a reduced body can exchange physical power
with strings.

A high-resolution finite-element model can be an offline reference while the
production engine uses a passive modal reduction. That division should be
explicit. A successful offline solve is not evidence that the same full matrix
can be solved within an audio callback.

## Failure gates and primary-source boundary

Reject negative mass eigenvalues, unexplained stiffness asymmetry, boundary
constraints applied to the wrong coordinates, inconsistent load units, and
quadrature that does not match the stated energy. Reject an element study whose
reference uses the same incorrect matrices.

The weak form and element matrices above are explicitly derived from the
continuum energy in this repository. The generalized eigensolver contract is
documented by LAPACK at
<https://netlib.org/lapack/explore-html/d1/d39/group__hegv_gadc3e6dc69532c1233818df364b7de912.html>.
For structural time integration, the primary Newmark paper is
<https://doi.org/10.1061/JMCEA3.0000098>; the next chapter states the exact
parameter conventions and limitations used here rather than relying on a method
name alone.
