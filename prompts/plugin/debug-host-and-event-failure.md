# Diagnose a host crash, stuck note, or state mismatch

## Diagnostic request

Investigate `[observed symptom]` in `[host/version/format/OS/architecture]` using
`[plugin binary hash and source revision]`. This request authorizes diagnosis
and bounded non-destructive checks. Implement a fix only if the task explicitly
includes it. Do not clear broad host caches, reset user preferences, reinstall
commercial software, or modify unrelated sessions without authorization.

Preserve the failing event sequence, state bytes, host project when permitted,
audio settings, logs, crash report, and exact candidate identity. Redact private
paths and participant data from public artifacts. Distinguish a scanner failure,
construction failure, processing failure, editor failure, and shutdown failure.
The lifecycle stage is often more informative than the final crash location.

## Minimize without changing meaning

Reproduce in a dedicated minimal session or harness. Remove unrelated effects
and tracks only in a copy. Keep the events that define the failure: repeated
pitch overlap, member-channel reuse, sustain transition, zero block, preset
change, or editor closure. A minimized sequence that no longer contains the
race or note lifetime is not a valid reproducer.

Compare the canonical event stream in the headless engine and adapter harness.
Log requested, translated, accepted, and rejected events with sample time,
source, target, and generation. Use bounded capture; do not add printing or
allocation to the callback and thereby alter the bug. Compare configurations
and asset hashes before comparing audio.

## Symptom-specific branches

For a stuck note, inspect accepted note-on lifetime, release identity, channel
reuse, sustain state, stolen-slot generation, queue capacity, and emergency
recovery. Determine whether the note-off was absent at input, lost during
translation, rejected during overflow, or applied to the wrong target. Do not
patch every note-off into an all-sound-off event; that hides identity defects
and breaks overlapping notes.

For a restore mismatch, compare serialized schema, parameter values, normalized
mapping revision, tuning map, model asset, requested generation, and active
prepared generation. Test whether an asynchronous worker completed before the
host began offline rendering. A correct preset title with old coefficients is
a state transaction defect, not a cosmetic issue.

For a crash or hang, identify ownership and thread context. Check last-reference
destruction, worker joins, editor callbacks after destruction, invalid buffer
extents, repeated preparation, and waits for acknowledgements after processing
has stopped. Use supported sanitizers/debuggers in an appropriate harness and
preserve their configuration. Do not suppress the entire framework to hide an
unexplained report.

For a sound difference across hosts, compare block partitions, event offsets,
precision, bus mapping, latency/tail behavior, and offline quality settings.
Different host output is not automatically a physical-model problem. Render
the same canonical input under several block sequences and inspect intermediate
state before modifying damping or gain.

## Test and hypothesis discipline

List competing causes and a distinguishing intervention for each. Change one
factor at a time, retaining the control build. Record both successful and failed
reproductions. If the failure is intermittent, report observation count and
conditions without interpreting a short nonfailure run as proof of repair.

Create a deterministic state-machine test for the discovered semantic failure
where possible, plus a stress test for scheduling sensitivity. A regression must
fail on the original behavior and assert the intended contract, not merely a
new golden audio hash. Verify that the fix does not introduce callback allocation,
unbounded work, or an incompatible saved-state interpretation.

## Handoff

Return a minimal reproducer, lifecycle/event trace, causal explanation supported
by intervention, remaining uncertainty, and smallest recommended fix. If a fix
was authorized, include changed files, independent tests, actual platform/host
results, and the original symptom's confirmation. Preserve unrun combinations as
unrun. End with recovery guidance that does not destroy user work, and distinguish
a temporary containment workaround from a corrected underlying contract.
