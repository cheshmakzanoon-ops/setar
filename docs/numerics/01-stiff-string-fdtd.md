# A precisely specified stiff lossy string FDTD scheme

## Scope and provenance

This chapter defines one particular three-level finite-difference scheme. Its
stability expression must not be reused for another loss discretization,
boundary closure, coupling rule, or nonlinear update without a new derivation.
Assume constant positive linear density, nonnegative tension and rigidity,
nonnegative loss coefficients, uniform spacing, and the compatible simply
supported matrix \(S=-D_{xx}\) defined in the operator chapter.

The physical PDE is the one in the continuum chapter. The requested spatial
bound agrees with equation (17) of Bilbao and Torin, *Numerical Simulation of
String/Barrier Collisions: The Fretboard*:
<https://www.pure.ed.ac.uk/ws/portalfiles/portal/17485791/dafx14_final.pdf>.
The proof here is independently written for the exact linear update below.
The source's larger nonlinear scheme is not implicitly implemented by this
linear formula.

## The exact update

Let \(A=c^2S+\kappa^2S^2\). Discretize acceleration and ordinary velocity loss
centrally, but use a backward time difference in the gradient loss:
\[
\delta_{tt}\mathbf q^n
=-A\mathbf q^n-2\sigma_0\delta_{t0}\mathbf q^n
-2\sigma_1S\delta_{t-}\mathbf q^n+\mathbf f^n/\mu.
\tag{F1}
\]
Here \(\mathbf f^n\) is a force-density vector in N/m. A generalized nodal
force vector must be divided by \(\mu h\), not merely by \(\mu\).
Rearranging gives
\[
(1+\sigma_0k)\mathbf q^{n+1}
=(2I-k^2A-2\sigma_1kS)\mathbf q^n
+[(\sigma_0k-1)I+2\sigma_1kS]\mathbf q^{n-1}
+k^2\mathbf f^n/\mu.
\tag{F2}
\]

The left side is scalar times identity, so the update is explicit for this
constant-coefficient model. “Explicit” means no solve for neighboring future
states; it does not mean all intermediate quantities are free of dependencies.
The old and current arrays must remain unchanged until the complete next state
has been computed.

For an interior point, define
\(\lambda=ck/h\), \(\beta=\kappa k/h^2\), and
\(\nu=2\sigma_1k/h^2\). Before dividing by \(1+\sigma_0k\), the current-state
coefficients are
\[
u_l^n(2-2\lambda^2-6\beta^2-2\nu)
+(u_{l-1}^n+u_{l+1}^n)(\lambda^2+4\beta^2+\nu)
-\beta^2(u_{l-2}^n+u_{l+2}^n),
\]
and the previous-state contribution is
\[
u_l^{n-1}(\sigma_0k-1+2\nu)
-\nu(u_{l-1}^{n-1}+u_{l+1}^{n-1}).
\tag{F3}
\]
Use the actual boundary rows of \(S^2\), not this interior expression with
arbitrary zero padding. A small matrix implementation is the reference for those
rows.

## Energy proof through an effective mass

Write \(M=\mu hI\), \(K=MA\),
\(C_0=2\sigma_0M\), and \(C_1=2\sigma_1MS\).
F1 is
\[
M\delta_{tt}\mathbf q+C_0\delta_{t0}\mathbf q+
C_1\delta_{t-}\mathbf q+K\mathbf q=\mathbf F^n.
\]
Using the exact time-difference identity gives
\[
M_e\delta_{tt}\mathbf q+(C_0+C_1)\delta_{t0}\mathbf q+
K\mathbf q=\mathbf F^n,\qquad M_e=M-\frac{k}{2}C_1.
\tag{F4}
\]
Define the half-step energy
\[
H^{n+1/2}
=\frac12(\delta_{t+}\mathbf q^n)^TM_e\delta_{t+}\mathbf q^n
+\frac12(\mathbf q^{n+1})^TK\mathbf q^n.
\tag{F5}
\]
Multiplication of F4 by \((\delta_{t0}\mathbf q^n)^T\) yields
\[
\frac{H^{n+1/2}-H^{n-1/2}}{k}
=(\delta_{t0}\mathbf q^n)^T\mathbf F^n
-(\delta_{t0}\mathbf q^n)^T(C_0+C_1)\delta_{t0}\mathbf q^n.
\tag{F6}
\]

This identity is exact in real arithmetic. It also specifies the conjugate
velocity for the external discrete force: the centered velocity at n. Using a
different diagnostic velocity would not test F6.

The cross-state potential in F5 can have either sign. Let
\(\mathbf d=(\mathbf q^{n+1}-\mathbf q^n)/k\) and
\(\mathbf m=(\mathbf q^{n+1}+\mathbf q^n)/2\). Then
\[
H^{n+1/2}
=\frac12\mathbf d^T\left(M_e-\frac{k^2}{4}K\right)\mathbf d
+\frac12\mathbf m^TK\mathbf m.
\tag{F7}
\]
Thus positivity follows when
\[
M-\frac{k}{2}C_1-\frac{k^2}{4}K>0.
\tag{F8}
\]
Strict positivity is preferred for a production envelope. At a marginal boundary
the energy may fail to control a highest-frequency state; lossless repeated roots
can produce algebraic growth even though their magnitudes equal one.

## Deriving the grid bound

