# Cross-platform tests and continuous-integration evidence

Every engineer is responsible for Linux, Windows, and macOS consequences of a
change. This does not mean pretending to have run systems that are unavailable.
It means maintaining portable tests, examining each required run, and labeling
missing evidence as pending or blocked. A configured GitHub Actions matrix is an
implementation artifact; only observed job results establish execution. A green
documentation check does not establish acoustic fidelity.

## Baseline interface and scope

The numerical scaffold uses these repository interfaces:

```sh
cmake --preset headless-debug
cmake --build --preset headless-debug
ctest --preset headless-debug
build/headless-debug/setar_render --output-dir artifacts/reference --duration 2 --sample-rate 48000
python3 tools/generate_report.py --input artifacts/reference --output artifacts/report
```

The executable path shown is the Unix-style path; use the generated executable
path and `.exe` suffix where appropriate on Windows. Check current CMake presets
and engineering documentation before assuming a configuration-specific layout.
These commands exercise synthetic reference scaffolding. The supplied uncoupled
modal stiff-string and passive resonator do not implement a calibrated setar,
nonlinear fingernail/fret interaction, or measured radiation. Report plots retain
the synthetic label.

The headless build must not require JUCE, an audio device, a display server, or
an implicit dependency download. Optional JUCE integration uses an explicitly
supplied checkout through `SETAR_ENABLE_JUCE=ON` and `JUCE_SOURCE_DIR`; it needs
separate build and validation evidence. A successful headless job says nothing
about an optional plugin target that was not configured.

## Required matrix as a specification

| Platform | Compiler coverage | Core evidence | Separate product evidence |
| --- | --- | --- | --- |
| Linux | GCC and Clang C++20 toolchains | Unit, reference, property, lifecycle harness, reports | VST3/standalone where built; supported display/audio paths |
| Windows | MSVC C++20 toolchain | Same portable suite and path/encoding cases | VST3/standalone, scanner, device, host behavior |
| macOS | Apple Clang C++20 toolchain | Same portable suite on declared architecture | AU and VST3/standalone, native editor, host behavior |

This table is a coverage requirement, not a statement that each cell has run.
Record exact compiler, standard library, operating-system image, architecture,
CMake, build type, and dependency revisions. GitHub runner labels can move;
preserve the observed image identity. Test x86-64 and ARM64 only when they are
declared supported and actually available; do not imply universal-binary testing
from compiling one architecture.

GitHub's [matrix documentation](https://docs.github.com/en/actions/how-tos/write-workflows/choose-what-workflows-do/run-job-variations)
describes job variations. The project should pin third-party actions to reviewed
revisions, retain readable version comments, and review updates deliberately.
Use minimal permissions. Never make untrusted pull-request code run with release
credentials or on a privileged studio machine merely to obtain a green check.

## Test layers and labels

Organize tests by purpose: mathematical unit tests, independent reference tests,
generated properties, state/queue semantics, allocation checks, long-run numeric
stability, report integrity, plugin lifecycle, visual interaction, and empirical
analysis. Proposed labels such as `core`, `reference`, `realtime`, `plugin`, and
`visual` are conventions until present in CTest; inspect the actual test list
instead of documenting nonexistent selectors as working commands.

Fast deterministic tests belong in the ordinary contribution path. Expensive
convergence sweeps, controlled-hardware performance, and listening studies need
separate explicit invocation and evidence. They remain gates for relevant claims
even when not suitable for every hosted runner. A skipped job must state the
reason and impact. Do not use broad path filters that skip physics tests whenever
a parameter or fixture changes outside a source-code directory.

Require a failed-fixture artifact when feasible. The test should print a stable
case identifier, seed, parameter values, expected condition, observed value, and
reproduction command. Avoid log dumps of private paths, participant identities,
or licensed data. Preserve the minimal relevant evidence without exposing secrets.

## Numerical reproducibility without false bitwise promises

Deterministic means a declared initialization and event sequence reproduce a
result under a defined execution environment. It does not automatically mean
bit-identical samples across libm implementations, compilers, architectures, or
floating-point contraction settings. Transcendental functions, fused operations,
vectorization, and reduction ordering can differ while remaining acceptable.

