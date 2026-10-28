# A bounded performance-event contract

## Events describe actions, not audio clips

The core event stream should be independent of MIDI, JUCE, file formats, and
recorded notes. It describes physical or performance actions against an existing
instrument state. A host adapter can generate those actions; an offline script
can generate the same actions; a test can replay them without a plugin. This
separation makes numerical and controller failures distinguishable.

The schema below is a proposed design contract, not a statement that these types
already exist in the repository. Inspect the actual engine before selecting final
names. The important invariants are stable identity, explicit units, bounded
payloads, sample-position semantics, prepared-resource references, and clear
ownership of persistent contacts.

Inputs to the design task are the four-slot inventory, gesture glossary,
prepared-state model, and real-time capacity budget. Outputs are validated event
types, a serialization representation for offline work, a deterministic
interpreter, and traces connecting commands to mechanical actions.

## Envelope fields

Every event needs an epoch, an absolute render-frame position within that epoch,
a stable event ID, a source ID, a sequence number, a configuration revision, and
an event kind. The epoch distinguishes a new render session or a deliberate
transport-state discontinuity from ordinary continuation. Frame position is not
wall-clock time and should not be interpreted as a project schedule.

Use an integer frame representation with checked conversion from host offsets.
The host block's local offset is meaningful only relative to that block's start
frame. If an offset equals the block length, it belongs to the next block under
the half-open interval convention. Negative, overflowing, or stale positions
must have documented handling rather than wrapping into a large unsigned value.

The sequence number resolves events from the same source that share a frame.
For multiple sources, define a deterministic merge rule and preserve a source
priority or causal transaction field where needed. Sorting by arbitrary pointer
address or container iteration order is not reproducible.

## Target and ownership fields

A target can name one physical slot, a course, an explicit slot mask, or a
prepared geometric path whose eligible strings are declared. Resolve musical
note intent before a low-level contact action if the action requires a unique
physical target. Do not let a low-level solver guess which string a note number
was intended to address.

Ownership identifies the controller note, gesture, or direct user action that
created a contact. Contact ID is separate from event ID: several events can
modify one persistent contact. Parent gesture ID groups a multi-string sweep or
alternating sequence. These relationships allow a release to affect its own
contacts without clearing unrelated ringing.

An event targeting a configuration revision different from the active prepared
state must be rejected or handled by an explicit transition protocol. A fret ID
from one layout must not be interpreted as the same physical position in another
layout simply because both are small integers.

## Proposed action families

Use distinct actions for beginning or updating a nail trajectory, engaging or
moving a finger, releasing a contact, requesting a tuning trajectory, installing
prepared configuration, and resetting state. A generic parameter-change event
can be convenient, but it should not bypass the physical validation associated
with each action.

`BeginNailPath` references a prepared path and material profile, a gesture ID, and
an eligibility target. `EngageFinger` references a finger identity, target slots,
a prepared surface or load profile, and a location policy. `ReleaseContact`
identifies an existing contact and a release trajectory or declared immediate
reduction. `RetuneString` identifies a slot and a prepared admissible trajectory.
`ResetInstrument` is explicitly nonphysical and carries a reason code.

Continuous control updates should identify their interpolation domain. A target
position in metres differs from a normalized pressure intent and from a force
in newtons. Strongly typed payloads prevent the interpreter from treating all
three as an unqualified floating-point value.

## An illustrative offline stream

The following JSON is synthetic documentation, not an existing preset or a
measured performance. The prepared-resource IDs are illustrative handles whose
definitions would have to be supplied by a real fixture. The sample rate and
frame positions define test timing only; no musical authenticity is implied.

```json
{
  "schema_version": 1,
  "provenance": "synthetic",
  "sample_rate_hz": 48000,
  "epoch": 7,
  "configuration_revision": 12,
  "events": [
    {
      "frame": 1000,
      "event_id": 101,
      "source_id": 3,
      "sequence": 1,
      "kind": "engage_finger",
      "owner_id": 90,
      "contact_id": 501,
      "target_slots": [0],
      "prepared_finger_profile": 21,
      "prepared_fret_id": 4
    },
    {
      "frame": 1040,
      "event_id": 102,
      "source_id": 3,
      "sequence": 2,
      "kind": "begin_nail_path",
      "owner_id": 90,
      "gesture_id": 701,
      "eligible_slots": [0],
      "prepared_path": 31,
      "prepared_nail_profile": 11
    },
    {
      "frame": 12000,
      "event_id": 103,
      "source_id": 3,
      "sequence": 3,
      "kind": "release_contact",
      "owner_id": 90,
      "contact_id": 501,
      "prepared_release_profile": 41
    }
  ]
}
```

