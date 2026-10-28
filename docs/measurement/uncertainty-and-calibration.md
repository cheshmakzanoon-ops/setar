# Uncertainty budgets and calibration decisions

## Uncertainty is part of the result

A reported quantity without a defensible uncertainty description cannot support
a precise comparison between model and instrument. The uncertainty need not
always be a single Gaussian standard deviation; it can include bounded,
asymmetric, correlated, or multimodal information. What matters is that its
meaning and origin are explicit and appropriate to the decision.

R-METROLOGY-JCGM-100 points to the official metrology framework. The project
procedures below are original applications to setar measurement. They do not
claim accredited calibration or compliance with every clause of a standard.
Laboratories should identify the exact edition and any applicable specialist
procedure they use.

## Start from the measurand

Define the quantity being estimated. “The pitch” is incomplete for a plucked
string with a time-varying trajectory. A more precise measurand might be the
fundamental-frequency estimate over a specified decay interval under a specified
excitation and finger state. “The body damping” is incomplete if support,
strings, radiation, and sensor loading are included but unacknowledged.

Write the measurement equation before assigning error bars. Let

\[
\mathbf q=g(\mathbf x),
\]

where `x` includes measured inputs, calibration factors, corrections, and
relevant nuisance variables. Identify which inputs are independent and which
share a calibration or physical cause. A common sample-clock error affects
multiple frequency estimates together; treating it as independent noise in
every estimate can exaggerate uncertainty in ratios while understating
uncertainty in absolute frequency.

## Build the input budget

For each input, record its estimate, unit, uncertainty model, evidence source,
sensitivity coefficient, correlation group, and validity domain. Separate
repeatability from calibration uncertainty, resolution, alignment, model
approximation, and environmental drift. Repeating a measurement reduces some
random components but does not eliminate a shared bias or unknown scale factor.

Type-A and Type-B evaluations describe how uncertainty is evaluated, not whether
an effect is random or systematic in a philosophical sense. Do not assume a
manufacturer tolerance is one standard deviation. Interpret the specification,
distribution, and coverage only when supported. If the specification is ambiguous,
record a conservative bound or seek clarification.

Correct known biases when a justified correction exists, and retain uncertainty
in that correction. Do not leave a known sign error in place and cover it with
a larger error bar. Conversely, do not subtract an uncertain “bias” inferred
from the same model being validated without disclosing the circularity.

## Linear propagation

For sufficiently smooth behavior near the estimate,

\[
\Sigma_q\approx J\Sigma_xJ^T,
\qquad J_{ij}=\frac{\partial g_i}{\partial x_j}.
\]

This approximation needs checking when nonlinearity, bounds, low signal-to-noise
ratios, or discontinuous event detection matter. Report covariance or a useful
reduced representation for vector quantities. Independent scalar error bars
cannot describe a strongly correlated FRF or geometry field adequately.

For the ideal tension estimate `T=4*mu*L^2*f^2`, use the gradient
`[T/mu, 2*T/L, 2*T/f]` with the full covariance of `mu`, `L`, and `f`. This
makes explicit that length and frequency uncertainty enter differently from
linear-density uncertainty. It also exposes correlations if the same geometric
measurement contributes to both length and a frequency correction.

For an interval `c=1200*log2(f2/f1)`, the gradient is
`[-1200/(ln(2)*f1), 1200/(ln(2)*f2)]`. A shared multiplicative frequency-scale
error cancels in an ideal simultaneous ratio, while independent estimator error
does not. If the notes were recorded on different clocks or under different
conditions, that cancellation may not apply.

## Nonlinear propagation

Use Monte Carlo or another suitable method when linear propagation is inadequate.
Draw from justified joint input distributions, preserving correlations and
physical constraints. Record the generator, seed, sample count, convergence
check, and handling of invalid draws. Sampling independent normal distributions
for every elastic constant can generate nonphysical tensors and misrepresent
the intended uncertainty.

If a parameterization enforces positive definiteness or positivity, define the
distribution in that parameterization and explain its interpretation. Rejecting
invalid draws from a different distribution changes the distribution; do not
hide that conditioning. For bounded geometric or timing intervals, use a model
appropriate to what is known rather than reflexively choosing a Gaussian.

Compare Monte Carlo and linear results on simple cases as an implementation
check. Investigate discrepancies before trusting either. A huge number of draws
does not repair a wrong measurement equation or incorrect covariance.

## Complex transfer uncertainty

