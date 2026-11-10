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

## Entry evidence and research map

Begin with the current [prototype timing and regression evidence](../realtime/05-prototype-realtime-evidence.md)
and [source audit](../engineering/2026-11-10-baseline-audit.md). The allocation,
partition and capacity tests are executable partial evidence. The float-output
containment, finite-speed active-finger movement and iteration-reporting defects
in the historical report are fixed; preserve their regressions. The existing
container benchmark still misses dense-pattern block deadlines. Neither an
unreduced prototype nor a successful CTest run constitutes the independent
reference or hardware qualification required for this phase.

| Decision | Research route and bounded reading | Current evidence boundary |
| --- | --- | --- |
| Callback/lifecycle ownership | `R-SDK-JUCE-AUDIOPROCESSOR` in the [source atlas](../research/source-atlas.md); inspect the selected checkout's `processBlock`, `prepareToPlay`, state and suspension contracts; [callback](../realtime/01-callback-contract.md) and [concurrency](../realtime/04-concurrency-state-and-telemetry.md) chapters | Host-neutral engine is implemented; publication and reclamation remain proposed. |
| Approximation error | `R-NUM-BILBAO-SMITH-2005`, §II/§III, and `R-CONTACT-BILBAO-2014`, collision potential/discrete-work derivation; [modal reduction](../numerics/05-modal-reduction.md) and [contact solver](../numerics/15-modal-contact-solver.md) | Sources provide methods, not specimen coefficients or proof of this complete solver. Record the sections actually inspected. |
| Timing uncertainty and numerical hygiene | `R-METROLOGY-JCGM-100`, §§4–5 and §7; [performance method](../realtime/03-performance-and-numerical-hygiene.md), [floating-point contract](../numerics/10-floating-point-and-realtime.md) and their official compiler references | Existing summary CSV is a development measurement; controlled raw block traces remain required. |

Freeze source/configuration/event hashes, supported capacities and physical
observables before each comparison. Missing specimen calibration blocks P12
promotion, while synthetic ownership, parity and performance experiments can
proceed with that limitation visible. The task-to-work-order mapping below follows
the [crosswalk](../../roadmap/phase-task-crosswalk.json); acceptance names in the
[requirements](../../roadmap/requirements.json) are not automatically CTests.

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

### Execution recipe

**Question and research.** Can every admitted gesture and fault path finish with
bounded work without losing the release of an already engaged contact? A bounded
loop that leaves a finger pressing forever changes the instrument. Read
`R-SDK-JUCE-AUDIOPROCESSOR` through the research map, then the callback and
[event-overflow](../realtime/02-events-capacity-and-overflow.md) chapters. Search
the selected JUCE source for `processBlock`, `MidiBuffer` iteration and message
ownership, and the compiler's official RealtimeSanitizer documentation for
detector coverage. These are interface/method sources, not setar measurements.

1. Trace current `Instrument::process` through event application, channel
   assembly, linear/contact solve, `step`, observation conversion, `snapshot`,
   `ledger` and reset in [instrument.cpp](../../src/instrument/instrument.cpp).
   Inventory each loop's controlling quantity, allocation/destruction path,
   external call, stack workspace and exceptional branch. Include the existing
   optional wrapper path separately; core hooks cannot certify JUCE processing.
2. Extract the actual limits from `ProcessSpec` in
   [config.hpp](../../include/setar/instrument/config.hpp), accepted configuration
   validation and prepared mode counts. Derive a bound in frames, modes,
   interaction channels, Newton iterations and event count; distinguish a
   configurable count from a wall-time guarantee. For each capacity test below,
   at and above the bound, record requested/accepted events and final contact
   state. Current excess events are ignored and counted; design any production
   release/panic protection explicitly rather than inferring it exists.
3. Run allocation/deallocation interception with a deliberate positive control
   and adverse synthetic sequences, then inspect library allocator, blocking and
   error paths outside its coverage. Preserve event offsets in samples, geometry
   in metres, forces in newtons and the same initial ringing state across runs.

Retain a callgraph/capacity table, detector configuration and blind spots, raw
fault traces and accepted/rejected/inconclusive decisions under `ENG-P13-T02`,
`REQ-RTS-001` and `REQ-RTS-003`. Current `realtime.allocation_hook`,
`realtime.max_work` and `realtime.capacity_overflow` in
[realtime_tests.cpp](../../tests/instrument/realtime_tests.cpp) check limited
synthetic paths; proposed `tests/realtime/hostile_callback_tests.cpp` extends
them. Reject an unbounded host scan, callback destruction or stuck-contact
counterexample. Hand explicit bounds and unresolved paths to T02, T04 and P14.

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

