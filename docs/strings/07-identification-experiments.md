# Experiments that identify string behavior instead of merely fitting audio

## Define the inference problem

An identification experiment should discriminate between hypotheses. A recording
that sounds good can be useful artistically while containing too little
information to identify mass, stiffness, loss, contact, and radiation separately.
Begin by writing the smallest question: for example, whether a two-parameter
loss law predicts partial decays over a stated band, or whether paired strings
exchange energy through the measured bridge response.

The inputs are a specimen inventory, a parameter dependency graph, a numerical
baseline, and permission to acquire the needed observations. The outputs are
estimated parameters or bounds, residuals, uncertainty, held-out predictions,
and a decision about whether the current model class is adequate. A fitted
number without those outputs is not a completed identification task.

Read the experimental-evaluation portion of [Mansour, Woodhouse, and Scavone
2016](https://doi.org/10.3813/AAA.919021) as an example of comparing a model with
measurements from the same physical system. The project must design its own
setar-specific experiments; it cannot inherit validation from another instrument.

## Establish a hierarchy of fixtures

Use synthetic analytic fixtures to test the estimator before using it on an
instrument. Generate known modal frequencies and decay rates, add explicitly
specified observation effects, and verify recovery. This reveals estimator bias
and resolution limits without conflating them with uncertain physics. Do not
add random noise merely to make plots look realistic; choose a noise model that
tests a stated robustness question and record its seed.

Next use a controlled mechanical string fixture if available and authorized.
Its purpose is to isolate string behavior from a complex body, not to replicate
the entire instrument. The fixture itself has compliance and loss, so characterize
or bound them. A supposedly rigid clamp can become the dominant source of a
high-frequency discrepancy.

Finally use the installed instrument, first in low-amplitude simple conditions
and then in performance-relevant conditions. Preserve the distinction between
parameters identified in the fixture and effective parameters identified in the
installed instrument. A parameter may be transferable, condition-specific, or
not identifiable. Those are useful outcomes.

## Choose observations with complementary information

Fundamental frequency constrains a combination of tension, density, and length.
Higher partials add information about dispersion and boundaries. Decay envelopes
constrain loss and coupling. Bridge force and velocity reveal mechanical
transfer. Two-axis motion helps distinguish polarization. A microphone observes
radiation through the room and cannot replace all these measurements.

If only audio is available, narrow the claim. It may be possible to identify an
effective input-output model over a limited band without identifying every
physical parameter. Such a model can still support useful synthesis research if
its limits are explicit. Do not call a fitted physical coefficient measured when
several alternative coefficients produce the same output.

Record sensor transfer functions and alignment. A force sensor with an unknown
high-frequency response can make an attack seem softer than it was. A microphone
position change can alter apparent spectral balance. Calibration uncertainty
belongs in the inference, not just in an appendix nobody consults during fitting.

## Separate excitation from response

A controlled pluck should document position, direction, loading history, and
release behavior as far as the apparatus permits. A nominally identical manual
pluck is not necessarily an identical force input. Repetition is valuable, but
averaging can smear the very contact variation the model needs to explain.
Retain individual trials and align them using a stated event definition.

Use a mechanical or instrumented excitation only if it answers the current
question. A break-wire or other idealized release can help isolate linear
response, while a real nail is necessary for contact realism. These experiments
answer different questions. Neither should be advertised as a complete substitute
for the other.

For repeated-pluck experiments, preserve the phase and amplitude of the residual
motion. A gate that waits until all sound is inaudible may still leave mechanical
motion significant to the next contact. If the purpose is to study capture, that
residual state is an input, not noise to remove.

## Fit in stages, then test jointly

Start with geometry and directly observed quantities. Fit the smallest set of
remaining parameters under simple conditions, inspect residuals, and only then
add coupled mechanisms. Joint optimization of every parameter from the first
recording can produce an excellent fit with no interpretable result.

Use physically meaningful bounds and transformed variables where appropriate.
Positive quantities can be represented logarithmically in an offline optimizer,
but a transformation does not make the bounds true. Their provenance and
validity still need documentation. Report sensitivity to initialization and
whether multiple substantially different solutions fit comparably well.

After staged fitting, test the combined model without refitting on withheld
conditions. Change pluck position, amplitude, tuning, or stopping condition in a
way chosen before examining the result. If parameters must be retuned for every
note, the model may be an effective per-condition fit rather than a predictive
instrument model. That limitation should be visible.

## Identifiability and uncertainty

Compute local sensitivities or finite differences of selected observables with
respect to parameters. Inspect whether columns are nearly dependent. A poorly
conditioned sensitivity matrix indicates that some parameter combinations cannot
be separated by the current experiment. More decimal places from an optimizer
do not fix that problem.

Uncertainty should include measurement uncertainty, repeat-to-repeat variation,
and model discrepancy where possible. These are different sources. A narrow
confidence interval conditional on a wrong model can be misleadingly precise.
Use alternate plausible models or residual analysis to expose that dependence.

When reporting a fitted parameter ensemble, preserve correlations. Independent
min/max ranges can include combinations never supported by the data. A preset
generation tool should not sample those ranges independently and then describe
the result as an observed instrument population.

## Paired strings and coupling experiments

Establish each slot's response with other strings damped in a documented way,
then repeat with them free. The damping intervention can itself change bridge
loading, so interpret the difference carefully. Measure or simulate the damper
where its influence is significant. Do not assume that damped means physically
absent.

Excite one member of the paired course and observe both members if possible.
Repeat after a small controlled detuning. Changes in beating or transfer can
help distinguish direct excitation overlap from coupling. A single microphone
cannot unambiguously assign every spectral component to one member; uncertainty
in that assignment belongs in the result.

The proposed engine should reproduce the experiment with four persistent states.
If a simpler model matches only because it creates independent note voices on
every trigger, it has not reproduced the same stateful experiment. Compare
mechanical diagnostics as well as rendered sound.

## Analysis hygiene

Keep raw data immutable and store derived analyses separately. Record filtering,
resampling, windowing, alignment, and normalization. Loudness normalization can
be appropriate for a perceptual comparison but must not precede an amplitude
identification calculation without preserving the original scale.

Avoid training/test leakage through preprocessing. A filter or alignment rule
chosen after looking at all recordings can leak information from a held-out
condition. State which choices were fixed beforehand and which were exploratory.
Exploration is legitimate; it simply cannot be reported as a predeclared
confirmation.

For spectral peak tracking, retain rejected candidates and confidence measures.
Manual reassignment should be documented. If two peaks cross or merge, an
algorithm can silently swap identities and create a false decay or dispersion
trend. A report should show enough intermediate evidence for another contributor
to recognize that possibility.

## Acceptance and failure decisions

Before acquisition, define acceptance in terms of the scientific question and
the intended engineering use. Frequency tolerance may be expressed in relative
error or cents, while force and energy use physical units. Avoid one aggregate
score that allows excellent low-frequency fit to hide unacceptable contact or
high-frequency behavior.

The task passes when the estimator is verified on known fixtures, the acquired
data and permissions are traceable, the chosen parameters are identifiable enough
for their use, and held-out predictions meet the predeclared criteria or are
honestly reported as failing. A failed model is not a failed experiment if the
experiment reliably exposes the failure.

If parameters remain unidentifiable, the next action is a new discriminating
measurement, a smaller claim, or a simpler effective model. It is not arbitrary
regularization presented as physical truth. Hand the unresolved alternatives to
the contact and performance work packages so they do not build expressive
controls on a falsely certain parameter set.
