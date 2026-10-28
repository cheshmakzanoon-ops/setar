# Stretching nonlinearities and coupled transverse motion

## Choosing the physical question

Amplitude-dependent pitch and exchange between motion directions are reasons to
investigate geometric nonlinearity. They are not reasons to replace a measured
string model with the most elaborate available PDE. The first task is to identify
which measured feature the linear model fails to explain, with excitation force,
bridge loading, and analysis uncertainty controlled. A pitch glide caused by
changing contact length is not automatically evidence for transverse stretching.

This chapter distinguishes a quasistatic global-tension model, a local polynomial
strain model with longitudinal inertia, and a geometrically exact centerline
model. They have different assumptions and costs. Do not combine the nonlinearity
from one with the energy from another.

Primary source context includes Bilbao and Torin's *Numerical Simulation of
String/Barrier Collisions: The Fretboard*, section 2 and equation (3),
<https://www.pure.ed.ac.uk/ws/portalfiles/portal/17485791/dafx14_final.pdf>;
Bilbao and Smith's *Energy-conserving finite difference schemes for nonlinear
strings*, publication record
<https://www.research.ed.ac.uk/en/publications/energy-conserving-finite-difference-schemes-for-nonlinear-strings/>;
and Ducceschi and Bilbao's *Simulation of the Geometrically Exact Nonlinear String
via Energy Quadratisation*, <https://arxiv.org/abs/2112.15008>.
The formulas below are independently derived for the explicitly stated reduced
models. A source's scheme must be read in full before importing its stability claim.

## Global tension from small-slope extension

Let \(\mathbf r=(y,z)^T\) and assume fixed axial endpoints, small slopes, and
axial stress equilibrating rapidly compared with transverse motion. The increase
in centerline length is approximated by
\[
\Delta L=\frac12\int_0^L |\mathbf r_x|^2\,dx.
\tag{N1}
\]
An axial spring of stiffness \(EA/L\) then produces an additional tension
\[
\Delta T=\frac{EA}{2L}\int_0^L|\mathbf r_x|^2\,dx.
\tag{N2}
\]
Define \(S_r=\int_0^L|\mathbf r_x|^2dx\), which has units metres. The associated
stretching potential is
\[
V_{\mathrm{str}}=\frac12\frac{EA}{L}(\Delta L)^2
=\frac{EA}{8L}S_r^2.
\tag{N3}
\]
Varying N3 gives an interior restoring force
\(\Delta T\,\mathbf r_{xx}\), with endpoint terms. The vector PDE is therefore
\[
\mu\mathbf r_{tt}
=(T_0+\Delta T)\mathbf r_{xx}-EI\mathbf r_{xxxx}
-2\mu\sigma_0\mathbf r_t+2\mu\sigma_1\mathbf r_{txx}
+\mathbf f.
\tag{N4}
\]

Both polarizations contribute to the same \(\Delta T\). Computing independent
tension changes for y and z would describe a different energy and break rotational
invariance. In an isotropic model, rotating the transverse coordinate axes must
not change \(S_r\), total energy, or predicted frequency.

N4 is often called a Kirchhoff–Carrier-type reduction. Its global coupling
represents an eliminated axial degree of freedom; it does not reproduce all
longitudinal traveling waves or every combination-frequency mechanism. It also
assumes a particular axial constraint. A compliant tuning mechanism or bridge
can alter the effective extension stiffness.

## Discrete potential and an exact two-point gradient

Let \(A_s\) be the symmetric matrix representing the integral of squared slope,
so \(S_h(\mathbf q)=\mathbf q^TA_s\mathbf q\). For two polarizations, stack their
coordinates and use the corresponding block matrix. Set
\[
V_h(\mathbf q)=a(\mathbf q^TA_s\mathbf q)^2,\qquad
a=EA/(8L).
\]
The ordinary gradient is
\[
\nabla V_h=4a(\mathbf q^TA_s\mathbf q)A_s\mathbf q.
\tag{N5}
\]
For endpoints \(\mathbf q_0,\mathbf q_1\), write
\(s_i=\mathbf q_i^TA_s\mathbf q_i\). The coefficient of a two-point gradient
must be derived rather than guessed. Since
\[
s_1-s_0=(\mathbf q_1+\mathbf q_0)^TA_s(\mathbf q_1-\mathbf q_0),
\]
the correct expression is
\[
\boxed{\overline{\nabla}V_h
=a(s_1+s_0)A_s(\mathbf q_1+\mathbf q_0).}
\tag{N6}
\]
Indeed its product with \(\mathbf q_1-\mathbf q_0\) is
\(a(s_1+s_0)(s_1-s_0)=V_h(\mathbf q_1)-V_h(\mathbf q_0)\).
At equal endpoints N6 becomes \(4as_0A_s\mathbf q_0\), agreeing with N5.
An extra factor of two would fail both this consistency check and the exact
potential-difference identity.

This discrete gradient can be inserted into the synchronized first-order
reference update from the port chapter. Its exact energy identity holds in real
arithmetic if the nonlinear equation is solved exactly. Finite solver residuals
produce a measurable work defect. The explicit three-level string update requires
its own compatible nonlinear construction; N6 is not permission to add an
arbitrary nonlinear force to that update.

## A local strain model with longitudinal inertia

