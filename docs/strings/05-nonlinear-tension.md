# Nonlinear tension without unexplained energy creation

## Separate three kinds of tension change

Tension can change because a tuning peg changes an equilibrium, because a finger
or fret changes the string's geometry, or because dynamic transverse motion
stretches the string. These mechanisms can coexist, but they are not the same
control. A MIDI pitch bend mapped to tension is a chosen actuator policy. A
dynamic tension term is a consequence of the mechanical state. Mixing the two in
one undocumented frequency multiplier makes both identification and energy
analysis difficult.

This chapter develops a candidate quasi-static geometric extension. It is a
proposed model for controlled experiments, not a claim that the current engine
implements it or that it explains every setar attack. Before enabling it, the
linear baseline, two-axis conventions, and contact-free amplitude tests must
already be reliable.

## A useful small-slope derivation

Let the transverse vector be `u = (y,z)` over a fixed reference interval of length
`L`. Under small slopes, the additional arc length from transverse deformation is
approximately

\[
\Delta\ell\approx\frac12\int_0^L |u_x|^2\,dx.
\]

For an extensible uniform string with fixed axial endpoints and quasi-static
axial response, a simple additional tension is

\[
\Delta T=\frac{EA}{L}\Delta\ell
=\frac{EA}{2L}\int_0^L |u_x|^2\,dx.
\]

The total tension in the transverse restoring term is then `T0 + DeltaT`.
`T0` is an established prestress, not a second copy of the dynamic extension.
The assumptions include small slopes, an appropriate axial modulus, uniform
material behavior, and axial equilibration sufficiently fast for the chosen
observables. A string with complex construction may need a different effective
axial law.

The corresponding added stretching energy is

\[
\mathcal E_s=\frac{EA}{8L}
\left(\int_0^L |u_x|^2\,dx\right)^2.
\]

Taking the appropriate variation gives the same additional tension force under
the stated boundaries. This pairing matters: inventing a tension formula and
then using an unrelated energy diagnostic can make an active implementation look
conservative. The formula is a continuous model identity, not an implementation
proof.

## What the approximation excludes

The quasi-static model does not resolve finite-speed longitudinal waves. It does
not automatically include material hysteresis, plastic deformation, winding
slip, thermal changes, or tension-peg friction. It also does not make a moving
contact harmless. If a finger changes the reference path or active segment,
external work and state transformation still need accounting.

Large slopes invalidate the truncated arc-length expression. A code path should
monitor a declared slope metric in reference experiments and identify when the
model is used outside its intended range. The response to an out-of-range state
is not silently clipping slope while retaining the original energy formula.
Possible policies include rejecting the configuration, ending the experiment
with diagnostics, or using a separately verified richer model.

The effective axial stiffness `EA` must not be guessed from a nominal material
name and then described as measured. If it is unavailable, synthetic sensitivity
experiments are still useful. Their purpose is to determine whether plausible
uncertainty could affect the target observable, not to announce the true modulus
of the installed string.

## Amplitude-dependent pitch is an observable, not a verdict

A nonlinear tension model can produce amplitude-dependent frequency shifts.
Measure those shifts with an estimator appropriate to a nonstationary signal.
A single FFT over the entire decay blends early and late frequencies. Short
windows improve time localization while reducing frequency resolution; state
that tradeoff and validate the estimator on synthetic signals with known drift.

Do not attribute every early pitch change to geometric nonlinearity. Nail loading
can change the release condition, fret contact can change the active length, body
coupling can perturb peaks, and overlapping partials can bias the estimator.
Use a contact-free synthetic experiment to verify the mechanism first, then a
controlled physical experiment to ask whether the mechanism is sufficient.