The release does not clear slot zero or end its body coupling. It requests a
contact transition; the resulting mechanical state continues. A complete fixture
would also declare course membership, physical parameters, prepared resources,
render length, observation points, and expected diagnostics.

## Offline representation versus callback representation

JSON is convenient for review and archival tests, not for parsing inside an
audio callback. Parse and validate it outside rendering, resolve strings and
resource names to handles, and compile it into bounded typed events. The callback
receives immutable prepared resources and fixed-capacity event storage.

A C++20 representation might use a tagged union or `std::variant` of small
trivially managed payloads. That choice must be checked for the actual types and
toolchain; `std::variant` alone does not guarantee that contained objects avoid
allocation. Do not put `std::string`, dynamically growing vectors, or arbitrary
callbacks into event payloads used on the real-time path.

Prepared handles need generation or revision checks to prevent use-after-reuse.
Their lifetime must extend through every event and active contact that references
them. Installing a new configuration cannot free old data while a rendering
thread still uses it. The real-time volume owns the detailed ownership mechanism;
this schema makes the lifetime requirement explicit.

## Causal groups and simultaneous events

Some actions should be atomic at one frame. A controller may establish a finger
position and begin a pluck together. Define whether the contact is installed
before the trajectory begins, whether both enter one combined solve, or whether
the adapter must provide a small explicit lead. Do not leave the result to
container order.

A transaction can carry a bounded ordered set of commands validated together.
If one command is invalid, choose all-or-nothing rejection or a documented partial
policy. Partial success is dangerous when a pluck proceeds but its intended stop
was rejected. The event trace must expose the outcome.

Separate command ordering from contact-solver ordering. Applying commands in a
deterministic order does not justify solving physically simultaneous contacts
sequentially without error analysis. The interpreter sets the physical problem;
the numerical layer solves that problem under its own verified scheme.

## Validation rules

Check finite numeric values, units, ID ranges, resource revisions, frame order,
capacity, target consistency, and supported physical domains before events reach
the core where possible. Reject NaN and infinity explicitly. A comparison such
as `value < minimum` alone does not reject NaN because floating-point comparisons
with NaN behave differently from ordinary numbers.

Validate ownership operations: release of an unknown contact, update from the
wrong owner, duplicate begin with the same contact ID, and reuse of an active ID
must have deterministic outcomes. A test should prove that malformed input does
not alter unrelated physical states. Logging the error after corrupting state
is not successful validation.

When a late event arrives, the engine cannot retroactively alter already emitted
audio without a separate lookahead or rollback architecture. Choose a policy:
reject, apply at the current frame with a late flag, or use a supported buffered
path. Do not silently pretend it arrived on time. The policy can differ between
offline rendering and live input if that difference is documented.

## Trace design

A bounded trace record can include event ID, application frame, resolved targets,
created or modified contact IDs, rejection code, and configuration revision.
Mechanical diagnostics can then connect force and energy changes to those IDs.
Text formatting and file writing occur outside the callback.

Trace capacity must be bounded independently of event capacity. If telemetry
overflows, count dropped records without blocking or changing the physical
result. A diagnostic overflow should not become an audible timing error. Tests
should force overflow and verify the declared behavior.

## Acceptance and next action

The schema passes review when independent adapters can produce equivalent core
events, malformed inputs are rejected without unrelated mutation, ownership is
unambiguous, and the same event stream has block-partition-independent meaning.
Serialization round trips must preserve units, IDs, revisions, and sequence,
not merely produce parseable JSON.

The next chapter maps MIDI and MPE into this contract. That adapter must preserve
the distinction between note identity and physical string identity, and it must
not import host-specific objects or voice-stealing assumptions into the string
solver.
