# Baseline audit for the November 2026 roadmap expansion

**Audit identity:** documentation and source inspection of commit
`b4ae94e8cb1e77710a22d85eb707977d65f6c5a4`, performed 2026-11-10 before the
roadmap expansion. This is a historical baseline, not a rolling capability
register or a new phase-completion decision. The checks below ran locally; no
C++ build, acoustic experiment, practitioner session, plugin host or listening
study was performed by this audit.

Use the [current prototype chapter](11-playable-prototype.md),
[master roadmap](../ROADMAP.md), [requirements](../../roadmap/requirements.json),
and [executable coverage registry](../../roadmap/executable-test-coverage.json)
for subsequent work. A later change must identify its own source revision and
evidence rather than inheriting the results in this snapshot.

## What was actually present

The starting repository already contained 215 Markdown documents and 252,788
prose words according to `tools/audit_docs.py`. The important missing layer was
not a larger word count: contributors needed a shorter route from an unresolved
physical question to an exact search or measurement, a recorded finding, a
specific model change, and an independent acceptance result.

| Baseline object | Observed state | Consequence for the expansion |
| --- | --- | --- |
| Scientific phases | 20, P00–P19 | Preserve phase identity and evidence dependencies. |
| Original task headings | 119: five in P00, six in every other phase | Expand the original tasks in place; preserve their stable IDs. |
| Engineering requirements | 41 | Each implementation proposal must identify the applicable `REQ-AAA-NNN`. |
| Engineering work orders | 44 | Use existing `ENG-Pxx-Tyy` routes; these are not the same namespace as phase task IDs. |
| Planned acceptance identifiers | 79 | Do not describe all 79 as runnable CTests. |
| Explicit partial executable mappings | 27 | Explain both the checked subset and the remaining gap. |
| Planned identifiers without a partial mapping | 52 | A test name or proposed path is a future deliverable, not existing coverage. |
| Fully accepted requirements | 0 in the coverage audit | More detailed documentation must not promote scientific status. |
| Acquisition template | 17 critical channels, zero acquired or review-ready | No placeholder may become a measured parameter merely by completing a form. |
| Product qualification template | 13 required gates, all not executed | Headless source tests do not qualify a plugin product. |

The engineering work-order statuses were 30 `specified_not_implemented`, nine
`in_progress`, and five `implemented_unverified`. A phase marked `specified`
may contain a partial synthetic prototype: its phase gate and its code presence
answer different questions. Preserve that distinction when improving a phase's
opening status paragraph.

## Source-to-model map

These are existing source locations at the audited commit. The last column
names the next evidence boundary rather than promising an implementation.

