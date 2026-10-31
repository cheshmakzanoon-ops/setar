# Realtime evidence for the playable prototype engine

**Status: executable partial evidence for REQ-RTS-001 and REQ-EXC-002 on one Linux
container. Not a host, DAW, operating-system or audio-hardware qualification.**
The engine is `setar::instrument::Instrument`
([`instrument.hpp`](../../include/setar/instrument/instrument.hpp)); its
callback contract is the one proposed in [the callback contract](01-callback-contract.md)
and [event capacity](02-events-capacity-and-overflow.md). This chapter records
what the realtime tests and the benchmark actually exercise, the numbers they
produced, and the defects they exposed.

## What the CTests establish

The runner is [`tests/instrument/realtime_tests.cpp`](../../tests/instrument/realtime_tests.cpp).
It builds a hand-made **synthetic** fixture (four strings, eight fixture frets,
a seven-mode body, strings truncated at 6 kHz so Debug runs stay short) and a
phrase that uses every gesture kind: fretted strokes, two finger slots on one
string, a course pair, slides across frets, re-strikes of a ringing string,
releases and `Silence`. Events are given in absolute sample time and sliced
into blocks by the test, exactly as a host adapter must do.

| CTest | Property checked | Negative control / limit |
| --- | --- | --- |
| `realtime.allocation_hook` | Zero calls to every replaceable `operator new`/`delete` during `process()`, `snapshot()`, `ledger()` and `reset()`, with the energy ledger off and on, block sizes 1, 64 and 512 | The hook must first count a deliberate allocation, so a broken hook cannot pass. `malloc` called directly by a library would not be seen. |
| `realtime.block_partition` | Bitwise-identical left/right output for block sizes 1, 7, 64, 256 and 512 with identical absolute event times | Reports the first differing sample if any. One fixture phrase only. |
| `realtime.max_work` | `max_newton_iterations` never exceeds `ProcessSpec::max_newton_iterations`; `max_active_channels` never exceeds the prepared capacity; no capacity overflow, rejection or containment in a low-action buzz stress with all strings struck and both finger slots active | Iteration and channel *counts* are bounded; the wall-time printout is informational, not an assertion. |
| `realtime.capacity_overflow` | (a) 12 events into an 8-event block: `EventOverflow`, exactly 4 rejected, block still processed. (b) Smallest accepted channel capacity, overloaded: `ChannelCapacityExceeded` reported and the solver never exceeds the prepared size. (c) Default capacity 32 under the stress phrase: overflow reported **and** no non-finite output sample | Case (c) currently **fails**; see defects below. |
| `contact.gesture_replay` | Two fresh instances, and one instance after `reset()`, produce bitwise-identical audio and energy ledgers; a changed release force changes the output | The engine has no random-number source, so there is no seed to vary. |

Execution on this container (GCC 13.3.0, Linux, 4 vCPU shared with other build
jobs), revision under development:

| Build | allocation_hook | block_partition | max_work | capacity_overflow | gesture_replay |
| --- | --- | --- | --- | --- | --- |
| Debug | pass, 16.5 s | pass, 12.9 s | pass, 16.7 s | **fail** (c), 1.8 s | pass, 11.4 s |
| Debug + ASan/UBSan | pass, 45.7 s | pass, 33.7 s | pass, 49.6 s | **fail** (c), 4.9 s, no sanitizer report | pass, 32.8 s |

Windows/MSVC and macOS have not been run by this work; the CI workflow is a
configuration, not a result. MSVC Debug checked iterators may run the dense
stress more slowly than the 120 s CTest limit; that has not been measured.

## Defects found by these tests

