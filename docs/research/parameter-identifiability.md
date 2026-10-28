# Parameter identifiability and inverse-model discipline

## What a fitted number means

A parameter is identifiable when the available observations distinguish its value
from alternatives within the model and experimental domain. A low residual is not
enough. Multiple combinations of thickness, stiffness, density, boundary compliance,
and excitation amplitude may produce nearly identical recorded signals. An
optimizer can return one precise-looking point from that family without measuring
any of its components precisely.

This chapter specifies an original inverse-analysis protocol. It uses the source
ledger to motivate the relevant physics, but it does not claim that the cited
papers supplied the project's parameter distributions. Until project measurements
exist, examples must remain synthetic and must not be exported as a reference
instrument preset.

## Write the observation model

Use an explicit mapping

\[
\mathbf y = h(\boldsymbol\theta,\mathbf u,\mathbf z)+
\boldsymbol\delta+\boldsymbol\varepsilon.
\]

Here `theta` contains physical or reduced-model parameters, `u` describes controlled
excitation, `z` contains observed nuisance state, `delta` represents model
discrepancy, and `epsilon` represents measurement error. Do not hide microphone
gain, string detuning, or fixture motion in a generic optimizer noise term if they
can be independently measured. Conversely, do not pretend that a residual
distribution captures missing physics simply because it is convenient to fit.

For frequency-response fitting, decide whether the residual operates on complex
responses, magnitude and phase, or a derived feature vector. Complex residuals
preserve phase but need an acquisition and timing chain that makes phase meaningful.
Magnitude-only fits can match resonances while missing sign and coupling. Log
magnitude emphasizes relative errors and requires a noise-floor policy. Every
choice changes what the fit is allowed to ignore.

Record how the data are weighted. Weighting by inverse estimated covariance can
be appropriate when that covariance is credible. Weighting every frequency bin
equally can let dense oversampling of one broad feature dominate a sparse but
important mode. Weighting by perceptual importance is a product objective, not
physical measurement uncertainty; keep those weighting systems distinct.

## Scale parameters and inspect sensitivities

Let `J_ij` be the derivative of observation `i` with respect to parameter `j` at a
candidate point. Work with scaled coordinates so that a metre, a gigapascal, and
a dimensionless damping ratio are not compared by their raw numerical magnitudes.
For positive parameters, logarithmic coordinates often make local fractional
changes easier to interpret. For bounded angular variables, choose a representation
that does not create artificial discontinuities at a wrapping boundary.

Estimate derivatives using verified analytic expressions, automatic
differentiation, or converged finite differences. A finite-difference derivative
must be checked over several perturbation sizes. Too large a perturbation probes
nonlinearity; too small a perturbation measures solver noise. Mesh adaptation,
mode-order switching, and contact-state changes can make a nominal derivative
discontinuous. Report those cases rather than smoothing them silently.

Inspect singular values and right singular vectors of a weighted sensitivity
matrix. A small singular value identifies a weakly observed parameter combination,
not necessarily a single bad parameter. Plot or tabulate the combinations in
physical language. “Thickness and longitudinal stiffness trade off in these
modes” is more actionable than “condition number is large.” Verify whether another
excitation port, a static mass measurement, or a thickness observation breaks the
confounding.

## Common setar confounders

For a simplified tensioned string, fundamental frequency scales approximately as
`sqrt(T/mu)/L`. One measured frequency cannot independently identify tension,
linear density, and length. If length and linear density are independently known,
the same observation can constrain tension under the assumed boundary model.
Adding partial-frequency structure and amplitude dependence may constrain bending
stiffness or geometric nonlinearity, but only if termination effects and estimator
bias are also addressed.

For bending-dominated plate behavior, thickness and elastic modulus both influence
stiffness, while thickness also affects mass. Measuring only resonance frequencies
can leave a family of compensating combinations. Spatial mode shapes, total mass,
local thickness, and port residues provide additional information. They do not
automatically make every orthotropic constant identifiable, particularly if all
observations excite similar deformation patterns.

Body damping, player contact, and radiation loss can all shorten a measured decay.
A microphone envelope alone usually does not assign the energy loss to one
mechanism. Compare mechanical and acoustic observations, support conditions, and
frequency dependence. Treat the inferred effective damping as an effective
quantity unless the mechanism decomposition is independently demonstrated.

Nail stiffness, contact duration, approach speed, and initial string displacement
can trade off in an onset spectrum. A fitted contact law should predict contact
timing and string motion as well as sound. If only acoustic data are available,
report the law as one physically admissible explanation, not a direct measurement
of the player's nail properties.

