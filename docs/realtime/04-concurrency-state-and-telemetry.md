# Concurrency, prepared-state exchange, and bounded telemetry

The audio thread, message thread, host callbacks, and background workers have
different responsibilities. Correct concurrency means explicit ownership and
lifetime, not merely sprinkling atomics over shared objects. This chapter defines
proposed exchange protocols and tests. It intentionally avoids presenting an
untested lock-free container implementation as production-ready code.

## Ownership table

| Object | Writer | Readers | Lifetime rule |
| --- | --- | --- | --- |
| Active numerical state | Audio processing owner | Audio processing owner | Never edited by UI or worker |
| Prepared immutable configuration | Preparation worker before publication | Audio owner after acquisition | Reclaimed only after reader acknowledgement |
| Scalar parameter target | Declared host/UI publication path | Audio owner | Atomic or event protocol with defined order |
| Diagnostic record | Audio owner | Non-real-time consumer | Fixed storage with explicit slot ownership |
| Serialized preset bytes | State-management path | Parser and host transfer | Never parsed in processing |
| Editor widgets | Message thread | Message thread and accessibility bridge | No retained audio pointers |

This table is a design example. The actual implementation must document every
shared object and account for host calls that occur on threads other than the
message thread. Do not assume all state callbacks arrive from the UI because a
single host happened to behave that way.

## Scalar values and compound state

An atomic scalar is suitable for an independent latest-value target when
intermediate values may be omitted. It is not enough for a compound physical
configuration whose fields must be mutually consistent. Reading length from one
revision and tension from another can violate a stability bound even if each
field is individually valid.

Publish compound configurations as immutable snapshots with a generation number
and a clear acquisition boundary. The audio thread either uses the previous
complete snapshot or a new complete snapshot. It never observes a partially
constructed one. Preparation validates units, dimensions, stability, and capacity
before publication. Rejected snapshots retain a diagnostic outside the callback.

Memory ordering follows the ownership protocol. A producer writes payload then
publishes availability with release semantics; a consumer acquires availability
before reading payload. Reuse needs a corresponding return-of-ownership signal.
Do not copy a generic ring-buffer pattern without checking whether it is single-
producer/single-consumer, whether host and UI are actually multiple producers,
and whether its atomic types are lock-free on supported targets.

## Reclamation is part of real-time safety

Swapping an atomic pointer is not sufficient if destroying the old object can
occur in the callback. Reference-counted ownership can free memory or invoke
complex destructors on the last decrement. `shared_ptr` is convenient outside
processing, but its existence does not establish a safe callback lifetime.

Use a bounded retirement protocol. The worker prepares a new object in an
available slot; the audio owner acquires it at a defined boundary and publishes
which generation is no longer referenced; the non-real-time owner reclaims that
generation after acknowledgement. Specify maximum in-flight and retired objects.
If the worker runs ahead and fills capacity, it coalesces or waits outside audio,
not by forcing the audio thread to free objects.

Avoid the naïve double-buffer trap: a producer cannot rewrite the old buffer
merely because a new pointer was published if a reader still holds the old one.
Likewise, a sequence counter around non-atomic shared payload does not magically
make racing C++ accesses legal. Use a protocol with actual ownership or correctly
designed atomic storage, and verify it with stress tests and review.

## State transitions and continuity

Installing a new configuration may require more than exchanging coefficients.
Define how physical state maps between configurations. Changing modal basis,
string length, body model, or sample rate can make old coordinates meaningless.
Options include a verified state projection, a transition between prepared
engines, or a clearly labeled reset when processing is suspended. Each has
musical and energy consequences.

Crossfading two outputs can conceal an audible jump but does not prove physical
state continuity. It also temporarily doubles work and may require both old and
new configurations to remain alive. Include that cost and lifetime in capacity
and timing tests. Do not use a crossfade as evidence that changing tension or
fret location obeys the physical work law.

Define which events belong to which generation. Pending note expression cannot
be applied blindly after a preset change that changes voice mapping. State
replacement can flush, translate, or preserve events under a documented rule.
Test note-off and panic during transition, not only a silent preset load.

## Telemetry as lossy observation

Diagnostic telemetry must be bounded and optional. The audio thread writes
fixed-size records or aggregate counters; the UI or worker consumes them. A
slow editor must not stall processing. Dropping a meter frame is usually
acceptable, but dropping an incident count is not. Keep monotonically tracked
counters or another bounded summary for overflow, solver failure, and protection
events even when detailed records cannot be retained.

Declare what each display means. A meter might show a decimated envelope, not
every sample peak. A solver plot might be a snapshot of one string, not the
entire instrument. Timestamp records so the display can identify stale data.
The UI should show “paused” or “stale” rather than animate invented values when
no new data arrive.

Do not publish raw mutable engine pointers to the editor. Copy bounded summaries
or transfer ownership of preallocated observation buffers. Opening and closing
the editor must not change synthesis state or allocation behavior. A timer may
poll telemetry on the UI side, but processing must not require that timer to run.

## Thread shutdown and cancellation

Shutdown follows ownership dependencies. Stop accepting new worker requests,
cancel or finish non-real-time work, prevent new publication, detach editor
consumers, and reclaim only when audio use has ended under the host lifecycle.
Do not join a worker thread from the audio callback. A worker waiting for audio
acknowledgement must have a shutdown path when the host has stopped callbacks.

Cancellation must not publish a half-built configuration. Use request generations
so an old expensive task cannot overwrite a newer selection after it finishes.
If the newest request fails, define whether the active state remains unchanged
or a fallback is selected. Report the failed request without confusing it with
the currently audible state.

## Concurrency test plan

Stress editor open/close, rapid parameter changes, repeated state saves/loads,
worker cancellation, host suspension, zero-sample callbacks, reprepare, and
destruction with queued work. Use randomized scheduling in a harness, but retain
the request sequence and seed. Add deterministic tests for each protocol state
transition so a rare race is not the only evidence.

Run ThreadSanitizer where supported and ensure relevant code is instrumented.
Interpret third-party reports carefully without suppressing broad project paths.
Test reclamation with objects whose destruction records the executing thread in
the harness. Assert that destruction and allocation never occur in callback
scope. Add generation counters and ownership assertions in debug builds.

Test a stalled consumer and a producer faster than the audio owner. Storage must
remain bounded, old state must remain valid, and recovery must not require a
callback wait. A benchmark with cooperative thread timing is not enough to prove
the protocol handles adversarial timing.

## Acceptance and next action

Deliver the ownership diagram/table, memory-order rationale, bounded-capacity
analysis, lifetime traces, stress results, sanitizer reports, and audible
transition evidence. The gate requires both race freedom in tested paths and a
reviewed lifetime argument; neither alone proves all possible executions. The
next action is integration into the actual host state lifecycle, followed by
repeating the stress suite through that wrapper.
