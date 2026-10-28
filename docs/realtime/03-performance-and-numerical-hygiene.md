# Performance budgets and floating-point hygiene

“Low CPU” is not a reproducible requirement. A useful performance statement names
hardware, operating system, power state, host, build, sample rate, block behavior,
workload, and measurement method. It reports the distribution and exceptional
cases, not just an average. This chapter specifies how to establish a real-time
budget without turning a noisy hosted-runner measurement into a hardware promise.

## Budget the complete path

For a block of (N>0) samples at sample rate (f_s), its nominal audio duration
is (N/f_s). Compare callback execution with that duration, but recognize that
the host and operating system also consume scheduling and processing resources.
A project may propose a maximum occupancy fraction with headroom; the fraction
must be justified on declared reference hardware and is not universal. Do not
divide by zero for a zero-sample callback.

Budget event translation, physical updates, radiation, output conversion,
telemetry, and any transition crossfade. A benchmark of the inner modal loop
omits costs that may dominate small blocks. Include preparation and state-load
costs in separate responsiveness measurements rather than hiding them inside
the audio budget. Memory capacity, stack use, and cache footprint are also
resources that need declared bounds.

Measure the highest supported configuration, not only a default patch. Include
maximum active strings or virtual instruments, strongest admissible contact,
maximum event density, longest supported tails, oversampling if present, and
simultaneous state transitions. Some worst cases occur during quiet tails when
subnormal arithmetic appears, not during the loudest attack.

## Two benchmark classes

A microbenchmark isolates a component to explain cost and compare implementations.
An integration benchmark exercises the full callback under a host-like workload.
Both are needed. A faster inner loop can increase total cost if it requires
frequent coefficient conversion or causes worse cache behavior. A complete
benchmark without component attribution can reveal a regression but not explain it.

Use a deterministic workload manifest with exact event sequence, parameters,
random generator, and output checks. Ensure the compiler cannot eliminate the
work. Verify the rendered result or a bounded summary after timing. Warm-up
policy, repetitions, clock source, and measurement overhead must be documented.
Separate cold-start behavior from steady-state behavior instead of discarding
it without explanation.

For callback timing, collect samples in preallocated storage or bounded bins,
then analyze outside processing. The measurement mechanism must not add locks
or logging to the path being measured. Calibrate its overhead and report whether
the measurement includes wrapper work and host scheduling.

## Distribution and outliers

Report median, selected upper quantiles, maximum observed value, sample count,
and occupancy relative to block duration. A percentile is not a proof of worst-
case execution time. The observed maximum depends on how much and what was tested.
Keep outliers visible and investigate whether they arise from code paths,
scheduling, page faults, throttling, device behavior, or instrumentation.

Do not remove outliers merely because they spoil a graph. If an external event
invalidates a run under the registered protocol, retain the run with its exclusion
reason and repeat under the same rule. A controlled machine can support a
stronger comparative claim than a shared hosted runner, but still needs honest
limits. Real hosts with multiple plugins add contention absent from a standalone
microbenchmark.

Record CPU model, architecture, core configuration, memory, operating-system
build, relevant power mode, compiler flags, and host/device settings. Do not
silently change system power policy, affinity, or privileged scheduling settings
on a user's machine. Such changes require authorization and belong in the
benchmark context. Results obtained under them should not be presented as
ordinary default-user performance.

## Cross-platform performance interpretation

Use hosted CI for gross regressions and workload correctness, with conservative
interpretation of noisy timing. Use designated reference machines for repeatable
budget decisions. Compare a candidate against a control build on the same
machine under the same protocol. Ratios can reduce some noise, but do not remove
thermal, scheduling, or workload-order effects.

Do not infer Windows performance from Linux, ARM64 from x86-64, or one host from
another. Document compiler vectorization and floating-point settings. If an
optimization depends on a particular instruction set, provide a supported
fallback and test dispatch. Unsupported instructions must not make a plugin
crash during scanning on a machine outside the intended target.

## Subnormals and silence

Long physical decays can enter the subnormal range. Determine the core's policy
for flush-to-zero and denormals-are-zero behavior on each target. A scoped wrapper
can preserve and restore the calling thread's floating-point environment. JUCE
provides [ScopedNoDenormals](https://docs.juce.com/master/classjuce_1_1ScopedNoDenormals.html)
as a platform helper; verify the pinned implementation and architecture behavior
rather than assuming one macro solves every numerical question.

Test the tail with and without the chosen policy and quantify its effect on
observable output and energy accounting. Do not add arbitrary noise to conceal
subnormal cost without documenting its acoustic and reproducibility impact.
If state is terminated below a threshold, define that threshold physically or
relative to a justified numeric floor, and test that re-excitation remains
correct. A quiet output can coexist with internal energy in a weakly radiating
mode, so output amplitude alone is not always a valid sleep condition.

Silence optimization requires a state predicate. All inputs being zero does not
mean a resonant instrument is silent. Include pending events, string/body state,
sympathetic energy, and release transitions. The predicate must not create a
discontinuity when a small state is reactivated. Preserve deterministic behavior
under block partition and sample-rate changes.

## Finite values, precision, and accumulation

Check coefficients at preparation and use bounded runtime checks at appropriate
places. NaNs can propagate through comparisons without triggering ordinary range
checks. Do not rely on `value < limit` alone to validate a floating-point input.
Choose explicit finite checks and a documented invalid-state path.

Decide where float and double precision are used. Double internal state can
improve long-decay accuracy but costs memory and possibly throughput; float can
be adequate only within a demonstrated error budget. Host double-precision
support is a wrapper contract, not proof that every internal operation is double.
Test conversion and accumulation at channel mixing boundaries.

Reduction order and fused operations affect roundoff. Preserve a verified
baseline and compare physically meaningful observables before enabling fast
math, approximate transcendental functions, or aggressive vectorization. A
speed improvement that alters passivity, tuning, or damping beyond the budget
is not an acceptable optimization. Keep the reference path readable even when
the production path becomes specialized.

## Adaptive quality and overload

If quality can change with load, define the policy in advance. Changing modal
count, oversampling, or contact resolution can change physical state and timbre.
Do not silently drop high modes whenever a callback runs late. Transitions need
state mapping, energy accounting, audible-artifact tests, and reproducibility
semantics. Offline rendering must declare whether it follows the same policy or
a fixed quality mode.

Prefer an explicit supported quality setting with predictable cost before an
opaque automatic scheme. When overload protection is necessary, report it and
provide a recoverable state. A failed budget is evidence to reduce supported
configuration, improve algorithms, or revise hardware requirements, not a reason
to fabricate a favorable average.

## Acceptance artifacts

Deliver workload manifests, output checks, raw timing data, summary distributions,
hardware/host context, memory bounds, numerical comparisons, and identified
outliers. Proposed occupancy thresholds remain proposed until supported by the
chosen hardware and host envelope. The next action is the smallest optimization
whose measured cost and numerical effect can both be explained. Re-run the
full callback workload after a local optimization; component speedups are not
automatically product speedups.