## Separate fixed, estimated, and nuisance quantities

Divide inputs into independently measured quantities, fitted physical parameters,
fitted effective parameters, and nuisance variables. The partition is a scientific
decision. A known geometry uncertainty should not become a fixed exact mesh merely
to make the optimizer smaller. A poorly measured nuisance gain should not be
unbounded if it can absorb the amplitude information needed to identify a residue.

Where practical, fit nuisance parameters jointly with their calibration priors or
profile over them. Report the effect of fixing them versus allowing their measured
uncertainty. If conclusions change substantially, the calibration chain needs
improvement before the physical interpretation is strong.

Use hierarchical models when specimens, players, or sessions vary. A common
parameter distribution can represent population structure while specimen-level
terms represent actual differences. Do not average all instruments into an
unphysical “typical” setar and then claim every specimen should match it. Keep the
reference-instrument claim distinct from a broader family-of-instruments claim.

## Constraints and priors

Enforce required physical properties such as positive mass, positive length,
admissible elasticity, stable poles, and passive mechanical coupling. These are
not optional preferences to be traded against a small residual improvement.
Parameterizations that enforce constraints by construction reduce accidental
invalid candidates, but they do not excuse a missing identifiability analysis.

Distinguish a hard safety or physical constraint from a prior belief. A literature
range can be a starting prior only if its specimen and method are relevant. An
uncertain published table that fails reciprocity is not an informative material
prior. Use the quarantine policy in the parameter-audit chapter.

Run prior-sensitivity analysis. If the posterior or fitted interval merely mirrors
the chosen prior, say that the data are weakly informative for that parameter.
That can still be useful for synthesis, but it is not a new material measurement.
If a regularization term favors smooth spatial thickness, report how much small
geometric detail is suppressed and whether held-out responses reveal the loss.

## Fit procedure

First freeze the observation selection, parameter definitions, bounds, unit
conventions, and loss function. Validate the forward solver at the required
resolution. Then run a synthetic recovery test with known parameters and realistic
noise and missing channels. Check that the analysis can recover the quantities it
claims to estimate. A failure here is an analysis failure, not an instrument
mystery.

Use multiple starting points or a suitable global exploration to expose distinct
solutions. Store failed runs and boundary-hitting solutions; do not keep only the
most favorable seed. Inspect residual structure by frequency, excitation port,
location, amplitude, and session. Random-looking aggregate error can conceal a
systematic directional mismatch.

After local fitting, profile key parameters or use a defensible uncertainty
method. A covariance from one local Hessian can be misleading near boundaries,
with multimodal solutions, or with strongly nonlinear contacts. State the method
and test its numerical convergence. If a confidence region is unbounded within
the admissible domain, report that instead of truncating the plot to a pleasing
interval.

Finally, predict held-out observations without refitting. An alternate excitation
port, support condition, tuning, or specimen is more informative than another
random frequency bin from the same already-fitted FRF. Any model change triggered
by the test turns that data into development data and requires a new independent
test for the next claim.

## Model discrepancy and escalation

When every admissible candidate misses the same feature, investigate missing
physics before increasing optimizer effort. Possible explanations include a
neglected polarization, nonideal joint, string afterlength, contact nonlinearity,
unmodeled support, or acquisition phase error. Design a discriminating experiment
between these explanations. Do not simply add a narrow equalization notch at the
failure frequency and call the mechanism solved.

An empirical residual correction may be permitted as a labeled model component
if its causality, stability, passivity where relevant, rights, and generalization
are established. Its use narrows first-principles claims. Keep the correction
separate in exports so future evidence can replace it without obscuring the
underlying model.

## Deliverables and gate

The inverse-model handoff includes the forward-model version, all parameter
definitions and units, calibration dataset identifiers, exclusions, objective,
constraint implementation, sensitivity diagnostics, multiple-solution search,
uncertainty method, residual analysis, and held-out prediction report. Include
parameter correlations and a list of quantities not identified by the data.

The gate fails if a single optimizer success code is the main evidence, if
unreported gain adjustments absorb physical discrepancies, if test data were used
for selection, or if fitted effective parameters are described as direct material
measurements. It passes for a scoped claim when the data constrain the required
prediction and unresolved parameter ambiguity is honestly represented. A usable
physical synthesizer can have uncertain microscopic parameters; it cannot have
an unexamined evidence chain.
