# Body 09 — Structural identifiability and discriminating experiments

## Purpose and evidence status

This proposed, unimplemented P07/P08/P12 chapter asks which physical parameters the available body measurements can actually determine. Inputs are a verified forward model, parameter provenance, calibration uncertainty, and candidate measurements. Outputs are identifiable parameter combinations, unresolved alternatives, experiment priorities, and a bounded calibration claim.

Optimization is not identification. A numerical optimizer can return many decimal places for parameters that the data barely constrain. The purpose of this chapter is to prevent geometry, material, joint, loading, and observation errors from compensating one another invisibly.

The scope is the body and its acoustic interfaces. It complements the repository’s general measurement and validation volumes by deriving structural-specific ambiguity tests. No proposed measurement here authorizes disassembly, new equipment purchase, or modification of an instrument.

## Parameters versus hypotheses

Separate continuous parameters from model hypotheses. Soundboard thickness, an elastic modulus, or a joint stiffness can vary continuously within one model. A bonded versus slipping bridge, sealed versus open cavity, or shell versus solid representation changes the governing assumptions.

Do not force a continuous optimizer to compensate for a wrong hypothesis. A very soft wood modulus might imitate a missing joint compliance over a small band while failing elsewhere. A huge acoustic loss might hide an incorrectly sealed opening.

Maintain a hypothesis table with the evidence supporting each choice, the observations it affects, and the tests that could distinguish alternatives. A hypothesis can remain unresolved without preventing synthetic numerical development. It does prevent an unsupported specimen-level conclusion.

## An explicit thickness–modulus ambiguity

For a simple thin plate with fixed planform, fixed Poisson structure, and no membrane preload, bending stiffness scales as \(Eh^3\) while areal mass scales as \(\rho h\). A characteristic bending frequency therefore scales as

\[
\omega\propto h\sqrt{E/\rho}.
\]

Its logarithmic differential is

\[
\delta\log\omega=\delta\log h+
\tfrac12\delta\log E-\tfrac12\delta\log\rho.
\]

Mass supplies another relation, \(\delta\log m=\delta\log\rho+\delta\log h\). Yet these two observables still cannot determine all three quantities independently. The perturbation

\[
\delta\log h=t,\quad
\delta\log\rho=-t,\quad
\delta\log E=-3t
\]

leaves both bending stiffness and areal mass unchanged to first order. In the idealized plate model it preserves much more than one frequency.

This is not a setar parameter estimate. It is a synthetic identifiability counterexample. It demonstrates why matching several bending frequencies and total mass may still fail to determine thickness, density, and modulus uniquely. Independent thickness or density evidence is needed.

## Geometry–joint ambiguity

A flexible rim can lower soundboard frequencies in a manner resembling reduced plate stiffness. Neck-joint compliance can alter modes that might otherwise be attributed to neck modulus or attachment geometry. Bridge position changes modal residues and can imitate changes in mode shape.

Use spatial response and multiple force directions to distinguish these mechanisms. A modulus change affects strain energy throughout a region; a localized joint change affects modes according to their interface deformation. Their frequency shifts may be similar for one mode and different for another.

Do not claim that a sensitivity ranking is universal. It depends on the current hypothesis, mode, frequency, support, and observation. Recompute sensitivities when the nominal model changes substantially.

## Eigenvalue sensitivities

For a simple eigenvalue with \(\phi^TM\phi=1\),

\[
\frac{\partial\lambda}{\partial\theta}
=\phi^T\left(
\frac{\partial K}{\partial\theta}
-\lambda\frac{\partial M}{\partial\theta}
\right)\phi.
\]

This equation separates stiffness and inertia effects. It also gives a useful independent check on finite differences. For positive \(\lambda\),
\(\partial\log\omega/\partial\theta=
\tfrac12\lambda^{-1}\partial\lambda/\partial\theta\).

The formula assumes a differentiable model and an isolated eigenvalue. At repeated modes, use a projected perturbation problem within the eigenspace. Near crossings, track subspaces rather than subtracting frequency ranks. Otherwise a finite-difference derivative can measure a mode-label switch rather than physical sensitivity.

Geometry parameters change both matrices and possibly the port map. Contact active-set changes can make derivatives nonsmooth. Detect such transitions and treat them as separate local regimes.

## FRF sensitivities

Let \(D(\omega)=K-\omega^2M+i\omega C\) and \(H=B^TD^{-1}B\) for a receptance port matrix. Its derivative includes

\[
\partial H=
(\partial B)^TD^{-1}B+
B^TD^{-1}(\partial B)
-B^TD^{-1}(\partial D)D^{-1}B.
\]

The port derivatives matter when bridge location, footprint, or direction changes. Ignoring them can falsely attribute residue changes to stiffness alone.

Compute derivatives through linear solves rather than explicitly forming a dense inverse. Verify selected derivatives by central differences at multiple step sizes. Choose perturbations above solver noise and below the scale where the local linear approximation fails.

Near a resonance, sensitivity can be large and strongly correlated with damping and frequency shift. Use complex response and realistic measurement covariance; a magnitude-only derivative can behave badly near zeros.

