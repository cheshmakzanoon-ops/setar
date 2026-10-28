# Identifiability, calibration, and differentiable-model pitfalls

A low fitting error does not prove that the fitted parameters represent the
instrument. Multiple combinations of tension, length, stiffness, contact,
excitation, body response, and observation gain can explain similar audio.
Calibration must therefore report what the experiment identifies, not merely
what an optimizer returned. This chapter specifies an offline workflow; none of
its optimization, matrix factorization, or file processing belongs in the audio
callback.

## Parameters, observations, and structural ambiguity

Start with a parameter table: symbol, SI unit, physical interpretation, admissible
range, source, uncertainty, and whether it is measured, fixed, fitted, or a
nuisance variable. State the observation operator and available inputs. A model
may be identifiable with measured bridge force and velocity but ambiguous from
one microphone waveform with unknown excitation and gain.

Look for exact or near-exact transformations leaving the observations unchanged.
For an ideal tension-dominated string, a frequency alone constrains combinations
of tension, line density, and length, not each independently. A microphone gain
and excitation amplitude can compensate each other. Adding more samples of the
same observation does not necessarily resolve such ambiguity. Additional kinds
of measurement may be needed.

Distinguish structural identifiability under idealized observations from
practical identifiability under finite noisy data. Profile-likelihood analysis
is one established tool for investigating parameter uncertainty; see Raue et al.,
*Bioinformatics* 25(15), 1923–1929,
[DOI: 10.1093/bioinformatics/btp358](https://doi.org/10.1093/bioinformatics/btp358).
The following setar workflow is a project-specific design, not an application
claimed to have been validated by that paper.

## Fix what is independently known

Use direct measurements to constrain geometry, string line density, tuning, and
receiver calibration when their uncertainty is adequate. Do not fix uncertain
quantities as exact merely to make fitting easier. Either propagate their
uncertainty or examine its effect in sensitivity runs. A narrow prior or tight
bound can create apparent precision that comes from the assumption rather than
the data.

Separate specimen parameters from session and take parameters. Geometry may be
shared across sessions; tuning and environmental state may not be. Excitation
position may vary per take. Letting every take have unrestricted body coefficients
will improve fits while destroying the claim that one body model explains the
instrument. Declare the sharing structure before optimization.

Use nuisance parameters sparingly and interpretably. A global timing offset or
calibrated gain may be justified. A freely warped time axis, per-partial arbitrary
phase, or per-note spectral correction can absorb model defects. Record which
degrees of freedom the measurement truly leaves unknown and which are merely
convenient numerical freedoms.

## Sensitivity and conditioning

Compute local sensitivities in a scaled parameterization. A Jacobian column for
meters cannot be compared naively with a column for pascals. Use physically
meaningful scales and report them. Inspect singular values and correlated
directions, but do not treat a single condition number as a complete global
identifiability verdict. The answer can change with operating point, stimulus,
noise model, and observation window.

Perturb parameters in both directions where admissible. Check that finite
differences resolve a region above numerical noise and below nonlinear curvature
effects. Repeat at multiple step sizes. If derivatives change wildly with the
step, determine whether contact topology, feature assignment, solver tolerance,
or numerical cancellation is responsible. A large derivative caused by a mode
label swap is not physical sensitivity.

Map weak directions back to understandable combinations. If stiffness and fret
compliance trade off, identify a measurement that excites them differently.
Sensitivity plots are useful only when they guide an intervention or bound a
claim. An attractive heatmap with unexplained scaling can mislead a reviewer.

## Objective design

Define the objective from observables and uncertainty, not from whichever features
are easiest to differentiate. Include separate attack, partial-frequency, decay,
coupling, and radiation terms where the experiment supports them. Keep essential
physical constraints explicit. Report component losses and failures instead of
only a weighted total.

An assumed Gaussian independent residual model is a modeling choice, not a
property of spectrogram bins. Adjacent windows and frequencies are correlated.
If a likelihood is used, justify its residual structure or use an analysis whose
uncertainty remains credible under the dependence. Robust losses can reduce the
effect of contamination, but can also hide systematically wrong high-energy
transients. Show which data they downweight.

Regularization encodes preference or prior information. Report the penalty,
strength selection procedure, and sensitivity. A smooth frequency-dependent loss
curve is often more interpretable than independent loss values for every mode,
but smoothness alone is not evidence that the curve describes material damping.
Do not call regularized parameter values “measured.”

## Optimization protocol

First recover parameters from synthetic data generated by a known model, with
noise and observation restrictions resembling the planned experiment. Use both
matched-model cases and deliberate model-mismatch cases. Recovery in an ideal
noise-free setting tests software; it does not establish recoverability in the
real experiment.

Run multiple defensible initializations and preserve all outcomes, including
failures. Compare solutions in parameter space and predicted observation space.
Different parameter vectors with nearly identical predictions indicate ambiguity,
not necessarily a bad optimizer. Check gradients and objective values at the
reported solution; “optimizer converged” can mean a stopping rule was met, not
that the global optimum or physical truth was found.

Perform profiles or conditional refits for important parameters. Hold one value
at several points and refit the others, then examine the objective change. Use
interval construction appropriate to the likelihood assumptions and sample
structure. When exact inferential assumptions are not defensible, present the
profile as a sensitivity diagnostic rather than a calibrated confidence interval.

## Differentiability is a hypothesis to test

Automatic differentiation differentiates executed operations; it does not certify
that the derivative corresponds to the intended physical problem. Contact onset,
release, active-set changes, hard constraints, integer mode selection, and
adaptive solver decisions can make the mapping nonsmooth or implementation-
dependent. A smooth surrogate can aid optimization while changing the contact
mechanism being calibrated.

Primary research on contact simulation documents important gradient difficulties:
Zhong et al., [*Improving Gradient Computation for Differentiable Physics
Simulation with Contacts*](https://proceedings.mlr.press/v211/zhong23a.html),
PMLR 211, 128–141, examines contact-gradient accuracy; Suh et al.,
[*Do Differentiable Simulators Give Better Policy Gradients?*](https://proceedings.mlr.press/v162/suh22b.html),
PMLR 162, 20668–20696, analyzes problems involving stiffness and discontinuity.
These are warnings to test our gradients, not evidence that their solutions
directly solve setar nail or fret contact.

Compare automatic derivatives with carefully scaled finite differences away from
events and with one-sided perturbations near events. Check the derivative of the
actual loss, including extraction and normalization. Test the prediction made by
a gradient step against an actual forward solve. When the prediction fails,
record the event topology and solver state rather than silently reducing the
learning rate forever.

If a surrogate contact law is fitted, evaluate the fitted parameters with the
production law and quantify the discrepancy. Do not transfer parameters across
different contact models as though their meaning were unchanged. Preserve a
derivative-free or coarse-search diagnostic for low-dimensional cases so an
incorrect derivative cannot become the sole judge of its own correctness.

## Held-out predictions and uncertainty propagation

Freeze model structure, feature weights, parameter-sharing rules, and selection
procedure before final held-out evaluation. Propagate parameter uncertainty to
predicted acoustics or gesture response. Poorly identified parameters can still
yield useful predictions in a narrow domain; conversely, tightly fitted parameters
can produce uncertain extrapolation. Report the prediction domain explicitly.

Use grouped resampling or a hierarchical analysis when repeated takes share a
session or specimen. Resampling individual spectrogram frames exaggerates the
effective information. Include uncertainty in excitation and observation where
it matters. A narrow numerical optimizer covariance matrix omitting those
uncertainties is not the complete experimental interval.

## Acceptance and handoff

The packet contains the parameter table, model/observation equations, fixed and
fitted quantities, split manifest, sensitivity diagnostics, synthetic recovery,
optimization histories, profile plots, held-out results, and unresolved ambiguity.
Acceptance means the selected claim is supported; it need not mean every
parameter is uniquely known. Mark nonidentified parameters honestly and avoid
exposing them as scientifically calibrated product controls.

The next action is either an informative new measurement, a reduced model with
identifiable combinations, or a narrower claim. Do not resolve nonidentifiability
by choosing the most visually plausible parameter vector without evidence.
