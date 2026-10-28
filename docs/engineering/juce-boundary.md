# Optional JUCE diagnostic: integration boundary, not finished instrument

## Status and purpose

The optional target demonstrates a narrow separation of responsibilities: the
numerical state lives in the dependency-free C++ core, while JUCE owns host and
window integration. The target is called `Setar Reference Diagnostic` and repeats
the synthetic-fixture limitation in its interface. It is not a completed musical
setar, a preset library, or an acoustically validated plugin.

The default headless build does not compile this source. Therefore a passing
default CI job does not establish that the JUCE target builds or works in a host.
Record an actual optional-target build, scan, and execution separately before
claiming them. The code uses modern JUCE APIs, including `FontOptions`; select
and record a compatible explicit JUCE checkout rather than assuming an arbitrary
old installation will work. The integration was initially authored against the
modern JUCE API contract, not certified against every JUCE release.

This boundary is useful for finding lifecycle and data-ownership problems early.
Its usefulness does not justify adding unsupported physical controls. A fixed,
clearly labeled fixture is easier to compare with the offline baseline than an
interface that quietly changes the physics in response to every MIDI field.

## Dependency and license decision

The repository never downloads JUCE automatically. `SETAR_ENABLE_JUCE=ON`
requires `JUCE_SOURCE_DIR` to point to an existing checkout with a CMake project.
An absent or invalid directory produces a configure error. The caller must
review the terms attached to the exact dependency revision and the intended
distribution. This repository does not choose a JUCE license, imply a commercial
entitlement, or supply legal advice about a product's distribution.

Do not copy a random global JUCE installation into the source tree to make a
build pass. Prefer an identified checkout outside the repository and retain its
commit identifier in the build record. Do not modify that checkout without
recording the patch. If the project later chooses a submodule or managed package,
that decision should preserve the explicit version and license boundary rather
than reintroduce an unpinned network fetch at configure time.

