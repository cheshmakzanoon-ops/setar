# Experimental solver evidence, numerical decision gates and the next comparison experiments

**Status: two real, executable C++20 spatial prototypes; not a selected setar production solver.**

This is an implementation companion to [the general solver decision protocol](05-discrete-solver-selection.md),
[the proposed module contracts](04-cpp20-module-contracts.md),
[the complete acceptance matrix](../engineering/verification-matrix.md) and
[the executable partial coverage map](../../roadmap/executable-test-coverage.json).
The implementation currently computes synthetic stiff-string motion. It has no measured
setar geometry or wooden body, bridge mobility, nail contact, movable frets, longitudinal
degree of freedom, or room-calibrated radiation. No final FDTD/FEM/modal/multirate
composition has been chosen.

## Where the experiment lives, and how to reproduce it

| Exact file | Contract or evidence |
| --- | --- |
| [`include/setar/numerical_prototype.hpp`](../../include/setar/numerical_prototype.hpp) | Explicit offline types, units, time levels, ownership and limitations |
| [`src/numerical_prototype.cpp`](../../src/numerical_prototype.cpp) | Dense-factorized implicit midpoint, local-stencil explicit leapfrog and two-mass oracle |
| [`tests/numerical_prototype_tests.cpp`](../../tests/numerical_prototype_tests.cpp) | Fifteen independently named native C++ cases (four for passive loss, see [chapter 14](../numerics/14-damped-midpoint-loss-verification.md)); deliberately small analytic benchmark |
| [`apps/compare_solvers.cpp`](../../apps/compare_solvers.cpp) | Deterministic 60-row frequency/CFL sweep, CSV on standard output |
| [`tools/generate_solver_report.py`](../../tools/generate_solver_report.py) | Portable CSV validation, actual SVG graphs, HTML and source-hashed evidence |
| [`tools/test_solver_report.py`](../../tools/test_solver_report.py) | Report consistency, convergence monotonicity, invalid-evidence rejection and CLI integration |
| [CI workflow](../../.github/workflows/engineering.yml) | Build, run and artifact collection on Linux, Windows and macOS; Linux sanitizers |

After installing a C++20 toolchain, CMake 3.25+ and Python 3.10+:

```sh
cmake -S . -B build -DSETAR_BUILD_NUMERICAL_EXPERIMENTS=ON -DSETAR_ENABLE_JUCE=OFF
cmake --build build --config Debug
ctest --test-dir build -C Debug -R '^(numerics|coupling|integration.solver_evidence)' --output-on-failure
```

The CI matrix additionally runs the executable sweep and produces
`artifacts/solver/report/index.html`, `spatial-convergence.svg`,
`temporal-dispersion.svg`, `explicit-dispersion.svg`, `solver-comparison.csv`
and `solver-evidence.json`. These graphs are **actual numerical outputs from the
current code**, not photographs of an operating plugin and not comparisons with
measured instrument audio. Preserve the CSV hash and Git revision together.

### Mathematical common ground

One simply supported string, one transverse polarization, uniform coefficients:

\[
\mu y_{tt}=T y_{xx}-EI y_{xxxx}+F(t)\delta(x-x_p),
\qquad y(0,t)=y(L,t)=y_{xx}(0,t)=y_{xx}(L,t)=0.
\]

With \(N\) *interior* displacement nodes and \(h=L/(N+1)\), define
\(B=-D_2\), the symmetric positive tridiagonal Dirichlet second derivative,
and \(K=TB+EI B^2\). The semidiscrete state solves

\[
\mu\ddot q=-Kq+\frac{F}{h}w,
\qquad E_h=\frac h2\left(\mu v^Tv+q^TKq\right).
\]

The point-port shape \(w\) is piecewise-linear interpolation at \(x_p\).
Its sampled velocity is \(v_p=w^Tv\). Because the nodal work quadrature is
\(h\sum_i v_i(Fw_i/h)\), **the same weights** must be used for output and
force input: \(P=Fv_p\). The endpoint interpolation weights are zero. These
details are testable and not interchangeable with a body-force density of
different units.

