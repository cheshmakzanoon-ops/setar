# Architecture decision records

These records preserve why a boundary exists, what alternatives were considered, and what evidence would justify changing it. “Accepted” means accepted project policy or design direction; it does not mean every proposed implementation exists.

| Record | Decision | Current implementation boundary |
|---|---|---|
| [ADR-0001](ADR-0001-reference-and-realtime.md) | Keep an inspectable reference alongside a reduced runtime | Synthetic reference exists; full coupled pair is proposed |
| [ADR-0002](ADR-0002-cpp-core-and-juce-boundary.md) | C++20 core independent of JUCE | Current CMake/core enforce the narrow dependency boundary |
| [ADR-0003](ADR-0003-evidence-and-parameter-provenance.md) | Parameters carry evidence and validity | Synthetic output labels exist; measured asset registry is proposed |
| [ADR-0004](ADR-0004-explicit-dependencies-and-licenses.md) | No automatic dependency/license selection | JUCE requires an explicit existing checkout |
| [ADR-0005](ADR-0005-persistent-physical-strings.md) | Full instrument owns persistent physical strings | Current diagnostic retriggers one fixture instead |
| [ADR-0006](ADR-0006-physical-loading-and-observation.md) | Separate conjugate loading from observation | Current core observes displacement; coupled ports are proposed |

Read the [architecture volume](../architecture/README.md) for ownership and actual APIs. Update a decision by explaining the new evidence and consequences; do not erase the reason an earlier implementation was chosen. Superseding a record should preserve a link to its replacement and a migration plan.

No record grants permission to purchase software, publish private measurements, change repository protection, or claim remote CI success. Those require their own authority and evidence.