### Execution recipe

**Question and research.** Can one complete model replace another while the old
ringing state remains owned, interpretable and reclaimable? Read
`R-SDK-JUCE-AUDIOPROCESSOR` for the pinned preparation/suspension/state-callback
contract and [prepared-state exchange](../realtime/04-concurrency-state-and-telemetry.md).
Search the selected compiler/standard-library documentation for atomic lock
freedom, acquire/release publication and destructor execution; retain exact
version and inspected location. Acquire measured transition recordings only
after a specimen and permissible retuning procedure exist; synthetic state
projection and lifetime tests do not require that acquisition.

1. Document the existing single-owner contract in
   [instrument.hpp](../../include/setar/instrument/instrument.hpp).
   `Instrument::prepare` builds a candidate `Impl` before installing it and
   preserves the prior engine on preparation failure, but it must not race
   `process`, `reset`, `snapshot` or `ledger`. This is not asynchronous asset
   publication. Inventory immutable coefficients, mutable modes/contacts,
   queues, scratch and observation state, with owner and destruction thread.
2. Specify a bounded generation/slot protocol before implementing proposed
   `src/engine/asset_publication.cpp`: worker builds and validates; audio adopts
   a complete generation at a declared boundary; worker reclaims only after
   acknowledgement. Set maximum pending/retired assets and the non-audio policy
   when full. Account for canceled older requests and shutdown after callbacks
   stop, without requiring audio to wait or reclaim.
3. For each change of rate, modal basis, geometry or body, declare reset,
   projection or dual-engine transition. Compare displacement/velocity and
   stored energy in SI units before/after; record external work or deliberate
   energy removal. A fade's pleasing output does not establish mechanical
   continuity. Include doubled state/work in T04 if transitioning two engines.
4. Force invalid preparation, rapid replacement, stalled workers/consumers,
   generation-mismatched releases and destruction with queued work. Use
   destructor thread markers and a separate race-instrumented harness, plus an
   independent ownership review.

Under `ENG-P13-T02`, `REQ-RTS-001` and `REQ-RTS-003`, retain the ownership diagram,
memory bounds, generation trace, energy-transition table and rejected cases.
`engine.config_validation` and `realtime.allocation_hook` are current partial
checks; `realtime.asset_handoff` and `realtime.telemetry_snapshot` remain planned.
Reject mixed-generation coefficients, callback reclamation or an unsupported
state projection; mark missing host synchronization evidence blocked. Hand the
reviewed installation contract to T06 and P14-T03/T04.

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

### Execution recipe

**Question and research.** Which production simplification preserves the contact,
bridge and observed response within an independently justified error budget?
Use `R-NUM-BILBAO-SMITH-2005` (§II/§III) and
`R-CONTACT-BILBAO-2014` (gap, potential and discrete work) through the atlas;
search `collocated modal truncation contact compliance` and
`string barrier temporal spatial convergence`. Read
[reduction](../numerics/05-modal-reduction.md) and
[convergence](../numerics/12-convergence-and-golden-data.md). Published methods
must be re-derived for this model's boundaries; none supplies measured setar
contact parameters.

1. Freeze synthetic `InstrumentConfig`, `ProcessSpec`, initial state, canonical
   `GestureEvent` timeline and observation operator. Coordinates run from nut
   to tail; `bridge_position_m` is the speaking length. Compare port force in
   N, displacement in m, velocity in m/s and energy/work in J before audio
   gain. Keep excitation and observation unchanged between candidates.
2. Establish an independent comparator in stages. Current
   [reference.cpp](../../src/reference.cpp),
   [numerical_prototype.cpp](../../src/numerical_prototype.cpp) and
   [instrument_oracles.hpp](../../tests/support/instrument_oracles.hpp) provide
   restricted analytic/spatial fixtures. They do not provide a complete
   nonlinear four-string oracle. Specify an independently implemented refined
   contact/coupling reference and demonstrate its own refinement convergence.
   Treat the existing unreduced `Instrument` as a regression control, never
   ground truth. If the independent reference is missing, report production
   parity acceptance blocked while retaining useful control comparisons.
3. Vary one approximation at a time: retained modes, arithmetic precision,
   nonlinear tolerance or substep scheme. Separate time refinement from spatial
   truncation: current preparation also caps modes at `0.45 * sample_rate_hz`,
   so increasing rate alone changes two factors. Match represented bandwidth
   deliberately and document initial-state projection.
4. Sweep pluck/fret positions, both polarizations, repeated strikes, sustained
   contact and decay. Compare release time, penetration, port response,
   pitch/decay and energy residual; derive tolerance budgets from reference
   convergence/roundoff and the declared use, before inspecting speedup.

