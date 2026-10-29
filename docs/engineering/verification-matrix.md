# Requirements-to-tests verification matrix and portable execution contract

**Status:** full acceptance specifications, with a synthetic numerical baseline
and **ten explicitly partial executable mappings**. The implemented subset
is audited in [executable-test-coverage.json](../../roadmap/executable-test-coverage.json),
and its remaining gaps are recorded in
[prototype contracts and test expansion](10-prototype-contracts-and-test-expansion.md).
Do not confuse a CTest with completion of the full acceptance specification.
The exact planned identifiers live
in [requirements.json](../../roadmap/requirements.json), and work ownership
lives in [engineering-tasks.json](../../roadmap/engineering-tasks.json).

## Test layers must answer different questions

| Layer | Question answered | Accepted comparator | Typical artifact |
| --- | --- | --- | --- |
| Analytic verification | Was this mathematical model solved correctly in a known limit? | Closed form, dimensional/energy identity, manufactured solution | Equation, raw residual, error trend |
| Spatial/time convergence | Does discretization approach its stated continuum or trusted reference? | Independently converged method, grid/time refinement | Order estimates, error vs resolution |
| Contact and power | Do interactions preserve work and constraints? | Exact scalar potential, two-mass oracle, energy ledger | Force/velocity/displacement/work traces |
| Structural/acoustic validation | Does the model predict a real instrument condition? | Calibrated *held-out* FRFs, pressure and gestures | Raw measurements, uncertainties, matched predictions |
| Performance validity | Can it perform idiomatic setar gestures? | Practitioner review and controlled phrase test | Gesture annotations, consent, failure examples |
| Realtime qualification | Is the supported operating envelope bounded and stable? | Allocator/lock instrumentation, deadline profile, stress | Call graph, distributions and hardware info |
| Host interoperability | Is the actual plugin usable across supported hosts? | Steinberg validator, real host sessions, differential render | Validator logs, scan, state/midi/bus traces |
| Perceptual validity | Are defined sounds and responses convincing under controlled listening? | Blinded preregistered study | Effect sizes/intervals, masking and randomization log |
| Packaging/compliance | Can a package be distributed lawfully and rebuilt? | Clean-install test, licenses, provenance hashes | Release manifest, installation and rights audit |

Green results at one layer cannot be used as a proxy for another. A numerical
mode-frequency convergence plot has no evidence about nail timing; a convincing
WAV has no evidence about passive bridge connection; a VST3 validator pass does
not prove realistic setar motion.

## Exact test families and independent expected results

### T0. Data, requirements and provenance

- `governance.claim_trace`: build a dependency graph from every public claim to
  a specimen, permission and evidence packet. Deliberately delete one source;
  the claim must become unsubstantiated. Human review still determines truth.
- `data.provenance_rejection`: load well-formed and corrupted unit, source,
  consent and tensor cases. A valid schema cannot make contradictory physical
  data admissible; check both structural and physical validators.
- `data.geometry_registration`: invert a transform, swap axes, perturb scale,
  or omit calibration. Compare reconstructed landmarks to independent
  calibrated references, propagating uncertainty.
- `materials.reciprocity`, `materials.positive_energy`: build explicit
  compliance/stiffness matrices, evaluate reciprocal Poisson relations and
  eigenvalue or Cholesky admissibility. Check rotated coordinates and deliberate
  non-SPD matrices. Never repair wrong literature values silently.
- `ci.matrix_coverage`: inspect **actual** completed OS/job records and hashes;
  a configured workflow or skipped job must not count as a pass.

### T1. Linear string and integrator

- `numerics.string_spatial_convergence`: fixed-free and supported boundary
  fixtures with known modal frequencies/mode shapes; independently assembled
  matrix vs optimized stencil; spatial refinement under fixed physics.
- `numerics.force_interpolation_adjoint`: random vector identity in the mass
  or quadrature inner product with boundary and rotated-frame cases. Detect
  a factor-of-grid-spacing error using the discrete work equation.
- `numerics.stability_envelope`: derive eigenvalues/amplification roots for the
  *actual* update rule. Sweep parameter extremes and an outside-bound case.
  Rejected unstable configurations are acceptable negative evidence.
- `numerics.discrete_energy_ledger`: free, driven and damped runs; compare
  \(E_{n+1}-E_n\) with independently integrated work and loss. Report absolute
  and scale-normalized numerical residual, not just a green boolean.
- `numerics.dispersion_grid_refinement`, `numerics.loss_law_passivity`: compare
  stiffness-dependent partial frequencies and pole decay with analytic modal
  limits, alternate discretization and zero-EI/zero-loss cases. The loss half is
  now partially executable for the uniform simply supported fixture; see
  [damped-midpoint loss verification](../numerics/14-damped-midpoint-loss-verification.md)
  for what it covers and what remains (measured loss, bridge leakage, nonuniform loss).

### T2. Nonlinearity, excitation, frets, retuning

- `numerics.polarization_decoupled_limit`: set transverse coupling to zero;
  independently verify each scalar solution, then rotate the physical frame.
