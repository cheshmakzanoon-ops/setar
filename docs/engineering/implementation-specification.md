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
   associates all 119 original `Pxx-Tyy` headings with one or more `ENG-Pxx-Tyy`
   work orders. This closes navigation gaps without overwriting phase-task
   identity or implying that two different tasks are semantically identical.

`Pxx-Tyy` identifies a scientific task and its execution recipe inside a phase
chapter; `ENG-Pxx-Tyy` identifies an implementation work order. Do not derive one
by adding or removing `ENG-`: for example, the crosswalk routes `P04-T02` to
`ENG-P04-T01`, while `ENG-P04-T02` serves the polarization/nonlinearity tasks
`P04-T03` through `P04-T05`. A work order can serve several scientific tasks,
including tasks from another phase. Its `phase` field identifies its owning
phase, whose document is included in every work order's `read` list; the
crosswalk identifies the additional recipes relevant to the chosen question.

`python3 tools/audit_implementation.py` tests these references, DAGs, original
phase heading coverage, and existing reading paths. It cannot certify that a
C++ class is implemented, an equation is correct, or a setar sounds like a real
one. Its mutation-tested consistency rules are necessary housekeeping, not a
substitute for physics.

## How to start a work order

Use the [roadmap execution guide](../roadmap-execution-guide.md) to select a
bounded result and the [research-to-model workflow](../research/research-to-model-workflow.md)
to carry it from evidence to an equation, parameter, observation, or interface
decision. The following procedure applies the selected phase's task-specific
recipe; it does not replace that recipe with a generic implementation checklist.

1. Identify the `Pxx-Tyy` question and look up its exact work orders in the
   crosswalk. When starting from an `ENG-Pxx-Tyy` assignment, inspect the
   crosswalk entries that reference it and select the relevant scientific
   recipes. Check both the phase's scientific prerequisites and the work order's
   `requires` dependencies. Distinguish prerequisites for doing a synthetic
   experiment from evidence required to promote its claim; missing specimen
   access need not block an independent synthetic oracle, but it blocks the
   corresponding specimen-validation claim.
2. Read every entry in the task's `read` list, including its owning phase,
   and the selected recipes' source routes, inputs, procedures, acceptance
   evidence and handoffs. Then inspect the governing mathematics, measurement
   protocol, relevant ADR and actual source. Use `implementation_evidence.paths`
   where present and the current ownership map below. Treat `touch` as an
   intended destination, not proof of an existing module or permission to build
   a parallel engine.
3. State one hypothesis, parameter domain, physical invariant, counterexample,
   and independent oracle. Follow the recipe's research route and
   [source atlas](../research/source-atlas.md); record exactly which source
   sections or raw records were inspected. Use the
   [research-finding template](../../templates/research-finding.md) to preserve
   extraction, SI conversion, sign/frame conventions, uncertainty, access gaps,
   conflicting evidence and a stopping rule. Fix what would falsify the change
   before tuning the implementation or inspecting decisive held-out data.
4. Give the finding an explicit outcome: accepted for stated scope, rejected,
   inconclusive, or blocked. Decide whether it supports no change, a parameter
   or equation change, a numerical method, an observation mapping, or a new
   measurement. Name the existing file/symbol, proposed destination if needed,
   owning requirement and work order, and downstream consumer. An inadmissible
   value remains quarantined; a missing value remains unknown. A negative result
   can resolve a question without passing its phase gate.
5. Prepare a minimal fixture with units, provenance and an immutable manifest.
   Implement the smallest change that tests the supported hypothesis, retaining
   the synthetic preset as a regression fixture. Record intentional
   approximations in output metadata and the ADR; do not hide normalization,
   resampling, or changes to parameter meaning. When acquisition is blocked,
   deliver the precise data request and the independent synthetic work that can
   proceed, with the empirical claim left open.
6. Execute independent unit/matrix/energy tests, numerical refinement,
   adversarial limits, and newly registered CTests. Run all existing tests and
   cross-platform CI for an implementation change. Inspect the
   [executable coverage registry](../../roadmap/executable-test-coverage.json)
   and actual assertions before claiming a planned acceptance name is covered;
   partial synthetic checks retain their stated gaps. Host, hardware, acoustic
   and human evidence have separate gates. Record unrun checks explicitly.
7. Before accepting the change into a versioned model, configuration or preset
   for a supported use, complete a [promotion record](../../templates/parameter-promotion.md).
   Include the quantity-to-field mapping, units and normalization, uncertainty
   and covariance, specimen/setup/raw lineage, identifiability, independent
   verification and held-out prediction as applicable, reviewer decision,
   accepted domain, new version and rollback reference. The current `Provenance`
   enum is metadata; use an evidence sidecar for per-quantity lineage and mixed
   inputs. One measured dimension does not make synthetic contact/body assets
   measured, and passing an admissibility check does not establish fidelity.
