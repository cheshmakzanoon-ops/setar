## Outcome and scope

State the bounded change and the claim it supports. Link the task, phase gate,
and [decision record](../templates/decision.md) when a design choice changed.
Identify behavior that remains proposed or experimental.

## Changes and interfaces

- Owned files and preserved unrelated work:
- Physical units, signs, frames, or parameter meanings changed:
- State ownership, thread, lifecycle, or serialization changes:
- Data, dependency, permission, or migration implications:

## Evidence actually obtained

| Check | Exact command or procedure | Observed result | Scope or blocker |
| --- | --- | --- | --- |
| Headless configure/build | | | |
| Complete applicable CTest suite | | | |
| Documentation audit | | | |
| Task-specific independent reference | | | |
| Additional platforms, hosts, or sanitizers | | | |

Name omitted checks explicitly. A configured workflow, successful build, or
synthetic plot is not measured instrument validation. Link actual CI runs and
rights-cleared artifacts when using them as evidence.

## Failure behavior and limitations

Describe invalid-input handling, numerical or real-time bounds, retained failed
experiments, and unsupported claims. Explain any tolerance change with evidence.
Do not hide failure through normalization, clipping, disabled tests, or revised
independent-test criteria.

## Review readiness

- [ ] I read [AGENTS.md](../AGENTS.md) and [CONTRIBUTING.md](../CONTRIBUTING.md).
- [ ] Sources, synthetic fixtures, and measurements are labeled accurately.
- [ ] Tests use an independent expectation and cover relevant failure paths.
- [ ] No private data, credentials, or unapproved third-party assets are included.
- [ ] Audio-path changes preserve bounded, allocation-free, nonblocking work.
- [ ] Local links and affected evidence/capability records were reviewed.

Explain any inapplicable checkbox rather than claiming an unperformed review.

## Handoff

List remaining blockers, the next smallest useful action, and the evidence gate
it serves. State which claims the reviewer is being asked to approve and which
are deliberately outside this pull request.
