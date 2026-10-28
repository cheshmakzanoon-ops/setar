# P17 — Evaluate physical, acoustic, perceptual, and expressive fidelity

**Status:** specification. **Prerequisites:** P16 plus independent specimen and
performer evidence. **Exit evidence:** a predeclared, domain-limited fidelity
assessment with uncertainty, ablations, and practitioner interpretation.

## Define what “high fidelity” means before listening

The goal of competitive physical-modeling quality is important, but it is not
a numerical unit and cannot be certified by matching a named commercial
product's brand. This phase converts the aspiration into falsifiable questions:
does the model predict independent observations, behave naturally under new
gestures, remain stable across expression, and convince listeners under
controlled comparison? No one test answers all four.

Read [validation](../validation/README.md),
[measurement](../measurement/README.md),
[performance](../performance/README.md), and
[acoustic radiation](../acoustics/README.md). The target is an identified setar
class/specimen and declared performance domain, not an undefined “perfect
setar” across every builder, player, environment, and musical practice.

## P17-T01: freeze claims and test partitions

Write the specific claims before opening the final held-out test set. Examples
include partial-frequency prediction on untrained finger positions, bridge
mobility under changed string tension, measured attack evolution under a
controlled stroke, or musician control naturalness in a defined gesture set.
Each claim needs observations, nuisance controls, comparison method, and a
decision rule that reflects uncertainty.

Ensure reference measurements and model outputs share the intended condition.
A changing nail, instrument humidity, microphone placement, or player support
may affect the result. Record those conditions rather than explaining any
disagreement as “human variability.” If the two conditions cannot be matched,
narrow the claim or model the condition difference explicitly.

Keep final evaluation data independent of architecture and parameter choices.
If evaluators inspect a held-out result and then tune the model repeatedly,
it has become development data. Record the exposure and establish a new
evaluation partition before making a fresh generalization claim.

## P17-T02: score mechanisms separately

Compare string partial frequencies, inharmonicity, onset trajectory, decay
behavior, contact timing, bridge transfer, body mobility, sound pressure,
directivity, and gesture-dependent response using measures appropriate to each.
Do not collapse them prematurely into one “accuracy percentage.” A wrong
mechanism may be obscured by compensating microphone equalization or output
gain while a global metric improves.

Use complex frequency response and spatial observations where available, not
only magnitude spectra. Record level alignment when applied. Align temporal
events according to the preregistered rule; freely shifting every event to
maximize similarity can hide attack-timing defects. Visualize distributions
across notes and gestures, not only the best example.

Report uncertainty and sensitivity. A difference smaller than microphone
calibration uncertainty may not support a strong conclusion; a difference
larger than combined uncertainty may indicate missing physics, wrong input
conditions, or a measurement issue. Investigate rather than reflexively
fitting another filter.

## P17-T03: evaluate ablations and generalization

Remove or simplify one mechanism at a time—string stiffness, two-polarization
motion, nail compliance, fret damping, body feedback, cavity loading, spatial
radiation—and compare against the full candidate on held-out tasks. The
question is whether that mechanism explains additional observations or
expressive behavior. Increased computation without demonstrable explanatory
value is not automatically progress.

Test parameter and environment changes not used during calibration: supported
tunings, stopped positions, dynamics, repeated excitation, microphone positions,
and physical strings with different properties. Study where the model fails.
The resulting validity boundary is a useful engineering result, even when it
reduces the scope of an initial release.

Check interactions. A contact model that works with a fixed string may fail when
coupled to a compliant bridge; a body reduction that matches an impulse may fail
under continuous gesture-driven feedback. Preserve those counterexamples as
regression tests and update the relevant earlier phase evidence.

## P17-T04: design controlled listening experiments

Predefine the listening question: discrimination, preference, similarity,
expressive responsiveness, or identification of a specific artifact. Use
appropriate procedures, controlled levels, randomized presentation, balanced
orders, and honest handling of the observation environment. A listener's
inability to distinguish two clips in one test is not proof of mathematical
or perceptual equivalence everywhere.

Distinguish blind comparison of recorded outputs from live expressive evaluation.
The latter involves response to gesture, predictability, and player adaptation.
Both matter for an instrument, but their outcome measures and confounders differ.
Record performer familiarity, control mapping, latency, and instrument experience
where relevant and permitted by consent.

Do not claim compliance with a listening standard unless its actual conditions
and analysis were followed. References such as ITU-R methodologies inform
design choices but do not automatically validate a small informal session.
Keep raw response handling, statistical analysis, exclusions, and uncertainty
specified before drawing conclusions.

## P17-T05: run practitioner-centered expressive trials

Ask qualified setar practitioners to perform a bounded set of musical actions
and assess agency, damping, repeated plucks, pitch behavior, gesture transitions,
and physical consistency. Include space for unexpected techniques rather than
forcing every response into an imported keyboard-synth vocabulary.

Treat differences among musicians as informative. The model may fit one setup
or school of technique while failing another. Document the range and avoid
declaring universal cultural authenticity from one participant. With consent,
link structured feedback to engine event logs and observation conditions so
the engineering team can reproduce actionable issues.

Evaluate whether the UI makes valid physical behavior accessible. A sound engine
may have an adequate nail model but expose it through controls too coarse to
play. Conversely, a cleverly mapped interface may conceal an incomplete
mechanical model. Keep both claims separate during interpretation.

## P17-T06: publish the evidence and limitations

Produce a result organized by claims, methods, independent data groups,
observed differences, uncertainty, competing explanations, and acceptance
decision. Include representative successes, worst cases, ablations, and
failure recordings with appropriate rights. Preserve listener analysis and
practitioner feedback boundaries.

An aspiration to Pianoteq-level responsiveness and fidelity can guide which
tests to prioritize; it is not a legal, technical, or marketing certification.
Do not imply access to third-party proprietary implementation or data. The
final decision may be to gather a new specimen, revise physics, improve control,
or narrow the product claim. None requires a calendar deadline.

> Predeclare one physical, one acoustic, one perceptual, and one expressive
> claim. Construct groupwise held-out tests with the correct specimen/setup
> controls. Compare mechanisms and ablations; present distributions and
> uncertainty, not handpicked clips. Run a listening procedure appropriate
> to its stated question and collect scoped practitioner evidence. Report
> failures and limits explicitly. Do not equate lack of ABX significance,
> a beautiful waveform, or market comparisons with 1:1 recreation.
