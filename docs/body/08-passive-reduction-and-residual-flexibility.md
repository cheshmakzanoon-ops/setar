# Body 08 — Passive reduction, residual flexibility, and validity bands

## Objective and status

This proposed P13 task converts an accepted offline body model into a smaller physical realization. It is unimplemented. Inputs are verified matrices or a passive identified multiport model, port maps, acoustic observations, and explicit accuracy requirements. Outputs are a reduced model, error maps, passivity evidence, state-transition rules, and a declared valid band.

Reduction is not simply retaining the lowest modes until CPU use looks acceptable. The reduced model must preserve the loading seen by strings, the surface motion needed for radiation, and relevant static behavior. Different outputs can require different bases.

## Congruence projection

Let \(q=Vq_r\) with full-column-rank \(V\). A Galerkin congruence gives

\[
M_r=V^TMV,\quad C_r=V^TCV,\quad K_r=V^TKV,\quad B_r=V^TB.
\]

The conjugate output remains \(v=B_r^T\dot q_r\). If the full matrices have positive mass, nonnegative stiffness on the intended subspace, and nonnegative damping, congruence preserves those properties. This is a strong starting point for passive reduction.

The basis need not be modal, but it must be numerically well conditioned. Mass-orthonormalize when appropriate and retain the transformation. Arbitrary column scaling can create poor numerical conditioning even when physics is unchanged.

Petrov–Galerkin methods with different trial and test spaces can be useful but do not inherit the same simple energy argument automatically. Require a separate proof or certificate rather than assuming all projection methods preserve passivity.

## Why frequency truncation is insufficient

High-frequency modes contribute low-frequency flexibility. For a constrained conservative model,

\[
H_x(0)=B^TK^{-1}B
=\sum_j\frac{b_jb_j^T}{\omega_j^2}.
\]

Discarding modes removes positive-semidefinite contributions to static compliance. The resulting endpoint can be too stiff even far below the discarded frequencies.

A residual flexibility correction can restore this behavior, but it must be incorporated in a physically consistent realization. Adding an arbitrary frequency-dependent output correction without corresponding loading can break the bidirectional model.

Free-free systems require a pseudoinverse or elastic-subspace definition because rigid-body compliance is not finite. State that definition explicitly; do not apply a constrained formula to a singular stiffness matrix.

## Interface-enriched bases

Static attachment or constraint modes can capture localized port deformation efficiently. Component-mode synthesis combines internal vibration modes with interface behavior. This is especially useful when bridge and joint coordinates must remain explicit.

Partition coordinates into interior and boundary sets. Static interior response to boundary displacement can be represented by solving the corresponding stiffness block, provided its nullspace and constraints are handled correctly. Retain sufficient fixed-interface modes to recover dynamic flexibility.

NASA’s primary engineering primer on Craig–Bampton reduction describes the component-mode construction and coupling context: https://femci.gsfc.nasa.gov/craig_bampton/Primer_on_the_Craig-Bampton_Method.pdf. Use the method’s structure, not a claim that aerospace acceptance thresholds apply to musical instruments.

Compare modal-only, interface-enriched, and full models on identical bridge forces. A smaller enriched basis may outperform a larger low-mode basis at port mobility. That outcome must be measured, not presumed.

## Frequency-band contract

Declare a lower and upper band for mechanical loading, a possibly different band for radiation observation, and the excitation envelope. The upper retained mode is not automatically the upper valid frequency.

Evaluate error on logarithmic and locally refined frequency grids. Refine near resonances, antiresonances, and rapid phase changes. Compare complex mobility, not magnitude alone.

Use several metrics: absolute mobility error, relative error away from zeros, weighted matrix norm, phase error where amplitude is meaningful, and energy-transfer discrepancy under representative forces. Report worst-case force directions through singular-value analysis where practical.

A model can have small average error and a severe narrowband defect that changes one note’s decay. Preserve local maxima in the report. Do not hide them in a broad spectral average.

## Identified rational reduction

If starting from measured frequency response, fit a shared-pole multiport realization with reciprocity and passivity constraints. Stable poles alone do not guarantee passive interconnection.

Check the Hermitian part of mobility or impedance over the relevant axis and use an analytic passivity test appropriate to the rational form. Sampling alone can miss a narrow violation. A certificate or sufficiently justified global assessment is preferable.

When passivity enforcement changes residues, quantify the resulting fit degradation. A large correction suggests an inconsistent model, insufficient order, calibration error, or wrong port definition. It is not merely a numerical inconvenience.

SINTEF’s primary passivity documentation is a useful distinction between stable fitting and passive realization: https://www.sintef.no/en/software/vector-fitting/passivity/. Mechanical normalization must be supplied by this project.

## Preserve acoustic observations

A structural basis selected only for bridge mobility may poorly represent radiating surface motion. Include acoustic output directions or surface snapshots in basis selection where needed, while retaining a passive mechanical projection.

Observation maps can be reduced without becoming loading maps. Pressure at a microphone is not a power-conjugate port to bridge force. Do not impose positive-real constraints on an arbitrary force-to-microphone transfer; impose them on the physical load interface.

Keep structural loading and acoustic observation errors separate. An excellent pressure fit at one microphone can conceal an incorrect bridge impedance. Conversely, a mechanically accurate reduced model can need a richer radiation observation basis.

## State mapping and parameter changes

Reduced coordinates are basis-dependent. Switching assets without mapping states can inject energy or erase an existing decay. Define a projection from old physical state into the new basis and quantify retained/discarded energy.

If exact mapping is unavailable, use an explicit transition policy whose acoustic behavior and energy interpretation are documented. Do not call a crossfade a physical parameter evolution unless it has been modeled as such.

Interpolation between positive matrices can preserve positivity under suitable constructions, but interpolating mode frequencies, residues, and eigenvectors independently can violate consistency. Mode crossings and sign ambiguity make naive interpolation especially hazardous.

Treat a family of instruments as a parameterized model with validation, not as arbitrary interpolation of unrelated assets. Retain specimen identity and uncertainty through the family representation.

## Reduction test sequence

1. Freeze the accepted full model and validity envelope.
2. Define mechanical and acoustic output weights.
3. Build a baseline modal basis.
4. Add interface/static enrichment if needed.
5. Project mass, stiffness, damping, ports, and observations consistently.
6. Check positivity, symmetry, conditioning, and normalization.
7. Compare full and reduced frequency responses.
8. Compare transient energy exchange with coupled strings.
9. Test state mapping and parameter transitions.
10. Publish order-versus-error-versus-cost results.

Measure runtime cost only after correctness is established. Do not invent benchmark values; record actual hardware, compiler, configuration, and workload. The offline asset generator can allocate and solve large systems; the audio callback cannot.

## Acceptance and limitations

The gate requires passive loading, bounded output error, preserved relevant static behavior, verified transient coupling, and a stated extrapolation policy. A low CPU number alone is not a successful reduction.

Outside the validated band, the model must remain stable and passive even if accuracy is limited. High-frequency excitation from sharp contacts makes this requirement important: an inaccurate but passive tail is safer than an apparently accurate in-band fit with active extrapolation.

The acoustic volume will add cavity and radiation loading. Any reduction performed before that coupling must be rechecked after it, because added mass, damping, and hybrid modes can change which structural states matter.
