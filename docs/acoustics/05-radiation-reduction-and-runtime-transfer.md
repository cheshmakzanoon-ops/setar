# Acoustics 05 — Radiation reduction and real-time transfer assets

## Objective and proposed status

This unimplemented P10/P13 task converts accepted offline acoustic results into bounded real-time computation. Inputs are converged loading and observation transfers, structural/cavity bases, port definitions, target sample rates, and an error budget. Outputs are immutable model assets, passive load realizations, causal pressure observations, state-transition rules, and full-versus-reduced evidence.

The real-time model must remain a physical state model. Measured or computed parameters are allowed; prerecorded note playback is not the engine. A radiation impulse response is an operator describing propagation from physical state, not a substitute note source.

Do not choose reduction order from an arbitrary round number. Determine it from loading error, radiation error, temporal behavior, spatial interpolation, and measured computational cost.

## Loading and observation are different reductions

Radiation loading maps surface velocity or aperture volume flow to conjugate effort. It belongs inside the feedback system and must be passive. Observation maps physical states or source coefficients to pressure at a location. It must be causal and stable but need not be positive real because microphone pressure is not conjugate to bridge force.

Keep separate data fields, fitting objectives, and tests for these two products. A pressure transfer that matches one microphone cannot certify loading. A passive impedance can be mechanically correct while its microphone observation remains spatially inaccurate.

If both are derived from the same acoustic solve, retain their shared provenance and basis. Independent fitting can still introduce inconsistencies; compare the resulting coupled response against the full reference.

## Surface source basis

Map structural coordinates to physical normal velocity through a surface operator \(S\):

\[
v_n=S\dot q.
\]

If \(q=Vq_r\), then \(v_n=SV\dot q_r\). Aperture volume-flow coordinates may add separate source columns. Store basis units and integration weights explicitly.

A source basis chosen only from structural eigenmodes can miss localized aperture flow or interface deformation. Enrich it with physically relevant patterns and evaluate the resulting improvement. Conversely, do not retain high-dimensional surface detail that contributes no meaningful loading or observation error.

For load projection, use the conjugate transpose or transpose appropriate to the real spatial basis and harmonic work convention. Congruence projection preserves the mechanical power relation. An arbitrary low-rank matrix approximation may not.

## Frequency sampling

Sample the offline transfer densely enough to resolve narrow resonances, antiresonances, phase delay, and rapid aperture effects. Begin with a broad grid, estimate interpolation error, then refine adaptively.

Retain complex values. Fitting magnitude alone cannot recover nonminimum-phase zeros or spatial cancellation. Unwrap phase only as a diagnostic; the complex data are the authoritative representation.

Include low-frequency asymptotes and high-frequency behavior needed for safe extrapolation. A fit restricted to audible peaks can become active or unstable between samples or beyond the fitting interval.

Use independent validation frequencies that were not selected by the fitting algorithm. Also compare time-domain responses, because a good sampled frequency fit can conceal small noncausal precursors or long-tail errors.

## Rational load realization

Fit a shared-pole matrix model or reduce a passive state-space realization. Preserve reciprocity where assumed. Stable poles are necessary but not sufficient for passive interconnection.

Assess passivity globally for the chosen rational form where practical. Frequency sampling alone may miss a narrow negative-resistance interval. If enforcement is needed, quantify the change and refit or increase order when the correction is too large.

Do not apply independent scalar passivity fixes to matrix entries. Cross terms determine whether simultaneous port excitation can extract unaccounted energy. Use matrix-level assessment.

SINTEF’s primary Vector Fitting resources describe rational fitting and passivity as separate operations: https://www.sintef.no/en/software/vector-fitting/. Package availability and license conditions must be verified before adopting a specific implementation; the method can be implemented or replaced independently.

## Causal observation fitting

Pressure transfer includes propagation delay. Isolate a physically justified reference delay if that improves fitting, then realize the remaining causal response. Do not remove different arbitrary delays from each mode without preserving their relative phase.

A far-field asymptotic transfer may factor out \(e^{-ikr}/r\). A near-field transfer generally contains richer distance dependence. Store which factorization was used so microphone motion does not apply distance attenuation or delay twice.

Minimum-phase reconstruction from magnitude is an approximation that can destroy spatial cancellation and attack timing. Use it only as a separately validated option, not a default replacement for computed complex transfer.

An FIR observation can be stable by construction but may be long. An IIR fit can be compact but needs pole, causality, and error checks. Partitioned convolution can reduce cost but introduces scheduling and latency considerations. None should alter physical feedback if the observation is strictly one-way.

## Spatial compression

Use singular-value or spherical-harmonic compression when it reduces multiple microphone transfers efficiently. Weight frequency and direction according to the validation objective. A rank chosen from total energy can miss a quiet but important directional feature.