For small strain with potentially important quadratic transverse slope terms,
define
\[
\epsilon=w_x+\frac12|\mathbf r_x|^2.
\tag{N7}
\]
One internally consistent potential is
\[
V=\int_0^L
\left[\frac{T_0}{2}|\mathbf r_x|^2+
\frac{EA}{2}\epsilon^2+
\frac{EI}{2}|\mathbf r_{xx}|^2\right]dx.
\tag{N8}
\]
With kinetic energy
\(\frac12\int\mu(w_t^2+|\mathbf r_t|^2)dx\), variation yields
\[
\mu w_{tt}=\partial_x(EA\epsilon),
\]
\[
\mu\mathbf r_{tt}
=\partial_x[(T_0+EA\epsilon)\mathbf r_x]
-EI\mathbf r_{xxxx},
\tag{N9}
\]
before external forces and explicitly chosen damping are added. The local axial
wave speed is of order \(\sqrt{E/\rho}\), which can be much larger than the
transverse speed. An explicit solver that resolves both fields may therefore
need a substantially different stability bound. Reusing the transverse-only
grid criterion is not justified.

The model is polynomial in strain and slope, not geometrically exact. It can
produce states outside its validity domain if longitudinal compression or large
slope develops. Monitor strain and slope as physical validity diagnostics, not
just as numerical magnitudes. A bounded state can still violate the small-strain
assumption under which N8 was introduced.

## Geometrically exact centerline kinematics

Let the deformed centerline derivative be
\(\mathbf a=(1+w_x,y_x,z_x)^T\), with stretch
\(\lambda=|\mathbf a|\). A simple axial constitutive potential can be written
\[
\psi(\lambda)=T_0(\lambda-1)+\frac{EA}{2}(\lambda-1)^2,
\tag{N10}
\]
subject to a consistent choice of prestressed reference and boundary work.
The force vector derived from this potential is
\(\psi'(\lambda)\mathbf a/\lambda\). The formula becomes singular if
\(\lambda=0\), a physically invalid collapsed segment for this model.
An implementation must reject that configuration rather than divide by a small
number and hope that damping fixes it.

N10 is a transparent constitutive example, not a complete rod theory. Adding a
small-slope Euler–Bernoulli bending term to exact axial kinematics is a modeling
hybrid whose range must be stated. Exact centerline kinematics do not automatically
give exact material behavior, torsion, shear, or contact geometry.

An auxiliary-variable or energy-quadratization method may improve solve cost,
but it introduces its own modified energy and consistency requirements. Require
convergence to the original physical model, not merely conservation of an
auxiliary quantity. A large potential shift or inconsistent auxiliary initialization
can change numerical behavior even if the code reports a nonnegative energy.

## Experimental and numerical separation

For a synthetic stretching test, initialize a smooth mode at several amplitudes
with identical physical parameters. Compare the frequency trend with an
independent high-accuracy reference. In the small-amplitude limit, the nonlinear
solution must approach the linear solution continuously. This is a model-limit
test, not a claim about the amplitude range of a real setar.

Use the same excitation energy or displacement definition across variants.
Comparing models at the same arbitrary digital output amplitude is not physically
controlled. A pickup gain or microphone normalization can conceal very different
string amplitudes and therefore different nonlinear regimes.

Test a planar initial condition in an isotropic two-polarization model. With no
out-of-plane force or asymmetric coupling, the unexcited plane should remain
zero up to rounding. Then rotate the initial condition and verify covariance.
Do not add random out-of-plane noise to hide a symmetry or coupling defect.

For the local strain model, test purely longitudinal motion with zero transverse
state. Then test small transverse motion with an independently prescribed axial
strain. Finally test the coupled problem. This staged approach separates an axial
sign error from a nonlinear interaction error.

## Solver and runtime implications

The global potential creates a low-rank-like dependence through a scalar slope
norm, but its Jacobian still has both a scaled stiffness part and an outer-product
part. Derive both before applying a rank-update formula. A missing outer product
may still let a damped iteration converge, but it is not Newton's method and can
have very different worst-case behavior.

For N5, the Hessian is
\[
\nabla^2V_h=8a(A_s\mathbf q)(A_s\mathbf q)^T+
4a(\mathbf q^TA_s\mathbf q)A_s.
\tag{N11}
\]
If \(a\ge0\) and \(A_s\ge0\), this is positive semidefinite. That fact helps
analyze an implicit reference solve. It does not prove that a finite-budget
Newton implementation converges from every predictor or for every contact state.

Preallocate work arrays and choose bounded iteration and fallback policies.
A reference solver may use stricter convergence and more expensive linear algebra
outside real time. Production optimization must be compared with that reference
on the actual intended amplitude and geometry envelope.

## Rejection gates and next action

Reject a nonlinear model if its potential is not stated, if independent y/z
tensions violate its intended rotational symmetry, if omitted longitudinal
mechanics are nevertheless claimed as reproduced, or if the reported numerical
energy does not correspond to the implemented force.

Reject a parameter fit that explains a pitch glide by allowing negative damping
or by altering unmeasured excitation energy without disclosure. Record alternative
explanations and identifiability limits. The next action is to establish contact
potentials and their nonlinear solve contracts; stretching and contact must later
be tested together, not merely as separately stable modules.
