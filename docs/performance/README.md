# Performance intent, controller translation, and evidence

The performance layer translates musical intent into bounded physical actions.
It does not generate sound independently of the physical engine. A note name,
stroke label, or controller value becomes an explicit selection, trajectory,
contact, or tuning command, with enough traceability to reproduce the result.

Begin with [practitioner gesture verification](01-gesture-verification.md), then
read [the event contract](02-event-schema.md). The
[MIDI/MPE adapter](03-midi-mpe-and-tuning.md) explains how a limited controller can
address four persistent strings without pretending that every note owns an
independent oscillator. [Deterministic transitions](04-deterministic-transitions.md)
define sample-offset behavior and ownership changes.
[Performance validation](05-performance-validation.md) tests repeated gestures,
ringing, and coupling. [Optional physiological
controls](06-optional-physiological-controls.md) separates expressive sensing from
unsupported anatomical or health claims.

All interfaces in this volume are proposed design contracts unless the actual
source and tests establish otherwise. C++20 owns validated event types and
mechanical state. JUCE may adapt host messages and user-interface controls at the
application boundary. No chapter requires a JUCE object inside a string solver,
contact law, or independent reference test.

Three distinctions are essential. First, a controller note is an ownership and
intent object, while a physical string persists after that note is released.
Second, a performer gesture is not identical to one fixed force trajectory;
practitioners can realize the same musical intention differently. Third,
deterministic rendering does not mean sterile performance. Variation can be
intentional and reproducible when its source, parameters, and seed are explicit.

The intended outcome is a playable instrument whose response can be explained,
tested, and revised. Empirical high fidelity is an aspiration requiring measured
references and practitioner evidence, not a property conferred by a large
control panel or a familiar technique name. Feedback that a mapping is awkward
is actionable even when the underlying solver is mathematically correct.

Use the [performance prompts](../../prompts/performance/) for bounded work
packages. Before implementing one, inspect the current source, read the relevant
numerical and contact chapters, and identify the smallest independent test.
Unimplemented mechanics can be represented by transparent synthetic fixtures
while the interpreter is developed, but must not be described as validated
physical gestures.

The implemented, unreviewed score grammar of the playable prototype is in
[the prototype score format](07-score-format.md).
