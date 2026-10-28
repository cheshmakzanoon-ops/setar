# Discrete operators, dimensions, and matrix contracts

## The reason to assemble a matrix reference

A production finite-difference loop may never store a dense matrix, but a small
matrix reference is invaluable. It makes symmetry, nullspaces, eigenvalues,
endpoint closures, and force scaling inspectable. The reference implementation
should assemble the exact operator represented by the optimized loop, not a
similar operator with more convenient boundaries. This chapter specifies that
reference and the checks required before numerical time integration is trusted.

Prerequisites are the continuum power balance and a fixed geometry. Inputs are
the number of intervals, physical length, material parameters, boundary conditions,
and coordinate basis. Outputs are mass, stiffness, damping, observation, and force
maps with explicit dimensions. These are proposed contracts; an actual test report
must identify the code revision and fixtures used.

## Uniform simply supported string

Divide \(0\le x\le L\) into \(N\) intervals, \(h=L/N\). There are \(N-1\)
unknown interior displacements. Let \(\mathbf q=(u_1,\ldots,u_{N-1})^T\), with
fixed endpoint displacements \(u_0=u_N=0\). Define

\[
D_{xx}=\frac1{h^2}
\begin{bmatrix}
-2&1&&\\1&-2&1&\\&\ddots&\ddots&\ddots\\&&1&-2
\end{bmatrix},\qquad S=-D_{xx}.
\tag{O1}
\]

Here \(S\) is symmetric positive definite. Its exact eigenvalues are

\[
s_j=\frac4{h^2}\sin^2\left(\frac{j\pi}{2N}\right),
\qquad j=1,\ldots,N-1.
\tag{O2}
\]

This expression is an independent test of assembly and indexing. It also shows
\(s_{\max}<4/h^2\) for this finite string; \(4/h^2\) is a convenient upper
bound, not its exact largest eigenvalue.

With uniform nodal quadrature \(W=hI\),

\[
M=\mu W,\quad
K=W(TS+EI S^2),\quad
C=2\mu W(\sigma_0I+\sigma_1S).
\tag{O3}
\]

Then
\[
M\ddot{\mathbf q}+C\dot{\mathbf q}+K\mathbf q=\mathbf F.
\tag{O4}
\]
The vector \(\mathbf F\) contains nodal generalized forces in newtons. If a
force-density vector \(\mathbf f\) in N/m is used instead, the right side is
\(W\mathbf f\). This distinction is responsible for many grid-dependent amplitude
errors. A force remains a force when the mesh is refined; its spatial density
changes as its support narrows.

For nodal translations, \(M\) has units kg, \(C\) kg/s, and \(K\) kg/s². The
normalized acceleration operator \(M^{-1}K=c^2S+\kappa^2S^2\) has units 1/s².
The matrices are not interchangeable simply because multiplication by a constant
can be folded into the loop.

## Why composition gives a particular bending boundary

Using \(S^2\) in O3 represents a compatible simply supported bending operator.
In an interior stencil it corresponds to the familiar fourth difference. Near
an endpoint its diagonal differs from a naive five-point stencil with zero-filled
outside cells. At the first interior row, the bending diagonal is \(5/h^4\),
not \(6/h^4\), because the zero-moment condition implies an odd ghost extension
\(u_{-1}=-u_1\). The right endpoint has the corresponding extension.

Consequently, “use the five-point stencil and set all ghost cells to zero” is not
the same model. It can change eigenfrequencies and boundary work even if the
interior samples look plausible. A clamped string requires a different closure.
Do not switch boundary flags while keeping the same stiffness matrix and expect
only a cosmetic change.

For a tiny grid, explicitly multiply \(S\) by itself and compare each row against
the loop. Tiny grids expose overlapping boundary cases that large grids conceal.
The supported minimum grid size must be stated; a loop written for five distinct
neighbors cannot assume those neighbors exist when only a few interior states
remain after a parameter change.

## Factorization exposes positivity

Let \(D\) map nodal displacements to segment slopes with fixed endpoints included.
For the standard grid, \(S=D^TD\) with the appropriate incidence and scaling.
Then tension energy is a sum of nonnegative segment squares. Bending energy
using \(S^2\) is \(\frac12 EI h\|S\mathbf q\|^2\).
These factorizations are preferable to inspecting random eigenvalue samples.

A randomized quadratic-form test can detect an obvious failure, but it cannot
prove positivity. Use a structural proof, Cholesky factorization where positive
definiteness is expected, and eigenvalue checks for small independently assembled
fixtures. If a zero mode is expected, test the nullspace explicitly and use the
appropriate semidefinite criterion rather than adding a diagonal constant to
make factorization succeed.

Suppose an unconstrained body has rigid translations. Its stiffness may be
positive semidefinite while its mass is positive definite. That is physically
reasonable. Silently removing low eigenvalues can inadvertently remove compliant
body modes as well as rigid motion. Identify the geometry-induced nullspace,
then constrain or retain it according to the physical setup.

## Nonuniform quadrature and weighted adjoints

