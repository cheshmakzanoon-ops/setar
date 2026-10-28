# Live engineering graphs, musician interface and authentic visual evidence

**Status: proposed interface and visual-test contract.** The repository
currently includes actual plots of a **synthetic numerical string**, an offline
HTML report, and an SVG **wireframe** of a possible musician interface. These
are not screenshots of a working setar VST3. Do not label wireframes or
generated diagrams “the final plugin,” and do not treat a chart as proof of
correct measured physics without the underlying manifest and independent test.

## Two connected interface modes, different purposes

### Musician view

The musician view is optimized for idiomatic expression: physical string
selection, tuning/microtonal configuration, right-hand gesture/nail response,
left-hand fret/contact movement, damping, microphone/observation choice,
output and appropriate presets. Every control has a visible help description
and a classification: **measured physical parameter**, **calibrated inferred
parameter**, **performer gesture**, **observation/post-processing**, or
**explicitly nonphysical convenience behavior**. A virtual “body material”
knob cannot be presented as a physically measured property if it only applies
an arbitrary EQ.

Do not require the performer to understand PDE time-step choices. Sensible
physical controls should be mapped into stable validated parameter domains;
advanced coupling/solver settings belong behind an engineering mode with
appropriate warnings and state migration behavior. Changes that require
preparation must not block the audio thread or silently reset existing strings.

### Engineering inspector

This mode is for reproducible investigation: selected physical string and
contact state, active model/asset SHA, parameter provenance, per-port energy,
nonlinear solve diagnostics, geometry/coupling and acoustic response. It can
pause an offline reference and export full traces. The realtime inspector
reads fixed-capacity snapshots only and explicitly reports decimation and
dropped telemetry. It must not directly iterate over mutable solver vectors
from a GUI thread.

The user can switch between musician/editor and laboratory/report views, but
the switch is not a change in physical simulation unless a control explicitly
requests one. A visual refresh or editor reopening must not inject new
excitation or change string energy.

## Required graph inventory and exact axes

| View | X-axis | Y-axis and units | Necessary metadata/guard |
| --- | --- | --- | --- |
| Waveform / string state | Physical time in s or ms | Displacement m/mm, velocity m/s or calibrated pressure Pa—separate scales | Physical point, observation gain, synthetic/measured |
| String shape | Position along string m | Two transverse displacements m/mm | End boundaries, fret/nail marker, current time, interpolation |
| Energy/work ledger | Time s | Mechanical/acoustic energy J, external work J, dissipated J, residual J | Define stored vs cumulative quantities, sampling, numerical tolerance |
| Modal map | Mode frequency Hz | Decay 1/s, residue in declared units, optional uncertainty | Origin of poles, mode rank, unresolved/missing modes |
| Dispersion | Mode index or Hz | Frequency deviation cents or Hz | Ideal string reference, EI/T, resolution and band |
| Bridge mobility | Frequency Hz | Complex FRF magnitude in (m/s)/N and phase rad/deg | Force/velocity axes, coherence, window, support conditions |
| Nail/fret contact | Time s | Penetration m, normal force N, sliding speed m/s | Contact mode/active-set history, camera/force calibration |
| Directivity | Angle degrees / frequency Hz | Pressure Pa or relative level dB with explicit reference | Mic radius, room treatment, calibrated or synthetic data |
| Realtime performance | Block index/time s | Callback time µs; deadline µs and xrun markers | CPU/OS, host, block, rate, instance count, profiler cost |
| Calibration/sensitivity | Physical parameter or singular-vector index | Sensitivity (stated units), posterior/uncertainty interval | Held-out/train label and parameter covariance |

All graphs need labeled axes, data units, source revision, model identity,
sample rate/time origin, observation location and whether data are synthetic,
measured, inferred or simulated from a fitted model. Avoid axes labeled simply
“amplitude” when they actually show displacement, energy, digital PCM or
pressure. A logarithmic axis requires its reference quantity and any floor.

