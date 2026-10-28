# Independent review record

## Scope and independence

- Review identifier and target revision/patch:
- Claimed behavior and permitted domain:
- Reviewer role and prior involvement:
- Files, interfaces, experiments, and artifacts inspected:
- Areas deliberately not reviewed:

State whether this is code review, mathematical verification, measurement
review, acoustic validation, participant review, or a combination. Expertise in
one area does not automatically certify every other area. A reviewer who helped
tune the model can provide valuable analysis without being an independent
held-out evaluator.

## Reproduction

- Environment, compiler, build configuration, and hardware:
- Exact commands actually run:
- Complete baseline versus selected tests:
- Observed output and failure diagnostics:
- Artifacts independently regenerated:
- Checks not run and their blockers:

The implemented baseline uses the `headless-debug` configure/build/test presets
and `python3 tools/audit_docs.py`. Additional release or sanitizer runs are
separate evidence. A CI definition is not a successful job; record actual run
identifiers when using remote evidence.

## Correctness questions

Check units, signs, frames, boundary conditions, initial conditions, state
ownership, and failure behavior. Verify that the independent reference does not
call the production implementation to create its expected result. Inspect
convergence, conservation, passivity, causality, and conditioning where relevant.
Review tolerance changes against the intended defect they must still detect.

For real-time paths, check allocation, locks, I/O, bounded work, event ordering,
and lifecycle transitions. Distinguish source-level inspection from measured
runtime evidence. For plugin changes, verify the host boundary does not introduce
a second undocumented sound engine or reinterpret physical controls silently.

## Evidence and rights questions

Trace decisive parameters to sources or measurements. Check source access status,
published-parameter quarantine, calibration, uncertainty, exclusions, and data
partitions. Ensure synthetic outputs remain labeled and that recordings are not
used as note-playback synthesis assets. Inspect whether public artifacts have
affirmative distribution permission without exposing private agreements.

Review claim wording separately from implementation. A correct oscillator may
still be described too broadly. A pleasant demonstration may still lack
independent validation. Identify exactly which statements the available evidence
supports and which must be narrowed.

## Findings

For each finding, record location, observed evidence, consequence, severity for
the stated claim, and the smallest useful corrective action. Distinguish a
blocking correctness or permission problem from a nonblocking style preference.
Do not report speculative defects as observed failures; propose a test when the
evidence is insufficient.

Include positive evidence that materially supports acceptance, such as an
independent analytic comparison or a demonstrated rejection path. Avoid empty
approval language that gives the next maintainer no basis for trust.

## Disposition

- Accept for the stated scope, request changes, reject approach, or inconclusive:
- Blocking findings:
- Nonblocking follow-up work:
- Claims explicitly not approved:
- Retest required after correction:
- Next reviewer or evidence gate:

Acceptance applies to the reviewed revision and domain. A material change after
review requires appropriate reinspection. Preserve unresolved disagreements and
their evidence rather than converting them into an unexplained approval.
