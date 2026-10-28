# Solver selection and discrete string–contact–bridge work contract

**Status: a decision protocol, not a selected production algorithm.** The
[actual explicit/implicit C++ comparison experiments](07-experimental-solver-evidence-and-choice.md)
now exercise synthetic fixed-end strings, energy ledgers and CFL rejection;
they do not establish which solver fits the real setar. Physical
fidelity depends on the selected setar, measured contact/bridge impedance,
required gesture bandwidth, source quality and available CPU. Choosing a single
solver in prose before those inputs and independent comparisons would create a
false engineering certainty. This document states exactly how the reference,
reduction and realtime schemes must be evaluated and how an engineer makes an
auditable choice. Read [time integration](../numerics/07-time-integration.md),
[nonlinear contacts](../numerics/09-nonlinear-algorithms.md),
[energy ports](../mathematics/03-energy-and-passive-ports.md) and
[physical ownership](02-physical-ports-and-state-ownership.md) first.

## Continuum model and the hypotheses it exposes

One transverse component of a uniform small-displacement string, ignoring
specified losses and external contact for the moment, has the illustrative form

\[
\mu\,y_{tt}=T\,y_{xx}-EI\,y_{xxxx}+f(x,t).
\]

Here \(\mu\) is kg/m, \(T\) newtons, \(EI\) N·m², \(y\) metres, and
\(f\) newtons/metre. Do **not** use this uniform equation unmodified after
adding spatially variable tension, joints, anisotropic transverse stiffness,
nonlinear elongation or moving contact. With position-dependent coefficients,
the corresponding conservative operators involve
\(\partial_x(T(x)y_x)\) and \(-\partial_{xx}(EI(x)y_{xx})\).

For the stated uniform, lossless and fixed-support hypothesis the illustrative
mechanical energy is

\[
E_s=\tfrac12\int_0^L\left(\mu y_t^2+T y_x^2+EI y_{xx}^2\right)\,dx.
\]

The model's extension adds (where justified): a second transverse component,
longitudinal strain or geometric nonlinearity, damping, finite nail and flesh
contact, fret/finger constraints, moving tension and supported endpoints,
shared bridge/body and cavity energy. A real pluck can excite components that
are absent from the one-polarization synthetic bootstrap; no amount of fitting
one scalar decay makes those omitted mechanisms proven.

Build an **energy term table** before coding: stored kinetic/elastic energy,
external work by performer/tuning, friction/damping loss, body/acoustic transfer,
and numerical residual. Put sign, units and accounting boundary on each row.
If radiation reaction is part of the coupled model, include acoustic work and
its loss. If a downstream microphone filter is observation-only, do not debit
its output gain from the mechanical energy ledger.

## Discrete work, time levels and adjoints

For one time interval of length \(h\), define a common discrete work

\[
W_{\mathrm{port}}=h\,F_{n+1/2}^{\mathsf T}v_{n+1/2},
\]

provided \(F\) and \(v\) are conjugate in the same coordinate basis. If the
two sides use opposite outward-positive force orientations, an ideal massless
connection requires \(v_A=v_B\) after transformation and \(F_A+F_B=0\) in a
common frame. Then the two numerical work contributions cancel. If a string
uses shape interpolation \(v_p=Bv_s\), its force injection must use the
weighted adjoint of \(B\). Verify

\[
\langle v_s,\,B^\star F_p\rangle_s
   =\langle Bv_s,\,F_p\rangle_p
\]

on random vectors including boundary and rotated-frame cases, not just one
normalized sinusoid. The exact adjoint depends on mass/quadrature inner
products; it is not automatically the unweighted array transpose.

For an unforced, passive, fixed-coefficient coupled test, check a discrete
ledger such as

\[
E^{n+1}-E^n=W_{\rm performer}+W_{\rm tuning}
 -D_{\rm material}-D_{\rm contact}-D_{\rm radiation}+R_{\rm numeric}.
\]

The sign of each term follows one documented convention. Report
\(R_{\rm numeric}\) numerically, normalized against an energy/work scale and
floating-point uncertainty; never subtract it from the displayed signal.
For a scheme that preserves a **modified** discrete energy rather than the
continuum expression, derive and test that modified energy by name. Claiming
continuum energy conservation from a plot of another quantity is invalid.

