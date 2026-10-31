# Prototype JUCE adapter for the setar engine

**Status: source written and configured; full compilation and any host run NOT
performed in this work.** This is an ENG-P14-T01 prototype adapter for
`setar::instrument::Instrument`. It is not a host-qualified, validated or
distributable plugin, and the sound it produces comes from a **synthetic**
physical model, not a measured setar. Qualification follows
[the host playbook](06-host-qualification-playbook.md) and
[product evidence gates](07-product-qualification-evidence-gates.md).

## Files and build switch

| Path | Responsibility |
| --- | --- |
| [`plugin/CMakeLists.txt`](../../plugin/CMakeLists.txt) | `juce_add_plugin(setar_prototype_plugin ...)`, formats VST3 and Standalone, links `setar::instrument` |
| [`plugin/Source/SetarProcessor.h`](../../plugin/Source/SetarProcessor.h), [`.cpp`](../../plugin/Source/SetarProcessor.cpp) | Host lifecycle, MIDI to gesture mapping, block splitting, parameters, state, telemetry |
| [`plugin/Source/SetarEditor.h`](../../plugin/Source/SetarEditor.h), [`.cpp`](../../plugin/Source/SetarEditor.cpp) | Minimal editor: two sliders, two selectors, status and telemetry, permanent SYNTHETIC label |

The root option `SETAR_ENABLE_JUCE_INSTRUMENT=ON` adds `plugin/`. JUCE is never
downloaded by CMake; `JUCE_SOURCE_DIR` must name an existing checkout. Because
the headless project declares only C++, `plugin/CMakeLists.txt` enables C before
adding JUCE (without it, CMake fails with "Missing variable is:
CMAKE_C_COMPILE_OBJECT"; the older `cmake/JuceDiagnostic.cmake` path has the same
omission). Review JUCE's licence before building or distributing anything.

## Boundary and threading

- **Physics stays in the engine.** The adapter never computes string or body
  motion. MIDI notes become physical gestures through
  `setar::instrument::MidiGestureMapper`; the realized pitch is whatever the
  model produces at the chosen fret.
