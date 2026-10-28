# Listening experiments: discrimination, fidelity, and playability

Listening is essential, but an informal preference for a louder or more polished
render is not evidence of physical fidelity. This chapter defines a proposed
project workflow for controlled listening and practitioner evaluation. It does
not claim that a panel has been run. Begin only after candidates satisfy relevant
numerical safety gates and the stimuli have permission for the intended use.

## Choose the question before the test

Separate four questions: can listeners detect a difference; which rendering
better matches a specified reference; which rendering they prefer; and whether
the instrument responds usefully while played. These are different outcomes.
A listener may prefer a physically inaccurate sound. A model may be detectably
different yet musically convincing. A playback comparison cannot establish
interactive responsiveness because the performer did not control the gesture.

ITU-R [BS.1116-3](https://www.itu.int/rec/R-REC-BS.1116-3-201502-I/en) addresses
small-impairment assessment; [BS.1534-3](https://www.itu.int/rec/R-REC-BS.1534-3-201510-I/en)
addresses intermediate-quality assessment and defines MUSHRA. Their Annex 1
sections cover experimental design, listeners, method, material, analysis, and
reporting. Consult the actual documents for a standards-conforming experiment.
The project procedures below are original adaptations for physical-instrument
research; deviations must be named and must not be advertised as conformance.

## Preregister the decision

Before evaluating the final candidate, record the hypothesis, primary endpoint,
stimulus population, listener population, randomization, exclusions, sample-size
justification, analysis, multiplicity policy, and stopping rule. Distinguish
confirmatory outcomes from exploratory descriptions. If a pilot changes the
design, freeze a new protocol before collecting the confirmatory data.

Select the smallest effect that would change an engineering decision, then
evaluate the design's ability to detect or bound it. There is no universal
listener count for this project. The number depends on within-listener variation,
stimulus diversity, dependence among responses, and the intended generalization.
Use a simulation or power analysis based on defensible pilot assumptions, and
show sensitivity to those assumptions. Do not stop because a favorable significance
threshold has just appeared unless that sequential procedure was planned.

The preregistration should also name the acoustic model revision and stimulus
hashes. A “small bug fix” after stimulus generation can change the comparison;
either retain the original candidate identity or create a revised experiment.
The analysis script should be exercised on synthetic responses before unblinding.

## Build stimuli without hidden advantages

Use held-out gestures and notes not selected solely because they flatter the
candidate. Include easy and difficult conditions from the declared domain.
Preserve the reference's provenance, observation geometry, and allowed processing.
Avoid placing a dry modeled signal against a reverberant recording and then
interpreting room differences as instrument differences.

For timbre comparison, level-match using a declared method and window, then verify
that residual loudness differences do not dominate judgments. Peak normalization
alone may not produce equivalent perceived loudness. The method must not alter
attack or decay independently unless that is an explicitly separate experiment.
Store the applied gain and an unadjusted version. For dynamic-response evaluation,
use calibrated levels instead of per-stimulus normalization.

Apply the same encoding, sample-rate conversion, playback path, and allowed
processing to all conditions. Strip identifying filenames and metadata from the
listener interface. Avoid candidate-specific silences, lengths, noise floors, or
clicks that reveal identity. If those differences are intrinsic to the phenomenon
under test, retain them and explain that the task includes them.

## ABX discrimination

In an ABX task, listeners compare labeled references A and B with X, which is
one of them under a randomized hidden assignment. The task measures successful
identity discrimination under its playback and repetition rules. Balance or
randomize assignments as specified, preserve the assignment log, and prevent
display cues or file-loading delays from leaking the answer.

For an elementary independent forced-choice design, chance accuracy is one half
and binomial calculations can be informative. Repeated listeners, repeated
stimuli, training, and response dependencies can violate the simple independent
trial model. Use an analysis that respects those dependencies, such as a
prespecified hierarchical model or clustered uncertainty procedure, and report
listener and stimulus variation.

Failure to reject chance discrimination is not proof of equivalence or
indistinguishability. Report an interval and what effect sizes remain compatible
with the data. If equivalence is the actual question, define a scientifically
meaningful equivalence region and adequate design in advance. Do not reinterpret
an underpowered null result as a commercial-quality certificate.

ABX of two synthetic revisions can identify audible regression, but it does not
establish which is closer to a real instrument. Add a reference-matching or
descriptive task if that is the engineering decision. Keep those endpoints
separate rather than collapsing them into one flattering score.

## Reference matching and graded judgments

For larger differences, a multi-condition reference-matching task can expose
which mechanisms dominate. Provide clear attributes such as attack character,
decay behavior, ringing interaction, and overall resemblance, with training that
explains the task without coaching toward a preferred candidate. Use a hidden
reference and suitable controls only when the chosen protocol supports them.
Do not call an arbitrary slider panel “MUSHRA” merely because it has several
audio buttons.

Anchors should demonstrate a relevant degradation and be chosen before results
are seen. An absurdly bad anchor can inflate apparent quality; an anchor with an
unrelated defect can distort the meaning of the scale. Pilot whether listeners
understand the intended attributes. Retain comments as qualitative evidence,
not as a substitute for the registered quantitative endpoint.

When a real performance cannot be reproduced exactly, acknowledge the comparison's
input uncertainty. A model and recording may differ because the gesture differs.
Use controlled excitation where possible and complement musical passages with
mechanism-specific stimuli. Avoid interpreting every listener comment as a direct
diagnosis of a physical parameter.

## Listener groups and practitioner expertise

Separate experienced setar practitioners, trained critical listeners, and other
musicians when their expertise addresses different questions. Do not pool them
without checking whether the intended population and analysis justify it.
Practitioners can identify idiomatic gestures and tuning behavior that generic
audio listeners may overlook. Critical listeners may identify small artifacts
that performers do not emphasize during playing.

Define training, equipment checks, consent, compensation handling, anonymization,
and exclusion criteria before collection. Exclude because of a prespecified
quality or protocol issue, not because someone dislikes the preferred model.
Report exclusions and their effect. Keep participant identifiers separate from
public response data. Provide comfortable playback levels and a way to stop or
take a break; fatigue and discomfort degrade both ethics and data quality.

## Interactive playability study

Use actual running builds with fixed mappings, audio settings, and controller
hardware. Record the build, controller configuration, end-to-end latency context,
and tasks. Ask performers to tune, sustain a ringing string, execute specified
ornaments, change dynamics, recover from an error, and understand a parameter
change. Observe control errors and successful recovery rather than asking only
whether the interface looks attractive.

Compare mappings under a counterbalanced order where practical. Give adequate
familiarization without treating learning difficulty as irrelevant. Separate
first-use discoverability from skilled control after practice. Record latency
and controller jitter as potential confounders. A stronger preference for one
mapping may arise from lower latency rather than better physical expressiveness.

Interactive studies may be unblinded because controls are visible. Name this
limitation. Do not combine their ratings with a blinded playback panel as though
they arose from one homogeneous experiment. Use each study to support the claim
it was designed to answer.

## Analysis, reporting, and gate

Show effect sizes, uncertainty, listener variation, stimulus variation, and
registered decisions. Distinguish exploratory subgroup observations from tested
hypotheses. Report negative results and failures of controls. A panel unable to
distinguish an intentionally degraded control may be uninformative for the
target difference; investigate the design rather than deleting inconvenient data.

The packet includes protocol, stimulus manifest, gains, randomization, playback
configuration, anonymized responses, exclusions, analysis code, and a claim-limited
summary. The gate passes only the registered perceptual claim. It does not
retroactively verify numerical stability, host safety, or cultural authenticity.
The next action is a targeted mechanism or mapping experiment supported by the
observed pattern, not a general declaration that listeners “proved realism.”
