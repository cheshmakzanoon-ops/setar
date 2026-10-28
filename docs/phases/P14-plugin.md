# P14 — Integrate the engine with JUCE and plugin hosts

**Status:** specification. **Prerequisites:** P13. **Exit evidence:** the supported
plugin/application formats pass lifecycle, state, event, processing, and real-host
tests without moving physical-model responsibilities into the wrapper.

## Treat the wrapper as a boundary with its own failure modes

JUCE can provide valuable cross-platform application and plugin infrastructure.
It does not automatically make an engine real-time safe or a plugin correct in
every host. This phase covers the adapter contract, not a replacement synthesis
implementation. Read [plugin engineering](../plugin/README.md),
[real-time engineering](../realtime/README.md), and
[the current diagnostic boundary](../engineering/juce-boundary.md).

The repository's initial optional diagnostic is deliberately limited and has
separate build evidence. It is not the production plugin described here.
Before enabling a JUCE dependency, record its exact revision and the owner's
applicable licensing choice. Do not automatically accept commercial terms or
relicense the repository to satisfy a dependency.

## P14-T01: define formats, buses, and lifecycle

Choose initial plugin formats and platform/architecture combinations explicitly.
For each, define audio buses, supported layouts, MIDI/event inputs, precision,
latency reporting, and tail behavior. A synthesizer with no audio input should
not accidentally accept or process an input bus because a framework example did.

Implement prepare, processing, release, reset, suspend, and state-load paths with
clear ownership. Hosts may call them in sequences that differ from a standalone
test. Support variable block sizes and zero-length calls where the wrapper
contract permits them. Repeated preparation must not leak or retain incompatible
sample-rate state.

Handle invalid or unsupported configurations explicitly. Never assume the host
will only use the sample rate or block size from one manual demonstration.
Preserve a safe, documented output on failure and retain diagnosable status
outside the callback.

## P14-T02: translate events without changing their meaning

Map host timestamps into the engine's sample-stamped event contract. Define
ordering of simultaneous automation, note, expression, and transport events.
Keep host-specific interpretation in the adapter and test the canonical event
stream independently. A wrapper must not invoke a different excitation model
than the headless renderer.

Define channel and expressive-note ownership, pitch-bend interpretation,
controller defaults, and overflow behavior. Test malformed or unexpected event
sequences. If a protocol carries information the physical engine cannot use,
document the mapping or limitation rather than implying complete support.

Record a host performance as canonical events and replay it offline. Compare
with the plugin output under the same configuration. This differential test is
especially useful for discovering event offsets, block-boundary errors, and
state-reset differences.

## P14-T03: design stable parameters and state

Assign stable parameter identifiers and meanings before public preset release.
Separate user-facing controls, physical parameters, and derived coefficients.
Changing a display label must not break automation; changing a parameter's
meaning requires an explicit migration/version decision.

Define state schema version, validation, defaults, unknown-field policy,
corruption handling, and migration. Parse untrusted preset bytes defensively
with size limits. Never deserialize executable objects. Load/prepare expensive
assets away from processing and use the engine's safe installation protocol.

Test round trips, older fixtures, truncated data, invalid numbers, missing
assets, and future/unsupported versions. A failed load should leave a coherent
current state, not half-install a new tuning with an old body asset. Preserve
user data and report recovery options without overwriting the only copy.

## P14-T04: keep the editor observational and asynchronous

The UI reads bounded telemetry and submits commands through defined ownership
boundaries. It must not read mutable solver vectors concurrently or hold a
mutex needed by processing. Diagnostic graphs may downsample or drop visual
frames; audio processing must not wait for drawing.

Create and destroy editors repeatedly during playback. Test closed-editor
operation, resizing, display-scale changes, accessibility, and multiple plugin
instances. UI state and physical state should have explicit lifetimes. A
hidden editor must not stop the engine's event delivery or create a different
sound.

## P14-T05: build a host qualification harness

Compile and test the actual optional wrapper across the supported platform
matrix after dependency rights are resolved. Keep headless tests independent
so physics contributors do not require GUI dependencies. Add format validation,
plugin scanning, state tests, and controlled processing where tooling supports
them. Pin external validators and record their versions.

Then test representative real hosts and audio-device scenarios. Automated
validator success is valuable but does not prove every DAW interaction works.
Record host version, architecture, plugin format, sample rate, block size,
steps, and observed result. Distinguish an untested combination from a failure
and from an unsupported configuration.

Avoid uploading redistributable plugin binaries until the release/licensing
gate authorizes that distribution. Compilation evidence and public artifact
publication are separate decisions. Do not store signing credentials in source
or untrusted pull-request workflows.

## P14-T06: accept the wrapper with evidence

The exit package contains platform build results, event-differential results,
state/migration fixtures, validator logs, host-session reports, and actual UI
captures. Link each result to a source revision. Screenshot correctness does
not replace audio/lifecycle tests, and headless success does not certify an
unbuilt optional target.

> Integrate the verified engine through one documented JUCE adapter. Preserve
> canonical event timing, physical state ownership, and stable parameter IDs.
> Test lifecycle permutations, state corruption/migration, changing block sizes,
> and editor open/close behavior. Compare headless and host-rendered canonical
> performances. Audit the exact dependency licenses before downloading/building
> or distributing optional products, and state precisely which host/platform
> combinations were actually exercised.
