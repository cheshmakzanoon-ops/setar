# Documentation and roadmap expansion — 2026-11-10

## Scope and baseline

This record tracks a documentation-only expansion using 24 distinct delegated
agents in eight waves of three, integrated by one coordinator. The requested
outcome is clearer instructions for every phase: what to do, where and how to
research, what to retain from findings, how they change the physical instrument
model, and what evidence accepts or rejects the result.

The inspected baseline is `b4ae94e8cb1e77710a22d85eb707977d65f6c5a4`.
Its existing audit reported 215 Markdown files and 252,788 prose words, with
20 phases, 119 original scientific tasks, 41 requirements, 44 engineering
work orders, and 79 planned acceptance identifiers. The expansion preserves
those identities. The [baseline audit](../engineering/2026-11-10-baseline-audit.md)
records current source, partial executable evidence, missing physical evidence,
and stale statements identified during inspection.

Documentation completeness and scientific completeness are separate. No physical
specimen is acquired by this work, no measured parameter is created, no DSP
mechanism is implemented, and no scientific phase is promoted by the editing.

## Delegation register

All 24 delegated scopes below are complete. The table identifies their ownership;
the integrated results and final review records follow it. File ownership was
non-overlapping within each authoring wave.

| Wave | Agent | Scope |
| --- | --- | --- |
| 1 | `w1_a_baseline` | Current implementation, tests, evidence gaps, audit constraints |
| 1 | `w1_b_sources` | Verified primary-source atlas and targeted research routes |
| 1 | `w1_c_research_workflow` | Findings-to-model workflow and reusable evidence templates |
| 2 | `w2_a_charter` | P00 scope, claims, responsibilities, acceptance budgets |
| 2 | `w2_b_evidence` | P01 source audit, specimen strategy, data acquisition and partitions |
| 2 | `w2_c_laboratory` | P02 reproducible laboratory, independent oracles and artifacts |
| 3 | `w3_a_strings` | P03–P04 string equations, numerics, dispersion, loss, nonlinear scope |
| 3 | `w3_b_contacts` | P05–P06 nail, fret, finger, release and moving constraints |
| 3 | `w3_c_body` | P07–P08 geometry, material fields, FEM/FRFs and reduction |
| 4 | `w4_a_coupling_acoustics` | P09–P10 bridge coupling, cavity, radiation and observation |
| 4 | `w4_b_performance_inference` | P11–P12 gestures, tuning, identification and calibration |
| 4 | `w4_c_runtime_plugin` | P13–P14 realtime approximation and host integration |
| 5 | `w5_a_interfaces_qualification` | P15–P16 interfaces and platform/host qualification |
| 5 | `w5_b_fidelity` | P17 independent acoustic, listening and expressive evaluation |
| 5 | `w5_c_release_maintenance` | P18–P19 packaging, rights, release and evidence maintenance |
| 6 | `w6_a_measurement_handoffs` | Practical acquisition-to-model field handoffs |
| 6 | `w6_b_model_examples` | Worked physical-model decisions from concrete findings |
| 6 | `w6_c_engineering_navigation` | Engineering work-order navigation and current/proposed boundaries |
| 7 | `w7_a_review_foundations` | Independent review of foundational phases and string/contact recipes |
| 7 | `w7_b_review_physics` | Independent review of body/acoustic/inference handoffs |
| 7 | `w7_c_review_product` | Independent review of realtime, plugin, qualification and release instructions |
| 8 | `w8_a_integrity_audit` | Final task coverage, links, registries and unchanged-status audit |
| 8 | `w8_b_research_review` | Independent mathematical/source and finding-promotion review |
| 8 | `w8_c_contributor_walkthrough` | Follow representative tasks as a new contributor and test handoff clarity |

## Integrated results

Wave 1 completed the dated baseline audit, source atlas, research-to-model
workflow, and finding/promotion templates. The coordinator added the roadmap
execution guide, navigation links, and narrowly scoped corrections to stale
realtime status statements. Later wave results and final verification are
recorded here only after their work is complete.

Wave 2 expanded P00–P02 with 17 task-specific execution recipes (5 + 6 + 6),
preserving all original task headings. Its checks covered local links, exact
requirement/work-order IDs, documented CTest names, and embedded laboratory
command syntax. The recipes now connect bounded claims, source admission,
specimen/acquisition routes, quarantines, held-out partitions, build evidence,
independent mechanics, and reproducible artifacts to their concrete handoffs.

