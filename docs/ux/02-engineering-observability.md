# Engineering observability without disturbing synthesis

The engineering view should make a physical or software hypothesis easier to
test. It is not a decoration layer, and it must not make the audio thread wait
for a graph. This chapter specifies proposed observability contracts for numerical
state, spectra, energy, events, and performance. The actual report generator may
provide only a subset; consult its generated manifest and current source rather
than treating every panel described here as implemented.

## Separate three evidence surfaces

The live diagnostic view provides bounded, possibly decimated observations of a
running engine. The offline experiment report provides complete reproducible
numeric output for a fixed fixture. The comparison report joins a candidate with
an independent reference or measurement. Each surface should identify which it
is. A live screenshot is not a substitute for an offline convergence study.

Live telemetry may be lossy. Show the sampling/decimation policy, data age, and
dropped-record count where relevant. Offline reports should preserve complete
data within the declared experiment scope. Comparison reports must preserve
both source identities and every alignment/normalization operation.

## Panel contracts

| Panel | Required meaning | Common misleading alternative |
| --- | --- | --- |
| Waveform | Observable, units, channel, time origin | Unlabeled normalized trace |
| Spectrum | Window, resolution, reference level, frequency units | Auto-scaled curves hiding missing energy |
| Decay | Band/mode assignment, fit interval, noise floor | One line through a multi-slope tail |
| Energy | Storage terms, input work, dissipation, residual | Output amplitude called total energy |
| Events | Canonical time, target identity, accepted/rejected status | MIDI note number alone |
| Solver | Residual definition, iteration count, fallback incidents | A green light with no criterion |
| Performance | Hardware/context, timing distribution, workload | One unexplained CPU percentage |
| Provenance | Model, data, build, configuration, evidence state | A generic “validated” badge |

The panel's numeric definition is part of its API. Changing the definition
requires a report/schema revision even if the visual appearance stays the same.
For instance, changing a spectrum from amplitude to power changes decibel
interpretation. Tests must detect that semantic change.

## Energy view

Display energy terms corresponding to the actual model: string kinetic and
potential terms, body terms, contact storage if present, external work, and
dissipative losses. State sign conventions. A residual such as change in stored
energy minus input work plus loss needs an exact definition and normalization.
Do not label it “error” without explaining its expected numerical behavior.

Show cumulative and local residuals where useful. A cumulative trace can hide
alternating positive and negative errors; a local trace can make harmless
roundoff appear alarming. Include an uncertainty or tolerance band justified
for the test. During moving constraints, external work can be nonzero even when
there is no pluck event. The display must not classify all energy growth as
instability without accounting for that work.

If the production model omits a term that the reference model includes, mark the
comparison's scope. A modal energy approximation can be useful, but should not
be presented as a complete continuum energy measurement. Explain what truncation
or observation prevents the panel from seeing.

## Spectrum and decay view

Keep candidate and reference axes linked. Frequency may be linear or logarithmic
depending on the question; label units and limits. Amplitude/power decibels need
a declared reference. Use consistent color scales for spectrogram comparisons.
Do not auto-normalize each candidate independently unless the experiment explicitly
permits level normalization.

Expose analysis settings without making them editable during a sealed final
comparison. Development reports can explore window size and fit intervals;
confirmatory reports should show the frozen choices and sensitivity analyses.
Store numerical feature tables alongside images so a reviewer can inspect exact
values and uncertainty.

For decay plots show measurement noise and unresolved regions. Do not draw a
continuous fitted line through a region where the partial cannot be tracked.
If a late decay is extrapolated, visually distinguish extrapolation from observed
data. Mark synthetic fixtures clearly and avoid naming them after a real specimen.

## Event and gesture trace

Display requested events, accepted canonical events, and physical response as
separate tracks. This reveals whether a mismatch arises in controller mapping,
scheduling, or physics. Include source, note generation, physical target, and
sample time. An event log that contains only pitch and velocity cannot diagnose
reused-channel or stolen-voice failures.

For dense streams, aggregate visually while preserving the raw bounded capture
or offline event file. Indicate dropped records. Do not interpolate missing
gesture points as though they were observed. A replay action should use the
captured canonical input and configuration, not reconstruct events from pixels.

Panic, overflow, rejected input, and state-generation changes deserve explicit
markers. Their appearance helps distinguish intentional containment from normal
physical decay. A graph that silently omits protection intervals can make an
unstable model appear well behaved.

## Performance and solver trace

Show timing distribution for a named workload and machine, not a universal CPU
claim. Live occupancy can help diagnose a session but is affected by host and
system scheduling. Keep instrumentation overhead visible. Distinguish callback
execution, worker preparation, and UI rendering; a slow graph is not necessarily
a slow audio engine.

Solver telemetry includes iteration counts, convergence status, residual scaling,
and fallback counts. A capped iterative solve must reveal when it reached its
cap without meeting tolerance. A green status means a defined assertion passed,
not merely that finite audio emerged. Include a persistent incident summary
even if detailed live records overflow.

## Data path and responsiveness

Use fixed-size telemetry records or preallocated observation buffers transferred
with the ownership protocol in the real-time volume. The UI can downsample,
format, allocate graphics resources, and export files outside processing. Opening
a panel must not allocate inside the callback or change the physical trajectory.

Test a stalled consumer, a hidden window, rapid resizing, and editor closure.
The audio owner continues independently. If data stop, show a stale/paused state
instead of animating fabricated traces. A frozen graph should preserve the
captured timestamp and configuration identity so it cannot be mistaken for the
current state after a preset change.

## Export and comparison workflow

Export a manifest and numeric data with the graph. Include model/build identity,
units, fixture/event hashes, sample rate, analysis settings, and evidence labels.
Export should not occur on the audio thread and should not silently include
private paths or participant details. If measurement redistribution is restricted,
offer an authorized derived-data packet or report the limitation.

For the current synthetic scaffold, the canonical report interface is
`python3 tools/generate_report.py --input artifacts/reference --output artifacts/report`
after the reference render. Inspect the generated outputs and label only what
actually exists. Future live panels, comparison dashboards, and GUI screenshots
remain proposed until implemented and captured.

## Acceptance and next action

Verify panel calculations against known synthetic fixtures, units against the
source definitions, and plots against exported tables. Test linked scales,
missing data, invalid values, overflow, and changed configurations. Capture
authentic screenshots after deterministic setup, but pair them with numeric and
semantic assertions. The next action is one panel that resolves a real diagnostic
question, rather than adding many attractive traces with unclear meaning.
