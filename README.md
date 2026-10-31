# Setar — physical-modeling engineering laboratory

A research-first, C++20 project for a physically modeled Iranian **setar** (سه‌تار).
The intended instrument generates sound from physical state and interactions, not
prerecorded notes. JUCE belongs at application/plugin boundaries; it is not the
physics engine.

**Current status:** an engineering handbook, an executable numerical bootstrap and
a **playable synthetic prototype** (`setar::instrument`): four persistent
two-polarization modal strings coupled through bridge springs to a synthetic
modal body, with fingernail, finger-pad and tied-fret contacts solved by an
energy-consistent discrete-gradient Newton step, sample-accurate physical
gestures, a score-driven offline player and an optional JUCE adapter. Every
parameter it uses is a declared synthetic placeholder. This is **not a finished
or validated setar synthesizer**, not a measured digital twin, and not a claim of
parity with any commercial instrument. Read the
[prototype chapter](docs/engineering/11-playable-prototype.md) for exactly what
is implemented, tested and still open.

## Start here

1. [Read the orientation and capability boundaries](docs/START_HERE.md).
2. [Follow the phase-by-phase engineering roadmap](docs/ROADMAP.md).
3. [Use the evidence and acceptance rules](docs/governance/quality-contract.md).
4. [Open the implementation-level engineering specification](docs/engineering/implementation-specification.md):
   [41 requirements](roadmap/requirements.json) and
   [44 detailed work orders](roadmap/engineering-tasks.json) with dependencies,
   source/test locations, implementation instructions, and evidence handoffs.
   The [phase-task crosswalk](roadmap/phase-task-crosswalk.json) connects all
   **119 original scientific phase tasks** to those granular work orders.
5. [Read the relevant technical volume](docs/INDEX.md), not the whole handbook at once.
6. [Select a bounded implementation or review prompt](prompts/README.md).
7. Build the reference scaffold, reproduce its tests, and attach evidence to changes.

The [open decision-gate ledger](docs/governance/engineering-decision-gates.md)
identifies measurements, method choices, permissions, and host qualifications
that cannot be resolved by writing more prose. **Planned tests and proposed
C++ interfaces are not necessarily implemented features.** The new
[executed-prototype coverage registry](roadmap/executable-test-coverage.json)
maps **27 of 79 planned acceptance names to *partial* executable checks**;
52 are not yet mapped, and the full 41 engineering requirements are not
claimed as completely verified. The
[solver comparison and decision dossier](docs/architecture/07-experimental-solver-evidence-and-choice.md),
[real specimen acquisition instructions](docs/measurement/13-acquisition-campaign-execution.md),
[C++ contract and executable test handoff](docs/engineering/10-prototype-contracts-and-test-expansion.md)
and [real VST3/product qualification gates](docs/plugin/07-product-qualification-evidence-gates.md)
identify the exact current evidence and outstanding work.

There are **no project deadlines or calendar estimates** in this roadmap. Phases
advance when their evidence gates are met. Activities such as data acquisition,
numerical verification, and interface design can proceed independently where the
dependency graph permits it.

## Project boundaries

| Commitment | Meaning |
| --- | --- |
| Physics-led synthesis | Derive motion and acoustic output from documented models and state. |
| C++ first | A dependency-light C++20 numerical core, with explicit units and ownership. |
| Honest evidence | Distinguish hypotheses, synthetic fixtures, published values, measured data, and validated conclusions. |
| Cross-platform | Linux, Windows, and macOS build/test workflows; separate manual host and audio-hardware evidence. |
| Reproducibility | Pinned inputs, deterministic fixtures, tolerance policies, artifacts, and traceable decisions. |
| Cultural specificity | Model Iranian setar construction and performance, with practitioner review; do not substitute an Indian sitar or generic guitar. |
| Measurable fidelity | Define a reference specimen, gestures, listening conditions, error budgets, and held-out tests. |

“Pianoteq-level” is a user aspiration for responsiveness and physical-modeling
quality. Pianoteq is a third-party product and trademark, not an implementation
reference, an affiliation, or a certified quality level. This project does not
copy proprietary source, presets, measurements, or designs.

## Build the numerical bootstrap and the prototype

The intended baseline is CMake, a C++20 compiler, and Python 3 for report and
documentation tooling. See [engineering setup](docs/engineering/README.md) for
the exact current build commands and toolchain requirements.

