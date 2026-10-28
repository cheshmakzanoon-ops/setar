# Engineering decision record

## Identity and status

- Decision identifier and title:
- Related phase, task, experiment, and interface:
- Status: proposed, accepted for scope, superseded, or withdrawn:
- Decision owners and required reviewers:
- Supersedes or is superseded by:

An accepted decision is not permanent truth. Record the evidence that could
reopen it and preserve the reasoning behind earlier choices. Do not rewrite an
old record to make a later outcome appear inevitable.

## Question and constraints

State the concrete choice. Identify physical invariants, numerical admissibility,
real-time limits, compatibility, rights, and available evidence. Separate hard
requirements from preferences. A passive mechanical interface cannot trade
energy creation for a smaller spectral residual; an optional visual preference
can reasonably trade against implementation effort.

Name what is unknown. Missing geometry, participant coverage, or calibration
does not become known because a decision must be made. A provisional design may
use a synthetic fixture or a restricted domain if that status remains explicit.

## Alternatives

For each serious alternative, record:

- Mechanism or architecture and its assumptions.
- Inputs, outputs, units, ownership, and lifecycle.
- Evidence supporting it and evidence against it.
- Complexity, failure behavior, and observability.
- Numerical and real-time implications.
- Data, permission, and dependency consequences.
- What implementation or experiment would distinguish it from the others.

Include the minimal current approach as an alternative when appropriate. Do not
construct weak alternatives solely to justify a preferred design. Avoid choosing
by the number of equations, parameters, or available software features alone.

## Evidence considered

List original source identifiers and exact sections, executed test records,
experiment identifiers, uncertainty, and platform evidence. Distinguish published
claims from project measurements and implementation verification from acoustic
validation. State whether data used to choose the alternative remain independent
for any later evaluation; usually they are now development evidence.

## Decision and rationale

State the chosen alternative and its permitted scope in plain language. Explain
the decisive tradeoff with concrete evidence. Identify assumptions that were
adopted provisionally, constraints enforced by code, and behaviors intentionally
left unimplemented. Do not turn a research convenience into a claim about all
real setars.

If an approximation or empirical correction is chosen, name it explicitly and
describe how it can be replaced when better evidence arrives. If a parameter is
effective rather than physically identified, keep that distinction in exported
metadata and user-facing claims.

## Consequences and migration

- Interfaces and serialized meanings that change:
- Existing artifacts or tests that become stale:
- Compatibility and migration/rejection behavior:
- New tests and diagnostics required:
- Documentation and permission records affected:
- Reversible path if the decision fails:

Do not hide a parameter-meaning change behind an unchanged identifier. Do not
silently reuse reports generated with an older calibration or model. A dependency
update or license question requires its own reviewed scope; this template does
not authorize a choice on the owner's behalf.

## Reopening conditions

Define observable triggers: a held-out response failure, an invalid energy
balance, a new calibration, a required gesture outside the model domain, or a
demonstrated host incompatibility. Avoid vague promises to revisit the choice.
Name the next useful experiment or implementation test and what outcome would
change the decision.