The analytic continuum and discrete-spatial eigenvalues for sine mode \(j\) are

\[
\omega_j^2=\frac{T(j\pi/L)^2+EI(j\pi/L)^4}{\mu},\qquad
\lambda_{h,j}=\frac{4}{h^2}\sin^2\!\left(\frac{j\pi}{2(N+1)}\right),\qquad
\omega_{h,j}^2=\frac{T\lambda_{h,j}+EI\lambda_{h,j}^2}{\mu}.
\]

These formulas are independent of the code's time stepping and permit
**separate spatial and temporal error budgets**. Do not report a single
waveform error that conceals cancellation between two biased approximations.

### Candidate A: spatial FD + implicit midpoint

The code uses the midpoint relation for displacement/velocity, and the
positive-definite solve

\[
\left(I+\frac{\Delta t^2}{4\mu}K\right)q_{n+1}
=q_n+\Delta t v_n-\frac{\Delta t^2}{4\mu}Kq_n
+\frac{\Delta t^2F_{n+1/2}}{2\mu h}w.
\]

One dense Cholesky factorization occurs at `prepare`. The lossless method
preserves the *semidiscrete* energy \(E_h\) for fixed SPD \(K\), and with an
applied midpoint force the exact identity is

\[
E_{n+1}-E_n=\Delta tF_{n+1/2}
\frac{v_{p,n}+v_{p,n+1}}2.
\]

This specific linear fixed-coefficient update has no explicit stability CFL
restriction, but is **dispersive**: its effective frequency is
\(\hat\omega=(2/\Delta t)\arctan(\omega_h\Delta t/2)\). Passive loss is added to this same update
in [chapter 14](../numerics/14-damped-midpoint-loss-verification.md), with an exact dissipation
ledger; at zero loss the statements here are unchanged.
Stability must never be equated with accurate high harmonics, correct contact,
or physically valid moving fret boundaries. The current dense \(N\times N\)
stiffness and solve use \(O(N^2)\) per step and \(O(N^3)\) preparation; treating
this particular reference implementation as a bounded audio callback would
be a performance mistake. Factorizations, state initialization and failures
can allocate or throw. The production design must make a separate transition
to sparse/banded, reduced, port-condensed or other tested realizations.

### Candidate B: spatial FD + explicit leapfrog

The independent C++ solver uses the same spatial operator but a three-level
stencil:

\[
q_{n+1}=2q_n-q_{n-1}-\frac{\Delta t^2}{\mu}Kq_n
+\frac{\Delta t^2F_n}{\mu h}w.
\]

It computes \(Bq\) and \(B^2q\) by local fixed-end stencils with \(O(N)\)
work, without a dense matrix or factorization. Its maximum spatial
eigenfrequency determines the strict linear stability condition
\(\Delta t\,\omega_{h,N}<2\); unsupported configurations are rejected at
`prepare`, even if a lower individual harmonic happens to be in band.
The observable temporal frequency is
\(\hat\omega=(2/\Delta t)\arcsin(\omega_h\Delta t/2)\), valid only inside
the CFL domain. Its *staggered modified energy*, not instantaneous continuum
energy, is

\[
\widetilde E_{n-1/2}
=\frac{\mu h}{2\Delta t^2}\|q_n-q_{n-1}\|^2
+\frac h2q_{n-1}^{T}Kq_n.
\]

For integer-time applied force, the tested work identity is
\(\widetilde E_{n+1/2}-\widetilde E_{n-1/2}
=\Delta tF_n w^T(q_{n+1}-q_{n-1})/(2\Delta t)\).
An explicit update that violates CFL can have growing motion and an
indefinite modified-energy interpretation; such a test must **fail**, not
return a frequency, zero error, or clipped audio. The prototype does not
contain loss, changing tension, contacts or variable grids, any of which can
introduce a more restrictive stability condition.

