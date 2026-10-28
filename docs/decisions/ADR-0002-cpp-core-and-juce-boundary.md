# ADR-0002 — Keep the C++20 numerical core independent of JUCE

Status: accepted and reflected in the current headless build. A complete product/plugin boundary remains proposed.

## Context

Numerical tests and offline experiments should run without an audio device, GUI, host SDK, or paid dependency decision. Host lifecycle and editor behavior are nevertheless important and need a deliberate integration layer.

## Decision

Use C++20 for the physical core with standard-library-facing interfaces. JUCE may own plugin/application adaptation, audio/MIDI buffers, windowing, and host serialization. It must not own a second undocumented version of the physical equations.

Current CMake builds setar_reference independently and adds JUCE only when explicitly enabled. The optional diagnostic links the same core. Python remains an offline analysis/reporting tool, not a callback dependency.

## Alternatives and consequences

Embedding the physics directly in an AudioProcessor would simplify a small demo but couple numerical verification to host/framework details. Reimplementing a separate headless synthesizer would create drift. The chosen boundary avoids both while requiring adapters and explicit lifecycle tests.

The core’s current in-tree alias is not an installed ABI promise. Future packaging can establish one after ownership and serialization contracts are stable.

Do not pass JUCE-owned mutable buffers or editor objects into long-lived physical state. Caller-owned spans are appropriate for bounded processing, provided lifetime and concurrency are documented.

## Acceptance and reconsideration

The headless dependency graph must remain buildable without JUCE. Optional-target builds and named-host execution are separate evidence, not inferred from headless tests.

Reconsider a framework dependency only when its specific benefit outweighs the loss of portability and test isolation, with an ADR and license review. Convenience in one application is insufficient by itself.

Related: [actual API map](../architecture/01-current-code-and-proposed-engine.md) and [JUCE engineering contract](../engineering/juce-boundary.md).
