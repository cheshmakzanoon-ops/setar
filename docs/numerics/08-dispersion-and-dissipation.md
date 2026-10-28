# Measuring dispersion, decay, aliasing, and computational tradeoffs

## Three separate questions

A solver may be stable, inaccurate, and fast at the same time. It may be
accurate but too expensive, or stable only within a narrower parameter envelope
than the product exposes. Numerical evaluation must therefore ask separately:
does the state remain controlled; does it approximate the chosen physical model;
and can the implementation meet its bounded runtime budget?

This chapter defines metrics that keep those questions distinct. Inputs are
analytic or independently converged reference responses, candidate solver output,
physical sample timing, and prepared parameter fixtures. Outputs are frequency,
decay, transient, coupling, and cost error records. A number is meaningful only
with its reference model, units, and analysis procedure.

## Physical dispersion of the stiff string

For a traveling component with spatial wavenumber \(\xi\), the undamped continuum
dispersion relation is
\[
\Omega(\xi)=\sqrt{c^2\xi^2+\kappa^2\xi^4}.
\tag{D1}
\]
For positive \(\xi\), phase and group velocities are
\[
c_p=\Omega/\xi,\qquad
c_g=\frac{c^2\xi+2\kappa^2\xi^3}{\Omega}.
\tag{D2}
\]
An ideal string has both velocities equal to c. In the bending-dominated limit,
phase velocity is approximately \(\kappa\xi\) and group velocity approximately
\(2\kappa\xi\). Thus physical stiffness produces dispersion even before any
numerical approximation is introduced.

Smith's primary author-written discussion *The Stiff String* is a useful
independent source for this model:
<https://www.dsprelated.com/freebooks/pasp/Stiff_String.html>.
The formulas here follow directly by substituting a traveling exponential into
the stated PDE.

## Spatial and temporal dispersion must be separated

For the centered second spatial difference,
\[
s_h(\xi)=\frac4{h^2}\sin^2(\xi h/2).
\]
The semidiscrete frequency is
\[
\Omega_h^2=c^2s_h+\kappa^2s_h^2.
\tag{D3}
\]
For lossless central time differences,
\[
\omega_{\mathrm{num}}(\xi)
=\frac2k\arcsin\left(\frac{k\Omega_h(\xi)}2\right).
\tag{D4}
\]
Comparing D3 with D1 isolates spatial error; comparing D4 with D3 isolates
temporal error; comparing D4 with D1 gives total error. Because the two errors
can have opposite signs, total error can look unusually small at one choice
of h and k. Do not infer that either discretization is independently accurate.

The discrete group velocity follows by differentiating D4. With
\[
\frac{ds_h}{d\xi}=\frac2h\sin(\xi h),\qquad
\frac{d\Omega_h}{d\xi}
=\frac{c^2+2\kappa^2s_h}{2\Omega_h}\frac{ds_h}{d\xi},
\]
one obtains
\[
c_{g,\mathrm{num}}
=\frac{d\Omega_h/d\xi}{\sqrt{1-(k\Omega_h/2)^2}}.
\tag{D5}
\]
Interpret limiting cases carefully where numerator and denominator both approach
zero. The formula is not permission to evaluate an unresolved endpoint ratio
without a limit.

## Frequency error metrics

For a positive reference frequency and a positive candidate frequency, use
\[
e_{\mathrm{cent}}=1200\log_2(f_{\mathrm{num}}/f_{\mathrm{ref}}).
\tag{D6}
\]
Also report absolute hertz error and relative error when useful. A cents value
near zero frequency is not meaningful. A nonoscillatory numerical mode should
be classified as such rather than assigned a fabricated positive frequency.

For a string, report error for each retained physically relevant mode, not only
the fundamental. Fundamental agreement can coexist with substantial high-partial
inharmonicity error. For contact-rich transients, evaluate mechanical bandwidth
needed to generate the force as well as audible output bandwidth.

An acceptable cents budget is a proposed design requirement until supported by
perceptual evidence in the actual listening context. Do not claim a universal
audibility threshold from a convenient numerical target. A mode's level, masking,
duration, and interaction with other strings affect perceptual relevance.

## Numerical damping from amplification roots

For a mode with amplification roots \(r_\pm\), complex-conjugate roots have
decay rate
\[
d_{\mathrm{num}}=-\log|r_\pm|/k,
\quad
\omega_{\mathrm{num}}=|\arg r_\pm|/k.
\tag{D7}
\]
For the backward-gradient-loss FDTD scheme, when the roots are genuinely complex,
\[
|r|^2=
\frac{1-\sigma_0k-2\sigma_1ks}{1+\sigma_0k}.
\tag{D8}
\]
Use D8 only when its complex-root assumptions hold. If the numerator is negative,
the roots cannot be a complex-conjugate pair with that product. A robust diagnostic
computes and classifies the roots instead of taking a logarithm of an invalid
quantity.

The continuous mode has decay parameter
\(d=\sigma_0+\sigma_1\xi^2\), while its semidiscrete counterpart uses s instead
of \(\xi^2\). Compare the three levels separately. The backward time loss also
perturbs effective inertia, so damping and phase errors are coupled.

