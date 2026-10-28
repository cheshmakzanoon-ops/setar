# Implementation-level engineering specification

## What this layer closes—and what it cannot close yet

The preceding handbook supplies scientific hypotheses, derivations, research
protocols, phase gates, and many proposed interface responsibilities. This
volume converts them into an executable **plan of record**. It is not a finished
source implementation and does not declare a single universal numerical scheme
or invent reference-specimen measurements. Where the correct architecture
depends on measured contact stiffness, bridge mobility, required gesture
bandwidth, or selected hardware, this specification defines the experiment that
must decide it.

There are four complementary indexes:

1. [The phase register](../../roadmap/phase-register.json) states scientific
   dependency and present capability. Its status is *not* release readiness.
2. [The requirements register](../../roadmap/requirements.json) contains stable
   `REQ-AAA-NNN` identifiers, an objective, named *planned* tests and the required
   evidence. A `specified` requirement means the test/design has **not** passed.
3. [The engineering work-order register](../../roadmap/engineering-tasks.json)
   contains `ENG-Pxx-Tyy` jobs with prerequisite work orders, files to inspect,
   intended files to create/change, implementation steps, falsifiable checks and
   the artifact to hand to the next engineer. A proposed file path is not an
   existing file; `<specimen-id>` placeholders are deliberate.
4. [The phase-task crosswalk](../../roadmap/phase-task-crosswalk.json)
   associates all 119 older `Pxx-Tyy` headings with one or more `ENG-Pxx-Tyy`
   work orders. This closes navigation gaps without overwriting phase-task
   identity or implying that two different tasks are semantically identical.

`python3 tools/audit_implementation.py` tests these references, DAGs, original
phase heading coverage, and existing reading paths. It cannot certify that a C++ class is implemented, an
equation is correct, or a setar sounds like a real one. Its mutation-tested
consistency rules are necessary housekeeping, not a substitute for physics.

## How to start a work order

1. Read the identified task and its parent tasks. Check whether the preceding
   phase passed the *scientific* gate, not merely whether its prose is finished.
   An early laboratory implementation may bypass a scientific gate only when
   declared synthetic and excluded from a measured-fidelity claim.
2. Read every entry in the task's `read` list, then the governing mathematics,
   measurement protocol and relevant ADR. Inspect actual source; a listed
   `touch` file can be a proposal and may need a different name after review.
3. State one hypothesis, parameter domain, physical invariant, counterexample,
   and independent oracle. Agree on *what would falsify the change* before
   tuning the implementation.
4. Prepare a minimal fixture with units and a manifest. Implement the smallest
   component that supports the hypothesis. Record any intentional approximation
   in the output metadata and ADR; no hidden normalizations or resampling.
5. Execute independent unit/matrix/energy tests, numerical refinement,
   adversarial limits, and newly registered CTests. Run all existing tests and
   cross-platform CI. Host, hardware and human evidence have separate gates.
6. Deliver raw results, plots, source revision, compile settings, tolerance
   derivation, performance profile where applicable, and a candid failure list.
   Update work status **only to the level actually supported** and identify the
   reviewer perspective: numerical, acoustic, software, practitioner, rights.

Do not mark a task `verified_for_declared_scope` because its C++ code compiles.
Do not mark it `scientifically_validated_for_declared_scope` because the model
reproduces its own synthetic recording. The reviewed evidence packet and test
result must be traceable by exact revision. The current registers deliberately
start all new work at `specified_not_implemented`.

## Architecture: files and ownership

The current source files are `include/setar/reference.hpp` and
`src/reference.cpp` plus small applications/tests. Keep that reference
independent while adding the *proposed* source packages:

| Proposed package | Owns | Must never own |
| --- | --- | --- |
| `include/setar/physics`, `src/physics` | SI quantity types, power port mapping, coupled-step operators | GUI widgets, DAW event IDs |
| `include/setar/numerics`, `src/numerics` | Scalar/grid/matrix reference algorithms, stable integrators, error diagnostics | Undocumented calibration gains |
| `include/setar/strings`, `src/strings` | Multiple physical polarization states, tension/loss, tuning transition | Fresh synth voice per note event |
| `include/setar/contact`, `src/contact` | Finite nail/flesh/fret/finger contact state and work | A recorded pluck as the only sound source |
| `include/setar/body`, `src/body` | Structural references, passive reductions, multi-axis bridge ports | Microphone observation normalization |
| `include/setar/acoustics`, `src/acoustics` | Cavity and exterior radiation/observation operators | Host buffers or plugin state |
| `include/setar/performance`, `src/performance` | Typed gesture/event semantics, tuning/fret grammar | Implicit MIDI note-to-string assumptions |
| `include/setar/engine`, `src/engine` | Prepared immutable assets, live four-string state, one coupled step, bounded render | File I/O, GUI, unbounded allocation inside render |
| `apps/plugin` (proposed) | JUCE/host adapter, editor, presets, buses and formats | Authoritative string PDE, contact energy |
| `tools`, `tests`, `experiments`, `data` | Oracles, manifests, calibration, charts, evidence | Silent promotion of simulated data to measured data |

