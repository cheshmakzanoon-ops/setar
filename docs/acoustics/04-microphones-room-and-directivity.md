# Acoustics 04 — Microphone observation, directivity, and room separation

## Objective and status

This proposed, unimplemented P10/P17 task defines what the rendered or measured audio actually represents. Inputs are accepted structural/acoustic states, pressure-field transfer functions, microphone geometry and calibration, and room conditions. Outputs are calibrated observation operators, directivity data, room-separated comparisons, and a reproducible mapping from physical pressure to digital audio.

The instrument’s state is not the same thing as one recording. Moving a microphone changes the observed spectrum without changing the body’s mechanical poles. Changing a room changes decay and interference. A credible physical model must state which of these effects are part of the instrument and which are part of observation.

## Observation chain

Use an explicit chain:

\[
\text{physical state}\rightarrow\text{radiated field}
\rightarrow\text{propagation/scattering}
\rightarrow\text{microphone response}
\rightarrow\text{electrical gain}
\rightarrow\text{digital samples}.
\]

Each arrow has units, a coordinate frame, and a validity assumption. The chain can be simplified for a virtual microphone, but the simplification must be visible.

Keep mechanical loading separate. A virtual microphone normally observes without changing the model. A physical microphone can scatter the field and has finite dimensions; whether that effect matters depends on frequency and distance. Do not add microphone response into bridge impedance merely because both are frequency-domain matrices.

## Position and orientation

Record microphone acoustic-center coordinates in the specimen frame and its orientation. Stand markings or nominal capsule distance may not equal acoustic-center distance. Keep the transformation into the room frame so repeated instrument orientations can be reconstructed.

A close microphone sees spatially varying pressure and may emphasize one soundhole or soundboard region. A far-field microphone sees a directional sum. Neither is inherently the “true sound”; they answer different questions.

Record uncertainty in position and orientation. At high frequency, small distance errors produce phase errors. At a cancellation null, small angular errors can cause large relative magnitude changes. Do not interpret such sensitivity as unstable instrument physics.

For arrays, calibrate relative timing and channel order. A channel permutation can preserve individual spectra while destroying reconstructed directivity. A common timing offset differs from channel-dependent latency and must be handled separately.

## Calibration field type

Pressure, free-field, and diffuse-field microphone calibrations are not interchangeable. NIST’s “Importance of the Free-Field Calibration of Microphones” explains why field type and orientation affect calibration: https://www.nist.gov/publications/importance-free-field-calibration-microphones.

Use calibration appropriate to the measurement field and microphone. A single sensitivity value at one frequency does not establish broadband magnitude and phase. Preserve the calibration certificate or measured transfer with its conditions and uncertainty.

The primary NIST report “Calibration of Pressure and Gradient Microphones” discusses complex sensitivity and phase measurement: https://www.nist.gov/system/files/documents/calibrations/eacous.pdf. Its methods are references for metrology planning, not a claim that this project owns laboratory-standard microphones.

Do not infer calibration from a consumer microphone’s marketing frequency-response plot. Such plots may omit unit-to-unit variation, phase, incidence angle, and processing. If a convenient microphone is used for exploratory data, label the resulting limitations.

## Pressure units and digital level

Maintain pressure in Pa until a documented output gain maps it to dimensionless digital amplitude. Store the mapping in the render manifest. A normalized WAV file loses absolute pressure unless its normalization factor is retained.

For sound-pressure level, specify RMS pressure and the reference pressure. Do not mix peak phasor amplitude with RMS formulas. A spectrum displayed in dBFS is not automatically dB SPL.

Loudness matching for listening tests is a separate operation. Preserve the original calibrated render and the applied matching gain. Otherwise a body model with incorrect radiation efficiency can appear correct after hidden normalization.

Clipping protection belongs to output engineering, not physical validation. If a limiter is enabled, report it and exclude limited regions from claims about the unprocessed physical waveform.

## Structural resonance versus radiated peak

A structural pole can have weak bridge participation, weak radiation efficiency, or a node toward one microphone. It may be prominent in mobility and almost absent in pressure. A microphone peak can be enhanced by radiation, source spectrum, or room response without introducing a new structural pole.

Compare transfer functions with the same input definition. Force-to-pressure and velocity-to-pressure contain different frequency factors. A derivative can tilt magnitude substantially without changing the underlying structural resonance.

Track poles and zeros jointly where useful. Antiresonances and directional cancellation can be perceptually important. Fitting only pressure peaks misses the mechanism that shapes the gaps between them.

## Near field and far field

Do not declare a universal far-field distance from instrument size alone without checking the band and source complexity. Evaluate field behavior at multiple radii. Far-field scaling should approach \(1/r\) amplitude with a stable angular pattern after propagation phase is removed.

Near fields contain reactive components and spatial variation that do not correspond directly to outward power. Estimating total radiated power from one close pressure sample can be badly wrong.

