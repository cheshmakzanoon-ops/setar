# Deriving realtime and fidelity budgets from evidence

**No fixed CPU percentage or psychoacoustic tolerance is claimed by this
document.** Budgets are requirements chosen for a declared platform, model
configuration, performance context and evidence level. The current synthetic
reference has no qualified plugin/audio-device worst-case measurements. It is
incorrect to convert “Pianoteq-level” aspiration into a fabricated universal
numerical threshold or CPU benchmark.

## Budget hierarchy

There are five separate budgets, each with its own comparator:

1. **Physical parameter uncertainty**: measured or inferred distribution of
   geometry, wood, string properties, bridge mobility, finger/nail contact.
   Evaluate sensitivity and identifiability; do not force an uncertainty-free
   point value into a calibrated asset.
2. **Numerical approximation error**: spatial, time, modal truncation, contact
   iteration, multirate transport, observation reduction. Derive a reference
   convergence/energy envelope at fixed *physical* parameters.
3. **Acoustic prediction error**: physical quantity versus independently
   acquired pressure/FRF/gesture data after matching conditions and noise.
   This includes model inadequacy distinct from numerical solver error.
4. **Perceptual/expressive criteria**: detectable timbre/attack/articulation,
   practitioner gesture response and usability. These need controlled listening
   or playing studies, not assertions that a dB/cent bound is inaudible.
5. **Computational/scheduling envelope**: latency, finite work, callback
   execution, peak scratch/state memory, worst-case contacts, portability and
   safe degradation. This is engineering qualification, not physical truth.

Do not allow success in one dimension to cancel failure in another. A fast
engine with incorrect contact energy does not qualify. A correct offline solver
that misses device deadlines is not a realtime instrument. A loudness-matched
spectral fit does not validate physically coupled bridge behavior.

## From host sample rate to a deadline

For a block of \(B\) samples at rate \(f_s\), the audio interval is
\(D=B/f_s\) seconds. For example, **64 frames at 48 kHz have an arithmetic
duration of about 1.333 milliseconds**; that is not an assertion about
available CPU time. The callback shares resources with host mixing, other
plugins, OS scheduling, devices and background work. To choose a defensible
maximum engine budget:

1. Declare minimum/maximum actual host frame counts, sample rates, supported
   CPU architecture, instance count, mode/contact capacities and quality mode.
2. Choose target host/DAW and hardware classes. Include representative
   low-power machines, not just one fast development desktop.
3. Measure an uninstrumented optimized build under attack/restrike/fret storm,
   maximum sympathetic sustain, difficult nonlinear contact, asset transition,
   silence/denormal tails, and realistic simultaneous plugin instances.
4. Record CPU frequency/power mode, OS scheduler, thread priority, compiler,
   optimization, SIMD feature, instance count, memory pressure, sample rate,
   block size, exact fixture hashes and profiler overhead.
5. Report median, high-tail distributions, **largest observed** callback
   duration, missed deadlines/xruns, sample count and confidence/coverage.
   Percentiles are summaries, not hard worst-case proofs.
6. Select a conservative budget below the host's available interval using the
   observed variability and required system headroom. Document this as a
   *proposed product requirement* and review after new platforms or physics.

A source-level asymptotic bound is valuable but not a scheduling proof. A
nonlinear solver with a hard cap \(N\) has bounded iterations, yet a bad
allocation or platform-denormal event can still produce a deadline miss. Use
source audit, allocator/lock instrumentation and machine timing together.

## Required stress grid before choosing limits

At minimum design experiments that vary the following *axes*; exact supported
values are decided by product requirements and actual host capabilities:

| Axis | Discriminating cases | What to record |
| --- | --- | --- |
| Sample rate | Common rates such as 44.1, 48, 96 kHz; all declared supported rates | Internal step, antialias mode, reported latency and cost |
| Block size | Zero/one frame, small/normal/large blocks, irregular partitions | Callback time, event timing, state checksum, work amount |
| Physical activity | Silence, decay, dense nail retriggers, fast fret motion, sympathetic response | Active solver dimensions, nonlinear iteration histogram |
| Contact stiffness | Weak to strongest admissible contact, overlapping contacts | Residual, penetration, failures, iteration cap |
| String tuning/material | Extremes of validated tension/stiffness/loss domain | Pole stability, required numerical resolution, alias energy |
| Observation | Single mic, stereo/directivity, optional room layer | Filter cost, group delay, gain normalization policy |
| Instance count | Single and stressed multi-instance sessions | Aggregate xruns and per-instance state/asset isolation |
| Concurrency | Asset swap, GUI telemetry, preset reload, editor open/close | Allocation, reclamation, stale snapshot and jitter |
| Host/OS | Declared Windows/macOS/Linux/DAW combinations | Actual binary/host version, scheduling environment |

Do not call all extremes “worst case” without measuring combinations. For
example, maximum fret count plus high damping may be cheaper than fewer very
stiff simultaneous contacts that consume every nonlinear iteration. Determine
the maximum accepted combination empirically and by operation-count analysis.

## Numerical error budget allocation

Before optimization, select primary measurable observables:

- At each string: displacement and velocity at selected spatial points;
  tension/stress when implemented; stored kinetic and elastic energy.
- At each contact: compression/penetration, force, tangential state, release
  time, external work and dissipated work.
