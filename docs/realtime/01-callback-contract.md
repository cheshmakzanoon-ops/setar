# The audio-callback contract

The real-time engine is a bounded evaluator of already prepared physical state.
Its callback is not a miniature application loop. File parsing, fitting,
allocation, modal reduction, mesh construction, and user-interface work belong
outside it. This chapter defines a proposed C++20 contract and the evidence needed
to accept an implementation. A headless renderer running quickly does not by
itself satisfy this contract.

## Boundary and ownership

The core receives prevalidated configuration, spans of host audio memory,
bounded event data, and a declared sample count. It owns numerical state and
preallocated workspaces. It returns audio, bounded diagnostics, and status flags.
It does not own the host's buffer and must not retain pointers beyond their
documented lifetime. Store sample rate and physical constants in an explicit
prepared configuration rather than querying a framework repeatedly.

Separate preparation from processing in the type/API design. Preparation may
allocate and can fail with a rich diagnostic. Processing uses a prepared object,
has a defined failure response, and does not throw through the host boundary.
`noexcept` helps express that contract but does not prove that a call graph is
allocation-free or bounded. A function can be `noexcept` and still lock, allocate,
or terminate the process.

Define maximum voices, physical strings, modes, contact points, events per block,
internal substeps, and nonlinear iterations. These limits follow error and
performance analysis, not arbitrary constants hidden in source. Store their
units and admissibility rationale. A configuration beyond supported capacity is
rejected or explicitly reduced outside processing, never silently truncated into
a different instrument.

## Forbidden callback work

Do not allocate or free heap memory, wait on locks, perform file/network I/O,
format strings, print logs, invoke modal dialogs, create threads, or call an
unbounded iterative algorithm. Check indirect paths: a container push can
reallocate; a copied string can allocate; a smart-pointer decrement can destroy
an object; an error handler can log; a framework notification can dispatch work
or take a lock. “Usually cached” is not a real-time guarantee.

A virtual call is not inherently forbidden if its target and work are controlled.
A mutex try-lock is not an adequate architectural substitute for ownership:
success can enter an unbounded critical section, and failure still needs a
correct sonic policy. An atomic operation is not automatically lock-free on every
target type and architecture. Check `is_always_lock_free` where appropriate and
validate runtime assumptions for the actual supported build.

Bound stack use too. Replacing a heap allocation with a huge local array can
overflow a host audio thread's stack. Preallocate workspaces during preparation,
touch memory outside processing when the platform policy permits, and avoid
large recursive traversals. A fixed capacity is useful only if every path honors
it and capacity exhaustion has tested semantics.

## Work bound

Express a processing bound in terms of declared maxima, for example number of
samples times strings, modes, and bounded contact iterations plus event work.
The expression need not predict exact CPU cycles, but should identify every
factor that can grow. An iterative contact solve must have a maximum iteration
count and a valid response when its residual remains unacceptable.

Do not silently accept an unconverged physical update. Options include a
prevalidated conservative fallback, a rejected gesture, or a controlled
containment path. Which option is appropriate depends on the model. Record the
event and residual through bounded telemetry. A fallback that changes the
physical law needs separate verification and clear status; it cannot be counted
as an ordinary successful solve.

Host blocks can vary. Prepare enough storage for the supported envelope and
process a larger-than-expected block in bounded chunks if that is the declared
strategy. Chunking must preserve absolute event offsets, physical state, and
automation semantics. If a configuration is unsupported, return a defined safe
result and diagnostics rather than allocate a larger buffer mid-callback.

## Input validation without redundant expense

Validate expensive configuration constraints during preparation: positive
geometry, admissible elasticity, stable discretization, finite coefficients,
capacity, and observation dimensions. At callback boundaries validate cheap
runtime facts: buffer sizes, event offsets, finite control values, and state
generation identity. Avoid repeated matrix admissibility tests per sample.

Define behavior for zero samples, zero channels, null optional playhead data,
unsupported bus combinations, and invalid parameter events. Do not divide by
block size in a zero-sample callback. A malformed control should not poison the
entire state with NaNs. Distinguish rejected input from a numerical defect arising
from admissible input; both need evidence but imply different fixes.

Physical parameter bounds must not become a hidden arbitrary clip. If the host
provides a normalized parameter within its advertised range that maps outside
the model's valid region, the mapping is defective. Reject unsafe state during
load and expose the error. During live automation, apply the declared mapping
and bounded transition policy, not an undocumented clamp that changes musical
meaning unpredictably.

## Audio output and fault containment

Write all output channels promised by the wrapper. Clear unused channels and
avoid leaking uninitialized memory. Specify whether the core emits physical
units or normalized audio and where gain conversion occurs. Output protection,
if present, is a product layer with a visible incident counter; it must not hide
an unstable solver from tests.

If finite-state checks detect failure, stop propagating invalid state. Use a
bounded containment path that avoids a large discontinuity where feasible,
marks the affected instance invalid, and requests non-real-time recovery.
Do not reset every state silently and continue as though the physical trajectory
remained valid. Test faults at different oscillation phases and output levels.

The fault mechanism itself must not allocate, log, or repeatedly rebuild. A
single atomic status word and fixed-size diagnostic record can communicate the
reason. Rate-limit or coalesce repeated reports on the consumer side. Maintain
an incident count even if detailed records are dropped.

## Instrumentation strategy

Use a test harness that executes the same processing entry point as the wrapper.
Instrument allocations and deallocations within callback scope, test known unsafe
operations to establish detector sensitivity, and exercise preset changes,
maximum polyphony, event overflow, faults, and editor activity. Do not conclude
“no allocations” from a test that only renders silence after preparation.

Clang's [RealtimeSanitizer](https://clang.llvm.org/docs/RealtimeSanitizer.html)
can detect known unsafe calls in instrumented nonblocking contexts. It is an
additional check, not a mathematical proof of bounded execution. Pin the
toolchain and verify availability; keep a portable allocation/call-graph strategy
for targets where that facility is unavailable. No instrumentation result
replaces reviewing third-party calls and destruction paths.

Use race detection in a separate concurrency harness and memory sanitizers in
appropriate builds. Inspect optimized builds too: correctness can depend on
undefined behavior that a debug build conceals. Do not use sanitizer overhead
as the final performance measurement.

## Acceptance artifacts

Provide the callback call graph, ownership table, maximum-capacity registry,
work-bound expression, allocation/deallocation logs, fault cases, queue tests,
and controlled-hardware timing report. Include all platform-specific assumptions
such as atomic lock freedom and floating-point environment handling. A reviewer
should be able to identify the worst plausible work path, not just the common
path.

The next action is to bring one numerical subsystem behind this contract and
run the harness with deliberately adversarial inputs. Keep the offline reference
separate so real-time approximations can be measured without contaminating the
oracle. Only after numerical error and callback behavior are both understood
should the subsystem be advertised as real-time capable.