- `numerics.nonlin_tension_work`: derive energy from strain potential and
  compare discrete state change to internally computed force work. Detect a
  wrong sign or missing cross polarization term.
- `contact.nail_passivity`, `contact.release_convergence`: use a separately
  evaluated scalar potential and high-resolution contact trace; measure
  compression, relative velocity, force, work and detachment time. Intentionally
  cap nonlinear iterations to test the failure response.
- `contact.gesture_replay`, `contact.strike_speed_envelope`: fixed event streams
  with timing and units, same-sample ordering and new strokes on existing state;
  compare block-partition outputs and contact work, then withheld stroke angles.
- `contact.fret_crossing_work`, `contact.release_afterlength`: move a unilateral
  constraint through an energized string, calculate external finger work, and
  verify afterlength energy is not destroyed by a note-on mapping.
- `controls.retune_energy_ledger`, `controls.sweep_continuity`: test both a
  physically modeled tension actuator and a declared convenience transition.
  A reset must show its nonphysical energy deficit explicitly.

### T3. Body, coupling, acoustic observation

- `fem.patch_test`, `fem.mesh_convergence`: independently known displacement,
  force or patch strain and mode/FRF refinement; inspect element orientation
  and boundary effects rather than fitting one microphone magnitude.
- `body.mobility_reciprocity`, `body.mobility_passivity`: full complex port
  matrices at declared frames and frequency band; compare Hermitian/power
  properties against appropriate passive assumptions and measurement
  uncertainty, not a universal reciprocity assertion without conditions.
- `body.reduction_heldout_frf`, `body.reduction_passivity`: hold out force
  directions, positions and frequencies; verify retained residues and passive
  extrapolation, not only a fitted set of peaks.
- `coupling.two_mass_oracle`, `coupling.port_power_balance`: analytic two-mass
  coupling and a monolithic high-resolution string/body reference. Include a
  deliberately lagged feedback path that must fail phase/work criteria.
- `coupling.cross_string_transfer`, `coupling.uncoupled_limit`: drive one of
  four strings with the others undriven; observe mechanical cross-port motion
  and its disappearance when structural coupling is removed.
- `acoustics.interface_adjoint`, `acoustics.cavity_mode_convergence`: verify
  paired pressure/volume-velocity work, manufactured boundary motion and
  independent known cavity modes under mesh refinement.
- `acoustics.observation_invariance`, `acoustics.directivity_holdout`: move
  microphone without altering mechanical state (for one-way observation) and
  compare withheld angles/distances with calibrated pressure data.

### T4. Integrated playing, calibration, realtime

- `performance.persistent_state`, `performance.restrike_memory`: two plucks on
  one physical string must preserve earlier motion; verify unstruck-string
  symmetry and shared body state, not a fresh independent synthesizer voice.
- `performance.gesture_matrix`, `performance.repertoire_holdout`: evaluate
  practitioner-approved gestures under an explicit event grammar, with
  prohibited or ambiguous host mappings identified.
- `identify.synthetic_recovery`, `identify.sloppy_direction`: synthetic known
  parameter recovery plus a deliberately rank-deficient model; check singular
  directions and uncertainties rather than only optimizer loss.
- `identify.heldout_reproduction`, `identify.leakage_guard`: freeze train/holdout
  split, assert holdout bytes never enter optimizer/normalization, then compare
  predictions and intervals to untouched recordings.
- `realtime.reference_parity`, `realtime.multirate_alias`: compare physical
  states/ports and output against an independent converged high-rate reference;
  sweep contact impacts, retuning and downsample filters separately.
- `realtime.allocation_hook`, `realtime.max_work`: instrument actual callback
  paths, including asset changes, repeated contacts and overload; test finite
  iteration and capacity policy. A bounded loop's source code is only partial
  evidence until timing is measured.
- `realtime.asset_handoff`, `realtime.telemetry_snapshot`: stress pointer
  lifetimes, invalid candidate rejection, stale snapshot consumption and lost
  diagnostics. Use sanitizer/race evidence where supported and a documented
  concurrency proof elsewhere.

### T5. Host, UI and qualification

- `plugin.core_headless_link`, `plugin.wrapper_smoke`: compile the physical
  library without JUCE and separately build/scan the actual plugin when the
  licensed SDK is available. Report missing dependency as unrun, not passed.
- `plugin.event_partition`, `plugin.state_roundtrip`, `plugin.malformed_state`:
  differential physical renders, stable IDs, endian/byte bounds, old schema,
  unknown fields, power failures and repeated preset replacement.
- `plugin.lifecycle_matrix`, `plugin.repeated_instance`: zero/variable blocks,
  bus layout, transport, rate changes, bypass, offline render, suspend/resume,
  multi-instance ownership and real host scans.
- `ui.control_semantics`, `ui.accessibility_keyboard`: ensure audible controls
  describe physical versus observation effects, reachability and keyboard
  focus; include actual setar-player reviews.