- **`prepareToPlay`** (host lifecycle thread): builds
  `synthetic_setar_config({tuning_id, fret_map_id})` and prepares the engine and
  mapper. The engine capacity is `max_block_frames = clamp(host block, 32, 2048)`,
  256 events per block, 32 interaction channels and 50 Newton iterations. A
  rejected sample rate (outside the engine's 22050–192000 Hz domain) or any
  preparation error leaves the plugin unprepared and silent, with the reason in
  the editor status. Nothing silently substitutes other physical parameters.
- **`processBlock`** (audio thread): clears the buffer, converts MIDI to gestures
  into a fixed 256-entry queue in absolute sample time (stable sorted insertion;
  overflow is counted as `dropped_gestures`), splits host blocks larger than the
  prepared maximum, renumbers event `sequence` per chunk, calls
  `Instrument::process`, applies the observation gain as a per-block ramp and
  publishes telemetry through relaxed atomics. It performs no allocation, locks
  or I/O in this source; that has **not** been verified with an allocation hook
  inside a host (the engine itself is checked by `realtime.allocation_hook`).
- **Tuning / fret-map changes** are polled by a 5 Hz message-thread timer, never
  handled on the audio thread. A change calls `suspendProcessing(true)`,
  re-prepares and resumes. JUCE's wrapper holds its callback lock during
  `processBlock`, so suspension waits for the current block; the adapter's own
  `processBlock` takes no lock. **Re-preparing silences all string motion:** this
  is a documented nonphysical transition, not the physical retuning required by
  REQ-FRT-002.
- `reset()` (host-serialized) zeroes engine state and the gesture queue.
  MIDI CC 120 (All Sound Off) maps to the engine `Silence` gesture. Pitch bend,
  aftertouch and MPE are ignored.

## Stable parameter IDs (version 1)

| ID | Type | Range / default | Meaning |
| --- | --- | --- | --- |
| `outputGainDb` | float | −48 to +24 dB, 0 | Observation gain applied after the engine; never feeds back into mechanics. Engine output is pressure with `output_gain_per_pa = 1`, so 0 dB means 1.0 full scale per pascal at the synthetic observation point. |
| `dynamicsScale` | float | 0.25 to 2.0, 1.0 | Multiplies MIDI velocity before mapping (result limited to 1–127, the MIDI domain), which selects nail stroke speed/release force in the mapper. |
| `tuning` | choice, not automatable | ids of `tuning_presets()`, index 0 | Re-prepares (see above). |
| `fretMap` | choice, not automatable | ids of `fret_map_definitions()`, index 0 | Re-prepares (see above). |

IDs are never renamed or reused. The order of the preset lists is part of the
choice-parameter contract, so the state also stores the IDs.

## State schema (version 1)

`getStateInformation` writes the `AudioProcessorValueTreeState` tree (type
`SetarPrototypeState`) as XML with three extra properties: `schemaVersion = 1`,
`tuningId` and `fretMapId`. `setStateInformation` ignores malformed XML, a
foreign tree type or any other schema version and keeps the current state. On a
valid version-1 state it restores the parameters, then resolves the stored IDs
to choice indices; an unknown ID keeps the restored index instead of guessing.
Migration rules for a future version 2 are not yet written.

## What was actually done here

Environment: Linux container, GCC 13.3.0, CMake 3.28.3, JUCE tag `8.0.10`
(commit `3af3ce0`) cloned into the session scratch directory, never into the
repository. The minimal Linux headers were installed with
`apt-get install libasound2-dev libxrandr-dev libxinerama-dev libxcursor-dev libxext-dev`
(X11 and FreeType headers were already present).

1. **Configure: succeeded.**

   ```sh
   cmake -S . -B build/w7juce -G Ninja -DCMAKE_BUILD_TYPE=Release \
     -DSETAR_BUILD_TESTS=OFF -DSETAR_BUILD_REPORT_TESTS=OFF \
     -DSETAR_ENABLE_JUCE_INSTRUMENT=ON -DJUCE_SOURCE_DIR=/path/to/JUCE
   ```

2. **Full build: not run.** The session's permission policy refused to build and
   execute the externally downloaded JUCE sources
   (`cmake --build build/w7juce --target setar_prototype_plugin_VST3 setar_prototype_plugin_Standalone`).
   No VST3 bundle or Standalone binary was produced, so nothing was loaded in a
   host, validator (pluginval / VST3 validator) or audio device.
3. **Syntax check: passed.** Both adapter translation units were parsed with
   `-fsyntax-only` using the exact compile commands CMake generated (JUCE
   headers, JUCE recommended warning flags, plugin defines). The result was zero
   errors and zero warnings after fixing one `-Wshadow` warning. This proves the
   sources match the JUCE 8.0.10 API; it does not prove that they link or run.

To finish this evidence on a machine where building JUCE is permitted, run
step 1, then `cmake --build build/w7juce --target setar_prototype_plugin_VST3
setar_prototype_plugin_Standalone`, then the validator and host matrix of
[the qualification playbook](06-host-qualification-playbook.md). Record each
result separately per OS, host and format.

## Known limitations

- The mapper drives at most one stopping finger and one damping touch per
  string (its slot 0 and slot 1); ornaments and course strokes are not
  reachable from plain MIDI.
- Telemetry shows totals since load; there is no reset control or energy-ledger
  display. The editor reads only atomics and the message-thread status string.
- `engine_status_` is written in `prepareToPlay`; a host that calls it off the
  message thread while the editor is open would race on that string.
- The adapter inherits the engine defects listed in
  [the realtime evidence](../realtime/05-prototype-realtime-evidence.md),
  including the non-finite sample under channel overflow and the cost of
  dense contact passages exceeding real time on the measured container.