### Reproduced synthetic numerical facts

The following numbers come from `setar_compare_solvers` with its code-default
uniform parameters, **not measurements**. In mode 3 the independent continuum
frequency is about 508.219474 Hz. At 48 kHz:

| Interior nodes | Spatial semidiscrete Hz | Midpoint Hz | Leapfrog Hz | Spatial absolute error Hz |
| ---: | ---: | ---: | ---: | ---: |
| 8 | 485.311980 | 485.148864 | 485.393624 | 22.907494 |
| 32 | 506.493837 | 506.308427 | 506.586649 | 1.725637 |
| 64 | 507.774351 | 507.587532 | 507.867869 | 0.445122 |
| 128 | 508.106439 | 507.919253 | 508.200140 | 0.113035 |

For the same synthetic stiffness and fixed boundary the explicit full-grid
minimum sample rates (strict CFL) are approximately 11.19 kHz for 32 nodes,
22.20 kHz for 64, and 45.11 kHz for 128. At 8 kHz the 64-node explicit
configuration is correctly rejected while implicit midpoint remains linearly
stable. **Neither conclusion says which version sounds more like a setar.**
Higher order accuracy, nonlinear contact, sparse solver cost, physical
boundary conditions and calibrated string/body dynamics can reverse a
practical decision.

## Portfolio of candidate technologies: what is tested vs undecided

| Technology | Evidence now | Mandatory next investigation | Decision status |
| --- | --- | --- | --- |
| Explicit spatial FDTD/leapfrog | Executable one-dimensional fixed-end uniform string, CFL rejection, modified-energy and phase checks | Movable contacts, nonuniform \(T\)/\(EI\), two polarizations, string termination, multirate oversampling, worst-case CPU | Not selected |
| Implicit finite-difference midpoint | Executable SPD one-dimensional string, dense Cholesky, force/energy checks | Sparse/banded factorization, nonlinear complementarity, high-frequency phase error, real-time bounded alternative | Not selected |
| Exact finite modal evolution | Existing separate `ModalString` reference with analytic poles and linear pluck projection | Body-reduced modes, parameter sensitivity, local contact convergence, moving fret updates, modal truncation energy | Synthetic oracle only |
| Structural FEM | Detailed work orders and patch/mesh requirements; **no actual body FEM** | Measured wooden soundboard/bowl/joints, coordinate orientation, mass/stiffness assembly, boundary/support studies | No selection evidence |
| Body modal/rational reduction | Proposed ports/FRF requirements; **no measured mobility or calibrated body model** | Passivity-preserving reduction of measured or converged FEM FRFs and held-out directions | No selection evidence |
| Multirate coupling | Analytical work/time-level requirements; **no executable string/body multirate coupling** | Interpolation/decimation energy work, contact bandwidth, alias/error vs cost, block-partition parity | No selection evidence |
| Nonlinear nail/fret mechanics | Measurement protocols and scalar law proposals; **no measured or executable setar contact** | Force-displacement trajectory, release, friction, discrete-gradient solver, failure caps and practitioner review | No selection evidence |

The comparator must use a common physical specification and explicit
observables: initial and boundary data, mode/harmonic phase, contact force and
release time, port energy, body FRF, radiation pressure, state memory,
worst-case callback time, peak memory and audible aliases. A comparison of
method names or generated example tones is insufficient.

## Next experiments, in decision order (no calendar estimates)

1. **Freeze a wider synthetic fixture package.** Add independent manufactured
   driven-string solutions, fixed/free and impedance boundaries, orthogonal
   second polarization and a known nonlinear spring contact. Record equations,
   SI units, exact boundary data, input hashes, numerical tolerances and
   conditioning. Keep at least one comparator implemented by a different
   algorithm or independently derived mathematical oracle.
2. **Break the explicit stability boundary deliberately.** Derive the complete
   \(K\) spectrum with measured candidate stiffness ranges; test just below
   and above CFL while inspecting amplification roots, energy and numerical
   amplitude. Add an incompatible moving-contact stiffness and verify
   rejection rather than an accidental apparently stable short run.
