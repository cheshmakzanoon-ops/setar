# Verified prototype contracts, acceptance mapping, and implementation handoffs

**Status: executable prototype verification, not frozen product ABI.**
The project now has two dependency-free C++20 linear spatial solvers and a
two-mass interface oracle in addition to its original exact synthetic modal
reference. These are deliberately isolated from JUCE and from the physical
instrument model. The final four-string asset schema, contact solver,
multiaxis physical port ABI, multirate state ownership, and realtime plugin
thread boundary **remain undecided**.

Use this document with [proposed target contracts](../architecture/04-cpp20-module-contracts.md),
[experimental numerical evidence](../architecture/07-experimental-solver-evidence-and-choice.md),
[the requirement-to-test matrix](verification-matrix.md), and
[the machine-readable partial coverage ledger](../../roadmap/executable-test-coverage.json).

## What is actually compiled today

| Library/target | Source | Runtime ownership | Permitted claim |
| --- | --- | --- | --- |
| `setar_reference` | [`reference.hpp`](../../include/setar/reference.hpp), [`reference.cpp`](../../src/reference.cpp) | Synthetic finite modal oscillator, linear pluck initial condition and simple resonator | Analytic reference only |
| `setar_numerical_prototype` | [`numerical_prototype.hpp`](../../include/setar/numerical_prototype.hpp), [`numerical_prototype.cpp`](../../src/numerical_prototype.cpp) | Linear fixed-end implicit spatial string with optional passive three-term loss, explicit lossless spatial string, two-mass spring, loss oracles and a decay estimator | Numerical experiments only |
| `setar_numerical_tests` | [`numerical_prototype_tests.cpp`](../../tests/numerical_prototype_tests.cpp) | Fifteen targeted C++ test functions dispatched individually as CTests, four of them for passive loss | Defined subset of synthetic numerical properties |
| `setar_loss_study` | [`loss_study.cpp`](../../apps/loss_study.cpp) | Deterministic decay and waveform-convergence tables for the three-term loss law; shares its analytic comparison with the CTest through `tests/support/modal_crosscheck.hpp` | Quantified synthetic loss-law verification, no measured string loss |
| `setar_compare_solvers` | [`compare_solvers.cpp`](../../apps/compare_solvers.cpp) | Deterministic 60-row sweep of spatial and temporal frequency relations | Quantified synthetic algorithm dispersion and CFL exclusions |
| Python evidence tools | [`generate_solver_report.py`](../../tools/generate_solver_report.py) | Outside callback, plots and JSON/HTML provenance | Actual numerical visualizations, no host screenshot |
| Measurement/qualification validators | [`validate_measurement_campaign.py`](../../tools/validate_measurement_campaign.py), [`validate_product_qualification.py`](../../tools/validate_product_qualification.py) | Offline schema, hash and completeness validation | Failure of fraudulent/missing metadata; not physical truth |

The two strings are not audio engines that allocate four setar courses or
run a musician gesture vocabulary. Their `advance` methods are small
scientific experiments; thread safety, host process admission and bounded
realtime asset lifetime have not been demonstrated. Dense implicit
factorization is intentionally unsuitable for an unstated callback CPU
budget. The explicit linear update's fixed \(O(N)\) loop is **not by itself**
a real-time qualification or proof that contacts remain stable.

## API contracts established in source and tests

### `StringGrid`: SI/domain checks before state mutation

- Inputs: length in metres, tension in newtons, linear mass density in
  kg/m, bending stiffness in N·m², sample rate in Hz, port position fraction
  and number of interior nodes. Defaults are synthetic fixture constants.
- Finite, positive lengths/tension/density, finite nonnegative bending
  stiffness and an admitted rate/node domain are required.
- `MidpointStiffString::prepare` builds the SPD discrete operator and dense
  Cholesky factor **before replacing the prior usable object**. A failed
  prepare leaves its old state and parameters intact. `advance` only accepts
  finite force and keeps state unchanged when its computed new energy fails.
- `LeapfrogStiffString::prepare` has the same common parameter checks **plus
  an analytic full-grid CFL precondition**. An invalid update rate must throw
  and leave any existing prepared configuration untouched.
- Displacement and velocity arrays in the implicit path are per-grid SI
  physical coordinates. They are not sample amplitudes, note velocities,
  microphone sound pressure or internal DSP gain parameters.

### Port contracts and exact temporal levels