- At bridge/body: three-axis force and velocity port histories, complex
  multiport mobility, individual modal energy and cross-string transfer.
- At acoustic observation: pressure impulse response, calibrated microphone
  position/directivity, time alignment, frequency/decay and separate room gain.

For each observable, estimate independent reference-solver uncertainty and
measurement uncertainty. Then define a candidate error limit using physical
significance and planned listener/performer tests. Never allow the optimized
engine's output to become the only oracle. Where reference resolution changes,
compute an error-versus-resolution plot; where model reduction changes,
compute rank-versus-error/CPU Pareto curves.

Typical distinctions that must appear in the report:

- Low-frequency pitch error in cents is meaningful away from missing/misidentified
  partials; it does not test attack physics or upper harmonics.
- Spectral log-magnitude error depends on analysis window, noise floor and
  calibration. Phase/time alignment affects mechanical transfer and transients.
- Contact release timing should be tested in physical time before comparing
  audio envelopes; a one-sample error has different significance across rates.
- Body FRF fitting residuals need magnitude, unwrapped phase, coherence and
  uncertainty. A matching modal frequency with wrong bridge residue is not a
  passing mechanical model.
- A stereo microphone impulse response is observation. Do not normalize it to
  hide a too-quiet body response or change it in a passivity test.

The numerical work ledger can use a normalized defect
\(r_E=|\Delta E-W_{\rm ext}+D|/E_{\rm scale}\), provided
\(E_{\rm scale}\) is positive, documented and chosen independently. Retain the
unscaled defect in joules for small-signal and near-zero cases. A tolerance can
combine roundoff, solver residual, and independently estimated truncation
error; do not simply choose the largest error from an unverified baseline.

## Parameter/budget domain record

Before calling any version realtime-qualified, freeze a machine-readable
configuration stating:

| Field | Required entry | Acceptance owner |
| --- | --- | --- |
| `reference_model_id`, `prepared_asset_sha256` | Source and calibrated asset identity | Numerical/acoustic |
| `solver_variant`, `scheme_version` | Time levels, coupling method and reduction | Numerical |
| `f_s_range`, `block_size_range` | Supported host input domain | Plugin/QA |
| `max_strings`, `max_modes_per_string` | State/memory envelope; four physical strings for target model | Engine/physics |
| `max_active_contacts`, `max_iterations` | Worst-case work and failure behavior | Contact/realtime |
| `max_events_per_block` | Overflow/release priority; no stuck-contact state | Engine/plugin |
| `internal_rate_factors` | Multirate placement, resampling and group delay | Numerical/realtime |
| `scratch_bytes`, `state_bytes`, `asset_bytes` | Fixed capacity and memory ownership | Software/QA |
| `callback_budget_s`, `latency_samples` | Derived target, measured actual and reported host latency | Plugin/realtime |
| `audio_output_policy` | Float/double conversion, safety containment and channel mapping | Plugin/acoustics |
| `numerical_tolerance_manifest` | Units, reference, derived/measurement/proposed category | Scientific reviewer |
| `qualification_matrix_id` | Actual machine/run/host evidence used to support claims | QA/release |

Do not commit values into the stable schema merely to fill a mandatory field;
an unset or unknown budget blocks promotion. Provide a clearly marked
*synthetic development profile* so tests can run before all measured values
exist, and ensure the release loader rejects that profile for measured claims.

## Real-time source audit and instrumentation

Instrument **all** paths that can execute inside the callback: primary render,
event translation, contact solver, asset adoption, stale asset disposal,
realtime parameter change, bypass transition, safety containment, telemetry
publication, output conversion, denormal handling. Do not limit inspection to
the main inner oscillator loop.

On each platform, use available profiling and allocation interception to test
for `new/delete`, allocator calls, mutex/waits, thread creation, filesystem
and logging, exceptions and dynamic type erasure. Some platform allocators and
system libraries are outside a custom C++ global `new` hook, so pair the hook
with source review and platform instrumentation. Run sanitizers on compatible
separate configurations; their slowdown is not a representative realtime
benchmark. Run thread-race tests on handoff paths only with compatible tools;
absence of one tool is a documented coverage limitation, not a proof of safety.

Fast-math, vector instruction changes, flush-to-zero and mixed precision
require their own physical parity tests. Measure floating-point drift after
long decays and many contact events. A denormal slowdown can cause an xrun even
on an otherwise silent fixture; treat that as a performance defect distinct
from audible output quality.

## Accepting, failing and revising a budget

An acceptance decision contains exact tested revision, asset hash, OS/CPU,
compiler and flags, host, output rate, buffer pattern, instrument activity,
test duration/coverage, measured callback distribution, deadline misses and
what is not covered. Failure is informative: reduce rank, change scheme,
precompute, narrow an explicitly chosen quality mode, or modify the supported
hardware envelope—but first compare the changed physical observables to the
offline oracle.

Product budgets are not timeless physical constants. Reopen them through a
documented decision when new setar measurements, playing gestures, OS or host
versions, or reduced-model choices change the evidence. Do not secretly relax
tests to make CI green. Connect acceptance to `REQ-RTS-001/002/003` and
`ENG-P13-T01/T02` in the [registers](../../roadmap/engineering-tasks.json).
