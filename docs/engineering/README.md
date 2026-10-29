# Engineering laboratory: build, inspect, and extend the reference

## Scope and present capability

The executable portion of this repository is a deliberately small numerical
reference, not a completed setar. It supplies a C++20 library, an offline renderer,
independent numerical tests, a deterministic report generator, and configured
cross-platform checks. An optional JUCE boundary demonstrates how the same core
could be reached from a host or standalone application. The optional target is not
part of the default tested dependency closure.

The reference simulates one polarization of a uniform, simply-supported, linear
stiff string using a finite set of uncoupled sine modes. Each mode evolves by the
closed-form underdamped free-response transition. A finite sine projection of a
triangular initial displacement excites it. The observation is displacement at a
chosen point; a fixed, explicit gain converts that displacement to a mono PCM16
WAV. No recording, sample library, soundboard model, microphone response, or
reverberation is hidden in that output.

The default parameters are constructed examples. Their units and equations are
meaningful, but their presence in a compilable program does not make them
measurements of any instrument. The filenames and UI repeat this distinction
because a useful development artifact can otherwise become misleading when
detached from its original discussion.

This volume covers what a contributor can actually build. Read
[the model contract](reference-model.md) before altering equations,
[the test contract](testing-and-ci.md) before changing tolerances, and
[the artifact contract](artifacts-and-reproducibility.md) before treating a graph
or WAV as evidence. The [JUCE boundary](juce-boundary.md) describes the optional
diagnostic and the substantial host work still required.

To begin a **new** subsystem rather than reproduce the current bootstrap,
read the [implementation-level specification](implementation-specification.md),
[independent verification matrix](verification-matrix.md), and
[performance/accuracy budget derivation](performance-budget.md). Newly
executable prototype contracts and their explicitly partial acceptance
status are documented in
[the numerical handoff](10-prototype-contracts-and-test-expansion.md).
The
[requirements](../../roadmap/requirements.json) and
[work-order register](../../roadmap/engineering-tasks.json) are machine-checked
plans; only the named executable subset has been implemented.

## Prerequisites

The default path needs CMake 3.25 or newer, a C++20 compiler with the normal
standard library, Python 3.10 or newer, and a native build tool. The checked-in
presets choose Ninja. A direct CMake configure can instead use Makefiles, Visual
Studio, or another supported native generator. CMake 3.25 is the repository's
minimum because the preset format and project contract are tested together; it
is not a claim that an older compiler or CMake release could never compile any
individual source file.

The headless core does not need an audio device, desktop session, JUCE checkout,
internet connection, package-manager download, graphics library, or third-party
test framework. Python is only used by offline tests and reports. It is neither
linked into the numerical library nor called from an audio callback. A library
consumer can disable tests and the renderer to remove that development-tool
requirement entirely.

Use a compiler installation intended for the target architecture. On Windows,
Ninja does not discover and initialize a Visual Studio development environment by
itself. Run the Ninja presets from an appropriate developer shell, or use a
Visual Studio generator. On macOS, distinguish the compiler architecture, the
deployment target, and the architecture of a future plugin host. Compiling a
headless executable on one architecture does not demonstrate that a universal
plugin bundle works on both architectures.

Before building, inspect `git status --short`, read `AGENTS.md`, and preserve
unrelated changes. Do not solve a failing configure by deleting the repository or
resetting somebody else's branch. A new build directory is normally enough to
separate a generator or compiler experiment from an existing configuration.

## Reproduce the default development build

From the repository root, with CMake and Ninja on the executable search path:

```sh
cmake --preset headless-debug
cmake --build --preset headless-debug
ctest --preset headless-debug
python3 tools/audit_docs.py
python3 tools/audit_implementation.py
```

The configure preset writes into `build/headless-debug`. The build produces the
`setar_reference` library, `setar_render`, and `setar_core_tests`, plus the
`setar_numerical_prototype` library, `setar_compare_solvers`, `setar_loss_study` and
`setar_numerical_tests` when numerical experiments are enabled. CTest invokes
named numerical cases separately, the renderer/report integration test, and the
documentation-audit self-tests, numerical prototype cases, evidence validator
mutation tests, the solver-plot integration test and the passive-loss evidence test. The
loss tests have a manual defect-injection companion, `tools/mutation_check_loss.py`. The handbook audit is an additional repository
check: the audit's self-tests show that the auditor detects its intended defects,
whereas running the auditor on this tree checks the actual handbook.