8. Deliver the finding and promotion decisions with raw results, plots, source
   revision, compile settings, actual commands/platforms, tolerance derivation,
   performance profile where applicable, failures and unresolved assumptions.
   Identify stale downstream assets/reports, the next work order and the still
   open gate. Update work status **only to the level actually supported** and
   identify the reviewer perspective: numerical, acoustic, software,
   practitioner, rights. Keep executed results distinct from planned checks.

Do not mark a task `verified_for_declared_scope` because its C++ code compiles.
Do not mark it `scientifically_validated_for_declared_scope` because the model
reproduces its own synthetic recording. The reviewed evidence packet and test
result must be traceable by exact revision. New work starts at
`specified_not_implemented`; existing `in_progress` or `implemented_unverified`
entries identify partial work with their own evidence limits. Better recipes and
navigation do not change those statuses.

## Architecture: files and ownership

### Current implementation to inspect first

The repository already contains the analytic reference, independent spatial
numerical experiments, and a playable `setar::instrument` prototype under
`include/setar/instrument/` and `src/instrument/`. The prototype has persistent
string/contact/body state, a performance layer, an offline player, and an
optional JUCE adapter. Read the [prototype capability register](11-playable-prototype.md)
and [baseline source audit](2026-11-10-baseline-audit.md), then inspect the exact
revision under review. Source presence and passing synthetic tests do not
establish a measured specimen model, practitioner approval, controlled realtime
deadlines, or plugin host qualification.

| Current responsibility | Existing entry points | Ownership and evidence boundary |
| --- | --- | --- |
| Analytic reference and spatial numerical laboratory | [reference.hpp](../../include/setar/reference.hpp), [reference.cpp](../../src/reference.cpp); [numerical_prototype.hpp](../../include/setar/numerical_prototype.hpp), [numerical_prototype.cpp](../../src/numerical_prototype.cpp) | Synthetic analytic/modal and spatial/time-step fixtures remain useful independent comparators. They are not a converged independent reference for the complete nonlinear instrument. |
| Configuration and synthetic presets | [config.hpp](../../include/setar/instrument/config.hpp), [config.cpp](../../src/instrument/config.cpp); [setar_preset.hpp](../../include/setar/instrument/setar_preset.hpp), [setar_preset.cpp](../../src/instrument/setar_preset.cpp) | Own physical input fields, admissibility, tuning/fret maps and synthetic defaults. Validation of a number's domain does not validate its provenance or specimen applicability. |
| Modal propagation and bounded contact solve | [numerics.hpp](../../include/setar/instrument/numerics.hpp), [numerics.cpp](../../src/instrument/numerics.cpp) | `ModalBank` and `SmallNewtonSolver` supply current numerical machinery. The proposed `src/numerics/` and `src/contact/` packages are not where this implementation currently lives. |
| Persistent mechanics and shared coupling | [instrument.hpp](../../include/setar/instrument/instrument.hpp), [instrument.cpp](../../src/instrument/instrument.cpp) | `Instrument` owns four persistent strings with two transverse polarizations, the shared modal body, nail/finger/fret channels, bridge springs and the energy ledger. Current contacts and bridge coupling are implemented here, not in the work orders' proposed separate contact/engine files. |
| Body construction and acoustic observation | [body_model.hpp](../../include/setar/instrument/body_model.hpp), [body_model.cpp](../../src/instrument/body_model.cpp); observation in `src/instrument/instrument.cpp` | Builds the synthetic plate/cavity/lateral-mode package and bridge mobility. The engine maps body modal acceleration to uncalibrated observation; measured FRFs, specimen geometry and qualified radiation remain missing. |
| Gesture, score and MIDI translation | [gesture.hpp](../../include/setar/instrument/gesture.hpp), [performance.hpp](../../include/setar/instrument/performance.hpp), [performance.cpp](../../src/instrument/performance.cpp) | `parse_score`, `schedule` and `MidiGestureMapper` create physical events on existing strings. The grammar is an engineering placeholder with no practitioner review; it is not yet a separate `src/performance/` package. |
| Offline rendering and analysis | [setar_play.cpp](../../apps/setar_play.cpp), [analyze_render.py](../../tools/analyze_render.py), [demo scores](../../data/scores/README.md) | The player passes scheduled gestures to the same instrument, writes synthetic WAV/diagnostic records, and leaves plotting/report generation to the analysis tool. Rendering a score supplies no measured acoustic validation. |
| Optional plugin boundary | [plugin/CMakeLists.txt](../../plugin/CMakeLists.txt), [SetarProcessor.cpp](../../plugin/Source/SetarProcessor.cpp), [SetarEditor.cpp](../../plugin/Source/SetarEditor.cpp) | The current adapter lives under `plugin/`, uses `MidiGestureMapper` and the same `Instrument`, and is gated by `SETAR_ENABLE_JUCE_INSTRUMENT`. Configuration/syntax evidence is not a completed plugin build, validator result or host session. |

Both score scheduling and MIDI translation produce `GestureEvent` inputs to
`Instrument::process()`; neither allocates a new physical string for each note.
The offline player owns file output, and the plugin owns host/UI translation.
The portable physical engine retains the mechanics. Preserve that common engine
when changing a wrapper or migrating a module.

