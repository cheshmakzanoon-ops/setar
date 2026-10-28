# Integration work orders

Integration connects accepted components without weakening their contracts.
Use these prompts after reading the affected domain prompts and
[CONTRIBUTING.md](../CONTRIBUTING.md). They do not authorize repository
publication, dependency licensing, participant contact, or unrelated refactoring.

## I-PROMPT-01: Integrate one physical boundary

> Identify exactly two components and their shared physical port. Read their
> equations, units, signs, frames, state ownership, and lifecycle. List which
> behavior is implemented, proposed, synthetic, or measured. Stop if the units
> or power-conjugate variables cannot be reconciled; do not hide the mismatch
> with an arbitrary gain.
>
> Write an interface table mapping input/output quantities, sample times,
> interpolation, initialization, reset, invalid-state handling, and parameter
> updates. Derive the combined energy or power balance. Specify what the
> interface stores and which component owns every mutable state. Preserve the
> distinction between mechanical response and acoustic observation.
>
> Build a minimal coupled fixture with an independent reference. Test an
> uncoupled limit, zero excitation, sign reversal, changed block partition,
> invalid parameters, repeated preparation, and the strongest admissible
> interaction relevant to this boundary. Show convergence or an error budget
> for interpolation and model reduction. Do not normalize unstable state.
>
> Run the applicable full headless baseline and record exact results. Acceptance
> requires both component contracts and the combined invariant to hold in the
> stated domain. Hand off the interface, evidence, limitations, and next
> coupling question. A passing isolated component does not automatically pass
> the coupled system.

## I-PROMPT-02: Integrate engine and host boundary

> Inspect the current core and optional plugin implementation. Do not assume
> that a documented host feature exists. Identify the requested lifecycle or
> event behavior and map host timestamps, parameters, note identities, and
> state restoration into the core's explicit contracts.
>
> Keep numerical physics in the core and host adaptation at the boundary.
> Preparation, allocation, parsing, and expensive validation occur away from
> audio processing. Define a bounded handoff for parameter or asset changes,
> including capacity, overflow, ordering, and failure behavior. Do not add a
> mutex or file access to an audio callback to resolve an integration problem.
>
> Compare a deterministic event sequence through the offline path and host
> adapter. Test empty and irregular blocks, same-sample events, repeated
> preparation, sample-rate changes, malformed state, and documented reset
> semantics where supported. Distinguish current tests from new required
> fixtures. Use actual host tests for host claims; headless tests alone do not
> establish plugin compatibility.
>
> Acceptance includes preserved core behavior, bounded execution, and exact
> state/event meaning. Report the hosts and platforms actually exercised and
> those not available. Do not enable downloads or select a JUCE license on the
> owner's behalf. Next: test the remaining lifecycle transition with the
> highest demonstrated failure risk.

## I-PROMPT-03: Produce an integration evidence report

> Inspect the target revision, worktree changes, CMake configuration, registered
> tests, and current capability statements. Reproduce the implemented baseline:
> `cmake --preset headless-debug`, `cmake --build --preset headless-debug`,
> `ctest --preset headless-debug`, and `python3 tools/audit_docs.py`.
> Run supported optimized or sanitizer configurations when relevant and record
> their separate outcomes. Do not represent a selected diagnostic subset as
> the complete suite.
>
> Generate a fresh synthetic renderer/report artifact through the documented
> engineering workflow. Check its metadata, hashes, local resources, actual
> rendering, and synthetic labels. Keep source revision and dirty-worktree
> provenance accurate. Do not overwrite accepted evidence or invent a
> screenshot, benchmark, or remote CI success.
>
> Trace each claimed capability to executable code and observed tests, then
> separately to measurement or participant evidence if such a claim is made.
> Review local links and phase dependencies without treating the documentation
> auditor as a scientific validator. Identify stale outputs caused by changed
> calibration, schema, or parameter meaning.
>
> Deliver a capability matrix: implemented, tested locally, tested on named
> platforms/hosts, physically validated for scope, and deferred. Lead with
> failures and blockers that affect the requested claim. Acceptance is an
> honest reproducible integration state, not a universal green label. Next:
> assign the smallest bounded task that removes the most consequential gap.
