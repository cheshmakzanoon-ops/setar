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

## Entry evidence and pinned-source map

Inspect the [prototype adapter record](../plugin/08-prototype-juce-adapter.md)
and current [plugin CMake](../../plugin/CMakeLists.txt) before planning a run.
`SETAR_ENABLE_JUCE_INSTRUMENT` selects `setar_prototype_plugin`, a wrapper around
the four-string synthetic engine; `SETAR_ENABLE_JUCE` selects the separate older
reference diagnostic. The prototype has configuration/syntax-check evidence
against JUCE tag `8.0.10` (recorded short revision `3af3ce0`), but no complete
build, validator result or host run. Preserve that status until actual evidence
exists. Historical engine defects described in older adapter prose are fixed
as recorded in [realtime evidence](../realtime/05-prototype-realtime-evidence.md);
the dense-pattern deadline limitation remains.

| Source identity | Exact reading route | Decision and required record |
| --- | --- | --- |
| `R-SDK-JUCE-AUDIOPROCESSOR` | [JUCE 8.0.10 AudioProcessor header](https://github.com/juce-framework/JUCE/blob/8.0.10/modules/juce_audio_processors/processors/juce_AudioProcessor.h), matching implementation/wrapper sources; [8.0.10 CMake API](https://github.com/juce-framework/JUCE/blob/8.0.10/docs/CMake%20API.md) | Lifecycle, buses, processing, state and target semantics. Record full checkout commit and local changes; current `master` API pages are navigation aids. |
| Same JUCE source family | [8.0.10 AudioProcessorValueTreeState header](https://github.com/juce-framework/JUCE/blob/8.0.10/modules/juce_audio_processors/utilities/juce_AudioProcessorValueTreeState.h), corresponding implementation, `juce_MidiBuffer.h`, `juce_MidiMessage.h`, `juce_Timer.h` | Parameter/state synchronization, event ownership and UI scheduling. Inspect the exact methods used; no source-reading claim is implied by this route. |
| `R-SDK-VST3-VALIDATOR` | [official validator](https://steinbergmedia.github.io/vst3_dev_portal/pages/What%2Bis%2Bthe%2BVST%2B3%2BSDK/Validator.html), [SDK repository](https://github.com/steinbergmedia/vst3sdk), [VST3 processor interface bundled with JUCE 8.0.10](https://github.com/juce-framework/JUCE/blob/8.0.10/modules/juce_audio_processors/format_types/VST3_SDK/pluginterfaces/vst/ivstaudioprocessor.h) and neighboring `ivstevents.h`/`ivstparameterchanges.h` | Compare the bundled interface revision with the independently selected validator SDK; pin SDK/submodules, validator binary/hash and applicable terms before execution. No validator revision or successful run is currently established. |

The [host playbook](../plugin/06-host-qualification-playbook.md) and
[product evidence gates](../plugin/07-product-qualification-evidence-gates.md)
define the experimental record. Synthetic adapter/lifecycle work can proceed
before specimen acquisition, while P13 qualification and setar fidelity remain
separate open gates. The [crosswalk](../../roadmap/phase-task-crosswalk.json)
determines the work orders below; all `plugin.*` acceptance names are planned,
not present CTests.

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

### Execution recipe

**Question and research.** Does every accepted host call sequence preserve the
same physical engine and write only the negotiated output? Read
`R-SDK-JUCE-AUDIOPROCESSOR` at the pinned methods `prepareToPlay`, `processBlock`,
`isBusesLayoutSupported`, `releaseResources`, `reset`, latency and tail accessors;
cross-check `R-SDK-VST3-VALIDATOR`'s selected SDK processing contracts. Search
official sources for `maximumExpectedSamplesPerBlock`, `numSamples zero`,
`setActive` and `setProcessing`. Use the [lifecycle chapter](../plugin/03-host-lifecycle-buses-latency.md).

1. Freeze the actual target contract from [SetarProcessor.h](../../plugin/Source/SetarProcessor.h)
   and [SetarProcessor.cpp](../../plugin/Source/SetarProcessor.cpp): VST3/Standalone,
   no audio input, stereo output only, float processing, MIDI input and no MIDI
   output. The reported tail is a fixed 10 seconds, not measured decay; record
   actual reported latency separately from the mapper's physical finger lead.
   Do not borrow the older diagnostic's mono support.
2. Build a transition table with expected readiness, physical state, pending
   events, clocks and output for prepare/process/reset/release/reprepare,
   suspension and failed preparation. Current `releaseResources` is empty;
   current `reset` clears engine state and queued gestures. Investigate mapper
   bookkeeping and clock consistency at those boundaries instead of asserting
   a completed lifecycle implementation. Source inspection is the initial
   artifact; actual calls in a compiled harness are the experimental evidence.
3. Exercise zero/one/irregular blocks and blocks beyond the host hint. Current
   preparation clamps scratch capacity to 32–2048 frames and processing chunks
   larger blocks; zero frames return without advancing the clock. Place events
   at each chunk boundary and verify offsets against an independent absolute
   timeline. Test supported rates and invalid rates, disabled/wrong buses,
   prepare failure, repeated release and destruction without editor creation.
4. Measure buffer extents, output silence on failure, ringing-state/reset scope
   and energy before/after transitions. Separate deliberate lifecycle reset
   from continuous retuning, which this adapter does not implement.

Retain call traces, expected/observed tables, output fingerprints and binary
manifest under `ENG-P14-T01`, `ENG-P14-T03`, `REQ-PLG-001` and `REQ-PLG-003`.
Current `realtime.block_partition` covers only the core. Proposed
`tests/plugin/lifecycle_host.cpp`, `plugin.wrapper_smoke` and
`plugin.lifecycle_matrix` must test the actual wrapper. Reject out-of-bounds
writes, incompatible rate state or unexplained tail/latency discrepancies;
mark unavailable builds blocked. Hand lifecycle semantics to T02–T05.

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

### Execution recipe

**Question and research.** Does a host performance become the same sample-stamped
physical action as headless playback, including releases while other strings
ring? Read `R-SDK-JUCE-AUDIOPROCESSOR` and the pinned `MidiBuffer`/`MidiMessage`
methods, then the selected VST3 event/parameter-queue interfaces from
`R-SDK-VST3-VALIDATOR`. Search `sampleOffset`, `noteId`, `channel`,
`parameter queue sample offset` and `zero velocity note on`; consult
[MIDI/MPE mapping](../plugin/04-midi-mpe-and-microtuning.md) and P13 event capacity.

1. Extract current behavior from `SetarProcessor::processBlock`, `enqueue` and
   [MidiGestureMapper](../../include/setar/instrument/performance.hpp). Note
   pitch selects a physical string/fret; velocity controls future nail gestures;
   note-off engages damping/lifts a finger. CC120 maps to explicit `Silence`;
   pitch bend, pressure and MPE are not implemented. Channel/note identity,
   CC123 and transport policy require deliberate decisions. Never reset ringing
   strings for an ordinary note-on or reinterpret observation gain as force.
2. Define a canonical trace with absolute sample index, stable tie-break,
   source/channel/note identity, target string/slot and SI gesture payload.
   Current offsets are clamped to the host block, then remapped into chunks;
   record whether out-of-range timestamps should instead be rejected/deferred.
   Establish simultaneous note/automation ordering before constructing tests.
   Include repeated same-pitch notes, absent note-offs, equal-time release/strike,
   zero blocks and events spanning block boundaries.
3. Audit the complete input traversal: a 256-gesture queue does not bound the
   current loop over all MIDI messages. Inspect owning `getMessage()` for long
   payload allocation, mapper-state changes preceding queue acceptance and
   `enqueue`'s drop-incoming policy: a full queue counts and rejects each new
   gesture, with no emergency release/panic priority. Retain source-inspected
   risks separately from failures observed in a later compiled host harness.
   Specify bounded admission and recovery with measurable rejected/coalesced
   counts; do not report that policy as implemented. Test reset/CC120 against
   mapper tracking as well as mechanics.
4. Capture host-normalized events and replay them through `Instrument::process`
   offline, keeping sample rate, preset and initial state fixed. Compare physical
   state/energy before audio. Separate event differences from the current
   block-ramped observation gain; its trajectory needs partition-invariant
   timing before full automation parity can pass.

Retain event manifests, first-divergence traces and accepted/rejected mappings
under `ENG-P14-T02` / `REQ-PLG-002`. Current `score.midi_mapper`,
`score.schedule_ordering`, `engine.event_ordering` and
`performance.persistent_state` are partial controls. Proposed
`src/plugin/event_adapter.cpp`, `tests/plugin/event_and_state_tests.cpp` and
`plugin.event_partition` supply wrapper acceptance. Reject changed excitation,
lost release integrity or offset drift; hand the approved mapping and unsupported
protocols to T03/T05 and P11/P15 reviewers.

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

### Execution recipe

**Question and research.** Can an old session restore the same physical meaning
without partially replacing the active instrument? Inspect
`R-SDK-JUCE-AUDIOPROCESSOR` state methods and the pinned
`AudioProcessorValueTreeState::copyState`, `replaceState`, `getRawParameterValue`
and `ParameterID` definitions. Search `normalized parameter mapping`,
`versionHint`, `state callback thread` and `copyState realtime`; read
[parameter compatibility](../plugin/02-parameters-state-and-compatibility.md).
The source route establishes API behavior; semantic compatibility requires
actual saved-state fixtures and independently expected decoded values.

1. Freeze the current dictionary from `SetarProcessor::create_layout`:
   `outputGainDb` is observation-only −48 to +24 dB, default 0;
   `dynamicsScale` is 0.25–2, default 1, applied to MIDI velocity;
   `tuning` and `fretMap` are nonautomatable choices. Preserve IDs, version hints,
   normalized mappings and list ordering independently of display labels.
   Changing a physical target while ringing requires an explicit state/work
   transition; current tuning/fret selection suspends and re-prepares, removing
   motion rather than performing physical retuning.
2. Document schema 1 as implemented: `SetarPrototypeState` XML binary,
   `schemaVersion`, `tuningId` and `fretMapId`. Foreign/malformed or non-1 schemas
   are ignored; an unknown stable choice ID currently retains its restored
   index. The existing `replaceState` followed by ID resolution and deferred
   preparation is not a verified atomic model-installation transaction.
3. Specify proposed `src/plugin/state_codec.cpp` byte/depth/field bounds,
   finite numeric validation, unknown critical-ID/version policy and immutable
   old-to-new migration fixtures. Decode/validate/prepare a complete candidate
   off processing, then use P13's installation protocol; failed or superseded
   candidates must preserve the active configuration and user-owned original
   bytes. Never serialize pointers or imply configuration state restores the
   instantaneous vibration field.
4. Compare round trips and migrations with independently written expected
   parameter/asset identities, then replay the same canonical gestures. Include
   truncation, nonfinite values, unsupported versions, reordered preset lists,
   unknown/missing assets, cancellation and concurrent save requests. Preserve
   all failure inputs and diagnose requested versus active state outside audio.

Retain dictionary, schema/migration decisions, original bytes, decoded tables,
hashes and physical/audio comparisons under `ENG-P14-T02` / `REQ-PLG-002`.
`plugin.state_roundtrip` and `plugin.malformed_state` are planned; no current
CTest verifies adapter state. Reject changed normalized meaning or partial
installation; mark unsupported migration blocked rather than fabricating old
fixtures. Hand accepted versions and recovery behavior to T05/P15/P19.

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

### Execution recipe

**Question and research.** Can the editor disappear, stall or reopen without
changing the sound or delaying processing? Read `R-SDK-JUCE-AUDIOPROCESSOR`'s
pinned editor/state lifecycle and the matching Timer/attachment implementations;
search `message thread`, `parameter attachment`, `timer callback` and
`suspendProcessing`. Use [concurrency](../realtime/04-concurrency-state-and-telemetry.md)
and [live UI evidence](../ux/05-live-engineering-graphs-and-screenshot-contract.md).
Rendered screenshots establish visible behavior only; thread and sound
comparisons need independent traces.

1. Map existing [SetarEditor.cpp](../../plugin/Source/SetarEditor.cpp) widgets,
   APVTS attachments and 10 Hz polling to `SetarProcessor::Telemetry`. Mark
   meters as sampled summaries, counters as cumulative and the synthetic model
   label as mandatory. `Instrument::snapshot` is taken by the processor; the
   editor must never obtain mutable solver vectors or call the single-owner
   engine probes concurrently.
2. Audit every producer, including host lifecycle callbacks. Current
   `engine_status_` is a `juce::String` written during preparation and read on
   the message thread; a host preparing off that thread creates an unresolved
   ownership risk. The 5 Hz processor timer also polls selections and re-prepares
   under suspension. Verify actual pinned synchronization rather than assuming
   suspension alone proves race freedom. Specify a bounded status/snapshot
   transport and active/requested generation before extending the editor.
3. Replay one canonical ringing sequence with editor absent, open, repeatedly
   closed/reopened and intentionally slow to consume telemetry. Compare
   mechanical/audio fingerprints, event offsets, fault counters and callback
   timings; permit dropped visual frames without dropping incident counts.
   Test multiple instances, resize/scale changes and stale-display behavior.
   Instrument destruction, attachments and queued callbacks in a non-timing
   race/memory harness.
4. Verify keyboard focus, names/units, screen-reader output and parameter
   gestures in the actual editor. Capture real windows with OS/DPI/font/host
   metadata. Practitioner interpretation of controls remains a separately
   authorized P15 acquisition route, not inferred from a synthetic UI test.

Under `ENG-P14-T02`, `ENG-P15-T01`, `REQ-PLG-002` and `REQ-UXI-001`, retain the
ownership table, UI-on/off differential, stall traces, captures and accessibility
results. No present CTest qualifies the editor; planned
`realtime.telemetry_snapshot`, `ui.control_semantics` and
`ui.accessibility_keyboard` require implementation/execution. Reject an audio
wait, data race or editor-dependent gesture mapping; label unavailable GUI/host
checks blocked. Hand bounded UI contracts to T05 and P15 without promoting
the currently unbuilt adapter.

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

### Execution recipe

**Question and research.** Does the exact built artifact load and obey its
contract in the declared platform/host matrix? Use
`R-SDK-JUCE-AUDIOPROCESSOR`'s pinned CMake/wrapper sources and
`R-SDK-VST3-VALIDATOR`'s exact SDK checkout, validator help and test-host
instructions. Search `validator command line`, `VST3 bundle architecture` and
the selected host's official plugin-scan/offline-export documentation. Follow
the [qualification playbook](../plugin/06-host-qualification-playbook.md);
record missing access as blocked, without installing dependencies or claiming
execution merely from this recipe.

1. With an already authorized, identified JUCE checkout, use the actual current
   instrument switch and target names in a separate build directory:

   ```sh
   cmake -S . -B build/juce-instrument -DCMAKE_BUILD_TYPE=Release -DSETAR_BUILD_INSTRUMENT=ON -DSETAR_ENABLE_JUCE_INSTRUMENT=ON -DJUCE_SOURCE_DIR=/absolute/path/to/reviewed/JUCE
   cmake --build build/juce-instrument --config Release
   ctest --test-dir build/juce-instrument -C Release --output-on-failure
   ```

   CMake does not fetch JUCE and disables automatic plugin copying. Adapt the
   generator/architecture for the recorded platform. The instrument format
   targets are `setar_prototype_plugin_VST3` and
   `setar_prototype_plugin_Standalone`; the complete build above also prepares
   CTest runners. CTest does not launch a host. Record the actual generated
   bundle path; do not assume another OS's artifact layout.
2. Hash the produced binary/bundle, pin compiler/JUCE/SDK/validator versions
   and retain configure/build logs. Run the official validator using the
   selected version's documented invocation against that bundle; save command,
   stdout/stderr, exit code and every warning. No such plugin binary or run is
   currently established by the repository's earlier syntax check.
3. In each authorized independent host, fresh-scan, instantiate, send T02's
   events, automate, save/reopen, bounce offline, vary rate/block/buses, bypass,
   suspend, duplicate and destroy instances while ringing. Exercise the T01–T04
   failure corpus with declared expected state, event and audio outputs.
   Measure device xruns separately from engine execution and capture real UI.

Retain per-case accepted/failed/blocked/skipped results and minimal reproduction
projects under `ENG-P14-T03` / `REQ-PLG-003`. Proposed
`tests/plugin/lifecycle_host.cpp`, `tools/plugin_validation/README.md` and
`experiments/host-matrix.md` organize planned `plugin.lifecycle_matrix` and
`plugin.repeated_instance`; no current CTest proves a host run. Reject failed
scan, changed bundle identity, crash or unexplained differential. Hand exact
binary-linked evidence to T06/P16; distribution/signing remains P18's gate.

## P14-T06: accept the wrapper with evidence

The exit package contains platform build results, event-differential results,
state/migration fixtures, validator logs, host-session reports, and actual UI
captures. Link each result to a source revision. Screenshot correctness does
not replace audio/lifecycle tests, and headless success does not certify an
unbuilt optional target.

### Execution recipe

**Question and research.** Which narrowly stated wrapper capabilities are
supported by reproducible evidence, and which remain untested? Re-read the
actual results against `R-SDK-JUCE-AUDIOPROCESSOR` and
`R-SDK-VST3-VALIDATOR` at their recorded revisions, the
[product evidence gates](../plugin/07-product-qualification-evidence-gates.md)
and [evidence contract](../validation/00-evidence-contract.md). Search official
release notes only for version-specific deviations encountered in tests; retain
the exact issue/API location. This task audits measured software behavior,
not acoustic fidelity or a measured instrument model.

1. Assemble a proposed `artifacts/host-qualification/<run-id>/` package following
   the existing playbook. Record source revision, product/format identifiers,
   plugin/bundle SHA-256, JUCE and SDK commits, validator binary/version,
   compiler/flags, OS/CPU/architecture, host/device versions, sample rates and
   actual block patterns. Include normalized event stream, physical trace with
   units, audio observation, immutable state fixtures, authentic screenshots
   and stdout/stderr/exit codes. Every derived plot points to raw inputs and
   its analysis revision.
2. Review each T01–T05 expectation against a result tied to the same binary.
   Separate compilation, conformity, host lifecycle, state compatibility,
   deadline behavior and accessibility. A validator cannot fill an independent
   real-host cell. A screenshot cannot establish event timing. Preserve failed
   and excluded runs, reasons, unresolved alternatives and the minimum next
   reproducer; absence of evidence is not a pass.
3. Use [product-qualification-template.json](../../data/product-qualification-template.json)
   as the existing metadata contract and
   [validate_product_qualification.py](../../tools/validate_product_qualification.py)
   to check a completed record. Its matrix requires Linux/Windows/macOS
   validator, real-host, realtime and UI cells plus the separate global
   perceptual cell. Keep unexecuted work absent/blocked as the schema permits;
   do not remove required rows to obtain a pass. The template's structural
   validation alone establishes no qualification. `--require-qualified` is
   appropriate only when asserting that complete product gate, which also
   depends on P17.
4. Have an independent reviewer reproduce one boundary/failure case and check
   artifact hashes, parameter identity and declared limitations. Reconcile any
   mismatch before accepting the corresponding capability.

Under `ENG-P14-T03` / `REQ-PLG-003`, deliver the supported matrix, rejection
decisions and open-work list to P15/P16/P18. Planned
`plugin.lifecycle_matrix`/`plugin.repeated_instance` remain distinct from
current headless and metadata checks. Reject mismatched provenance, missing
required execution or a silently changed physical event contract. Until an
actual wrapper build and host results exist, retain **unbuilt/unhosted** status;
even a later scoped wrapper acceptance cannot close setar fidelity gates.

> Integrate the verified engine through one documented JUCE adapter. Preserve
> canonical event timing, physical state ownership, and stable parameter IDs.
> Test lifecycle permutations, state corruption/migration, changing block sizes,
> and editor open/close behavior. Compare headless and host-rendered canonical
> performances. Audit the exact dependency licenses before downloading/building
> or distributing optional products, and state precisely which host/platform
> combinations were actually exercised.