Current engine checks live under `tests/instrument/`; the standalone references
have their own tests. [setar_benchmark.cpp](../../apps/setar_benchmark.cpp) and
the [realtime evidence chapter](../realtime/05-prototype-realtime-evidence.md)
record a provisional development-container profile, including dense-gesture
deadline misses. Allocation-hook and bounded-work checks have narrow scopes;
neither they nor that profile qualify target hardware or a plugin host. Use
the [offline player chapter](12-offline-player-and-analysis.md) and
[adapter chapter](../plugin/08-prototype-juce-adapter.md) for their actual
pipelines and outstanding evidence.

### Proposed package layout

The following packages are a recommended destination for reviewed separation of
responsibilities. Most work-order `touch` paths are proposed production files,
not the current implementation locations above. Preserve the independent
references and migrate or extend existing prototype code deliberately; do not
create a second solver simply to make a proposed filename exist.

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
| `apps/plugin` (proposed; current adapter is `plugin/`) | JUCE/host adapter, editor, presets, buses and formats | Authoritative string PDE, contact energy |
| `tools`, `tests`, `experiments`, `data` | Oracles, manifests, calibration, charts, evidence | Silent promotion of simulated data to measured data |

This is a **recommended directory intent**, not permission to freeze unused
abstractions. Before extracting a package, record the current owner, the proposed
owner, state lifetime and interfaces, and the independent checks that must remain
valid. Add one package at a time after a thin analytic test demonstrates its
necessity. Avoid circular coupling: physics/numerics know nothing of JUCE;
the engine calls them; JUCE only translates events and renders output. Offline
FEM/measurement tooling can have heavier, separately reviewed dependencies than
the portable C++20 realtime core.

## Integration order and parallel streams

The machine-readable work DAG is normative for work-order readiness; the phase
register retains the scientific gates. The intended evidence/integration spine
for the verified reference and production architecture is:
reproducible oracle → linear string spatial operator →
time stepping → two-mass work port → nail contact → fret contact → bidirectional
bridge → four-string state → calibrated observation → offline instrument →
realtime reduction → host adapter. A separate branch develops specimen
geometry, orthotropic material validation, body FEM/FRFs, cavity and radiation.

The synthetic prototype already spans several of these responsibilities through
offline rendering and the optional adapter source. Use it as the implementation
under test; the sequence does not imply that those mechanisms or files are absent.
Independent full-instrument comparison, specimen calibration and host
qualification remain open evidence gates.

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
sample`. In the PR body, name the engineering work order and originating
`Pxx-Tyy` recipe separately, then requirement IDs, finding/promotion records,
scope, evidence path, independent expected result, actual test commands and
platforms, linked risks and *explicit* exceptions. For a promoted model or asset,
identify old/new versions, invalidated downstream evidence and rollback. Keep
code, data and reviewer sign-off separate.

The next work order must be determined by prerequisites and missing evidence,
not a date. If a planned class signature conflicts with a better verified
scheme, update the interface decision record and requirements together rather
than implementing the wrong numerical method to satisfy a document.

## Review completion checklist

- [ ] The requested change has exactly one traceable work-order scope.
- [ ] The owning phase and relevant original task recipes are linked through the crosswalk; `Pxx-Tyy` and `ENG-Pxx-Tyy` remain distinct.
- [ ] Physical variables, SI units, frame, sign, time level and data validity are written.
- [ ] The numerical update includes an independent comparator and energy/work ledger.
- [ ] Tests include adversarial inputs, limiting cases and refinement or a reason why inapplicable.
- [ ] Current source owners and proposed destinations are distinguished; implementation and test files match the registered objective.
- [ ] Actual Linux, Windows and macOS CI outcomes are linked after the run finishes.
- [ ] JUCE, actual VST3 hosts, device latency and UI screenshots are claimed only when exercised.
- [ ] Measured versus synthetic results, uncertainty and raw-data permissions are explicit.
- [ ] Findings have scoped outcomes; model/parameter promotions have provenance, independent evidence, versioning and rollback, with unrun gates still open.
- [ ] CPU and solver iteration claims cite measured worst-case configuration and hardware.
- [ ] A changed acceptance tolerance has an evidence-based justification and new regression tests.
- [ ] Public claims do not exceed the relevant Q0–Q4 evidence state.
- [ ] The requirement/task registers, affected ADRs and handoff notes are updated.

Read next: [roadmap execution guide](../roadmap-execution-guide.md),
[research-to-model workflow](../research/research-to-model-workflow.md),
[current prototype contracts](10-prototype-contracts-and-test-expansion.md),
[proposed C++ contracts](../architecture/04-cpp20-module-contracts.md),
[solver and coupling gate](../architecture/05-discrete-solver-selection.md),
[verification matrix](verification-matrix.md),
[performance-envelope method](performance-budget.md),
[specimen completeness](../measurement/specimen-completeness-matrix.md),
and [the release fidelity protocol](../validation/09-final-fidelity-acceptance.md).
