# Modal contact solver of the playable prototype: scheme, energy and verified limits

**Status: `verified` for the declared synthetic scope of the numerical building
blocks; not `validated`.** This chapter documents the time-stepping core under the
playable four-string prototype: an exact-modal oscillator bank, scalar interaction
channels with a discrete-gradient force, and a bounded small dense Newton solve
([`numerics.hpp`](../../include/setar/instrument/numerics.hpp),
[`numerics.cpp`](../../src/instrument/numerics.cpp)). The tests below check these
blocks in isolation against independent oracles. They do **not** verify the
assembled instrument (string, bridge, body, nail, finger and fret channels together);
that belongs to the engine tests in
[`engine_tests.cpp`](../../tests/instrument/engine_tests.cpp). Nothing here describes
a measured setar, and every coefficient in the tests is a synthetic value.

Traceability: the energy and failure clauses of
[REQ-STR-002](../../roadmap/requirements.json) (energy-stable time stepping), the
contact-law clause of REQ-EXC-001 (finite nail/flesh excitation), the conjugate
force/velocity clause of REQ-CPL-001 (power-conjugate bridge connection), and the
work orders `ENG-P05-T02` (finite-contact solver and release-state machine) and
`ENG-P09-T01` (bidirectional string–bridge–body step) in
[`engineering-tasks.json`](../../roadmap/engineering-tasks.json). This chapter
supplies a building block for those work orders; it does not complete them.

## Prerequisites, sources, inputs and outputs

Read [time integration](07-time-integration.md),
[nonlinear algorithms](09-nonlinear-algorithms.md),
[energy and passive ports](../mathematics/03-energy-and-passive-ports.md) and
[contact potentials](../mathematics/05-contact-potentials-and-solvers.md) first.

The method follows an established line of work; the derivations below are written
out for this code rather than quoted, and their sign conventions are the ones in
[`config.hpp`](../../include/setar/instrument/config.hpp).

