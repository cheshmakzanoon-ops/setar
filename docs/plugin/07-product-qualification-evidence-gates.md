# Actual VST3, realtime, screenshot and perceptual qualification evidence gates

**Status: executable evidence checks, zero completed real-host qualifications.**
The project has no measured four-string setar engine, finalized VST3 build,
licensed DAW test run, authenticated live editor screenshot, hardware
real-time trace or controlled listening/playing study. A passing headless
CI run is important, but it cannot establish those product facts.

The [blank 13-gate qualification matrix](../../data/product-qualification-template.json),
[fail-closed validator](../../tools/validate_product_qualification.py) and
[mutation tests](../../tools/test_evidence_gates.py) make it difficult to
accidentally publish a green qualification report without actual evidence.
They do not create the missing plugin, laboratory participants or host
software. Read [the host lifecycle playbook](06-host-qualification-playbook.md),
[realtime contract](../realtime/README.md),
[UI capture contract](../ux/05-live-engineering-graphs-and-screenshot-contract.md)
and [final fidelity acceptance](../validation/09-final-fidelity-acceptance.md)
before recording a pass.

## Exactly which product matrix is required in the current template

| Scope | Validator/format conformance | Real independent host | Physical realtime profile | Running UI capture |
| --- | --- | --- | --- | --- |
| Linux | `linux.validator` | `linux.real_host` | `linux.realtime` | `linux.ui` |
| Windows | `windows.validator` | `windows.real_host` | `windows.realtime` | `windows.ui` |
| macOS | `macos.validator` | `macos.real_host` | `macos.realtime` | `macos.ui` |

The thirteenth gate, `global.perceptual`, is a separate blind, preregistered
listener/player evaluation of a defined fidelity claim. One positive
reviewer, a promotional demo or a generated screenshot is not a blind study.

Each OS row must record the exact architecture, kernel/OS version, plugin
binary hash, dependency and SDK versions, compiler and build configuration,
case identifiers, who ran it and what logs/screenshots/trace files were
preserved. Intended support may be narrowed in a formally reviewed product
scope ADR; simply deleting an inconvenient row from the current manifest
fails validation. Current coverage target is all three operating systems.

### Gate A: a real VST3 plugin and independent validator

**Before a test run:** approve actual JUCE source/license and, if needed,
Steinberg SDK version and license; prepare a complete tested physical engine
or clearly identify an earlier synthetic *diagnostic* plugin. Verify plugin
class/parameter identity, state-compatibility policy, bus arrangement and
expected audio render; build a real VST3 binary and hash it. Store the source
revision and reproducible dependency lockfile.

