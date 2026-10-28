# Calibrated mechanical FRFs and modal identification

## Objective and validity domain

Measure the relationship between a known mechanical force and a mechanical
response at explicitly defined ports. The primary output for string/body coupling
is usually mobility, velocity divided by force. A recording of an impact sound
without a force reference is not a calibrated mechanical FRF. A clean spectrum
does not establish that the instrument was unperturbed by its supports or sensors.

This proposed protocol is motivated by R-SETAR-MANSOUR-SCAVONE-2012, sections 3
and 4, and the structural studies in the source ledger. It is not a claim to
reproduce those experiments exactly. The project's own instrument, frequency
band, amplitude domain, support condition, and uncertainty target must be approved
before acquisition.

## Define the port contract

A port includes a location, force direction, response direction, and reference
frame. For collocated translational ports, choose force and velocity signs so
`F*v` is positive power entering the modeled body. Rotational ports use torque
and angular velocity. If a force is applied over an area, record the area and
distribution approximation rather than pretending it is an exact point input.

The response quantity matters:

| Quantity | Definition | SI unit |
| --- | --- | --- |
| Mobility | `Y = V/F` | m s^-1 N^-1 |
| Accelerance | `H_a = A/F` | m s^-2 N^-1 |
| Receptance | `H_x = X/F` | m N^-1 |
| Rotational mobility | angular velocity / torque | rad s^-1 (N m)^-1 |

With a Fourier convention where differentiation corresponds to `j*omega`,
`Y=H_a/(j*omega)` and `H_x=Y/(j*omega)` away from zero frequency. Do not divide
by zero or integrate broadband acceleration noise without a documented low-
frequency policy. Store the original measured quantity and the conversion
operator. A plot labeled only `FRF` is incomplete.

## Acquisition-chain calibration

List each sensor, conditioner, preamplifier, acquisition channel, cable, filter,
and digital scaling step. Record the sensitivity and its units, uncertainty,
frequency dependence where relevant, calibration validity, and polarity. If the
force-channel voltage is `e_F` and its sensitivity is `s_F` in V/N, then
`F=e_F/s_F` after applying the documented conditioner and channel gains. Apply
the same explicit accounting to velocity or acceleration. Do not multiply by a
sensitivity that is already incorporated into the acquisition software.

Perform an end-to-end check with a benign reference system whose response is
known sufficiently for the decision. This check should expose sign reversal,
channel swapping, gain errors, and relative latency. Electrical loopback tests
check electronics but not the mechanical transducer. Mechanical checks test the
transducer but may have their own fixture uncertainty. Use both where practical
and preserve their distinct scopes.

Relative channel delay creates a frequency-dependent phase ramp. Measure it or
bound it before interpreting reciprocity or passivity. Record anti-alias filters,
sample rate, channel synchronization, and whether devices share a clock. A
common nominal rate is not proof of sample alignment. Monitor overload indicators
and raw integer or floating-point values; a waveform can be distorted before
digital full scale is reached.

## Fixture and support pilot

Begin with a small set of ports and repeat them after removing and replacing the
instrument in its support. Measure or bound fixture motion and identify fixture
resonances. A soft suspension approximates free boundaries only above a frequency
range where suspension effects are sufficiently small; it is not exactly free.
A clamp imposes its own stiffness and loss. A player-like support is closer to
performance in some respects but less reproducible.

Do not declare one support universally correct. Choose it for the claim and
compare alternatives when the intended model needs their relationship. Record
contact area, padding, placement, and any safe preload estimate. Never tighten a
clamp beyond the approved handling procedure to improve a noisy trace.

## Sensor-loading control

Prefer a noncontact response method when it gives adequate signal and does not
require harmful surface treatment. Noncontact sensing still has limitations:
line-of-sight direction, optical dropout, target reflectivity, rigid-body motion,
and spatial registration. A one-axis laser measurement captures a projection of
velocity, not the full vector.

If a contact sensor is used, quantify its mass, attachment, cable force, and
location. Compare selected responses with and without the sensor through an
independent method where possible. A dummy mass or controlled mass sequence can
help diagnose loading, but it does not reproduce all stiffness and damping of
the real attachment. Avoid treating a small total mass fraction as proof that
local modal loading is negligible.

For a shaker, account for stinger, force sensor, attachment, and the impedance of
the excitation system. A force reference can measure the applied force while the
attached apparatus still changes the target's dynamics. Assess that perturbation
against the uncertainty target. If the response cannot be measured without
meaningful loading, either model the loaded assembly or restrict the claim.

## Excitation design

Choose impact, swept-sine, multisine, or another excitation according to required
bandwidth, linearity, safety, and repeatability. An impact is convenient and can
minimize persistent attachment, but double hits, inadequate high-frequency force,
and variable location are common hazards. A steady excitation can improve
signal-to-noise ratio while introducing attachment loading and prolonged energy
input. No choice is automatically superior for every question.

Set safe excitation limits through a pilot and specialist-approved handling,
not by increasing force until the microphone sounds loud. Record force peak,
impulse, contact duration, location, and direction for every trial. Reject double
hits according to a defined detection and review rule. A force spectrum must
have sufficient usable energy throughout the claimed band; do not divide by
near-zero input bins and interpret the resulting spikes as modes.

