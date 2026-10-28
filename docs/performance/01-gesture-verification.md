# A practitioner-verified gesture vocabulary

## Treat vocabulary as an empirical interface

The model needs a vocabulary that musicians can use without accepting the
engineer's hidden assumptions. A named technique can encompass several
trajectories, accents, and left-hand relationships. Conversely, two gestures
that look similar to an engineer can have different musical functions. Build
the vocabulary through documented collaboration rather than a list copied from
an unrelated instrument tutorial.

The target is Iranian setar. Kiya Tabassian's [Atlas instrument
note](https://www.atlasensemble.nl/assets/files/instruments/Setar/Setar%20by%20Kiya%20Tabassian.pdf)
is a useful performer-authored orientation to its nail-based, bidirectional
playing. It is not a complete technique taxonomy. Terms such as riz and
left-hand ornament labels need verification with the practitioners who will
evaluate this model, including their preferred spelling and explanation.

The work package's inputs are a documented instrument configuration, the current
mechanical capability list, and voluntary practitioner participation. Its outputs
are a versioned gesture glossary, reference examples with permissions, mappings
to physical actions, known variation, and unresolved disagreements. The glossary
is a living interface, not a declaration of one universal tradition.

## Separate intent, execution, and observation

For every gesture, record the musical intent in the practitioner's terms. Then
record the visible or described execution: stroke direction, selected strings,
left-hand contact, timing relationship, and any relevant accent. Finally record
what was actually observed or measured. These three layers may not coincide
perfectly, and the difference is valuable.

For example, a request for a rapid alternating texture may lead to unequal
forward and reverse strokes, changing string selection, or deliberate accents.
An engineer who reduces the request to an evenly spaced note repeater has made
a modeling choice. That choice can be tested, but it should not be attributed to
the practitioner without confirmation.

Likewise, a finger-driven onset without a new right-hand stroke may involve
contact loading, release, or another mechanism. Use a neutral provisional
description until the practitioner supplies or approves a term. Do not assume
that guitar labels such as hammer-on and pull-off capture all intended setar
details, even if they are useful comparative language in an engineering note.

## A compact gesture record

A record should contain a stable gesture ID, preferred label, alternate
transliterations if supplied, language context, practitioner attribution
permission, instrument configuration, target strings or courses, and a concise
intent description. Add a physical-action hypothesis, not a claim of certainty:
which contacts move, which remain, and which variables are controlled.

Include examples and counterexamples. A counterexample is particularly useful
when two commands sound similar but feel different to the performer. Record
whether the distinction is rhythmic, timbral, fingering-related, contextual, or
not yet understood. Do not force every distinction into the parameters the
current engine happens to expose.

The record also needs an evidence status. A label can be practitioner-confirmed
while the force trajectory remains unmeasured. A synthetic demonstration can
illustrate the proposed mapping while providing no empirical validation. These
statuses should be independently visible.

## Designing a review session

Coordinate any recorded motion study with the [pluck/contact acquisition
protocol](../measurement/pluck-contact-and-motion.md). The glossary supplies
intent and terminology; the acquisition record supplies observations and
uncertainty. Neither should be substituted for the other when constructing a
physical trajectory.

Prepare a small set of concrete questions rather than an open-ended request to
certify authenticity. Ask which strings are normally intended in the example,
what changes when the stroke reverses, what the left hand preserves between
events, and what a performer would change to make the result more natural.
Allow the practitioner to reject the premise or demonstrate a different variant.

Show the current limitations before collecting feedback. If the model has no
finite nail contact or no compliant fret, say so. Otherwise the practitioner may
spend effort describing a missing mechanism while the team mistakes the feedback
for a minor preset-tuning request. An honest capability list makes the session
more useful.

Obtain permission separately for recording, analysis, redistribution, public
attribution, and use of quotations. Participation should not require broad
rights. Keep private contact details out of public repository files. Preserve a
withdrawal or correction process for materials under the project's control,
subject to any explicit agreement.

## Riz as a structured verification problem

Use riz as a practitioner-defined target, not an excuse for a generic tremolo
LFO. Ask how stroke alternation, grouping, accents, selected strings, and phrase
context should be represented in the intended examples. Record whether the
performer considers a perfectly periodic sequence a useful exercise, an
inadequate musical rendering, or a particular valid variant.

Mechanically, each stroke must encounter the continued string state. A sequence
generator should issue trajectories or contact commands with stable identities,
not reset the string at every onset. Variation in prior phase and residual energy
can affect the next contact. The validation matrix must therefore include
sequences, not only isolated plucks.

Distinguish deliberate variation from stochastic decoration. If timing or
strength varies according to an accent pattern, encode that pattern. If a
stochastic model is explored, state its purpose and compare it with measured
variation. Independent random jitter is not automatically human or stylistically
appropriate. A deterministic sequence can be more faithful than an unjustified
random one.

## Left-hand continuity

Ask what remains in contact when a higher finger is added, when it is removed,
and when the phrase returns to an open string. A physical model can preserve
several fingers and their contact histories. A single current-note variable
cannot represent every such relationship.

For each reviewed gesture, record the ordering of finger engagement, right-hand
stroke, pressure change, and release. The ordering may matter even when events
are close together. If observations cannot resolve the order, retain an
uncertainty interval and test plausible alternatives instead of inventing a
precise sequence.

Slides and pitch inflections require special care. A continuous pitch command
may represent lateral tension change, changing contact position, pressure, or an
assisted nonphysical control. Ask which interpretation is intended. The interface
should not map every expressive pitch movement to a hidden resampler and call it
a left-hand model.

## Tuning and repertoire context

A gesture can depend on open-string tuning, available fret positions, and which
course supplies a reference or drone. Review it in its intended tuning context.
Transposing a controller pattern without changing physical constraints can
produce a request no longer playable on the configured instrument.

Store context without turning one example into a universal rule. A practitioner's
preferred tuning or fingering is evidence about that practice and example.
Different practitioners may make different legitimate choices. Preserve the
differences and, where useful, provide named profiles with permission rather
than averaging them into a supposedly neutral default.

Do not assume a uniform quarter-tone keyboard describes the fret layout. The
physical tuning chapter keeps measured positions, intended intervals, and
realized frequencies separate. Gesture review should use those distinctions so
a mapping error is not mistaken for poor intonation by the performer.

## From observation to implementation task

Translate each confirmed requirement into a falsifiable engineering statement.
For example: a repeated stroke must preserve the prior string state; releasing
one finger must reveal another retained stop; a course sweep must allow distinct
encounter times for its two members. These statements can be tested without
claiming that a single waveform defines the technique.

Choose a minimal implementation that addresses one statement. If the required
mechanics are absent, create a synthetic interpreter test first and identify the
missing physical dependency. Do not add a compensating audio effect and mark the
gesture complete. A temporary audition prototype can still be useful if labeled
as such.

Return the implementation and its limitations to practitioners for another review.
Ask whether the intended control dimension is usable and whether the behavior
changes in the expected direction. Do not ask them to infer numerical stability
or parameter provenance from listening; those remain engineering obligations.

## Acceptance and disagreement

The work passes when the glossary is traceable, permissions are recorded,
physical hypotheses are explicit, and reviewed examples lead to testable
requirements. It does not require every practitioner to agree or every gesture
to be implemented. An unresolved distinction is a valid output when its
consequences and next discriminating question are clear.

For later fidelity claims, use held-out phrases and practitioners where feasible,
report the evaluation conditions, and avoid generalizing from a single positive
comment. A musician saying that one gesture feels useful is strong evidence for
that interaction and weak evidence for universal instrument authenticity.

The next action is to encode the verified vocabulary through the event schema.
The schema must preserve uncertainty and ownership without embedding cultural
assumptions into low-level numerical types. Technique names belong in profiles
and documentation; physical events describe what the engine is actually asked
to do.