Represent a complex FRF through real and imaginary components or another
well-defined coordinate system. Magnitude and phase become poorly behaved near
zeros, where phase can be effectively unconstrained. A small magnitude with a
large phase uncertainty is not necessarily a bad experiment; it may be the
expected behavior near an antiresonance with limited signal-to-noise ratio.

Separate trial-to-trial variability from systematic gain and timing uncertainty.
An unknown relative delay produces correlated phase error across frequency.
A force sensitivity error scales all frequencies together. Treating each bin as
independent can make a fit appear far more informative than the actual
calibration supports.

Estimate uncertainty from repeated acquisitions at meaningful levels: repeated
impacts, repositioned sensors, remounted instruments, and separate sessions. Each
level answers a different question. A narrow confidence band from hundreds of
averages on one unchanged setup does not include remounting uncertainty.

## Geometry uncertainty

Geometry errors are spatially structured. Scale calibration affects many
dimensions together. Registration can introduce coherent rotation or translation
error. Surface fitting suppresses small-scale detail. Inaccessible thickness
regions require inference. Build perturbations that reflect these mechanisms
rather than adding independent random noise to every mesh vertex.

Propagate important geometry alternatives through the forward model and compare
their effect with material and boundary uncertainties. If geometry dominates,
additional optimization of elastic constants may merely compensate for the wrong
shape. The uncertainty report should guide the next measurement, not just decorate
the final parameter table.

## Temporal and annotation uncertainty

For contact events, include frame exposure, frame timing, synchronization,
occlusion, and annotation disagreement. An event between frames is an interval.
Sub-frame interpolation requires a validated signal model and should be tested
on synthetic or independently timed events. Do not report microsecond precision
from a camera that only resolves a much coarser interval.

For audio onset alignment, distinguish physical release from acoustic arrival
and estimator threshold crossing. Each has different meaning. If the model is
shifted to match every reference onset independently, report that alignment as
a removed degree of freedom; it cannot then be used to claim accurate release
timing.

## Repeatability, reproducibility, and variation

Design nested repeats to separate within-trial estimator variation, repeated
gesture variation, remounting, session, player, and specimen effects. Do not
collapse all variation into one standard deviation. Some variation is the
musical behavior the model should reproduce; some is measurement uncertainty;
some is uncontrolled state drift that should be reduced.

Use a statistical model consistent with the sampling unit. Many notes from one
player are not many independent players. A confidence interval computed as if
all notes were independent can be misleadingly narrow. Resampling and mixed-
effects approaches must respect the grouping used by the experiment.

## Acceptance decisions

Define tolerances from the intended physical or perceptual claim before examining
the final test results. A measurement uncertainty larger than the tolerance may
make the decision inconclusive. Do not call that a pass merely because the
central estimate lies close to the target. Distinguish pass, fail, and
insufficient-evidence outcomes.

When reporting an expanded interval, state the coverage factor and its intended
interpretation. `k=2` does not universally guarantee an exact 95 percent interval;
distribution, effective degrees of freedom, and method matter. Use appropriate
coverage calculations or state the approximation. Avoid false precision in both
the estimate and uncertainty.

Model discrepancy must remain distinct from measurement uncertainty. Inflating
the measurement error until every prediction fits hides model failure. If the
model omits a physical mechanism, report that discrepancy and its scope. A wider
honest prediction interval may be appropriate, but it does not turn the omitted
mechanism into a calibrated observation.

## Calibration maintenance and triggers

Recheck the chain after changes that can alter its response: sensor replacement,
gain changes, cable or conditioner changes, overload, physical shock, software
scaling changes, or suspicious drift. A calendar certificate is useful but does
not guarantee the current configuration is correct. Conversely, an expired
document does not quantify the actual error by itself; follow the laboratory's
approved validity policy and restrict claims when necessary.

Keep before/after checks with the acquisition packet. If they disagree beyond
the declared criterion, identify the affected data interval and reassess its
validity. Do not average the checks and assume the chain was stable between them.
The cause may be an abrupt change rather than smooth drift.

## Deliverable

The uncertainty packet contains the measurand definition, measurement equation,
input budget, covariance assumptions, propagation method, sensitivity ranking,
coverage interpretation, and decision rule. It also identifies the most useful
next calibration or acquisition improvement. The gate passes when a reviewer can
recompute the interval and understand what it excludes. It fails when error bars
are chosen after seeing the desired result or when missing knowledge is hidden
inside an unexplained percentage.

