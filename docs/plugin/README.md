# Plugin and application integration

This volume specifies an optional JUCE boundary around a framework-independent
C++20 physical core. The current numerical laboratory is synthetic reference
scaffolding. A diagnostic plugin, if built, remains a diagnostic plugin until
numerical, acoustic, host, and usability evidence supports a broader claim.

1. [Architecture and boundaries](01-architecture-and-boundaries.md) defines ownership, test seams, and format scope.
2. [Parameters and state](02-parameters-state-and-compatibility.md) defines stable meaning, automation, smoothing, migration, and transactions.
3. [Host lifecycle](03-host-lifecycle-buses-latency.md) defines preparation, blocks, buses, precision, latency, tails, and offline behavior.
4. [MIDI, MPE, and tuning](04-midi-mpe-and-microtuning.md) separates protocol identity from physical gestures and tuning interpretation.
5. [Validation and packaging](05-validator-host-and-packaging-evidence.md) defines tool, host, license, and distribution evidence.
6. [Executable host-qualification playbook](06-host-qualification-playbook.md) maps actual plugin binaries to validators, test hosts, event/state cases, OS coverage and authentic screenshots.

The [real-time volume](../realtime/README.md) supplies callback and concurrency
constraints. The [UX volume](../ux/README.md) supplies musician/engineering views,
accessibility, and authentic visual evidence. Use the
[plugin prompt pack](../../prompts/plugin/README.md) for scoped implementation,
review, or diagnosis.

## Dependency and execution status

An optional build accepts an explicitly supplied JUCE checkout; headless work
must remain dependency-light and must not download JUCE implicitly. Read the
actual engineering setup and CMake configuration before invoking a target.
Dependency license review and product redistribution authorization are separate
from successful compilation. Current official APIs are cited in chapters, but
the pinned source revision governs the actual build.

For every claimed format/platform, retain binary identity, observed test logs,
and missing-check status. VST3, AU, standalone, generic validators, and actual
hosts require separate evidence. None establishes acoustic fidelity by itself.
