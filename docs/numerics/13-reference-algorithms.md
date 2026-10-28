# Reference algorithm blueprints and reproducible equation checks

## Status and intended use

This chapter is an implementation blueprint, not a declaration that the described
classes already exist in the repository. Code fragments are explicitly
pseudocode. Translate them into the actual source architecture only through a
scoped implementation task with tests and review. Keep the C++20 mechanical core
independent of JUCE; adapters may convert host events and audio buffers at the
boundary.

The algorithms deliberately expose preparation, state proposal, acceptance, and
diagnostics. Optimizations can later fuse operations, but they should preserve
these logical contracts. Do not optimize away the distinction between a failed
proposal and an accepted state.

## Blueprint A: prepare an explicit stiff-string configuration

Inputs are L, linear density, tension, flexural rigidity, loss coefficients,
internal sample frequency, maximum capacity, boundary variant, and proposed
stability margin. The output is either an immutable prepared configuration or
a descriptive preparation error.

1. Check that every input is finite and has its required sign.
2. Convert all external quantities to SI before computing derived coefficients.
3. Compute \(c^2=T/\mu\), \(\kappa^2=EI/\mu\), and \(k=1/f_s\).
4. Compute the scheme-specific h minimum from the explicit chapter, using a
   robust radical evaluation.
5. Choose an integer interval count satisfying both capacity and the strict
   stability margin, then set actual \(h=L/N\).
6. Check the minimum supported grid size and recompute the exact margin.
7. Prepare interior and endpoint coefficients for the chosen closure.
8. Prepare paired interpolation and force maps for stationary ports.
9. Allocate all state and work arrays outside real time.
10. Store the equation variant and every derived quantity in diagnostic metadata.

Do not silently round a target frequency into an unrelated physical length.
If the desired configuration cannot meet stability and accuracy within capacity,
return an unsupported-configuration result. The caller may choose a different
prepared quality mode, but that choice must be explicit.

Illustrative C++20 pseudocode:

~~~cpp
enum class PrepareError {
    nonFiniteInput, invalidPhysicalRange, unsupportedGrid,
    capacityExceeded, stabilityMarginFailed
};

struct StringPhysicsSI {
    double lengthM;
    double linearDensityKgPerM;
    double tensionN;
    double flexuralRigidityNm2;
    double sigma0PerSecond;
    double sigma1M2PerSecond;
};

struct PreparedExplicitString {
    std::size_t intervals;
    double actualSpacingM;
    double stepSeconds;
    double stabilityRatio;
    // Immutable coefficients and boundary-map storage follow.
};

// Pseudocode: actual result type and allocation ownership are project choices.
PrepareResult prepareExplicitString(
    const StringPhysicsSI& physics,
    double internalSampleRateHz,
    std::size_t preparedCapacity,
    double requestedMargin);
~~~

The unit suffixes are useful at API boundaries. Inner loops can use concise local
names after the preparation contract has established their meaning.

## Blueprint B: one explicit reference step

Inputs are previous and current displacement arrays, prepared coefficients, and
the generalized force at the documented integer time. Compute the next array
without modifying the two input arrays. Use a matrix-based path for small tests
and a stencil path for the candidate implementation.

After computing the proposal, evaluate finiteness and any configured diagnostic
checks. In the reference harness, compute the exact half-step energy and work
balance using independently assembled matrices. Only then rotate state ownership.
The final output observation must declare whether it represents displacement,
centered velocity, half-step velocity, or bridge force.

The force input is a physical force vector. If the stencil accepts force density
instead, convert using the quadrature matrix exactly once. Test this conversion
with a unit impulse fixture and different grid counts.

For sample-accurate host events, split processing at the event offset or consume
an event schedule inside the loop. The physical sequence must remain independent
of host block partition. Do not rebuild coefficients in the callback.

## Blueprint C: scalar conservative contact

For a free mass against a fixed barrier, the scalar residual is
\[
R(s)=s-s_{\mathrm{free}}+\gamma\overline F(s_0,s),
\qquad s_{\mathrm{free}}=s_0-kv_0,\quad\gamma=k^2/(2m).
\]
Compute \(F_u=\overline F(s_0,s_{\mathrm{free}})\). If it is zero, the free
prediction is already the root. Otherwise, use
\[
\ell=s_{\mathrm{free}}-\gamma F_u,\qquad u=s_{\mathrm{free}}.
\tag{A1}
\]
Because the divided difference is nondecreasing in its second argument,
\(R(\ell)\le0\le R(u)\). This is a finite analytic bracket for this particular
convex scalar model, not a generic multi-contact result.

