# Numerical tests, integration checks, and cross-platform evidence

## What a passing baseline establishes

The baseline suite establishes a limited set of executable properties of the
synthetic reference. It verifies parameter rejection, characteristic frequencies,
free response, energy behavior, initial-shape projection, displacement convergence,
reset semantics, block partition independence, resonator impulse behavior, and
observation endpoints. It also checks the compiled renderer, WAV encoding, CSV
contracts, deterministic report generation, and representative rejection paths.

It does not establish measured setar fidelity, acoustic radiation accuracy,
real-time performance on a particular computer, complete host compatibility,
thread safety under unsupported concurrent access, or the correctness of every
proposed chapter in the handbook. A test name should say what it checks. A passing
result should be quoted at that same scope.

The suite uses a small C++ runner rather than a third-party framework so the
headless dependency closure remains inspectable. Each case is a separate CTest
entry. The runner reports failure through a nonzero exit code and a diagnostic,
not through a debug-only assertion that disappears in optimized builds.

## Independent expectations

An independent test does not mean every mathematical fact is derived from a
different physical theory. It means the expected result is not obtained by
calling the same implementation helper and comparing it with itself. For this
reference, different checks deliberately use different computational routes.

The characteristic-frequency test evaluates the expanded tension and bending
expression in `long double`. The analytic-response test evaluates the solution
at the final physical time directly, whereas production repeatedly applies a
precomputed transition matrix. The pluck-projection test numerically integrates
the piecewise-linear shape with Simpson quadrature, whereas production uses a
closed-form sine coefficient. The integration test derives the observed signal
from physical input metadata without reading production modal frequencies or
amplitudes. The PCM check independently implements the stated quantizer.

On platforms where `long double` has the same precision as `double`, the algebraic
and procedural independence still matters, but the test does not gain extra
mantissa bits. Do not describe it as an extended-precision oracle on every
platform. If a future acceptance criterion needs higher precision universally,
use a deliberately selected offline reference library or exact symbolic result
and document that dependency.

## Test inventory

| CTest name | Independent property | Important boundary |
| --- | --- | --- |
| `core.parameters` | Valid supported domain; rejection of nonfinite, negative, zero, excessive, and derived-invalid inputs | Parameter rejection, not measured parameter plausibility |
| `core.frequencies` | Characteristic equation, modal mass, and harmonic spacing at zero EI | Simply-supported uniform linear model only |
| `core.analytic_response` | Modal displacement and velocity at a known physical time | Unforced underdamped evolution |
| `core.lossless_energy` | Finite-mode mechanical conservation over repeated updates | No damping and fixed modal truncation |
| `core.passivity` | Nonnegative energy and bounded roundoff-scale positive steps | No external work after initialization |
| `core.pluck_projection` | Fourier coefficients from independent quadrature and invalid-pluck transactionality | Finite projection of a sharp triangle |
| `core.convergence` | Decreasing displacement reconstruction error and common-time sample-rate agreement | Not continuum bending-energy convergence |
| `core.reset` | Silent reset, deterministic retrigger, and transactional prepare | Same executable and owned model instance |
| `core.block_partition` | Identical samples across irregular and empty blocks | No concurrent control mutation |
| `core.resonator` | Momentum/energy impulse jump, analytic motion, near-critical limit, and invalid state rejection | One passive scalar oscillator |
| `core.endpoints` | Silent fixed endpoints, zero-height silence, and sign symmetry | Observation can be silent while internal energy is nonzero |
| `integration.report` | Actual executable output, PCM16, independent observation, file identity, SVG/HTML, and malformed-input rejection | Standard-library report, not a browser screenshot test |
| `documentation.audit_self_tests` | Auditor behavior on controlled valid and invalid temporary fixtures | Tests the auditor, not every actual handbook claim |
| `documentation.implementation_traceability` | 41 requirements, 44 work orders, unique IDs, reading paths, coverage, DAG and future-test ownership | Planning consistency, **not** executed future physics tests |
| `documentation.implementation_mutations` | Controlled duplicate, orphan, path, cycle, schema and status/coverage defects are rejected | Mutation tests of the planning auditor, not physical fidelity |

The numerical cases carry `core` and `numerical` labels. The report case carries
`integration` and `report`. Audit self-tests carry `documentation`. Use these
labels to select a diagnostic subset, but run the complete applicable baseline
before proposing an accepted change.