A one-sample lag between string force and bridge velocity invalidates the
above joint midpoint cancellation unless it is a separately derived delayed
model with its own stability/error proof. Do not bury such a lag in a plugin
buffer boundary or bridge filter. Port coupling must be verified at arbitrary
host buffer partitions.

## Nonlinear nail/fret contact: solver contract

Start with a scalar compression \(\delta\) and a one-sided conservative
potential; e.g. a *synthetic test law* for \(\delta>0\):

\[
\Phi(\delta)=\frac{k}{p+1}[\max(0,\delta)]^{p+1},\quad
F_n=\frac{d\Phi}{d\delta}.
\]

\(k\)'s dimensions depend on \(p\); never label it universally N/m. This
law is an initial verification fixture, not a measured setar nail law. For an
energy-consistent update, investigate the discrete gradient

\[
\bar F=\frac{\Phi(\delta_{n+1})-\Phi(\delta_n)}
{\delta_{n+1}-\delta_n},
\]

with the derivative limit when the denominator vanishes. Orient this force
to oppose penetration. Treat tangential friction and internal compliance
memory separately: a dissipative contact force must remove, rather than
unaccountably create, relative mechanical work. A moving nail/finger is an
**external work source** and may legitimately add energy; measure or prescribe
its trajectory and include that work in the ledger.

Define exactly what is solved at each step: string displacements and velocities,
bridge/body states, contact compressions, tangential states, active-set choices,
and optional cavity variables. The solver must have a finite residual norm,
guarded line search or bracketing where applicable, scale-aware stopping rules,
and maximum work capacity if it is intended for realtime. A reference solver
may adapt its step or report nonconvergence, but must preserve the failed
fixture and not output a false pass.

Test contact onset, an exact no-contact limit, detachment, zero relative
velocity, repeated chatter/recontact, simultaneous two-fret constraints,
high/low stiffness, fast moving frets, and near-zero overlap. Validate
penetration and force *as well as* energy. A numerical method can be stable
yet violate physically required unilateral-contact conditions.

## Candidate algorithms and their actual decision questions

| Candidate | Primary value | Required verification | Principal risk to investigate |
| --- | --- | --- | --- |
| Explicit FDTD with stiffness | Local string state, transparent boundaries and contact positions | Exact stencil/CFL bound, grid dispersion, endpoint closure, adjoint injection | High-frequency stability imposes a smaller step than audio output rate; contact stiffness may dominate |
| Implicit/discrete-gradient grid | Work-consistent nonlinear constraints and stiff coupling | Nonlinear solve residual, discrete work identity, convergence and worst-case iterations | Matrix factorization cost, bounded realtime convergence and contact branch changes |
| Analytical/modal string | Efficient long decay, inspectable modes, finite-dimensional passive state | Mode normalization/residues, truncated contact compliance, dispersive transient and re-pluck tests | Localized contact can require many modes; sharp initial shape has high-frequency content |
| Energy-compatible reduced FEM | Geometrical fidelity at nonuniform body/bridge constraints | Mesh/refinement, boundary work, reduced-port passivity and held-out FRFs | Expensive full solver, modal reduction loses local/contact detail |
| Digital waveguide/scattering hybrid | Potentially low cost for approximately traveling-wave portions | Dispersion/loss filters, scattering passivity, physical port phase and stiffness correction | Simple delay-line boundary and localized high-order bending/contact may be inaccurate |

All are *candidates*. Do not select one because a commercial instrument is
rumored to use it. Choose from independently reproducible performance on this
project's physical and gestural parameter envelope. A hybrid can use an offline
implicit string–body reference and an optimized modal/waveguide production
model, but their observables and error budget must be directly comparable.

### Selection experiment, in order

1. Freeze synthetic analytic fixtures: damped oscillator, ideal fixed string,
   stiff string, two masses with spring, scalar contact, one string plus one
   bridge mode, two strings plus a modal bridge. Define reference tolerances
   *before* testing optimized candidates.
