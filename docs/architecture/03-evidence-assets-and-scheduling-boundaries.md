# Evidence assets and preparation/processing boundaries

## Evidence is part of the data model

This document proposes the asset and scheduling architecture beyond the current synthetic reference. An accepted runtime coefficient set needs a model identity, units, source evidence, validity domain, and a reproducible preparation path. A float array with a pleasant sound is not enough.

The current renderer already labels its output synthetic and records physical inputs, observation meaning, gain, and sample timing. The report generator validates those files and hashes them. This is a useful limited implementation, not a complete measured-asset registry or authenticated source-to-binary chain.

## Four different kinds of object

| Object | Typical owner | Mutability and lifetime |
|---|---|---|
| Raw evidence | Acquisition/research workflow | Preserved unchanged; permissions controlled |
| Reference hypothesis | Offline solver workflow | Versioned and reproducible |
| Prepared runtime asset | Preparation service | Immutable after validation |
| Live instrument state | Processing owner | Mutable, instance-specific |

An asset references the evidence and hypothesis that produced it without necessarily redistributing every raw measurement. Privacy, copyright, owner consent, and dependency licenses may restrict those files differently.

Do not equate a source hash with physical validation. It identifies bytes. Calibration quality, correct units, measurement honesty, and held-out performance remain separate evidence.

## Preparation pipeline

The proposed pipeline parses and validates inputs, resolves units and coordinates, assembles or loads the canonical model, checks physical admissibility, creates sample-rate-specific coefficients, allocates bounded state/scratch, and publishes a complete candidate.

All expensive or fallible work remains outside the processing callback. This includes file I/O, decompression, schema migration, matrix factorization, filter fitting, geometry operations, and dependency lookup.

Validation should occur before publication, not lazily on the first audio block. A failed candidate retains its diagnostics and does not replace the accepted live model. This extends the current ModalString preparation principle without claiming that a general asset loader already exists.

A candidate’s success state must include its capacity requirements. If the requested mode count, contact count, or event capacity exceeds the supported envelope, reject it explicitly. Silent truncation changes physics and can discard stored energy.

## Canonical model versus rate-specific realization

Keep a canonical physical representation separate from coefficients derived for one sample rate. Store enough information to regenerate and verify the latter. A preset containing only opaque biquad coefficients may be difficult to migrate or validate physically.

Rate-specific assets identify the discretization, observation bandwidth, numerical tolerances, and any prewarping or delay factorization. A host sample-rate change can require new preparation and a state-transition policy.

Do not silently retune the instrument, remove modes, or reset every string when the host changes rate. If continuity cannot be preserved in the initial implementation, disclose the exact safe transition and test it.

## Publication and reclamation

The loader builds a candidate independently of live state. At an approved boundary, the processing owner adopts it and applies the defined state map. Old storage is reclaimed through a mechanism that keeps unbounded destruction off the processing path.

An atomic pointer is not a complete design. The implementation must establish who owns the pointed-to object, when readers stop using it, and which thread performs destruction. Shared ownership can still run a final destructor on the audio thread if used without care.

The architecture does not prescribe one queue or reclamation library before requirements are established. It requires bounded operations, explicit capacity, race tests, and a failure policy. Select a mechanism supported by the target platforms and verify its actual behavior.

## Deterministic processing

For fixed assets, initial state, and normalized event sequence, processing should produce the same result under different block partitions within the stated arithmetic environment. This extends the current reference’s block-partition principle to event-bearing simulations.

Processing work is bounded by prepared capacities and the accepted solver strategy. Nonlinear iterations need an explicit upper bound and a physically justified failure response. “Usually converges quickly” is not a contract.

Separate a slow, inspectable reference solver from the production implementation where useful. Compare them on physical time and observables, not matching internal state layouts. A different reduced basis can be correct without identical state vectors.

## Telemetry and reports

Publish bounded diagnostic snapshots or counters for energy, solver residual, overflow, invalid state, and asset identity. Sampling and decimation policy must be stated. A display can be lower rate than the solver without hiding failures from retained numeric checks.

The UI formats numbers, draws plots, and writes requested reports outside processing. It does not read a live internal span concurrently or ask the callback to serialize a large model.

Telemetry failure must not destabilize sound generation. If a bounded queue fills, record a lost-snapshot count according to policy. Distinguish dropped diagnostic data from dropped physical events; they have different consequences.

## Claims and execution evidence

Maintain separate evidence for source review, compilation, numerical verification, integration, sanitizer execution, host behavior, real-time measurements, specimen validation, and listening evaluation.

A configured Windows job is not a Windows pass. A Linux result does not certify macOS. A headless pass does not certify JUCE. A real editor screenshot does not prove numerical correctness. A measured FRF fit does not establish all performance gestures.

When reporting a result, retain revision, dirty-tree state, commands, environment, actual exit status, and artifacts. Do not guess a remote run status from a local commit or an uploaded workflow definition.

## Failure evidence and migration

Malformed or incompatible assets fail before publication with actionable diagnostics. Preserve the rejected input or a permitted minimized reproducer. Never migrate unknown units by guessing or label an old synthetic parameter bank as measured because the schema evolved.

State serialization has separate physical and host concerns. A saved preset describes configuration; a full live-state snapshot may include ongoing vibration and contact history. Decide which is supported rather than promising both through one generic save operation.

Schema migrations must be deterministic and tested against old fixtures. A migration that changes physical interpretation needs a new model identity and an explicit compatibility statement, not only an incremented integer.

## Acceptance boundary

Accept the proposed pipeline only after malformed-input, publication, reclamation, state-transition, event-overflow, and deterministic-block tests exist. Measure actual processing behavior on claimed platforms; do not infer a CPU figure from coefficient count.

Use the [solver exchange contract](../acoustics/06-solver-exchange-and-evidence-packages.md) for scientific payloads and the [engineering artifact contract](../engineering/artifacts-and-reproducibility.md) for current report behavior. Architecture connects these records to runtime ownership; it does not replace their scientific or software acceptance gates.