| Model | Input | Output/diagnostic | Time-level promise | Known limit |
| --- | --- | --- | --- | --- |
| Midpoint string | `advance(F_mid_N)` | `WorkStep{v_port_mid_m_per_s, external_work_j, stored_energy_change_j, dissipated_energy_j, residual_j}`; residual = stored change − work + dissipated | Force, interpolated velocity and the dissipation form share \(n+\tfrac12\) | Linear fixed support, uniform nonnegative loss, no contact/body |
| Explicit string | `advance(F_at_n_N)` | `LeapfrogWorkStep{work_j, modified_energy_change_j, residual_j}` | Force at integer \(n\), velocity is centered \((q_{n+1}-q_{n-1})/(2\Delta t)\) | Stable CFL only, modified staggered energy |
| Two-mass oracle | `advance()` | Equal/opposite forces, each port's midpoint velocity, spring-energy change, two-port work and total-energy change | Both masses and spring share midpoint | Not a real string–wooden-body connection |

These result structures specify signs and units; they do not imply a
physically fixed source orientation for a three-dimensional bridge. The
future interface must explicitly carry local/global frame transforms,
force/velocity direction and reciprocal interpolation. Complex mobility
at a measured multi-axis bridge requires frequency, tensor and interpolation
conventions that do not belong in this scalar prototype API.

### Failure and lifecycle semantics to preserve

1. `prepare` can allocate or throw and must not run on the audio thread.
   Configure off-thread, then hand off an immutable valid model by a
   separately specified, bounded reclamation policy.
2. Invalid setup, sample rate, point location, span shape, state or force
   fails explicitly rather than being clamped silently. Conversion of
   external messages into physical units belongs to the adapter/preparer.
3. Source/test model state has single-thread ownership; changing the
   coefficient grid while another thread advances it has **undefined
   application behavior** and is not permitted.
4. A prepared, reset synthetic grid produces exact zero state. Prepared
   zero is distinguishable from unprepared; production interfaces should
   return structured error codes rather than throw from `process`.
5. Energy diagnostics are model-specific: midpoint reports semidiscrete
   physical energy; leapfrog reports a discrete staggered modified energy.
   Do not compare their absolute state-energy values as if the definitions
   were identical at arbitrary time levels. Compare physical observables
   and derived conservation identities at aligned times.
6. The scalar two-mass spring is a port time-level oracle. It gives neither
   full 3D force/torque nor a proof of passivity of an independently
   integrated string–FEM reduction. That proof remains a future component gate.

## Exact acceptance status: 79 full planned cases, 10 partially executable mappings

The [requirements registry](../../roadmap/requirements.json) still contains
41 requirements and 79 proposed acceptance identifiers. The
[coverage registry](../../roadmap/executable-test-coverage.json) intentionally
maps only **ten** identifiers to executable *partial* checks:

| Planned acceptance case | Current exact CTest | What it does **not** yet establish |
| --- | --- | --- |
| `numerics.string_spatial_convergence` | Same name | Other supports, nonuniform string or moving boundary |
| `numerics.force_interpolation_adjoint` | Same name | Multiple axes, arbitrary interpolation, random covariant frame checks |
| `numerics.stability_envelope` | Same name | Nonlinear or variable-coefficient stability and complete failure policy |
| `numerics.discrete_energy_ledger` | Same name | Damping/contact/radiation/tuning energy terms |
| `numerics.dispersion_grid_refinement` | Same name | Observed dispersion across actual measured setar strings |
| `numerics.loss_law_passivity` | Same name; see [chapter 14](../numerics/14-damped-midpoint-loss-verification.md) | Measured loss, string-versus-bridge loss separation, nonuniform or contact-coupled loss, other supports |
| `coupling.two_mass_oracle` | Same name | Coupled string/body PDE or measured bridge mobility |
| `coupling.port_power_balance` | Same name | Independent solver/multirate bridge port identity |
| `data.provenance_rejection` | `data.acquisition_manifest_integrity` | Tensor positivity, certified calibration, actual consent |
| `qualification.matrix_integrity` | `data.acquisition_manifest_integrity` | Actual OS/host execution, working plugin, human study |

Additional executable tests `numerics.explicit_cfl_rejection`,
`numerics.explicit_work_ledger`, `numerics.explicit_implicit_comparison`,
`numerics.invalid_parameters`, `numerics.loss_decay_estimator`, `numerics.loss_decay_oracle`,
`numerics.loss_modal_reference_crosscheck`, `integration.solver_evidence`, `integration.loss_evidence`, and
`documentation.executable_coverage_integrity` deepen the prototype but do
not automatically complete further planned cases. The other **69** planned
acceptance names have no partial implementation mapping. No full requirement
is promoted to measured/qualified or even globally numerically verified.

The machine audit
[`tools/audit_executable_coverage.py`](../../tools/audit_executable_coverage.py)
checks the planned IDs, required gaps, runner paths and CMake registrations.
It must fail on invented cases, missing runners or attempts to relabel
prototype tests as full acceptance. Reviewers should reject a PR that changes
the count by simply adding a label or a green empty test.

## How to add the next accepted C++ component without weakening the matrix