Wave 3 expanded P03–P08 with 36 recipes, bringing coverage to 53 original tasks.
The string recipes distinguish continuum, grid, modal-reference and instrument
domains, including loss identifiability and different overdamped-mode handling.
Contact recipes retain fixed state-transition behavior and specify the missing
finite-patch, friction, release and retuning evidence. Geometry/body recipes
preserve hidden-surface uncertainty, material quarantine, force/velocity ports,
normalization, preload/support effects and the limits of a measured FRF route.
All three authors checked unchanged headings, per-task recipe counts, local links,
existing IDs and whitespace. A worker interruption was resumed using the same
three agent identities and existing drafts.

During Wave 4 source inspection, two reviewers identified a stale coverage
description for `body.mobility_passivity`. The coordinator checked
`tests/instrument/body_tests.cpp` and corrected only that entry's
`verified_subset` and `remaining_gap`: the existing test evaluates the full
synthetic 8×8 Hermitian mobility matrix on 2,001 frequencies from 10 Hz to 20 kHz,
in addition to diagonal checks. The entry remains `executable_partial`; no test,
mapping, requirement status, measured claim, or continuous-frequency guarantee
was added. The executable-coverage audit still reports 27 partial mappings and
zero full accepted requirements.

Wave 4 expanded P09–P14 with 36 recipes, bringing coverage to 89 original tasks.
The coupling/acoustics chapters distinguish signed work-conjugate ports,
connector storage, actual simultaneous updates, passive loading and one-way
pressure observations. They specify independent coupling and acoustic benchmarks,
calibrated complex transfers, loss ownership, spatial holdouts and reduction.
Performance/calibration chapters connect persistent physical strings and gesture
semantics to identifiable parameters, nuisance models, frozen objectives and
independent data groups. Realtime/plugin chapters separate reference parity,
bounded callback work, controlled deadline measurements and source-level wrapper
contracts from the still-unperformed host qualification. Each author checked
the original headings, one recipe per task, local links, exact IDs and whitespace.

Wave 5 expanded P15–P19 with the remaining 30 recipes, completing all 119 tasks
across all 20 phases. Interface recipes map musical intent to actual event and
physical control semantics, diagnostic quantities, observation transforms and
accessibility evidence. Qualification recipes identify exact builds, supported
matrices, current manifest limits, fault/concurrency cases and independent review.
Fidelity recipes separate mechanism metrics, meaningful ablations, playback and
playable studies, grouped holdouts and effect/uncertainty-based claim decisions.
Release/maintenance recipes connect complete model identity, dependency and asset
rights, reconstruction, migration, issue triage, data withdrawal and regression
to explicit future deliverables. All authors checked preservation, task counts,
links, exact IDs and whitespace. The coordinator independently confirmed all 119
original H2 task headings unchanged and exactly one recipe inside every task.

During Wave 6 integration, the navigation author preserved every non-`read`
work-order value and flagged the stale `ENG-P13-T02` evidence note. After checking
the existing benchmark and realtime evidence chapter, the coordinator separately
corrected that one descriptive `implementation_evidence.note`: provisional
container deadline profiles exist and report dense-pattern misses; general asset
publication, coherent telemetry handoff and controlled hardware/host deadline
qualification remain open. The work order stays `in_progress`; its evidence
paths, CTest names, dependencies and acceptance scope are unchanged.

Wave 6 completed the practical handoffs. The acquisition chapter now maps all
17 channels to calibrated procedures, identifiable quantities, current model
fields or explicit extensions, promotion decisions and downstream work. Four
worked examples carry decay, body mobility, observation/loading and nail-contact
findings through calculations, rival explanations, evidence limits and dispositions.
Their synthetic arithmetic was independently recomputed without running the DSP.
The implementation specification now maps the actual instrument pipeline and the
proposed package layout separately. Forty-one work-order reading lists gained
their missing phase chapter; three already contained it, giving 44/44 coverage.
The author verified every other JSON value unchanged before the separate
coordinator correction described above. A drafting worker was interrupted and
resumed under the same identity to complete its bounded deliverable; no extra
agent was added. Local links, IDs, unchanged template gates and whitespace were
checked; the acquisition author also ran the 16 existing evidence-gate checks.