The primary integration reference is the official
[JUCE CMake API](https://github.com/juce-framework/JUCE/blob/master/docs/CMake%20API.md).
The selected checkout's own documentation and source are authoritative for the
actual build. The current project requests Standalone and VST3 formats, disables
automatic copying into system plugin directories, and does not enable a web
browser or Curl in the diagnostic.

## Build procedure

Use a distinct build directory so optional system dependencies do not leak into
the headless baseline:

```sh
cmake -S . -B build/juce-diagnostic -DSETAR_ENABLE_JUCE=ON -DJUCE_SOURCE_DIR=/absolute/path/to/JUCE -DCMAKE_BUILD_TYPE=Debug
cmake --build build/juce-diagnostic --config Debug --target setar_diagnostic_Standalone setar_diagnostic_VST3
ctest --test-dir build/juce-diagnostic -C Debug --output-on-failure
```

Replace the example path with the actual reviewed checkout. On Windows, choose
an appropriate generator and architecture as described in the engineering setup.
On Linux, JUCE's audio and GUI modules may require development headers and system
libraries that the headless core does not need. Diagnose those dependencies from
the selected JUCE revision and configure output; do not add speculative package
lists or network installers to the numerical core.

CTest still exercises the core and offline integration. It does not automatically
launch the standalone window, scan the VST3, drive a DAW, or test audio hardware.
Those are separate execution steps with separate evidence. The output bundle
location is determined by JUCE and the chosen generator/configuration; use the
actual generated path rather than a hard-coded path from another platform.

## Current host behavior

The diagnostic has no input audio bus and supports mono or stereo output. Both
stereo channels receive the same point-displacement observation. This is dual
mono, not a radiation directivity model or an acoustic stereo field.

Preparation constructs the same default 16-mode reference used by the offline
renderer, at the host's sample rate. The pluck height, position, pickup, and gain
are fixed. A button requests a retrigger at the next audio block. A short MIDI
note-on with nonzero velocity retriggers the same prepared excitation at its
clamped sample offset in the current block. Multiple note-ons replace the state;
there is one monophonic fixture, not a polyphonic voice allocator.

MIDI note number does not retune the string. Velocity does not change height.
Note-off, sustain pedal, pitch bend, channel pressure, and other controllers do
not alter the model. These omissions are visible in the editor. They are
intentional capability boundaries, not hidden bugs to be explained away after
someone tries to play a melody. Implementing them later requires a physical
mapping and a host-event contract.

The diagnostic accepts no persistent user parameter state. State serialization
is empty and program operations expose one fixed synthetic fixture. It therefore
does not demonstrate preset migration, parameter automation, undo behavior, or
host restoration of an expressive instrument state. Those features are proposed
work, not silently supplied by subclassing `AudioProcessor`.

## Audio-thread and UI ownership

`prepareToPlay()` is the preparation boundary. It allocates core storage and
precomputes excitation and transition coefficients. `processBlock()` clears the
output, consumes the atomic UI trigger, iterates relevant MIDI events, and runs
the prepared fixed-state renderer. No model configuration or vector allocation
belongs in that processing loop.

The processor overrides the host's `reset()` operation: it clears the live modal
state and the pending UI trigger, while retaining preparation, the configured
pluck coefficients, and the current ready status. With no new MIDI or UI event,
the next prepared block is therefore silent. A later retrigger still uses the
same prepared fixture. `releaseResources()` performs that reset and then marks
the processor not ready; a subsequent `prepareToPlay()` must succeed before
processing resumes.

Clearing the atomic trigger defines the reset cut-off. Requests ordered before
that clear are discarded; a UI request ordered after it is a new request, not an
old request revived by reset. A request made while released cannot survive the
next preparation's trigger clear. This does not make concurrent host reset and
audio rendering valid: the host must serialize lifecycle and processing calls.
The optional target remains unbuilt until its separately recorded build and
host tests are performed; headless reset tests cover the core, not this adapter.

The UI button communicates through an explicitly lock-free atomic boolean.
Multiple button clicks before consumption can coalesce into one request; this is
a diagnostic trigger, not an event queue with guaranteed click delivery. A future
gesture interface that needs every event must use a bounded, documented event
transport and an overflow policy rather than pretending a boolean is a queue.

The MIDI loop inspects short raw event bytes rather than constructing an owning
message from arbitrary SysEx payloads. That avoids a subtle allocation path for
data the fixture does not need. Recognized note-ons retain sample-offset order;
unrecognized messages are ignored. At most 4,096 events are inspected in one
block; the diagnostic's explicit overload policy is to ignore the remainder.
This bounds event-flood processing but is not a finished instrument's event-loss
policy. A future production boundary needs overflow telemetry and deliberate
priority rules. The host buffer is cleared of MIDI output
because the plugin does not produce MIDI.

The processor and core follow host lifecycle serialization. The core itself is
not internally synchronized. Do not call `prepare()` from the editor while the
audio thread renders. If future parameter changes require new coefficient
storage, prepare an immutable candidate away from the callback and design an
explicit ownership handoff and reclamation policy.

## Failure behavior and inspection

Unsupported sample rates or preparation errors leave the diagnostic silent.
They do not silently reduce modal count, substitute a different physical string,
or normalize an invalid state. This is a minimal safe boundary behavior, not a
complete user-facing error-reporting system. A production application needs a
bounded, nonblocking status channel that explains the unsupported configuration
without logging or allocating on the audio thread.

The tail estimate is a fixed conservative presentation value for the selected
decay, not a calibrated instrument measurement. There is no automatic state
zeroing at that time. Host bypass, suspension, block-size changes, device loss,
and editor reopen behavior need actual host tests. A successful standalone launch
does not prove any of those conditions in a plugin.

For a genuine screenshot, build and run the actual target, retain the dependency
revision and platform, and capture the rendered window. Do not use the HTML
report to imply that a JUCE editor ran. Conversely, an editor screenshot does not
prove that audio processing, MIDI offsets, or physical energy were correct. Pair
visual evidence with the appropriate numerical and host evidence.

## Acceptance before expanding the boundary

Before adding a musical parameter, specify its stable identifier, units, physical
meaning, legal range, automation rate, interpolation behavior, state-version
representation, and invalid-state policy. Test it at block boundaries and inside
blocks. Determine whether changing it adds external work to the physical model;
parameter smoothing is not automatically energy neutral.

Before calling the optional target supported, build it on each claimed platform,
exercise the actual supported formats in named hosts, verify scan and unload,
inspect state round trips, run block/sample-rate variations, measure real-time
behavior, and review the visible limitations and accessibility. Keep the
headless numerical comparison available throughout. JUCE should remain a clear
boundary around verified physics, not a second undocumented synthesis engine.
