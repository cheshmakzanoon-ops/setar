# MIDI, MPE, microtuning, and physical gesture mapping

A keyboard protocol is not a physical model of a setar player's hand. The adapter
must translate symbolic notes and expressive controls into explicit physical or
artistic actions. This chapter specifies that translation and its tests. It does
not claim that MIDI 2.0, every tuning protocol, or every controller is implemented.
Supported message families must appear in the capability register.

## Three identities

Keep MIDI note identity, engine voice identity, and physical string/course
identity separate. A note number describes a pitch request under a tuning map.
A voice is an allocation unit. A physical string is part of a coupled instrument
with history. Two simultaneous keyboard notes may require two virtual instruments
or a deliberate nonphysical mode rather than two independent states on one real
string.

Declare playing modes. A physically constrained mode can map notes to available
strings and contacts with a defined fingering policy. An expanded polyphonic mode
can provide musical convenience while admitting that it exceeds one physical
setar. Do not use the latter as evidence of realistic string/body interaction.
Give each mode its own voice-allocation and note-termination tests.

## Basic MIDI semantics

Normalize note-on, note-off, channel control, pitch bend, pressure, sustain, and
emergency messages according to the supported protocol. Handle velocity-zero
note-on as required by the MIDI 1.0 interpretation used by the adapter. Preserve
channel and note identity through repeated pitches and overlapping lifetimes.
Unknown or unsupported messages should be ignored or reported under a bounded
policy, never parsed into arbitrary memory.

Define the meaning of release velocity where supported. It could influence a
declared damping action, or be ignored explicitly. MIDI note-off need not mean
instant physical silence. The product may map it to lifting a finger, damping
a string, or ending a virtual note; choose and document the behavior. Sustain
likewise needs a mapping rather than a piano-derived assumption hidden in code.

Pitch bend, channel pressure, and other controls can arrive before note-on or
after note-off. Define retained channel defaults and reset behavior. Test channel
reuse so stale bend or pressure does not unexpectedly affect a new note. Handle
all-notes-off, all-sound-off, and controller reset according to declared product
semantics, including during event overflow.

## MPE support

The MIDI Association's [MPE specification page](https://midi.org/mpe-midi-polyphonic-expression)
describes channel-based per-note expression. JUCE's
[MPEInstrument reference](https://docs.juce.com/master/classjuce_1_1MPEInstrument.html)
is a useful implementation boundary when the chosen JUCE version supports the
required behavior. These sources establish protocol/API context; they do not
define the project's physical mapping.

Represent zone configuration, member channels, master-channel controls, bend
ranges, and note lifetimes explicitly. Test zone changes, invalid configurations,
channel reuse, simultaneous notes, and expression at note boundaries. Distinguish
master pitch effects from member-note pitch effects. Avoid applying the same
bend twice through both channel and per-note paths.

Choose meaningful mappings for pressure and timbre dimensions. Pressure might
control a finger normal-force target only within a validated contact range;
it should not be labeled force in newtons without a calibration or explicit
mapping. A timbre dimension could move pluck position for future attacks or
adjust an artistic macro. Explain when a control affects a currently ringing
string versus the next excitation.

MPE support does not automatically imply MIDI 2.0 support, VST3 Note Expression
support, or every controller's proprietary messages. Test each claimed route
and document conversion loss. If a wrapper collapses rich note identity to
plain MIDI channel/pitch, state the resulting limitation.

## Tuning model

Store a tuning as explicit pitch relationships plus reference and mapping data.
Do not assume all intervals are equal-tempered, all periods are octaves, or all
Iranian practice is a fixed quarter-tone grid. Practitioner-informed tuning
choices need provenance and context. A preset is a proposed musical setup, not
a universal definition of a tradition.

For an equal-tempered diagnostic mapping only, frequency can be expressed as
\(f=f_r2^{(m-m_r+b)/12+c/1200}\), with reference note \(m_r\), reference
frequency \(f_r\), bend \(b\) in semitones, and offset \(c\) in cents. A
general scale should instead use explicit ratios or cents and a keyboard map.
Keep pitch request separate from the physical calculation that realizes it
through tension, contact, or another supported mechanism.

Test reference-note changes, non-octave periods, negative and large degree
indices, unmapped keys, and finite positive ratios. Reject invalid ratios and
nonfinite values before logarithms or coefficient construction. Preserve
precision internally while formatting understandable values in the UI.

## Scale and keyboard-map import

The Scala author's [scale-file documentation](https://www.huygens-fokker.org/scala/scl_format.html)
distinguishes scale definitions from keyboard mappings. Supporting `.scl` alone
does not imply supporting `.kbm` or every possible keyboard layout. Read the
actual format documentation before implementing either parser and preserve
source text/provenance where permitted.

Parse outside audio processing with bounded file size, line length, item count,
and numeric range. Handle comments, whitespace, ratio versus cents notation,
declared counts, and malformed input according to the format. Do not accept a
partially parsed scale and silently fill the rest with equal temperament. Show
an actionable error and keep the active tuning unchanged.

Use immutable parser fixtures including unusual valid cases and hostile inputs.
Test import, display, state save, restore, and actual rendered frequencies.
An import dialog that shows the right name does not prove the keyboard map is
correct. Include keyboard labels and unmapped-key behavior in usability tests.

## Retuning while sound is active

Distinguish changing a future note's target from changing an already vibrating
string. A new pitch table can apply only to new notes, or active notes can follow
a declared retuning policy. Neither should happen accidentally because one code
path caches frequency and another reads the table every sample.

Physical retuning may require changing tension or contact geometry, with state
and work consequences. A smooth frequency multiplier is a product effect unless
derived and verified as the chosen physical action. Define allowed rate and
range, handle inadmissible requests, and test energy and artifacts. Do not use
parameter smoothing as a substitute for that derivation.

For MPE bends, state whether the mapping represents a contact gesture, an
idealized pitch control, or a deliberately nonphysical expressive mode. Musicians
can use all three, but scientific evidence must not confuse them. Keep the
mapping revision in saved state and listening-study manifests.

## Controller calibration and usability

Controllers have different ranges, resolutions, neutral points, and noise.
Provide calibration or sensitivity settings where justified, and avoid turning
small idle jitter into persistent physical excitation. Filtering should have a
declared latency and effect on gesture shape. Test slow ramps, fast changes,
held values, boundary values, and disconnected/reconnected devices in standalone.

Show which source controls which physical target. A player should be able to
discover why one string bends while another rings unchanged. Provide a panic
action that uses the same tested termination semantics as host emergency events.
Do not require the editor to be open for controller interpretation to work.

## Acceptance artifacts

Deliver protocol capability tables, canonical message fixtures, expected state
transitions, tuning-parser cases, frequency checks, block-partition comparisons,
overflow/recovery tests, and practitioner-reviewed mapping notes. Actual controller
and host tests identify device and software versions. The next action is one
well-defined expressive mapping with observable physical meaning, not a broad
compatibility checkbox that conceals unsupported routes.