When d is nonzero, a relative decay error is useful. When d is zero or very
small, report an absolute artificial decay rate and accumulated energy drift.
Dividing by a near-zero reference rate creates a huge relative number that
does not communicate physical significance.

## Decay estimation from rendered signals

Do not estimate modal decay by measuring peaks of a multimode waveform without
separating modes. Beating can produce an envelope that rises and falls even in a
strictly dissipative system. A body-loaded string can have coupled modes rather
than one independent exponential per bare-string sine shape.

For a single-mode synthetic fixture, fit the known oscillator form or use its
state energy directly. For empirical signals, state the analysis window, noise
floor, modal separation method, and uncertainty. Repeat the fit across suitable
windows and inspect residuals rather than reporting one coefficient with false
precision.

If amplitude decays as \(e^{-dt}\), an amplitude reduction by one thousand occurs
at \(\ln(1000)/d\). Use this relation only where the exponential model applies.
A contact that continues to exchange energy with the string invalidates a
free-decay interpretation during that interval.

## Transfer functions and bridge mobility

For the linear semidiscrete system under an \(e^{i\omega t}\) convention,
the collocated velocity mobility is
\[
Y(\omega)=i\omega B_p^T
(K-\omega^2M+i\omega C)^{-1}B_p.
\tag{D9}
\]
Specify the Fourier convention, since changing it conjugates the expression.
Compare magnitude and phase, not magnitude alone. Near an antiresonance, a small
absolute response can make relative magnitude error unstable; use an absolute
or mixed metric there.

For a multiport reciprocal model, compare the full mobility matrix. A scalar
bridge impedance fit may conceal cross-direction or cross-string coupling errors.
Check symmetry, passivity-related real parts in the appropriate convention,
and the response to independent port excitations.

Frequency-response agreement does not validate nonlinear contact, but it is an
efficient way to expose linear coupling defects before nonlinear tests make them
harder to interpret.

## Transients, impulses, and observation bandwidth

A point impulse contains arbitrarily broad temporal bandwidth in the ideal
mathematical model. A finite solver cannot represent that entire spectrum.
Use impulses as distributional benchmark inputs with a declared discrete impulse
mapping, or use a smooth force pulse for a finite-bandwidth convergence test.

Similarly, a sharp triangular displacement can be unsuitable for a stiff-string
energy convergence test because its curvature is singular. A smooth pluck
profile or a mechanically modeled contact gives a better controlled input.
Do not “repair” an inadmissible fixture by adding arbitrary output filtering
and then claiming the underlying mechanical convergence is established.

Compare arrival time, phase-aligned waveform error, unaligned waveform error,
peak contact force, integrated impulse, and energy distribution. Alignment can
be useful for diagnosing shape error, but it must not hide a propagation-delay
error. Report both aligned and physically timed comparisons when alignment is used.

## Aliasing and oversampling

Nonlinear contact can generate frequencies above the internal Nyquist frequency.
An output low-pass filter cannot undo aliasing already created inside the state
update. Oversampling the mechanical solve, using a physically justified contact
regularization, or changing the representation may be required.

Internal oversampling also changes the explicit stability and grid-resolution
tradeoff. Recompute coefficients and admissible spacing. A fixed oversampling
factor is not a universal accuracy guarantee: contact stiffness, impact speed,
and modal bandwidth determine the required resolution.

Separate internal anti-aliasing from sample-rate conversion to the host. The
decimator needs its own passband, stopband, latency, and numerical tests. Its
latency is a signal-path property, not a reason to delay one side of a mechanical
feedback port.

## Accuracy versus cost experiment

Select a fixed set of synthetic physical fixtures spanning low and high tension,
low and high rigidity, several loss values, smooth and contact-rich excitation,
and relevant coupling strengths. For each candidate configuration, record
physical errors, state count, iterations, memory, and measured processing cost.

Use a sufficiently accurate reference and verify that tightening the reference
does not change the reported candidate error materially. If two methods share
the same spatial grid, their agreement cannot establish spatial accuracy.
Include a different representation or analytic case.

Bensa, Bilbao, Kronland-Martinet, Smith, and Voinier compare stiff-string
approximations at controlled computational cost in section 5 of *Computational
Modeling of Stiff Piano Strings Using Digital Waveguides and Finite Differences*:
<https://www.pure.ed.ac.uk/ws/files/11222295/ActaAcustica2005.pdf>.
Use that comparison as methodological background, not a present-day runtime
ranking. Its section 4 includes an additional spatial optimization parameter;
importing that parameter would require a new scheme audit and stability proof.

Plot or tabulate Pareto-efficient configurations only after all rejected or failed
cases are recorded. A configuration with an occasional solver failure is not
equivalent to a reliable configuration with the same average cost. Worst-case
bounded behavior matters in the audio callback.

## Rejection gates and next action

Reject evaluations based only on a stable waveform, one FFT peak, or one audio
example. Reject unexplained numerical damping in a physical parameter fit.
Reject resolution claims based only on audible output when unresolved mechanics
feed back into the contact force.

The next action is to translate the chosen nonlinear method into a bounded,
diagnosable solver. Numerical budgets should remain explicit proposals until
their physical and perceptual significance is measured.
