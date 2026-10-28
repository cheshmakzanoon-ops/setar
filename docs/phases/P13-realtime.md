# P13 — Turn the reference into a bounded real-time engine

**Status:** specification. **Prerequisites:** P12 for calibrated production
behavior; synthetic performance engineering may begin earlier. **Exit evidence:**
bounded callback work, safe state ownership, qualified performance, and controlled
error relative to the reference model.

## Optimize a contract, not an attractive average

The audio engine must return each block within the supported execution budget.
Average throughput alone does not protect a musician from sporadic glitches.
Likewise, a fast model is not acceptable if its simplifications erase the
mechanisms that made the reference predictive. Read [real-time engineering](../realtime/README.md),
[numerics](../numerics/README.md), and [engineering](../engineering/README.md).

Define the initial supported platforms, sample rates, block sizes, instrument
counts, gesture densities, and quality settings. These are qualification
conditions, not calendar commitments. Performance claims must name hardware
and build configuration; a percentage without those conditions is meaningless.

## P13-T01: inventory all callback work

Trace the entire call graph reachable from processing. Identify allocations,
deallocations, locks, waits, file/network access, logging, exceptions, lazy
initialization, framework callbacks, and unbounded loops. Include error paths
and destructors. A normally unused failure branch can still run on the audio
thread when a performer sends an extreme event.

Assign explicit bounds to modes, contacts, events, iterations, telemetry,
channels, and asset transitions. State what happens at each bound. Ignoring
the remainder of a diagnostic event flood is not automatically an acceptable
production policy; critical release/panic state and ownership must be preserved
according to the engine contract.

Create tests that detect allocation and blocking behavior where tools permit,
and supplement them with code review. No single instrumentation technique
proves the absence of all real-time hazards. Keep a documented list of operations
that are forbidden by design and the evidence used to check them.

## P13-T02: separate preparation from processing

Prepare geometry-derived assets, reductions, coefficients, factorization,
memory, and validated control mappings outside the callback. Define a prepared
object's ownership and lifetime. Successful preparation creates an installable
object; failed preparation leaves the active engine unchanged.

Specify state exchange and retirement without relying on a UI thread destroying
an object that the audio thread still uses. Atomic pointer publication alone
does not solve reclamation. Choose and test a bounded ownership protocol,
including rapid repeated requests, canceled preparation, shutdown, and host
reprepare.

Separate immutable configuration from mutable physical state. Sharing immutable
body assets across instances may save memory, but sharing oscillator/contact
state would couple unrelated instruments. Make those distinctions visible in
types and tests rather than informal comments.

## P13-T03: establish the reference/production comparison

Replay identical physical events into both paths. Match initial conditions,
sample-rate interpretation, observation chain, and parameter versions. Compare
mechanism-specific metrics, work/energy behavior, and output differences under
the declared tolerance policy. Use a richer or more accurate reference where
the production path deliberately approximates a solve.

Evaluate reductions and optimizations one at a time. Changing mode count,
precision, contact iteration, and damping together prevents attribution of
error. Keep the before/after artifacts and a statement of the cost saved per
error introduced. Reject a speedup that passes one isolated note but fails
repeated contact or long decay behavior.

## P13-T04: profile representative and adversarial workloads

Measure distributions and worst observed execution times for the intended
hardware/build matrix. Use realistic event streams, maximum supported active
contacts, rapid state changes, silence, denormal-prone tails, and invalid inputs
that reach containment paths. Separate debug instrumentation cost from release
performance without abandoning correctness checks.

Control the measurement method: warmup, timer overhead, power-management state,
background load, and repetitions. Hosted CI is useful for regression signals,
but its shared hardware is not a substitute for hardware-qualified real-time
claims. Store raw measurements and context, not only a single best number.

Measure memory and startup/preparation behavior separately. An asset that fits
the callback budget but causes unacceptable transient memory growth during
replacement still needs engineering attention. Keep resource limits visible
and fail preparation with actionable diagnostics rather than exhausting memory.

## P13-T05: optimize with numerical discipline

Use data layout, vectorization, caching, and reduced representations only after
profiling identifies the relevant cost. Compare compiler modes and floating-
point behavior. Fused operations, reassociation, denormal settings, and fast-
math options can change the numerical contract; they require explicit review
and regression evidence.

Do not promise bitwise identity across architectures unless it is actually
required and demonstrated. Establish tolerance-based physical/numerical
equivalence where appropriate. At the same time, deterministic event order and
repeatability within a defined build should remain testable.

Bound nonlinear work. If a production solve cannot meet residual criteria
within its budget, execute the documented containment/recovery policy and emit
bounded telemetry. Do not return arbitrary last iterates as if the physical
solve succeeded. Track the rate and conditions of containment events in
qualification; frequent fallback may invalidate the supported domain.

## P13-T06: qualify long-running and concurrent behavior

Exercise repeated preparation, many plugin instances, start/stop cycles, asset
replacement, suspend/resume, and sustained event streams. Use sanitizers and
race-oriented testing in suitable non-real-time builds. Inspect numerical
drift, memory growth, ownership errors, and cross-instance interference.

Exit with a capability matrix, measured resource envelope, reference-error
report, and explicit unsupported cases. Hand the engine a stable host-neutral
API so P14 does not need to understand solver internals.

> Profile and optimize the identified bottleneck without changing the physical
> contract silently. Inventory callback hazards and prove explicit work bounds.
> Compare identical event replays against the reference, including adversarial
> ringing/contact cases. Demonstrate preparation ownership and retirement under
> stress. Report hardware-qualified latency distributions and numerical error,
> not just average CPU or a faster isolated benchmark.