The implementation traceability auditor is also invoked directly by the
portable GitHub Actions headless jobs. Every one of its **79 planned test
identifiers** is a specification for future independent C++/host/measurement
tests; those 79 cases are **not** registered CTests in the present scaffold.
Its test suite checks only that the plan's dependency graph and files remain
internally consistent. Developers must implement each named physical test and
then expand the cross-platform CTest matrix before claiming a new capability.

```sh
ctest --preset headless-debug -L numerical --output-on-failure
ctest --preset headless-debug -R core.analytic_response --verbose
ctest --preset headless-debug -L integration --output-on-failure
```

An intentionally narrow run is useful evidence for the selected question. It is
not a replacement for the omitted tests. Record the selection expression so
another developer can distinguish a subset from a complete result.

## Tolerances and why they differ

Tolerance is part of the model's evidence contract. A relative tolerance is
meaningful away from zero; an absolute tolerance is needed near a zero crossing.
The analytic state checks therefore combine both. Using only relative error for
a near-zero displacement can turn harmless rounding into a large reported ratio.
Using only a large absolute tolerance can conceal a serious error in a small
signal.

The lossless test requires the maximum relative finite-mode energy error to stay
below 1e-10 over its configured 48,000 updates. It reports the observed maximum
rather than merely printing a boolean. This is a bound on the selected fixture,
compiler, arithmetic, and sequence; it is not a universal theorem about all
possible parameter values or arbitrarily long execution.

The damped test allows a positive step of at most 1e-13 times initial energy.
That allowance addresses floating-point effects without normalizing the state.
The report, which parses decimal text and evaluates additional independent
expressions, uses its own documented check tolerances. The report manifest
records them. A plot can look monotonically decreasing while a numerical check
fails; the numerical result governs the conclusion.

The projection quadrature uses a pluck position exactly represented on its
integration grid. Otherwise the triangle's kink could introduce a quadrature
error unrelated to the coefficient implementation. The separate displacement
convergence test samples a dense spatial grid and compares errors at 4, 8, 16,
32, and 64 modes. It does not assert a fictitious energy convergence for the
sharp triangle.

When a tolerance needs adjustment, state the changed source of error, the
reference precision, the observed error distribution, the proposed limit, and
why the limit still detects the intended defect. Include a deliberately broken
case if practical. A test that only accepts the current implementation is less
valuable than a test shown to reject a relevant wrong implementation.

## Renderer and report integration

The integration test runs the compiled renderer into temporary directories. It
renders the same fixture twice and compares the WAV, samples CSV, modes CSV, and
metadata byte for byte. It then parses the WAV with Python's independent standard
library, verifies its channel count, sample width, sample rate, frame count, and
each quantized sample, and compares selected physical samples with a direct
analytic expression.

The report generator is run twice into different destinations. All expected
output files are compared. SVGs are parsed as XML, local HTML links are checked,
required synthetic labels are checked, and every linked report resource must
exist. This establishes structural integrity and deterministic generation. It
does not inspect pixels, browser layout, font substitution, keyboard navigation,
or screen-reader behavior. Those require a real rendered-interface review.

Invalid command-line values include unsupported rates, malformed numeric tokens,
non-finite values, invalid mode counts, impossible positions, excessive slope,
negative damping, and a fixed gain that would clip. Rejected requests must not
create an output directory. An overwrite request must fail without altering the
existing known files. A deliberately non-finite CSV row must be rejected before
the report writes output. A single-sample zero-height fixture exercises empty-
range and zero-energy display behavior.

These are representative adversarial cases, not an exhaustive parser-fuzzing
campaign. Future work can add property-based parameter generation, malformed
UTF-8 and CSV cases, file-system fault injection, integer boundary cases, and
larger data volumes. Such additions should retain minimized failing fixtures
rather than relying on a random seed that nobody records.

## Configured GitHub Actions matrix

The workflow in `.github/workflows/engineering.yml` configures headless builds on
`ubuntu-24.04`, `windows-2022`, and `macos-14`. Each job builds the C++ targets,
runs CTest, audits the handbook, executes a fresh synthetic render, builds the
report, and uploads evidence. A separate Linux job builds and runs the suite with
AddressSanitizer and UndefinedBehaviorSanitizer.

