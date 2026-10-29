# Passive loss in the implicit-midpoint string: derivation, oracles, and verified limits

**Status: `verified` for the declared synthetic scope; not `validated`.** This chapter
documents the first time-domain solver in the repository that dissipates energy, and
exactly what its tests establish. It closes the executable part of requirement
[REQ-STR-003](../../roadmap/requirements.json) that concerns *loss law passivity*
(`numerics.loss_law_passivity`, work order `ENG-P04-T01`, with the energy-ledger
clause of `REQ-STR-002`). It does **not** show that any loss law describes a real
setar string, and every loss coefficient used below is an arbitrary synthetic value.

## Prerequisites, sources, inputs and outputs

Read [dispersion and dissipation](08-dispersion-and-dissipation.md) (metrics D6–D8),
[linear loss and dispersion](../strings/03-loss-and-dispersion.md) (the PDE and the sign
argument), and [time integration](07-time-integration.md). The implicit-midpoint rule is the
trapezoidal rule applied to a first-order system; its conservation of quadratic
invariants is treated in Hairer, Lubich and Wanner, *Geometric Numerical Integration*,
Springer, 2006, DOI [10.1007/3-540-30666-8](https://doi.org/10.1007/3-540-30666-8), chapter IV
(DOI [10.1007/3-540-30666-8_4](https://doi.org/10.1007/3-540-30666-8_4)). The identities used here are derived
below rather than quoted. Finite-difference string modelling with loss follows the lineage of
Chaigne and Askenfelt, *J. Acoust. Soc. Am.* 95(2), 1112–1118 (1994), and Bilbao,
*Numerical Sound Synthesis*, Wiley, 2009 (ISBN 978-0-470-51046-9); neither source's
parameters are used, and the present scheme is symmetric in time, unlike the
backward-gradient-loss explicit scheme of [chapter 2](01-stiff-string-fdtd.md). A DOI for
the Chaigne–Askenfelt article and for the Bilbao book was not independently resolved
while writing this chapter; check those references against the publisher before quoting.

| Item | Location |
| --- | --- |
| Inputs | `StringGrid` (SI units) with `StringLoss{decay_constant_per_s, decay_wavenumber_squared_m2_per_s, decay_frequency_squared_s}` in [`numerical_prototype.hpp`](../../include/setar/numerical_prototype.hpp) |
| Solver | `MidpointStiffString::advance`, returning `WorkStep{…, dissipated_energy_j, work_residual_j}` |
| Oracles | `continuum_decay_rate_per_s`, `spatial_grid_decay_rate_per_s`, `midpoint_damped_roots` |
| Measurement | `estimate_decay` (gain-invariant damped-pair fit) |
| Analytic reference | `setar::reference::ModalString`, now with `decay_wavenumber_squared_m2_per_s` |
| Outputs | CTests below, two CSV tables from [`setar_loss_study`](../../apps/loss_study.cpp), and graphs from [`generate_loss_report.py`](../../tools/generate_loss_report.py) |

## The loss law and its three coefficients

The uniform baseline is the equation of [strings/03](../strings/03-loss-and-dispersion.md),
with one additional passive term that is common in stiffness-proportional (Rayleigh)
damping:
\[
\mu y_{tt}=-L y-2\mu(\sigma_0+\sigma_1B)y_t-2\sigma_2 L y_t+f,\qquad
L=-T\partial_{xx}+EI\,\partial_{xxxx},\quad B=-\partial_{xx}.
\]
On the uniform grid with simply supported ends, \(B\) and \(L\) share the discrete sine vectors
as eigenvectors, so mode \(n\) has amplitude decay
\(\gamma_n=\sigma_0+\sigma_1\lambda_n+\sigma_2\Omega_n^2\), where \(\lambda_n\) is the grid
eigenvalue of \(B\) and \(\Omega_n^2\) that of \(L/\mu\). In the continuum, \(\lambda_n\) and
\(\Omega_n^2\) become \(k_n^2\) and \(\omega_n^2\), \(k_n=n\pi/L\).
All three coefficients are required to be finite and nonnegative. The damping operator
\(D=2(\sigma_0 I+\sigma_1 B+\sigma_2 L/\mu)\) is then a nonnegative combination of symmetric
positive definite matrices, so it is positive semidefinite *by construction*; no sign is
chosen by hand and nothing is clipped. Invalid values throw `std::invalid_argument`.

| Law | `docs/strings/03` | `reference::StringParameters` | `experimental::StringLoss` |
| --- | --- | --- | --- |
| \(\sigma_0\), 1/s | \(\sigma_0\) | `decay_constant_per_s` | `decay_constant_per_s` |
| \(\sigma_1\), m²/s (\(k^2\) law) | \(\sigma_1\) | `decay_wavenumber_squared_m2_per_s` (new) | `decay_wavenumber_squared_m2_per_s` |
| \(\sigma_2\), s (\(\omega^2\) law) | — | `decay_frequency_squared_s` | `decay_frequency_squared_s` |

Before this change the modal reference had only \(\sigma_0\) and \(\sigma_2\) while the
handbook PDE had \(\sigma_0\) and \(\sigma_1\); the two were not the same model. They now are.

## The discrete scheme and its exact energy identity

With \(A=L/\mu\) (discrete), the first-order system \(q'=v,\ v'=-Aq-Dv+g\), and the trapezoidal
rule, one step solves
\[
\Bigl(I+\tfrac{\Delta t^2}{4}A+\tfrac{\Delta t}{2}D\Bigr)q_{n+1}
=\Bigl(I-\tfrac{\Delta t^2}{4}A+\tfrac{\Delta t}{2}D\Bigr)q_n+\Delta t\,v_n+\tfrac{\Delta t^2}{2}g,
\qquad v_{n+1}=\tfrac{2}{\Delta t}(q_{n+1}-q_n)-v_n.
\]
The left matrix is symmetric positive definite, so the dense Cholesky factor built at
`prepare()` always exists and the update is unconditionally stable for the declared
domain. Define \(E=\tfrac{\mu h}{2}(v^{\mathsf T}v+q^{\mathsf T}Aq)\), \(\bar v=(v_n+v_{n+1})/2\),
\(\bar q\) likewise. Using the symmetry of \(A\),
\[
E_{n+1}-E_n=\mu h\bigl[\bar v^{\mathsf T}(v_{n+1}-v_n)+\bar q^{\mathsf T}A(q_{n+1}-q_n)\bigr]
=\underbrace{\Delta t\,F\,\bar v_p}_{\text{work}}-\underbrace{\Delta t\,\mu h\,\bar v^{\mathsf T}D\bar v}_{\text{dissipated}\ \ge 0},
\]
exactly, not to first order. The code computes the dissipated term from \(D\) and the midpoint
velocity only, and the stored-energy change from \(E\) before and after the step, so
`work_residual_j = ΔE − W + dissipated` is a genuine consistency check rather than an
identity enforced by construction. With no loss the previous behaviour is unchanged: all 29
pre-existing CTests still pass, and a one-off comparison against the pre-change commit
`07efaf5` (GCC `-O1 -ffp-contract=off`, 1000 forced steps) gave an identical FNV-1a hash of every
reported work, energy change, residual, port velocity and the final state. That comparison is not
a CTest; repeat it by building the old sources if the zero-loss path is touched.

## What the scheme does to the decay rate

For one mode with eigenvalue pair \(s=-\gamma\pm i\omega_d\) the amplification roots are the
Cayley images \(r=(1+s\Delta t/2)/(1-s\Delta t/2)\). With \(a=\gamma\Delta t/2\),
\(b=\omega_d\Delta t/2\),
\[
|r|^2=1-\frac{4a}{(1+a)^2+b^2},\qquad \arg r=\arctan\frac{b}{1-a}+\arctan\frac{b}{1+a}.
\]
Hence \(d_{\text{num}}=-\ln|r|/\Delta t\approx\gamma/(1+b^2)\): the midpoint rule **under-damps**
every resolved mode by a relative \(b^2/(1+b^2)\), never over-damps it, and adds *exactly zero*
algorithmic damping when \(\gamma=0\). This is the "solver loss versus physical damping"
separation that chapters 7 and 8 require: a fitted decay measured from this solver differs
from the declared law by an amount that is known in closed form and shrinks as \(\Delta t^2\).
If \(\gamma\ge\Omega\) the roots are real; `midpoint_damped_roots` then reports
`oscillatory=false`, `frequency_hz=0` and the slowest root, and beyond critical damping a larger
\(\gamma\) gives a *smaller* slow-root decay (a property of overdamped oscillators, tested).

## Test inventory and the independent comparator for each

| CTest | Claim | Independent comparator | A failure would mean |
| --- | --- | --- | --- |
| `numerics.loss_law_passivity` | Each law dissipates ≥ 0 per step; stored energy never rises unforced; ledger closes every step, forced and unforced; zero loss gives exactly zero dissipation; the lossless limit is continuous | Pseudo-random rough state exciting every grid mode; a ledger that omits the loss term must show orders of magnitude more residual | A sign, factor or matrix error in \(D\), or the solver gaining energy |
| `numerics.loss_decay_estimator` | The damped-pair estimator recovers known roots, is gain-invariant, reports growth as negative decay, flags two-mode data through its residual, and rejects degenerate input | Closed-form damped cosines, overdamped sums of exponentials, extreme gains | Measurements in the other tests would be untrustworthy |
| `numerics.loss_decay_oracle` | Decay and frequency measured from simulated, projected modes equal the Cayley amplification oracle; overdamped classification; refinement order; a wrong law fixed at mode 1 is rejected by higher modes | Eigenvalue-level oracle sharing no code with the matrix solve; sine projections built in the test | The solver does not implement the stated amplification matrix |
| `numerics.loss_modal_reference_crosscheck` | Waveform at an on-grid pickup converges at second order to the analytic modal reference; a reference with the wrong loss law is rejected | `setar::reference::ModalString`, exact in time and space, built from different code; linked only into the test | The solver, the reference or the loss mapping disagree |
| `core.wavenumber_loss` | The \(\sigma_1\) term of the analytic reference, alone and mixed, against a long-double closed form | Extended-precision characteristic equation | The reference oracle itself is wrong |
| `numerics.invalid_parameters` (extended) | Negative, NaN and infinite coefficients are rejected transactionally; the explicit solver refuses loss instead of ignoring it | Rollback checks on a prepared string | Silent acceptance of non-passive input |
| `integration.loss_evidence` | The C++ study tables, strict provenance gates, plots, and thirteen rejection paths of the report tool, each required to fail for its intended reason | Recomputed derived columns and orders | Corrupt or inconsistent evidence could be published |

`tools/mutation_check_loss.py` injects 13 plausible defects into a scratch copy of the sources
(sign flip, dropped \(\sigma_1\), \(K\) instead of \(K/\mu\), ledger off by two, missing
\(\Delta t D/2\) in the system matrix, oracle missing the \(1/2\), wrong overdamped root,
estimator reporting energy decay, a one-step lag, the explicit solver ignoring loss, negative
loss admitted, and two reference defects). **All 13 are killed, most by several tests.** Run it
after any change to the loss path; a surviving mutant means a test must be strengthened.

## Results from the executed experiments

All numbers come from the commands in the last section on Linux: the full CTest suite under
GCC 13 (Debug, Release and ASan/UBSan), and the loss tests compiled and run with Clang 18 under
the project's warning flags. They are synthetic-fixture results, not instrument measurements.

**Passivity and ledger.** With a rough pseudo-random state (about 6×10⁻⁴ J), 2000 steps dissipate
22% (constant), 98% (\(k^2\)), 99% (\(\omega^2\)) and 99.8% (combined law) of the stored energy while the
summed ledger residual is 6.6×10⁻¹⁷, 9.0×10⁻¹⁸, 2.3×10⁻¹⁷ and 4.9×10⁻¹⁸ J. A driven, lossy
run has a worst per-step residual of 4.3×10⁻¹⁸ J at a peak stored energy of 5.9×10⁻⁴ J.

**Measured decay versus oracle.** Across the 96-row decay table (four laws, four
refinement levels, modes 1–8) the worst relative disagreement between decay measured from the
simulation and the oracle is 5.6×10⁻¹⁰, the worst single-damped-pair fit residual is
2.7×10⁻¹⁵, and the regressor condition number never exceeds 180.

**Convergence of the scheme's decay to the continuum law.** Halving \(h\) and \(\Delta t\)
together (15→127 interior nodes, 24→192 kHz), the finest-pair observed order is between
1.98 and 2.00 for all 24 (law, mode) series. The error is large on coarse grids for high modes:
for the combined law and mode 8, 2.0×10⁻¹ at 15 nodes falling to 3.6×10⁻³ at 127; for mode 1,
1.4×10⁻³ falling to 2.2×10⁻⁵.

**Waveform against the analytic modal reference (3-mode band, pickup at 25%).** The relative
L∞ error of the combined law falls 1.31×10⁻¹, 3.52×10⁻², 8.90×10⁻³, 2.23×10⁻³ with finest-pair order
1.996; the other four laws and a stiffer string (EI = 8×10⁻⁵ N m²) give finest-pair orders of
1.9956–1.9959. A reference with the **wrong** loss law does not converge: against a lossless
reference the error stays at 0.151 (finest-pair order 0.006) and against a constant-only reference at
0.066 (order 0.021), versus 2.2×10⁻³ for the matched reference at the same level. That plateau is the
signature of a model-form error, which no refinement removes.

**Resolution rule.** A 6-mode band is *not* resolved on the coarsest grids: the lossless
error orders are 0.48, 1.83, 2.01 and the combined-law orders 1.18, 1.99, 2.04. Convergence
claims in this chapter therefore apply to the 3-mode band, whose highest mode is 3/16 of the
mode count of the coarsest 16-interval grid; the 6-mode band (6/16) is outside the resolved domain
there. A larger band needs a finer starting grid.

**An identifiability limit, stated numerically.** For \(EI=1.5\times10^{-5}\) N m² the decay profiles
of a pure \(\sigma_1\) law and a pure \(\sigma_2\) law, each calibrated at mode 1, differ by at most
5.8×10⁻⁴ over modes 2–8, so measured partial decay cannot separate them; for a bending-dominated
stress fixture (EI = 0.01) they differ by 28%. A calibration that reports both coefficients for
a setar-scale string from decay data alone would be reporting noise.

## Failure modes and how the code reports them

- Negative, NaN or infinite loss coefficients, or a non-finite derived matrix entry:
  `prepare()` throws and the previous prepared string is untouched.
- A declared loss on `LeapfrogStiffString`: `prepare()` throws; the explicit prototype is lossless.
- A non-finite right-hand side, energy or dissipation: `advance()` throws and leaves state unchanged.
- `estimate_decay` throws for fewer than eight samples, non-finite data, a zero or constant
  series (rank deficient), and reports the fit residual and regressor condition instead of hiding a
  poor fit. Its roundoff-limited decay error is about \(\varepsilon\,\kappa/(2\gamma\Delta t)\).
  Sampling far above the oscillation frequency over a short window raises \(\kappa\); the
  study keeps it at or below 180.
- Reference parameters with any mode at or beyond critical damping are rejected by the
  analytic reference, whereas the midpoint solver accepts them and classifies the mode as
  non-oscillatory. The two therefore agree only on the underdamped domain.

## Limitations

- Simply supported, uniform, linear, one polarization, uniform grid. The sine modes
  diagonalize the damping operator *only* because the coefficients are uniform; a position-dependent
  loss, a bridge, or a contact breaks the mode-by-mode argument.
- The \(O(N^2)\)-per-step dense solver is an oracle, not a callback path. No timing is claimed.
- The loss magnitudes are arbitrary. No measured partial decay, no separation of string loss from
  bridge leakage (see strings/03), and no frequency-dependent loss richer than three
  nonnegative terms is covered.
- Temporal decay error \(\approx\gamma b^2\) means a *fitted* \(\sigma\) from a coarse run is biased;
  calibration must be repeated under refinement, as the handbook already requires.
- Convergence orders are for joint refinement; spatial and temporal orders were not isolated
  separately in the waveform study.
- Only Linux was run for this change. The Windows and macOS legs of the workflow are configured to
  execute the same CTests and the evidence step but have not been observed here.

## Reproduce

```sh
cmake --preset headless-debug && cmake --build --preset headless-debug
ctest --preset headless-debug -R "loss|wavenumber|invalid_parameters"
python3 tools/mutation_check_loss.py
build/headless-debug/setar_loss_study decay > decay.csv
build/headless-debug/setar_loss_study waveform > waveform.csv
python3 tools/generate_loss_report.py --decay decay.csv --waveform waveform.csv --output artifacts/loss --source-revision <actual-revision>
```

The full study takes about a minute in a Debug build; add `--max-nodes 63` for the quick version
used by `integration.loss_evidence`. The report tool refuses to publish a study whose simulated
decay disagrees with the oracle by more than 10⁻⁶.

## Next actions

1. Isolate spatial and temporal orders separately (fixed small \(\Delta t\), then fixed fine \(h\)).
2. Add the passive bridge and body port to this string (`ENG-P03-T03`, `ENG-P09-T01`) and re-run the
   ledger with the string loss and bridge leakage as separate, nonnegative entries.
3. Extend the loss family beyond three terms only with passive internal variables, and add a
   passivity test over the full represented band before any fitted law is admitted.
4. Replace the dense factorization by a banded solve only after a reference-parity test against the
   present dense solver is in place.