For a computed field, compare full potential evaluation with a far-field asymptotic operator at increasing distance. Bempp’s potential-operator documentation provides the relevant point-evaluation concept: https://bempp.com/handbook/api/potential_operators.html. The selected library’s harmonic convention still needs verification.

## Directivity definition

In the far field, write

\[
p(r,\Omega)=A(\Omega)\frac{e^{-ikr}}{r}.
\]

For peak phasors in a lossless medium,

\[
P_{\mathrm{rad}}=\frac{1}{2\rho_0c}
\int_{4\pi}|A(\Omega)|^2\,d\Omega.
\]

A directional factor is

\[
D(\Omega)=
\frac{4\pi|A(\Omega)|^2}
{\int_{4\pi}|A|^2d\Omega}.
\]

State whether a plot shows absolute pressure, normalized amplitude, directivity factor, or directivity index. Normalizing each frequency to its own maximum hides radiation efficiency. It is useful for shape inspection but cannot establish absolute sound output.

Use angular quadrature weights. A latitude–longitude grid has unequal area per point; averaging samples without weights overrepresents the poles. Validate integration on a monopole and low-order spherical patterns.

## Spatial sampling and interpolation

Choose angular resolution from the maximum spatial complexity in the retained band. Increase it until integrated power and salient lobes stabilize. A visually smooth interpolation can conceal under-sampling.

For spherical-harmonic compression, state basis normalization, ordering, coordinate convention, and truncation order. Test reconstruction at withheld directions. A model fitted on one horizontal circle does not establish full-sphere directivity.

Near-field observation cannot always be compressed by the same angular basis at one radius. Use radial dependence or a different representation when microphone motion includes close positions.

Interpolate complex transfers with phase awareness. Linear interpolation of wrapped phase causes artificial discontinuities. Magnitude-only interpolation discards cancellation and propagation information.

## Room separation

First establish a free-field or explicitly characterized propagation reference. A room impulse response can then be applied as a separate observation layer if its use is authorized and its assumptions are appropriate. This is not note-sample synthesis; the instrument state still generates the source.

A single room response may be inadequate for a directional source whose radiation pattern changes with frequency and excitation. A more accurate model combines directional source components with room propagation. State when an omnidirectional room approximation is used.

Do not calibrate body damping against a reverberant tail without separating room decay. Conversely, aggressive time gating can remove low-frequency information and bias the comparison. Report the gate’s frequency-resolution consequences.

If no anechoic facility is available, design a measurement that bounds reflections through geometry, time selection, or independent room characterization. Do not label an ordinary room measurement free-field because the first impulse peak looks clean.

## Player and nearby-object scattering

A player’s torso, hands, clothing, and nearby surfaces can affect both mechanical support and acoustic scattering. Keep these mechanisms separate. A support impedance changes instrument state; scattering changes propagation.

Begin with an instrument-only reference and a clearly documented supported/played condition. Compare them when data permit. Do not use a generic human mesh as if it were measured anatomy of the performer.

For practical virtual microphones, a limited set of validated positions may be preferable to unrestricted motion with unvalidated interpolation. The interface should not imply physical certainty outside the observation domain.

## Measurement comparison

Compare calibrated force-to-pressure transfers where possible, not just note spectra. This separates excitation uncertainty from radiation. For musical gestures, also retain the force or motion proxies needed to interpret differences.

Align recordings using a justified common time reference. Arbitrary per-frequency phase adjustment can conceal propagation errors. Arbitrary time shifting can conceal wrong source-to-microphone delay.

Use held-out microphone positions and orientations. A fit at one microphone can overfit a room null or local radiation feature. Predicting another position is a stronger test of spatial physics.

Report both absolute and perceptually relevant errors. A small relative error at a nearly silent null may be unimportant; a modest error in a dominant attack band may matter greatly. Do not collapse all discrepancies into one score without explaining its weighting.

## Observation task

1. Define specimen and room frames.
2. Record microphone centers, orientation, and uncertainty.
3. Obtain appropriate calibration and gain metadata.
4. Compute physical pressure transfers.
5. Verify near/far-field behavior and propagation phase.
6. Measure or bound room contamination.
7. Validate directivity integration and interpolation.
8. Compare held-out locations.
9. Map Pa to digital amplitude explicitly.
10. Preserve unnormalized and listening-test versions separately.

## Acceptance and handoff

The gate requires calibrated units, spatial prediction, room separation, and correct pressure-to-audio scaling. A pleasant stereo render is not sufficient evidence.

The runtime asset includes observation basis, coordinate transforms, valid microphone region, delay convention, calibration gain, and uncertainty. Mechanical loading remains in the body/acoustic state model. This separation allows creative microphone and room controls without pretending that they alter wood stiffness or string energy.
