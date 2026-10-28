# Start here: how to use this engineering repository

## What this repository is

This is a detailed development contract for a physically modeled Iranian setar,
plus an executable numerical scaffold. Its purpose is to let a developer arrive
without the original conversations, discover the current evidence, perform a
bounded engineering task, and leave reproducible results for the next developer.
It is not a collection of promises that all named features already work.

The target is expressive, stable, efficient sound synthesis with fidelity assessed
against identified physical instruments. The instrument class has four metal
strings arranged in three playing courses, a wooden body, movable tied frets, and
an important fingernail excitation mechanism. Exact construction and playing
choices must be attached to a specimen and practitioner, not inferred from the
instrument's name. Consult the research and ethnomusicology volumes before using
anatomical or performance assumptions in code.

The desired sound engine is primarily C++. Python may support offline analysis,
data conversion, documentation audits, and reproducible plotting. CMake handles
portable builds. JUCE is useful for plugin/application integration, parameters,
MIDI, and interfaces. None of those tools supplies missing acoustic evidence.

## First reading path

Start with the [master roadmap](ROADMAP.md), then read the
[quality contract](governance/quality-contract.md). Next choose the volume that
matches your work. A contributor investigating nail release should read contact
modeling and the relevant numerical energy interfaces. A contributor fixing state
serialization should read the plugin and real-time volumes rather than modifying
equations. A measurement researcher should begin with specimen identifiers,
calibration, consent, and uncertainty before collecting impressive-looking audio.

For implementation, the additional
[engineering specification](engineering/implementation-specification.md),
[41 requirements](../roadmap/requirements.json), and
[44 dependency-ordered `ENG-Pxx-Tyy` work orders](../roadmap/engineering-tasks.json)
turn these phases into concrete proposed source locations, procedures, independent
tests and evidence packets. The [engineering decision ledger](governance/engineering-decision-gates.md)
marks unresolved physical/host/license choices that documentation cannot
legitimately fill in. Work orders and their 79 named *planned tests* are not
implemented runtime features; use the registers to choose the next honest gate.

Each phase has entry conditions, task sequence, outputs, tests, failure rules, and
an exit gate. The gate is not an authorization to ignore uncertainties in later
phases. It states what has been demonstrated for the phase's explicitly limited
domain. A model can pass its mathematical verification gate and still fail to
represent a real setar. A convincing recording can pass a listening exercise and
still be unstable under a different gesture. Both facts belong in the evidence.

## Three distinct products

### 1. Offline reference laboratory

The reference laboratory prioritizes inspectability, accuracy, independent tests,
and controlled experiments over interactive speed. It can use smaller steps,
larger meshes, higher precision where warranted, and explicit solver diagnostics.
It should make conservation errors, boundary work, contact residuals, and geometry
assumptions observable. It is the baseline against which approximations are
measured; it is not automatically ground truth simply because it is slow.

### 2. Real-time physical engine

The real-time engine uses validated representations and controlled approximations
to meet an explicit execution budget. Its inputs are physical or performance
events with defined units, order, timestamps, and capacity. Its outputs are audio
and bounded diagnostic telemetry. It must not allocate, wait on a mutex, open a
file, call a network service, or perform an unbounded nonlinear solve in an audio
callback. Offline assets are prepared away from that callback and installed via a
documented state transition.

### 3. Instrument application and plugin

The plugin translates host events and human controls into the engine's contracts.
It owns host lifecycle, presets, parameter identifiers, presentation, accessibility,
and state migration. It must not own undocumented alternative physics that makes
the standalone renderer and plugin disagree. A JUCE editor screenshot establishes
that something rendered; it does not establish a correct boundary condition.

## Evidence labels

Use these terms precisely in documents, reports, tests, and reviews:

