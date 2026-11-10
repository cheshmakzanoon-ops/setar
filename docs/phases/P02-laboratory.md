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

### Entry record and research map

Begin with an identified checkout and the existing synthetic fixtures; specimen
acquisition is not a prerequisite for numerical verification. Record the source
revision, dirty-tree patch, intended model, question, and proposed acceptance
rule before running. This chapter supplies execution instructions, not new build,
measurement, screenshot, or platform results. Preserve P00/P01's units and evidence
labels, and use the [shared research workflow](../research/research-to-model-workflow.md)
for finding disposition rather than treating source access as model acceptance.

| Research route | Exact starting point and useful search | Laboratory consequence |
| --- | --- | --- |
| Build and test contract | Official CMake/CTest links in [engineering setup](../engineering/README.md#primary-tool-references); search the installed version's `CXX_STANDARD_REQUIRED`, `cmake-presets`, and `ctest --show-only=json-v1` documentation | Record the actual compiler/options and discover the tests configured in that build. These tool references do not supply instrument coefficients. |
| Independent mechanics | `R-NUM-BILBAO-SMITH-2005`, [source card and author manuscript](../research/source-atlas.md#r-num-bilbao-smith-2005--nonlinear-string-energy-and-discretization), §II before §III/appendix; search `simply supported stiff string energy modal eigenfrequency` | Derive the continuum oracle separately from its numerical realization. Its assumptions do not certify a different discrete solver. |
| Further numerical derivation | `R-NUM-BILBAO-2009`, [source card](../research/source-atlas.md#r-num-bilbao-2009--deeper-finite-difference-foundation), authorized chapter on the chosen operator/energy method | Record the actual inspected edition/section; the atlas does not claim full-book access. A blocked reading can coexist with a direct synthetic derivation. |
| Uncertainty and incoming measurements | `R-METROLOGY-JCGM-100`, [official-source card](../research/source-atlas.md#r-metrology-jcgm-100--uncertainty-as-part-of-the-quantity), §§4–5 and §7; `R-SETAR-MANSOUR-SCAVONE-2012`, [apparatus/source card](../research/source-atlas.md#r-setar-mansour-scavone-2012--measurement-loading-and-supports), §§3.1–3.4/4.1–4.2 | Keep calibration and support/sensor-loading uncertainty separate from numerical error. These methods supply no calibrated values for the project's specimen. |

The measured-data route is the [acquisition campaign](../measurement/13-acquisition-campaign-execution.md)
and [provenance contract](../measurement/data-contracts-and-provenance.md): require
specimen/configuration, channel calibration, signed coordinates, units, clocks,
raw hashes, and permitted use before an empirical comparison. Missing inputs
remain blocked; use separately named synthetic fixtures meanwhile. All six tasks
serve `REQ-LAB-001` and `REQ-LAB-002`; the [crosswalk](../../roadmap/phase-task-crosswalk.json)
maps T01/T02/T03/T05 to `ENG-P02-T01` and T04/T06 to `ENG-P02-T02`. The planned
acceptance names `verification.oracle_independence` and `ci.matrix_coverage` are
not currently registered CTests. Never substitute those names for an execution log.

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

### Execution recipe

**Question and sources.** Can a second workstation reproduce the same declared
string equations and discover the same enabled checks without a graphics or
audio dependency? A missing test or changed floating-point mode could conceal a
pitch, decay, or energy defect. Under `ENG-P02-T01`, `REQ-LAB-001`, and
`REQ-LAB-002`, inspect [CMakeLists.txt](../../CMakeLists.txt),
[presets](../../CMakePresets.json), and [ProjectOptions.cmake](../../cmake/ProjectOptions.cmake)
against the official version-matched tool manuals above. Use
`R-NUM-BILBAO-SMITH-2005` §II and the [reference contract](../engineering/reference-model.md)
to freeze the physical problem independently of the selected compiler.

1. Record `git rev-parse HEAD`, `git status --short`, and the relevant tracked
   patch plus any untracked source inputs. Record `cmake --version`,
   `ctest --version`, `ninja --version`, and `python3 --version`; substitute
   `python` where that is the installed command. Retain the compiler's full
   version output, target architecture, OS version, generator, and selected C++
   standard library. Obtain library identification from that compiler's installed
   version headers or package metadata; do not infer libc++, libstdc++, or MSVC
   library version from the compiler brand alone. Preserve `CMakeCache.txt`,
   available generated `CMakeCXXCompiler.cmake`, and compile commands/build flags.
2. Use a fresh build directory for each compiler/configuration. The current
   contract requires CMake ≥3.25, C++20 without extensions, and Python ≥3.10 for
   report tests; the presets select Ninja. Run from the repository root:

   ```sh
   cmake --preset headless-debug
   cmake --build --preset headless-debug --parallel 2
   ctest --preset headless-debug --show-only=json-v1
   ctest --preset headless-debug --output-on-failure
   cmake --build --preset headless-debug --parallel 2
   ```

   Save each invocation, stdout/stderr, and exit code separately. Discovery lists
   registration only; retain a completed CTest result for execution. Repeat in
   `headless-release` to exercise optimized code. On Windows use a Visual Studio
   developer shell for Ninja, or the native-generator commands in engineering
   setup with `--config Debug` and `ctest -C Debug`.
3. Exercise a fresh path containing spaces with
   `cmake -S . -B "build/lab path check" -DCMAKE_BUILD_TYPE=Debug`, then
   `cmake --build "build/lab path check" --config Debug --parallel 2` and
   `ctest --test-dir "build/lab path check" -C Debug --output-on-failure`.
   For a deliberate configuration rejection, run
   `cmake -S . -B build/lab-invalid-report -DSETAR_BUILD_TESTS=ON -DSETAR_BUILD_RENDERER=OFF -DSETAR_BUILD_REPORT_TESTS=ON`.
   Expect nonzero exit and the renderer prerequisite message. A misspelled CMake
   variable can merely warn; it is not an adequate invalid-option test. There is
   currently no supported install/exported-package contract to certify.

**Acceptance and handoff.** Have a second contributor compare configured options,
test names, and physical errors, retaining failures and unavailable environments.
Reject silent test loss, changed physics by platform, or a disabled warning policy
presented as baseline success. The current targets include `setar::reference` and
`setar::instrument`; the register's `include/setar/numerics/` and `src/numerics/`
are proposed paths, not this checkout's existing numerical library. Deliver a
source/toolchain/options manifest and actual logs to T02/T03/T05. This establishes
laboratory reproducibility, not any measured parameter or acoustic claim.

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

### Execution recipe

**Question and derivation.** Does a prescribed finite, unforced stiff string
produce the expected frequencies, decay, and observation timing without changing
physical state to fit the WAV range? This is the synthetic baseline for later
attack and decay investigations under `ENG-P02-T01`, `REQ-LAB-001`, and
`REQ-LAB-002`. Read `R-NUM-BILBAO-SMITH-2005` §II as a methods source, then
[reference-model.md](../engineering/reference-model.md) and
[the analytic benchmarks](../numerics/11-verification-and-benchmarks.md).
Search `sine projection triangular displacement stiff string modal mass` if the
normalization is unclear; retain the inspected source and derivation, not a
borrowed specimen coefficient.

1. Choose the existing `StringParameters` fixture in
   [reference.hpp](../../include/setar/reference.hpp): `L=0.66 m`, `T=35 N`,
   `mu=0.0007 kg/m`, `EI=0.000015 N m²`, 16 modes, and constant amplitude decay
   `sigma=0.8 s⁻¹`. These are synthetic choices. Let x run from 0 to L, require
   `u=u_xx=0` at both ends, and use unit-peak sine modes. Integrating their squared
   shapes gives `m_n=mu L/2`; substituting into the PDE gives
   `omega_n²=(T/mu)(n pi/L)²+(EI/mu)(n pi/L)⁴`. Independently integrate the triangle
   against each sine, giving `q_n(0)=2h sin(n pi a)/(n² pi² a(1-a))`, with zero
   initial velocity. This must be derived without reading production modal output.
2. Freeze `h=0.0005 m`, `a=0.23`, observation `x/L=0.73`, 48 kHz, 2 s, and fixed
   gain `800 m⁻¹`. With `w_d=sqrt(omega_n²-sigma²)`, compare against
   `q_n(t)=q_n(0) exp(-sigma t)[cos(w_d t)+(sigma/w_d) sin(w_d t)]` and its
   derivative. Sum `q_n sin(n pi x/L)` for displacement and
   `m_n(v_n²+omega_n² q_n²)/2` for energy. Physical time is `j/48000`; the first
   observation precedes the first update. Hold mode count fixed: a sharp triangle
   has no converged continuum bending energy, so it cannot be an energy-refinement
   fixture merely by adding modes.
3. After the T01 build, choose a fresh output directory and run the current CLI:

   ```sh
   build/headless-debug/setar_render --output-dir artifacts/p02-baseline/raw --duration 2 --sample-rate 48000 --modes 16 --length-m 0.66 --tension-n 35 --density-kg-m 0.0007 --stiffness-n-m2 0.000015 --decay-per-s 0.8 --decay-freq2-s 0 --height-m 0.0005 --pluck-position 0.23 --pickup-position 0.73 --gain-per-m 800
   ```

   Retain `samples.csv`, `modes.csv`, `metadata.json`, and `render.wav` with hashes.
   On Windows use the executable suffix/configuration directory from T01. The
   present renderer has no CLI for the API's wavenumber-squared loss coefficient;
   a nonzero value needs a separately specified experiment, not an invented flag.

**Checks and consequence.** Execute `core.frequencies`, `core.analytic_response`,
`core.pluck_projection`, `core.endpoints`, and `integration.report` through CTest;
retain their actual outputs and T03's tolerance rationale. The current
[render_reference.cpp](../../apps/render_reference.cpp) returns 2 for invalid
input and 1 for an overwrite/runtime failure. Check the existing integration
fixture for clipping rejection, malformed input, repeat identity, and overwrite
preservation. Reject nonfinite state, unexplained energy growth, alias-domain
violations, or a normalized result offered as the original data. An accepted
result concerns `ModalString`/`PassiveResonator`, not nail excitation or calibrated
pressure. Transfer the frozen fixture to T03/T04 and P03; transferring a measured
length later requires the acquisition record, and the instrument's
`StringConfig.total_length_m`/`bridge_position_m` must remain distinct.

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

### Execution recipe

**Question and source route.** Can the verification distinguish a wrong restoring
sign, normalization, or time level from a correct implementation? Otherwise a
stable but wrong pitch or response could become the reference for every later
model. Under `ENG-P02-T01`, `REQ-LAB-001`, and `REQ-LAB-002`, use
`R-NUM-BILBAO-SMITH-2005` §II and §III only for their respective continuous and
discrete problems; obtain `R-NUM-BILBAO-2009` through an authorized copy if a deeper
scheme proof is required. Search `oscillator closed form energy midpoint discrete
invariant` and read [verification benchmarks](../numerics/11-verification-and-benchmarks.md)
with [error budgets](../validation/01-test-taxonomy-error-budgets.md).

1. Write a test-to-oracle table before implementation. The current
   [reference tests](../../tests/reference_tests.cpp) evaluate characteristic
   frequency with expanded `long double` algebra, direct-time analytic motion
   against repeated updates, and Simpson quadrature against the closed-form
   pluck coefficient. They also exercise exact same-binary block partitioning,
   reset, and invalid-state transactionality. State their shared assumptions:
   the same physical PDE is intentional; sharing the production coefficient
   helper would invalidate the comparison. `long double` is not higher precision
   on every platform. Record its `numeric_limits` precision for any precision claim.
2. For extension, derive an isolated `PassiveResonator` impulse independently:
   `v⁺=v⁻+J/m` and `Delta H=v⁻J+J²/(2m)` for impulse J in N s. Test nonzero
   displacement and velocity, both impulse signs, zero loss, near-critical loss,
   and rejected unsupported damping. Critical/overdamped evolution is not an
   existing `PassiveResonator` feature. For spatial work use actual
   [numerical_prototype.hpp](../../include/setar/numerical_prototype.hpp),
   [numerical_prototype.cpp](../../src/numerical_prototype.cpp), and
   [its tests](../../tests/numerical_prototype_tests.cpp); distinguish discrete
   eigenvalues from continuum error and keep space/time refinements separate.
3. Discover first, then execute the bounded subset in both configurations:

   ```sh
   ctest --preset headless-debug --show-only=json-v1
   ctest --preset headless-debug -R '^core\.' --output-on-failure
   ctest --preset headless-release -R '^core\.' --output-on-failure
   ctest --preset headless-debug -R '^(numerics\.(string_spatial_convergence|force_interpolation_adjoint|stability_envelope|discrete_energy_ledger)|coupling\.(two_mass_oracle|port_power_balance))$' --output-on-failure
   ```

   The Release build must already exist. For each new mechanism, choose the
   smallest informative defect, such as a reversed spring force or omitted
   interpolation transpose, and test a disposable candidate copy. Retain its
   exact patch, targeted failing assertion, and unchanged baseline result; a
   compilation failure does not demonstrate numerical sensitivity. Never leave
   the defect in the normal source.

**Portable acceptance.** Compare integers and schemas exactly; compare physical
quantities with `|observed-reference| <= absolute + relative*|reference|`, including
an absolute SI floor near a node. For example, current `core.analytic_response`
uses `3e-15 m + 2e-10 |q|` and `3e-12 m/s + 2e-10 |v|` after 6000 steps. These are
existing fixture tolerances, not universal perceptual thresholds. Derive any new
budget from reference precision, step count, scale, and refinement error before
seeing the acceptance result. Preserve maximum and aggregate residuals, failed
fixtures, raw curves, proposed/accepted threshold status, and unresolved rounding
floors. Reject a test that also accepts its intended numerical mutant or whose
oracle calls production code. Current named cases provide bounded evidence relevant
to the broader planned `verification.oracle_independence`, which remains unmapped
in the executable coverage registry. Hand the derivation and gap list to P03 and
the current instrument solver tests, without closing fidelity gates.

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

### Execution recipe

**Question and sources.** Can a reviewer trace an apparent change in decay or
dispersion back to exported physical state and distinguish it from plotting,
gain, or timing error? Work under `ENG-P02-T02`, `REQ-LAB-001`, and
`REQ-LAB-002`. Read [artifact contracts](../engineering/artifacts-and-reproducibility.md),
the `R-METROLOGY-JCGM-100` §§4–5/7 source route, and Python's official `csv`,
`wave`, and `hashlib` references linked from engineering setup. Search `JCGM
measurement model correlated inputs reporting uncertainty`; the guidance informs
future calibrated error bars, not uncertainty invented for synthetic inputs.

1. Begin with the T02 raw files and an independent expected quantity. Generate
   the report from those bytes with the existing tool, recording its exit code:

   ```sh
   python3 tools/generate_report.py --input artifacts/p02-baseline/raw --output artifacts/p02-baseline/report --source-revision working-tree-p02-baseline
   ctest --preset headless-debug -R '^integration\.report$' --output-on-failure
   ```

   The example label explicitly denotes a working-tree experiment; replace it
   with the actual clean revision only when the build chain supports that claim.
   Inspect `manifest.json` and its input/output hashes. The current validator
   checks all sample rows and samples the analytic displacement at deterministic
   indices; that sampled comparison is not an all-sample independent error norm.
2. Review the actual plots against raw extrema and rows: displacement in meters
   is displayed in millimeters, energy in joules is displayed in microjoules,
   and dispersion is cents relative to the tension-only harmonic. Preserve the
   first-sample timing convention. Read `chart_processing` before interpreting
   the energy curve, which is subsampled for plotting. A narrow energy spike must
   be judged from the full numeric data. If adding a residual plot, retain its
   comparator, alignment policy, denominator, uncertainty, and derivation as
   proposed new report work; no unrecorded smoothing or per-note normalization.
3. Inspect [test_report.py](../../tools/test_report.py) to see which rejection
   cases are actually covered. Retain invalid-header, nonfinite-value,
   inconsistent-time, empty-series, and mismatched-model cases as separate
   negative fixtures when extending coverage; do not claim every proposed
   mutation exists today. A changed unit or input hash must be distinguishable
   from a genuine model failure. Keep the rejected source files and diagnostic.
4. Where the optional Playwright/Chromium prerequisites are already available,
   capture the generated report, not a reconstruction:

   ```sh
   python3 tools/capture_report.py --input artifacts/p02-baseline/report/index.html --output artifacts/p02-baseline/capture/report.png --viewport-width 1280 --viewport-height 900
   ```

   Inspect the actual PNG for loaded plots, legible axes, clipped legends, and
   visible synthetic warnings. Retain its adjacent JSON with browser version,
   viewport, source hash, and PNG hash, plus OS/font information in the experiment
   record. If browser execution is unavailable, report the observed blocker and
   keep numerical/report results separately. No screenshot is implied by an SVG
   existing on disk, and this capture is not a JUCE or host qualification.

**Acceptance and translation.** A second reader must reproduce one table/plot
from the saved inputs and explain `ModalString::displacement()` versus mechanical
energy versus PCM gain. `BodyMode.radiation_weight` in the instrument is a
different, uncalibrated acceleration observation; this report cannot certify
its acoustic feedback or pressure calibration. Reject unexplained unit changes,
missing raw files, hidden failed checks, or image values inconsistent with data.
Deliver raw/derived manifests, visual review, and accepted/rejected/inconclusive/
blocked decisions to T05/T06. `tools/validate_evidence.py` and
`data/evidence.schema.json` remain proposed work-order paths; existing report
validation is useful but does not implement their entire contract.

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

### Execution recipe

**Question and research route.** Does one frozen numerical experiment preserve its
physical predictions and rejection behavior on Linux, Windows, and macOS? A
compiler-dependent decay or a skipped negative test undermines later instrument
comparisons. Under `ENG-P02-T01`, `REQ-LAB-001`, and `REQ-LAB-002`, inspect the
current [workflow](../../.github/workflows/engineering.yml),
[testing chapter](../engineering/testing-and-ci.md), and official tool manuals
for the installed versions. Search `CTest output-junit multi configuration -C`
and `compiler floating point contraction standard library`. Use the same
`R-NUM-BILBAO-SMITH-2005` §II fixture on every system; an alternate compiler
is an implementation comparator, not an independent physical theory or measured
setar dataset.

1. Freeze commit/patch, CMake options, fixture inputs, mode count, observation,
   sample rate, and the tolerance revision from T01–T03. Record each runner's
   actual image/toolchain/library/architecture; an OS label alone does not pin
   every compiler update. The existing workflow selects `ubuntu-24.04`,
   `windows-2022`, and `macos-15`, with a separate Linux ASan/UBSan job. Its
   headless commands are:

   ```sh
   cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DSETAR_BUILD_TESTS=ON -DSETAR_BUILD_REPORT_TESTS=ON -DSETAR_ENABLE_JUCE=OFF -DSETAR_WARNINGS_AS_ERRORS=ON
   cmake --build build --config Debug --parallel 2
   ctest --test-dir build -C Debug --show-only=json-v1
   ctest --test-dir build -C Debug --output-on-failure --output-junit test-results.xml
   ```

   Use a fresh worktree/build location for this workflow-shaped reproduction;
   do not replace another generator's cache. Preserve discovered test inventory,
   complete result XML, `Testing/Temporary/LastTest.log`, and logs from failed
   configure/build stages. Resolve the JUnit file in the actual test working
   directory and confirm that it was uploaded rather than assuming the filename.
2. Run the same T02/T04 fixture using the generated executable path. The workflow
   currently uses `build/Debug/setar_render.exe` on Windows and
   `build/setar_render` on Linux/macOS. It additionally runs solver/loss reports
   and every score in `data/scores`; enumerate actual score/input hashes.
   Compare exact schema, indices, sample count, physical configuration, and
   evidence labels first; then compare both platforms to the independent oracle
   and compare their residuals using the declared SI budgets. Require repeated
   renders within one binary to retain identity, but do not require cross-compiler
   PCM/CSV hashes to match when allowed floating-point differences remain.
3. Reproduce sanitizer checks on a supported GCC/Clang installation with
   `cmake --preset asan-ubsan`, `cmake --build --preset asan-ubsan --parallel 2`,
   and `ctest --preset asan-ubsan --output-on-failure`. Retain a startup failure,
   numerical assertion, timeout, or sanitizer finding under its actual category.
   A narrowed run with a disabled detector needs its own label and does not
   replace the unchanged full-job result. Do not infer callback timing, lock
   freedom, or host compatibility from sanitizer success.

**Acceptance and handoff.** Record run URL, exact commit, each job's actual
passed/failed/skipped/cancelled result, artifact names/hashes, and missing files.
The current workflow uses platform-specific artifact names, `if: always()`, and
14-day retention; promote accepted scientific records to durable evidence before
expiry. Its defined YAML and an older passing run cannot qualify this revision.
Reject a platform qualification if its test inventory unexpectedly differs or
its artifacts cannot be associated with the tested source. A platform blocked by
access remains blocked while independent synthetic work continues. Hand a scoped
matrix to T06 and P16; planned `ci.matrix_coverage` is not itself a CTest, and the
optional JUCE adapter still requires P14's distinct binary/validator/host evidence.

## P02-T06: prepare experiment reuse

Write an experiment template that captures hypothesis, model/configuration,
input provenance, expected results, command, metrics, artifact paths, and
limitations. Separate source files from generated outputs. Avoid committing
large unlicensed audio or nondeterministic build folders. The documentation
auditor should check local links and dependency-graph consistency and report
actual prose count without treating word count as correctness.

### Execution recipe

**Question and research route.** Can another analyst reproduce a decisive result,
including a failed hypothesis, without private instructions or silently changed
inputs? That protects later judgments about pitch, contact work, body coupling,
and sound quality from stale or mislabeled artifacts. Under `ENG-P02-T02`,
`REQ-LAB-001`, and `REQ-LAB-002`, start with the existing
[experiment template](../../templates/experiment.md),
[finding template](../../templates/research-finding.md), and
[reference-corpus provenance chapter](../validation/02-reference-corpus-provenance.md).
Use `R-METROLOGY-JCGM-100` §§4–5/7 for measurement quantities and
`R-SETAR-MANSOUR-SCAVONE-2012`'s apparatus sections for support/loading confounders.
Search `setar sensor loading support repeatability` only when designing the
empirical extension; no paper's apparatus result supplies this specimen's error bar.

1. Create a **proposed** record such as `experiments/p02-baseline/experiment.md`
   from the template, preserving the question, rival explanation, source ID and
   exact inspected section, synthetic/measured status by field, SI parameters,
   signed coordinates, initial state, solver, observation, and acceptance rule.
   Keep definitions separate from generated `artifacts/p02-baseline/` outputs.
   Record the source-to-binary chain, command arguments, exit codes, toolchain,
   environment, analysis transforms, and planned versus actually executed checks.
   These are proposed record locations, not files supplied by this chapter.
2. Retain both the report's existing data/asset hashes and an explicit source/data
   manifest. For a Bash shell, this portable Python-standard-library example
   hashes actual tracked file bytes, including working-tree edits, plus the raw
   fixture; it deliberately refuses to replace an existing manifest:

   ```sh
   python3 - <<'PY'
   import hashlib, json, pathlib, subprocess
   root = pathlib.Path.cwd()
   names = subprocess.check_output(["git", "ls-files", "-z"]).decode().split("\0")
   paths = [root / name for name in names if name]
   paths += sorted((root / "artifacts/p02-baseline/raw").glob("*"))
   hashes = {}
   for path in paths:
       if path.is_symlink() or not path.is_file():
           raise SystemExit(f"record link or missing input explicitly: {path}")
       hashes[path.relative_to(root).as_posix()] = hashlib.sha256(path.read_bytes()).hexdigest()
   record = {
       "source_revision": subprocess.check_output(["git", "rev-parse", "HEAD"], text=True).strip(),
       "tracked_source_and_raw_sha256": hashes,
       "limitations": "Untracked inputs, binary, build flags, rights and calibration need separate records"
   }
   with (root / "artifacts/p02-baseline/source-data-manifest.json").open("x", encoding="utf-8") as out:
       json.dump(record, out, indent=2, sort_keys=True, allow_nan=False)
       out.write("\n")
   PY
   ```

   Verify that all four raw files exist before running this example; an empty
   glob is not proof of a completed render. Add explicit hashes for the actual
   executable and every untracked input, analysis script, or external asset used.
   Store relative public paths and restricted provenance separately. Hashes prove
   identity, not calibration, rights, or physical truth. On non-Bash systems run
   the same Python body as a recorded script; its hash becomes another input.
3. Preserve each failed/invalid trial, original files where permitted, command,
   exclusion reason, and descendants affected. Use accepted for stated scope,
   rejected, inconclusive, or blocked; do not delete negative results when a
   later candidate passes. For measured extensions attach calibration versions,
   correlated uncertainties, permitted-use record, and session/specimen grouping;
   never split reprocessed versions of one take across development and holdout.
4. Ask an independent reviewer to regenerate one decisive quantity and one
   rejection from the packet in a separate checkout. Compare against the oracle
   and invariant/tolerance policy rather than merely refreshing a golden WAV.
   Run `python3 tools/audit_docs.py`, `python3 tools/audit_implementation.py`,
   and `python3 tools/audit_executable_coverage.py`; retain each result and any
   concurrent-tree/link limitation. These audits validate their own structural
   scope, not the experiment's physical conclusion.

**Acceptance and model handoff.** Record the equation/parameter/state/port/observation
affected and current destination symbol. A stiffness finding may address
`StringParameters.bending_stiffness_n_m2`; transferring it to `StringConfig`
also needs speaking length, afterlength, and bridge conditions. A decay finding
must distinguish the amplitude-loss coefficients' SI units from modal damping
ratio. A list of body frequencies cannot populate modal masses, bridge residues,
or `BodyMode.radiation_weight`. If the evidence cannot discriminate a mechanism,
retain the simpler fixture and specify the next measurement. The proposed
general evidence schema/validator in `ENG-P02-T02` remains open until implemented
and mutation-tested; do not pretend the manifest example completes it. Hand P03
the verified synthetic baseline and P01/P12 any blocked acquisition or
identification needs, with all original scientific gates unchanged.

## Exit gate and next step

A new contributor must reproduce the bootstrap from a clean checkout, interpret
its outputs, and explain why it is not yet a setar. All three platform jobs must
report their actual outcomes. The independent reference tests become P03's
baseline; measurement work may proceed in parallel without contaminating the
synthetic fixture identities.

Use this handoff checklist for the claimed scope, including negative outcomes:

- Exact source/dirty-patch, binary/build, input, analysis, and artifact identities
  are reconstructable; the toolchain and actual commands have retained outcomes.
- Independent derivation, controlled physical problem, coordinate/unit mapping,
  tolerance rationale, observed residuals, and relevant failure fixture are available.
- Discovered CTests, executed results, and unimplemented acceptance identifiers
  are separate; every operating-system job and optional visual step has an honest status.
- Raw tables remain available beside reports/captures; gain, truncation,
  subsampling, quantization, exclusions, and uncertainty sources are explicit.
- A second reader can reproduce one result and one rejection, identify the
  current/proposed C++ destination, and name P03's next bounded experiment.
- Synthetic verification can proceed while specimen data are blocked, but no
  build, screenshot, report, or reused methods source closes acoustic validation.

> Implement or repair one laboratory capability under P02. Preserve the
> dependency-free headless build. Show the exact test that fails before your
> change and passes afterward, or justify a new independent expected result.
> Produce machine-readable output and one useful plot if the quantity warrants
> it. State platforms actually exercised, test artifacts, and remaining gaps.
> Do not advertise the reference fixture as a finished instrument.
