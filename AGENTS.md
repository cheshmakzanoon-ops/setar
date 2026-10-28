# Contributor instructions

This repository is a research-first C++ physical-modeling project for the Iranian
setar. Read `README.md`, `docs/START_HERE.md`, and the relevant phase before editing.

## Non-negotiable rules

- Never claim a completed instrument, validated setar fidelity, or commercial parity
  from a generic string demo or a successful build. Separate implemented behavior,
  proposed architecture, synthetic fixtures, and experimental evidence.
- Sound generation must be physical-model based. Measured parameters are permitted;
  recorded note playback is not the synthesis engine. Measurement recordings remain
  reference/validation data subject to explicit permissions and provenance.
- Main engine language is modern C++ (C++20 baseline). Keep the core independent of
  JUCE; use JUCE at plugin/application boundaries when justified.
- Use SI units at physical-model boundaries. Document sign conventions, assumptions,
  coordinate systems, discretization, energy, stability, and validity limits.
- All new numerical behavior needs independent reference tests, convergence evidence,
  and explicit failure handling. Never silently clip or normalize unstable solutions.
- Do not put allocations, locks, file/network I/O, or unbounded work in audio callbacks.
- Work in ordered phases with evidence-based gates, not calendar deadlines.
- Use original primary sources and exact DOI/URL/section references. Do not copy
  copyrighted papers into this repository. Audit suspicious published parameters.
- Do not add invented measurements, fake screenshots, fabricated benchmarks, or
  repeated prose to inflate documentation word counts.
- Update cross-platform tests and documentation with each change. A configured
  workflow is not the same thing as a successful Linux/Windows/macOS run.
- Link new scientific work to stable `REQ-AAA-NNN` requirements and `ENG-Pxx-Tyy`
  work orders. Keep `roadmap/requirements.json` and
  `roadmap/engineering-tasks.json` and the 119-task
  `roadmap/phase-task-crosswalk.json` consistent and run
  `python3 tools/audit_implementation.py`. Planned tests are not present CTests.
- Preserve unrelated changes. No force pushes, history rewriting, or automatic
  changes to repository protection, secrets, billing, or publishing settings.

## Parallel work

Parallel research, implementation, and review are encouraged when the coordinator
assigns non-overlapping files. Workers must not commit or push independently.
Record uncertain claims and hand interface changes back to the coordinator.

## Documentation task contract

Every substantial task should state prerequisites, sources, inputs/outputs, exact
procedure, tests, acceptance evidence, failure modes, limitations, and next actions.
Detailed instructions should be specific to the task rather than repeated filler.
