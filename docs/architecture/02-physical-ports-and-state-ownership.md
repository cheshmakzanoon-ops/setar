# Physical ports and state ownership

## Status and purpose

This document proposes the full engine’s interface responsibilities. The current reference header does not implement these multiport, persistent-instrument, or event-transport interfaces. The purpose is to constrain future APIs before incompatible data ownership and numerical assumptions become embedded in applications.

The numerical derivations remain in the [energy-port contract](../mathematics/03-energy-and-passive-ports.md), [bridge volume](../body/03-bridge-contact-and-multiport-coupling.md), and [cavity chapter](../acoustics/01-cavity-and-structural-acoustic-coupling.md). This document assigns ownership rather than repeating those equations.

## Boundary vocabulary

| Boundary quantity pair | Physical meaning | Required metadata |
|---|---|---|
| Force / translational velocity | Mechanical power exchange | Direction, location, sign, time level |
| Moment / angular velocity | Rotational power exchange | Frame, lever arm, angular convention |
| Pressure / volume flow | Acoustic port exchange | Aperture, area normalization, orientation |
| Traction / surface velocity | Distributed interface work | Basis, quadrature, surface normals |
| Physical state / microphone pressure | Observation, not conjugate loading | Position, propagation, calibration |

An array of doubles is not a sufficient contract. Each port needs coordinate and unit definitions, interpolation rules, and the precise discrete time level at which work is evaluated. Current-time, midpoint, and staggered half-step values are not interchangeable.

If one side interpolates velocity, the other side uses the corresponding conjugate force map. The interface owner ensures this pairing. Two subsystems must not independently choose similar-looking interpolation and hope that work cancels.

## Who owns coupling

The coupled-step owner assembles or evaluates interactions once using the states participating in the same numerical step. A string and body should not each advance independently and exchange stale samples through an undocumented one-sample delay.

Subsystems expose sufficient response information for the selected solver, but the connection decides the common force and velocity consistently. The exact information depends on the verified discretization; it may be an effective admittance, an implicit residual/Jacobian contribution, or another derived relation.

Do not standardize a “force in, displacement out” callback merely because it is easy to wire. It can force an unsuitable causal ordering and hide the need for an implicit solve. Prototype the connection on small fixtures before freezing a public interface.

## Persistent physical strings

The proposed instrument owns a stable set of physical strings and their ongoing states. A new musical event acts on an existing string; it does not normally allocate a fresh independent note voice. This supports continued vibration, sympathetic exchange, damping by touch, fret changes, and repeated excitation.

Musical polyphony is therefore not identical to conventional voice count. Several pitches or partial patterns can coexist within one physical string history, while several strings can interact through the same body. A host note identifier is an event identity, not a unique physical oscillator.

A mapping layer chooses which physical string and gesture an input represents. It must handle ambiguous MIDI input explicitly. It may offer a deliberately simplified playing mode, but that mode must not erase state silently while claiming continuous physical interaction.

The current diagnostic’s retrigger behavior remains a fixture behavior. It is not evidence that the persistent-string design has been implemented.

## Contact and geometry ownership

String and body geometry are prepared from immutable assets. Dynamic contact state—engagement, relative displacement, friction memory, or other model-specific variables—belongs to the live instrument owner.

Do not store live contact history in a shared immutable material asset. Two instrument instances can share coefficients but must not share mutable contact or oscillator state accidentally.

Geometry changes such as fret movement or retuning may alter energy and state representation. Their APIs need an explicit physical operation or a documented nonphysical transition policy. Recomputing coefficients in place is not automatically a valid state update.

Unknown contact parameters remain unknown or prior-based in the asset. The event layer must not silently turn them into measured values by exposing a control with a physical-sounding label.

## Single-writer state

One execution owner mutates the live physical state during processing. UI, loader, and analysis threads submit requests or consume snapshots; they do not obtain mutable references to internal vectors.

Read-only references are not inherently concurrency-safe. The current modes span illustrates why: a const view can still refer to data being mutated or replaced elsewhere. A proposed telemetry snapshot needs independent storage and a bounded publication mechanism.

Prepared coefficients may be shared across instances when immutable. Reclamation must occur away from the audio callback if destruction can allocate, lock, or free large storage. A pointer swap alone does not establish safe lifetime management.

## Event timing and ordering

Events carry a defined physical timestamp or sample offset, type, target, payload units, and stable tie-breaking order. The adapter translates host conventions once. The physical solver consumes the normalized representation.

Specify what happens at equal timestamps, outside a block, after a reset, or during sample-rate changes. Preserve order for causally dependent actions such as contact engagement followed by excitation. Do not rely on container iteration order accidentally.

Use bounded transport and explicit overflow policy. Dropping every event equally may lose a contact release or emergency silence while retaining less consequential motion updates. Priority rules require review and observable counters; the current diagnostic’s ignore-remainder policy is not adopted automatically.

Scheduling is by event time and numerical dependency, not project dates. Block partition must not change a deterministic render when the same normalized event sequence is supplied.

## Observation ownership

Observation reads physical state and produces displacement, velocity, pressure, or digital audio according to an explicit map. It must not normalize the physical state to fit a level target.

A microphone control can change observation without changing mechanical loading. A bridge or material control generally changes the physical model. Keep those categories distinct in the API and interface so a cosmetic sound change is not confused with altered energy dynamics.

Room processing, listening gain, and final safety limiting belong downstream with visible metadata. Validation renders preserve the unprocessed physical observation and all applied transformations.

## Error and reset semantics

Preparation errors leave the accepted live asset/state usable or enter an explicitly defined safe silent state at the boundary. Do not install partially validated dimensions or coefficients.

A reset has a named scope: clear all physical energy, clear only host event state, clear observation memory, or prepare a new model. One ambiguous reset method should not perform different operations depending on which application calls it.

The processing path needs bounded status reporting without formatting strings or writing logs. A nonfinite or solver-failure event should produce a defined safe outcome and diagnostic code, not hidden clipping or automatic parameter substitution.

## Acceptance before API commitment

Test units, work pairing, event ordering, block partition, lifecycle transitions, and ownership races on a small synthetic coupled fixture. Verify two independent instrument instances share only immutable data. Verify stale snapshots remain valid under asset replacement.

Only then name and implement a public engine interface. The architectural objective is a narrow, inspectable contract that supports future physics—not a large set of speculative classes whose semantics are deferred.