```sh
cmake --preset headless-debug
cmake --build --preset headless-debug
ctest --preset headless-debug
build/headless-debug/setar_play --score data/scores/01-open-strings.score --output-dir artifacts/play/open-strings
python3 tools/audit_docs.py
python3 tools/audit_implementation.py
python3 tools/audit_executable_coverage.py
python3 tools/validate_measurement_campaign.py data/measurement-campaign-template.json
python3 tools/validate_product_qualification.py data/product-qualification-template.json
```

The headless path must not download JUCE or require an audio device. The optional
JUCE path requires an explicitly supplied checkout and a reviewed license choice.
Do not mistake a passing bootstrap test, or a pleasant prototype render, for
measured instrument validation. `setar_play` writes the render of a synthetic
score together with its diagnostics; see the
[offline player chapter](docs/engineering/12-offline-player-and-analysis.md).

## Repository map

| Path | Responsibility |
| --- | --- |
| `docs/phases/` | Ordered work packages, dependencies, evidence gates, and handoffs. |
| `roadmap/requirements.json`, `roadmap/engineering-tasks.json` | Machine-checked requirements, 44 implementation work orders, named planned tests and exact procedures. |
| `docs/engineering/implementation-specification.md`, `docs/architecture/` | Implementation contracts, proposed C++ ownership/interfaces, solver selection and signal-flow diagrams. |
| `docs/research/`, `docs/measurement/` | Primary-source audit, specimen acquisition, measurement protocols, and uncertainty. |
| `docs/mathematics/`, `docs/numerics/` | Governing equations, discretization, energy, stability, and convergence. |
| `docs/strings/`, `docs/contacts/`, `docs/performance/` | String behavior, nail/fret contacts, gestures, and tuning. |
| `docs/geometry/`, `docs/body/`, `docs/acoustics/` | Geometry, elastic structure, bridge coupling, cavity, radiation, and observation. |
| `docs/realtime/`, `docs/plugin/`, `docs/ux/` | Bounded processing, host contracts, and musician/engineering interfaces. |
| `docs/validation/` | Verification, acoustic validation, perceptual evaluation, and release evidence. |
| `docs/governance/`, `docs/decisions/` | Scope, acceptance policy, risks, decision records, and stewardship. |
| `include/`, `src/` | Public numerical interfaces and implementation; `include/setar/instrument` and `src/instrument` hold the playable prototype engine. |
| `apps/`, `tests/`, `tools/` | Executable experiments, independent tests, reports, and audits. |
| `data/`, `experiments/` | Data contracts, synthetic demonstration scores (`data/scores`) and reproducible experiment definitions, not unlicensed recordings. |
| `plugin/` | Optional JUCE adapter for the prototype (`SETAR_ENABLE_JUCE_INSTRUMENT`), not part of the headless matrix. |
| `prompts/` | Scoped instructions for human or AI-assisted implementation, review, and investigation. |
| `.github/` | Cross-platform CI and contribution templates. |

## Evidence and visuals

Numerical reports must display their input manifest, model identity, units, source
commit, and limitations. Graphs of synthetic data must say so. Screenshots are
captures of actual rendered interfaces/reports; design mockups are labeled as
mockups. CI artifacts and review evidence are described in the engineering and
validation volumes. No promotional image is evidence of acoustic fidelity.

[View the reproducible diagnostic graphs and visual-capture instructions](docs/assets/README.md).
These visualize a **synthetic string reference**, not a validated setar model.

![Actual synthetic modal-string displacement graph, millimeters against milliseconds](docs/assets/waveform.png)

The [finite-mode energy graph](docs/assets/energy.svg),
[stiffness-dispersion graph](docs/assets/dispersion.svg), and
[proposed musician-interface wireframe](docs/ux/assets/musician-wireframe.svg)
are also included. The wireframe is a design proposal, **not** an implemented
plugin editor or a screenshot from a running VST.

CI additionally generates three actual synthetic numerical solver error
graphs and a 60-row explicit-versus-implicit CSV in the `solver/report`
artifact. The explicit finite-difference scheme rejects configurations
outside its calculated CFL domain. These are real executable experiments,
**not measured setar audio, live UI captures or finished VST3 host tests**.

## Contributing

Read [CONTRIBUTING.md](CONTRIBUTING.md) and [AGENTS.md](AGENTS.md). Contributions
must include tests, relevant uncertainty/assumption updates, and appropriate
cross-platform evidence. Research data and third-party dependencies carry their
own permission and provenance obligations. No open-source redistribution license
is selected by this initial setup; see [rights and dependencies](docs/governance/rights-and-dependencies.md).
