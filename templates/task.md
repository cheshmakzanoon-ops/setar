# Engineering task record

Copy this template into the appropriate work record and replace prompts with
task-specific content. Do not mark a field complete merely because the template
contains an example. Remove irrelevant sections only with a brief explanation.

## Identity and question

- Task identifier and short title:
- Related phase and exact gate:
- Requested mode: inspect, diagnose, design, implement, measure, or review:
- One falsifiable question or bounded outcome:
- Why this changes an engineering decision:

State the claim at the level the task can establish. For example, validating
transactional rejection of invalid string parameters is narrower than validating
all string physics. Avoid tasks whose only success criterion is “sounds better”
without a defined comparison, participants, and listening conditions.

## Prerequisites and authority

- Required chapters and source identifiers/sections:
- Existing implemented capability relied upon:
- Required data, calibration, specimen, or permission records:
- Authorized interventions or external actions:
- Missing prerequisites and permitted work while blocked:

Confirm the request authorizes the intended change. An analysis task should not
quietly become a public release, participant contact, instrument modification,
or dependency purchase. Unknown measurement values remain unknown; synthetic
fixtures must be named and labeled separately.

## Ownership and interfaces

- Files or directories owned by this task:
- Shared interfaces requiring coordination:
- Inputs, units, signs, coordinate frames, and evidence labels:
- Outputs, failure states, and validity domain:
- State ownership, thread, lifetime, and ordering rules where relevant:

Inspect the current worktree and preserve unrelated changes. Identify what must
remain compatible and what intentionally changes. If a parameter meaning changes,
name the migration or rejection policy; reusing the same identifier with a new
meaning is not a transparent change.

## Procedure

1. Reproduce the relevant baseline and record its actual result.
2. Construct the independent expected result or discriminating experiment.
3. Add the smallest fixture that fails for the intended defect or missing behavior.
4. Implement or acquire only the scoped change.
5. Exercise normal, boundary, invalid, and repeated-state cases.
6. Compare results with the predefined acceptance rule.
7. Update evidence, assumptions, and handoff documentation.

Replace this sequence with exact task operations, commands, and artifact paths.
Do not leave generic steps where a future operator needs a calibration procedure,
solver configuration, or channel convention.

## Tests and acceptance evidence

- Independent reference and why it is independent:
- Exact test command or laboratory procedure:
- Quantity compared, unit, tolerance, and justification:
- Convergence, conservation, passivity, or causality checks as applicable:
- Invalid-input and failure-path checks:
- Full baseline versus selected diagnostic subset:
- Platforms, hosts, and hardware actually exercised:

Existing commands include `ctest --preset headless-debug` after configuration
and build, and `python3 tools/audit_docs.py` for documentation integrity. They
do not replace task-specific scientific evidence. Record omitted checks as
unrun rather than passed.

## Failure and stop rules

- Results that falsify the proposed approach:
- Permission, instrument-safety, data-quality, or numerical stop conditions:
- Safe fallback that preserves the original task scope:
- Evidence to retain from failure:

Do not widen tolerances, alter test partitions, normalize unstable state, or
substitute guessed inputs merely to complete the task. A changed hypothesis
requires an explicit decision record.

## Handoff

- Outcome: accepted for stated scope, rejected, inconclusive, or blocked:
- Changed files and interfaces:
- Commands actually executed and observed results:
- Evidence artifacts and provenance:
- Remaining assumptions and unsupported claims:
- Next action and the gate it serves:

The handoff must be understandable without private chat history. A future
developer should know both how to continue and what not to claim.
