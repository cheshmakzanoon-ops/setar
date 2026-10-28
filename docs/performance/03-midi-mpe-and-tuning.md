# MIDI, MPE, and tuning without virtual extra strings

## A controller adapter, not the physics engine

MIDI and MPE communicate musical intent. They do not directly measure nail force,
finger position, or string tension. The adapter therefore implements a declared
mapping from controller state to the physical event contract. Good mappings are
predictable, expressive, and testable; they need not pretend that a keyboard is
an instrumented setar hand.

The [MIDI Association's MPE
specification page](https://midi.org/mpe-midi-polyphonic-expression) identifies MPE
as a MIDI 1.0 practice for per-note expressive control through channels. Use the
current official specification for protocol behavior and record the version
actually implemented. Do not infer MIDI 2.0 support or every controller's
capabilities merely because the product advertises expressive control.

JUCE can help interpret host MIDI and MPE at the boundary. Its [MidiBuffer
documentation](https://docs.juce.com/master/classjuce_1_1MidiBuffer.html) describes
integer-timestamped events, while [MPEZoneLayout](https://docs.juce.com/develop/classjuce_1_1MPEZoneLayout.html)
and [MPEInstrument](https://docs.juce.com/master/classjuce_1_1MPEInstrument.html)
document zone and note-state handling. Check the pinned JUCE checkout rather
than assuming web documentation exactly matches it. The core remains C++20 and
JUCE-independent.

## Four physical strings constrain resolution

An MPE controller can express more concurrent note intentions than the setar has
physical strings. The adapter must resolve those requests into four persistent
states, not allocate a new physical string for every note. A creative extended
polyphony mode may be offered separately, but it is not the same instrument model.

Resolution policies can prioritize an explicit string selection, preserve an
existing fingering, choose a feasible alternate string, or reject an impossible
request. Document the policy and expose its result. Silent voice stealing is
especially misleading when it erases ringing that the physical model should
retain.

Two note owners can address one string only if their physical actions are
compatible under the chosen mode. For example, a retained lower finger and a
new higher finger can coexist, but two contradictory simultaneous tuning targets
cannot both govern one tension. The resolver needs a conflict rule before the
commands reach mechanics.

## Note-on and velocity

A note-on can request a fingering selection and a right-hand gesture. These are
two actions with an explicit timing relationship, not an implicit oscillator
creation. In a direct-string mode, note identity may choose a slot or course
while another control chooses the fret. In a keyboard mode, a resolver chooses
a feasible slot and fret from the requested pitch and configured tuning.

Velocity can map to a target trajectory speed, depth, or supplied-work range.
Choose one interpretation and calibrate the response. Mapping the same velocity
independently to force, displacement, and post-synthesis gain can triple-count
dynamic intent. A final output gain remains a separate mixing control if needed.

The mapping should be monotonic in the intended expressive dimension where
possible, but physical output loudness need not be strictly monotonic for every
stateful repeated pluck. A string moving against the nail can change the work
transfer. Test isolated gestures and repeated gestures separately rather than
forcing the latter into a reset-state loudness curve.

## Note-off and released notes

The adapter's note ledger and the instrument's mechanical state have different
lifetimes. Note-off may release a finger ownership claim, request a damping
gesture, or leave the string ringing under a named performance mode. It should
not automatically clear string and body state.

Keep released-note ownership long enough to interpret any supported expression
or pedal semantics correctly, while respecting the selected protocol and
controller behavior. A member channel may be reused for a new note. Old
expression values must not accidentally retune or damp the previous string
unless the documented mapping intends that relationship.

All-notes-off, all-sound-off, controller reset, and host transport stop are
separate inputs. Map them deliberately. A panic reset can clear all state with a
presentation fade, but ordinary note release should not invoke that path merely
because it is convenient.

## Pitch expression and physical feasibility

Pitch bend is interpreted through the active zone or channel range, not a
hardcoded assumption. Configuration changes to the range must update the adapter
state consistently. Keep zone-level and member-level expression distinct before
combining them according to the protocol and mapping policy.

The resulting musical pitch request still needs a physical interpretation. It
may select a fret, command a tension change within a prepared range, move a
contact, or invoke an explicitly assisted nonphysical pitch mode. These are not
interchangeable. A fixed fret does not move simply because an MPE finger slides
on a controller surface.

For physical fret selection, compare the requested interval with the configured
fret positions and predicted stopped frequencies. Do not assume equal-tempered
semitones or a uniform 24-tone grid. The tuning data may include measured
positions, performer-approved interval intentions, and their uncertainties. A
nearest-fret policy should report the selected fret and residual pitch request.

## Pressure and the third expressive dimension

MPE provides pressure and a third expressive dimension commonly carried by
CC 74, in addition to pitch expression. The [official MPE-related profile
overview](https://midi.org/6-new-profile-specifications-adopted) lists the relevant
control families; the exact implemented MIDI 1.0 behavior must follow the
selected specification rather than mixing profile details indiscriminately.

Pressure can map to finger-load intent, damping contact strength, or another
documented physical control. CC 74 can map to pluck position, nail orientation,
or contact-region preference. Choose a coherent default and allow remapping
where useful. Neither value is a measured force or anatomical coordinate unless
the input device was independently calibrated for that purpose.

Continuous pressure should not create a fresh pluck on every update. Distinguish
gesture initiation from trajectory modification. Likewise, a timbre change
should not reset loss filters or replace a ringing string state. Test dense
control streams to expose accidental retriggering and state discontinuities.

## Initial expression and channel reuse

Controllers can send expression before or around note-on. The adapter should
maintain the relevant channel state and apply the correct initial values to the
new note under the selected protocol. A note that begins with a stale bend or
pressure from a previous owner can be physically and musically wrong.

Create tests for expression-before-note, note-before-expression at the same
sample, channel reuse, zone reconfiguration, and overlapping note identities.
Use official conformance expectations for protocol interpretation, then separate
tests for the physical mapping. A JUCE helper passing its own unit tests does
not establish that the project's string-selection policy is correct.

MPE's channel organization does not eliminate the need for stable internal note
IDs. Repeated pitches on one channel can occur in real input streams or malformed
tests. Define the matching and release policy rather than indexing ownership
solely by note number.

## Sample offsets and real-time work

Preserve host sample offsets when translating messages. Do not process an entire
block's MIDI first and then render all samples from the final controller state.
That moves every event to the block start or end and makes behavior depend on
buffer size. Compile events into the core's half-open frame convention.

Preallocate adapter storage for the declared maximum event density. Audit the
actual chosen JUCE methods and containers for allocation and listener behavior
on the rendering path. A class being intended for audio use does not make every
possible operation allocation-free. Prepared configuration, string lookup, and
text formatting belong outside the callback.

When incoming density exceeds capacity, use a documented bounded policy. Dropping
continuous updates may be acceptable under a designed coalescing rule, while
dropping a release can leave a stuck contact. Classify events by semantics rather
than applying one arbitrary drop strategy to all of them.

## Verification and acceptance

Build an adapter-only test harness that converts known MIDI/MPE streams into
expected core events without audio. Verify zone changes, pitch ranges,
note-off policies, ownership, and sample offsets. Then replay the resulting core
events through a synthetic physical fixture and compare with a direct event
script. Equivalent intent should produce equivalent core behavior.

Test block-partition invariance, channel reuse while strings ring, impossible
polyphony requests, paired-course selection, and microtonal fret layouts. Include
malformed or out-of-range input and verify that it cannot corrupt unrelated
strings. Record rejected requests visibly enough for debugging without blocking
the callback.

The task passes when protocol interpretation, physical resolution, and rendering
are independently testable; no extra strings appear silently; and controller
expression has documented physical or assisted meaning. Practitioner evaluation
then determines whether the mapping is useful. Protocol compliance alone cannot
make a keyboard mapping feel like a credible setar performance interface.