| Label | Meaning |
| --- | --- |
| `proposed` | An engineering design or experiment not yet established. |
| `published` | A statement or value transcribed from an identified source; not automatically accepted. |
| `synthetic` | A deliberately constructed fixture, not measured on an instrument. |
| `measured` | A result tied to specimen, method, calibration, environment, and raw record. |
| `verified` | Implementation agrees with a specified mathematical/reference property within a stated tolerance. |
| `validated` | Model predictions agree with independent observations for a defined domain and criterion. |
| `quarantined` | An input or result fails a consistency, permission, or provenance requirement and must not silently enter production. |
| `unimplemented` | A named design requirement without an executable implementation. |

Never use “measured” to describe a literature number whose original measurement
you did not acquire. Call it `published` and preserve the source's uncertainty and
limitations. Never replace a suspicious value with a plausible one without
recording the transformation, reason, and consequences.

## What counts as progress

A good change removes a specific unknown or adds a verified capability. Examples
include demonstrating modal-frequency convergence for a fixed-end test problem,
showing that a contact update has nonnegative dissipation under the chosen sign
convention, measuring a bridge mobility with force and velocity calibration, or
preventing a state-load operation from allocating on the audio thread. Adding
fifty controls to an unverified synthesizer is not equivalent progress.

The handbook contains extensive proposed work. Check the actual source tree,
tests, commit history, and CI run before stating that a feature exists. Future
developers must maintain a capability register in their change record: implemented,
tested locally, exercised in CI, acoustically validated, and deferred are separate
columns. A green badge represents its workflow's assertions, not all claims in
the handbook.

## Choosing your first task

1. Identify the phase and its dependencies.
2. Reproduce the current baseline on your platform.
3. Read the exact referenced chapter and primary source sections.
4. Write the smallest falsifiable question the task answers.
5. Choose an independent expected result before implementation.
6. Write or extend the fixture and acceptance rule.
7. Implement one bounded change behind a clear interface.
8. Collect numerical, performance, or visual artifacts as applicable.
9. Review whether the evidence actually answers the original question.
10. Update uncertainty, capability status, and the next handoff.

If a dependency is missing, contribute the missing contract or experiment rather
than guessing an implementation and hoping the next person will correct it.
There is no calendar penalty for accurately reporting that data are unavailable.

## Reading scientific sources responsibly

The repository's primary-source inventory is in [research](research/README.md).
Bibliographic availability is different from access to a full text, which is
different from access to original meshes and measurement records. A public image
of a mesh is not a reusable mesh. A finite-element result is not an executable
solver. A set of natural frequencies alone does not determine modal masses,
coupling residues, damping, or radiativity.

One especially important audit concerns published setar wood-property values.
Linear orthotropic elasticity requires a reciprocal, admissible constitutive
description. The research volume checks suspicious reported values rather than
treating publication as automatic verification. This is a normal engineering
responsibility, not a reason to reject all prior work.

## Documentation depth and word counts

The user's intended scale is roughly 200,000 words of valuable engineering
documentation. The audit reports actual prose counts and link integrity. This is
a coverage target, not a correctness metric. Repeated boilerplate, generated
enumerations, copied source material, and comments added only to inflate the count
are unacceptable. Do not mark the handbook complete because a counter passes.

The actionable unit is a work package with a physical question, a numerical or
experimental method, artifacts, and an acceptance test. Long derivations are useful
when their assumptions and implementation consequences are explicit. Additional
chapters should address genuine missing decisions, not paraphrase existing ones.

## Non-goals

- No promise of mathematically exact reproduction of every possible real-world state.
- No inferred permission to redistribute recordings, papers, meshes, or proprietary presets.
- No automatic choice of an open-source license on the owner's behalf.
- No claim that a generic plucked-string sound is an acoustically validated setar.
- No replacement of practitioner review with fixed equal-tempered or quarter-tone stereotypes.
- No mandatory graphics dependency in the headless numerical core.
- No calendar deadlines or duration estimates in the phase gates.

Proceed to [the roadmap](ROADMAP.md) and choose the next unresolved evidence gate.