- `ui.plot_schema`, `ui.screenshot_regression`: validate plot units, measured
  labels, provenance and deterministic UI rendering. Actual captures, not SVG
  wireframes, are the visual baseline.
- `qualification.matrix_integrity`, `qualification.host_rescan`: map every
  claimed OS/host combination to real run metadata and artifact hash.
- `qualification.fault_injection`, `qualification.note_release_priority`:
  inject solver, queue and nonfinite faults; show safe silence and no stuck
  contacts without converting the physical failure to a pass.

### T6. Validation, release, stewardship

- `fidelity.heldout_acoustic`, `fidelity.metric_reliability`: preregistered
  paired pressure/transient/partial/decay/directivity comparisons with
  uncertainty, independent sessions, matched observation and noise floors.
- `fidelity.blind_listening`, `fidelity.performer_coverage`: blinded auditory
  judgments and separately measured playability/gesture coverage; counterbalance
  presentation and report power/effect sizes and negative findings.
- `release.license_audit`, `release.asset_rights`: enumerate *actual* package
  files, libraries, SDK and source recordings, checking authorization rather
  than inferring rights from repository access.
- `release.rebuild_trace`, `release.package_smoke`: clean rebuild and smoke test
  the installed VST3 and any approved other format by exact binary/asset hash.
- `maintenance.golden_review`, `maintenance.corpus_migration`,
  `maintenance.provenance_replay`, `maintenance.revocation_path`: detect stale
  benchmarks/consents, migrate in a defined schema, and trace withdrawal of
  invalidated claims and newly prohibited assets.

## CTest and cross-platform acceptance mechanics

The recommended headless layout adds bounded executables in `tests/<domain>/`
while keeping all third-party dependencies optional/offline where possible.
For each proposed case, register a stable CTest name, labels including
`numerical`, `integration`, `contract`, `realtime` or `documentation` as relevant,
and an explicit timeout. Failed tests must be nonzero in release builds too;
`assert` alone is not enough under `NDEBUG`.

```cmake
# Illustrative future registration; not a target that currently exists.
add_test(NAME coupling.port_power_balance
         COMMAND setar_coupling_tests --case port_power_balance)
set_tests_properties(coupling.port_power_balance PROPERTIES
    LABELS "contract;numerical;coupling" TIMEOUT 60)
```

The existing portable configure/build/test sequence must remain valid:

```sh
cmake -S . -B build -DSETAR_ENABLE_JUCE=OFF -DSETAR_BUILD_TESTS=ON
cmake --build build --config Debug --parallel 2
ctest --test-dir build -C Debug --output-on-failure
python3 tools/audit_docs.py
python3 tools/audit_implementation.py
python3 tools/test_implementation_audit.py
```

When an engineer implements a test family, the ordinary Linux/Windows/macOS
`headless` GitHub Actions jobs must execute its CTest. If a test needs a
special machine, real audio hardware, user interaction, purchased SDK/DAW, a
graphics server or nonredistributable data, label it **conditional** and
separate its evidence from the portable headless gate. This is not permission
to skip all portable aspects: the same module still needs deterministic
synthetic unit/interface tests in normal CI. Sanitizers, race testing, AVX/NEON
variants, host scanning and device profiling are complementary, not substitutes.

## Tolerance rubric (required in every numerical test)

Before inspecting the candidate output, record:

1. Quantity, units, observation location and expected zero/nonzero scale.
2. Independent oracle and its own error/precision limitations.
3. Derived stability/consistency bound or measurement repeatability/noise.
4. Tolerance form (`absolute`, `relative`, `weighted`, spectral or statistical).
5. Valid input domain and intended adversarial counterexamples.
6. Refinement trend or independent cross-method comparison that guards against
   simply blessing the current implementation.

For a generic energy ledger use a *dimensionally consistent* comparison such as
`abs(delta_energy - external_work + losses) <= tau_abs_J + tau_rel * energy_scale_J`,
with each tolerance justified from numerical residual, reference error and
conditioning. Do not adopt a universal numerical value merely because one
existing synthetic test happens to pass it. Near a zero result, report absolute
error and the energy scale separately; a relative-only comparison is undefined.

For time-domain audio, keep any synchronization shift explicit. For spectra,
state window, length, weighting, level reference and frequency band. For
measured FRFs, retain phase/coherence and uncertainty—log-magnitude correlation
alone can mask a wrong bridge power response. For listening, don't treat a
non-significant small study as proof of 1:1 indistinguishability.

## Review the negative path

Before merging any new case, deliberately break one expected property: invert a
contact normal, add one sample of bridge delay, change microphone gain while
comparing physical energy, swap orthotropic axes, reduce a contact iteration
limit, or leak held-out samples into fitting. Demonstrate the targeted test
rejects the defect. Save the minimized reproducer and the test's expected
failure category without checking in proprietary raw recordings.

See [performance budget](performance-budget.md) for realtime measurement
requirements, [VST3 qualification](../plugin/06-host-qualification-playbook.md)
for executable host steps, and [fidelity protocol](../validation/09-final-fidelity-acceptance.md)
for statistical/performer acceptance beyond CI.
