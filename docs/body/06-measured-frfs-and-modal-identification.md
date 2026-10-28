# Body 06 — Measured FRFs, residues, and physical identification

## Objective and evidence status

This proposed P08/P12 procedure identifies structural response from calibrated force and motion. No measured FRF or fitted setar asset is supplied here. Inputs are specimen state, calibrated channels, raw records, geometry, and experiment design. Outputs are complex FRFs, uncertainty, common poles and residues, physical scaling, reciprocal/passive checks, and held-out predictions.

The task is not to place resonators at spectral peaks. A microphone spectrum combines excitation, structure, radiation, microphone response, and room propagation. Even force-to-acceleration data need calibration, polarity, leakage control, and sensor-loading assessment before they determine mechanical parameters.

## Identify the transfer quantity

Distinguish receptance \(X/F\), mobility \(V/F\), and accelerance \(A/F\). With time convention \(e^{i\omega t}\),

\[
Y=i\omega H_x,\qquad H_a=-\omega^2H_x.
\]

Conversions amplify different noise regions and become ill-conditioned near zero. Do not integrate noisy acceleration and call the result reliable static compliance.

Specify peak versus RMS phasors. Ratios are unchanged when both channels use the same convention, but power calculations differ. Record sensor axes and polarity in the specimen frame. An FRF row is an output and a column an input; preserve this orientation through fitting and export.

## Acquisition and estimator quality

Choose excitation with useful energy over the target band without damaging the specimen or leaving the intended linear regime. Hammer tip, direction, and contact duration affect bandwidth. Shaker tests introduce attachment and force-path questions. Neither instrument is assumed available.

Define rejection rules for double hits, overload, clipping, cable impact, and support movement. Preserve rejected records and reasons. Averaging bad records does not repair them.

Use coherence, repeatability, force spectrum, and noise floor together. High coherence does not prove calibration or linearity. A systematic gain error can have perfect coherence. Low coherence can arise from noise, nonlinearity, leakage, or unmeasured input.

Roving sensors change mass distribution; roving excitation changes location and direction. Select a strategy according to fragility, access, sensor mass, and required observability. Record each physical state rather than pretending the entire campaign is one unchanged configuration.

Document estimator assumptions. H1-type and H2-type estimates respond differently to input and output noise. Compare them when the noise allocation is uncertain. Do not treat an estimator name as a quality certificate.

Force windows and exponential response windows can bias damping. If an exponential window adds decay, preserve both the raw estimate and the justified correction. Zero padding improves display interpolation, not physical frequency resolution. Report record length and effective window bandwidth.

## Modal residues and physical scaling

For a mass-normalized lightly damped reciprocal model,

\[
H_x(\omega)=
\sum_j\frac{b_jb_j^T}
{\omega_j^2-\omega^2+2i\zeta_j\omega_j\omega}
+H_{\mathrm{res}}(\omega).
\]

Here \(b_j\) is the modal participation across mechanical ports. With different input and output sets, the numerator is an output vector times an input vector transpose. Absolute scale is physical. A unit-height shape plot cannot replace it.

For one real isolated mode and collocated reciprocal ports, the receptance residue matrix is symmetric and rank one under these assumptions. Overlap, complex damping, incomplete observation, and residual terms complicate the structure. Use it as an uncertainty-aware consistency test rather than forcing every noisy matrix into rank one.

Cross-residue signs matter. Models with identical driving-point magnitudes and frequencies can predict opposite cross-string motion. Preserve phase and calibrated polarity.

Operational shapes obtained without known force generally lack absolute modal scale. Calibrated force response, a justified added-mass experiment, or an independently trusted FE mass model can supply more information. Each method has assumptions.

If unit modal mass is imposed, port coefficients carry the scale. Do not assign another arbitrary modal mass and scale coupling twice. Maintain a normalization ledger mapping every fitted coefficient to physical coordinates.

## Fit common physics, not unrelated channels

Fit complex FRFs jointly using a common pole set. Independent channel fits can create incompatible poles and an impossible multiport system. Weight data using uncertainty and the intended observable, not only relative magnitude.

Include physically meaningful residual flexibility or inertia for out-of-band modes. An unrestricted polynomial residual can absorb boundary mistakes and calibration errors. State asymptotic behavior and validity limits.

Assess stability across model order, fitting window, data subsets, and initialization. A pole appearing only at one order is not established. Plot complex-plane residuals and phase as well as magnitude.

Hold out entire channels, excitation directions, or specimen states where possible. Withholding scattered frequency bins adjacent to training data is a weak test of structural prediction. A held-out cross-mobility is more discriminating.

Estimate uncertainty from repeats and calibration. Bootstrap resampling characterizes some random variability but does not include systematic gain, phase, location, or support errors automatically. Report those separately.

## Reciprocity and passivity

Exchange excitation and response at the same physical ports and compare after accounting for axes and units. Report asymmetry and uncertainty before imposing symmetry. A mismatch may reveal calibration, state drift, nonlinearity, or incorrect coordinates.

Passivity is a matrix property. Check the Hermitian part of fitted mobility and use analytic assessment appropriate to the realization. Stable poles alone do not guarantee passive loading. Positive diagonal real parts do not exclude an active simultaneous-force direction.

A negative measured real driving-point mobility can arise from phase error, noncollocation, noise, or nonstationary behavior. Investigate rather than clipping it. If enforcing a passive model modifies the fit, report the correction and its effect on held-out prediction.

SINTEF’s primary Vector Fitting documentation separates stable-pole fitting from passivity assessment: https://www.sintef.no/en/software/vector-fitting/passivity/. Its examples do not remove the requirement to define mechanical power variables correctly.

## Amplitude and environmental dependence

Repeat measurements across safe levels. Compare poles, residues, harmonics, and coherence. Changes may come from bridge microslip, joints, geometry, or instrumentation. They do not uniquely identify nonlinear wood.

Do not pool all levels into a single FRF and claim validity for all gestures. Define the small-signal domain. If body response is effectively linear while string/contact response is nonlinear, preserve that useful separation rather than adding unnecessary nonlinear body states.

Record temperature, humidity, tuning, support, and physical settling condition. Recheck configuration before attributing a shifted resonance to a model defect. A small bridge movement can change residues without a substantial frequency shift.

## Identification procedure

1. Freeze specimen state and channel geometry.
2. Calibrate force, motion, and polarity.
3. Acquire repeated records with rejection metadata.
4. Compute FRFs, spectra, coherence, and uncertainty.
5. Fit common poles across channels.
6. Audit residues, normalization, and residual terms.
7. Evaluate reciprocity and passivity.
8. Validate held-out channels and states.
9. Compare FE modes through the actual sensor map.
10. Export model, uncertainties, and validity limits.

The Sandia Rattlesnake modal-testing documentation is a primary software reference for channel metadata and reciprocity inspection: https://sandialabs.github.io/rattlesnake-vibration-controller/book/chapter_15.html. It is not a required acquisition platform.

## Acceptance and failure evidence

Acceptance requires calibrated complex data, uncertainty, joint fits, physical scale, and independent predictions. A published frequency list is insufficient, and one microphone match is less specific still.

If poles match but residues do not, inspect port location, force scale, sensor direction, and mode normalization. If phase disagrees systematically, inspect latency and polarity before changing damping. If only one support configuration fits, investigate support impedance rather than declaring a universal body model.

The reduced model receives poles, residues or second-order matrices, port definitions, damping interpretation, and residual corrections together. Frequencies and decay times without coupling coefficients cannot reproduce physical string loading.