| Physical or software responsibility | Existing code and test entry points | Boundary still open |
| --- | --- | --- |
| Linear analytic reference | `include/setar/reference.hpp`, `src/reference.cpp`, `tests/reference_tests.cpp`; `core.frequencies`, `core.analytic_response`, `core.lossless_energy`, `core.passivity` | A verified synthetic modal fixture is not measured string behaviour. |
| Independent spatial numerical experiments | `include/setar/numerical_prototype.hpp`, `src/numerical_prototype.cpp`, `tests/numerical_prototype_tests.cpp`; `numerics.string_spatial_convergence`, `numerics.explicit_cfl_rejection`, `numerics.discrete_energy_ledger` | Current uniform fixed-support problems do not establish moving supports or nonlinear contact stability. |
| Instrument parameter boundary | `include/setar/instrument/config.hpp`, `src/instrument/config.cpp`; `StringConfig`, `BodyMode`, `ContactLaw`, `ProcessSpec`; `engine.config_validation` | Numerical admissibility cannot certify measurement provenance or parameter identifiability. |
| Modal propagation and contact solve | `include/setar/instrument/numerics.hpp`, `src/instrument/numerics.cpp`; `ModalBank`, `SmallNewtonSolver`; `solver.modal_exact_free_response`, `solver.discrete_gradient_identity`, `solver.newton_coupled_oracle` | Independent high-resolution contact comparison, complete parameter-domain evidence and missing physical mechanisms remain separate tasks. |
| Persistent instrument mechanics | `include/setar/instrument/instrument.hpp`, `src/instrument/instrument.cpp`; `Instrument::prepare`, `Instrument::process`, `EnergyLedger`; `engine.lossless_energy_ledger`, `performance.persistent_state` | The synthetic engine does not establish specimen fidelity or real-time deadline qualification. |
| Nail, fret and finger interactions | `src/instrument/instrument.cpp`, `tests/instrument/engine_tests.cpp`; `contact.nail_passivity`, `contact.release_convergence`, `contact.fret_crossing_work`, `contact.release_afterlength`, `controls.sweep_continuity` | Measured contact geometry, compliance and friction; pressing-induced tension changes; practitioner-approved gestures. |
| Body and bridge mobility | `include/setar/instrument/body_model.hpp`, `src/instrument/body_model.cpp`; `build_body`, `bridge_mobility`; `materials.reciprocity`, `body.plate_frequency_oracle`, `body.mobility_passivity` | The rectangular plate, simplified cavity and lateral mode are synthetic; actual bowl/soundboard geometry, measured FRFs and modal residues are absent. |
| String specifications and tuning | `include/setar/instrument/setar_preset.hpp`, `src/instrument/setar_preset.cpp`; `tension_for_frequency`, `fret_positions`, `synthetic_setar_config`; `preset.tension_inverse`, `preset.fret_positions` | Synthetic gauges and historical-theory fret maps are not specimen measurements or practitioner approval. |
| Performance translation | `include/setar/instrument/performance.hpp`, `src/instrument/performance.cpp`; `parse_score`, `schedule`, `choose_fret`, `MidiGestureMapper`; `score.course_sweep`, `score.midi_mapper` | Correct parser/scheduler behaviour does not establish authenticity of the gesture vocabulary. |
| Real-time-shaped execution | `tests/instrument/realtime_tests.cpp`, `apps/setar_benchmark.cpp`; `realtime.allocation_hook`, `realtime.block_partition`, `realtime.max_work`, `realtime.capacity_overflow` | Allocation hooks have a stated scope; bounded iterations do not establish that block wall time meets a deadline. |
| Offline listening and diagnostics | `apps/setar_play.cpp`, `tools/analyze_render.py`, `data/scores/`; `integration.setar_play` | Audio renders are synthetic evidence about this engine; their pleasantness cannot validate the instrument. |
| Optional plugin adapter | `plugin/CMakeLists.txt` and the files under `plugin/`; option `SETAR_ENABLE_JUCE_INSTRUMENT` | Existing documentation records configuration/syntax checking, without a completed build or validator/host qualification. |

Every added phase recipe should point to one of these existing locations where
appropriate, then explicitly distinguish proposed production modules from the
current prototype. For example, the work order's planned
`src/contact/nail.cpp` is not where the existing nail solver lives. Sending a
contributor there without explaining the distinction encourages duplicate or
disconnected implementations.

### Existing contracts that a research finding must respect

The current `config.hpp` defines string coordinate `x` from nut to tail anchor;
`bridge_position_m` is the speaking length and may differ from total length.
Physical string indices and playing courses are different identities. The two
polarization directions are documented explicitly. A measurement worksheet must
use those conventions or record the transformation into them.

`StringLossLaw` supplies nonnegative constant, wavenumber-squared and
frequency-squared coefficients for modal amplitude decay. Fitting total acoustic
decay to all three while also fitting bridge leakage would confound mechanisms.
The phase instructions must state which observed decay belongs to the string,
which belongs to body loading, what was held fixed, and which combinations are
actually identifiable.

`BodyMode` requires frequency, damping ratio, modal mass, bridge participation
for each physical string/polarization, and observation weights. A list of
published resonance frequencies cannot fill that structure. A replacement asset
needs a consistent normalization plus evidence for the residues and observation
mapping. Reusing arbitrary modal mass while copying frequencies would change
coupling strength without any supporting observation.

The prototype's `Provenance` field is metadata: the engine does not validate a
measurement because that enum says `Measured`. Parameter promotion needs an
external evidence record with specimen/setup/session, raw-data identity,
uncertainty, transformation, admissibility review and validation scope.

## Stable phase and work-order identities

The following ranges are inclusive. Exact many-to-many routes remain in the
[phase-task crosswalk](../../roadmap/phase-task-crosswalk.json); the table is an
editorial allocation guide, not a substitute for that crosswalk.

