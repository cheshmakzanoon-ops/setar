# Documentation expansion integrity review — 2026-11-10

## Scope and result

**Result: no actionable integrity defect found in the inspected expansion.**
This review compares the working tree with
`b4ae94e8cb1e77710a22d85eb707977d65f6c5a4`, which remained `HEAD` during
inspection. It follows [the expansion record](2026-11-10-documentation-expansion.md)
and [baseline audit](../engineering/2026-11-10-baseline-audit.md). Project content
was read only; this report is the reviewer's sole added file. Final navigation,
integration prose and document totals belong to the coordinator's later pass.

The result concerns documentation structure, traceability and the permitted
metadata changes. No native build, CTest execution, new test/tool, DSP experiment,
measurement, host session, commit or push was performed by this review.

## Preserved identities and task coverage

| Check | Observed result |
| --- | --- |
| Original scientific tasks | All **119 original H2 task-heading lines** match the baseline literally and remain in their original order across **20 phases**. |
| Execution recipes | Exactly one `### Execution recipe` occurs inside every original task: five in P00 and six in each of P01–P19. |
| Phase status | Original status lines are unchanged; the phase register is byte-identical to the baseline. |
| Stable registers | **41 requirements**, **44 engineering work orders**, **79 distinct planned acceptance IDs**, and **119 crosswalk entries** remain. |
| Task-to-work-order route | Every original task section explicitly mentions all engineering IDs assigned to it by the unchanged crosswalk. Every section also contains a known requirement ID. No unknown concrete `REQ-AAA-NNN` or `ENG-Pxx-Tyy` identifier was found in changed/new Markdown. |
| Work-order status | Unchanged: 30 `specified_not_implemented`, nine `in_progress`, five `implemented_unverified`. |
| Reading-list navigation | Exactly 41 work orders gained their missing owning-phase chapter as a new first reading; the previous reading entries and their order remain intact. Three already had that chapter, giving **44/44** coverage. |

The requirements register, phase register, phase-task crosswalk, acquisition
template and product-qualification template were compared directly with the
baseline and are byte-identical. The existing implementation audit additionally
accepts the blank specimen template and complete crosswalk.

## Exact change boundary

The tracked diff and untracked-file inventory contain Markdown plus only the two
permitted JSON files. There are no changes to runtime source, tests, build
configuration, workflow definitions, executable tools or physical data.

For [engineering-tasks.json](../../roadmap/engineering-tasks.json), a parsed
comparison found only the 41 reading-list additions and the following two
`implementation_evidence.note` corrections:

- `ENG-P07-T02` now acknowledges the existing
  [manual published-parameter audit](../research/published-parameter-audit.md).
  The note retains the missing specimen tensors, rotated-axis validation and
  complete automated ingestion/audit. The existing synthetic plane-stress checks
  in [body_tests.cpp](../../tests/instrument/body_tests.cpp) do not supply those
  missing results.
- `ENG-P13-T02` now acknowledges the existing
  [benchmark](../../apps/setar_benchmark.cpp) and
  [provisional container profiles](../realtime/05-prototype-realtime-evidence.md),
  including dense-pattern deadline misses. General asset publication, coherent
  telemetry handoff and controlled hardware/host qualification remain open.

Every other engineering JSON value, including statuses, dependencies,
requirements, evidence paths and CTest names, is unchanged.

For [executable-test-coverage.json](../../roadmap/executable-test-coverage.json),
only `body.mobility_passivity.verified_subset` and `.remaining_gap` changed.
Source inspection confirms that the existing `body_mobility_passivity()` checks
all eight driving-point real parts and the full 8×8 Hermitian part, `Re Y` for
this reciprocal model, at 2,001 logarithmically spaced frequencies from 10 Hz to
20 kHz. Its smallest-eigenvalue tolerance is `-1e-12` times the largest real
driving-point mobility. It also checks synthetic provenance and modal metadata.
The revised description preserves the absence of measured multiport evidence,
arbitrary imported-model coverage and continuous-frequency guarantees. All other
coverage values are unchanged: **27 partial mappings, 52 unmapped planned IDs,
zero requirements declared fully accepted**. The supporting source and manual
audit already existed at the baseline; no executable result was added here.

