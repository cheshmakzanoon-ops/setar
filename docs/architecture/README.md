# Architecture: implemented reference and proposed instrument

This volume separates what the repository executes from the architecture it intends to establish. The current public API is [include/setar/reference.hpp](../../include/setar/reference.hpp), implemented by [src/reference.cpp](../../src/reference.cpp). It is a synthetic numerical reference, not a complete setar engine.

Read these documents in order:

1. [Current code and proposed engine boundaries](01-current-code-and-proposed-engine.md).
2. [Physical ports and state ownership](02-physical-ports-and-state-ownership.md).
3. [Evidence assets and scheduling boundaries](03-evidence-assets-and-scheduling-boundaries.md).

Use the [engineering volume](../engineering/README.md) for exact build commands and executable behavior, the [numerics volume](../numerics/README.md) for numerical contracts, and [decision records](../decisions/README.md) for accepted policy and unresolved implementation consequences.

A proposed interface is not an existing class. A source-level bounded loop is not measured real-time qualification. Configured CI is not a passed remote run. Every architecture change must preserve these distinctions in code, tests, reports, and visible product language.