## Observation and calibration nuisance parameters

A force calibration gain and a velocity calibration gain scale measured mobility by their ratio. If those gains are unknown, absolute modal scaling may be confounded with the mechanical model.

Timing error produces a frequency-dependent phase factor. Location error changes modal participation and propagation phase. Microphone orientation changes sensitivity. Include these as nuisance parameters with independent calibration information rather than forcing the body model to absorb them.

Do not let an unrestricted observation gain rescue every model. A global gain may be appropriate for a listening comparison, but it removes evidence about absolute radiation efficiency. Preserve both the calibrated and gain-adjusted comparisons.

Room uncertainty belongs in the observation model. A body parameter that changes to fit a room reflection should not be exported as a wood property.

## Sensitivity matrix and rank

Construct an observation vector from calibrated real and imaginary FRF components, modal quantities, mass, geometry, and other independent measurements. Scale each component using its uncertainty or a justified engineering weight.

The Jacobian \(J\) maps small parameter changes to observation changes. Singular values identify weakly observed parameter combinations. Right singular vectors reveal compensations such as thickness versus modulus or joint stiffness versus support compliance.

Rank is numerical and uncertainty-dependent. A tiny nonzero singular value does not establish practical identifiability. Compare predicted changes along that direction with measurement noise, calibration uncertainty, and forward-model discrepancy.

Report the scaling used for parameters. A Jacobian mixing Pa, meters, and dimensionless ratios can look ill-conditioned solely because of units. Log parameters or normalized perturbations can help, but their interpretation must be stated.

## Correlated observations

Frequency bins from one windowed record are not necessarily independent. Channels can share calibration errors. Modes extracted from the same FRFs can have correlated uncertainty. Treating every bin and every derived mode as independent evidence can create false confidence.

Use a covariance model appropriate to the data. If a full covariance is impractical, document the approximation and test its impact. Do not count both raw FRFs and modal parameters extracted from them as independent likelihood contributions without accounting for their relationship.

Model discrepancy is not just extra white noise. A missing physical mechanism can produce coherent frequency- and direction-dependent residuals. Inspect residual structure before interpreting narrow statistical intervals.

## Experiment selection by discrimination

Choose the next measurement to separate the most consequential unresolved hypotheses. If density and thickness are confounded, another similar resonance may add little information; an independent thickness observation or mass distribution measurement may be more valuable.

If bridge lateral coupling is unknown, excite and observe a lateral direction rather than repeating normal excitation. If support effects dominate, compare a controlled support change. If cavity geometry is uncertain, pressure or aperture-flow information may discriminate better than another structural accelerometer.

Every proposed perturbation must be safe and authorized. A useful numerical experiment does not imply the physical instrument can be altered freely. Use reversible noncontact observations where they answer the question.

Prioritize by expected information gain and consequence of error, not by ease of producing an impressive plot. A modest calibration check can be more important than a dense new scan.

## Force and sensor placement

Place force and response channels to observe competing mode families and port directions. Avoid relying entirely on points near predicted nodes. Include locations whose behavior differs between plausible hypotheses.

An FE-based sensor design can be biased toward the current model. Reserve additional validation locations or alternative designs to detect missing modes. Evaluate design robustness over the uncertainty ensemble, not just one nominal mesh.

Sensor mass and attachment can change the system. Include their influence in candidate designs. A theoretically optimal location is not useful if the required sensor significantly perturbs a light soundboard or risks finish damage.

For laser observation, include beam direction and reflectivity constraints. A large displacement component orthogonal to the beam is not observed merely because the point is well chosen geometrically.

## Sequential calibration without circular validation

Calibrate geometry and mass-related quantities before using modal data to fit stiffness where practical. Calibrate channel gains and phase before interpreting residues. Establish support effects before transferring free-free modes to a played state.

This ordering reduces compensation, but it is not a rigid rule that ignores coupled uncertainty. Carry uncertainty forward rather than freezing early estimates as exact.

Reserve held-out observations before tuning. Do not repeatedly inspect them and adjust the model while continuing to call them held out. If they become part of calibration, acquire or designate a new independent validation set.

The final validation question is predictive: does the model explain a condition not used to choose its parameters? Reproducing the training FRF is necessary but insufficient.

## Profile and ensemble diagnostics

Profile a consequential parameter by fixing it over a range and reoptimizing the others. A flat objective reveals compensation that a local covariance estimate can miss. Record how other parameters move along the profile.

Use ensembles of admissible material and geometry hypotheses to propagate uncertainty into bridge mobility, decay, and radiation. Reject invalid tensors or geometry explicitly and report the rejection mechanism.

Do not equate the optimizer’s starting-point spread with a posterior distribution. Algorithmic variability and physical uncertainty are different. If a probabilistic interpretation is claimed, specify priors, likelihood, and computational diagnostics.

A deterministic bounded set may be more honest when probability distributions are poorly supported. It can still produce useful worst-case response bands and experiment priorities.

## Synthetic recovery tests

