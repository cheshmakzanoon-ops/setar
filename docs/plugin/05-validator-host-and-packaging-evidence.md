# Validator, host, and packaging evidence

Plugin validation has several layers: format conformance tools, generic stress
harnesses, actual host workflows, and distribution/package checks. Their results
overlap but are not interchangeable. This chapter is a proposed qualification
procedure. Tool installation, commercial host access, code signing, notarization,
and publication require the appropriate authorization and credentials; do not
perform them merely because this document describes them.

## Pin tools and audit licenses

Record the exact JUCE checkout, VST SDK revision, validator revision, pluginval
revision, compiler, and build flags. Current Steinberg documentation states that
VST 3 uses the MIT license from SDK 3.8 onward; see the official
[license page](https://steinbergmedia.github.io/vst3_dev_portal/pages/VST%2B3%2BLicensing/VST3%2BLicense.html)
and [FAQ](https://steinbergmedia.github.io/vst3_dev_portal/pages/FAQ/Licensing.html).
Older SDK copies and wrappers can contain different license material. Audit the
actual pinned files rather than copying obsolete dual-license guidance or
assuming every bundled component changed at the same time.

JUCE has its own license requirements, independent of the VST SDK. Validator
applications, optional test-host binaries, fonts, icons, and measured assets can
also have separate terms. Maintain notices and a dependency inventory. Running
a tool, linking its code into the product, and redistributing its binary are
different operations. Do not copy test-tool source into the numerical core
without reviewing that consequence.

## Steinberg validator

The official [validator description](https://steinbergmedia.github.io/vst3_dev_portal/pages/What%2Bis%2Bthe%2BVST%2B3%2BSDK/Validator.html)
identifies a command-line test host for VST3 conformance. Build or obtain the
approved revision and use its actual help output for invocation. Save the full
log, exit status, tested plugin path/hash, architecture, and environment.
Do not invent command flags from memory or treat a tool launch as a successful
validation run.

Run the candidate as built, including its real metadata and parameter list.
Review warnings and unsupported-feature reports as well as errors. If a validator
defect is suspected, preserve a minimized reproducer and compare the relevant
contract with the pinned SDK documentation. Do not simply suppress the test
because another host appears to work.

The SDK's GUI test host and command-line validator are distinct artifacts.
Availability, architecture support, and license obligations can differ. A
successful command-line run does not imply a GUI host was exercised. Record
which tool actually ran and what it tested.

## pluginval

[Tracktion/pluginval](https://github.com/Tracktion/pluginval) is a cross-platform
plugin testing project with headless and GUI operation. Pin the tool, inspect
its supported formats and options, and record the selected strictness and test
configuration. Keep the executable separate from the product unless a reviewed
integration decision says otherwise.

Run lifecycle, parameter, state, and stress tests applicable to the candidate.
Capture logs even on failure. A validator crash should preserve the child-process
exit and plugin identity; it must not become “no failures found.” Reproduce
failures under a debugger or instrumented build where authorized, then turn the
root cause into a repository regression test.

Passing pluginval is useful robustness evidence, not proof of every DAW workflow
or correct physical synthesis. Its random tests can find defects but do not
replace canonical deterministic fixtures with expected semantics. Store random
seeds and minimized cases where the tool exposes them.

## Audio Unit validation

On macOS, inspect the installed validation tool's help and the actual Audio Unit
identifiers. Apple's archived [Audio Unit development guide](https://developer.apple.com/library/archive/documentation/MusicAudio/Conceptual/AudioUnitProgrammingGuide/AudioUnitDevelopmentFundamentals/AudioUnitDevelopmentFundamentals.html)
describes `auval` as part of the AU development workflow; current operating-system
behavior must be checked on the tested machine. Do not assume a Linux CI job can
exercise AU, or that a VST3 result covers it.

Record component discovery, architecture, identifiers, validation log, and actual
host loading. Test after a clean install or controlled scanner-cache state where
appropriate. Never delete broad host caches or user preferences without explicit
authorization; prefer a dedicated test account or isolated test environment.

## Actual-host matrix

Select representative authorized hosts based on intended users and supported
formats. Each matrix row includes host version, OS, architecture, format, plugin
hash, sample rates, block behavior, and result. Mark unavailable combinations
unrun. A row labeled only “macOS: pass” is insufficient.

The script covers discovery, insertion, play without editor, editor resize,
automation record/replay, state save/reopen, preset change, duplicated instances,
freeze/bounce/export, transport stop/seek/loop, sample-rate changes, bypass,
tail capture, and removal. Include a project with several instances and a
controller-heavy passage. Use controlled levels and a safe monitoring setup.

Test ordinary user recovery. A missing asset should produce an understandable
message and preserve a valid state. An unsupported layout should be rejected
cleanly. A plugin should not hang the host while a background task waits for an
audio callback that the host has stopped. Save a minimal host project only when
its contents and redistribution permissions permit it.

## Packaging correctness

Check bundle layout, executable architecture, runtime dependencies, identifiers,
version metadata, resources, and required notices. Verify that debug-only assets,
private paths, test recordings, and credentials are absent. A plugin working
inside the build tree may depend on files that are missing from its package.
Test a clean installation context rather than only the developer's machine.

Use the platform's supported packaging and signing mechanisms when distribution
is authorized. Do not claim a build is signed, notarized, or accepted by a host
unless the operation completed and evidence was inspected. A signature establishes
an identity/integrity property, not acoustic fidelity or freedom from defects.
Keep signing credentials outside untrusted CI jobs.

Do not publish automatically from a successful compile. Release approval includes
evidence review, license/asset review, and the owner's publication authorization.
Test artifacts can be useful internally without being suitable for public
distribution. Separate package creation from upload and promotion.

## Regression and acceptance

Every product defect should produce a bounded regression at the lowest layer
that exposes it, plus an integration check where needed. A parser bug belongs in
hostile-state tests; a scanner lifecycle bug needs a construction/lifecycle
fixture; an automation replay bug needs canonical event evidence. Retain actual
host confirmation for the original symptom.

Acceptance artifacts include dependency/license inventory, validator logs,
pluginval logs, AU evidence where supported, host matrix, package manifest,
binary hashes, notices, and explicit missing checks. The next action is to
execute the smallest supported-format qualification path on a real built binary.
Do not fill this packet with hypothetical passes or screenshots of tools that
were never run.