Use exact comparison for integers, identifiers, event order, serialized schema
fields, and deliberately exact mathematical cases. Use justified absolute and
relative tolerances for numerical results. Use feature or norm comparisons for
long trajectories where roundoff sensitivity makes per-sample equality unsuitable.
Do not widen a tolerance until every platform is green without investigating
the magnitude, pattern, and physical relevance of the difference.

Record floating-point flags and denormal policy. Reference builds should avoid
unreviewed fast-math transformations. Production optimizations require comparison
against the verified baseline, including passivity, finite outputs, and physical
observables. A bitwise checksum of a waveform can detect local regression, but
should not become a cross-platform oracle without a deliberately controlled
numeric implementation and a stated reason for that cost.

Test block-partition invariance using the same absolute event times. Distinguish
acceptable roundoff from a semantic difference such as applying automation once
per host block. For a chaotic or highly sensitive nonlinear regime, define
appropriate invariant or statistical comparisons and bound the claim; do not
declare all divergence harmless merely because the model is nonlinear.

## Sanitizer lanes

Clang [AddressSanitizer](https://clang.llvm.org/docs/AddressSanitizer.html) detects
important memory errors. [ThreadSanitizer](https://clang.llvm.org/docs/ThreadSanitizer.html)
targets data races and has platform and instrumentation constraints.
[RealtimeSanitizer](https://clang.llvm.org/docs/RealtimeSanitizer.html) checks
instrumented nonblocking contexts for known unsafe calls. Microsoft's
[AddressSanitizer documentation](https://learn.microsoft.com/en-us/cpp/sanitizers/asan)
describes MSVC support. Verify support in the pinned toolchain; do not assume
all sanitizers exist on all three platforms.

Run compatible sanitizer combinations in separate builds. ASan/UBSan and TSan
should not be casually combined into one universal configuration. A TSan harness
should model engine/UI/worker concurrency with adequate instrumentation; loading
an instrumented plugin into an uninstrumented proprietary host may obscure
interpretation. Keep suppressions narrow, explained, and reviewed. A suppressed
third-party race is not permission to suppress the project's entire wrapper.

Sanitized timing is not a production CPU benchmark. Conversely, a fast release
build does not prove absence of races or allocation. Preserve each lane's purpose.
Allocation interception and real-time sanitizer tests cover executed paths, not
all possible paths; combine them with call-graph review and adversarial fixtures.

## Artifacts, retention, and reports

Use unique artifact names containing job identity, compiler, architecture, build
type, and candidate revision. Preserve test logs, manifests, numeric results,
plots, and failure screenshots. GitHub's [artifact documentation](https://docs.github.com/en/actions/concepts/workflows-and-actions/workflow-artifacts)
and [store/share guide](https://docs.github.com/en/actions/tutorials/store-and-share-data)
describe upload/download and digest facilities. Verify downloaded evidence against
its manifest instead of trusting a filename.

Retention is part of evidence policy. Short-lived CI artifacts alone may be
insufficient for a long-lived scientific claim. Archive approved evidence through
an authorized project mechanism with licensing and privacy controls. Do not
upload recordings whose permissions permit local analysis but not redistribution.
Reports should display `not run` when empirical inputs are absent, never generate
plausible replacement measurements to fill the page.

## Manual and self-hosted checks

Hosted runners are suitable for many builds and automated harnesses but do not
prove real audio-device operation or every DAW interaction. Maintain a manual
host matrix with exact host/build/format/architecture and test script. Self-hosted
machines can supply controlled hardware evidence, but require explicit ownership,
access controls, and isolation from untrusted code. GitHub's
[self-hosted runner reference](https://docs.github.com/en/actions/reference/runners/self-hosted-runners)
is the platform source; project security policy determines allowed deployment.

Never install commercial hosts, change licenses, publish packages, or alter
repository protections as an implicit part of testing. Obtain the relevant
authorization. A manual result is valid when documented and reproducible; it is
not less honest than an automated result, but its coverage and operator
dependencies must be visible.

## Acceptance and engineer responsibility

For each change, list affected layers, required platform jobs, observed outcomes,
artifact links, and missing checks. Review warnings rather than only exit status.
The cross-platform gate passes only when required executions and artifacts exist.
Acoustic and listening gates remain independent. The next action after a platform
failure is a minimized reproducer and a justified diagnosis, not disabling the
platform or changing the comparison until it passes.