These are workflow definitions. Their presence is not evidence that the jobs ran
or passed. When reporting platform support, link the actual run and commit,
inspect each job's result, and distinguish build/test success from uploaded
artifact availability. A skipped or cancelled job is not a pass. A report from
Linux does not stand in for a missing Windows result.

The workflow uses read-only repository-content permission, does not persist the
checkout credential, and pins external actions to full commit hashes. The
initial action references were checked against official releases and action
input declarations. Updating a version comment without updating the hash has no
effect on the code that runs. Updating the hash without reviewing the release
and its declared inputs is not a complete dependency update.

The workflow deliberately does not grant package publication, repository writes,
secret administration, release creation, or automatic merge authority. A build
does not need those powers. Pull-request tests must continue to avoid executing
untrusted code in a privileged publication context. Adding signing or release
jobs later requires a separate reviewed trust boundary.

## Sanitizer interpretation

AddressSanitizer and UndefinedBehaviorSanitizer detect important classes of
executed defects. They do not prove numerical stability or physical fidelity.
They also do not prove the absence of all race conditions, allocation events,
real-time deadline misses, or incorrect algebra. The numerical tests remain
necessary in an instrumented build.

For a local GCC/Clang experiment:

```sh
cmake --preset asan-ubsan
cmake --build --preset asan-ubsan
ctest --preset asan-ubsan
```

If a platform or runtime prevents a sanitizer process from starting, preserve the
diagnostic and identify the environment restriction. Do not quietly change a
failure to a pass or disable detection. If a sanitizer report identifies a real
source defect, retain the reproducer, fix the cause, and rerun the affected and
full suites. Resource leaks in test setup and real invalid state accesses deserve
separate diagnoses even when they appear in the same job.

A separately labeled narrowed run can still inspect unaffected categories. For
example, if LeakSanitizer cannot inspect process metadata in a restricted runtime,
an explicitly leak-excluded AddressSanitizer/UndefinedBehaviorSanitizer run can
provide evidence for those executed checks. It is not a successful leak check or
a substitute for the unchanged full sanitizer job on a compatible runner.

ThreadSanitizer is not configured for the initial baseline. The core's contract
is single-owner mutation rather than internal synchronization. A future
thread-safe handoff or telemetry path needs a specific race-testing strategy.
Adding a mutex to make a race disappear inside an audio callback would violate a
different requirement and must not be accepted as an unexamined fix.

## Real-time claims and missing instrumentation

The render implementation visibly contains no dynamic container growth, lock,
file I/O, network I/O, or unbounded solver iteration. Its work is proportional to
the validated retained mode count and requested sample count. Preparation and
excitation configuration are outside that render contract. These are current
source-level properties and lifecycle assumptions, not a completed performance
certification.

The baseline does not yet install a platform allocator hook or collect
per-callback execution-time distributions. It does not test scheduling under
CPU contention, device changes, power-management transitions, editor interaction,
or multiple plugin instances. Those experiments need representative hardware,
an explicit block-size/sample-rate matrix, and retained raw measurements. Do not
invent a percentage CPU figure from the fact that the example feels lightweight.

An allocation probe should distinguish preparation from processing and test the
actual host boundary as well as the core. A timing experiment should identify
the clock, warmup, affinity assumptions, competing load, instrumentation
overhead, and tail statistic. Average render speed alone is not sufficient
evidence for a bounded real-time callback.

## Evidence packaging and review

Retain the exact command, relevant environment/tool versions, source revision,
dirty-tree status, exit code, test report, and generated manifest. For a failure,
also retain the smallest input that reproduces it. Screenshots can accompany this
record but should not replace machine-readable outputs. CI artifacts have a
retention policy; an accepted scientific result needs an intentionally durable
record rather than an assumption that a temporary download link will last.

A review should be able to answer four questions without rerunning everything:
what was checked, against what independent expectation, under which conditions,
and with what result. It should also make rerunning possible. If the evidence
cannot answer either side, improve the record before broadening the claim.

The next test work should follow actual risk. A new force input calls for work
balance and forced-response tests. A new state serializer calls for malformed,
truncated, unknown-version, and round-trip cases. A new host boundary calls for
actual host scans and lifecycle tests. Repeating the existing tests under a new
name is not equivalent to covering the new failure mode.
