# Deterministic sample-offset handling and physical state transitions

## Determinism starts with meaning

A reproducible event stream needs more than a fixed random seed. The engine must
define when commands take effect, how simultaneous actions are ordered, which
state survives, and what happens when configuration changes. If those meanings
depend on host block size, the same performance can produce different contact
histories even when every floating-point operation is deterministic.

This chapter concerns physical event interpretation. The real-time volume owns
threading, memory reclamation, and platform execution details. The two contracts
must agree: a lock-free queue that delivers events in the wrong causal order is
not a correct performance system.

Inputs are validated core events, prepared resources, current mechanical and
performance state, and an absolute frame counter. Outputs are applied-action
traces, continued state, audio under a declared observation convention, and
bounded diagnostic codes. The implementation must be testable without JUCE.

## Define the sample boundary

Use an interval convention explicitly. A useful reference convention is that
events at frame `n` become active before advancing mechanics over the interval
from `n` to `n+1`. The numerical core then produces the observation associated
with that interval according to its own documented sampling rule. Whether the
observation uses an endpoint, midpoint, or another consistent quantity is a
numerical decision; it must not vary between standalone and plugin rendering.

A block beginning at frame `b` with length `N` owns event positions in
`[b, b+N)`. An event at `b+N` belongs to the next block. Test both boundaries.
Applying an end-boundary event twice is as serious as dropping it. Checked
arithmetic is required when computing the end frame.

Zero-length processing calls need a lifecycle policy. They may update transport
or queue events, but they must not advance mechanical time accidentally. If an
event is accepted during such a call, record whether it is pending or applied
administratively so the next nonempty call cannot apply it again.

## Sub-block rendering without semantic drift

One implementation can render up to the next event, apply the event group, and
continue. Another can examine a bounded event list at each sample. Either can
be correct if it preserves the same sample semantics. Performance differences
do not justify changing the physical meaning.

Trajectory evaluation should use absolute gesture time or an equivalent stable
local origin, not the fraction of the current host block. A ramp that restarts
at every block boundary creates discontinuities and buffer-size-dependent
contacts. Store its start frame, endpoint conditions, and interpolation policy.

For long sequences, avoid accumulating timing solely through repeated additions
to a floating-point phase if that causes unacceptable drift. Use a declared
integer, fixed-point, rational, or carefully analyzed phase representation.
Tempo-based gestures need an explicit conversion policy when tempo changes;
already emitted samples cannot be retimed.

## Simultaneous commands and transactions

Separate administrative changes, contact-intent changes, and mechanical solves.
For example, a prepared configuration may need to be installed before a fret ID
can be resolved. A finger contact and a nail path at the same frame may need to
enter the same subsequent mechanical interval. Define a bounded transaction
whose validation and application order are unambiguous.

Do not use a universal priority list as a substitute for understanding causality.
If two same-frame commands are contradictory, such as two owners demanding
different positions for one physical finger, reject or resolve the conflict
under an explicit policy. Applying the last one silently can make a result
depend on an unrelated source's insertion order.

Preserve source order within a causal stream. When merging independent streams,
use a documented stable key and record the resolved order. A replay should not
depend on memory addresses, hash-table order, or thread scheduling. The event
trace is the evidence that the merge rule was actually followed.

## Physical transitions are not assignments

Engaging a finger, releasing a nail, changing tension, and replacing geometry
each have mechanical consequences. A transition may add a contact potential,
change an equilibrium, alter a coefficient, or project state. Its energy effect
must be derived or measured against a reference. A smooth UI animation does not
make the underlying state assignment physical.

For a contact begin, preserve the existing string state and establish the
contact's initial geometry and history consistently. If the surfaces initially
overlap, either the prepared action intentionally begins with compression or
the event is invalid. Do not secretly reposition the string to make the gap zero.

For release, remove constraints according to the declared contact law and
release trajectory. Stored contact energy must be transferred or dissipated
explicitly. For a retune, use the parameter-work ledger in the string volume.
For a configuration replacement, use the prepared mapping or declared reset
policy rather than treating a new parameter pointer as sufficient.

## Ownership transitions

A controller note can end while its physical string rings. A contact can remain
owned by another note or direct gesture. Track these relationships explicitly.
The last released note is not necessarily the owner of every contact on its
string, and a reused MIDI channel is not a physical string identity.

Contact IDs should not be recycled while pending events can still reference
them. Use generations or sufficiently scoped identities and validate ownership
on update. A stale release must not detach a newly created contact that happens
to occupy the same storage slot. This is a semantic use-after-reuse bug even if
memory safety tools report no problem.

When a source disconnects, define whether its contacts release, damp, or remain
until an explicit command. A lost controller should not leave an unbounded active
driver, but a disconnection should also not silently erase the whole instrument.
Provide a deterministic recovery action and a diagnostic reason.

## Transport, looping, and seeking

Host transport state is not mechanical state. Stopping playback can leave an
instrument ringing; seeking can begin a new scripted performance while old
energy remains. Choose named policies for those operations. Offline rendering
may reset to a known initial state, while live use may preserve tails. The
difference must be visible and testable.

A loop boundary can replay a gesture sequence against the continued state or
restart a fixture from its initial state. These are distinct experiments. Do not
accidentally reseed variation or clear strings on every loop merely because the
transport position returns to zero.

Use an epoch change for a deliberate discontinuity in the event-time domain.
Pending events from an old epoch are rejected or migrated through an explicit
rule. An absolute frame counter and a host musical position should not be
conflated; the host can move backward while rendered audio time continues.

## Capacity and error handling

Bound active gestures, contacts, pending events, and trace records. Validate
prepared paths so one command cannot expand into unbounded work. A fixed maximum
number of strings does not bound all possible overlapping finger and surface
contacts automatically.

Classify overflow by consequence. Coalescing superseded continuous control
targets can be safe under a defined interpolation rule. Dropping a release may
leave a driver active indefinitely. Dropping a configuration install while
accepting events that reference it corrupts semantics. The policy must therefore
consider event dependencies, not only queue occupancy.

On numerical failure, preserve a bounded diagnostic and invoke the engine's
documented safe output/state policy. Do not continue emitting NaNs, normalize
energy to conceal divergence, or claim the event completed. A last-valid-state
fallback needs its own tests because freezing state while time advances can
create later discontinuities.

## Determinism test construction

Create a canonical event script with events at frame zero, adjacent frames,
same-frame transactions, and exact block boundaries. Render it as one block,
many fixed-size blocks, and an irregular partition. Compare applied-event traces
first, then mechanical diagnostics and audio. If traces differ, the scheduler is
wrong before numerical tolerance is considered.

Repeat with a trajectory spanning many blocks, a paired-course sweep, a retune
during ringing, and an overlapping finger release. Include a deliberately late
event and a stale configuration revision. Verify the documented rejection or
late-application result and ensure unrelated strings are unchanged.

Bitwise identity may be a reasonable target on the same build and platform if
the operation order is identical. Cross-platform floating-point behavior may
require tolerance-based comparison. State the promise precisely. Tolerance must
not hide a whole-sample event shift or a different physical target.

## Acceptance and next action

The work passes when event meaning is independent of block partition, ownership
survives channel and storage reuse, every supported transition has an energy or
reset interpretation, and failure behavior is bounded and observable. Include
the canonical traces and their expected results in tests rather than relying
only on an interactive demonstration.

The next chapter evaluates musical sequences against those invariants. Once
scheduling is deterministic, a difference between two performances can be
attributed to their intended inputs or model behavior rather than an accidental
buffer boundary.
