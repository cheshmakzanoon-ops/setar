# Tuning, physical fret coordinates, and energy-aware retuning

## A tuning is not a temperament switch

The instrument configuration needs separate records for open-string equilibria,
fret geometry, and musical interpretation. Open-string tuning determines the
equilibrium of each physical string. Fret geometry describes where contacts can
occur. Musical interpretation associates names, intervals, or performance
functions with those outcomes. Combining all three into a table of MIDI note
numbers discards information that the physical engine needs.

Kioumars Poorhaydari's [Empirical Evaluation of Intervals and Fretting Systems in
Persian Art Music](https://journal.iftawm.org/previous/2023-volume-11-no-2/poorhaydari/)
uses performer reports and fret-position analysis to investigate practical
intervals and their variation. Its proposed theoretical models are proposals,
not a mandate that all performers use one fixed scale. Read the methods and
limitations before reusing a reported interval. This project should preserve
performer-specific and specimen-specific choices rather than assume uniform
24-tone equal temperament.

The presence of microtonal notation does not imply that every pitch lies on a
50-cent grid. A fret can have a measured position, an associated intended
interval, and a realized frequency that varies with string, pressure, and
contact condition. Store these separately. An expressive interface may offer
named tuning presets, but the underlying data must remain explicit and auditable.

## Representing physical frets

Use a nut-to-bridge coordinate `x_f` in metres for each fret crossing, plus shape,
height, width, compliance, and uncertainty as available. A tied fret can be
slightly angled or have different contact coordinates across the string array.
A single scalar fret position is a reduction that should be recorded as such.
Do not round geometry to a musical grid before computing contact.

For an ideal flexible string with unchanged tension and a perfectly rigid stop,
the speaking length after a stop at `x_f` is `L - x_f`, and the frequency ratio to
the open string is `L/(L - x_f)`. This is a useful geometric check. It is not a
complete stopped-string model: bending stiffness, changed tension, finite fret
radius, finger pressure, and compliant termination alter the result.

If a desired interval ratio `r` is used to propose a synthetic fret location,
the ideal formula gives `x_f = L(1 - 1/r)`. Mark that coordinate as derived from an
ideal assumption. Do not label it measured or use it to overwrite a practitioner's
actual fret layout. An inverse-design tool can compare proposed and measured
layouts, but it should show the model discrepancy rather than silently moving
frets to make pitches appear correct.

## Open-string targets and admissible tension

A target frequency is a musical command. Preparing the string requires a
physical equilibrium consistent with mass, length, bending, and boundary
conditions. Use the inverse relation only when its assumptions are appropriate;
otherwise solve the prepared model's eigenproblem or static/dynamic calibration
outside the audio callback. Preserve the difference between the requested
frequency and the predicted result.

Validate tension against the declared computational domain and any justified
material constraints. A preset cannot demand arbitrary transposition while
claiming unchanged physical construction. Large transpositions may require a
different string, geometry, or explicit nonphysical mode. The interface should
make that distinction clear. An optional creative mode is permissible, but it
must not be presented as a measured physical instrument configuration.

Tune all four physical slots independently even when a course contains two
members. If a paired course uses a chosen interval, encode the two targets and
their relationship. Do not infer one member's state by duplicating or pitch
shifting the other's audio. Their losses, rigidity, coupling, and contacts remain
independent physical quantities.

## Retuning performs work

For a fixed spatial domain and a linear transverse energy term
`T(t)/2 * integral |u_x|² dx`, changing tension contributes

\[
P_T=\frac{\dot T}{2}\int_0^L |u_x|^2\,dx
\]

to the transverse energy balance. This is parameter-control work under the stated
model. It does not include every energy involved in a real peg, string stretching,
or support deformation. A more complete axial model has a more complete ledger.
The point is that time-varying coefficients are not automatically passive.

For an instantaneous idealized tension change at fixed displacement and velocity,
the corresponding change in this stored energy is
`(T_after - T_before)/2 * integral |u_x|² dx`. If the implementation also changes
state, there are additional terms. A retune that raises energy can be physically
consistent with an actuator supplying work. A retune that silently changes
energy without recording why is not verified.

Ramping tension may reduce abrupt audible artifacts, but smoothness does not
prove passivity or correct work. The ramp's trajectory is part of the input.
Linear interpolation in frequency and linear interpolation in tension produce
different physical commands. Name the chosen domain and derive the other
quantities from it. Never use an undocumented smoothing filter as a substitute
for a state-transition contract.

## Length changes require more than a coefficient update

Changing speaking length through a contact differs from changing the actual
material domain. A stop partitions an existing string; it does not remove mass
from the instrument. Changing the installed reference length can involve stretch,
mass-density changes, and endpoint motion. A preset that simply changes `L` in a
frequency formula while retaining the same grid and energy interpretation may
represent neither case correctly.

Prefer a fixed material or spatial domain with explicit contacts when studying
fretting. If a reduced model changes its active segment, define a state projection
for the retained segment and a treatment for excluded motion. Report projection
error and energy change. A disappearing afterlength is a reduction, not an
unobservable implementation detail.

For remeshing, prepare interpolation or projection operators away from rendering.
Test constant fields where meaningful, smooth modes, high-frequency content, and
energy changes under the exact mass and stiffness metrics. A visually smooth
interpolation can still add energy or alter phase. If the desired transition
cannot be bounded and verified, limit it to stopped playback or an explicitly
documented reset mode.

## Three distinct user operations

The interface should distinguish live retuning, configuration replacement, and
emergency reset. Live retuning attempts to preserve mechanical continuity while
an actuator changes a physical parameter. Configuration replacement may install
different geometry or material and therefore needs a prepared transition or a
declared discontinuity. Emergency reset clears state to recover from failure or
honor a panic command; it is not a physical gesture.

An output crossfade can conceal a configuration switch, but it does not make the
combined output a single conserved mechanical system. It may be an acceptable
presentation strategy when labeled and tested for bounded work. Do not use a
crossfade result as evidence that live physical retuning is implemented.

Likewise, a host preset load should not opportunistically reuse incompatible
contact history. A stored tangential spring, for example, has a meaning tied to
its contact geometry and material. If those change, the transition needs an
energy-aware mapping or an explicit release/reset policy.

## Prepared-grid safety

The numerical discretization must remain valid for the maximum reachable tension,
rigidity, loss parameters, and boundary behavior in the declared live-retune
range. The explicit FDTD scheme's own bound is authoritative for that scheme;
do not substitute a frequency-only heuristic. A target that lies within the
displayed keyboard range can still invalidate a mesh prepared for lower tension.

Prepare the supported range outside the callback. The renderer should accept a
retune event only if it stays within that prepared domain or references a valid
prepared replacement. Failure should be deterministic and visible: preserve the
last valid state, reject the command with a bounded diagnostic, or execute a
documented safe transition. Clamping to an unknown value while reporting the
requested tuning is misleading.

Sample-rate changes are also configuration transitions. A host may change block
size without changing the physical timestep, but a sample-rate change alters the
discretization and possibly the represented band. Rebuild prepared state outside
rendering and define what happens to ringing energy. Do not assume that scaling
delay lengths alone preserves dispersion and losses.

## Verification experiments

Start with a lossless synthetic string in a known mode. Apply a slow tension
trajectory and compare the numerical energy change with integrated control work.
Then test a sharp but defined transition against an offline reference. Repeat
at several phases of the initial oscillation; energy change depends on the
instantaneous deformation, so a single phase is not representative.

Next test retuning an initially quiescent string. Under a model that changes only
transverse stiffness while leaving the equilibrium and initial displacement zero,
the string should remain transversely quiescent. If a richer model moves an
endpoint or changes equilibrium, motion may occur and must be explained by that
actuator. This pair of tests prevents accidental clicks from being mistaken for
real tuning noise.

Then test a stopped note and a paired course. Verify independent slot targets,
preserved ringing where intended, and correct rejection of incompatible
configuration revisions. Finally compare requested intervals, physical fret
positions, predicted frequencies, and observed frequencies without collapsing
them into one field.

## Deliverables and exit gate

Inputs are the tuning records, fret geometry, parameter provenance, prepared
numerical domain, and transition policies. Outputs are a tuning schema, physical
preparation procedure, work-ledger tests, interval reports, and user-visible
rejection behavior. The gate requires correct distinction among retune,
configuration change, and reset; no hidden temperament assumption; and a
demonstrated energy account for supported live transitions.

Unknown empirical intonation remains unknown after a numerically correct
implementation. The next action is practitioner review of the mapping and
controlled measurement of realized pitches under documented stopping conditions.
Do not solve a mismatch by silently moving a fret or correcting output pitch.
That would replace the physical question with an undisclosed effect.