2. Derive matrix/stencil, boundary conditions and energy for each candidate.
   Reject choices with missing force/velocity adjoints, wrong unit scaling or
   inconsistent half-step definitions before benchmarking sound.
3. Sweep spatial step, time step, modal count, numerical damping and solver
   tolerance independently. Fit observed convergence order only where a
   genuine asymptotic regime is visible, rather than asserting a claimed
   order from two samples.
4. Add synthetic nail/fret contact and swept gesture trajectories. Measure
   onset timing, force/penetration, work defects, frequency drift, spectral
   aliasing and release continuity.
5. Couple each candidate through a joint bridge port. Stress both high and low
   driving-point impedances and near-resonant cross-string states; record
   phase, transfer and energy over long silent decays.
6. Compare with a *different* converged reference method or analytic limit,
   then with permitted measured FRFs/contact data. Keep calibration separate
   from numerical truth.
7. Benchmark CPU cost only after a candidate passes applicable physical tests.
   Include preparation time, memory, worst-case nonlinear iteration and
   sample-rate changes separately from steady-state callback cost.
8. Document the selection in an ADR: passed scope, excluded gestures/ranges,
   competing variants, numerical error/CPU Pareto, raw artifacts, reviewers
   and conditions that would reopen the decision.

## Multirate and antialiasing: exact integration requirements

Let the host rate be \(f_s\). A solver island at integer factor \(r\) uses
internal sample step \(1/(r f_s)\), and its event times must be represented
on that grid or interpolated with a documented rule. The audio output is then
low-pass filtered and downsampled, with known passband, stopband, phase/group
delay, reset state and declared latency. Oversampling cannot rescue a bad
contact law, and simply computing \(r\) samples then retaining one is not
valid downsampling.

Evaluate three tests independently:

- A high-accuracy all-at-high-rate reference with fixed physical time and
  matched events, against the proposed multirate island.
- Aliasing energy above output Nyquist, under strong nail/fret impacts and
  parameter modulation, versus an independently designed resampling oracle.
- Mechanical port phase and work after bridging different numerical rates.
  A filtering delay in a feedback power port is a changed mechanical model,
  not merely an audio-rendering choice.

Do not place bridge feedback on a lower rate without deriving a passive
interface or measuring its defect. A band-limited **observation** may be
downsampled more freely than a physical interconnection; record which is which.

## Scientific and production failure semantics

| Situation | Offline reference | Realtime candidate |
| --- | --- | --- |
| Parameter violates stability/positivity | Reject with field, units, offending bound | Refuse new prepared asset, preserve accepted state |
| Contact root does not converge | Reject step or adapt with recorded refinement | Fixed-work safe containment with explicit solver-failure counter |
| Nonfinite physical state | Stop and save permitted reproducer | Finite bounded output or safe silence, fault exposed |
| Port work defect exceeds preregistered tolerance | Fail comparison and retain raw trace | Mark approximation unqualified; never conceal with clipping |
| Event overflow | Test harness should detect malformed capacity scenario | Priority/release policy, observable loss counts, no indefinite voice |
| Unsupported sample-rate/geometry change | Reprepare, reject if inadmissible | Defer/reject transactionally or apply validated state map |

Production acceptance requires **both** numerical and scheduling envelopes.
One dense offline solve passing an acoustic metric does not qualify a bounded
callback; one quiet realtime benchmark does not qualify nail impacts.

## Test-driven handoff requirements

`ENG-P03-T01` and `ENG-P03-T02` must deliver a spatial and temporal error table;
`ENG-P03-T03` delivers the first power-port identity; `ENG-P05-T02` delivers nonlinear
contact residuals; `ENG-P08-T03` delivers a passive reduced-body interface;
`ENG-P09-T01` delivers the *joint* energy experiment; `ENG-P13-T01` delivers the
reduction error-versus-cost comparison. The planned test names and requisite
review artifact are in [requirements.json](../../roadmap/requirements.json).

A phase can be rejected scientifically and still teach the next method choice.
Preserve its fixture, equations and negative evidence; do not convert it to a
green milestone by changing the microphone gain or relaxing every tolerance.