3. **Separate spatial, temporal and reduction errors.** Sweep grid density,
   time rate, modal truncation and solver tolerance independently. Plot errors
   on a single target observable, with \(\omega\), phase, decay and port
   work disaggregated. Add independent high-precision or converged references.
4. **Introduce the boundary first.** Replace one fixed end with a known
   positive-real lumped mobility, then a two-port model and finally a
   measured complex bridge mobility. Use one common time level and equal/
   opposite force convention. Reproduce the coupled two-mass oracle in the
   split string/body implementation and include a deliberately one-sample-
   delayed negative test.
5. **Create a body structural FEM benchmark.** Before a setar mesh, use
   analytic rods/plates and patch tests, independent eigenfrequencies,
   boundary-condition perturbations and mesh convergence. Then calibrate the
   actual instrument geometry, wood orthotropy, joints and bridge supports;
   reduce the converged FEM in a passivity-preserving way. Never fit missing
   body stiffness by an arbitrary microphone EQ curve.
6. **Prototype nail/fret active sets.** Add documented one-sided potentials,
   force/velocity law, energy-consistent release and friction dissipation to
   a small oracle. Test simultaneous contacts, recontact, exact zeros, extreme
   compliance, nonconvergence and external performer work before any host UI.
7. **Explore hybrid/modal/multirate schemes only against those oracles.**
   Explicitly publish time-level mappings, decimator group delay and contact
   work across rates. Retain a high-rate full reference case as an independent
   control. Compare output signal **and hidden physical states**.
8. **Measure runtime after numerical parity.** Report exact CPU, compiler,
   vectorization, floating-point mode, active contact count, density, buffer
   partitions and performance distribution. Allocate/lock instrumentation,
   maximum-work controls, thread and sanitizer reports must accompany any
   callback-readiness claim. The current dense reference cannot earn this
   gate through mathematical stability alone.
9. **Select through an ADR only after instrument evidence.** Record candidate
   variants, all failed cases, fidelity metrics, software/HW cost,
   practitioner relevance, holdouts and remaining disagreement. It is valid to
   select different techniques for string propagation, contact, structural
   reduction, radiation and offline reference: a hybrid is likely to be
   justified by measured performance rather than doctrinal preference.

### Primary material for the next engineers

- Bilbao and colleagues, *The simulation of piano string vibration: from
  physical models to finite difference schemes and digital waveguides*
  ([archival research abstract](https://pubmed.ncbi.nlm.nih.gov/12942987/)).
  This is a methodological comparison; **a piano string is not a setar**.
- *Implicit and explicit schemes for energy-stable simulation of string
  vibrations with collisions: refinement, analysis, and comparison*
  ([DOI 10.1016/j.jsv.2023.117968](https://doi.org/10.1016/j.jsv.2023.117968)).
  Examine the stated contact energy, algorithmic assumptions and its
  comparison of modal versus finite-difference methods; port its proofs
  rather than copying one result out of boundary context.
- *Simulation of the geometrically exact nonlinear string via energy
  quadratisation* ([University of Edinburgh research record](https://www.research.ed.ac.uk/en/publications/simulation-of-the-geometrically-exact-nonlinear-string-via-energy/)).
  Investigate longitudinal/transverse energy and sparse/full solve separation.
- [Numerical time integration](../numerics/07-time-integration.md),
  [nonlinear algorithms](../numerics/09-nonlinear-algorithms.md),
  [passive ports](../mathematics/03-energy-and-passive-ports.md) and
  [code review rules](../engineering/implementation-specification.md) are
  project-specific implementation companions.

**Release gate:** the current code is research infrastructure. Full numerical
selection awaits enough empirical information to define and test an honest
accuracy/latency/error envelope. Do not change requirement statuses to
`numerically_verified` merely because the narrowly specified synthetic
prototype's CTests pass.