1. **Non-finite output under channel overflow** (`realtime.capacity_overflow` case c).
   The engine checks finiteness of the observed pressure in `double`
   (`src/instrument/instrument.cpp`, observation block in `step()`), then casts to
   `float` when writing the output span. A finite but huge `double` (the state
   was already diverging after 26 Newton failures caused by dropped channels)
   becomes `inf`, so one sample violates the `NonFiniteState` contract ("output
   for that sample is 0"). The containment check needs to test the value that is
   actually written, and the divergence after overflow needs its own fix.
2. **Divergence when a pressed finger is re-placed.** A `FingerPress` on an
   already-active slot relocates the pad instantaneously while it is pressed
   below the fret line. The engine books the potential jump as external work,
   so the ledger residual stays near `1e-14` J, yet repeated relocation pumped the
   string energy from about `7e-3` J to `3e4` J within 3 s; Newton then failed
   and the state diverged (peak output `6.7e34`). Lifting before re-pressing is
   stable. The ledger is consistent; the gesture semantics are physically
   unrealistic and need either a finite-speed relocation or rejection.
3. **Iteration report above the cap.** With the full synthetic preset the
   summary reported `max_newton_iterations = 100` while the cap was 50, which
   suggests two solve passes are summed per sample. Either the summary or the
   contract wording needs to change; `realtime.max_work` passes only because the
   fixture needs at most 12 iterations.

## Benchmark: wall time of `process()` on this container

[`apps/setar_benchmark.cpp`](../../apps/setar_benchmark.cpp) renders a dense,
deterministic synthetic pattern (a gesture group every 60 ms: fretted strokes,
slides, lifts, alternate stroke directions and course strokes) on
`synthetic_setar_config()` for each sample rate and block size and prints CSV.
`cpu_load` is wall time divided by audio time on one core; values above 1 mean
slower than real time. Command and environment:

```sh
cmake -S . -B build/w7rel -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build/w7rel --target setar_benchmark
./build/w7rel/setar_benchmark --seconds 5
```

GCC 13.3.0 Release (`-O3 -DNDEBUG` from CMake defaults plus the project
warning flags), energy ledger off, `max_interaction_channels` 32, preset
"synthetic-c4-g3-c3-c4" with fret map "pythagorean-17-safi-al-din", 71/88/112/71
string modes per polarization and 48 body modes (732 degrees of freedom). The
1-minute load average was 1.2–1.6 during the run because other jobs shared the
machine, so these are upper-bound-contaminated single measurements, not a
controlled benchmark.

| Rate (Hz) | Block | cpu_load | p50 block (µs) | p99 block (µs) | max block (µs) | Budget (µs) | Max channels | Max Newton it. | Overflows |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 44100 | 64 | 1.54 | 2173 | 3885 | 7206 | 1451 | 31 | 21 | 0 |
| 44100 | 256 | 1.57 | 8814 | 16219 | 18782 | 5805 | 31 | 21 | 0 |
| 44100 | 512 | 1.56 | 17662 | 31507 | 48318 | 11610 | 31 | 21 | 0 |
| 48000 | 64 | 1.63 | 2101 | 4305 | 11510 | 1333 | 32 | 27 | 0 |
| 48000 | 256 | 1.63 | 8435 | 16295 | 21407 | 5333 | 32 | 27 | 0 |
| 48000 | 512 | 1.59 | 16758 | 27071 | 38133 | 10667 | 32 | 27 | 0 |
| 96000 | 64 | 2.69 | 1749 | 3025 | 5160 | 667 | 32 | 15 | 3 |
| 96000 | 256 | 2.73 | 7067 | 12009 | 19980 | 2667 | 32 | 15 | 3 |
| 96000 | 512 | 2.69 | 14152 | 21573 | 25187 | 5333 | 32 | 15 | 3 |

No Newton failures, rejected events or containment resets occurred in these
runs; the 96 kHz runs reported 3 channel-capacity overflows. The engine is
therefore **not real-time on this container for this dense pattern**. A
separate timing of simpler patterns on the same build (ledger on, 2 s of
audio, 48 kHz) gave about 0.33 for nail strokes only (8–9 active channels) and
0.69 for fretted notes with lift-before-press (10–15 channels), so cost tracks
the number of simultaneously active interaction channels. That attribution is
an inference from three runs, not a profile.

## Next actions

- Fix defect 1 at the float write and add a regression to the engine tests.
- Decide the physical semantics of re-pressing an active slot (defect 2).
- Profile the channel-dependent cost (dense LU of the channel system and
  per-channel O(modes) gathers) before reducing it; any reduction needs
  `realtime.reference_parity` evidence against the unreduced engine.
- Run the same CTests under MSVC and Apple Clang and record them separately.