**Perform:** invoke Steinberg's official command-line validator against the
**actual built plugin bundle**. Preserve command, stdout, stderr, exit
code, tool version, native plugin path, file/bundle hash, runner identity,
all warnings and failures. The [official VST3 validator](https://steinbergmedia.github.io/vst3_dev_portal/pages/What%2Bis%2Bthe%2BVST%2B3%2BSDK/Validator.html)
is a conformance tool, not a physical fidelity measure. Run the
[Steinberg VST3 test host](https://steinbergmedia.github.io/vst3_dev_portal/pages/What%2Bis%2Bthe%2BVST%2B3%2BSDK/Plug-in%2BTest%2BHost.html)
as an additional lifecycle check where a supported binary/environment exists.
Do not describe a validator log created by a synthetic test script as an
official run; retain a chain from the actual process and output to the
binary hash. A zero exit code is necessary, not sufficient.

**Fail when:** plugin bundle cannot load, scan fails, class identity changes
unexpectedly, a validator assertion fails or the tested hash differs from
the binary being distributed. Unsupported architectures are not silently
counted as another platform's pass.

### Gate B: a real DAW/host, not the validator renamed

Choose actual authorized hosts relevant to declared supported users, with
exact versions. For each host install, fresh-scan, instantiate, load a preset,
send events with exact sample offsets, resize blocks, switch sample rate,
automate physical parameters, save/reload project, render offline, bypass,
stop/start transport, suspend/resume, create multiple instances and reopen
editors during active sound. Capture input event streams and compare a
headless differential render *when a production core exists*.

For VST3 specifically test:

1. Note-on/off ordering at the same sample, overlapping four-course gestures,
   release while sympathetic motion remains, all-notes-off versus emergency
   silence, and repeated note-on state memory.
2. Zero, one, short, irregular, maximum-admitted and larger-than-admitted
   callback blocks; sample-rate/bus changes and channel mismatch.
3. Versioned state save/load, malformed chunks, nonexistent assets, rollback
   after rejection and no collision between physical state and host note ID.
4. Parameter automation at attack/release boundaries, automation while editor
   is closed, MIDI/MPE limits and deterministic same-time event ordering.
5. Offline rendering, host vs core parity where meaningful, bypass tail,
   project rescan and clean instance destruction while sound is active.

Preserve a per-test result and a failure reproduction file. `Steinberg
validator` is explicitly barred from being the value of `host_name` for a
real-host pass. If a proprietary host is unavailable or prohibited by
license, mark `blocked` or `skipped` with a reason; never write `passed`.

### Gate C: measured realtime performance

Compile and profile the **actual shipping-style render path**, not the dense
offline prototype. Freeze instrument preset, maximum active contacts,
string/body modes or grid density, scene, sample rate and block sizes.
Measure on actual named hardware/OS/architecture, under a repeatable event
stress corpus. A CPU utilization percentage from one easy musical phrase
does not measure worst-case callback duration.

At each supported rate and admitted block size:

- Record callback entry/exit monotonic timestamps, count and jitter; compute
  maximum, p99 and distribution by operating condition, not just a mean.
- Record audio-thread heap allocations, waiting/locking, file/network I/O,
  page faults where instrumentable, thread wake-ups and missed deadlines.
  Source inspection alone is partial evidence; use platform instrumentation.
- Compare measured worst callback duration against the *declared* per-buffer
  budget with margin for host/audio-driver work. For instance, 64 samples
  at 48 kHz have a nominal 1.333 ms buffer interval, not an automatic
  promise that 1.333 ms of it is available to the plugin.
- Test rapid fret changes, repeated nail contacts, four strings with existing
  energy, dense simultaneous event loads, state swaps and editor activity.
  Handle overload by a documented bounded containment response; the failure
  must be reported and physically invalid output cannot be silently certified.
- Profile before and after model reduction or multirate changes and compare
  physical errors to the high-rate reference. Do not trade away release
  timing or power-port stability without quantifying it.

The manifest validator requires numeric `sample_rate_hz`, `block_size`,
`worst_callback_ms`, `p99_callback_ms`, `declared_budget_ms`, an actual
hardware-profile ID, callback-trace ID, evidence-file hash and reviewer.
It rejects `worst_callback_ms < p99_callback_ms` and a purported pass whose
worst-case callback exceeds its declared budget. These metadata checks
cannot establish that the trace was genuinely recorded; independent review
and an actual run are required.

### Gate D: genuine running UI screenshots and graph evidence

A design mockup, wireframe, offscreen synthetic graph or manually drawn
```.svg``` is valuable design documentation but **not a screenshot from a
running plugin**. For each supported platform, instantiate the actual
plugin in a real permitted host and capture PNG screenshots with the OS
screenshot mechanism. Link the screenshot to host, plugin hash, revision,
viewport, scaling, locale, font availability, physical controls shown and
live performance/diagnostic preset. Capture multiple relevant UI states:
attack, decay, sympathetic response, contact/fret position, error and
calibration/unknown markers.

The current metadata validator checks PNG signature, the IHDR dimensions
(at least 640 × 360), a real file hash and a declared
`live_host_screen_capture` method. It **cannot** tell a fraudulent PNG from
an actual host capture. An independent human reviewer must compare the
running application and screenshot, inspect access to keyboard/screen-reader
controls, and verify all plot axes/units/provenance. Screenshot regression
checks need deterministic rendering conditions and a documented tolerance;
such a regression suite does not yet exist.

### Gate E: perceptual fidelity and performer usability

First preregister which claims are being evaluated: instrument identity,
response to dynamics, pluck/release, sympathetic strings, intonation,
idiomatic Iranian setar gestures, latency/playability, and blind realism
against a particular reference instrument and recording chain. Record
the acquisition permissions, independent test set, normalization policy,
exact software/binary revision, microphones/room assumptions and anticipated
confounds. A sampled reference must be level-matched, but the comparison
must not erase physically informative attack or dynamic response.

Use a balanced blind randomization with separated listener and practitioner
tasks, trial ordering, repeat conditions, missing responses, consent and
ability to withdraw. Select participant counts via a documented statistical
precision/power rationale rather than an invented universal threshold.
Archive permitted stimuli, randomization commitments, analysis code,
predeclared effect sizes/intervals and all negative outcomes. Separate
"sounds plausible in one excerpt" from "expressive control behaves like
this specimen". A listener study without active performer interaction
cannot by itself certify playability.

The manifest requires a preregistration reference, consent ledger,
randomization record, analysis script hash reference, sample-size rationale,
coordinator and positive actual participant count. These are only
structural preconditions. The model must pass a real preregistered criterion
on held-out material to substantiate the associated fidelity claim.

## The machine-readable gate protocol

The published template contains **all 13 required cells and zero executions**:

```sh
python3 tools/validate_product_qualification.py data/product-qualification-template.json
python3 tools/test_evidence_gates.py
```

The default validator must report `passed: 0` and `not_executed: 13`.
To record real results, copy the template into an evidence-controlled
qualification directory, set `evidence_class` to
`actual_product_qualification_record`, identify source revision, product
and actual binary hash, then append one `executions` entry per cell with
`id`, `status` (`passed`, `failed`, `blocked` or `skipped`), platform and
runner/tester/reviewer/host metadata, plus SHA-256-referenced evidence files
relative to that private manifest. Add kind-specific fields described above.

```sh
python3 tools/validate_product_qualification.py private-qualification/manifest.json
python3 tools/validate_product_qualification.py private-qualification/manifest.json --require-qualified
```

The second command is **supposed to reject the public blank template and
any skipped or failed cell**. It checks completeness and file identity,
not the authenticity or physical adequacy of the submitted results. Real
independent review is still required before a product claim.

CI runs the template linter and deliberate rejection/mutation tests, but
does not run a licensed DAW, human listener, standalone UI screenshot tool
or Steinberg validator by merely checking that this schema exists. A future
CI host job must distinguish `unavailable` from `passed`, record the real
host process and preserve evidence artifacts. A green Linux core build,
Windows CTest and macOS CTest are portable software verification, not
three actual VST3 host qualifications.

## Signoff and honesty policy

Product owners and engineers should keep a visible *unqualified until
proven* status for each claimed platform, specific host and feature.
Reviewers must inspect actual binary, run, raw trace and consent evidence,
not merely check that hashes are formatted correctly. The scientific
acceptance of a physically modeled Iranian setar additionally depends on
the [real specimen acquisition campaign](../measurement/13-acquisition-campaign-execution.md),
[solver selection experiments](../architecture/07-experimental-solver-evidence-and-choice.md)
and held-out performer/acoustic outcomes. There is no deadline in this
evidence-driven progression, and no claim that any finite checklist can
establish absolute identity down to every physical detail.
