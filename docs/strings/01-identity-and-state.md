# Physical identity, courses, and persistent state

## The instrument boundary

The target is the Iranian setar, not the Indian sitar. Similar transliterations
in publications are a search hazard, not evidence that construction, bridge
contact, playing technique, or sympathetic-string architecture transfers.
Research records should retain the source's spelling while tagging the intended
instrument explicitly. An imported model with a large bank of separately tuned
sympathetic strings, a jawari bridge, or a plectrum assumption is not a harmless
starting point unless the feature is independently justified for the particular
setar under study.

Kiya Tabassian's performer-authored [Atlas Ensemble instrument
note](https://www.atlasensemble.nl/assets/files/instruments/Setar/Setar%20by%20Kiya%20Tabassian.pdf)
describes four strings organized as two singles and one doubled course,
adjustable gut frets, multiple tunings, and index-fingernail strokes in both
directions. Treat it as a compact practitioner reference, not a dimensional
specification. Its historical narrative is not needed to determine any DSP
coefficient. This project must verify the actual instrument's string order,
pairing, fret arrangement, and terminology with its collaborators.

The engineering inventory therefore has four immutable physical slot IDs. Human
labels such as melody, second, drone, and bass are metadata attached to those
slots. A course record contains one or two slot IDs and playing geometry. It does
not contain an audio oscillator. A tuning record supplies a physical target or
measured equilibrium for each slot. A gesture record names a course, a slot, or a
geometric path, with explicit resolution rules if those choices disagree.

This distinction prevents several subtle errors. A note on the paired course
must not necessarily excite both members with equal energy. Their diameters,
relative heights, tensions, and order along a stroke can differ. A drone may
continue ringing while another member is stopped. Their bridge reactions may
interact through the body. Assigning one oscillator to the pair would hide these
differences before any measurement could reveal them.

## Persistent means physically continuous

A physical string remains present when no MIDI note is active. It stores
displacement, velocity, loss-filter memory if the representation uses it,
contact history where physically required, and coupling state. A note-on does not
erase that state. It starts or modifies an excitation process. A note-off does
not imply exponential amplitude decay unless a documented performance adapter
chooses an explicit damping action. Silence in the host's note ledger and zero
mechanical energy are independent conditions.

Consider a repeated pluck before the previous response has decayed. The nail
encounters a string whose position and velocity are not zero. A finite-contact
model may capture it, slide against it, push it briefly, or miss it under a
particular geometric trajectory. Even a simplified excitation must superpose
with the existing state rather than resetting it. The resulting amplitude can
be larger or smaller depending on phase. An implementation that produces the
same isolated note on every trigger has changed the physical problem.

Likewise, a stopped note changes constraints on an existing string. The segment
between finger or fret and bridge may become the main speaking segment, while
the other segment retains some motion. Whether that other segment is resolved,
represented approximately, or dissipatively eliminated is a model decision.
Calling it a new voice conceals the decision and makes energy bookkeeping
difficult. The state transition must say what happened to the old segment's
energy.

## A proposed state inventory

At configuration time, `InstrumentConfiguration` owns immutable geometry,
parameter provenance, port definitions, and capacity limits. During rendering,
`InstrumentState` owns four `StringState` objects, body state, and bounded contact
state. `PerformanceState` owns intent such as active fingers, pending gestures,
and controller ownership. Keeping intent separate from mechanics allows a test
to replay identical commands against multiple numerical representations.

Within a string, distinguish equilibrium geometry from dynamic displacement.
Equilibrium can include a static break angle, action height, and fretting-induced
deflection. Dynamic fields are deviations around the chosen equilibrium. If an
equilibrium changes, the coordinate transform must preserve absolute geometry
and velocity or explicitly account for work. Subtracting a new baseline from an
old field without transforming state can create a false impulse.

A useful diagnostic snapshot contains slot ID, current configuration revision,
mechanical energy estimate, current contact IDs, effective speaking constraints,
bridge force, target tuning, and estimated instantaneous pitch with confidence.
The pitch estimate is a diagnostic, not a hidden servo. Feeding it back to force
every output into equal temperament would remove physical detuning and override
the tuning representation.

No example type here fixes a final source ABI. The implementation task is to
establish a minimal representation whose ownership and capacities are obvious:

```cpp
struct StringSlotId { std::uint8_t value; }; // Validated range: [0, 4).
struct CourseId { std::uint8_t value; };     // Validated range: [0, 3).
struct ContactId { std::uint32_t value; };
struct ConfigurationRevision { std::uint64_t value; };

struct CourseDefinition {
    std::array<StringSlotId, 2> members;
    std::uint8_t count; // One or two; established outside rendering.
};
```

These wrappers are not numerical safety by themselves. The implementation must
include the appropriate standard headers and use its actual engine types. The
boundary validator rejects invalid IDs, duplicate membership where prohibited,
ambiguous slot orders, and revisions that do not match prepared state. A raw
integer wrapped in a type is still invalid if nobody checks it.

## Excitation ownership is not voice ownership

A controller note instance needs an identity even though physical strings are
shared. For example, two keys may request two different fingering interpretations
on the same string. The resolver chooses a documented policy: reject the second
request, replace the finger intent, move an existing finger, or route to another
physically available string. It cannot create a fifth string silently. Store
that decision in an event trace so a performer can understand why a request was
unplayable.

Contacts have their own ownership. A right-hand nail can interact successively
with several strings during one sweep. A left-hand finger may cover more than one
string. A sustain pedal may be mapped to a damping permission rather than to an
actual mechanical pedal. Those ownership graphs should not be encoded as a
single `note -> oscillator` pointer. They require explicit relationships among
intent, contact, and physical slot.

Ownership also governs release. Releasing one MIDI note must not remove another
note's finger contact merely because both used the same channel. Releasing a
course-level gesture must not damp all four strings unless that was the
configured action. Tests should deliberately overlap note IDs, channels, and
course targets to expose accidental global operations.

## Course geometry and selection

Record each string's transverse position and height at the plucking region.
Course grouping alone cannot determine encounter order. A stroke in one
direction traverses the geometry differently from its reverse, and a curved nail
may reach one member while missing the other. A simplified scheduler can use
measured or synthetic encounter offsets, but its status must remain an
approximation until compared with a finite-contact path.

Selection modes should be explicit. `slot` means only an identified physical
string is eligible. `course` means members are eligible according to the gesture
geometry. `sweep` means intersection with a spatial path decides eligibility.
`automatic` means a controller-specific resolver chooses among those modes and
records its choice. Do not allow an empty mask to mean both all strings and no
strings in different call sites.

Course detuning is a difference between two physical equilibria, not a chorus
effect. If a synthetic experiment detunes one member, preserve its measured or
chosen `mu`, `EI`, and length and derive the corresponding tension under the
declared model. If the experiment changes diameter instead, multiple properties
change together. This makes a beating test physically interpretable.

## Establishing the invariant tests

The first test constructs four quiescent strings and excites exactly one slot.
With coupling disabled, only that slot should acquire energy beyond the declared
numerical tolerance. With passive coupling enabled, other slots may acquire
energy, but the total ledger must account for its transfer. Neither test proves
the body model is accurate; together they establish that slot identity is real.

The second test excites one slot twice with variable spacing. It compares the
second response with a reference that continues the first state and with an
intentionally reset-state control. The production engine must agree with the
continuous-state reference, not the reset control. Sweep the second pluck's phase
and inspect both constructive and destructive interactions. A single convenient
delay can conceal a reset bug.

The third test plays a course-level event and records per-slot contact times,
forces, and energy. It must be possible for the two course members to differ.
The fourth test releases one controller note while a sympathetic string rings;
that unrelated state must survive unless an explicit damping gesture targets it.
The fifth test serializes configuration and event intent, reconstructs prepared
state, and reproduces the same slot resolution. Exact floating-point audio
identity across unrelated processors is a separate promise that must not be
assumed.

## Review gate and unresolved questions

Required inputs are an instrument inventory, source annotations, coordinate
definition, and controller ownership policy. Required outputs are a configuration
schema, bounded capacities, and invariant test records. Exit evidence includes
four independently addressable states, verified paired-course membership,
demonstrated non-resetting repeated plucks, and a documented result for
contradictory note requests.

Failure modes include inferring string order from a photograph without checking
orientation, confusing role labels with immutable slot IDs, resetting a string
on every note, treating note-off as a universal mute, and merging paired strings
into a single state. Any of these invalidates later expressive-control claims.
The next action is to acquire or explicitly synthesize the physical specification
in the following chapter. Missing measurements should produce marked unknowns,
not confident numbers imported from another instrument.