On systems whose Python executable is named `python`, substitute that command in
manual report and audit invocations. CMake discovers an interpreter through its
Python3 package support and supplies the resolved executable to CTest, so the
CTest command itself does not need that substitution.

To inspect which tests would run without executing them:

```sh
ctest --preset headless-debug --show-only
ctest --preset headless-debug --show-only=json-v1
```

A successful configure is not a successful build. A successful build is not a
successful test run. A successful test run is not acoustic validation. Record
each of these separately, including the exact revision and whether there were
uncommitted changes. Do not use a screenshot of a green terminal as the only
record when the command output and CTest results can be retained directly.

## Direct native-generator alternatives

A Linux or macOS contributor without Ninja can use the platform's default
generator:

```sh
cmake -S . -B build/native-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build/native-debug --parallel 2
ctest --test-dir build/native-debug --output-on-failure
```

For a Visual Studio installation on Windows, a multi-configuration build is
often more convenient:

```powershell
cmake -S . -B build/windows-native -A x64
cmake --build build/windows-native --config Debug --parallel 2
ctest --test-dir build/windows-native -C Debug --output-on-failure
```

In this configuration the renderer normally lives under the configuration
subdirectory, for example `build/windows-native/Debug/setar_render.exe`. Use the
actual generated target location rather than assuming the Unix executable path.
The `--config` and `-C` options matter for multi-configuration generators; setting
`CMAKE_BUILD_TYPE` does not select their build configuration.

When selecting a different compiler, use a fresh build directory and record the
compiler's full version string. A CMake cache contains compiler and platform
decisions; repurposing it can create confusing failures unrelated to the source.
Do not add forced cache values to the repository merely to make one workstation's
installation discoverable. Put workstation-specific choices in an ignored
`CMakeUserPresets.json` or supply them on the configure command line.

## Produce an inspectable run

The default two-second reference experiment is:

```sh
build/headless-debug/setar_render --output-dir artifacts/reference --duration 2 --sample-rate 48000
python3 tools/generate_report.py --input artifacts/reference --output artifacts/report --source-revision working-tree-experiment
```

Open `artifacts/report/index.html` in a browser. The generated page is an offline
report with local SVG graphics, a local WAV, local metadata, and a hash manifest.
It does not need a server or remote fonts. The waveform displays the first 25 ms
of actual exported displacement. The energy graph spans the exported run. The
dispersion graph compares exported natural frequencies with the tension-only
frequencies derived independently from the same stated inputs.

Choose a new output directory for each experiment. Both the renderer and report
generator refuse to overwrite their known output files. This makes a common
mistake visible: rerunning with different parameters while retaining a graph
whose provenance appears unchanged. If a previous run is intentionally obsolete,
archive or remove that exact generated directory yourself after checking its
contents. Never solve an overwrite error with a broad recursive deletion.

The `--source-revision` value is a caller-supplied provenance identifier. Use the
actual commit for a clean committed run. If the source is modified, label it as a
working-tree experiment and retain the patch separately. Passing a commit hash
does not prove that the binary was built from that commit; the surrounding build
record must establish that relationship. The report's input hashes establish
the specific data it consumed, not an authenticated chain from source to binary.

The fixture is intentionally audible but is not normalized. At the default
height and gain, its PCM level remains below full scale. A different height or
gain can exceed that limit; the renderer rejects the request before writing any
audio rather than silently clipping or adjusting it. Lowering an observation gain
is an explicit presentation change, not a fix for physical instability.

## CMake options and their interactions

| Option | Default | Meaning |
| --- | --- | --- |
| `SETAR_BUILD_TESTS` | `ON` | Build and register the numerical CTest executable. |
| `SETAR_BUILD_RENDERER` | `ON` | Build the offline `setar_render` executable. |
| `SETAR_BUILD_REPORT_TESTS` | `ON` | With tests enabled, require Python and exercise WAV/CSV/report integration plus audit self-tests. |
| `SETAR_WARNINGS_AS_ERRORS` | `ON` | Treat project-target compiler warnings as errors. |
| `SETAR_ENABLE_SANITIZERS` | `OFF` | Add address and undefined-behavior instrumentation on supported GCC/Clang toolchains. |
| `SETAR_ENABLE_JUCE` | `OFF` | Add the optional JUCE diagnostic Standalone/VST3 targets. |
| `JUCE_SOURCE_DIR` | empty | Explicit existing JUCE source directory; never fetched automatically. |

If report tests are enabled while tests are built, the renderer must also be
built. CMake rejects that contradictory configuration. If all tests are disabled,
the report-test option has no effect because there is no test target to register.
Keeping the default options is the simplest way to reproduce the repository's
baseline. Turning off warnings-as-errors can help investigate a new toolchain,
but it must not silently become the project's acceptance configuration.

