# VST3/JUCE execution and host-qualification playbook

**Status: planned for the full instrument.** The present optional JUCE target
is only a synthetic diagnostic. It has not been established as a complete
four-string setar engine, production editor, tested VST3 binary, or
host-qualified product. The headless reference build currently has no JUCE
dependency. Keep that independent build working after the full adapter exists.
The [13-gate executable metadata contract](07-product-qualification-evidence-gates.md)
now verifies that blank qualification templates and certain false pass
records are rejected; **no actual plugin host or human study has been run**.

## Primary interface authority

- JUCE's official
  [AudioProcessor reference](https://docs.juce.com/master/classjuce_1_1AudioProcessor.html)
  governs processor lifecycle, buffer methods and state serialization.
- JUCE's
  [AudioProcessorValueTreeState reference](https://docs.juce.com/master/classjuce_1_1AudioProcessorValueTreeState.html)
  explains the host-visible parameter/state helper; evaluate thread/access
  and real-time implications rather than assuming every access is safe.
- Steinberg publishes the official
  [VST3 validator](https://steinbergmedia.github.io/vst3_dev_portal/pages/What%2Bis%2Bthe%2BVST%2B3%2BSDK/Validator.html)
  and [plugin test host](https://steinbergmedia.github.io/vst3_dev_portal/pages/What%2Bis%2Bthe%2BVST%2B3%2BSDK/Plug-in%2BTest%2BHost.html).

Review the applicable dependency and distribution licenses before downloading,
embedding or distributing JUCE or a VST3 SDK. A documentation example is not
a grant to redistribute third-party source, proprietary host software, plugins,
fonts or recorded instruments. The project owner must approve that choice.

## Architecture prerequisites (must be true before wrapper code)

1. `setar_engine` or an equivalent C++20 library builds and tests without any
   JUCE header or audio system. It consumes a prepared model and typed normalized
   physical events, and writes bounded PCM buffers.
2. Sample rates, block capacities, parameter domains, model identity, event
   overflow behavior and reset semantics are explicit. `process` is
   nonthrowing and cannot allocate or wait.
3. A headless differential-render harness can drive the same normalized event
   stream used by the plugin and capture physical diagnostics. It is the
   control condition for host comparisons.
4. A source-level ownership review has identified where preparation, host
   messages, GUI, state decoding, asset publication and reclamation execute.

The adapter may combine physical output with approved downstream observation
or room processing, but it must not reimplement nail mechanics, convert note
velocity to an unexplained audio transient, or reset unrelated string state
for convenience.

## Adapter-to-engine contract

| Host input or lifecycle | Adapter action | Physical/host test |
| --- | --- | --- |
| `prepareToPlay(rate, blockHint)` | Request or receive a fully prepared rate-specific asset and bounded state before processing; define behavior if preparation cannot complete | Repeated prepare, rate changes, invalid rate, zero/low/max block hints |
| `processBlock` | Map available channel buffers, normalized sample offsets and accepted immutable asset; render only supported frame count, with safe policy for oversize blocks | Zero/one/irregular blocks; mono/stereo and mismatched buses |
| MIDI note-on/off | Apply configured physical string/gesture selection; note identity is host bookkeeping, not a physical oscillator | Same-sample order, repeated notes, overlapping courses, note-off with ringing state |
| MPE/pitch/microtuning | Translate documented per-note expression to physical contact/tension or explicitly labeled convenience mode | Per-note mapping limits, pitch range, pressure/slide, invalid channel |
| Parameter automation | Stable ID to physically meaningful control with smoothing and range validation | Automation within block, fast modulation, state vs observation distinction |
| Host transport/reset | Apply *named* reset scope, preserve or intentionally clear physical energy by policy | Seek, stop/start, all-notes-off, all-sound-off and project reload |
| Bypass | Report and implement how physical state evolves while audio may be bypassed | Bypass toggles with decaying/sympathetic string energy |
| State save/restore | Encode stable versioned canonical configuration; decode into rejected/accepted temporary candidate before adoption | Round-trip, byte limits, corrupt chunk, old schema, missing asset |
| Editor open/close | Consume bounded snapshots and submit control requests; never lock/render by directly reading mutable physical arrays | Repeated editor lifecycle under active notes and asset reload |
| Host offline render | Use same physical semantics with declared offline quality policy; report differences and latency | Deterministic repeated export, host versus headless differential |

Steinberg/VST3 event and JUCE method semantics evolve across SDK versions.
Freeze and record the actual reviewed API version at implementation. Do not
invent a supported note-expression mapping just because a host UI displays
an automation lane.

## Exact execution sequence

**A. Dependency authority.** Choose and record the approved JUCE source
revision and licensing terms; verify the optional CMake integration. Record
compiler/SDK/runtime dependencies and target formats. Run the existing
headless C++ build as a dependency-closure regression before touching JUCE.

**B. Minimal wrapper smoke.** Build a plugin whose physical core is still a
verified synthetic fixture; confirm format identity, channel negotiation,
load/unload, state callbacks and sample offsets. This isolates host defects
before unknown contact/body physics is introduced. Mark it **diagnostic**.

**C. Physical engine connection.** Replace only the diagnostic render call with
the prepared persistent-string engine after P13 passes. Retain the same event
manifest through host and offline paths, and compare state, port work and
output samples within declared arithmetic tolerance. If the adapter changes
sound without a documented transform, treat it as a bug.

**D. Parameter/state contract.** Write a parameter ID dictionary and test a
fresh preset, a saved session, a version-migrated session and malformed bytes.
Apply state transactionally and reject external asset references if unavailable
or untrusted. A preset should not automatically serialize transient physical
energy unless explicitly offered as a separate full-state feature.

**E. Validator.** Build the official VST3 SDK validator/host following the
version's documented procedure. Invoke it on the **actual built plugin** and
retain full stdout/stderr, exit code, validator version, plugin absolute path
and binary hash. Fix failures at their real semantic boundary; do not ignore
warnings by deleting required tests. Distinguish validator coverage from all
possible host behaviors.

**F. Host matrix.** Define representative actual DAWs/hosts and OS versions
based on the intended audience and access rights. For each permitted
combination: install cleanly, scan, instantiate, play, automate, save/reload,
render offline, change rate/block/buses, duplicate instances, close/open editor,
and handle bypass/all-notes-off. Capture logs and real screenshots. Report any
unavailable proprietary DAW as *not tested*, not a success.

**G. Product stress.** Perform dense contact/ornament inputs and fast physical
control automation, while opening editor, replacing presets and stressing CPU.
Record latency/tail values as reported to host and actual observed behavior.
Check physical energy/state integrity, safe failure response, no stuck notes,
bounded processing and source-level ownership.

**H. Packaging.** On every supported OS/architecture perform a clean install
and rescan of final artifacts with exact hashes, notarization/signing evidence
only if authorized, and explicit data/SDK license audit.

## Mandatory platform matrix design

| Platform | Portable headless gate | Conditional plugin gate | Human/host gate |
| --- | --- | --- | --- |
| Linux | CMake C++20, CTest, docs/register audit, synthetic offline report | Pinned and authorized JUCE/VST3 build, validator, optional GUI capture with a provisioned display server | Actual supported Linux DAW versions, audio device/jitter and editor accessibility |
| Windows | Same compiled numerical and integration tests, including MSVC-specific warnings | Reviewed JUCE build, VST3 validator, state migration and GUI capture | Actual supported Windows DAWs, device buffers, DPI/font and installation |
| macOS | Same numerical/integration tests, Clang/macOS behavior | Reviewed JUCE VST3 build and validator; AU only if approved as a separate product target | Actual supported macOS DAWs, signing/notarization where authorized, architecture and GUI capture |

This matrix is a specification, not a statement that the conditional jobs
currently exist. Add a CI job when the dependency and source access are
actually configured, and make that job's result mandatory only after the
test route is reliable and legally runnable. Avoid a workflow that claims
successful validation while skipping compilation because the JUCE checkout
is missing. Never put signing secrets or distribution credentials into
untrusted pull-request execution.

## Deterministic host failure corpus

1. `frames=0`, `1`, `2`, common/minimum/maximum supported values, and a sequence
   of irregular partitions that reconstructs one identical event timeline.
2. Repeated note-on before note-off, note-off with absent on, all-notes-off
   during release, multiple simultaneous string assignments and host loop seek.
3. Two events on exactly the same sample, release before engage, tension
   automation at an attack, and contact events at a block boundary.
4. Parameter automation outside supported physical domain and malformed
   nonfinite host values; state chunk truncated at each selected byte offset.
5. Two simultaneously open editor windows/instances, slow preset parsing while
   audio runs, failed asset validation, queued state replacement after host stop.
6. VST3 scan with editor disabled, with GUI available, with changed system DPI,
   and with a different locale/font fallback.
7. Sample-rate changes while a string carries energy, bypass on/off, transport
   stopped but ringing, offline bounce and reopen of the saved project.
8. Deliberately reduced queue capacity so that an emergency release competes
   with continuous motion updates; test priority and stuck-contact prevention.

Every item needs both a *host-visible* expected behavior and the underlying
physical-state expectation. A timeout or crash is a test failure; a silent
fallback can be a safety success and simultaneously a physical-model failure.

## Evidence directory layout for each executed test

```
artifacts/host-qualification/<run-id>/
  manifest.json          # revision, OS/CPU, host/SDK, plugin hash, sample rate
  validator.log          # if executed; include version, exit code and errors
  lifecycle-events.json # normalized offsets, source identity, reset scopes
  physical-trace.csv     # where permissible; units, energy/work, fault counters
  audio-observation.wav  # calibrated or explicitly synthetic and permission-safe
  screenshots/           # actual app/editor captures with rendering metadata
  results.json           # per-case pass/fail/skipped/not-run and issue IDs
```

This is a proposed future artifact layout. Do not check in private DAW files,
licensed binaries, musician recordings or unnecessary personally identifying
information. Attach permitted evidence through CI artifacts or appropriate
access-controlled storage; preserve checksums and retention policy.

Gate owner: `ENG-P14-T01/T02/T03`, `ENG-P16-T01/T02`. For release decisions use
[requirements REQ-PLG-* / REQ-QLF-*](../../roadmap/requirements.json),
[implementation verification](../engineering/verification-matrix.md) and
[realtime budget](../engineering/performance-budget.md).
