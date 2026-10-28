# Musician interface, engineering observability, and visual evidence

This volume describes proposed interface contracts. It distinguishes design
wireframes, deterministic component snapshots, native application/plugin captures,
and offline report images. Do not present one as evidence of another.

| Chapter | Focus |
| --- | --- |
| [Musician view](01-musician-view-and-interaction.md) | Play, tune, map gestures, understand state, and recover |
| [Engineering observability](02-engineering-observability.md) | Units, energy, spectra, events, solver and performance meaning |
| [Visual testing](03-visual-testing-and-screenshots.md) | Deterministic fixtures, numeric assertions, authentic captures, and reviewed thresholds |
| [Accessibility and localization](04-accessibility-localization-and-feedback.md) | Keyboard, screen reader, contrast, Persian text, and actionable feedback |

The [musician wireframe](assets/musician-wireframe.svg) is an authored SVG design,
not an implemented UI or captured screenshot. It deliberately contains no
fabricated measured curves, live CPU values, or acoustic-validation claims.
Real screenshots require a built/running candidate and a capture manifest.

Begin with a small control surface over verified capabilities. Test semantic
state and numeric layout before comparing pixels. Keep live telemetry bounded
and optional so closing or stalling the editor cannot change audio behavior.
Read the [concurrency contract](../realtime/04-concurrency-state-and-telemetry.md)
before connecting a graph to the engine.

Evidence includes task-based musician evaluation, practitioner review where
appropriate, accessible semantics, numeric assertions, raw captures, and
platform/host context. Visual polish is welcome; it cannot substitute for
correct controls, physical evidence, or honest capability labels.
