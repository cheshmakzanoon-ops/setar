# Parameters, state schemas, automation, and compatibility

Host parameters are a public interface that can outlive many engine revisions.
Saved sessions depend on identifiers, normalized mappings, defaults, and
interpretation. A change that sounds better in a new preset can still break an
old session. This chapter defines proposed compatibility rules and tests; it
does not claim that a finished parameter set or migration system exists.

## Parameter inventory

For every parameter record a stable ID, display name, unit, physical or artistic
meaning, normalized mapping, range, default, quantization, automation policy,
transition policy, and state-schema introduction. Distinguish performance
controls from specimen configuration and output presentation. A pluck-location
control and a decorative brightness macro should not share ambiguous semantics.

Use stable IDs independent of display names and translated labels. Renaming a
control should not change its identity. Do not reuse a removed ID for a different
meaning. If a parameter's interpretation must change, create an explicit migration
or new parameter, and decide how old automation remains meaningful. Position in
a vector is not a sufficient long-term identity contract.

Some values should not be host-automatable. Replacing a body mesh, changing a
large modal basis, or loading a calibration asset may require preparation and
state mapping. Expose a deliberate selection operation with pending/active
status instead of advertising a continuous automation parameter that cannot
meet its contract. Product convenience must not create unsafe physical updates.

## Normalized mapping

Hosts commonly work with normalized parameter values while the engine works in
physical units. Define a reversible mapping and its endpoint behavior. Use
logarithmic or other perceptually useful mappings only where their semantics
are justified. Test round trips, endpoints, representative interior values,
text parsing, formatting, and invalid input. Display rounding must not feed back
into the stored value during routine refresh.

A range change can reinterpret old automation even if the ID is unchanged.
For example, normalized 0.5 may no longer represent the same tension or damping.
Preserve legacy mapping or migrate using stored version information when the
host format permits it. Do not assume all hosts store physical text rather than
normalized numbers. Include actual host reopen tests for critical changes.

Keep SI units at the core boundary. A UI may show millimeters or cents, but the
conversion is explicit and tested. Distinguish absolute frequency, relative
cents, pitch-bend semitones, and tuning reference. A convenient unit label cannot
hide a control that changes multiple physical quantities.

## Automation timing and gestures

Define whether automation is sample-accurate, block-rate, or a latest-value
approximation for each adapter path. Do not claim sample accuracy merely because
the core accepts timed events; the wrapper must preserve host timing too. Test
automation at zero, interior, and final sample offsets, plus coincident note
events. Keep source order where it affects the resulting gesture.

UI edits should bracket host automation gestures appropriately using the
framework's supported mechanism. Keyboard changes, text entry, reset, and drag
must all follow the declared undo/automation behavior. A drag should not create
thousands of independent undo entries unless that is explicitly intended. Verify
host recording and replay, not only the editor's response.

Do not feed host automation through the UI. The processor must work without an
editor. Conversely, the UI should observe the authoritative parameter state,
not maintain an unrelated local copy that drifts during automation. Avoid
feedback loops in which displaying a rounded value writes it back to the host.

## Smoothing is not physical retuning

Choose smoothing by control meaning. Output gain can use a verified amplitude
ramp. A desired pluck-position change may affect future excitations only. A
tension change alters stored energy and frequency; a fret movement changes
constraints and may perform work. Interpolating coefficients smoothly does not
prove the resulting transition is physical or stable.

JUCE's [SmoothedValue](https://docs.juce.com/master/classjuce_1_1SmoothedValue.html)
offers linear and multiplicative value ramps; multiplicative smoothing does not
reach zero. It is a useful utility for appropriate controls, not a replacement
for deriving state transitions. Review the chosen domain, endpoints, sample-rate
behavior, and repeated-target semantics.

Test smoothing across block partitions and sample-rate changes. Reinitializing
the ramp every block can make its duration depend on host block size. Repeated
targets can prevent a ramp from completing if handled incorrectly. Abrupt
transport or preset operations may require a separate reset policy. Compare
the parameter trajectory and the resulting energy/audio, not just absence of
an audible click.

## State contents and versions

State should identify schema version, model version, parameter values, tuning
and keyboard mapping, selected specimen/calibration asset, controller mapping,
quality mode, and any additional information required to recreate sound. Keep
editor geometry and purely visual preferences distinct where practical. Do not
serialize raw pointers, allocator details, platform-dependent object layouts,
or arbitrary in-memory numerical buffers as a portable preset format.

Decide whether a saved preset restores configuration only or a sounding physical
state. Most product presets need the former; exact continuation requires a much
larger contract including solver state, event history, sample-rate context, and
model realization. Do not promise continuation because a parameter tree was
saved. State documents should name the intended restoration semantics.

Set size, depth, string-length, numeric-range, and asset-reference limits before
parsing. Treat imported state as untrusted input. Reject malformed or unsupported
future schemas without partially applying them. Preserve the active valid state
on failure and report an actionable reason outside audio processing.

## JUCE state boundary

JUCE's [AudioProcessorValueTreeState reference](https://docs.juce.com/master/classjuce_1_1AudioProcessorValueTreeState.html)
documents parameter access and state helpers. Its `copyState` and `replaceState`
operations use synchronization and are not real-time-safe. Keep them out of
audio processing. A pointer returned for atomic parameter reads does not make
arbitrary traversal or mutation of the associated ValueTree safe in the callback.

Understand the calling context of host state methods in the pinned wrapper and
tested hosts. A state callback is not automatically a message-thread callback.
Design serialization and preparation so they do not race active processing. If
expensive preparation is asynchronous, define when restored state becomes active
and how offline rendering waits or fails predictably. Silently rendering the old
preset while a new one prepares is not a correct session-restore contract.

## Migration procedure

Maintain immutable fixtures from every supported historical schema. Parse to an
intermediate versioned representation, migrate through explicit transformations,
validate the complete result, prepare it, then install atomically. Record which
defaults were supplied and which obsolete fields were ignored. Unknown critical
fields should not be guessed into a plausible but different instrument.

Test old-to-current migration, current round trip, malformed input, unsupported
future version, missing asset, changed asset hash, and canceled preparation.
Check sound-relevant equivalence after restoration using canonical events.
Byte-identical serialization may not be required if field ordering is benign,
but semantic equality and asset identity must be established.

Never overwrite historical fixtures with the current serializer and call that
migration coverage. Keep expected migrated values and interpretation alongside
the original bytes. If exact sonic compatibility cannot be preserved after a
model correction, document the break, offer a deliberate compatibility choice
where supportable, and avoid silently changing old sessions.

## Preset loading and failure recovery

A preset change is a transaction. The active configuration remains valid while
a candidate is parsed and prepared. On success it is installed at a declared
boundary; on failure the old state remains active and the UI distinguishes
requested from active selection. A failed load should not leave half the controls
showing the new preset while the engine uses the old one.

Test changes while notes ring, during automation, during state save, and during
editor closure. Include worker cancellation and repeated requests. The newest
request must not be overwritten by an older worker result. Preserve note-off
and emergency semantics across generation changes.

## Acceptance artifacts

Deliver the parameter registry, normalized-mapping tests, automation replay,
transition-energy tests where physical, state schema, immutable migration
fixtures, hostile-input cases, and actual host reopen evidence. The next action
is to freeze the smallest useful parameter interface and prove round-trip
behavior before adding more controls. Compatibility is cheaper to preserve when
parameter meaning is explicit from the beginning.
