# Review plugin compatibility and real-time safety

## Independent review request

Review `[candidate change/revision]` for `[specific host/core contract]`. This is
a review, not authorization to refactor, rewrite presets, publish a package, or
change CI permissions. Read the implementation, tests, dependency versions, and
relevant plugin/real-time chapters. Separate confirmed defects, unsupported
claims, and questions requiring experiment.

Begin with architecture. Does the numerical core remain independent of JUCE?
Do headless and plugin paths use the same physical model and canonical events?
Does the editor own only presentation, or does closing it alter sound? Are
physical state, control targets, prepared configuration, and output observation
distinct? Identify any wrapper-only correction that invalidates existing core
evidence.

## Callback review

Trace every reachable callback path, including errors, overflow, preset
installation, and last-owner destruction. Look for allocation/free, locks,
logging, file/network access, string formatting, thread operations, unbounded
sorting, and uncapped iterative solves. Verify stack and workspace bounds.
Check atomic lock freedom assumptions on supported targets.

Examine numerical failure behavior. Are invalid inputs rejected before poisoning
state? Does a capped solve report nonconvergence? Does output protection conceal
an instability? Are denormal and floating-point environment policies scoped and
tested? Does sleeping depend on a valid physical-state predicate rather than
zero input or a quiet single receiver?

## Concurrency review

For every shared object identify writer, readers, publication, acquisition,
reuse, and reclamation. Check generation handling and stale worker completion.
An atomic pointer without a lifetime protocol is insufficient. A sequence
counter around racing non-atomic payload is not automatically legal C++.
Verify that a stalled UI or worker cannot make audio wait or grow memory.

Read race/memory/real-time instrumentation results and their scope. Check that
tests exercised active notes, state changes, and faults rather than only silence.
Treat sanitizer success as evidence about tested executions, not proof of all
interleavings. Narrow suppressions require documented reasons.

## Compatibility review

Compare stable parameter IDs, normalized mappings, ranges, defaults, text
conversion, and automation timing against the previous contract. Determine
whether old host automation changes meaning. Check state-schema migration using
immutable historical fixtures, missing assets, unsupported future states, and
transactional failure behavior. A restored UI value is not sufficient if the
audible prepared model differs.

Check event identity and overflow. Can rejected note-ons later receive harmless
note-offs? Can old note-offs terminate reused slots? Can a full queue swallow
the recovery request? Does continuous-control coalescing erase meaningful
gestures? Verify the declared behavior through an independent state-machine
oracle and block-partition tests.

Check host lifecycle: variable/zero blocks, sample-rate and precision changes,
bus negotiation, unused outputs, offline restore, transport discontinuity,
latency changes, tails, suspension, and destruction. Inspect actual wrapper tests
instead of assuming the framework guarantees the product's intended semantics.

## Evidence and findings

Verify binary and dependency identities for validators and real-host tests.
Distinguish configured CI from observed Linux/Windows/macOS runs. Distinguish
VST3, AU, standalone, generic tools, and specific DAWs. A package's signature
does not prove correct sound or lifetime behavior.

For each finding provide severity, exact file/contract, reproducible scenario,
why existing tests miss it, consequence, and smallest corrective experiment or
change. Do not demand an unrelated redesign when a narrow contract fix suffices.
Do not approve a release merely because the code looks clean if required evidence
is absent.

Conclude with supported scope, blocking defects, missing evidence, and residual
risks. If the change is acceptable only as a diagnostic synthetic build, say so.
The review should leave a concrete path to a stronger claim without rewriting
unexecuted tests as passes.
