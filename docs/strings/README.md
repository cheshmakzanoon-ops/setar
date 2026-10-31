# Strings, contacts, and playable state

This volume specifies a proposed physical and performance architecture for an
Iranian setar. It is not evidence that the repository already implements that
architecture. A compiler accepting an interface, a synthetic fixture sounding
pleasant, and a practitioner recognizing an instrument are different outcomes.
The documents deliberately keep those outcomes separate.

The central decision is to preserve four physical string states throughout a
performance. Musical events operate on those states through excitations,
contacts, boundary changes, and controlled physical parameters. They do not
instantiate an unlimited collection of independently decaying note oscillators.
Three courses describe a useful playing organization: two single courses and a
course containing the bass and drone strings. Course membership does not merge
two strings into one differential equation. An instrument configuration records
which physical slot plays each role; a performer or maker must verify the
configuration for the actual instrument.

The content is organized by what must become known before a claim can be made.
Start with identity, geometry, and provenance; then establish the linear string;
then add interactions with demonstrated need; finally evaluate whether gestures
produce controllable, culturally credible results. The ordering is a dependency
ordering, not a schedule. No chapter assigns a deadline or promises a particular
fidelity level.

## Checked bootstrap boundary

At this volume's source inspection, `include/setar/reference.hpp` exposes a
single-polarization `ModalString`, a passive resonator, synthetic parameters, and
finite-mode triangular-displacement initialization. It does not expose four
persistent performance strings, finite nail contact, compliant frets, nonlinear
tension, or body coupling. Its `retrigger` operation is a fixture operation, not
the repeated-contact behavior specified here. The optional application adapter
must not be mistaken for those missing mechanics. Recheck the source and tests
when this capability statement is updated.

## Chapter index

| Chapter | Question answered |
| --- | --- |
| [Identity and state](01-identity-and-state.md) | What persists, and how do four strings relate to three courses? |
| [Physical specification](02-physical-specification.md) | Which parameters are measured, derived, inferred, or synthetic? |
| [Loss and dispersion](03-loss-and-dispersion.md) | Does the linear baseline solve its declared model? |
| [Polarizations and extensions](04-polarizations-and-extensions.md) | Which motion components are justified by evidence? |
| [Nonlinear tension](05-nonlinear-tension.md) | How can geometric stretching be added with a consistent energy? |
| [Tuning and retuning](06-tuning-and-retuning.md) | How do physical frets, musical intervals, and actuator work differ? |
| [Identification experiments](07-identification-experiments.md) | Which observations discriminate competing parameter explanations? |

## Reading paths

For a DSP contributor, begin with [state and identity](01-identity-and-state.md),
[physical specification](02-physical-specification.md), and
[loss and dispersion](03-loss-and-dispersion.md). Read the numerical volume before
writing an update equation. The stable explicit example in
[the FDTD chapter](../numerics/01-stiff-string-fdtd.md) has a particular temporal
discretization of its loss terms. Its bound is not permission to substitute a
different damping scheme or attach an independently designed nonlinear contact.

For an interaction contributor, begin with [the contact volume](../contacts/),
then [the performance volume](../performance/). A controller mapping is a policy
that translates limited observations into a plausible action. It is not a
measurement of nail force. The mapping should expose which variables are user
commands, inferred quantities, synthetic defaults, and measured observations.

For an experimental collaborator, begin with physical specification and
[identification experiments](07-identification-experiments.md). The essential
deliverable is not a large folder of recordings. It is a collection of documented
conditions, calibrations, physical coordinates, uncertainties, and repeatable
analyses that can distinguish competing models. Permission to listen to a
recording is not automatically permission to redistribute it or use a person's
name in product marketing.

For a practitioner, the most relevant entry is
[gesture verification](../performance/01-gesture-verification.md). The proposed
vocabulary is deliberately revisable. Transliteration, stroke labels, fingering
preferences, and ornament execution vary. The system must not turn a provisional
engineering label into a claim of a single authoritative tradition.

## What counts as evidence

Every parameter belongs to one of five provenance classes. `measured` means an
identified procedure produced a value and uncertainty for an identified physical
object. `derived` means a reproducible calculation used stated inputs.
`published` means an explicitly cited source reported the value for its own
object or model. `synthetic` means a chosen value serves an experiment or test.
`inferred` means a fit or interpretation depends on a model that may be wrong.
These are provenance labels rather than replacements for the repository's
verification and validation labels. A careful synthetic fixture is excellent
numerical evidence and poor evidence of a particular setar's construction.

