# P03 — Verify the linear string before adding realism

**Status:** specification beyond the limited modal bootstrap. **Prerequisite:**
P02. **Exit evidence:** independently verified string modes, boundaries, energy,
forcing, and convergence over a declared numerical domain.

## Objective

Implement a transparent mathematical reference for one string. This phase is
not a tone-design exercise. If a simple string is numerically wrong, body
resonances and nail noise can make the output more attractive while obscuring
the defect. Establish correctness where analytic solutions are available.

Read [mathematics](../mathematics/README.md), [numerics](../numerics/README.md),
and [strings](../strings/README.md). Use the exact scheme derivation in the
numerics volume; do not combine a stencil from one paper with a stability bound
from another.

## Entry contract and research map

P02 supplies the synthetic fixture identity, independent-oracle policy, build
evidence and artifact manifest. This chapter adds work instructions; it does not
promote the existing prototype to completion of P03. Specimen access is not needed
for these mathematical tests. Actual endpoint compliance and calibrated acoustic
claims remain acquisition work under P01/P07.

| Reading route | Decision it supports |
| --- | --- |
| `R-NUM-BILBAO-SMITH-2005`, [author manuscript](https://www.pure.ed.ac.uk/ws/portalfiles/portal/11222313/acustica_final.pdf), §II, then §III and the appendix for the selected scheme | Continuum assumptions, discrete energy, boundary/loss conventions; record the exact equations actually inspected. |
| `R-CONTACT-BILBAO-TORIN-2014`, [conference paper](https://dafx.de/paper-archive/2014/dafx14_stefan_bilbao_numerical_simulation_of_s.pdf), §2–§3 | Stiff-string boundary and explicit-scheme reading route; its contact model is outside this linear phase. |
| [Operator contract](../mathematics/02-linear-operators-and-units.md), [time integration](../numerics/07-time-integration.md), [solver dossier](../architecture/07-experimental-solver-evidence-and-choice.md) | Match the derivation to the implementation and separate stability from phase accuracy. |

The [source atlas](../research/source-atlas.md) records access limits.
`R-NUM-BILBAO-2009` is supplementary reading through its publisher link and an
authorized copy; record that copy's exact chapter before relying on it. None of
these methods sources supplies a measured setar coefficient.

Current reference views are `reference::ModalString` in
[reference.cpp](../../src/reference.cpp), and
`experimental::MidpointStiffString`/`LeapfrogStiffString` in
[numerical_prototype.cpp](../../src/numerical_prototype.cpp).
The first is a finite modal free response; the latter two are uniform, simply
supported grid experiments. The playable `Instrument` adds an internal bridge
and afterlength, so its spectrum is a different comparison problem. For each
recipe retain the [finding record](../../templates/research-finding.md),
raw/derived hashes, exact source locations, commands and actual test outcomes.
Use accepted for the stated scope, rejected with a counterexample, inconclusive
when the comparator cannot discriminate, or blocked with the missing prerequisite.
Proposed work and existing CTests remain separate throughout.

## P03-T01: declare the continuum problem

Use a coordinate x along a string of length L, transverse displacement y in
metres, tension T in newtons, and linear density mu in kilograms per metre.
Begin with constant positive tension/density and small transverse motion. For
the flexible lossless reference, mu y_tt = T y_xx. Document the endpoints,
initial displacement, initial velocity, and any force distribution.

The first boundary fixture is fixed displacement at both ends. The corresponding
normal modes and frequencies are analytic. A later stiff-string fixture requires
additional boundary conditions; fixed displacement alone is insufficient for
a fourth-order spatial equation. Do not silently call a simply supported model
“clamped,” because that changes eigenfunctions and partial frequencies.

Define the output variable. The displacement of one point, endpoint force,
velocity at a bridge, and radiated pressure are not interchangeable. This phase
may observe displacement or velocity, but must not label it sound pressure
without an acoustic observation model.

### Execution recipe

**Question and mapping.** Which closed physical problem fixes the pitch, motion
and endpoint reactions that later string extensions must preserve? Work under
`ENG-P03-T01` and `REQ-STR-001`. Read `R-NUM-BILBAO-SMITH-2005` §II through
the map above and [continuum mechanics](../mathematics/01-string-continuum-and-power.md),
especially C1–C8. Search `fixed string sine mode normalization boundary power`
only to resolve a specific derivation gap; retain the original source location.

1. Freeze a synthetic interval `0 <= x <= L`, displacement `y` in m, velocity in
   m/s, positive `T` in N and `mu` in kg/m. Set stiffness and all loss terms
   explicitly to zero: the current reference defaults are stiff and lossy.
   Specify fixed displacement, compatible smooth initial displacement/velocity,
   force density `f` in N/m, and any point force `F delta(x-xp)` with `F` in N.
   State the time origin and whether observation occurs before or after advance.
2. Substitute `sin(n pi x/L)` into the equation independently. Derive
   `omega_n²=(T/mu)(n pi/L)²` and modal mass `mu L/2` for unit-peak sine shapes;
   derive the force projection using that same normalization. Integrate initial
   kinetic and tension energies independently. Dimensional agreement and zero
   boundary power are required before choosing a numerical representation.
3. Declare observations separately: point displacement, point velocity and signed
   endpoint reaction. A stationary fixed endpoint can exert force while doing
   zero work. Pressure requires P10. For future empirical comparison, obtain
   calibrated length, density, timing and termination records through
   [strings/setup measurement](../measurement/strings-frets-and-setup.md);
   missing records block that comparison, not the synthetic derivation.
4. Map `L` to `StringParameters.length_m` or `StringGrid.length_m`. In
   [StringConfig](../../include/setar/instrument/config.hpp),
   `bridge_position_m` is the speaking length and `total_length_m` extends
   nut-to-tail. Substituting total length into an isolated speaking-string
   formula changes the physical problem; preserve both lengths and bridge state
   in the eventual instrument handoff.

**Acceptance and retention.** Retain the equation/unit table, analytic modes,
energy, boundaries, exclusions and field mapping. A second derivation must recover
the same frequency and mass normalization without reading model state.
`core.frequencies`, `core.analytic_response` and `core.endpoints` in
[reference_tests.cpp](../../tests/reference_tests.cpp) cover limited existing
modal behavior; they do not establish general boundary conditions. Roundoff
tolerances must scale with frequency/mass and arithmetic precision; measurement
uncertainty is a separate future budget. Reject ambiguous endpoints or output
units. An unresolved real termination remains inconclusive. Hand the accepted
synthetic contract to T02/T03 and P04; the register's
`include/setar/numerics/string_operator.hpp` remains a proposed destination.

## P03-T02: implement at least two independent reference views

A modal solution and a spatial finite-difference solution can cross-check one
another in a shared linear case. Their errors and assumptions differ: modal
truncation removes high-frequency components, while a finite grid changes wave
propagation and imposes a resolution limit. Do not interpret agreement between
two similarly truncated representations as proof at frequencies neither resolves.

Start with one exact mode and a known amplitude. Compare time-domain phase,
frequency, energy, and spatial shape. Then superpose a few modes with deliberately
different amplitudes and phases. Verify linearity by comparing the response to a
sum of excitations with the sum of individual responses. Floating-point
differences require a justified tolerance, not exact equality of every sample.

Keep reference computations separate enough to avoid repeating the same indexing
mistake. An expected modal frequency should come from the continuum formula,
not from the model's internal stored frequency returned through an accessor.
An output test must exercise the actual output interpolation or observation path.

### Execution recipe

**Question and research route.** Do different representations predict the same
declared motion, or merely share an erroneous frequency/initialization? This
governs trustworthy pitch and transient comparisons under `ENG-P03-T01`,
`ENG-P03-T02`, `REQ-STR-001` and `REQ-STR-002`. Read
`R-NUM-BILBAO-SMITH-2005` §II–§III, then
[verification benchmarks](../numerics/11-verification-and-benchmarks.md).
Search `string modal expansion finite difference eigenvalue independent oracle`.

1. Independently generate a finite sine sum with prescribed displacement and
   velocity coefficients, initially one mode and then several separated modes
   with nonzero phases. Use identical `L,T,mu`, boundaries, time origin and
   physical observation points. Compute exact mode trajectories and their
   spatial sum without calling production frequency helpers.
2. Initialize `MidpointStiffString::set_state` from that formula. Compare
   full state, modal projections and the actual `port_displacement_m()` path.
   `ModalString` currently exposes triangular initialization, not arbitrary
   modal-state assignment: use an independent test-side analytic sum for this
   fixture, and retain a separate triangle comparison with independently derived
   projection coefficients. Do not describe an invented setter as implemented.
3. Separate three comparators: continuum sine frequencies, independently assembled
   finite-grid eigenvalues, and each integrator's amplification roots.
   For the common grid, midpoint warps frequency downward and leapfrog upward;
   agreement of total errors at one rate can be cancellation. Repeat at a second
   grid/rate and verify superposition, sign reversal and different initial phases.
4. Audit oracle dependence. The current
   [modal_crosscheck.hpp](../../tests/support/modal_crosscheck.hpp) reads initial
   amplitudes from `ModalString::modes()` and observes an on-grid pickup.
   Its separate evolution is useful, but it does not independently verify
   projection or fractional-position interpolation. Add those as proposed test
   fixtures, with a second analyst deriving one expected waveform.

**Acceptance and handoff.** Retain raw state/time tables, oracle derivation,
frequency/phase/energy errors, source hashes and a dependency sketch.
`numerics.explicit_implicit_comparison` and
`numerics.loss_modal_reference_crosscheck` are present limited CTests in
[numerical_prototype_tests.cpp](../../tests/numerical_prototype_tests.cpp);
the latter's resolved three-mode joint-refinement evidence is not a general
independence proof. Set analytic comparison budgets from floating-point
accumulation/conditioning, and approximation budgets from refinement; do not
choose tolerances after seeing the candidate error. Accept only the tested band,
reject shared expected-value construction or phase errors, and mark unresolved
upper modes inconclusive. Missing reference access blocks its claimed method
transfer while analytic fixtures continue. T04/T05 consume the comparators;
`src/numerics/integrator.cpp` is proposed, not another current engine.

## P03-T03: establish boundary and force normalization

Verify endpoint displacement zeros and the sign of reflected waves. Test a
force at a selected position using a distribution whose discrete normalization
represents newtons correctly. A point force on a grid typically requires
considering the cell measure; inserting the same number into an acceleration
array is not a unit-consistent force model.

Use a smooth spatial forcing profile before a sharp point excitation so the
expected convergence is not dominated by a singularity. Compare work computed
from force times the collocated velocity with the state-energy change. An
interpolation operator used to observe contact velocity needs the adjoint
distribution operator for consistent work transfer. Test fractional positions,
not only exact grid nodes.

Exercise positions near both boundaries, equal and opposite force pairs, and
forces with zero integrated spatial sum. Boundary-force sign errors often remain
hidden when every plot uses absolute magnitudes.

### Execution recipe

**Question and mapping.** Does one physical newton perform the same signed work
at every grid resolution and contact position? An error here changes attack
strength, bridge transfer and apparent damping. Follow `ENG-P03-T01`,
`ENG-P03-T03`, `REQ-STR-001`, `REQ-STR-002` and `REQ-CPL-001`.
Read `R-NUM-BILBAO-SMITH-2005` §III boundary handling, the
[weighted-adjoint derivation](../mathematics/02-linear-operators-and-units.md)
and [power ports](../mathematics/03-energy-and-passive-ports.md).
Search `point force quadrature weighted adjoint string boundary work`.

1. Declare `v_p=I v`, nodal generalized force `I^T F` in N, quadrature `W`
   in m, and force density `W^-1 I^T F` in N/m. Acceleration uses
   `M^-1 I^T F`, where `M=mu W`; do not divide by mass twice.
   Independently check `v^T I^T F = F v_p` before integrating.
2. Use a smooth spatial density of known integral and a smooth timed force pulse
   first; retain total force/impulse under refinement. Distributed forcing is a
   proposed harness extension: the current grid API has one scalar point port.
   Then exercise its point port at node and fractional positions, mirror the
   location, approach both endpoints, and reverse force sign. Fixed-end reduced
   interpolation excludes support degrees of freedom; do not renormalize its
   surviving weights near an endpoint to conceal the support reaction.
3. Independently compare flexible-string static compliance at the loaded point,
   `x_p(L-x_p)/(T L)` in m/N, and modal force projection. Add a proposed
   two-port equal/opposite-force case: zero net transverse force does not imply
   zero deformation or work. Test reflected displacement sign and endpoint
   constraints. Nonzero prescribed-support work needs a separate boundary
   fixture; fixed zero velocities alone cannot verify its sign.
4. Compute force work using the solver's actual conjugate time level: midpoint
   velocity for `MidpointStiffString`, centered integer-time velocity for
   `LeapfrogStiffString`. In `MidpointTwoMassPort` include spring-energy change
   in the port-work sum; a compliant connector does not impose identical
   endpoint velocities.

**Evidence and decision.** Retain interpolation/injection matrices, SI impulse,
signed force/velocity traces and independent work residuals. Derive a mixed
absolute/relative work budget from arithmetic and force/velocity scale, using
absolute error near zero work. Present CTests
`numerics.force_interpolation_adjoint`, `coupling.two_mass_oracle` and
`coupling.port_power_balance` cover scalar fixed-grid/two-mass fixtures;
vector, nonuniform and moving-port tests remain extensions. Missing cell measure,
one sign reversal or a one-step force delay must fail a discriminating check.
Reject any such failure; label insufficient boundary evidence inconclusive.
Accept only documented ports, or record blocked measured calibration.
Hand the contract to P05/P06/P09; `include/setar/physics/port_contract.hpp` is
the proposed work-order path.

## P03-T04: derive and test discrete energy

Write the continuum energy for the selected model and derive the numerical
energy for the exact time update. A convenient sum of squared samples is not
automatically that energy. For time-staggered schemes, the energy may involve
states at adjacent times; use the expression established by the numerical
analysis, including positivity restrictions.

Run unforced lossless fixtures over many cycles and inspect drift and bounded
oscillation in the appropriate energy. Repeat at several amplitudes, grid sizes,
sample rates, and initial mode combinations. Include a case close to, but inside,
the stated stability boundary and a deliberately invalid case that preparation
rejects. Do not intentionally emit unbounded audio to speakers during a
stability investigation; use offline state analysis.

Separate accumulation error in the diagnostic sum from error in the solver.
If necessary, compare compensated or higher-precision diagnostic accumulation
without changing the integration. Report absolute energy residual and a
well-conditioned normalization relative to initial energy or delivered work.

### Execution recipe

**Question and sources.** Can the chosen update distinguish supplied work from
numerical energy creation before stronger plucks or body coupling amplify a
defect? This task maps to `ENG-P03-T02`/`ENG-P03-T03` and
`REQ-STR-001`, `REQ-STR-002`, `REQ-CPL-001`. Read
`R-NUM-BILBAO-SMITH-2005` §III and the relevant appendix, then
[time integration](../numerics/07-time-integration.md) and
[the exact explicit scheme](../numerics/01-stiff-string-fdtd.md).
Search `staggered string discrete energy positivity amplification roots`.

1. Start from `H=integral(mu v²+T y_x²)/2 dx` for the flexible fixture.
   Derive the grid mass/stiffness forms with the same boundary rows and
   quadrature as T03. Midpoint balances the integer-time mechanical energies
   `H(q_n,v_n)` and `H(q_(n+1),v_(n+1))` using midpoint port velocity;
   lossless leapfrog uses the half-step kinetic energy plus the cross-state
   potential `q_(n+1)^T K q_n/2`. Independently verify the ledger rather than
   defining dissipated or supplied work as the unexplained energy difference.
2. For leapfrog derive positivity of `M-dt² K/4` and
   `dt omega_max < 2`. Check the actual finite-grid highest eigenvalue and
   initialization. The implemented `leapfrog_minimum_stable_rate_hz` uses this
   spectrum and preparation requires a strict margin. The handbook's lossy
   backward-gradient scheme has a different effective mass; it is not the
   currently lossless `LeapfrogStiffString`. Midpoint's linear stability does
   not imply accurate high-mode phase or nonlinear-contact stability.
3. Run zero-force smooth modes, nonzero initial velocities, mixed modes,
   amplitude scaling and T03's driven port. Include a near-boundary admissible
   case and an invalid preparation case. Evaluate independent energy sums with
   higher-precision or compensated accumulation while leaving the candidate
   solver unchanged. Track per-step and cumulative residual in J.
4. Normalize with declared initial energy plus accumulated absolute input work
   and a positive fixture scale; do not divide by decaying instantaneous energy.
   Derive tolerance from operation count, conditioning and accumulation length,
   and test whether changing diagnostic precision changes the conclusion.
   A suspected solver defect must survive that diagnostic comparison.

**Acceptance and handoff.** Preserve the derivation, time-level table,
stability margin, energy/work traces and rejected configurations.
`core.lossless_energy`, `numerics.discrete_energy_ledger`,
`numerics.stability_envelope`, `numerics.explicit_cfl_rejection` and
`numerics.explicit_work_ledger` are current limited checks, not a proof for
changing supports. Accept a scoped identity plus independent trajectory check;
reject unexplained growth or hidden clipping. If accumulation masks the error,
record inconclusive; unavailable apparatus only blocks later physical validation.
T05 consumes the validity envelope and P04/P09 inherit the ledger before adding
loss, nonlinearity or coupling.

## P03-T05: establish convergence and dispersion

Choose a physical problem and vary spatial and temporal resolution in a way that
respects stability. Measure frequency and phase error over a stated duration,
spatial error at aligned physical coordinates, and energy behavior. A successful
run at one resolution does not establish convergence. If changing grid spacing
also changes the physical string length, the comparison is invalid.

When an explicit stability relation links step sizes, explain what quantity is
being held fixed and what order of accuracy the experiment can reveal. Do not
fit a slope across a pre-asymptotic region or round-off plateau and call it the
scheme order. Plot the residuals and identify the usable convergence regime.
Non-smooth initial displacement can lower observed convergence; distinguish
that effect from an implementation error using a smooth fixture.

Compute a numerical-dispersion map showing error by partial number or wavelength.
Use it to define a usable frequency band, not only a Nyquist limit. A scheme can
be stable while badly dispersive near the grid limit. Later real-time reduction
must compare against the established reference band.

### Execution recipe

**Question and route.** Which partials and propagation times are accurate enough
to serve as a reference, even when every run is stable? Incorrect upper-partial
spacing changes brightness and beating. Use `ENG-P03-T01`/`ENG-P03-T02`,
`REQ-STR-001` and `REQ-STR-002`. Read `R-NUM-BILBAO-SMITH-2005` §III,
[dispersion metrics D1–D8](../numerics/08-dispersion-and-dissipation.md) and
[convergence design](../numerics/12-convergence-and-golden-data.md).
Search `string numerical dispersion modified wavenumber temporal refinement`.

1. Freeze geometry, material, smooth finite-band initial state, force history,
   observation coordinates and physical duration. With `N` intervals set
   `h=L/N` exactly; the API's `interior_nodes` is `N-1`.
   Independently compute `s_n=4 sin²(n pi/(2N))/h²` and continuum modes.
   Compare grid eigenvalues directly to isolate spatial error without a
   time-step estimator.
2. Hold one grid fixed and refine `dt` against its independently computed
   semidiscrete trajectories. Separately refine space with temporal error shown
   to be below the spatial residual. When explicit stability prevents a fixed
   step across grids, state the coupled refinement and do not report separate
   orders from it. Preserve admissible and rejected resolution combinations.
3. Plot signed Hz/cents error, phase accumulation in radians, spatial
   displacement/velocity norms at aligned physical positions, and energy/work
   error. Compare midpoint `2 atan(dt omega_h/2)/dt` and leapfrog
   `2 asin(dt omega_h/2)/dt` against the same `omega_h`.
   Fractional pickup interpolation is a separate error; repeat at aligned and
   nonaligned pickups. A triangle is a useful finite-mode regression but its
   singular curvature is unsuitable for stiff-string energy convergence.
4. Estimate slopes only where successive refinements show an asymptotic regime;
   retain pre-asymptotic and roundoff plateaus. Define the accepted band through
   a proposed pitch/phase/transient error allocation and downstream need.
   Convert a proposed cents budget to relative frequency error with
   `2^(c/1200)-1`; do not call that budget a measured hearing threshold.

**Decision and translation.** Retain convergence tables, signed dispersion maps,
effective mode counts, error allocations and reference-refinement sensitivity.
`numerics.string_spatial_convergence` compares analytic formulas for two low
modes on five grids; `numerics.dispersion_grid_refinement` and
`numerics.explicit_implicit_comparison` add limited scheme checks. They do not
cover every boundary or independently assembled operator. Reject fitted
stiffness used to cancel discretization, or a claimed order from a plateau.
Accept a bounded reference band, report inconclusive unresolved modes, and record
blocked empirical audibility separately. Hand usable resolutions and errors to
P04 and P13's reference-versus-runtime comparison.

## P03-T06: document failure boundaries

Reject nonpositive lengths/densities, invalid tension, nonfinite inputs, unsupported
sample rates, and insufficient resolution. Preserve the previous prepared state
if a replacement configuration fails validation. Document whether changing a
parameter is allowed during processing or requires preparation. Thread ownership
of the state must be explicit even in the offline prototype.

Add tests for reset, repeated prepare, segmentation into different block sizes,
and deterministic forcing schedules. These tests ensure the mathematical
experiment is not changed by software buffering. They become essential when
MIDI and host callbacks arrive.

### Execution recipe

**Question and mapping.** Can an unsupported configuration or buffering change
silently alter the mathematical experiment? Work under `ENG-P03-T02` and
`REQ-STR-002`. Use `R-NUM-BILBAO-SMITH-2005` §III for the model-specific
stability argument and [floating-point/failure contracts](../numerics/10-floating-point-and-realtime.md).
Search `string CFL positive discrete energy invalid configuration` for a
specific unresolved bound; derive software limits from inspected source rather
than treating a paper's domain as the API's.

1. Make an admissibility table from
   [reference.hpp](../../include/setar/reference.hpp),
   [numerical_prototype.hpp](../../include/setar/numerical_prototype.hpp)
   and their validation bodies. Currently the isolated reference accepts
   1–128 modes and the grid 4–128 interior nodes; their supported rates are
   8–384 kHz. Positive tension is required by these APIs even though a
   different beam model could permit zero tension. The modal reference requires
   each retained mode below `0.45 fs` and undercritical damping. These are
   implementation limits, not measured instrument limits.
2. For each bound test a valid interior case, the declared boundary, an invalid
   neighboring value, NaN/infinity where applicable, and overflow/underflow in
   derived mass/frequency. Check explicit CFL rejection and refusal of any
   nonzero leapfrog loss. Verify exception/fault category, previous configuration,
   physical state and next deterministic sample after failed replacement.
3. Prepare once, run a frozen force schedule indexed by absolute sample, reset
   and repeat, then process the same schedule through unequal partitions and
   zero-length calls where supported. Test repeated successful preparation
   separately from rejected preparation. The grid exposes per-step advance,
   so block partition is a harness concern; `ModalString::render` exercises
   the existing render boundary. Do not introduce concurrency into an object
   whose contract forbids concurrent preparation and processing.
4. Retain the first failed configuration and its raw diagnostics. A nonfinite
   state invalidates evidence; containment is not successful synthesis.
   Parameters requiring preparation must not be changed by mutating a stored
   configuration during processing.

**Acceptance and handoff.** `core.parameters`, `core.reset`,
`core.block_partition` and `numerics.invalid_parameters` are present;
the instrument's `engine.config_validation` has its own broader contract.
Exact same-binary partition identity is appropriate where operation order is
unchanged; cross-compiler physical residuals use P02's numerical budget.
Reject state mutation after failure or a block-dependent force time. Mark an
uncovered boundary inconclusive and unavailable platform evidence blocked.
Deliver the domain/rejection table, commands and state comparisons to P04,
P11 and P13; preserve the proposed general integrator tests as open work.

## Evidence and prompt

Required artifacts include equations with assumptions, scheme specification,
analytic comparisons, boundary/work tests, convergence plots, dispersion maps,
invalid-input results, and exact commands. All fixtures are synthetic.

> Implement the specified linear-string case only. State the continuum and
> discrete boundary conditions, SI units, and supported parameter domain. Derive
> the relevant energy and stability conditions or cite the exact verified scheme
> derivation. Add independent modal, forcing-work, boundary, and convergence
> tests. Do not add a body filter, aesthetic EQ, randomized variation, or a claim
> of setar realism. Report both successful and rejected configurations.

Advance to P04 only when the new physics can be compared against a reliable
linear baseline. Preserve the P03 fixtures permanently as limiting-case tests.

The handoff must identify the following without relying on private notes:

- Exact continuum/discrete boundary, SI state/port convention and speaking-span
  mapping, with the actual current implementation and any proposed extension.
- Independently derived trajectories and force/work checks, both resolution
  directions, accepted partial band and the reason for every numerical budget.
- The actual CTest inventory/results, negative cases and unimplemented portions
  of the registered acceptance names.
- Raw/derived manifests and source locations, an explicit disposition for every
  tested mechanism, and P04/P09/P13 consumers of the accepted scope.
- Separate blocked specimen claims; synthetic mathematical verification may
  continue without changing any acoustic-fidelity status.