Retain raw event/state traces, convergence plots, error-versus-cost tables and
rejected candidates under `ENG-P13-T01` / `REQ-RTS-002`. Current
`solver.modal_exact_free_response`, `solver.newton_coupled_oracle` and
`contact.release_convergence` cover narrower mechanisms.
`realtime.reference_parity`, `realtime.multirate_alias` and
`tests/realtime/reference_parity_tests.cpp` are planned. Reject changed contact
semantics despite plausible audio; hand admitted approximations and unresolved
reference error to T05 and P12/P17 claim owners.

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

### Execution recipe

**Question and research.** Which admitted blocks miss their deadline, and which
physical interaction causes the cost? Read `R-METROLOGY-JCGM-100` §§4–5 and §7
for a timing measurement/uncertainty record, together with the
[budget](../engineering/performance-budget.md) and performance chapters. Search
the actual OS/CPU profiler's official documentation for `monotonic clock`,
`timer overhead`, `page faults` and `sampling profiler`; record the version and
measurement scope. The measurement route here is controlled hardware timing,
not an inferred percentage from listening or a specimen coefficient.

1. Freeze workload, build flags, CPU/OS/architecture, power state, thread
   placement, thermal/warmup policy and background load. Pair baseline and
   candidate runs with repeated, alternated order. Record clock resolution and
   overhead separately; retain cold start and outliers with their causes or
   uncertainty. Do not silently change machine scheduling/power settings.
2. Start with the existing [setar_benchmark.cpp](../../apps/setar_benchmark.cpp).
   A later execution may build `setar_benchmark` from a release headless build,
   then run `build/headless-release/setar_benchmark --seconds 10
   --rates 44100,48000,96000 --blocks 64,256,512 --channels 32`; repeat with
   `--ledger` as a separately labeled condition. These are current options;
   the program emits summary CSV, not a retained per-block trace or host xrun
   measurement. Its dense synthetic pattern already demonstrates deadline
   misses in the recorded container results, including after optimization.
3. Extend the harness for maximum contacts, silence/subnormal tails, invalid
   events, preparation transitions and multiple instances. Proposed
   `tools/profile_engine.cpp` must collect timestamps into preallocated storage,
   exporting afterward. For each actual nonzero block use deadline `N / fs`
   seconds; qualify a smaller plugin budget with documented host/device
   headroom. Record sample count, p50/p99, maximum observed time, miss count/rate
   and distributions by workload. Zero-length call cost is reported separately.
4. Profile channel assembly, dense solves and modal gathers to attribute cost;
   separately measure preparation latency, resident/peak memory and retirement
   backlog. Validate output/fault counters before interpreting a faster run.

Retain raw timings, profiler captures, manifests and confidence/coverage limits
under `ENG-P13-T02`, `REQ-RTS-001` and `REQ-RTS-003`. `realtime.max_work` bounds
counts, not deadlines. Reject a claimed supported configuration with misses
under its registered acceptance rule; label noisy runs inconclusive, retaining
them. Hand bottleneck evidence and reference-matched workloads to T05/P16.

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

### Execution recipe

**Question and research.** Does the measured cost reduction preserve the actual
numerical/physical contract, including failure detection? Read
`R-NUM-BILBAO-SMITH-2005` and `R-CONTACT-BILBAO-2014` only for their applicable
energy assumptions, plus [floating-point behavior](../numerics/10-floating-point-and-realtime.md).
Use its official GCC Optimize Options, Clang floating-point and Intel FTZ/DAZ
links; search the exact compiler version for `fp-contract`, `finite-math-only`
and vectorization reports. Record inspected options and emitted build flags,
not an assumption that “Release” specifies identical arithmetic everywhere.

1. Select one T04 bottleneck and retain a strict baseline. Current
   [instrument.cpp](../../src/instrument/instrument.cpp) already eliminates
   linear channels, warm-starts contact solves and skips provably separated
   fret checks. Inspect those paths and
   [numerics.cpp](../../src/instrument/numerics.cpp) before proposing SIMD,
   sparse compliance or mode reduction. Do not reintroduce already fixed
   float-containment or active-finger teleportation defects as new discoveries.
2. Derive each changed dependency/reduction and compare against T03's independent
   reference at fixed physical parameters. Measure force/displacement residuals,
   release timing, sympathetic transfer, cumulative J of work/dissipation and
   final output. Test denormal handling with a long quiet decay and subsequent
   re-strike; quantify removed energy and restore the host floating-point
   environment. Validate target instruction-set dispatch and fallback.