Test at multiple safe levels. If normalized responses depend on level beyond
uncertainty, the linear FRF assumption is limited. The cause may be contact,
preload change, joint motion, or sensor behavior. Preserve the level dependence
instead of averaging it away. Linear identification should use a declared
small-signal region; nonlinear performance experiments need separate protocols.

## Record length and windowing

Choose record length so the response behavior relevant to damping and low-
frequency resolution is captured. A nominal bin spacing `delta_f=1/T_record`
describes the transform grid, not guaranteed separation of closely spaced modes.
Zero padding creates more plotted points but no additional information.

Capture enough pre-trigger data to estimate noise and verify the event onset.
Avoid truncating the force pulse. If the response has not decayed within the
record, lengthen the record where possible. An exponential response window can
reduce leakage but adds apparent damping; its influence must be included in the
identification or corrected with a verified method. Do not apply the same window
blindly to force and response when their time support differs.

Document window definitions numerically, including start, end, taper, and any
exponential coefficient. Store raw unwindowed data. Test the processing on a
synthetic known oscillator to show how the chosen record and window affect
estimated frequency, damping, and residue.

## Spectral estimators and coherence

Define cross spectra explicitly as `S_xy = E[X*conj(Y)]`. With force input `F`
and velocity output `V`, the common output-noise-oriented estimator is

\[
\hat Y_1=S_{vF}/S_{FF}.
\]

A complementary estimator is `Y_2=S_vv/S_Fv` under its corresponding noise
assumptions. Neither is universally unbiased when both channels are noisy,
correlated disturbances exist, or the system is nonlinear. Compare estimators
and noise models when their differences affect the conclusion. Averaging
individual ratios is not generally equivalent to forming a ratio of averaged
spectra.

Magnitude-squared coherence is

\[
\gamma^2=\frac{|S_{vF}|^2}{S_{vv}S_{FF}}.
\]

It is a diagnostic of the measured relationship under the averaging assumptions,
not proof of correct calibration, negligible loading, or physical validity. A
systematic gain error can coexist with high coherence. Low coherence near an
antiresonance may reflect poor response signal-to-noise ratio rather than a
nonlinear instrument. Report coherence alongside force spectrum, response noise,
and repeated-trial variability rather than using a single universal threshold.

## Multiport measurements

For several force ports and response ports, collect a matrix with each element's
location, direction, and calibration. Preserve the same configuration while
roving a response or excitation point. Revisit reference ports between groups
to detect drift. If a sensor is moved, consider whether each position creates a
different loaded structure; a matrix assembled from incompatible structures
cannot be treated as one system without qualification.

For a reciprocal linear passive body, appropriate swapped force/response ports
should agree within uncertainty after sign and coordinate conventions are aligned.
Use reciprocal measurements as a diagnostic, not a command to average away
differences. Investigate noncollocation, sensor loading, timing, state drift, and
direction errors before asserting nonreciprocal physics.

Passivity is a property of the collocated power-port matrix, not of every scalar
off-diagonal transfer entry. Negative real parts in an individual cross transfer
do not automatically imply active energy generation. For an appropriate mobility
matrix, examine the Hermitian part under the declared frequency convention and
uncertainty. A fitted model must preserve passivity where required by the coupled
synthesis architecture.

## Modal identification

A simple real-mode mobility representation has the form

\[
Y_{ij}(\omega)=\sum_r
\frac{j\omega\,\phi_{ir}\phi_{jr}/m_r}
{\omega_r^2-\omega^2+2j\zeta_r\omega_r\omega}
+Y_{ij}^{\mathrm{res}}(\omega).
\]

The exact representation may need complex modes or another state-space form,
but the equation shows why frequencies alone are insufficient. Residues,
normalization, damping, port shapes, and out-of-band residual terms matter.
Record how modes are matched across model orders and repeated acquisitions.
Closely spaced modes can exchange labels; compare subspaces when individual
mode labeling is unstable.

Use stabilization analysis, residual checks, and independent response channels
to distinguish physical modes from numerical fit poles. Validate damping against
time-domain decay where appropriate. A narrow apparent peak can be a fixture or
string resonance rather than a body mode. The classification must use spatial
and configuration evidence, not only the attractiveness of a fitted pole.

## Quality review and handoff

For every accepted FRF, retain raw force and response, calibration chain,
configuration, location/direction uncertainty, estimator settings, windows,
averaging, rejected trials, usable band, and uncertainty. Deliver the complex
response rather than only a magnitude image. For modal outputs, include fit
covariance or another defensible uncertainty description, model order, residuals,
mode normalization, and held-out response prediction.

The gate passes only for the specified amplitude and frequency domain. Stop when
force or response saturates, input energy is insufficient, channel units are
unknown, timing invalidates phase, or attachment perturbation exceeds the claim's
uncertainty. A narrower valid band is preferable to a broad unreliable one.
Next actions should target the dominant limitation: a better calibration, another
port, safer excitation, improved support, or an independent loading control.