1. **Pick exactly one requirement.** Open its precise statement in
   `roadmap/requirements.json`, then the owning `ENG-Pxx-Tyy` work order
   and the long-form method in `docs/phases`. Write the physical claim in
   an issue/ADR: specimen scope, equations, parameter domain, failure
   conditions, independent oracle and measurable pass criterion.
2. **Create an unmeasured synthetic fixture first.** No private recording or
   physically unverified modulus is allowed in a golden C++ fixture. Freeze
   exact SI data, boundary conditions and mathematical expected result.
   Include a version and content hash where it materially affects a claim.
3. **Implement an independent oracle.** An analytic limit, manufactured PDE,
   converged alternate method or independently assembled high-precision
   matrix must exist *before* tuning the production implementation. Merely
   calling a second function backed by the same code path is not independent.
4. **Define the C++ boundary narrowly.** Include units, coordinate frame,
   observable state, time levels, ownership, capacity and initialization.
   Keep allocations and failure throws in preparation; supply fixed-capacity
   nonthrowing processing only when its proof and tests actually exist.
5. **Implement adversarial tests.** At minimum: zero and limiting physical
   parameters, nonfinite inputs, changed sample rate, supported boundary,
   wrong sign/adjoint, roundoff conditioning, energy and work conservation,
   convergence as spatial/temporal resolution changes, and failure rollback.
   Include a deliberately invalid case that would have produced a false pass.
6. **Register each native test in CMake/CTest.** Use a stable exact case ID,
   timeout and meaningful labels. A Python report or screenshot may support
   it but cannot replace independent C++ state/energy tests for a numerical
   component. On Linux add ASan/UBSan and wherever feasible race/allocator
   instrumentation for thread-sensitive code.
7. **Run the three-OS matrix.** Use Linux GCC/Clang, Windows MSVC and macOS
   Clang as available in GitHub Actions. Inspect compiler warnings,
   floating-point tolerances, actual test XML/logs and artifact checksums.
   A workflow file containing an OS name is not proof that a completed
   job passed on that OS.
8. **Publish evidence as inspectable files.** Prefer a raw CSV plus actual
   graph, input manifest, source hash, expected oracle and observed residual
   over a screenshot of a console's `PASS`. Report uncertainty/tolerance
   separately from the numerical error. Do not hide failed runs.
9. **Review the scope before promoting status.** In the coverage registry,
   state exactly which part of a planned test is now executable and which
   pieces are missing. Promotion to a full requirement status needs every
   specified boundary including the real-data and host portions where
   applicable; it cannot be inferred from the file count.

## Cross-platform workflow handoff

```sh
cmake -S . -B build -DSETAR_BUILD_TESTS=ON -DSETAR_BUILD_REPORT_TESTS=ON -DSETAR_ENABLE_JUCE=OFF
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure --output-junit test-results.xml
python3 tools/audit_docs.py
python3 tools/audit_implementation.py
python3 tools/audit_executable_coverage.py
python3 tools/validate_measurement_campaign.py data/measurement-campaign-template.json
python3 tools/validate_product_qualification.py data/product-qualification-template.json
```

CI captures the compiled experiment's exact CSV, renders three actual
numerical SVGs, checks rejection paths and preserves XML/logs on all three
OS. The optional JUCE diagnostic is **not** built by the dependency-free
matrix, so no claim of real VST3 compatibility follows from this gate.

### Future signatures still subject to prototype-backed ADRs

- `PreparedPhysicalModel` must identify a specific reference specimen or a
  clearly synthetic scenario, exact asset hashes, uncertainty and permitted
  parameter domain; admissibility is distinct from empirical correctness.
- `PhysicalPort` must define a 3D coordinate/force orientation, interpolation
  \(B\), conjugate force injection, simultaneous time level, buffering and
  cross-domain work residual. Scalar `WorkStep` is only the initial oracle.
- `ContactState` must express normal complementarity, nail/flesh contact
  memory, tangential friction, release, moving finger/fret and externally
  applied work. No unilateral-law data have been acquired yet.
- `InstrumentState` must own four persistent string histories, a shared
  body/cavity state and all contact states, with memory independent of host
  note events. No such class is currently integrated.
- `RealtimePlan` must bound contact iterations, solver complexity, event
  storage, sample-rate factors, maximum block and asset reclamation. No
  callback allocation/lock audit or measured deadline trace yet exists.
- `HostAdapter` must preserve exact event offsets, tested state migrations,
  actual plugin binary identity and zero/variable-block lifecycle. A JUCE
  prototype is not a measured or qualified product.

The final acceptance of these signatures is a result of integration tests,
not of how polished their declarations look. Retain the simple compilable
experimental interfaces as regression oracles even when faster production
implementations replace them.