For an eigenvalue \(s\) of S, F8 becomes
\[
1-\sigma_1ks-\frac{k^2}{4}(c^2s+\kappa^2s^2)>0.
\]
Using \(s<4/h^2\) gives the convenient sufficient condition
\[
\eta_{\mathrm{stab}}
=\frac{c^2k^2}{h^2}
+\frac{4\kappa^2k^2}{h^4}
+\frac{4\sigma_1k}{h^2}<1.
\tag{F9}
\]
The nonstrict boundary of this inequality is
\[
\boxed{
h_{\min}=
\sqrt{\frac{c^2k^2+4\sigma_1k+
\sqrt{(c^2k^2+4\sigma_1k)^2+16\kappa^2k^2}}{2}}.}
\tag{F10}
\]

Every term inside the outer square root has units m². The coefficient
\(\sigma_0\) does not appear because of its centered time treatment in F1.
This absence does not mean arbitrarily large \(\sigma_0 k\) is accurate.
The finite-grid eigenvalue in O2 can give a less conservative check, but a
specialized relaxation should not be introduced before the simpler bound and
its accuracy consequences are thoroughly tested.

Evaluate the inner radical using a hypot-like operation on
\(a=c^2k^2+4\sigma_1k\) and \(4\kappa k\) where appropriate. That avoids
unnecessary overflow from squaring two representable quantities. Validate
intermediate values and handle the all-zero dynamics case separately.

## Grid construction procedure

Choose the internal step k independently from the host block size. Select a
positive stability margin as a proposed engineering budget, then find an integer
N such that \(h=L/N\) is supported by the stencil and F9 satisfies that margin.
Because a smaller h is more demanding for an explicit scheme, increasing N at
fixed k can destabilize the model.

A practical construction starts from a spacing larger than F10, computes
\(N=\lfloor L/h_{\mathrm{candidate}}\rfloor\), and recomputes the exact
\(h=L/N\). Verify F9 again with the final values. Do not keep a nominal h that
does not divide the physical length. Reject N below the supported minimum.
Changing internal oversampling changes k and therefore the allowable grid count.

The stability margin is not an accuracy metric. A coarse string may be very
stable and badly dispersive. The final choice must also meet frequency, decay,
contact-resolution, and runtime budgets. Store N, h, k, the exact coefficients,
and the computed margin in the render metadata.

## Independent modal amplification check

For one eigenmode with eigenvalue s, let
\(a_s=c^2s+\kappa^2s^2\) and seek a solution proportional to \(r^n\).
The amplification polynomial is
\[
(1+\sigma_0k)r^2+
(-2+a_sk^2+2\sigma_1ks)r+
(1-\sigma_0k-2\sigma_1ks)=0.
\tag{F11}
\]
Evaluate these roots independently in tests. When complex conjugate roots occur,
their magnitude gives numerical decay and their argument gives numerical angular
frequency divided by k. When roots are real, describe the mode as numerically
overdamped or alternating as appropriate; extracting a fictitious sinusoidal
frequency from an FFT is misleading.

F11 provides an exact discrete reference for the implemented finite grid.
Comparison with it isolates implementation error. Comparison with the continuum
formula additionally includes spatial and temporal approximation errors. Both
comparisons are needed and must not be conflated.

## Temporal order and the centered-loss alternative

The backward derivative satisfies
\[
\delta_{t-}u=u_t-\frac{k}{2}u_{tt}+O(k^2).
\]
Therefore F1 is generally first order in time when \(\sigma_1>0\), despite
centered acceleration and ordinary damping. With \(\sigma_1=0\), smooth fixed
data and compatible initialization, the interior time discretization is second
order. A convergence report must distinguish those cases.

If the gradient loss is centered instead, the scheme becomes
\[
M\delta_{tt}\mathbf q+(C_0+C_1)\delta_{t0}\mathbf q+
K\mathbf q=\mathbf F^n.
\tag{F12}
\]
Its future-state matrix is \(M+k(C_0+C_1)/2\), requiring a linear solve rather
than F2's scalar division. Its energy uses M, not \(M_e\), and the corresponding
positive-energy bound is \(M-k^2K/4>0\); the \(\sigma_1\) term in F10 no
longer follows. F12 is a distinct second-order candidate, not a coefficient tweak.

## Initialization and sample-state meaning

Supply physical initial displacement \(\mathbf q_0\) and velocity \(\mathbf v_0\).
For a smooth test, compute acceleration from the semidiscrete continuous equation
and initialize an adjacent state with a second-order Taylor formula. Alternatively,
use the exact modal state at the first two time levels in a benchmark. The latter
isolates the recurrence from initialization error.

If the implementation begins with previous and current arrays, document the
physical times they represent. Setting previous equal to current gives zero
backward velocity but not necessarily the desired centered velocity once the first
force is applied. That distinction matters for an impulse and for energy tests.

Output interpolation should observe a declared state time. If the output represents
velocity, use the velocity definition appropriate to its purpose and account for
the half-step or centered timing. Do not mix a displacement sample from one time
with a force sample from another when estimating mobility.

## Required tests and failure behavior

Test lossless ideal propagation, stiff single modes, ordinary damping alone,
gradient damping alone, combined damping, point-force normalization, and all
boundary rows. Sweep parameters through a deliberately chosen stability boundary
in a non-production harness and verify the diagnostic detects loss of the energy
condition before the audio engine accepts the configuration.

Check F6 using independently evaluated matrices and energies. Test convergence
at fixed spatial grid with and without \(\sigma_1\). Use nontrivial initial
velocities so a mistaken initialization cannot pass by symmetry.

Reject nonfinite coefficients, unsupported N, violated margins, and inconsistent
unit conversions before processing. If a runtime state becomes nonfinite, mark
the render invalid and invoke explicit fault containment; do not clip it, inject
noise, or normalize it back into range. The next step is a compatible boundary
and coupling implementation, not an arbitrary bridge-force feedback term.