For a minimal library-only build:

```sh
cmake -S . -B build/core-only -DSETAR_BUILD_TESTS=OFF -DSETAR_BUILD_RENDERER=OFF -DSETAR_BUILD_REPORT_TESTS=OFF
cmake --build build/core-only
```

There is not yet a versioned installed-package ABI, an exported CMake package,
or a supported binary distribution. Consumers inside this build can link
`setar::reference`. Treat the public headers as a research interface whose changes
still require review and tests, not as a promise of indefinite ABI compatibility.

## Compiler and numerical policy

Project targets request C++20 without compiler extensions. GCC and Clang builds
enable conversion, sign-conversion, shadowing, and common correctness warnings.
MSVC builds request a strict language mode and precise floating-point behavior.
Floating contraction is disabled where the configured compiler flag is
supported. No project target enables fast-math assumptions that discard NaN or
infinity semantics, because explicit finite-input validation is part of the API.

These choices improve inspectability; they do not guarantee identical binary
samples across every compiler and mathematical library. Trigonometric and
exponential functions can differ by small rounding amounts. Repeated renders
inside the same executable must be byte-identical. Cross-toolchain comparisons
use numerical tolerances and preserve separate artifacts. A future requirement
for portable bit-exact output would need a separate, deliberate mathematical
library and evaluation-order contract.

The core uses doubles for state and coefficients. The optional JUCE boundary
converts its observation to float only after multiplying by the fixed gain.
Changing the state to floats is not an innocuous optimization: it changes
roundoff, conservation error, allowable tolerance, and potentially the useful
duration of an unforced decay. Such a change requires independent evidence,
not just a successful conversion of compiler types.

## Failure triage

If configuration fails because Python is unavailable, install or select a
supported interpreter, or deliberately configure a core-only development
experiment. Do not remove the report test from the default acceptance path just
because one shell cannot find Python. If CMake cannot find Ninja, select a native
generator or make the existing Ninja executable discoverable; an audio library
will not solve that build-tool problem.

If the compiler reports a warning treated as an error, inspect the warning in
the project source and consider whether it indicates a real portability issue.
Do not disable broad warning families around numerical code without documenting
why. If a third-party JUCE module emits a warning, keep that diagnosis separate
from a core failure: the default target has no third-party module source.

If a test fails, first reproduce the exact named case with output visible. The
runner accepts a case name, such as
`build/headless-debug/setar_core_tests analytic_response`. Preserve the actual
and expected values, compiler flags, platform, and parameter set. A tolerance
change is a numerical design change. It needs a reasoned error budget and should
not be the first response to an unexplained disagreement.

If an executable cannot start, check the file, architecture, runtime dependency,
and executable permission. An operating-system launch failure is not a numerical
test failure. Conversely, do not classify a numerical assertion as an environment
problem merely because it appears on one platform. The test matrix exists partly
to expose assumptions that a single local toolchain would conceal.

## Where to go next

The next contribution should answer a bounded question. Examples include a
smooth finite-energy excitation with independently derived coefficients, a
verified external-force input for one resonator, a second polarization with a
clear energy metric, or a stronger allocation probe around the render path. Each
changes a specific contract and can be reviewed independently.

Do not start by adding realistic-looking controls whose physical meaning is
undefined. Do not relabel this reference as a validated instrument after hearing
a plausible pluck. The project advances when a documented uncertainty is removed,
an approximation is bounded, a measurement is acquired with provenance, or a
reproducible failure becomes a test. The [change workflow](change-workflow.md)
turns those principles into an actionable engineering handoff.

## Primary tool references

- CMake's own [command documentation](https://cmake.org/cmake/help/latest/manual/cmake.1.html),
  [preset specification](https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html),
  and [CTest manual](https://cmake.org/cmake/help/latest/manual/ctest.1.html).
- The official [JUCE CMake API](https://github.com/juce-framework/JUCE/blob/master/docs/CMake%20API.md),
  especially `add_subdirectory`, `FORMATS`, and `COPY_PLUGIN_AFTER_BUILD`.
- Python's standard-library documentation for
  [wave](https://docs.python.org/3/library/wave.html),
  [csv](https://docs.python.org/3/library/csv.html), and
  [hashlib](https://docs.python.org/3/library/hashlib.html).

These links describe tools. The repository's numerical acceptance criteria are
defined by its own derivations and tests, not supplied by those tools.
