# Body 07 — Damping, dissipation, and causal material models

## What is being identified

This proposed, unimplemented P08/P12 task distinguishes observed modal decay from the mechanisms causing it. Inputs are calibrated FRFs or decays, a verified conservative model, support conditions, and radiation information. Outputs are a dissipative model, energy accounting, parameter uncertainty, and evidence against double counting.

A measured decay includes wood loss, joints, bridge contact, supports, air loading, radiation, and possibly sensor effects. Assigning the entire decay to a “wood damping ratio” creates a parameter that is not transferable between boundary conditions. The model can still use an effective modal decay, but it must be labeled as such.

## Modal viscous damping

For a mass-normalized coordinate,

\[
\ddot\eta_j+2\zeta_j\omega_j\dot\eta_j+\omega_j^2\eta_j=g_j,
\]

the mechanical energy derivative is

\[
\dot E_j=g_j\dot\eta_j-2\zeta_j\omega_j\dot\eta_j^2.
\]

Nonnegative \(\zeta_j\) gives dissipative behavior. The lightly damped amplitude envelope decays approximately as \(e^{-\zeta_j\omega_jt}\); the energy envelope decays twice as fast in the exponent. Do not confuse amplitude decay time, energy decay time, bandwidth, and logarithmic decrement.

Estimate decay only over a region where a single or explicitly modeled set of modes dominates and noise is controlled. Beating between nearby modes can look like nonexponential loss. A room tail can obscure structural decay in microphone measurements.

## Rayleigh damping and its limits

The model \(C=\alpha M+\beta K\) is convenient and passive for nonnegative coefficients when \(M\) and \(K\) are appropriate positive matrices. It implies

\[
\zeta_j=\frac{\alpha}{2\omega_j}+\frac{\beta\omega_j}{2}.
\]

This frequency law is restrictive. Matching two modes does not establish accurate damping elsewhere. The mass-proportional term strongly affects low frequencies; stiffness-proportional loss increases with frequency.

Use Rayleigh damping as a verification or bounded approximation when justified. Do not describe its coefficients as measured wood constants unless a physical identification supports that interpretation. Evaluate the implied damping over every retained and residual mode, not only the fitting band.

For free rigid motion, mass-proportional damping acts against absolute velocity unless formulated relative to another body. That may represent environmental drag but not internal material friction. Internal damping should not dissipate a perfectly rigid translation of an isolated body.

## Local dissipative laws

A Kelvin–Voigt law can be written

\[
\sigma=C_e:\epsilon+C_v:\dot\epsilon.
\]

The viscous tensor must produce nonnegative dissipation \(\dot\epsilon:C_v:\dot\epsilon\). For orthotropic damping, assigning unrelated scalar losses to tensor entries can violate this property. Construct and test the full dissipative tensor in an energy-consistent representation.

Kelvin–Voigt response has a particular frequency dependence. It may overpredict high-frequency loss outside a narrow calibration range. Its simplicity does not make it universal.

A generalized Maxwell representation introduces internal relaxing branches. In scalar form, a positive equilibrium stiffness plus positive branch stiffnesses and relaxation times gives a useful passive realization. Tensor extensions require positive-semidefinite branch stiffnesses and consistent internal variables.

For a branch stress \(z_r\),

\[
\dot z_r+\tau_r^{-1}z_r=C_r\dot\epsilon,
\qquad \sigma=C_\infty\epsilon+\sum_r z_r.
\]

Derive stored energy and dissipation for the chosen tensor realization. Do not merely observe that poles lie in the left half-plane and infer thermodynamic admissibility.

COMSOL’s official “Linear Viscoelasticity” theory section describes generalized Maxwell storage and loss behavior: https://doc.comsol.com/6.4/doc/com.comsol.help.sme/sme_ug_theory.06.029.html. This is a formulation reference, not a claim that the proprietary software or modules are available.

## Complex modulus and causality

A frequency-domain complex stiffness can fit a measured loss factor efficiently. It does not automatically define a causal time-domain algorithm. Real and imaginary parts of a physically causal response are linked; choosing them independently at every frequency can produce a nonphysical model.

Constant hysteretic loss across all frequencies is especially problematic as a global time-domain statement. It can be a band-limited engineering approximation, but the extrapolation and realization must be specified.

Fit a causal internal-variable or rational model over the declared band, then test its asymptotes and passivity. Preserve conjugate symmetry so real input produces real output. Do not mirror only magnitudes while discarding phase.

If fractional models are used, document memory approximation, initialization, truncation, and energy behavior. A compact fractional formula may require substantial history state. Its real-time implementation is a separate approximation, not a free exact feature.

## Separate radiation and structural loss

If exterior radiation impedance is coupled explicitly, some energy leaves through acoustic radiation. A modal damping fit from an assembled instrument already includes that loss unless the experiment separated it. Adding both without adjustment can double the decay.

Use an energy ledger with distinct entries for material dissipation, joint loss, support loss, contact friction, acoustic absorption, and outward radiation. The partition may be uncertain, but it must not be silently duplicated.

A useful comparison is structural response under differing acoustic or support conditions when safely obtainable. However, changing conditions can alter stiffness and mass as well as loss. Interpret differences through the coupled model rather than subtracting damping ratios mechanically.

## Identification and observability

Damping parameters are often less robustly estimated than frequencies. Their uncertainty grows with short records, close modes, low signal-to-noise, and window corrections. Report intervals and sensitivity to fitting choices.

Multiple local loss distributions can produce similar modal damping. If data identify only modal rates, export modal rates rather than inventing a unique spatial material-loss field. A spatial model requires additional observables or physically justified regularization.

Compare loss models on held-out modes and states. A highly flexible branch model can fit a training FRF but extrapolate poorly. Prefer the smallest model that explains the evidence within uncertainty while satisfying passivity.

## Numerical realization tests

1. Derive a continuous energy balance.
2. Test relaxation under a prescribed strain step.
3. Test creep or harmonic response against an independent formula.
4. Verify nonnegative dissipated work for arbitrary strain histories.
5. Verify rigid-body motion does not create internal material loss.
6. Compare frequency-domain and time-domain realizations.
7. Refine time step and branch approximation.
8. Check coupled string/body energy.
9. Audit radiation and support double counting.
10. Export parameters with validity band and uncertainty.

Use synthetic parameter sets with positive branches and deliberately invalid negative branches. The invalid case must be rejected or clearly flagged. Do not normalize output amplitude after instability; that hides the failure.

## Runtime considerations and acceptance

Internal states require deterministic initialization, bounded updates, and parameter-transition rules. Changing relaxation time or stiffness can change stored energy. A preset transition must account for state mapping or use an explicitly bounded crossfade outside the physical claim.

Acceptance requires nonnegative dissipation, causal realization, cross-domain agreement, and held-out decay prediction. A pleasing decay curve alone is insufficient.

The next task reduces the body while preserving these properties. Reduction must project the dissipative model consistently, not retain stiffness modes while attaching unrelated guessed decays.