| Phase | Original task IDs | Owning engineering work orders | Requirement families |
| --- | --- | --- | --- |
| P00 | P00-T01 through P00-T05 | ENG-P00-T01 through T02 | REQ-GOV-001 |
| P01 | P01-T01 through P01-T06 | ENG-P01-T01 through T02 | REQ-GOV-002 |
| P02 | P02-T01 through P02-T06 | ENG-P02-T01 through T02 | REQ-LAB-001, REQ-LAB-002 |
| P03 | P03-T01 through P03-T06 | ENG-P03-T01 through T03 | REQ-STR-001, REQ-STR-002 |
| P04 | P04-T01 through P04-T06 | ENG-P04-T01 through T02 | REQ-STR-003, REQ-STR-004 |
| P05 | P05-T01 through P05-T06 | ENG-P05-T01 through T03 | REQ-EXC-001, REQ-EXC-002 |
| P06 | P06-T01 through P06-T06 | ENG-P06-T01 through T02 | REQ-FRT-001, REQ-FRT-002 |
| P07 | P07-T01 through P07-T06 | ENG-P07-T01 through T02 | REQ-GEO-001, REQ-GEO-002 |
| P08 | P08-T01 through P08-T06 | ENG-P08-T01 through T03 | REQ-BOD-001 through REQ-BOD-003 |
| P09 | P09-T01 through P09-T06 | ENG-P09-T01 through T02 | REQ-CPL-001, REQ-CPL-002 |
| P10 | P10-T01 through P10-T06 | ENG-P10-T01 through T02 | REQ-ACO-001, REQ-ACO-002 |
| P11 | P11-T01 through P11-T06 | ENG-P11-T01 through T02 | REQ-PRF-001, REQ-PRF-002 |
| P12 | P12-T01 through P12-T06 | ENG-P12-T01 through T02 | REQ-IDN-001, REQ-IDN-002 |
| P13 | P13-T01 through P13-T06 | ENG-P13-T01 through T02 | REQ-RTS-001 through REQ-RTS-003 |
| P14 | P14-T01 through P14-T06 | ENG-P14-T01 through T03 | REQ-PLG-001 through REQ-PLG-003 |
| P15 | P15-T01 through P15-T06 | ENG-P15-T01 through T02 | REQ-UXI-001, REQ-UXI-002 |
| P16 | P16-T01 through P16-T06 | ENG-P16-T01 through T02 | REQ-QLF-001, REQ-QLF-002 |
| P17 | P17-T01 through P17-T06 | ENG-P17-T01 through T02 | REQ-FID-001, REQ-FID-002 |
| P18 | P18-T01 through P18-T06 | ENG-P18-T01 through T02 | REQ-REL-001, REQ-REL-002 |
| P19 | P19-T01 through P19-T06 | ENG-P19-T01 through T02 | REQ-MNT-001, REQ-MNT-002 |

## Reproduction commands and observed results

These commands were actually run for this documentation audit, at the baseline
before new files were added:

```sh
python3 tools/audit_docs.py
python3 tools/audit_implementation.py
python3 tools/audit_executable_coverage.py
python3 tools/validate_measurement_campaign.py data/measurement-campaign-template.json
python3 tools/validate_product_qualification.py data/product-qualification-template.json
```

All five exited successfully. The documentation audit reported zero broken local
links/phase errors and zero repeated long paragraphs. The engineering auditor
reported zero register errors. The coverage audit reported
`passed_no_overclaims`. The acquisition and product validators accepted their
**blank schemas**, while reporting `review_gate_satisfied: false` and
`qualified_for_declared_matrix: false`, respectively. Successful validation of an
empty template is the expected rejection of a premature scientific claim.

The source-supported headless reproduction sequence is:

```sh
cmake --preset headless-release
cmake --build --preset headless-release
ctest --preset headless-release --output-on-failure
```

This audit inspected those commands and their definitions; it did not execute
them. `CMakePresets.json` uses Ninja, requires CMake 3.25 or newer, and the report
tests require Python 3.10 or newer. The headless path has no JUCE download.
Windows users without Ninja should use the generator-independent procedure in
[engineering setup](README.md), with `--config Debug` and `-C Debug` for a
multi-configuration generator.