Before fitting a specimen, generate data from a known synthetic model and recover its parameters using an independent discretization or richer reference. Add controlled random and systematic errors.

Include an intentionally unidentifiable case such as the plate scaling example. The identification pipeline should report ambiguity rather than return a falsely precise unique result.

Include a wrong-hypothesis case, such as fitting a bonded bridge to synthetic slipping-contact data. The pipeline should expose structured residuals and failed held-out predictions, not merely drive material parameters to extremes.

Include missing-mode and truncated-band cases to test residual flexibility. These fixtures verify the identification workflow itself and can be reproduced without instrument access.

## Acceptance evidence

### Worked synthetic ambiguity fixture

Create a rectangular Kirchhoff–Love plate fixture with a fixed planform, fixed Poisson ratio, and unchanged boundary conditions. This is intentionally not a setar soundboard. Let one candidate use thickness 0.002 m, density 500 kg/m\(^3\), and modulus \(8\times10^9\) Pa. Let a second use thickness multiplied by 1.1, density divided by 1.1, and modulus divided by \(1.1^3\).

The second thickness is 0.0022 m, density approximately 454.545 kg/m\(^3\), and modulus approximately \(6.01052\times10^9\) Pa. Both candidates have areal mass 1 kg/m\(^2\) and the same product \(Eh^3=64\) N·m. Under this deliberately restricted plate model, their bending matrices and mass matrices are identical when represented on the same midsurface.

The point of the fixture is not the numerical values. It is that even noiseless bending mobility and a perfect optimizer cannot distinguish these candidates within the assumed model. Any pipeline that reports one unique thickness, density, and modulus from those observations alone is expressing an unstated prior or numerical preference.

Run the two forward models through the same observation map and confirm equality to the tolerance expected from input rounding and assembly precision. Then add one independent thickness observation. The ambiguity should collapse in the corresponding direction, while uncertainty from calibration and the remaining model still persists.

Now deliberately add a physical mechanism that violates the idealized equivalence, such as transverse shear in a sufficiently thick reference formulation, independently measured three-dimensional geometry, or a through-thickness observation. Quantify whether its predicted difference exceeds actual measurement and discretization uncertainty. A theoretically nonzero distinction can remain practically unidentifiable.

For an orthotropic extension, scale all relevant in-plane stiffness coefficients by the same inverse-cubic factor while preserving their ratios and scale density inversely with thickness. Keep shear and rotary-inertia assumptions explicit. Do not generalize the exact thin-plate equivalence to a full three-dimensional wood solid.

### Predeclared decision table

Before fitting data, create a decision table with four columns: competing explanations, discriminating observation, expected effect, and rejection condition. For a thickness/modulus ambiguity, the observation might be a permitted independent thickness map. For support/joint ambiguity, it might be a controlled support change with unchanged joint geometry. For radiation/material-loss ambiguity, it might be simultaneous mechanical and acoustic power measurements or a carefully bounded acoustic-condition comparison.

Write rejection conditions in observable terms. “The optimizer converges” is not a rejection condition. A useful condition might state that the predicted cross-mobility phase and magnitude must remain within a declared uncertainty envelope at withheld ports over a specified band. The envelope comes from calibration, repeatability, model discrepancy, and numerical convergence; it is not selected after inspecting the candidate’s worst error.

Include an inconclusive outcome. If both explanations fit within uncertainty, the experiment has not distinguished them. Retain both or simplify to an effective parameter that the evidence supports. Forcing a winner makes the final asset appear more certain than the experiment.

Record which conditions are safe and achievable before recommending a physical perturbation. An ideal mathematical experiment involving removal of the soundboard is not an actionable next step on an intact valuable instrument without explicit permission and a qualified handling plan. Numerical experiments can explore that hypothesis while physical work remains noninvasive.

### Stopping an overparameterized fit

Inspect parameters repeatedly reaching bounds, strongly correlated posterior or profile directions, and large changes between equally good fits. These are reasons to reconsider parameterization, not automatically to widen every bound. A wider range can admit unphysical tensors or geometries while leaving the inference unresolved.

Replace unidentifiable groups with effective quantities when the application permits it, and state the transfer limitation. An effective rim compliance calibrated for one assembly may support that specimen’s runtime model while remaining unsuitable as a glue-material database entry. This is an honest useful result, not a failed attempt at microscopic identification.

The calibration gate requires a parameter provenance table, admissibility checks, sensitivity/rank analysis, nuisance-parameter treatment, held-out predictions, and unresolved alternatives. A low training error alone cannot pass.

Report which quantities are directly measured, inferred, effective, prior-dominated, or unidentifiable. If only a combined stiffness is supported, export that combined quantity rather than a fictitious decomposition.

Primary uncertainty reading is JCGM 100, Chapters 4–6 and 8: https://www.bipm.org/en/doi/10.59161/jcgm100-2008e. It supports disciplined uncertainty reporting; the structural derivatives and counterexamples above are explicit project derivations to be tested.

The next action is selected from the largest consequential uncertainty. It may be another measurement, a better boundary model, or a simpler parameterization. It is not automatically a larger optimizer or a more elaborate mesh.