| Source | Used for | Verification of the reference |
| --- | --- | --- |
| O. Gonzalez, "Time integration and discrete Hamiltonian systems", *J. Nonlinear Sci.* 6, 449–467 (1996), DOI [10.1007/BF02440162](https://doi.org/10.1007/BF02440162) | Discrete gradient: a difference quotient of the potential that makes the discrete energy change exact | Citation and DOI confirmed by search against the Springer record |
| V. Chatziioannou and M. van Walstijn, "Energy conserving schemes for the simulation of musical instrument contact dynamics", *J. Sound Vib.* 339, 262–279 (2015), DOI [10.1016/j.jsv.2014.11.017](https://doi.org/10.1016/j.jsv.2014.11.017); preprint [arXiv:1501.01493](https://arxiv.org/abs/1501.01493) | Two-point (\(n-1\), \(n+1\)) discrete-gradient contact force, scalar Newton solve, collisions of strings and bars with obstacles | DOI taken from the preprint's publication notice; the Elsevier page itself was not reached |
| M. van Walstijn and J. Bridges, "Simulation of distributed contact in string instruments: a modal expansion approach", *Proc. 24th EUSIPCO* (2016), pp. 1023–1027, DOI [10.1109/EUSIPCO.2016.7760403](https://doi.org/10.1109/EUSIPCO.2016.7760403) | Contact forces in a modal string framework with exact modal frequencies and damping | Citation from the authors' institutional record; full text not read in this session |
| M. van Walstijn, J. Bridges and S. Mehes, "A real-time synthesis oriented tanpura model", *Proc. 19th Int. Conf. Digital Audio Effects (DAFx-16)*, Brno (2016), pp. 175–182 | Real-time modal string with exact modal parameters and simplified bridge/thread contact | Citation from search metadata; no DOI found; full text not read in this session |
| S. Bilbao, *Numerical Sound Synthesis*, Wiley (2009), ISBN 978-0-470-51046-9, ch. 3 "The oscillator" and ch. 7 "Linear bar and string vibration" | Three-level oscillator schemes and their energy; stiff-string modal relation | Chapter titles confirmed from the publisher's table of contents; no DOI resolved |

The search tools could not reach the conference PDFs from this environment, so the
two 2016 papers are cited as lineage only. No equation, parameter or figure is copied
from them. Read the originals before attributing a specific formula to them.

| Item | Location |
| --- | --- |
| Inputs | Modal masses (kg), undamped angular frequencies (rad/s), amplitude decay rates (1/s), time step (s); per channel a `ChannelLaw`, the previous relative coordinate \(z^{n-1}\) (m), the free prediction and the compliance matrix |
| Outputs | Next modal displacements, channel forces (N), Newton status (`iterations`, `residual_m`, `converged`, `finite`) and the energy terms used by the engine ledger |
| Tests | Ten `solver.*` CTests in [`numerics_tests.cpp`](../../tests/instrument/numerics_tests.cpp) |
| Oracles | [`instrument_oracles.hpp`](../../tests/support/instrument_oracles.hpp): closed-form free and sinusoidally forced damped oscillators, the point-spring string eigenvalue equation, a single-sinusoid frequency estimator, Simpson quadrature. It includes no instrument header. |

## 1. Exact-modal oscillator scheme

### Continuous mode

One retained mode with modal mass \(m\) (kg), undamped angular frequency
\(\omega\) (rad/s) and amplitude decay \(\sigma\) (1/s), driven by a generalized
force \(f\) (N for a mode shape of unit displacement), obeys
\[
m\left(\ddot q+2\sigma\dot q+\omega^2 q\right)=f,\qquad 0\le\sigma<\omega .
\]
For a string mode \(\omega_n^2=(T/\mu)k_n^2+(EI/\mu)k_n^4\) and
\(\sigma_n=\sigma_0+\sigma_1k_n^2+\sigma_2\omega_n^2\), \(k_n=n\pi/L\), with
\(m=\mu L/2\) for the shape \(\sin(k_nx)\). For a body mode \(\sigma=\zeta\omega\).
The free solution is \(e^{-\sigma t}(C_1\cos\omega_d t+C_2\sin\omega_d t)\),
\(\omega_d=\sqrt{\omega^2-\sigma^2}\).

### Discrete mode

With step \(k=1/f_s\), the bank uses the three-level recursion
\[
q^{n+1}=a\,q^{n}-b\,q^{n-1}+g\,f^{n},\qquad
a=2e^{-\sigma k}\cos(\omega_d k),\quad b=e^{-2\sigma k},\quad g=\frac{k^2(1+b)}{2m}.
\tag{M1}
\]
The characteristic roots of the homogeneous part are \(e^{(-\sigma\pm i\omega_d)k}\),
so the free discrete response equals the continuous response sampled at \(t=nk\):
there is no frequency warping and no numerical damping of a free mode. That is the
sense of "exact" here; it does not extend to forced or coupled motion.

(M1) is algebraically the centred difference scheme
\[
\delta_{tt}q+2\tilde\sigma\,\delta_{t\cdot}q+\tilde\omega^2q^{n}=f^{n}/m,
\tag{M2}
\]
with \(\delta_{tt}q=(q^{n+1}-2q^n+q^{n-1})/k^2\) and
\(\delta_{t\cdot}q=(q^{n+1}-q^{n-1})/(2k)\). Multiplying (M2) by \(k^2\) gives
\((1+\tilde\sigma k)q^{n+1}=(2-\tilde\omega^2k^2)q^n-(1-\tilde\sigma k)q^{n-1}+k^2f/m\),
so matching (M1) requires
\[
\tilde\sigma k=\frac{1-b}{1+b}=\tanh(\sigma k),\qquad
\tilde\omega^2k^2=2-\frac{2a}{1+b}=\frac{4\left(\sinh^2(\sigma k/2)+\sin^2(\omega_dk/2)\right)}{\cosh(\sigma k)},
\qquad g=\frac{k^2}{m(1+\tilde\sigma k)}.
\]
The library evaluates the right-hand forms, which have no cancellation for small
\(\omega k\); the test derives the coefficients from the left-hand forms instead.
Admissibility, checked by `modal_coefficients` with `std::invalid_argument`, is:
finite \(m>0\), \(\omega>0\), \(0\le\sigma<\omega\), \(k>0\) and \(\omega_d k<\pi\).
An overdamped or super-Nyquist mode is rejected, never clamped; the engine decides
which modes to retain *before* calling the bank.

### Energy and dissipation identity

Multiply (M2) by \(m\,\delta_{t\cdot}q\). With
\(\delta_{tt}q\,\delta_{t\cdot}q=\delta_{t-}\big[\tfrac12(\delta_{t+}q)^2\big]\) and
\(q^n\delta_{t\cdot}q=\delta_{t-}\big[\tfrac12 q^{n+1}q^n\big]\),
\[
H^{n+1/2}=\frac m2\left(\frac{q^{n+1}-q^{n}}{k}\right)^2+\frac m2\tilde\omega^2q^{n+1}q^{n},
\]
\[
H^{n+1/2}-H^{n-1/2}=-\underbrace{2m\tilde\sigma k\left(\delta_{t\cdot}q\right)^2}_{D^n\ \ge 0}
+\underbrace{f^n\,\frac{q^{n+1}-q^{n-1}}{2}}_{W^n}.
\tag{E1}
\]
Writing \(H=\tfrac m2(1-\tilde\omega^2k^2/4)(\delta_{t+}q)^2+\tfrac m2\tilde\omega^2
\big(\tfrac{q^{n+1}+q^n}2\big)^2\) shows \(H\ge0\) whenever \(\tilde\omega k<2\).
Because \(\cos(\omega_dk)/\cosh(\sigma k)>-1\) for every admitted mode,
\(\tilde\omega^2k^2<4\) always holds, and \(H\) is a nonnegative norm of the two-level
state. `ModalBank::energy_j` returns \(H\) between `current` and `previous`;
`step_dissipation_j` returns \(D^n\) before `commit`.

## 2. Interaction channels and the discrete-gradient force

A channel is a scalar relative coordinate
\[
z=B\,q-u(t)\quad\text{(m)},
\]
where \(B\) is a row of mode-shape values (possibly spanning both polarizations of
a string and body modes) and \(u\) a prescribed obstacle position. Its potential
\(V(z)\) (J) and damping coefficient \(R\ge0\) (N·s/m) are one of:

| `ChannelLawKind` | \(V(z)\) | Physical use in the prototype |
| --- | --- | --- |
| `OneSidedPower` | \(\frac{K}{\alpha+1}\max(0,-z)^{\alpha+1}\), \(K\ge0\) (N/m\(^\alpha\)), \(\alpha\ge1\) | nail, finger pad, fret obstacle; penetration is \(-z\) |
| `LinearSpring` | \(\frac K2(z-z_0)^2\) | bridge connection, horizontal fret stick |
| `Damper` | \(0\) | lateral flesh damping |

The generalized force on \(z\) at level \(n\) is the two-point discrete gradient
with \(s=z^{n+1}-z^{n-1}\):
\[
F(s)=-\frac{V(z^{n-1}+s)-V(z^{n-1})}{s}-\frac{R}{2k}s,
\qquad\text{so}\qquad
F\,s=-\left(V^{n+1}-V^{n-1}\right)-\frac{R}{2k}s^2 .
\tag{C1}
\]
It is applied to the modes as \(f=B^{\mathsf T}F\). For the linear spring the quotient
is exactly \(K(z^{n-1}-z_0+s/2)\), the trapezoidal average of the spring force over
\(n\pm1\).

**Small \(s\).** When \(|s|\le10^{-7}(|z^{n-1}|+|z^{n-1}+s|)\) the code uses the
tangent \(V'(z^{n-1}+s/2)\). The secant–tangent difference is
\(s^2V'''/24+O(s^4)\), relative error at most \(\alpha(\alpha-1)(2\cdot10^{-7})^2/24\) of
\(V'\), far below the \(\varepsilon\,|z|/|s|\) cancellation that the direct quotient
would suffer. A sign change of \(z\) across the step always takes the direct branch
because then \(|s|=|z^{n-1}|+|z^{n+1}|\).

**Monotonicity.** For convex \(V\) the secant slope is nondecreasing in its endpoint,
so \(dF/ds\le0\). The derivative used by Newton is
\(dF/ds=-(V'(z^{n-1}+s)-G)/s-R/(2k)\), \(G\) the quotient, with the tangent limit
\(-\tfrac12V''-R/(2k)\). When both levels are separated (\(z^{n-1}\ge0\),
\(z^{n-1}+s\ge0\)) a one-sided channel exerts exactly \(-Rs/(2k)\). The engine
contract is to pass \(R=0\) for a contact that is not in contact at level \(n\);
the gate is then fixed during the solve, so damping stays passive and a separated
contact exerts exactly zero force.

## 3. The coupled step and its channel equation

All modes predict freely, \(\hat q^{n+1}=a\odot q^n-b\odot q^{n-1}\). With
\(G=\operatorname{diag}(g)\) and the \(m\) active channels stacked in \(B\),
\[
q^{n+1}=\hat q^{n+1}+G B^{\mathsf T}F,\qquad
z^{n+1}=\hat z^{n+1}+MF,\qquad M=BGB^{\mathsf T}\ (\text{m/N, symmetric PSD}),
\]
and the unknown increments satisfy
\[
R(s)=s-b-MF(s)=0,\qquad b=\hat z^{n+1}-z^{n-1}.
\tag{N1}
\]
`ModalBank::compliance` returns one bank's \(\sum_i g_i B_{ci}B_{di}\); the engine
sums the banks a pair of channels shares. `SmallNewtonSolver::solve` receives the
assembled \(M\) row-major.

### Existence and uniqueness

Let \(\Gamma_c(s_c)=-F_c(s_c)\), nondecreasing, and \(\Lambda_c'=\Gamma_c\). With modal
increments \(y=q^{n+1}-\hat q^{n+1}\) we have \(s=By+b\) and
\(y=-GB^{\mathsf T}\Gamma(s)\), which is the stationarity condition of
\[
\Phi(y)=\tfrac12y^{\mathsf T}G^{-1}y+\sum_c\Lambda_c(B_cy+b_c).
\]
\(G^{-1}\) is positive definite and each \(\Lambda_c\) is convex, so \(\Phi\) is strictly
convex and has exactly one minimizer; (N1) therefore has exactly one solution for any
\(M=BGB^{\mathsf T}\), including a singular \(M\) from two channels with nearly identical
\(B\) rows. The Newton Jacobian of (N1) is \(J=I+MD\) with \(D=-\operatorname{diag}(dF/ds)\ge0\).
\(MD\) is similar to \(D^{1/2}MD^{1/2}\) when \(D>0\) (and has the same nonzero spectrum
otherwise), so its eigenvalues are real and nonnegative, every eigenvalue of \(J\)
is at least 1, and \(J\) is nonsingular. Uniqueness is what lets the coupled test
compare the channel-space solver with an independently coded minimizer of \(\Phi\)
in modal variables (different unknowns, different factorization).

### Iteration, bounds and failure reporting

The solver starts from the caller's guess (the engine uses \(s=b\), free motion),
forms \(J\), solves \(J\Delta=-R\) by partial-pivot elimination, and backtracks the
step by halving up to eight times while the max-norm residual does not decrease.
The last trial is accepted regardless, so every iteration has bounded work; the
total is capped by `max_iterations`. On return, `force` is \(F(s)\) at the returned
\(s\) and `residual_m` is the max-norm of (N1) there, so a caller that accepts a
non-converged iterate still applies an internally consistent force. The solver
returns `converged = false` when the cap is hit or \(m>\) capacity, and
`finite = false` when a pivot or the residual is not finite. A NaN residual is
propagated rather than dropped by `std::max` (a defect found and fixed while
writing these tests: before the fix a NaN input reported convergence). The
solver never clips forces or scales the step to hide a failure; the engine maps
these flags to `FaultCode::NewtonNotConverged` and `FaultCode::NonFiniteState`.

Work per iteration is \(O(m^3/3)\) for elimination plus \(O(m^2)\) evaluations; with the
default capacity of 32 channels this is bounded but not small, so the engine keeps
inactive contacts out of the system.

## 4. Energy balance of the coupled step, including moving obstacles

Sum (E1) over all modes, insert \(f=B^{\mathsf T}F\), and use
\(B(q^{n+1}-q^{n-1})=s+\Delta u\) with \(\Delta u=u^{n+1}-u^{n-1}\). With (C1):
\[
\sum H^{n+1/2}+\bar V^{n+1/2}-\Big(\sum H^{n-1/2}+\bar V^{n-1/2}\Big)
=-\sum D^n-\sum_c\frac{R_c}{4k}s_c^2+\sum_c\frac{F_c\,\Delta u_c}{2},
\qquad \bar V^{n+1/2}=\tfrac12\big(V(z^{n+1})+V(z^{n})\big).
\tag{E2}
\]
The last term is the work done by prescribed obstacle motion (nail stroke, finger
pad descent); it is signed. Bridge and stick channels have \(\Delta u=0\). With every
\(R\), \(\sigma\) and \(\Delta u\) zero the stored total is invariant to roundoff, for any
contact stiffness. That is an unconditional bound on \(H\), hence stability, but not
accuracy: a stiff spring's high coupled mode is frequency-warped by the trapezoidal
average, and the solver does not resolve it.

## 5. Activation, release and moving contact points (engine contract)

(E2) assumes a fixed channel set and fixed \(B\) over one step. The engine changes
both between steps and must book the difference explicitly in its ledger:

- **Adding** a channel whose potential is zero at levels \(n\) and \(n-1\) (a separated
  contact) changes nothing. A contact excluded from the system must be checked after
  the solve; if it penetrates at \(n+1\) the step is repeated with it included.
- **Removing** a channel (nail slip-off, stroke end, finger lift with residual
  compression) deletes its \(\bar V\) from the stored total; this is booked as release
  dissipation, which is nonnegative because \(V\ge0\).
- **Anchoring** a linear spring at contact onset (horizontal fret stick) can leave a
  nonzero \(\bar V\) at its first step; that signed amount is booked as switching
  energy, not hidden.
- **Moving** a contact point along the string changes \(B\) at fixed state; the change
  \(V(B^{n+1}q)-V(B^{n}q)\) is booked as actuator work of the moving finger.

These rules are part of the engine's specification. They are not exercised by the
`solver.*` tests and must be verified by the engine ledger tests.

## 6. Executed verification

Run with `ctest --test-dir <build> -R '^solver\.' --output-on-failure`. All ten pass
(Linux, GCC 13.3, Debug, 3.4 s total). Values below are from that run.

| CTest | Independent expectation | Result |
| --- | --- | --- |
| `solver.modal_exact_free_response` | Closed-form damped oscillator through the two initial samples; 8 modes from 27.5 Hz to \(0.49f_s\), \(\sigma\) up to \(0.9\omega\), at 44.1, 48 and 96 kHz, 2 s each | max \(\lvert q-q_{\rm exact}\rvert/q_0=4.0\times10^{-9}\) (undamped, \(0.49f_s\)); \(\le1.1\times10^{-9}\) below 5 kHz; bound \(10^{-7}\) |
| `solver.modal_energy_identity` | Test-derived \(\tilde\sigma,\tilde\omega^2\) from \((a,b)\); (M2) and (E1) evaluated by the test, random forcing, 20 000 steps, 6 modes | (M2) defect \(2.7\times10^{-16}\), (E1) defect \(3.8\times10^{-15}\) (relative); `energy_j`/`step_dissipation_j` match test formulas |
| `solver.modal_forced_convergence` | Analytic response to \(F_0\sin\Omega t\) from rest; \(f_s=4\)–128 kHz | error \(1.3\times10^{-2}\to1.3\times10^{-5}\), observed order 2.00 at every halving |
| `solver.sine_shape_accuracy` | Long-double `std::sin` for \(N=1024\), \(\theta\in\{0,10^{-9},\dots,\pi\}\) and 20 random | max error \(4.2\times10^{-14}\); bound \(10^{-15}(i+1)\) |
| `solver.discrete_gradient_identity` | (C1) with test-side potentials; 19 laws × 3008 \((z,s)\) pairs incl. onset at zero gap, tiny \(s\), crossings | worst relative defect \(8.4\times10^{-16}\); exact zero force when separated and undamped |
| `solver.contact_law_monotonicity` | Analytic secant derivative and central finite differences | \(dF/ds\le0\), \(F\) nonincreasing; 97 160 FD checks, worst scaled error \(5\times10^{-9}\) |
| `solver.newton_scalar_oracle` | Bisection on the same scalar equation with a test-side force, 600 random stiff cases | \(\le10\) iterations; worst \(\lvert s-s_{\rm bis}\rvert\) \(9.5\times10^{-13}\) relative |
| `solver.newton_coupled_oracle` | Test-side residual and an independently coded primal Newton/Cholesky minimizer of \(\Phi\); 120 systems of 2–7 mixed channels, one third with near-duplicate rows; iteration cap, capacity, NaN and empty cases | \(\le9\) iterations; worst \(\lvert s-s_{\rm primal}\rvert=4.8\times10^{-18}\) m; capped/over-capacity/NaN reported as not converged |
| `solver.point_spring_eigen_oracle` | Roots of \(Tk(\cot kx_0+\cot k(L-x_0))+K=0\) for an ideal string with a grounded spring (\(K=300\) N/m at \(0.37L\)); frequency of the simulated projection by least-squares recurrence fit | errors for modes 1–3 at 16/32/64/128 modes: \(4.6/2.2/1.1/0.46\times10^{-3}\), \(2.3/1.1/0.53/0.22\times10^{-3}\), \(2.3/1.1/0.52/0.21\times10^{-4}\); order 1.0–1.3; always above the continuum (Ritz bound); lossless energy drift \(\le1.4\times10^{-12}\); 96 vs 192 kHz change \(\le4.4\times10^{-5}\) |
| `solver.invalid_inputs` | Every documented precondition; transactional `prepare` | all rejected with `std::invalid_argument`; failed prepare keeps size, step, state and energy |

**Mutation check.** Eight single-line defects were injected into a scratch copy of
`numerics.cpp` (forcing gain \(g\), \(\tilde\omega\) using \(\omega\) instead of
\(\omega_d\), \(\tilde\sigma=\sigma\), always-tangent gradient, Jacobian sign, NaN
masking, damping factor, dissipation factor). Each was killed by at least one test:
\(g\) by the energy and forced tests; \(\tilde\omega\), \(\tilde\sigma\) and the dissipation
factor by the energy identity; the gradient branch and damping factor by all four
contact/Newton tests; the Jacobian sign by both Newton tests; NaN masking by the
coupled test. The injection script was run ad hoc and is not part of CTest.

## 7. Validity limits

- **Modal truncation.** A point contact in a truncated sine basis is smoothed; its
  static point compliance and coupled eigenfrequencies converge only as \(O(1/N)\)
  (measured above) and from above, as Rayleigh–Ritz predicts. A fret or bridge
  stiffness much larger than the truncated basis can resolve does not make the
  contact more accurate. Mode-count convergence of pitch must be shown for each
  engine configuration, not assumed from this fixture.
- **Point contacts.** Nail, pad and fret are points; a finite nail or flesh patch,
  its contact-area growth and the string's wrapping over a fret crown are absent.
- **Frictionless contact.** Normal forces only. The horizontal fret stick spring
  is a surrogate for static friction with explicit switching energy; there is no
  Coulomb slip, stick–slip or nail-surface friction.
- **Linear string.** No tension modulation, pitch glide, longitudinal motion or
  geometric coupling of the polarizations; pressing a string does not raise its
  tension.
- **Temporal accuracy of coupling.** Free modes are exact; the forced/coupled
  response is second order (measured order 2.00). Very stiff channels are stable
  but their own high-frequency motion is warped and is not a validated model of
  any physical termination.
- **Three-level contact force.** The force at \(n\) depends on \(z^{n-1}\) and
  \(z^{n+1}\); impacts are smeared over two steps, and a contact lasting about one
  step is not resolved. Contact duration convergence with \(f_s\) belongs to the
  engine tests.
- **Bounded iteration.** A capped iterate is reported, not certified. The default
  cap and tolerance are engine parameters, not derived error budgets.
- **Precision.** Double precision throughout; the free-response phase error grows
  roughly linearly with run length (about \(4\times10^{-9}\) after 2 s near Nyquist).

## Failure modes and responses

| Symptom | Likely cause | Response |
| --- | --- | --- |
| `std::invalid_argument` from `prepare` | Overdamped, super-Nyquist or non-finite mode; bad step | Fix mode selection before preparing; never clamp |
| `converged = false`, finite | Cap too low, extremely stiff \(\alpha>1\) contact, poor guess | Count and report the fault; inspect `residual_m`; do not raise the cap silently |
| `finite = false` | Non-finite input or state | Containment reset in the engine; log as numerical failure |
| Energy ledger residual grows | Unbooked activation, removal or moving-\(B\) term (section 5) | Fix the engine bookkeeping; the blocks here close (E1)/(C1) to roundoff |

## Next actions

1. Engine ledger tests that close (E2) with the section-5 bookkeeping for nail
   release, finger slide and fret stick (`ENG-P05-T02`, `ENG-P09-T01`).
2. Mode-count and sample-rate convergence of fretted pitch and nail release state
   for the actual prototype configuration.
3. An independent finite-difference contact reference for one fretted, plucked
   string (`realtime.reference_parity`, REQ-RTS-002).
4. Replace the synthetic contact constants with measured or identified ranges
   ([finite nail contact](../contacts/01-finite-nail-contact.md)).
