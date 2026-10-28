# Acoustic metrics: pitch, spectra, attacks, and decay

This chapter turns acoustic comparison into a set of observable-specific
experiments. It assumes a versioned reference corpus and a frozen candidate
model. It does not prescribe universal tolerances. All numerical acceptance
limits begin as proposed entries in the threshold registry and require a
measurement uncertainty analysis. The output is a multidimensional result packet,
not a single unexplained “realism score.”

## Define the observation before rendering

A string displacement, bridge velocity, radiated pressure, and microphone signal
are different observables. Compare like with like. A displacement spectrum can
have very different weighting from a velocity spectrum, and microphone geometry
can alter a radiated spectrum. Name the observation operator, units, axis,
location, and any calibration. If the physical model stops at bridge force,
do not compare it directly with microphone voltage and attribute every mismatch
to the strings.

For a controlled comparison, fix the specimen, tuning, string configuration,
gesture, and receiver view. Declare whether the input is measured or inferred.
Preserve native output before permitted conditioning. Generate at least one
internal-state diagnostic so that a radiation mismatch can be distinguished
from a string-motion mismatch. A post-filter may improve the microphone match
while worsening the inferred bridge mechanics; the internal diagnostic exposes
that compensation.

## Alignment policy

Separate acquisition synchronization from aesthetic alignment. Correct a known
sensor delay using its calibration. Estimate an unknown global offset only within
a preregistered range and record the fitted value. Do not allow unrestricted
dynamic time warping in a test of gesture timing. It can erase incorrect contact
duration, attack slope, or vibrato phase.

Provide both a tightly synchronized comparison where possible and a
phase-insensitive feature comparison. Samplewise error is appropriate for
deterministic synthetic fixtures with identical initialization. It is often
misleading for separately performed plucks: tiny pitch or onset differences
produce large waveform error despite a similar envelope. Feature comparisons
are less phase-sensitive, but can conceal phase-dependent beating and stereo
errors. The two views answer different questions.

## Fundamental and partial frequency

Estimate frequency over declared time intervals, including onset and late decay
when signal quality permits. A single FFT peak is not a complete pitch estimate.
The window, interpolation, harmonic assignment, and confidence rule matter.
Verify the extractor on synthetic inharmonic strings and close mode pairs before
using it on measurements.

For positive reference and candidate frequencies, cents error is

\[
e_c=1200\log_2(f_{\mathrm{candidate}}/f_{\mathrm{reference}}).
\]

Report the estimate and uncertainty for each resolved partial, not just the
fundamental. Track time-varying frequency if the mechanism predicts it. Use
consistent modal assignment; near crossings, the nearest-frequency rule can
swap identities. Mark unresolved or ambiguous partials rather than inventing
correspondence. Distinguish apparent peak movement caused by two beating modes
from a genuine time-varying modal frequency.

An inharmonicity summary can aid diagnosis, but does not replace partial
trajectories. A fitted coefficient may be a convenient descriptive statistic
without uniquely identifying stiffness or boundary compliance. State the model
used to obtain it and the register over which that description is adequate.
Retuning the candidate's fundamental before comparison is allowed only for a
separate timbre experiment; it invalidates the original tuning-accuracy test.

## Spectral envelope and multi-resolution analysis

Use a documented family of analysis windows. Short windows resolve attack timing
and lose frequency resolution; long windows resolve close modes and smear onset.
Multiple resolutions reduce dependence on one arbitrary compromise, but their
scores remain correlated. Report component metrics and do not count them as
independent confirmations.

A log-magnitude error needs a floor because near-zero bins otherwise dominate.
Tie the floor to a declared noise or dynamic-range policy, not whichever value
makes the plot attractive. State whether bins are linearly or perceptually
weighted, whether broadband noise is included, and whether the comparison uses
absolute calibrated levels or a permitted shared gain. A spectral centroid alone
can be matched by many physically different spectra.

