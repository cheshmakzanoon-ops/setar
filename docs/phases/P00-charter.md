# P00 — Define the instrument, claims, and scientific contract

**Status:** specification. **Prerequisites:** none. **Exit evidence:** reviewed
scope, requirement register, claim vocabulary, and risk ownership. This phase has
no delivery date and is revisited when the target changes.

## Why this phase precedes equations

An instrument name does not define a complete mathematical problem. “Setar”
includes different makers, materials, strings, fret arrangements, tunings, player
techniques, and recording conditions. A goal of “perfect sound” cannot tell an
engineer whether an amplitude discrepancy comes from a model error, a microphone
position, a tuning difference, or a different nail gesture. Scope must identify
which of those variables are controlled, varied, or outside the initial claim.

Read [orientation](../START_HERE.md), the [quality contract](../governance/quality-contract.md),
and the [ethnomusicology index](../ethnomusicology/README.md). Treat the user goal
of commercial-class physical-modeling quality as a design ambition. Translate
that ambition into specific, falsifiable requirements without inventing numerical
tolerances that have no perceptual or measurement basis.

## P00-T01: define the reference domains

Separate three domains in the charter. The physical domain lists specimens,
strings, environmental conditions, support conditions, and player contact. The
musical domain lists tunings, gestures, dynamics, tempo-dependent behavior,
ornaments, and allowed simultaneous sounds. The software domain lists host
formats, platforms, sample rates, block sizes, offline rendering, state recall,
and interface accessibility.

For each domain, write an initial inclusion list, exclusion list, and uncertainty
list. An inclusion means the project intends to collect enough evidence to make
a claim; it does not mean the implementation already works. An exclusion means
the release must not advertise the omitted behavior. An uncertainty means an
experiment or decision is needed before assigning either status.

Do not classify instrument structure from a stock photograph. Record the source
of construction facts and the person who will verify them against the selected
specimen. If no specimen is yet available, state that the initial executable work
is synthetic verification. Avoid inventing a maker/model identity to make the
charter appear complete.

## P00-T02: establish the claim ladder

Adopt the quality contract's Q0–Q4 labels. Define how a result advances through
proposal, executable reference, verified component, independently validated
component, and integrated qualification. Require a separate statement of domain
at each transition. A model that passes a lossless single-mode test has not
demonstrated a lossy four-string instrument.

Use negative claims to expose ambiguity. Examples include “this plot does not
validate fingernail excitation,” “this test does not run inside a DAW,” and “these
frequencies were transcribed from a paper, not remeasured.” Such statements
should clarify a specific boundary rather than fill every paragraph with generic
disclaimers. Place the limitation beside the result that could otherwise mislead.

Acceptance for this task is an example evidence packet at each proposed level,
including a deliberately insufficient packet and the reason it fails. Reviewers
should agree on classification without knowing who produced the work.

## P00-T03: specify the physical-model constraint

The synthesis source is physical state, excitation, coupling, and observation.
Measured model parameters are permitted and usually essential. Reference
recordings are permitted for analysis and independent validation subject to
permissions. Replaying recorded instrument notes is not the engine. If an
impulse response, fitted filter, or reduced model is introduced, identify what
physical quantity it represents, how it was obtained, its validity range, and
whether it participates in feedback or merely observation.

An impulse response used only as a post-filter does not reproduce the reciprocal
string–body interaction. A measured modal model can be physically meaningful
without solving a spatial mesh at every audio sample. Conversely, a huge
finite-element mesh can be inaccurate if its materials and boundary conditions
are wrong. This project evaluates derivation and evidence, not the apparent
complexity of the algorithm.

## P00-T04: define interface and responsibility boundaries

Assign ownership for physical model specification, numerical implementation,
measurement, real-time integration, plugin behavior, interfaces, and validation.
The same person may fill several roles, but each change must identify the
perspectives that reviewed it. Keep model input/outputs separate from GUI units
and user-facing macro controls. A slider value is not a physical parameter until
its mapping is defined.

Write an architecture decision that the core is C++20 and independent of JUCE.
The offline reference renderer and plugin must share defined physical interfaces,
not hidden duplicate implementations. Data import belongs away from the audio
callback. Tooling may use Python for analysis and diagrams; those scripts must
not silently become the deployed sound generator.

## P00-T05: define measurable ambitions without premature targets

For pitch, specify whether error is measured in cents relative to an analytic
string, an experimental partial, or a performer-selected tuning. For decay,
define the window, partial tracking, noise threshold, and environmental controls.
For responsiveness, define event-to-output behavior separately from audio-driver
round-trip latency. For CPU budget, identify a hardware class and worst-case
state before selecting a numerical limit. For realism, specify the task a
listener performs and the population to whom the conclusion applies.

If a target cannot yet be justified, mark it proposed and give the experiment
that will set it. Do not create a table of impressive percentages that no one
can measure. A future revision must preserve the old target and explain why
the new one better serves the claim, rather than overwrite a failed criterion.

## Outputs and tests

Produce a charter, claim examples, requirement identifiers, ownership map, and
initial risk register. Review the charter against three counterexamples: a
generic guitar-like model with a setar label, a physically sophisticated model
with no measurement validation, and a sample player that sounds excellent but
violates the synthesis constraint. The charter must classify all three correctly.

The documentation auditor checks links and phase dependencies, not the
substantive quality of this work. A human review must establish that the scope
is bounded and internally coherent. Record unresolved choices explicitly.

## Developer prompt

> Read P00 and the quality contract. Draft a scope amendment for the specific
> requested capability. Identify the physical, musical, and software domains it
> changes; list evidence already present and evidence still missing. Propose
> falsifiable requirements without claiming unmeasured accuracy. Do not implement
> DSP or alter licensing, CI permissions, or release claims. Return the amended
> charter, a requirement-to-evidence table, and the smallest next experiment.

## Exit gate and handoff

Advance when the owner/reviewers can explain the intended instrument, the
permitted model/data practices, the claim ladder, and what the first release will
not claim. Handoff the unresolved measurements to P01 and the executable
verification requirements to P02. Keep P00 open to evidence-driven refinement.
