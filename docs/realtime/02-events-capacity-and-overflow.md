# Event time, finite capacity, and semantic overflow

An event queue that never allocates can still be musically wrong. Losing a
note-off, changing equal-time order, or applying a pitch gesture one host block
late can break the instrument without producing a crash. This chapter specifies
a proposed event contract and adversarial tests. It must be implemented at the
host-to-core boundary and shared by the headless scheduler so both paths mean
the same thing.

## Canonical time and identity

Use an explicit sample-time domain for events delivered to the engine. A host
block supplies offsets relative to its beginning; the wrapper converts them to
the core's defined representation without losing ordering. Long-running absolute
counters need a width and overflow policy. Do not mix floating-point seconds,
host ticks, and samples without a documented conversion and rounding rule.

Each event carries type, source identity, target identity, time, sequence key,
and bounded payload. Target identity distinguishes physical string/course,
logical note, and allocated voice. These are not interchangeable. Two notes with
the same MIDI pitch can have different lifetimes; an MPE channel may be reused
for a later note. Generation identifiers prevent stale release or expression
events from affecting a newly allocated target.

Declare ordering at equal time. Preserve source order when it is meaningful.
A blanket “all note-offs before note-ons” rule can change a legato or retrigger
sequence. If different sources must be merged, choose a stable tie-break rule
and test it. Emergency containment is a separately prioritized control, not an
ordinary musical event that accidentally wins because of an enum value.

## Rendering event segments

Process samples up to the next event, apply the event at its exact boundary,
then continue. Define whether an event at offset zero affects the first sample
and whether an event exactly at the block end belongs to the next block. Ensure
the same convention is used in tests and wrappers. Multiple events at one offset
must not introduce extra silent samples or advance the physical solver twice.

Continuous parameter trajectories can be represented by bounded segments or
sample-accurate points with a declared interpolation rule. The engine must not
reinitialize smoothing at every host block, because that makes behavior depend
on block partition. Test one event stream rendered as many block-size sequences,
including one-sample segments around event boundaries.

Late events need an explicit policy. A live input may arrive after its intended
time; apply at the next legal boundary with a lateness diagnostic, reject it, or
use another declared bounded strategy. Do not retroactively rewrite physical
history. Offline rendering can require stricter ordering and reject malformed
timelines rather than imitate live jitter.

## Capacity accounting

Count storage for queued events, in-block events, deferred events, active notes,
voice state, and expression updates. A queue capacity alone does not bound the
work required to sort or merge an arbitrary input batch. Prefer already ordered
sources or bounded algorithms with documented maximum work. Preallocate all
merge and sorting storage.

At the external boundary, detect whether the host input exceeds the supported
envelope without copying it into a growing container. Understand the framework
API's complexity: a seemingly simple event count may itself traverse the buffer.
If the wrapper cannot inspect an arbitrarily long stream within its work bound,
it must have a defined overflow mode rather than promise to find every hidden
note-off in that stream.

Partition capacities where it improves semantics, but do not assume a reserved
note-off queue can never fill. A malformed or extreme source can exceed any
finite reservation. The final fallback must be defined even when every queue is
full. State the maximum supported workload and test just below, at, and above it.

## Semantic overflow policy

Discarding the newest event indiscriminately is unacceptable: it can lose release
events and leave persistent sound. A proposed robust policy has three levels.
First, coalesce superseded continuous-expression values for the same target when
intermediate points are not semantically required. Second, reject new note-ons
when voice or event capacity is exhausted while preserving releases for already
accepted notes. Third, if release integrity cannot be guaranteed, enter a bounded
overflow-recovery mode that releases all affected active notes and suppresses new
note-ons until a defined recovery boundary.

The third level is not musically transparent, but it is explicit and prevents
indefinite stuck notes. It can be triggered by an overflow flag outside the
ordinary queue so a full queue cannot swallow the recovery request. Implement
the flag and acknowledgement with a generation protocol; otherwise a stale flag
can release notes accepted after recovery.

Release-all work is bounded by the configured maximum active states. Use a
prevalidated damping or containment transition rather than an unbounded cleanup.
If an emergency output fade is used, label it as protection rather than physical
simulation and test its discontinuity behavior. Preserve an incident count and
the affected source/generation. Do not silently call the event stream successful.

## Coalescing rules

Coalescing is safe only under an explicit semantic contract. A latest-value
controller sampled at a boundary can replace earlier values before that boundary.
A sequence describing a gesture path, trigger, switch, or release cannot generally
be collapsed to its last value. Even continuous pitch points may contain a
short bend that disappears if coalesced indiscriminately.

Specify each event class as replaceable, accumulative, ordered, or terminal.
For example, a meter update can be dropped, while an excitation impulse must not
be merged as a scalar without considering time and phase. A sustain transition
can change the meaning of nearby note-offs. Test pairs and short sequences, not
just individual event types.

## Voice allocation and physical continuity

Define whether a logical note selects an existing physical string, allocates a
virtual instrument instance, or creates a nonphysical polyphonic layer. A real
setar's strings share a body; arbitrary independent voice allocation changes that
interpretation. Keep “physical instrument mode” and expanded polyphonic modes
distinct in the mapping and evidence.

When stealing a voice is allowed, specify the selection rule, transition, target
identity invalidation, and treatment of later note-offs. Do not let an old
release terminate the new voice occupying the same slot. Test stealing while
expression is active, while sustain is held, and at simultaneous repeated
pitches. A deterministic allocator aids reproducibility but still needs musical
review.

## Adversarial fixture suite

Generate dense same-time events, alternating on/off pairs, unmatched releases,
duplicate releases, overlapping repeated pitches, reordered expression,
out-of-range offsets, empty blocks, large blocks, channel reuse, sustain changes,
panic during overflow, and state replacement with pending events. Store the
canonical event list and expected state transitions.

Assert bounded memory and work, no surviving accepted notes after recovery,
stable event identity, correct diagnostic counts, and consistent output under
different block partitions. Verify every accepted note has a defined termination
path. Use a simple reference state machine independent of the optimized queue.
Fuzz the boundary parser separately from physically admissible performance
sequences so malformed input does not obscure ordinary semantics.

## Gate and next action

Acceptance requires a capacity table, ordering specification, overflow state
machine, recovery audio, minimized failures, and cross-platform test results.
Report rejected notes and coalesced controls; a bounded system should reveal
when it has exceeded its supported envelope. The next action is integrating the
host adapter with the same canonical scheduler used by offline fixtures, then
testing the actual wrapper rather than assuming translation is lossless.