The existing workflow configures Ubuntu 24.04, Windows 2022, macOS 15 and a
separate Linux ASan/UBSan job. The baseline README links
[run 41177416020](https://github.com/cheshmakzanoon-ops/setar/actions/runs/41177416020)
for prototype commit `1db2dcf`. During this audit the coordinator additionally
queried the GitHub connector and received successful conclusions for
[run 41181544708](https://github.com/cheshmakzanoon-ops/setar/actions/runs/41181544708)
and [run 41181542795](https://github.com/cheshmakzanoon-ops/setar/actions/runs/41181542795)
at the audited full `b4ae94e` revision. These are retrieved workflow conclusions,
not new local execution or evidence for a later documentation commit. Any final
documentation change must record its own validation result. The old container
runtime measurements remain historical even though later CI passed.

## Auditor limitations the editorial review must cover

`tools/audit_docs.py` checks Markdown file links, phase document existence and
dependency cycles. It skips external URL verification and does not verify that
a `#fragment` points to an existing heading. It strips fenced blocks before
counting prose or examining links. The duplicate-paragraph signal is advisory;
passing it cannot establish original research or scientific correctness.

`tools/audit_implementation.py` discovers original task headings with
`^##\s+(P\d{2}-T\d{2})\b`. Those IDs must remain unique and must agree with
`phase-task-crosswalk.json`. Add procedural subsections at H3 or below within an
existing task. Adding, removing or renumbering an H2 task requires an intentional
crosswalk change. Its `read` paths must exist; `touch` paths deliberately allow
future files and package directories. Neither the latter field nor a work order
with status `implemented_unverified` establishes that the proposed path exists.

`tools/audit_executable_coverage.py` checks that partial mappings refer to known
planned IDs, existing runners and recognized CMake registrations, and refuses
dispatch to the literal `not_implemented` stub. It does not prove that a test is
mathematically independent or that its assertions are adequate. Keep
`verified_subset` and `remaining_gap` tied to the actual assertions; correcting a
stale description does not promote its status or extend the evidence. A passing
mapping audit is not execution of the C++ test suite.

The acquisition/product validators check schemas, declared gates and referenced
hashes. An artifact can be internally consistent yet physically wrong. Human
review must still inspect calibration, units, physical plausibility, split
integrity, permissions and the actual measured or host-observed result.

## Baseline inconsistencies to resolve in the expansion

The following were present at the audited commit. They are editorial follow-ups,
not newly discovered engine defects. Later editors should annotate or correct
them while preserving the useful historical evidence.

| Baseline wording | Evidence already present | Required clarification |
| --- | --- | --- |
| `docs/START_HERE.md` says the engine has no measured deadline profile. | `apps/setar_benchmark.cpp` and `docs/realtime/05-prototype-realtime-evidence.md` contain a provisional container benchmark with block percentiles. | Say a provisional benchmark exists and misses the dense-pattern deadlines; controlled hardware/host qualification remains absent. |
| The realtime evidence chapter's final next actions still say to fix float-output containment and decide active-slot re-press semantics. | That chapter records all three original defects as fixed; `instrument.cpp` checks the float cast and `controls.sweep_continuity` exercises finite-speed re-press. | Preserve the failure history and remove those already-completed actions from the current queue. |
| Historical realtime text says Windows/macOS have not been run. | The later README points to the three-platform prototype workflow run; the coordinator also retrieved successful workflow conclusions for the audited `b4ae94e` revision above. | Date/scope the older statement and link the later runs; do not erase failed historical evidence or call the new documentation commit tested by an older run. |
| The future `RealtimePlan` paragraph in the prototype-contract chapter says no callback allocation audit or deadline trace exists. | Narrow allocation hooks and provisional timing exist; full lock/syscall auditing and qualified deadlines do not. | Identify exactly which narrow test exists and which broader contract is still proposed. |
| Many engineering `touch` paths differ from implemented prototype paths. | `implementation_evidence` records and the current `src/instrument/` layout explain the prototype. | Every phase must give the current inspection path and label the proposed destination separately; avoid creating a parallel engine accidentally. |

## Editorial priorities and review gate

For each original task, the expansion should answer an operational question
that the existing overview leaves implicit. State the exact source type and
search terms; the inclusion/exclusion test; what must be extracted; how
conflicting or inaccessible evidence is recorded; the units, uncertainty and
validity domain; and the resulting change to a physical equation, parameter,
boundary condition, observation operator or experimental design. A task that
only asks a contributor to read more papers has not closed that gap.

Physical findings must lead to a reviewable disposition: reject/quarantine,
retain as a candidate assumption, schedule a discriminating measurement, or
promote a traceable parameter/model asset for a stated domain. The handoff must
name the current code entry point, owning requirement and engineering task,
independent oracle, expected artifact and downstream consumer. If no measurement
is available, specify the synthetic verification work that can proceed and the
fidelity claim that remains blocked.

Review the resulting task against a concrete example. A reported body resonance
should lead to a mode-identification/normalization question, not a pasted
frequency list. A measured nail trajectory should lead to a signed contact gap,
force/work convention and release prediction, not an arbitrary attack envelope.
A practitioner correction to a stroke should change the event/trajectory
contract and its review corpus, not merely rename a UI control.

Accept the documentation expansion when all original tasks remain traceable,
recipes identify executable versus proposed locations, sources and assumptions
are auditable, current defects are separated from historical ones, and the
checks above still pass. This editorial gate neither passes a scientific phase
nor establishes acoustic fidelity.
