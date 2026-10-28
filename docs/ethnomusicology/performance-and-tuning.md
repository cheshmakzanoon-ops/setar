# Performance and tuning as explicit model inputs

## Scope

The physical model must separate construction, setup, musical intention, and
realized sound. A fret location is a geometric quantity. A target interval is a
musical choice. A played pitch trajectory is an observation. A note label is a
representation. These quantities influence one another but are not interchangeable.

R-SETAR-TABASSIAN is an identified performer's handout, and
R-FRETTING-POORHAYDARI-2023 is an empirical and theoretical study. Their evidence
and limits are recorded centrally in the source ledger. The engineering design
below is the project's proposal: support contextual tunings and measured physical
states without asserting a universal equal-tempered or quarter-tone system.

## Physical strings and playing courses

Represent each physical string independently at the mechanical level, even when
performance treats two strings as a course. Independent state is needed for
tension, phase, damping, detuning, polarization, and coupling. A course is a
performance grouping, not permission to duplicate one waveform and call it two
strings. The grouping can define which strings a gesture contacts and in what
sequence while preserving their separate dynamics.

Do not assume string numbering conventions are universal across every diagram,
player, or software interface. Anchor the project's identifiers to a labeled
specimen photograph and explicit physical locations. Store player-facing names
and alternate terminology as metadata. This prevents a tuning table from being
applied to the wrong physical string when a source lists strings in another order.

Allow unplayed strings to retain physical state. Sympathetic response, residual
ringing, and damping actions depend on that state. A keyboard note-off should not
automatically erase all other string motion. Define the control mapping from
host events to player actions so that convenience does not silently remove an
important mechanism.

## Absolute reference and relative tuning

Store an explicit frequency reference in hertz and relative intervals or ratios
for each string. Note spelling alone is insufficient, because a nominal label
does not uniquely fix the chosen reference frequency. When a player uses a
relative naming convention, preserve it and separately record the measured
absolute frequencies.

Use cents as a convenient logarithmic comparison unit, not as a declaration of
equal temperament. For frequencies `f_a` and `f_b`, the interval is
`1200*log2(f_b/f_a)`. Ratios and cents can describe non-equal systems. Preserve
the original ratio or target representation when supplied; converting everything
to rounded cents can lose intended exact relationships or create false precision.

A tuning preset should contain physical-string assignments, intended target
frequencies or ratios, reference frequency, fret-map identifier, musical context,
source or contributor, and approval status. It also needs a statement of whether
the target is a documented practice, a specific player's setup, an experimental
design, or an educational approximation. These labels should be visible enough
that users are not misled by an authoritative-sounding preset name.

## Fret maps are not scale labels

Movable fret maps should store physical contact positions and optional intended
interval labels. The same physical fret can produce different realized pitch
depending on string, action, tension, and finger behavior. A scale table that
simply retunes an oscillator does not reproduce those physical consequences.
It may still be a useful alternate interface, but it should be named as a pitch
mapping rather than a measured fret simulation.

Do not assume every desired pitch must correspond to one static fret location.
Investigate which effects in the intended repertoire arise from setup, finger
placement, continuous gesture, or contextual interpretation. Ask players to
demonstrate the distinction rather than deducing it from a notation symbol alone.
Where the model does not represent a mechanism, state the approximation explicitly.

Preserve fret-map versions. If a player adjusts a fret during a session, the
change is evidence about setup, not a correction to erase. Record the reason,
pre- and post-position, intended context, and resulting pitch observations. This
helps distinguish a deliberate musical decision from drift or measurement error.

## Context-dependent interval observations

When measuring an interval, record the phrase or isolated task, direction of
motion where relevant, register, string, fingering, target role as described by
the player, and accompaniment or reference used. These fields support testing
whether context affects the observation. They do not assert in advance that every
field must have an effect.

