# ADR-0006 — Separate physical loading from acoustic observation

Status: accepted design direction. Current code produces displacement observation; coupled body/acoustic loading is proposed.

## Context

One microphone transfer can make a simple string sound colored, but it does not determine the force and velocity exchanged at the bridge. Treating a body as only an output effect misses reciprocal interaction, sympathetic response, and body-dependent decay.

## Decision

Use work-conjugate physical ports for bidirectional interactions and separate observation maps for displacement, pressure, microphone response, and digital level. The coupling owner enforces shared time levels and conjugate interpolation.

Mechanical or acoustic loading must satisfy the selected passivity and stability contracts. A force-to-microphone observation instead requires causal, stable, calibrated behavior; it is not incorrectly subjected to a positive-real requirement for a nonconjugate quantity.

## Alternatives and consequences

A one-way body filter is useful as an explicitly limited prototype or observation approximation. It is not accepted as the full physical coupling architecture. Conversely, making every microphone or room control modify mechanical state would confuse observation with the instrument itself.

The separation requires two error budgets and two kinds of test. A model can pass bridge-mobility tests while failing directivity, or match one microphone while loading strings incorrectly. Both failures remain visible.

## Acceptance and reconsideration

Require coupled energy/work tests and held-out multiport response for loading. Require pressure units, propagation phase, spatial prediction, and observation calibration for radiation rendering. Output normalization cannot repair a physical energy defect.

Reconsider a simplified loading approximation only with a bounded validity band and comparison to the accepted coupled reference. Preserve the physical/observation distinction even when both are implemented by compact filters.

Related: [bridge multiports](../body/03-bridge-contact-and-multiport-coupling.md), [radiation reduction](../acoustics/05-radiation-reduction-and-runtime-transfer.md), and [physical ownership](../architecture/02-physical-ports-and-state-ownership.md).