Woodhouse's [2017 study of damping and nonlinearity in plucked
strings](https://doi.org/10.3813/AAA.919135) is useful precisely because it examines
competing explanations for brightness changes in its tested nylon-string system.
Read its nonlinear-effect and measurement sections. Its observations are not a
setar parameter table, and they do not justify treating axial coupling as a
universal explanation for bright attacks.

## Discretize the potential, not just the formula

Choose a spatial derivative and quadrature compatible with the linear energy.
Compute the discrete extension from those operators. Then derive the discrete
force from the same potential or use a suitable discrete-gradient construction.
A different derivative in the tension estimate than in the restoring force can
break the intended energy relation even when both are accurate in isolation.

For an offline reference, the numerical volume's energy-consistent nonlinear
framework is the appropriate starting point. Its solver must report residuals,
iteration counts, and failure conditions. An explicit update with a lagged
tension may be a useful approximation, but it needs an independently derived
stability and energy argument. The linear FDTD bound does not automatically cover
state-dependent tension, especially when the instantaneous tension exceeds the
prepared maximum.

For a real-time implementation, a bounded solve or prevalidated approximation
must have a declared domain. A maximum iteration count is necessary for bounded
work but insufficient for correctness. If convergence is not reached, the engine
needs a documented safe policy and a diagnostic record. Continuing with a wildly
incorrect force because the loop count was bounded is not acceptable failure
handling.

## Interaction with two polarizations

The extension depends on the sum of squared slopes of both transverse fields.
Thus the fields can interact through shared tension even if their linear
equations and boundary ports are otherwise independent. An implementation that
calculates a separate tension for each axis has selected a different model.
Rotation-invariance tests from the previous chapter should still hold in an
isotropic, contact-free configuration.

Initialize equal-energy states with different orientations and compare invariant
observables. Then initialize motion in one axis and a small probe in the other.
Use the reference model to characterize the coupling and distinguish it from
numerical leakage. Do not require the second axis to remain identically zero
under every richer model without examining symmetry and initial conditions.

When a bridge or fret breaks rotational symmetry, document the source of the
asymmetry. A combined test should first disable each coupling mechanism in turn.
Otherwise an unexpected transfer can be blamed on the wrong component, making
parameter fitting less meaningful.

## Fretting and equilibrium extension

Pressing a string toward a fret changes its static path and may alter tension.
If the model resolves that geometry, solve or prepare the corresponding static
equilibrium before interpreting dynamic displacement. Do not add an empirical
pitch-sharpening curve on top of an already modeled geometric extension without
checking for double counting.

A simplified stopping model may approximate the effect through a documented
effective equilibrium and compliance. That is legitimate if its error is measured
over the intended range. The simplification should expose what it omits: finger
position behind the fret, pressure-dependent contact, afterlength motion, and
possible change in axial stretch. A note number alone cannot determine those
quantities.

The static equilibrium solver must identify impossible geometry or excessive
stress. It should not force an arbitrary finger target through the instrument by
increasing tension without limit. Physical admissibility ranges need provenance;
when material failure properties are unknown, the software can still enforce a
conservative declared computational domain without claiming a real-world safety
limit.

## Experiment sequence

Begin with a single analytic transverse mode and vary its initial amplitude over
a synthetic range that remains within the declared small-slope regime. Compare
frequency drift, energy exchange, and convergence against the offline reference.
Repeat with two modes and two polarizations. Test the zero-amplitude limit and
the `EA -> 0` limit to recover the linear model.

Next compare constant tension, quasi-static tension, and a dynamic axial
reference if one exists. Use identical geometry and initial work. Report which
observable distinguishes them. If the difference is below uncertainty or below
the relevant perceptual threshold, the richer model may remain unnecessary for
the real-time path.

Only then add a controlled contact or stopped-note condition. Keep a work ledger
for the contact actuator and monitor geometric validity. A rise in stored energy
is not necessarily instability when an external actuator supplies work. The
criterion is unexplained energy, not a universal demand that energy always fall.

## Acceptance, limitations, and handoff

Inputs are the linear baseline, an axial-stiffness record, a chosen nonlinear
potential, and a domain of slopes and tensions. Outputs include the derivation,
the exact discrete operators, reference trajectories, energy residuals,
amplitude-frequency plots, and bounded-work behavior. Acceptance requires a
consistent zero-nonlinearity limit, rotation covariance where expected,
convergence under refinement, and a clear explanation of every energy source.

The model remains unvalidated for the target instrument until measured
amplitude-dependent observations are predicted outside the fit conditions.
Failure to improve prediction is useful evidence. Do not retain a complicated
nonlinearity merely because it sounds interesting. The next work package is
retuning and configuration transitions, where externally commanded parameter
changes require a separate work ledger rather than being disguised as this
internal geometric mechanism.
