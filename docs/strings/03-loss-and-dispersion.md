# Linear loss, dispersion, and a trustworthy string baseline

## The question this baseline answers

Before modeling a nail, establish that a single unforced string transports,
stores, and dissipates energy as specified. The experiment is deliberately less
ambitious than a playable instrument. It asks whether the numerical string has
the frequencies, decay rates, spatial shapes, and boundary reactions predicted
by its declared equations. If these are wrong, later contact fitting can conceal
the error but cannot repair the evidence.

For one transverse displacement `u(x,t)`, a useful uniform baseline is

\[
\mu u_{tt}=T u_{xx}-EI u_{xxxx}
-2\mu\sigma_0u_t+2\mu\sigma_1u_{txx}+f(x,t).
\]

Here `sigma_0` has units of inverse seconds and `sigma_1` has units of square
metres per second. Both are nonnegative in this baseline. Their signs follow from
integration by parts: the spatially differentiated loss term removes energy under
compatible boundaries. A sign copied without its operator convention can turn
loss into gain. This equation is a selected approximation, not a universal
constitutive law for every string material.

Set `c² = T/mu` and `kappa² = EI/mu` when comparing with normalized numerical
equations. The symbol `kappa` is not a wave number here; use a different symbol,
such as `k_n`, for modal wave number. Confusing them creates dimensionally
plausible-looking but incorrect stability expressions.

## Boundary conditions are part of the model

A fourth-order spatial equation needs more boundary information than an ideal
flexible string. Simply supported ends impose zero transverse displacement and
zero bending moment. Clamped ends impose zero displacement and zero slope. A
bridge connection can instead impose a relation between boundary force and body
velocity, with a separate moment assumption. These choices produce different
spectra and reactions. The phrase fixed end is therefore insufficient in a test
manifest.

For an analytically convenient simply supported fixture, use sine modes. With
`k_n = n*pi/L`, the undamped angular frequency and modal amplitude-decay coefficient
are

\[
\omega_{0,n}^2=c^2k_n^2+\kappa^2k_n^4,
\qquad \gamma_n=\sigma_0+\sigma_1k_n^2.
\]

An underdamped mode oscillates at
`sqrt(omega_0,n² - gamma_n²)`, not exactly `omega_0,n`. An envelope fit that ignores
this distinction should state why its damping is sufficiently weak. Mode
amplitude decays as `exp(-gamma_n t)` in this fixture; mode energy decays as
`exp(-2 gamma_n t)`. Confusing amplitude and energy rates introduces a factor of
two that can survive a surprisingly large amount of listening-based tuning.

## Energy and its useful diagnostics

For constant physical parameters, the continuous stored energy is

\[
\mathcal E=\frac12\int_0^L
\left(\mu u_t^2+T u_x^2+EI u_{xx}^2\right)\,dx.
\]

Compatible boundary conditions and applied forces give a balance containing
external work and the nonnegative loss rates

\[
\mathcal D=2\mu\sigma_0\int_0^L u_t^2\,dx
+2\mu\sigma_1\int_0^L u_{tx}^2\,dx.
\]

This expression motivates the design; it is not automatically the conserved
quantity of a discrete update. The numerical chapter must derive the discrete
energy associated with its exact temporal and spatial operators. A continuous
energy estimate can oscillate slightly for an otherwise energy-consistent
discretization. Conversely, a cosmetically smooth estimate can miss an unstable
grid mode if diagnostics project onto only low modes.

Record total energy, individual modal energies in simple fixtures, accumulated
dissipation, and boundary work. A residual is meaningful only if its convention
is stated, for example `current stored energy - initial stored energy + lost
energy - supplied work`. Normalize it by a nonzero, documented scale. Near
silence, an absolute tolerance is necessary because division by a nearly zero
initial energy magnifies harmless roundoff.

## Numerical representation is a decision, not a brand

A finite-difference string offers spatially local contacts and visible geometry.
A modal string offers a clear reference for linear uniform boundaries and can be
efficient when only a bounded band matters. A wave-based representation can be
efficient for propagation but requires carefully designed dispersion, loss,
fractional-delay, and termination behavior. None is automatically more physical
because of its name. Compare them on the same equations and observables.