Where useful, display a **shaded measurement uncertainty** band or independent
oracle envelope; do not turn it into the width of a cosmetic stroke. A graph
that says “simulated vs reference” must identify whether reference means
analytic truth, independently converged model, or an instrument recording.
Offer raw CSV/JSON export so reviewers can regenerate axes and residuals.

## Architecture of visual telemetry

1. The solver owns numeric physical state, not UI widgets. It computes at its
   own internal time resolution with a defined numerical scheme.
2. A bounded snapshot publisher copies selected, finite, dimensioned values
   into a preallocated queue/ring; it may decimate values for realtime display
   but must not silently decimate acceptance evidence.
3. The inspector consumes snapshots on a non-audio thread and retains a
   coherent asset/model ID per sample batch; it does not keep spans into live
   physical vectors after asset publication.
4. Chart rendering/formatting, CSV export, screenshot capture and analysis
   execute outside the callback. UI frame drops may be acceptable; dropped
   physical events are a separate, high-priority fault.
5. Reference-study graphs are regenerated offline from complete saved traces,
   never a screenshot of an already truncated realtime meter buffer.

## Deterministic capture procedure

For each screenshot, record `capture_id`, app/host executable and version,
source revision, preset/asset hash, underlying test/fixture, platform, display
scale, viewport, theme, locale, font availability, animation state, timestamp
and generation command. Use a fixed test render/evidence trace with a
permitted source. Capture the **actually rendered interface**, without
retouching changed numeric values or manufacturing an editor in image software.

For an HTML scientific report, use a pinned browser and viewport if available;
for JUCE editor, use an authorized desktop environment and actual plugin
instance. Headless Linux CI may need a separately provisioned virtual display;
Windows/macOS require corresponding rendering support. Do not assume a
browser launcher or VST host exists on every CI runner. When it is unavailable,
record “capture not run” and retain graph data/structural verification rather
than inventing screenshot evidence.

Visual testing has three levels:

- **Structural:** named controls/graphs exist, have accessible labels/units,
  valid ranges, stable IDs, focus order, reasonable layout constraints, and
  no crash when an editor is resized or reopened.
- **Deterministic data:** saved SVG/HTML plot resources parse and use the
  expected numeric manifest, axes, annotations, synthetic/measurement labels
  and independent input hashes.
- **Pixel/perceptual:** actual screenshots are compared to an approved
  platform-specific baseline under pinned fonts/scale. Store diff maps and
  changed-region explanations; use tolerances only for known rasterizer
  variation, not to ignore hidden or clipped controls.

The existing `tools/generate_report.py` implements limited deterministic
scientific data/plot generation; `tools/capture_report.py` offers an optional
capture path. Neither is a production JUCE editor test, and browser launch
availability must be reported from the actual environment. Any future visual
CI job must retain a genuine captured PNG and its metadata before it can
claim screenshot coverage.

## Musician review and localization

Instrument terminology should be reviewed with Iranian setar performers,
including Persian-script labels where useful, rather than borrowed
uncritically from guitar or Indian sitar plugins. Verify right-to-left label
rendering, microtonal tuning notation, keyboard accessibility, screen-reader
labels, high-DPI scaling, contrast and color-independent error indicators.
The input hardware's limitations should be stated: a MIDI keyboard cannot
directly express every high-dimensional nail/finger contact gesture without
an explicit mapping or external controller.

**Acceptance:** a working editor with actual host-state screenshots,
practitioner review, non-blocking telemetry and deterministic graph data is
required before `REQ-UXI-001/002` can move past `specified`. The present
[wireframe](assets/musician-wireframe.svg) remains a design illustration.
See [ENG-P15-T01/T02](../../roadmap/engineering-tasks.json),
[visual QA fundamentals](03-visual-testing-and-screenshots.md), and
[host qualification](../plugin/06-host-qualification-playbook.md).