This is a **recommended directory intent**, not permission to freeze unused
abstractions. Add one package at a time after a thin analytic test demonstrates
its necessity. Avoid circular coupling: physics/numerics know nothing of JUCE;
the engine calls them; JUCE only translates events and renders output. Offline
FEM/measurement tooling can have heavier, separately reviewed dependencies than
the portable C++20 realtime core.

## Integration order and parallel streams

The machine-readable work DAG is normative for readiness. The practical first
integration spine is: reproducible oracle → linear string spatial operator →
time stepping → two-mass work port → nail contact → fret contact → bidirectional
bridge → four-string state → calibrated observation → offline instrument →
realtime reduction → host adapter. A separate branch develops specimen
geometry, orthotropic material validation, body FEM/FRFs, cavity and radiation.

Run *joint* acceptance when streams meet. A passive string and a passive body
can still produce an unstable combination if their sampling levels, delays,
force signs or interpolation adjoints disagree. At that point a `P09` connection
must prove the joint discrete work identity against the independent oracle.

Calibration is not a way to repair an invalid solver. It starts on a separately
verified and sufficiently identifiable model, using a frozen train/validation
split. Realtime reductions follow an accepted offline solution, never replace
the offline model as the only truth. The final VST is an adapter to the physical
instrument, not a synth patch whose outputs are described after the fact.

## Change impact and progress reporting

When a task changes physical parameter meaning, event timing, observation
definition, state layout, or contact algorithm, identify downstream requirements
and task IDs before review. For example, changing a modal basis may invalidate
`REQ-BOD-003`, `REQ-CPL-001`, `REQ-RTS-002`, and migration tests even if one note
still sounds pleasant. Changing microphone gain can invalidate acoustic
comparison but should not alter mechanical state or numerical energy.

Recommended PR subject: `ENG-P09-T01: couple bridge and string at a common power
sample`. In the PR body, use the task ID, requirement IDs, scope, evidence path,
independent expected result, actual test commands and platforms, linked risks,
and *explicit* exceptions. Keep code, data and reviewer sign-off separate.

The next work order must be determined by prerequisites and missing evidence,
not a date. If a planned class signature conflicts with a better verified
scheme, update the interface decision record and requirements together rather
than implementing the wrong numerical method to satisfy a document.

## Review completion checklist

- [ ] The requested change has exactly one traceable work-order scope.
- [ ] Physical variables, SI units, frame, sign, time level and data validity are written.
- [ ] The numerical update includes an independent comparator and energy/work ledger.
- [ ] Tests include adversarial inputs, limiting cases and refinement or a reason why inapplicable.
- [ ] The implementation and test files match the registered objective; stale paths are corrected.
- [ ] Actual Linux, Windows and macOS CI outcomes are linked after the run finishes.
- [ ] JUCE, actual VST3 hosts, device latency and UI screenshots are claimed only when exercised.
- [ ] Measured versus synthetic results, uncertainty and raw-data permissions are explicit.
- [ ] CPU and solver iteration claims cite measured worst-case configuration and hardware.
- [ ] A changed acceptance tolerance has an evidence-based justification and new regression tests.
- [ ] Public claims do not exceed the relevant Q0–Q4 evidence state.
- [ ] The requirement/task registers, affected ADRs and handoff notes are updated.

Read next: [proposed C++ contracts](../architecture/04-cpp20-module-contracts.md),
[solver and coupling gate](../architecture/05-discrete-solver-selection.md),
[verification matrix](verification-matrix.md),
[performance-envelope method](performance-budget.md),
[specimen completeness](../measurement/specimen-completeness-matrix.md),
and [the release fidelity protocol](../validation/09-final-fidelity-acceptance.md).