Retain withheld positions and directions for testing. Spatial interpolation should reproduce propagation phase and amplitude trends, not just a smooth-looking gain surface.

A low-rank observation approximation does not automatically define a passive low-rank load. If the same basis is reused for loading, project through the work pairing and recheck positivity.

Store angular convention, basis normalization, direction ordering, radial factors, and coordinate transforms. These are as important as filter coefficients. A swapped azimuth convention can make a correct asset point in the wrong direction.

## Discretization at audio rates

Choose a discrete update consistent with the coupled energy contract. Midpoint/trapezoidal discretization is a useful baseline for passive linear state equations, but port time levels and supply rate must be derived for the complete connection.

The bilinear frequency mapping is

\[
\omega=\frac{2}{\Delta t}\tan(\Omega/2).
\]

It introduces frequency warping. Evaluate resulting modal and radiation errors rather than assuming a stable transform is acoustically exact. Prewarping individual parts independently can disrupt a jointly fitted physical relationship.

Exact isolated-mode integration under a chosen input hold is another option, but its interconnection still needs a compatible work definition. Exact integration of each subsystem separately does not prove exact or passive coupling.

Generate sample-rate-specific coefficients from a canonical physical realization. Do not treat coefficients prepared at one sample rate as universally valid. Verify all supported rates and transitions.

## Aliasing and out-of-band excitation

Sharp contact forces can contain energy above the validated acoustic band. The model must remain stable and passive there even if detailed radiation is inaccurate. A safe tail is not the same as a validated high-frequency sound.

Oversampling, physically finite contact duration, and energy-aware filtering can help, but each has a defined role. Do not filter only the audible output while leaving an unstable high-frequency feedback load inside the engine.

If force or velocity is filtered at a physical interface, include the filter in the power and latency analysis. A causal low-pass filter can be stable yet alter phase enough to destabilize an otherwise passive connection.

Multirate coupling requires conservative transfer or a derived work ledger. Decimating bridge force and interpolating body velocity independently is not automatically energy preserving.

## Runtime asset contract

The asset should include a schema version, provenance hashes, physical units, harmonic convention, coordinate frames, valid parameter range, mechanical ports, state dimension, canonical matrices, sample-rate realization metadata, observation basis, delays, and uncertainty.

Separate immutable coefficients from mutable state. Construct, validate, and allocate the asset away from the audio callback. Publish a ready asset atomically through the runtime architecture’s approved mechanism.

The callback performs bounded arithmetic over fixed-capacity arrays. It must not parse files, fit filters, allocate matrices, lock a mutex, or run an unbounded nonlinear solve. A successful offline fit is not evidence of real-time safety.

## Initialization and state transitions

Initialize mechanical, cavity, and radiation memory consistently. Zero state is appropriate for a silent reset, but not for changing microphone position during an ongoing decay if the filter representation carries propagation memory.

Changing observation position may require parallel filters or a state mapping. Changing loading parameters changes physical stored energy and requires a different policy. Do not use one generic crossfade for both and describe it as exact physics.

Map old reduced states into a new basis through a physical projection when possible. Quantify lost and added energy. If a deliberately nonphysical transition is used for user convenience, isolate it from validation renders and document its bounded behavior.

## Verification matrix

Test a unit impulse, smooth broadband input, narrowband forcing near resonances, multiport simultaneous excitation, and zero-input decay. Include worst-case parameter combinations within the declared range.

Compare full and reduced bridge mobility, cavity pressure, radiated power, and microphone pressure. Report phase, absolute gain, decay, and spatial error separately. A single audio difference norm cannot diagnose the cause.

For each sample rate, verify discrete poles, energy balance, passivity where applicable, finite outputs, and latency. Include long-run tests and block-size variation to ensure the model does not depend on host partitioning.

Measure actual processing cost with a declared hardware/compiler/workload manifest. Never invent a performance claim from state dimension alone. Cache behavior, vectorization, denormals, and host scheduling can matter.

## Task sequence and gate

1. Freeze the accepted offline transfer package.
2. Define load and observation error budgets separately.
3. Select and verify a source basis.
4. Adaptively sample frequency and direction.
5. Fit passive loading and causal observation.
6. Assess global physical constraints and extrapolation.
7. Discretize with synchronized ports.
8. Validate rates, blocks, and transitions.
9. Measure bounded runtime cost.
10. Publish asset and reproducibility report.

Acceptance requires both physical and runtime evidence. A compact filter bank that sounds plausible does not pass if it changes string decay incorrectly. A mechanically accurate model does not pass if it allocates inside the callback or loses state on every block.

The exchange-format chapter defines how to retain enough information for another engineer to reproduce this reduction without the original proprietary solver or an undocumented notebook.
