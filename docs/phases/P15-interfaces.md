# P15 — Build musician and engineering interfaces from verified state

**Status:** specification. **Prerequisites:** P14, with research and interface
prototypes permitted earlier. **Exit evidence:** usable, accessible interfaces
that present physical controls and diagnostic evidence honestly without imposing
unsafe real-time work on the audio process.

## Keep musical intent and physical state legible

The musician should be able to play and configure the setar without operating a
finite-element solver. An engineer must be able to identify why a sound changed
without guessing which of hundreds of hidden parameters did so. These are related
but distinct interfaces over one documented engine contract.

Read [musician and engineering UX](../ux/README.md),
[performance](../performance/README.md),
[plugin](../plugin/README.md), and the
[reference reporting contract](../engineering/artifacts-and-reproducibility.md).
The existing UX wireframe is a *proposed design*, not a screenshot of an
implemented instrument. The synthetic HTML renderer report is a working
engineering interface and must retain its evidence labels.

## P15-T01: construct a control inventory

Classify each candidate control as performance input, specimen/setup parameter,
physical calibration, presentation choice, or diagnostic. For every exposed
control, define stable identifier, internal unit, musical presentation unit,
legal range, default source, smoothing/state-change semantics, and whether
changes involve physical work. Do not place a raw Young's-modulus slider beside
a musical volume knob without explaining which is user performance and which
changes the modeled specimen.

Separate calibrated and exploratory modes. A musician may deliberately explore
unrealistic construction parameters; the interface must not silently write those
changes into the validated specimen profile. Preserve a reversible comparison
between a documented reference setup and an edited variant.

Define the status language for unsupported values and configurations. Avoid a
false impression that every input combination is mechanically valid. If a
chosen string tension, fret position, or contact setting violates solver
constraints, state the condition and the supported alternatives. A hidden clamp
is not an understandable musical control.

## P15-T02: design the musician interaction path

Map a playable gesture into observable engine events. Explain how a simple
note-on produces the chosen fingering/excitation approximation and how an
expressive control changes nail trajectory, damping, or pitch behavior. Keep
convenience mappings identifiable as such. Provide an intelligible way to see
four physical strings and their playing-course grouping without turning the
screen into a literal image of a generic six-string guitar.

Support tuning and fret systems as editable, versioned configurations. Names,
interval labels, Persian script, and performance terms must be reviewed in
context with practitioners. Do not hardcode one supposed universal radif,
dastgah, or microtonal grid into the numerical core. A UI may offer a documented
starter mapping while retaining a way to describe its actual provenance.

Show physical-state continuity: a ringing string remains ringing when an editor
panel changes. Provide clear musical stop/release functions with different
semantics from technical reset. Test multiple open/close cycles, resizes, focus
changes, plugin instance counts, and host transport transitions during sound.

## P15-T03: design calibrated observability

Specify bounded diagnostic streams for physical energy, solver residuals,
contact status, bridge force/velocity, model version, load failures, and timing
where appropriate. Each stream has a unit, sampling cadence, loss/overflow
policy, and trust boundary. The UI must not directly dereference mutable solver
vectors from another thread. Screens are projections of telemetry, not hidden
alternate numerical paths.

In the reference application, display the source revision, input manifest,
model assumption list, evidence level, and checks alongside each plot. Use
actual computed data with known transformations. Distinguish displacement,
velocity, force, acoustic pressure, and output PCM; a waveform glyph with no
unit or variable label can mislead even if the underlying numbers are correct.

When plots downsample data, describe the method and avoid hiding narrow spikes
with simple arbitrary point skipping. For energy and failure dashboards,
maximum-preserving envelopes or other justified aggregations may be necessary.
Store the unaggregated evidence packet for verification.

## P15-T04: specify graphs, animations, and screenshots

Build a visual contract for common plots: axes and units, range, model/sample
identity, method, tolerance bands where justified, legend, and color-independent
identification. A mode-shape visualization must distinguish undeformed
geometry, deformation magnification, loading/support conditions, and whether
the shape is simulated or measured. Numerical animations must not invent
unobserved spatial detail.

Create real screenshots through a reproducible capture path from a known build
and fixture. Store capture configuration, dimensions, source revision, and
whether the target is a headless HTML report, standalone UI, or plugin editor.
Do not label a Figma-style concept drawing an implemented screenshot. Include
usable alt text and textual numerical summaries so the essential result remains
available without inspecting pixel colors.

Test interactive plots with boundary and failure data, not only elegant nominal
curves. A nearly silent signal, nonfinite run rejection, missing external asset,
contact solver fault, and huge parameter scale all need understandable UI states.

## P15-T05: test access, language, and input devices

Support keyboard focus, screen-reader labels where applicable, high-contrast
readability, scalable text, and meaningful indicators that do not depend on
color alone. Check both left-to-right and right-to-left presentation where
Persian text is included. Technical parameter identifiers should remain stable
even when visible translations change.

Use practitioner review to test whether terms and controls represent intended
gestures. Consider expressive controllers and accessibility alternatives, not
only mouse dragging. Record where a requested gesture cannot yet be represented
by the engine rather than designing a decorative control that does nothing.

## P15-T06: qualify interaction and presentation

Perform scripted UI reviews and usability sessions against the same build.
Record task, participant role where consent permits, expected outcome, actual
observation, and revision. Include low-vision/keyboard tests, repeated editor
lifecycles, invalid parameter edits, error recovery, and many-instance behavior.
Capture actual screenshots with labels identifying their evidence status.

Acceptance requires that controls produce documented engine events, diagnostics
do not compromise callback bounds, screenshots are traceable, and users can
recover from invalid operations without corrupting calibrated data. Beautiful
screenshots alone do not pass this phase.

> Implement one musician control panel and one engineering diagnostic panel over
> the same stable engine API. For every control, list the event, unit, physical
> meaning, range, and state-change policy. For every graph, name the underlying
> variable, provenance, rendering transform, and evidence level. Test editor
> lifecycles, failure states, accessibility, and callback isolation. Provide
> actual captured evidence and keep conceptual wireframes explicitly separate.
