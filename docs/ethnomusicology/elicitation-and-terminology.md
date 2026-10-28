# Expert elicitation, terminology, and musical task design

## Purpose

Expert consultation should reveal requirements and challenge assumptions, not
provide quotations that decorate a predetermined design. The project needs
players, makers, and scholars to explain which distinctions matter and where the
engineering representation is misleading. Their input becomes evidence when
its context, scope, and interpretation are recorded.

This chapter is an original proposed workflow. R-SETAR-TABASSIAN and
R-FRETTING-POORHAYDARI-2023 identify useful starting contexts; R-ETHICS-OHA
provides a primary professional reference for respectful interview practice.
None of these sources replaces project-specific consent, institutional review
where applicable, or qualified rights advice.

## Build a panel around questions

Recruit expertise relevant to the intended claims. A performer can assess
articulation and musical response; a maker can explain construction and setup;
a scholar can help interpret terminology and historical claims. These roles may
overlap, but do not assume that one famous participant represents all of them.
Document the recruitment rationale and the kinds of practice not represented.

Seek variation that matters to the study without tokenizing participants. Training
background, repertoire, instrument setup, teaching practice, and experience with
electronic controllers can affect feedback. Ask only for information needed to
interpret the study, and let participants describe their own background. Do not
infer school, identity, or authority from names or appearance.

Separate advisory participants from final evaluators where feasible. Advisors
who hear every prototype provide valuable longitudinal feedback but are not
independent blinded listeners. A participant can occupy different roles in
different studies if the access history is recorded.

## Prepare without leading

Before interviewing, read the relevant sources and define the engineering
unknowns. Convert each unknown into an open question and a possible demonstration.
Avoid starting with the desired answer. Instead of “Is this quarter-tone preset
correct?”, ask how the participant sets the relevant frets and how that choice
changes with the task. Instead of “Does this model need more brightness?”, ask
which audible differences make the example unlike their intended sound.

Prepare alternative explanations for expected observations. If a participant
describes a short sound, it might involve pluck, damping, body response, recording
position, or musical context. A follow-up demonstration can help distinguish
these without asking the participant to adopt engineering terminology.

Give a plain description of the project: a physical synthesis engine, the
measurements proposed, the planned uses of recordings, and the limits of current
implementation. Do not describe a generic string prototype as a nearly finished
setar in order to elicit favorable feedback. Participants should know what is
actually being tested.

## Interview structure

Begin with the participant's own description of the instrument, setup, and tasks
they consider important. Then move to concrete demonstrations. Ask for contrasts:
two attacks that a novice might confuse, a resonance that should continue versus
one that should stop, or a tuning choice that changes with context. Contrasts
often reveal boundaries more clearly than isolated definitions.

After each demonstration, ask what the participant intended, what they actually
did, and whether the take was representative. Do not assume every performed
example is an endorsed target. Preserve corrections and comments. If the
participant demonstrates an intentionally undesirable sound, label it as such
so it cannot later become a reference exemplar by filename accident.

Close by asking what the proposed model or task list leaves out. This gives
participants a chance to challenge the framing rather than merely answer within
it. Record unanswered questions and topics outside their expertise without
pressuring them to produce an opinion.

## Terminology record

For each term, store the original script when supplied, the participant's chosen
transliteration, language, pronunciation reference if permission allows, literal
or explanatory translation, contextual definition, and demonstrated examples.
Keep an engineering identifier separate from the displayed term. This permits
stable code while terminology is refined.

Do not force one transliteration across all source quotations. Use a consistent
editorial form for project prose while preserving original forms in attribution
and search metadata. If two spellings refer to the same operational concept,
record aliases. If one spelling is used for different concepts, record the
ambiguity rather than merging the entries automatically.

Translations should be reviewed by a competent speaker familiar with the musical
context. Literal translation can be misleading when a term refers to a technique
or role rather than an everyday word. Mark uncertain translations and avoid
product labels that imply more certainty than the review supports.

## From description to operational task

An operational task specifies starting state, allowed actions, intended contrast,
observable outputs, and success criteria as understood by the participant. For
example, a resonance task can define which strings begin undamped, which string
is intentionally excited, and when a damping action occurs. It need not require
the player to reproduce an exact force waveform unless that is the explicit
mechanism experiment.

Create paired controlled and natural versions. The controlled version isolates
one factor and supports physical analysis. The natural version tests whether the
same distinction remains meaningful in a phrase. If the controlled task changes
the technique so much that the participant rejects it as representative, label
the limitation and revise the task rather than insisting on laboratory convenience.

Attach task identifiers to recordings, annotations, and model tests. This allows
an implementation issue to point back to the musical requirement. A bug report
that says “too artificial” is difficult to act on; a report that says a specified
cross-string resonance contrast disappears under a named gesture is testable.

## Avoiding observer and designer effects

Do not reveal the model variant or desired ranking in a blinded comparison.
Randomize presentation and use neutral labels. Keep the facilitator's prompts
consistent. If the participant asks what changed, explain the study design
without disclosing the condition before their response when blinding is part
of the protocol.

Separate exploratory discussion from confirmatory rating. In an exploratory
session, follow-up questions and prototype changes are useful. In a confirmatory
test, changing the task after seeing responses undermines the planned inference.
Both kinds of work are valuable if their status is explicit.

Record familiarity with prototypes and prior involvement. A developer-musician
who knows every model limitation may evaluate differently from a new user.
Neither perspective is invalid, but they answer different questions. Do not
pool them without considering the role difference.

## Handling disagreement

Classify disagreement before resolving it. Participants may disagree about
terminology, preferred setup, the goal of a task, whether a take represents that
goal, or the model's success. These require different responses. A terminology
conflict may need a glossary alias; a setup difference may need separate presets;
a disputed physical mechanism may need measurement.

Do not average incompatible target tunings into an allegedly authoritative
middle value. Preserve contextual alternatives and test whether the model can
represent both. Do not discard a minority criticism simply because most listeners
prefer the model. The criticism may identify an important untested technique or
a subgroup outside the original claim.

When consensus is necessary for a product decision, document the decision rule
and whose scope it represents. A practical default can be chosen without claiming
that alternatives are wrong. The product should expose relevant configurability
where it is safe and usable rather than hiding all variation in one preset.

## Review and attribution

Provide participants an opportunity to review the portions of interviews,
translations, labels, and public examples attributed to them according to the
agreed process. Keep technical analysis and participant statements distinguishable.
If the engineering team infers a mechanism that the participant did not state,
label it as the team's inference.

Credit contributions accurately and with permission. Consulting, performing,
co-designing, reviewing terminology, and endorsing a product are different roles.
Do not upgrade one into another in acknowledgments or marketing. Compensation,
authorship, access to results, and review expectations should be discussed
explicitly rather than assumed from goodwill.

## Elicitation packet

Deliver the question guide, consent linkage, participant role descriptions,
task matrix, terminology records, reviewed excerpts where permitted, annotation
guide, disagreement register, and resulting engineering requirements. Include
requirements rejected or deferred and why. This prevents the team from
remembering only the comments that supported its preferred architecture.

The gate passes when each important requirement can be traced to a participant
context, observed demonstration, or explicit project design decision. It fails
when quotations are detached from context, when one participant is treated as
the universal authority, or when a leading demonstration is presented as an
independent evaluation. The next action is a small reviewed task pilot before
large-scale acquisition or product labeling.