Within the bracket, evaluate a Newton proposal using a verified derivative.
Accept that proposal only if finite, inside the bracket, and compatible with
the selected safeguarded strategy. Otherwise bisect. Update the bracket from
the residual sign. Stop using both residual and bracket-width criteria.

After convergence, reconstruct force, velocity, and displacement. Recompute the
potential change using an independent evaluation and check total energy.
If any evaluation overflows or a tolerance cannot be met within the prepared
iteration bound, reject the step. An analytic bracket does not excuse a poorly
scaled force calculation.

Illustrative C++20 pseudocode:

~~~cpp
struct ContactSolveResult {
    bool accepted;
    double signedCompressionM;
    double forceN;
    double residualM;
    double bracketWidthM;
    unsigned iterations;
    ContactFailure failure;
};

// Pseudocode: potential and derivative must use stable divided differences.
ContactSolveResult solveScalarContact(
    double previousCompressionM,
    double previousVelocityMPerS,
    double massKg,
    double stepSeconds,
    const PreparedContactLaw& law,
    const SolverBudget& budget) noexcept;
~~~

The residual in this eliminated scalar equation has metres, unlike the
full-coordinate force residual. Name the units in diagnostics so a tolerance
cannot be reused accidentally between the two formulations.

## Blueprint D: coupled linear midpoint ports

For each subsystem, prepare
\(A_q=2M/k+C+kK/2\) and its factorization. Solve its free displacement
increment from \(2M v_0-kKq_0\). Compute free midpoint port velocity and
the port mobility \(Y=B_p^TA_q^{-1}B_p\).

For two rigidly connected components, solve
\((Y_s+Y_b)f=v_b^0-v_s^0\). Reconstruct both increments with equal and
opposite port loads. Then update velocities and verify that port velocities
agree and internal works cancel.

Preparation should detect redundant port directions and singular constraints.
If a small regularizing compliance is intentionally introduced, store it as a
physical model element with energy, not as an anonymous numerical epsilon.
For nonlinear contacts, replace the rigid equality with the appropriate relative
coordinate and solve the resulting coupled residual.

## Blueprint E: an experiment runner

The runner should accept a fixture description, choose the specified reference
and candidate methods, initialize physical states, apply events, collect bounded
diagnostics, and write analysis results outside real time. It should not depend
on a graphical instrument panel.

Separate configuration validation from execution. Preserve rejected fixtures
with their failure reason. For accepted runs, report the actual grid, sample
frequency, precision, integrator, solver tolerances, and event semantics. A
fixture requesting one grid but silently receiving another is not reproducible.

Comparison code should compute absolute and relative state errors, energy
residual, modal frequency and decay errors, contact impulse and penetration,
and accepted-step counts. Select the relevant subset for each fixture; not
every metric applies to every model.

## Executed synthetic equation audit

The following checks were actually executed while authoring this volume, using
an in-memory Python/NumPy calculation. They verify algebraic formulas, not the
repository's engine implementation or any acoustic setar measurement.
No claim of cross-platform execution is made.

For the FDTD identity, the synthetic parameters were:

| Quantity | Value |
| --- | --- |
| Interval count | 18 |
| Length | 0.6 m |
| Linear density | 0.0008 kg/m |
| Tension | 40 N |
| Flexural rigidity | 0.00002 N m² |
| Internal sample frequency | 96000 Hz |
| Ordinary loss parameter | 0.4 1/s |
| Gradient loss parameter | 0.002 m²/s |

The old state was a sum of first and fourth sine modes with amplitudes
\(10^{-4}\) m and \(2\cdot10^{-5}\) m. The current state added
\(10^{-7}\cos(3\pi x/L)\) at interior nodes. The force vector was
\(10^{-5}\sin(2\pi x/L)\) N. This intentionally arbitrary discrete state tests
the algebra; it is not an excitation calibrated to an instrument.

The independently evaluated half-step balance residual was approximately
\(1.90\cdot10^{-21}\) J at an initial half-step energy of
\(2.67\cdot10^{-6}\) J. The computed h minimum was approximately
0.00265484 m, while actual h was 0.0333333 m. The stability margin
\(1-\eta_{\mathrm{stab}}\) was approximately 0.995033. This coarse mesh was
selected for an identity check, not as an accuracy recommendation.

A two-coordinate quartic-potential test used
\[
A_s=\begin{bmatrix}2&-0.5\\-0.5&1\end{bmatrix},\quad
q_0=(0.2,-0.3)^T,\quad q_1=(0.4,0.1)^T,\quad a=0.7.
\]
The two-point gradient's work matched the potential difference to the displayed
floating-point result of zero residual. This is one check, not a proof replacing
the algebraic derivation.