Woodhouse's [On the Synthesis of Guitar Plucks, Acta Acustica 90,
928–944](https://euphonics.org/wp-content/uploads/2022/03/Guitar_I.pdf), especially
the method comparisons, is a useful primary reading for this distinction.
Mansour, Woodhouse, and Scavone's [2016
paper](https://doi.org/10.3813/AAA.919021), particularly its time-domain model and
evaluation sections, revisits refined wave-based implementation. Neither paper
licenses selecting coefficients by ear while calling the result identified.

The repository's explicit FDTD example has a particular loss discretization.
Read [its complete scheme and
bound](../numerics/01-stiff-string-fdtd.md) before preparing a grid. Its sufficient
bound does not apply to centered replacement terms, arbitrary compliant
boundaries, or nonlinear contacts inserted after the fact. A parameter change
can invalidate a prepared grid even when the new fundamental remains in the
audible range. Grid admissibility belongs in configuration validation.

## A loss model should fail visibly when it is too small

The two-coefficient loss law predicts a simple dependence on squared modal wave
number. Measured decay may not follow it across the useful band. Start with the
small model, inspect residuals, and add a richer passive representation only if
the residuals matter to the intended output. A larger polynomial in frequency is
not safe merely because it fits positive decay rates at sampled points; between
those points it may imply gain or create an unstable discretization.

Candidate extensions include passive internal variables, constrained relaxation
models, or stable loss filters in a wave representation. The appropriate choice
depends on the material and numerical architecture. Require nonnegative
dissipation or a justified passivity condition over the full represented band,
not just at the fitted partials. Document extrapolation behavior beyond the
measurement band and avoid implying confidence there.

Separate string loss from bridge leakage. If a measured partial decays quickly
because it couples strongly to a body mode, fitting that rate as intrinsic string
damping and then coupling the same body adds loss twice. A string-only fixture,
joint fit, or carefully designed transfer-function experiment can help resolve
the ambiguity. If it cannot be resolved, state the effective-model interpretation
and restrict reuse of the parameters.

## Dispersion and pitch are not interchangeable

Bending stiffness makes partial spacing depart from exact harmonics. Tuning the
first partial does not tune every higher partial. A correction that forces all
partials onto integer multiples of the fundamental removes the behavior the
stiff-string model was introduced to represent. Likewise, a global frequency
scale can hide a length error without fixing its contact geometry.

For a proposed waveguide, compare loop phase against the target modal spectrum
over the intended band. Fractional delay and dispersion compensation interact;
their combined phase, not each component's isolated specification, sets
frequencies. A unit-magnitude allpass can be stable while producing a wrong
dispersion curve. Record both magnitude/passivity and phase-error tests.

For a finite-difference model, physical dispersion and numerical dispersion
coexist. Refinement should reduce the latter while retaining the former. Plot
frequency error against both mode number and mesh spacing. Do not interpret
numerical bending of the spectrum as measured material stiffness. A calibrated
parameter should not need to drift dramatically as the grid is refined.

## A falsifiable experiment sequence

Choose initial conditions with finite energy under the selected equation. A
continuum triangular displacement has a slope discontinuity. For nonzero bending
rigidity, its second derivative is not square-integrable, so the ideal sharp
triangle does not have finite bending energy in this model. A finite-mode
projection is a well-defined synthetic fixture, but its bending energy need not
converge as the mode count increases. In a simply supported expansion, triangle
coefficients fall approximately as inverse mode number squared, while the
bending contribution weights their squares by the fourth power of mode number.
This explains why more modes can keep adding bending energy.

Use smooth finite-width loading or a sufficiently regular displacement when
claiming energy convergence. The bootstrap's finite-mode triangular fixture can
test its stated modal evolution and displacement convergence; it is not a
finite-contact nail model and must not be used to claim continuum bending-energy
convergence. Keep this limitation in reports even when its rendered tone sounds
plausible.

First disable forcing, losses, and coupling and initialize one analytic mode.
Measure frequency and energy residual over enough cycles to expose accumulated
phase error. Repeat for several modes including one near, but inside, the
declared usable band. Modes outside that band still matter for stability, even
if they are not intended as accurate audio components.

Next enable one loss mechanism at a time. Compare fitted envelope rates with the
analytic fixture and ensure that the lossless limit is recovered continuously.
Then initialize a mixture of modes and verify that the linear model obeys
superposition. This catches state-sharing and indexing defects that a single-mode
test misses. Finally apply a finite-width force with known time history and
compare work against the energy ledger.

Repeat the suite under refinement and with at least two independent reference
paths. A high-resolution run of the same mistaken update is not independent
evidence. Analytic modes, a separately constructed matrix exponential for a small
system, or a different discretization can provide useful checks. The reference
must use the same boundary model and initial condition before differences are
interpreted as numerical error.

## Deliverables and gate

Inputs are the parameter records, boundary specification, sample rate, intended
band, and declared error budget. Outputs are derivations, a prepared-state
validator, frequency/decay reports, energy traces, and failed-case diagnostics.
Acceptance requires convergence in the declared observables, no unexplained
energy growth, correct recovery of ideal and lossless limits, and rejection of
inadmissible configurations before rendering.

A passing result establishes this linear baseline only. It does not establish
fingernail realism, fret behavior, body radiation, or cultural playability. The
next chapter extends spatial motion only after the scalar baseline is reliable;
later contact chapters must repeat the energy and convergence argument for the
combined system rather than inheriting a pass from an uncoupled string.
