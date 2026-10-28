# Setar — physical-modeling engineering laboratory

A research-first, C++20 project for a physically modeled Iranian **setar** (سه‌تار).
The intended instrument generates sound from physical state and interactions, not
prerecorded notes. JUCE belongs at application/plugin boundaries; it is not the
physics engine.

**Current status:** an engineering handbook and executable numerical bootstrap.
This is **not a finished setar synthesizer**, not a measured digital twin, and not
a claim of parity with any commercial instrument. The first executable fixtures
are explicitly synthetic. They establish a reproducible place to verify equations,
inspect outputs, and progressively replace assumptions with evidence.

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
C++ interfaces are not implemented features.** All 41 new engineering
requirements begin at specification-only status.

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

## Build the numerical bootstrap

The intended baseline is CMake, a C++20 compiler, and Python 3 for report and
documentation tooling. See [engineering setup](docs/engineering/README.md) for
the exact current build commands and toolchain requirements.

```sh
cmake --preset headless-debug
cmake --build --preset headless-debug
ctest --preset headless-debug
python3 tools/audit_docs.py
python3 tools/audit_implementation.py
```

The headless path must not download JUCE or require an audio device. The optional
JUCE path requires an explicitly supplied checkout and a reviewed license choice.
Do not mistake a passing bootstrap test for measured instrument validation.

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
| `include/`, `src/` | Public numerical interfaces and implementation. |
| `apps/`, `tests/`, `tools/` | Executable experiments, independent tests, reports, and audits. |
| `data/`, `experiments/` | Data contracts and reproducible experiment definitions, not unlicensed recordings. |
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

## Contributing

Read [CONTRIBUTING.md](CONTRIBUTING.md) and [AGENTS.md](AGENTS.md). Contributions
must include tests, relevant uncertainty/assumption updates, and appropriate
cross-platform evidence. Research data and third-party dependencies carry their
own permission and provenance obligations. No open-source redistribution license
is selected by this initial setup; see [rights and dependencies](docs/governance/rights-and-dependencies.md).
