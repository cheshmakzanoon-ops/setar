# Architecture: implemented reference and proposed instrument

The architecture remains evidence-gated: numerical interfaces should only be
frozen after their units, discrete power balance, and lifecycle are verified.

This volume separates what the repository executes from the architecture it intends to establish. The current public API is [include/setar/reference.hpp](../../include/setar/reference.hpp), implemented by [src/reference.cpp](../../src/reference.cpp). It is a synthetic numerical reference, not a complete setar engine.

Read these documents in order:

1. [Current code and proposed engine boundaries](01-current-code-and-proposed-engine.md).
2. [Physical ports and state ownership](02-physical-ports-and-state-ownership.md).
3. [Evidence assets and scheduling boundaries](03-evidence-assets-and-scheduling-boundaries.md).
4. [Proposed C++20 interfaces and ownership](04-cpp20-module-contracts.md).
5. [Discrete solver selection, contact, and port work](05-discrete-solver-selection.md).
6. [Physical/evidence flow diagrams](06-physical-and-evidence-flow.md).

Use the [engineering volume](../engineering/README.md) for exact build commands and executable behavior, the [numerics volume](../numerics/README.md) for numerical contracts, and [decision records](../decisions/README.md) for accepted policy and unresolved implementation consequences.

A proposed interface is not an existing class. A source-level bounded loop is not measured real-time qualification. Configured CI is not a passed remote run. Every architecture change must preserve these distinctions in code, tests, reports, and visible product language.
