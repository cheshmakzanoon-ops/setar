# P02 — Establish the reproducible C++ reference laboratory

**Status:** limited bootstrap implementation plus broader specification.
**Prerequisites:** P00 and P01 conventions. **Exit evidence:** reproducible
cross-platform builds, independent reference tests, reports, and traceable
experiment manifests. Passing this gate does not validate instrument fidelity.

## Objective and reading

Create a place where physical hypotheses can be implemented, falsified, compared,
and reviewed without a DAW or audio device. Read [engineering](../engineering/README.md),
[mathematics](../mathematics/README.md), and [validation](../validation/README.md).
Inspect the actual source files to distinguish the current analytic/synthetic
reference from the future spatial setar model.

The laboratory must support raw numerical output and human-readable diagnostics.
A WAV file alone is insufficient: clipping, normalization, or microphone scaling
can hide a state-energy error. Conversely, a data table without an audible or
visual interpretation makes regression review needlessly difficult. Preserve both.

## P02-T01: freeze the build contract

Use CMake with a C++20 standard requirement, out-of-source builds, clear options,
and no implicit external downloads in the headless default. Document supported
compiler families and configure/build/test commands. If Ninja requires a
developer environment on Windows, say so; do not assume a Unix shell discovers
MSVC automatically. Use a path with spaces in at least one portability test.

Expose an optional JUCE integration path separately from the core. Require an
explicit upstream checkout/version and license review. Configure-time failures
should name missing prerequisites rather than silently disable requested tests.
Avoid feature-detection that makes one platform run a different scientific model
without reporting the difference.

Test a clean configure, incremental build, test discovery, installation if
provided, and an invalid option. Ensure a no-audio-device environment can execute
the numerical suite. Record actual compiler and standard-library versions in
artifact metadata. Do not demand reproducible binary hashes before the build
system and dependency paths have been designed for that property.

## P02-T02: define a minimal executable experiment

The initial experiment may be a synthetic modal string or passive oscillator.
It must state its governing equation, boundary assumptions, chosen parameters,
initial condition, and output variable. Use physically admissible values chosen
for transparent tests, not values labeled “measured setar” without provenance.

Produce a sample stream, mode/state metadata, and explicit quantity units. If
mapping physical displacement or velocity to PCM, state the gain and refuse
unintended clipping instead of automatically normalizing every render. The
normalized sound of an unstable model can be deceptively convincing. Keep
normalization for listening comparisons in a separate analysis operation with
its own manifest.

CLI arguments need validated ranges and deterministic failure exit codes.
Reject invalid sample rates, nonfinite physical values, impossible geometry,
negative damping, unsupported configuration, and accidental output overwrite.
Do not allow a typo in a requested output directory to erase prior experiments.
Reference state should be reproducible after reset.

## P02-T03: implement independent unit tests

Choose analytic cases before coding the model. A passive oscillator can be
compared to its closed-form transition, energy decrease, and lossless limit.
A modal stiff string can be checked against independently evaluated partial
frequencies and known boundary zeros. State tests should exercise reset,
prepare-failure transactionality, block partition invariance, and zero-length
processing.

Use tolerances appropriate to the quantity. Unit tests should not rely entirely
on byte-identical WAV files across compilers. Compare physical errors and exact
integer metadata separately. Add negative tests for inputs outside the declared
domain. A test suite composed only of “does not crash” checks is not numerical
verification.

Tests should run with assertions enabled as well as in an optimized build.
Avoid depending on the C/C++ `assert` macro for test failure reporting when a
Release build can remove it. Report the failed quantity, expected value,
observed value, tolerance, and fixture identity.

## P02-T04: create scientific reports and real visual captures

Generate plots from the experiment's saved CSV/JSON or equivalent typed data,
not from a separately reimplemented signal formula. Plot amplitude/state,
energy or decay where meaningful, mode spacing, and residual/error measures.
Include axes, units, sample rate, synthetic/measured status, and the source
experiment identifier. Make files portable: no absolute workstation paths in
HTML, SVG, or manifests.

A report generator should reject malformed headers, nonfinite numbers, empty
series, and inconsistent time axes. Escape data strings before inserting them
into HTML/SVG. Test report generation with a tiny known fixture and with invalid
inputs. An optional screenshot step should open the actual report, wait for
layout/fonts, and capture deterministic desktop/mobile or native-window states.
Do not fabricate a future plugin screenshot to satisfy a visual requirement.

Screenshots complement numeric checks. They expose unreadable axes, clipping,
broken layout, and missing warnings. They cannot certify a differential equation.
Store a small representative visual in documentation and upload full diagnostics
as CI artifacts with an explicit retention policy.

## P02-T05: exercise all three operating-system families

Create GitHub Actions jobs for Linux, Windows, and macOS using pinned action
commits and read-only repository permissions. Each job configures, compiles,
executes CTest, renders a deterministic fixture, generates a report, and uploads
diagnostics. Unique artifact names must encode platform/configuration so parallel
jobs cannot overwrite one another.

Add sanitizers where supported and document platform differences. Run safety
jobs with representative numerical and input-validation tests, not only an empty
program. Avoid untrusted fork execution in privileged contexts. Do not configure
release signing, secrets, repository administration, or external publication in
this phase.

Record successful run URLs and exact commit identities. A workflow YAML file is
only a configuration until GitHub actually executes it. Failed or skipped jobs
must remain visible in the handoff. If a platform cannot run an optional graphics
test, preserve the headless coverage and identify the missing visual evidence.

## P02-T06: prepare experiment reuse

Write an experiment template that captures hypothesis, model/configuration,
input provenance, expected results, command, metrics, artifact paths, and
limitations. Separate source files from generated outputs. Avoid committing
large unlicensed audio or nondeterministic build folders. The documentation
auditor should check local links and dependency-graph consistency and report
actual prose count without treating word count as correctness.

## Exit gate and next step

A new contributor must reproduce the bootstrap from a clean checkout, interpret
its outputs, and explain why it is not yet a setar. All three platform jobs must
report their actual outcomes. The independent reference tests become P03's
baseline; measurement work may proceed in parallel without contaminating the
synthetic fixture identities.

> Implement or repair one laboratory capability under P02. Preserve the
> dependency-free headless build. Show the exact test that fails before your
> change and passes afterward, or justify a new independent expected result.
> Produce machine-readable output and one useful plot if the quantity warrants
> it. State platforms actually exercised, test artifacts, and remaining gaps.
> Do not advertise the reference fixture as a finished instrument.