Begin with isolated repeatable notes to understand the physical setup, then
observe musically meaningful phrases. Isolated and contextual measurements answer
different questions. An isolated target can reveal a stable fret relation while
a phrase reveals expressive trajectories or contact transitions. Do not reject
the phrase as inaccurate merely because its instantaneous frequency differs from
an isolated tuner reading.

Likewise, do not label every discrepancy expressive. Estimator bias, sympathetic
interference, clock error, and inconsistent finger placement can also move an
estimated pitch. Use synchronized observations and uncertainty analysis to
separate plausible mechanisms. Expert interpretation should inform the analysis
without replacing measurement quality controls.

## Ornamentation and continuous state

Create an articulation inventory through player elicitation. For each item,
record the intended musical function, observable actions, relevant strings,
contact transitions, pitch behavior, damping behavior, and dependencies on
previous state. Avoid representing every ornament as a fixed prerecorded
sequence. The synthesis engine should produce sound through physical excitation
and contact; a higher-level gesture sequencer may control those mechanisms.

Some gestures are defined partly by their timing relation to preceding and
following notes. A bag of isolated articulation samples will miss that relation.
Measure transitions and repeated patterns as well as single events. Store
continuous control trajectories and discrete contact events with common timing.
If the controller interface cannot express a required action, document the
mapping compromise rather than claiming the physical model alone solves it.

## Directional plucking and attack families

Distinguish approach direction, string-crossing order, and release behavior in
the physical descriptor. Two strokes with similar total energy can excite
different polarizations or neighboring strings. A velocity-only mapping may
sound consistent while lacking the contrast players expect. Test this with
controlled gestures and natural phrases before choosing the final controls.

Do not hard-code stereotyped “humanization” as random pitch jitter or timing
noise. Variation should reflect measured conditional relationships or an explicit
artistic design. Correlations among contact location, direction, displacement,
and timing may matter more than independent random variation in each field.
Keep deterministic controls available so performers can reproduce intended results.

## Resonance and damping vocabulary

Ask players to identify when other strings should ring, when they should be
suppressed, and which damping actions are musically meaningful. Translate these
into physical state changes with finite timing and contact behavior. A global
envelope that kills all sound at note-off cannot represent selective damping
unless that is the intended simplified mode.

Separate bridge/body resonance from room reverberation and from unplayed-string
response in both documentation and listening tasks. A participant's descriptive
word may refer to a combination of them. Follow up with demonstrations and
controlled variations rather than assigning the word to one DSP parameter based
only on intuition.

## Tuning import and host control

When importing external tuning files or host tuning messages, define whether they
change pitch targets, physical string tension, fret positions, or a controller
mapping. These choices have different mechanical consequences. A physical
retuning operation can change preload and stored energy; an instantaneous
frequency jump in a delay line is not automatically a valid physical operation.
The real-time implementation must use a stable, documented transition law.

Support arbitrary ratios or frequencies within safe model bounds where practical,
but do not imply every setting corresponds to a playable real instrument. Expose
out-of-domain status before an invalid physical state reaches the solver. A
designed electronic mode can allow broader behavior if clearly separated from
the calibrated instrument mode.

## Expert review of presets

Review a preset with the contributor who supplied its context and with an
independent qualified player when possible. Ask whether the description, available
gestures, and audible behavior match the stated purpose. Do not ask only whether
the sound is pleasant. Preserve requested changes and unresolved disagreements.

Version presets with their evidence. If only target intervals are reviewed,
do not label the entire physical response player-validated. If a player approves
a demonstration, that does not automatically authorize endorsement language or
use of their name in marketing. Those are separate permissions.

## Acceptance

The performance specification passes when physical string identities, course
groupings, tuning references, fret maps, gesture definitions, and context are
explicit and reviewable. It fails when a convenient keyboard mapping is
presented as the tradition itself, when unknown targets are filled with a fixed
quarter-tone assumption, or when continuous gestures are hidden inside recorded
note playback. The next action is a player-reviewed task matrix that connects
each important behavior to a measurable and implementable mechanism.