3. Sweep admissible stiffness, damping, contact count and solve limits around
   difficult cases. Current `FaultCode::NewtonNotConverged` reports capped
   iterations while using the last iterate; this is an explicit prototype
   limitation, not the production acceptance policy above. A proposed rollback
   or fallback requires its own energy/error evidence, telemetry and bounded
   cost. Test nonfinite detection on the final float conversion under each
   accepted compiler mode.

Under `ENG-P13-T01` / `REQ-RTS-002`, retain source/build hashes, instruction/flag
reports, error/cost plots and accepted/rejected/inconclusive decisions. Current
`engine.newton_failure_reporting`, `solver.discrete_gradient_identity`,
`realtime.capacity_overflow` and `realtime.block_partition` are regressions;
planned `realtime.reference_parity`/`realtime.multirate_alias` supply broader
acceptance only after implementation. Reject gains obtained through altered
event order, hidden contact loss, failed safety checks or reference-budget
excess. Hand the chosen arithmetic, supported domain and fallback frequency
to T06 and P14; a faster shared-container run does not qualify hardware.

## P13-T06: qualify long-running and concurrent behavior

Exercise repeated preparation, many plugin instances, start/stop cycles, asset
replacement, suspend/resume, and sustained event streams. Use sanitizers and
race-oriented testing in suitable non-real-time builds. Inspect numerical
drift, memory growth, ownership errors, and cross-instance interference.

Exit with a capability matrix, measured resource envelope, reference-error
report, and explicit unsupported cases. Hand the engine a stable host-neutral
API so P14 does not need to understand solver internals.

### Execution recipe

**Question and research.** Does a prepared instrument remain independent and
bounded after prolonged ringing, repeated replacement and concurrent instances?
Read `R-SDK-JUCE-AUDIOPROCESSOR` for lifecycle serialization and the concurrency
chapter for ownership; inspect the exact compiler's official ThreadSanitizer
and AddressSanitizer documentation for supported platforms and exclusions.
Search `shutdown pending publication`, `stalled consumer reclamation` and
`cross-instance mutable state`. Physical consequences include tail truncation,
stale releases and unintended sympathetic coupling between separate instances.

1. Freeze the T01 capacity sheet and T02 generation protocol. Design a duration
   justified by the longest admitted decay and replacement cadence, rather than
   calling an arbitrary short render a soak test. Include late tail re-strikes,
   long sample counters, counter-wrap boundary fixtures, failed/canceled
   preparation and shutdown with outstanding requests. Retain deterministic
   operation sequences and scheduling seeds where a harness uses randomness.
2. Render identical instances in isolated and interleaved schedules, then
   deliberately change one instance's gestures. Compare the untouched instance's
   output and mechanical summaries. Immutable assets may be shared; the
   `Instrument::Impl`, nail/finger state, mapper state and scratch must not be.
   Use `Instrument::snapshot`/`ledger` only from their audio owner; route copies
   through the proposed telemetry protocol for another thread.
3. Run a separate race/memory harness with destructor-thread assertions,
   producer-ahead and stalled-consumer cases. Hosts must serialize operations
   the current API declares single-owner; deliberately racing `prepare` with
   `process` is not a supported usage to normalize. Measure retained/peak
   memory and state generations across many transitions, distinguishing bounded
   caches from monotonic leaks. Repeat the valid stress corpus in unsanitized
   qualification builds for T04 timing.

Retain lifetime traces, per-instance fingerprints, memory curves, fault/energy
trends and all unavailable-tool gaps under `ENG-P13-T02`, `REQ-RTS-001` and
`REQ-RTS-003`. Current `contact.gesture_replay` and `realtime.block_partition`
exercise deterministic finite fixtures; neither proves concurrency or long-run
reclamation. Proposed `realtime.asset_handoff` and
`realtime.telemetry_snapshot` must add independent ownership checks. Reject a
race, cross-instance response change, unbounded backlog or unexplained energy
growth; mark missing platform runs blocked. Hand P14/P16 the host-neutral API,
source/configuration hashes, capacity/resource matrix, independent error report,
actual deadline evidence and explicit unsupported cases. P13's scientific gate
remains open wherever calibrated behavior or independent parity is missing.

> Profile and optimize the identified bottleneck without changing the physical
> contract silently. Inventory callback hazards and prove explicit work bounds.
> Compare identical event replays against the reference, including adversarial
> ringing/contact cases. Demonstrate preparation ownership and retirement under
> stress. Report hardware-qualified latency distributions and numerical error,
> not just average CPU or a faster isolated benchmark.