Inspect residual spectrograms with common axes and color limits. Use the same
reference amplitude for candidate, measurement, and difference images. Automatic
per-image normalization can make missing high-frequency attack content appear
equally strong. Include a numeric table for selected bands and a trace of the
noise floor. Label any smoothing across frequency, especially near narrow body
modes that smoothing may erase.

## Decay is not necessarily one exponential

A logarithmic envelope can show an early slope, a later slope, beating, contact
interruptions, and a noise-limited tail. Do not fit one straight line over all
of it by default. Define fit windows from physical questions and signal quality,
then report sensitivity to reasonable alternatives. An early slope can describe
initial energy transfer while a late slope reflects a different coupled mode.

For a resolved mode with amplitude envelope (A(t)=A_0e^{-\sigma t}\), amplitude
decays by sixty decibels after \(\ln(1000)/\sigma\). This identity is useful for
synthetic verification. Do not claim a measured sixty-decibel decay time if the
recording only resolves a much shorter dynamic range; name any extrapolation
and its assumption. Distinguish amplitude decay from energy decay so the factor
of two in exponential rates is not lost.

Track modal amplitude and residual noise separately. Use repeated takes to
estimate uncertainty. Close modes may create envelope minima that are not extra
damping. A narrow-band energy estimator, a modal fit, and a time-domain envelope
can disagree for valid reasons. Investigate those disagreements on synthetic
fixtures before choosing the most flattering estimator.

Include cross-string energy transfer. A target string's decay can speed up while
a sympathetic string gains energy; fitting independent exponential damping to
each can mistake coupling for dissipation. Compare total mechanical energy where
available and receiver-dependent acoustics separately. A quiet microphone tail
does not prove that internal state is at rest.

## Attack and release

Define onset from a measured excitation event when available. Otherwise register
an onset detector and its uncertainty. Report rise-time features, initial
spectral distribution, broadband noise, and transient duration. Avoid a single
peak-amplitude alignment because the peak can move when a body mode changes.
For fingernail release, the timing and direction of the release can matter as
much as eventual string displacement.

Separate note release, finger contact release, and MIDI note-off. The acoustic
instrument does not necessarily become silent when a keyboard key is released.
Define what the controller event means in the chosen playing mode before testing
its decay. Measure damping transitions and any audible discontinuity independently
of the ordinary free decay. The product mapping may intentionally differ from
the physical action, but the distinction belongs in the report.

Use controlled perturbations: shift pluck position, change direction, increase
input energy, damp a companion string, or alter contact duration. A credible
model should change in the correct direction across a validated range. A
candidate that matches one attack but responds backward to a stronger pluck
has failed a mechanistic test even if its average spectrum is good.

## Dynamic response and level policy

Evaluate two separate tasks. A level-matched timbre comparison asks whether
spectrotemporal shape differs after a declared gain adjustment. A calibrated
dynamic-response comparison asks whether physical input changes produce correct
output changes. Do not use the former as evidence for the latter.

Record the relation between measured or commanded input and output energy,
peak pressure, selected partial levels, and decay behavior. Test monotonicity only
where the model and experiment predict it. Stronger plucks may change contact
regimes; a simple globally monotonic spectral-centroid assertion can be wrong.
Document saturation or admissibility boundaries explicitly. A hidden output
limiter must not disguise an energy instability as a plausible dynamic response.

## Report design and acceptance

For each stimulus show the reference identity, candidate revision, observation,
conditioning, resolved features, uncertainty, and threshold revision. Include
distribution across repetitions and a worst-case table. Display failure cases,
not just median examples. A summary can highlight which mechanisms are consistent
with evidence and which remain ambiguous.

The packet passes its analysis-readiness gate when extractors recover known
synthetic cases, transformations are reproducible, uncertainty is visible, and
the result is robust to declared analysis choices. The candidate passes an
acoustic claim only when the registered empirical criterion is met on the
appropriate held-out data. Next work should target the largest residual with a
mechanism-specific intervention, not an arbitrary equalizer adjustment.