For a Hermite element of length 0.17 m, independent six-point Gauss integration
was compared with the closed-form unscaled mass, tension, and bending matrices.
Relative matrix-norm differences were approximately
\(1.56\cdot10^{-16}\), \(2.52\cdot10^{-16}\), and
\(3.74\cdot10^{-16}\), respectively.

## Reproducing the core algebra check

The following Python fragment is diagnostic code, not the synthesis engine.
It intentionally assembles matrices rather than reusing an optimized stencil.

~~~python
import math
import numpy as np

N, L, mu, T, EI = 18, 0.6, 0.0008, 40.0, 0.00002
k, sigma0, sigma1 = 1.0 / 96000.0, 0.4, 0.002
h = L / N
S = (2*np.eye(N-1) - np.eye(N-1, k=1)
     - np.eye(N-1, k=-1)) / h**2
M = mu*h*np.eye(N-1)
K = M @ ((T/mu)*S + (EI/mu)*(S @ S))
C0, C1 = 2*sigma0*M, 2*sigma1*(M @ S)
Me = M - 0.5*k*C1
x = np.arange(1, N)*h
old = 1e-4*np.sin(math.pi*x/L) + 2e-5*np.sin(4*math.pi*x/L)
now = old + 1e-7*np.cos(3*math.pi*x/L)
force = 1e-5*np.sin(2*math.pi*x/L)
nxt = np.linalg.solve(
    M + 0.5*k*C0,
    (2*M-k*k*K-k*C1) @ now
    + (-M+0.5*k*C0+k*C1) @ old + k*k*force)

def half_energy(a, b):
    velocity = (b-a)/k
    return 0.5*velocity @ Me @ velocity + 0.5*b @ K @ a

centered_velocity = (nxt-old)/(2*k)
work = k*(centered_velocity @ force
          - centered_velocity @ (C0+C1) @ centered_velocity)
residual = half_energy(now, nxt) - half_energy(old, now) - work
print(residual)
~~~

Small floating-point differences are expected across numerical libraries.
Interpret the residual relative to energy scale and arithmetic precision;
do not turn the displayed decimal into a universal bitwise golden value.

## Executed temporal-order and stability spot checks

A separate scalar-mode calculation checked the temporal-order warning in the
explicit chapter. It used undamped natural angular frequency 10 rad/s, ordinary
decay parameter 0.4 1/s, unit initial modal displacement, zero initial velocity,
and exact first-step initialization. The final physical observation was at one
second. The spatial modal eigenvalue was held fixed; the gradient-loss product
\(\sigma_1s\) was either zero or 0.7 1/s.

| Number of equal steps | Error with zero gradient loss | Error with gradient-loss product 0.7 1/s |
| --- | --- | --- |
| 100 | 0.00141517 | 0.00758961 |
| 200 | 0.000354444 | 0.00360061 |
| 400 | 0.0000887692 | 0.00175374 |
| 800 | 0.0000222169 | 0.000865463 |

The three successive observed orders were approximately 1.997, 1.997, and
1.998 without gradient loss, and 1.076, 1.038, and 1.019 with it. These measured
synthetic results support the Taylor-expansion analysis: the complete backward-
gradient-loss scheme approaches first order when that term is nonzero.
They are not a universal convergence claim for contacts or moving boundaries.

A separate modal stability check used natural angular frequency 20 rad/s,
gradient-loss product 1 1/s, and ordinary decay parameter 0.2 1/s. The exact
scalar boundary solves
\(k^2\Omega^2+4k\sigma_1s=4\), giving
\[
k_{\mathrm{crit}}=
\frac{2}{\sqrt{\Omega^2+(\sigma_1s)^2}+\sigma_1s}.
\]
At \(0.999k_{\mathrm{crit}}\), the largest amplification-root magnitude was
approximately 0.961166. At \(1.001k_{\mathrm{crit}}\), it was approximately
1.03028. This is a direct discrete-root check, not a noisy waveform estimate.

The scalar analytic contact bracket was also evaluated for several endpoint
crossings and power exponents. Every tested lower endpoint had nonpositive
residual and every upper endpoint had nonnegative residual, including a
separated zero-force case where both were zero. The algebraic monotonicity proof
remains the general justification; a finite list of examples cannot prove the
bracket for all inputs.

## Handoff and rejection gates

An implementation task should name which blueprint it implements, which
assumptions remain, which tests were executed, and which physical features are
still absent. A successful analytic modal scaffold is not a completed FDTD
instrument, and a generic contact demonstration is not validated setar fidelity.

Reject a handoff that omits actual commands and results, mixes pseudocode with
claimed implementation, or changes source outside its assigned scope. The
matching prompts in prompts/numerics turn these blueprints into implement,
review, and debug tasks. Consult docs/ROADMAP.md for the master phase gate
before promoting any result.
