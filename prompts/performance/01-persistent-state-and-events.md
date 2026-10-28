# Work brief: four persistent strings and deterministic physical events

## Prompt to give the contributor

You are implementing a narrow event/state capability in the Iranian setar
physical-modeling repository. Read `AGENTS.md`, `README.md`,
`docs/START_HERE.md`, `docs/strings/01-identity-and-state.md`,
`docs/performance/02-event-schema.md`, and
`docs/performance/04-deterministic-transitions.md` completely. Inspect the actual
source and tests before selecting names. Treat every proposed interface in the
documentation as a design contract, not proof that an implementation exists.

Your goal is to demonstrate that a single event stream addresses four persistent
physical string states, with two single courses and a paired bass/drone course,
without creating a new string for every note. Preserve the project's C++20 core
boundary and keep JUCE out of numerical state and independent tests. Do not add
a GUI, a new synthesis method, a noise layer, or a broad controller framework.
Do not commit or publish unless the task owner separately requests it.

## Establish the baseline

Inspect the worktree and preserve unrelated edits. Identify the current string
state representation, renderer entry point, test framework, and configuration
validation. Reproduce the existing relevant tests and record the exact commands,
platform, and outcome. If the baseline cannot run, report the concrete blocker
and continue only with unaffected contract or test work.

Create a capability table before coding: existing four-state support, existing
event timestamps, contact ownership, course selection, reset semantics, and
block-partition behavior. For each row distinguish implemented, tested, proposed,
and absent. Do not infer support from a filename or an empty class declaration.

Choose the smallest testable change. If the current engine has only a simple
synthetic string, use it honestly as the mechanical fixture. You can verify
ownership and continuity without claiming a complete nail model. Do not make
the event layer depend on a future contact implementation that is not available.

## Implement the contract

Introduce or refine validated slot IDs, course membership, event identity,
configuration revision, owner identity, and absolute frame semantics. Use bounded
payloads and prepared handles. Document capacities and failure outcomes. Do not
parse JSON, allocate strings, lock, or perform file I/O in rendering.

Implement only the action families needed for the selected tests. A trigger may
use the current verified excitation model, but it must act on the existing state.
A release must have an explicit policy and must not accidentally clear unrelated
strings. A reset is a distinct nonphysical operation with a reason code.

Use half-open block intervals and define same-frame ordering. Preserve source
sequence and configuration checks. Reject stale handles, invalid slot masks,
unknown owners, NaN payloads, and unsupported targets without mutating unrelated
state. A debug assertion alone is insufficient for untrusted host input in a
release build.

Keep event-to-action tracing bounded. Trace records should identify application
frame, resolved slots, owner/contact IDs where applicable, and rejection code.
Format and write traces outside the callback. Telemetry overflow must not alter
audio timing or physical state.

## Independent tests

First, excite one slot with coupling disabled and prove that other slots do not
receive direct excitation. Then excite the same slot twice at several phases of
its continued motion. Compare with an independently constructed continuation
reference and an intentionally reset-state control. The implementation must
follow continuation, not the reset control.

Test paired-course selection without merging its members. Give the two members
different synthetic parameters and verify independent state and diagnostics.
Use an explicit synthetic manifest; do not present those parameters as measured
setar strings.

Render the same event script as one block, fixed-size blocks, and irregular
blocks. Include events at frame zero, adjacent frames, exact block boundaries,
and same-frame transactions. Compare event traces before comparing audio. A
whole-sample timing difference cannot be excused by a broad floating-point
tolerance.

Test ownership reuse and stale events. Release one owner while another remains
active; reuse a storage slot or channel; deliver an old release; change
configuration revision. Verify that no stale command affects the new owner.
Force event and trace capacity limits and demonstrate the documented behavior.

## Acceptance report

Deliver the focused code change, tests, exact run commands, trace artifacts, and
a short explanation of remaining limitations. Report allocation checks or
bounded-work evidence only if actually performed. Do not equate compilation with
real-time safety or acoustic validation.

The change is acceptable when four states remain persistent, repeated excitation
does not reset them, course targeting is explicit, frame semantics are invariant
to block partition, and malformed events cannot corrupt unrelated state. If an
acceptance condition fails, preserve the failing fixture and explain the smallest
next investigation rather than broadening the implementation until the failure
is obscured.