For a nonuniform grid, \(W\) is not generally a scalar multiple of identity.
An operator may be self-adjoint in the \(W\)-weighted inner product without being
symmetric in ordinary Euclidean coordinates. The required condition is
\(WA=A^TW\). Equivalently, its similarity transform
\(W^{1/2}AW^{-1/2}\) is symmetric. Confusing weighted self-adjointness with
entrywise symmetry is a common way to reject a valid operator or accept an invalid
force map.

If an interpolation row \(I_p\) maps nodal displacement to contact displacement,
the nodal generalized force is \(I_p^TF\). If the equation is written in force
density, injection is \(W^{-1}I_p^TF\). The work identity is

\[
\dot{\mathbf q}^TW(W^{-1}I_p^TF)
=F(I_p\dot{\mathbf q}).
\tag{O5}
\]

The mass matrix belongs to the dynamics, not to the interpolation definition.
Some implementations directly store the acceleration response
\(M^{-1}I_p^T\); that is acceptable only if the stored object is named and tested
as an acceleration map. Reusing it as a force map would divide by mass twice.

## Nondimensionalization without losing physical meaning

Choose positive reference scales \(L_\star,t_\star,u_\star\). Write
\(x=L_\star\hat x\), \(t=t_\star\hat t\), and \(u=u_\star\hat u\).
The normalized PDE has coefficients

\[
\hat c^2=c^2t_\star^2/L_\star^2,\quad
\hat\kappa^2=\kappa^2t_\star^2/L_\star^4,\quad
\hat\sigma_0=\sigma_0t_\star,\quad
\hat\sigma_1=\sigma_1t_\star/L_\star^2.
\tag{O6}
\]

These dimensionless groups are useful for conditioning and fixture design.
For instance, choosing \(t_\star=L_\star/c\) when \(c>0\) sets \(\hat c=1\).
Do not make that choice in a zero-tension beam fixture; select a bending scale
instead. A universal normalization formula should not divide by a parameter that
the test suite deliberately sets to zero.

Nondimensional internal coordinates are allowed, but SI quantities remain the
model boundary contract. Record the transformation for every exported state,
force, energy, and tolerance. A nonlinear solver tolerance of \(10^{-8}\) means
nothing without its normalization. An absolute displacement tolerance in metres
is not an absolute force residual tolerance in newtons.

For coordinate scaling \(\mathbf q=D\hat{\mathbf q}\), with nonsingular diagonal
\(D\), the energy-consistent transformed matrices are
\(\hat M=D^TMD\), \(\hat C=D^TCD\), and \(\hat K=D^TKD\).
The transformed force is \(\hat{\mathbf F}=D^T\mathbf F\).
Scaling only rows of the equation is a numerical preconditioner, not the same
operation as changing physical coordinates; keep those two operations distinct.

## Conditioning and solve strategy

A positive definite matrix can still be poorly conditioned. Measure a condition
estimate for representative reference systems and relate it to expected floating-
point error. Do not make an unqualified claim that Cholesky produces an accurate
physical result merely because it returns success. The residual can be small
while forward error is large in an ill-conditioned system.

Prefer solving a factorized linear system to explicitly forming an inverse.
For fixed geometry and coefficients, prepare the factorization outside the audio
callback. A low-rank contact update may permit a small Schur complement, but its
dimension and definiteness must be derived. A dense inverse assembled for a
diagnostic is not a production algorithm.

Check both residual and backward error. For \(A\mathbf x=\mathbf b\), a useful
dimensionless normwise quantity is
\[
\eta=\frac{\|A\mathbf x-\mathbf b\|}
{\|A\|\,\|\mathbf x\|+\|\mathbf b\|}.
\]
When the denominator is zero, treat the exact zero system separately. Choose
matrix and vector norms consistently and report them. The expression is not a
universal perceptual error metric; it measures the linear solve.

## A concrete assembly procedure

1. Validate finite, physically admissible SI inputs and a supported interval count.
2. Compute actual \(h=L/N\); never keep a nominal spacing inconsistent with length.
3. Assemble the small reference \(S\) and verify its endpoint rows.
4. Form reference \(M,K,C\), with independent expressions for their energies.
5. Check symmetry or weighted self-adjointness, expected nullspaces, and positivity.
6. Compare eigenvalues against O2 and continuum values in their respective limits.
7. Construct each observation map and its exact transpose force counterpart.
8. Test work equality with several deterministic vectors, including endpoint cases.
9. Compare matrix multiplication with the optimized stencil on identical states.
10. Preserve the assembly metadata with every resulting test artifact.

A proposed numerical budget for symmetry error should be scaled by matrix norm
and floating-point precision; it is not a measured setar requirement. A reviewer
must distinguish a derived zero from a tolerance selected for finite precision.
If symmetry error grows with grid size in a way not explained by rounding,
investigate assembly before loosening the budget.

## Source and next action

The matrix derivations here follow directly from the stated operators. For the
generalized symmetric-definite eigenproblem used by reference tests, the primary
LAPACK documentation specifies \(A x=\lambda Bx\) with positive-definite \(B\):
<https://netlib.org/lapack/explore-html/d1/d39/group__hegv_gadc3e6dc69532c1233818df364b7de912.html>.
Select the problem type deliberately; the other supported forms are not equivalent
without a transformation. Proceed to the passive-port chapter before coupling
these matrices to another subsystem.