An experiment should preserve its raw observation separately from its model fit.
For example, a measured sequence of partial frequencies is not the same object
as the fitted bending stiffness. A stiffness estimate can change when the
termination model improves without changing the original spectrum. Similarly,
an observed contact duration is not a universal fingernail parameter. The player,
string state, motion, camera calibration, and uncertainty travel with it.

The repository should distinguish three claim levels in reviews. A mathematical
claim states what follows from declared equations and assumptions. A numerical
claim states what a particular implementation and test established. An instrument
claim states what measurements or qualified listeners support about the target
instrument. Moving between those levels requires new evidence. A stable contact
solver cannot establish authentic riz; an enthusiastic listening comment cannot
establish energy conservation.

## Shared physical conventions

Use SI units at the model boundary. The longitudinal coordinate `x` runs from nut
to bridge. The transverse coordinate `y` lies approximately in the soundboard
plane and crosses the string array; `z` points away from the soundboard. Local
contact frames may rotate relative to this global frame, but transforms must be
explicit. A per-string displacement is relative to a documented equilibrium
configuration, not an arbitrary display origin. Geometry and equilibrium stress
are separate state components.

The string-local interval uses `x = 0` at the nut and `x = L` at the bridge.
A specimen or body frame may instead put its origin at the bridge while retaining
the same positive longitudinal direction. That is a translation, not a sign
change; store the transform explicitly when connecting measured geometry and
body ports to the string-local interval.

The symbols used throughout are speaking length `L` in metres, linear density
`mu` in kilograms per metre, tension `T` in newtons, bending rigidity `EI` in
newton square metres, transverse displacement `u` in metres, and distributed
force `f` in newtons per metre. A point force is in newtons and requires a
documented interpolation/injection pair when coupled to a spatial discretization.
The string port and body port use opposite forces and the same physical
connection velocity, so their exchanged powers cancel in a closed energy ledger.

## Implementation boundary

The intended engine is C++20 and independent of JUCE. It accepts validated,
bounded commands and returns audio plus optional diagnostics. JUCE may own host
messages, plugin buses, user interface, preset transport, and device adaptation.
It must not become a hidden dependency of string state, numerical operators, or
reference tests. No proposed interface authorizes allocations, locking, file
access, network access, or unbounded iteration in an audio callback.

The [tailored prompts](../../prompts/performance/) are work briefs, not executable
proof. Each asks for a narrow implementation, independent tests, a report of
unsupported assumptions, and a handoff artifact. A contributor must inspect the
actual repository before selecting names or adding files. Where a documented
feature is absent, its status remains proposed rather than being implied by a
cross-reference or a successful unrelated build.

## Primary reading checkpoints

The following are reading coordinates, not reproduced source material:

- Mansour, Woodhouse, and Scavone, 2016, DOI
  [10.3813/AAA.919021](https://doi.org/10.3813/AAA.919021): section 2.4 on simulating
  a pluck and section 3 on evaluating the model. The tested system is not a setar.
- Woodhouse, 2004, [On the Synthesis of Guitar
  Plucks](https://euphonics.org/wp-content/uploads/2022/03/Guitar_I.pdf), Acta
  Acustica 90, 928–944: section 2.2 provides the modal-synthesis entry point.
- Tabassian, [The
  Setar](https://www.atlasensemble.nl/assets/files/instruments/Setar/Setar%20by%20Kiya%20Tabassian.pdf):
  the opening page describes construction and playing; the following tuning
  examples require their stated musical context. Source ID `R-SETAR-TABASSIAN`
  in the research ledger supplies the shared provenance entry.
- Poorhaydari, 2023, [Empirical Evaluation of Intervals and Fretting Systems in
  Persian Art Music](https://journal.iftawm.org/previous/2023-volume-11-no-2/poorhaydari/):
  numbered paragraphs 7–14 describe the empirical and pictorial methods. Shared
  source ID: `R-FRETTING-POORHAYDARI-2023`.

Search-indexed primary text and metadata were available for the two acoustics
papers, but direct retrieval of the McGill-hosted 2016 PDF encountered a redirect
failure and the linked 2004 PDF timed out during this documentation pass. The
Atlas PDF was directly accessible. Obtain and inspect the complete original
before reproducing an algorithm; an indexed excerpt is not a substitute for its
assumptions, equations, or permission terms.

The playable prototype's synthetic string gauges, tunings and fret maps are
documented in [prototype string, tuning and fret presets](08-prototype-string-and-fret-presets.md).
They are placeholders, not a measured string set.