Wave 7 source comparison identified another stale descriptive note,
`ENG-P07-T02.implementation_evidence.note`. The coordinator checked the existing
manual published-parameter audit and the synthetic plane-stress material tests.
The corrected note acknowledges the manual reciprocal-pair audit while retaining
the absence of specimen tensors, rotated-axis validation and a complete automated
published-tensor ingestion/audit. Status, evidence paths and CTest mappings are
unchanged; this is a description correction, not a new material validation result.

Wave 7 completed three independent reviews across all 119 recipes and their
shared handoffs. Corrections include midpoint energy time levels, isolated-span
versus nut-to-tail comparisons, precise critical/overdamped handling, an actual
contact-handler symbol, assumptions of the quasi-static release oracle, modal
numerator versus pole-residue terminology, the gesture enum owner, valid
observation indices, the existing wrapper boundary and its drop-incoming queue
policy. Reviewers checked scoped links/anchors, IDs, current versus planned tests,
synthetic arithmetic and preservation of all original tasks and statuses.

Two reported issues lay outside reviewer file ownership and were corrected by
the coordinator. The damped-midpoint technical chapter had generalized a
light-damping approximation into a universal under-damping claim. Its exact
Cayley modulus, quadrant-safe phase, small-step expansion and a synthetic
near-critical counterexample now delimit that claim; historical experimental
tables were not changed or rerun. The prototype adapter chapter now qualifies
transitive callback work and allocation evidence, identifies the actual overflow
policy and telemetry limits, and distinguishes already-fixed engine defects from
open host qualification. Source and runtime behavior remain unchanged.

Wave 8 completed the final independent integrity audit, mathematical/source
review and contributor walkthroughs. The reviewers confirmed stable task/status
identity and permitted metadata changes, recomputed the five synthetic worked
examples and the midpoint counterexample, and followed P05, P08 and P12 from a
question through sources, operations, findings, current fields, checks and
handoffs. Their reported corrections were integrated: the exact-critical
zero-radius oracle exception; uncertainty-dependent interpretation of weak but
nonzero loss-law separation; and the current contact exponent admission range.
The coordinator also repeated the nail-position bound beside the event mapping.
No blocking issue remained in the inspected scopes.

The complete final review records are:

- [Integrity, traceability, links and unchanged status](2026-11-10-integrity-review.md).
- [Independent mathematics, source access and promotion review](2026-11-10-research-review.md).
- [Contributor walkthroughs for nail, body and identification work](2026-11-10-contributor-walkthrough.md).

## Content coverage

The existing prose auditor excludes fenced source. Counts describe the size of
the instructions; they are not a scientific acceptance criterion. Every added
recipe is attached to its original task rather than a disconnected appendix.

| Measure | Baseline | Integrated result |
| --- | ---: | ---: |
| Markdown files | 215 | 226 |
| Repository prose words | 252788 | 341017 |
| Prose words in the 20 phase chapters | 19948 | 76176 |
| Original scientific tasks | 119 | 119 |
| Execution recipes inside original tasks | 0 | 119 |
| Work orders whose read list includes the owning phase | 3 | 44 |

## Verification boundary

The coordinator reran the five existing audits/validators after integrating all
three final reviews and their corrections. All five exited successfully. A
separate baseline comparison reconfirmed the exact 119 headings and per-task
recipes, 44/44 phase-reading coverage, the permitted JSON changes, byte-identical
requirements/phase/crosswalk and acquisition/product templates, and unchanged
runtime, tests, tools, build definitions and workflows. The final change set
contains 49 files: 47 Markdown documents and two narrowly edited JSON registers.

| Final structural check | Observed result |
| --- | --- |
| Documentation audit | Zero local-link/phase errors; zero repeated long paragraphs flagged. |
| Implementation audit | Zero errors; 41 requirements, 44 work orders, 79 planned tests and 119 mapped scientific tasks. |
| Executable coverage audit | `passed_no_overclaims`: 27 partial mappings, 52 unmapped planned IDs, zero fully accepted requirements. |
| Acquisition template validator | Schema valid; 17 channels, zero acquired and zero review-ready; review gate remains false. |
| Product template validator | Schema valid; 13 gates, zero passed and 13 not executed; declared-matrix qualification remains false. |
| Engineering status preservation | 30 specified, nine in progress and five implemented but unverified; no status promotion. |

The existing documentation, implementation-traceability, and executable-coverage
auditors are used for their actual structural assertions. Review additionally
checks every original task's execution recipe and the distinction between
existing checks, proposed tests, synthetic fixtures, and physical measurements.
These checks cannot certify setar fidelity, host compatibility, or a numerical
method that has not been independently implemented and exercised.
