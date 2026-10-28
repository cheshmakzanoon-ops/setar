# ADR-0005 — Model persistent physical strings rather than disposable note voices

Status: accepted full-engine design direction; unimplemented. The current diagnostic intentionally retriggers a single synthetic fixture.

## Context

A setar’s strings continue vibrating between gestures and can exchange energy through their shared structure. Fretting, damping, retuning, and excitation act on that ongoing state. A conventional new-voice-per-note abstraction can erase history or create independent duplicate strings.

## Decision

The proposed instrument owns persistent physical string identities and their coupled states. Input notes or gestures are interpreted as actions targeting those strings. They are not automatically instructions to allocate another oscillator or reset an existing one.

A performance adapter resolves ambiguous host events using an explicit playing mode. It preserves contact releases, ordering, and sympathetic motion. Simplified modes may exist, but their departures from physical performance remain visible.

## Alternatives and consequences

Disposable note voices are familiar to plugin frameworks and simplify polyphonic bookkeeping, but do not naturally represent shared body loading or repeated excitation of a vibrating string. A completely gesture-only interface is physically expressive but may be difficult for ordinary MIDI users.

The chosen approach separates host convenience from physical identity. It requires mapping policy, bounded event transport, persistent contact history, and explicit reset semantics. It also makes voice count an inadequate measure of instrument complexity.

## Acceptance and reconsideration

Test repeated excitation without forced state erasure, initially silent-string response through coupling, fret/contact transitions, and equivalent results under different block partitions.

Do not change the existing fixture’s retrigger semantics merely to imply this decision is implemented. Add verified persistent behavior deliberately. Reconsider the abstraction for a clearly labeled nonphysical performance mode, without replacing the physical baseline or its claims.

Related: [state ownership](../architecture/02-physical-ports-and-state-ownership.md) and the [string volume](../strings/README.md).