## Links and current-versus-proposed structure

The repository documentation auditor reports no broken local file links or phase
errors. A separate read-only scan resolved local links and heading fragments in
changed/new Markdown, accounting for fenced examples, heading punctuation and
duplicate heading suffixes. No missing target or heading anchor was found.
The same scan checked unchanged Markdown for baseline-only link/anchor failures;
none was found. External URLs and remote source availability were outside this
integrity review.

Entry statements in all 20 phases distinguish work instructions from acquired
evidence. The [implementation specification](../engineering/implementation-specification.md)
separates current source ownership from proposed packages and explicitly keeps
`Pxx-Tyy` distinct from `ENG-Pxx-Tyy`. The shared
[execution guide](../roadmap-execution-guide.md) labels future handoffs and separates
synthetic work that can proceed from evidence needed to promote a claim.

Concrete handoffs retain the relevant boundaries: contacts/coupling live in
`src/instrument/instrument.cpp`; the current body builder is synthetic, not a
general FEM/import pipeline; the existing adapter is under `plugin/`; and planned
plugin/fidelity acceptance names do not become present CTests. Review of task
sections and their shared entry contracts found no structural promotion from
instructions, proposed paths or synthetic checks to measured or qualified status.
This is not an independent mathematical verification of every recipe.

## Commands actually run and their meaning

All commands below exited with status 0 in the inspected working tree.

| Command | Result and evidential limit |
| --- | --- |
| `python3 tools/audit_docs.py` | Zero local-link/phase errors and zero repeated long paragraphs flagged. It checks file targets, phase documents and dependency structure; it does not validate heading fragments, external sources or scientific correctness. Heading fragments were checked separately above. |
| `python3 tools/audit_implementation.py` | Zero engineering-register errors; 41 requirements, 44 work orders, 79 planned tests and 119 original tasks mapped. It checks registry structure, dependencies, reading paths, crosswalk coverage and the blank specimen contract. Proposed `touch` paths need not exist. |
| `python3 tools/audit_executable_coverage.py` | `passed_no_overclaims`; 27 partial mappings, 52 unmapped planned tests, zero full accepted requirements. It checks known IDs, runners, recognized CMake registrations and dispatch against a literal `not_implemented` stub. It does not execute CTests or establish oracle independence and adequate assertions. |
| `python3 tools/validate_measurement_campaign.py data/measurement-campaign-template.json` | The blank schema passes: 17 channels, zero acquired, zero review-ready, `review_gate_satisfied: false`. No scientific validity is established. |
| `python3 tools/validate_product_qualification.py data/product-qualification-template.json` | The blank schema passes: 13 required gates, zero passed, 13 not executed, `qualified_for_declared_matrix: false`. No host, UI or listening result is established. |
| `git diff --check` | No tracked-diff whitespace errors. This is a formatting check, not a validation of documentation claims. |

The validators can inspect referenced hashes when evidence is supplied, but these
blank templates contain no acquired or executed evidence to substantiate. Their
successful schema checks leave the scientific and product gates open.

## Baseline-only observations and integration handoff

No additional baseline-only structural defect was found. The historical wording
issues and evidence limits catalogued in the baseline audit remain distinct from
new execution results. The two work-order note corrections and mobility coverage
correction describe evidence that was already present; they do not advance status.

The coordinator should rerun the existing documentation and traceability checks
after final navigation/ledger integration. This review deliberately does not fix
the moving Markdown or prose totals. Its stable counts and unchanged-status
conclusions support acceptance of the documentation expansion within the stated
review scope, while every physical, numerical, host and fidelity gate retains its
own required evidence.
