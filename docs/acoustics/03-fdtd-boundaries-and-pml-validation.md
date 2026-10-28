# Acoustics 03 — Time-domain air grids, boundaries, and PML verification

## Objective and implementation boundary

This proposed, unimplemented P10 route provides an independent acoustic reference and time-domain diagnostic. Inputs are a controlled geometry, air properties, prescribed or coupled boundary motion, and a spatial/temporal accuracy target. Outputs are pressure and velocity fields, energy ledgers, boundary-reflection measurements, convergence records, and comparisons with analytic and BEM fixtures.

A three-dimensional air grid is not assumed suitable for the real-time audio callback. It can be an offline reference used to generate and validate reduced radiation assets. Real-time use would need its own measured cost and accuracy evidence.

Start with acoustics alone and prescribed moving boundaries. Coupling a complex elastic instrument before the air solver passes its own tests makes errors difficult to localize.

## Baseline first-order grid

Use pressure at cell centers and particle-velocity components at staggered faces. A conventional leapfrog update stores pressure at integer times and velocity at half times:

\[
\rho_0\frac{\mathbf u^{n+1/2}-\mathbf u^{n-1/2}}{\Delta t}
=-\nabla_h p^n,
\]
\[
\frac{p^{n+1}-p^n}{\rho_0c^2\Delta t}
=-\nabla_h\cdot\mathbf u^{n+1/2}.
\]

Define gradient and divergence as compatible discrete operators with volume and face weights. Their summation-by-parts relation is the discrete counterpart of the continuous energy identity. Independently chosen stencils can break this relation at boundaries.

Do not call stored half-step velocity simultaneous with integer-time pressure. Intensity and interface work need consistent temporal interpolation or a derived staggered energy expression. The numerics volume’s port contract applies here as it does to strings.

## Stability and dispersion

For the standard homogeneous rectangular-grid second-order scheme, a familiar CFL condition is

\[
c\Delta t\sqrt{\Delta x^{-2}+\Delta y^{-2}+\Delta z^{-2}}\le1.
\]

This formula is not universal. Higher-order stencils, variable coefficients, cut cells, damping discretization, and boundary coupling can change the admissible step. Derive or bound the actual operator’s spectral radius and test the complete update.

Use a strict stability margin, not equality rounded by floating-point arithmetic. Stability is separate from dispersion accuracy. A stable grid can have unacceptable phase error that changes interference and directivity.

Measure numerical phase velocity along axes, face diagonals, and body diagonals. Grid anisotropy can make a physically symmetric source appear directional. Report dispersion across the retained frequency band, not only at one long wavelength.

## Source injection

Distinguish an initial pressure condition, a volume source, a prescribed velocity boundary, and a force coupled through a structure. They have different units and energy injections.

For a volume-flow source distributed over cells, normalization must preserve integrated volume flow as the grid changes. A fixed numerical pressure increment at one cell does not represent the same physical source after refinement.

Record source work in the energy ledger. A hard source that overwrites a state can inject or remove energy in a way that depends on the existing field. It may be useful for a benchmark, but its physical meaning must be explicit.

Test a source in an otherwise uniform domain against an analytic monopole or another appropriate reference. Verify amplitude scaling with distance, phase delay, and total radiated power.

## Geometry on a Cartesian grid

Staircase boundaries can shift cavity volume, opening area, and wall orientation. Refinement may reduce these errors slowly or nonmonotonically when a small hole changes voxel connectivity.

Use a geometry convergence test separate from field convergence. Track volume, surface area, opening area, and normal distribution. A soundhole represented by one or two cells is not a trustworthy geometric model merely because the time step satisfies CFL.

Cut-cell or immersed-boundary methods can improve geometry, but require careful stability and conservation treatment. Very small cut cells may impose restrictive steps or need a validated stabilization. Do not merge cells or modify volumes silently.

For a thin soundboard, decide whether it is a zero-thickness moving boundary, a resolved solid layer, or a coupled surface. A voxelized thick wall can distort cavity volume and acoustic path length. Each representation needs a declared band of validity.

## Passive wall boundaries

A rigid wall has zero normal particle velocity. A prescribed moving wall injects work \(p v_n\) with the appropriate domain sign. An impedance wall requires a causal relation between pressure and normal velocity and a nonnegative absorbed power.

A frequency-independent resistive boundary is a useful test, but a real wall or radiation boundary can have reactive and frequency-dependent behavior. Fit a passive rational impedance if memory is needed.

Avoid applying impedance by simply multiplying pressure by a decay factor at boundary cells. That may alter the wave equation without the intended physical reflection coefficient. Verify the actual numerical reflection against the target impedance for several incidence angles.

## Absorbing boundaries versus physical loss

An exterior truncation boundary is a numerical device. Its absorbed energy must not be interpreted as material damping or measured radiation loss without accounting for the domain energy and flux.

A sponge layer can reduce reflections but is not automatically perfectly matched. A PML is derived through a modified system or coordinate stretching. Its continuum matching property does not guarantee reflection-free discrete behavior.

Label absorption in the ledger as physical fluid loss, wall loss, or numerical exterior removal. This prevents a later calibration from treating PML tuning as a sound-quality parameter.

## PML implementation discipline

Implement one documented PML formulation completely, including auxiliary variables, damping profiles, staggered placement, and corners. Do not mix equations from incompatible split-field and unsplit formulations.

Check how damping is integrated in time. Explicit damping updates can impose extra restrictions; exponential or centered updates have different properties. The stability of the full coupled scheme must be established, not inferred from the undamped CFL bound.

The primary paper “Energy Decay and Stability of a Perfectly Matched Layer for the Wave Equation” proves results under specified assumptions, including constant damping in the stated analysis: https://doi.org/10.1007/s10915-019-01089-9. Its conclusions must not be generalized automatically to arbitrary graded, cornered, discrete implementations.

## PML placement and parameter tradeoffs

Place sources, structures, and observation points outside the absorbing region. Leave enough physical-domain separation for near fields and geometry representation. A PML intersecting a source can alter the source itself rather than merely absorb outgoing waves.

Thickness, grading, and maximum damping interact. Excessive damping over too few cells can increase reflection. Very weak damping can allow waves to reach the outer boundary. Optimize against a measured reflection metric rather than selecting the largest coefficient.

Test low frequencies, grazing incidence, corners, and broadband impulses. A PML that performs well for an axis-aligned plane wave may fail for tangential propagation or long-time low-amplitude fields.

k-Wave’s primary “Controlling the Absorbing Boundary Layer” example illustrates parameter effects: https://www.k-wave.org/documentation/example_na_controlling_the_pml.php. k-Wave uses a k-space/pseudospectral approach in its documented first-order solvers; it is an independent alternative, not the same finite-difference scheme described above.

## Reflection measurement

Use a reference domain large enough that returning boundaries cannot affect the observation window. Compare a smaller PML-terminated domain against it. Separate incident and reflected components using controlled plane-wave fixtures or time separation where valid.

Report reflection as a function of frequency and angle, with an explicit denominator and noise floor. A single peak-amplitude number can hide a narrowband failure. Include the time at which boundary contamination first becomes detectable.

Do not use a short time window to claim excellent low-frequency absorption when the window cannot resolve those frequencies. State the bandwidth supported by the measurement duration.

## Long-time and zero-input tests

After a finite excitation stops, run the homogeneous system and inspect energy. Passive physical losses and a stable absorber should not create growing energy. A late-time instability can be far below audible level initially and still invalidate a runtime asset.

Test zero initial conditions, random small fields, concentrated near-boundary fields, and states in PML auxiliary variables. The latter catches initialization bugs that a pressure-only impulse may miss.

Check finite precision and denormal behavior separately from physical decay. Do not add an arbitrary noise floor or state clipping to conceal instability. A deliberate tiny-state flush policy belongs to runtime engineering and must have a bounded error analysis.

## Pressure/velocity observation alignment

Pressure and velocity live at different positions and times. Interpolating them for intensity requires both spatial and temporal alignment. A naive product can produce incorrect power even when each field looks plausible.

The k-Wave “Recording the Particle Velocity” example documents staggered output locations and times: https://www.k-wave.org/documentation/example_ivp_recording_particle_velocity.php. Use it as a reminder to inspect actual data layout, not as permission to copy another solver’s correction formula blindly.

For microphone pressure, interpolate from the pressure field with a documented spatial kernel. For velocity or intensity probes, state the interpolation and its frequency response. Compare on-grid and off-grid observers against analytic fields.

## Cross-method verification

Compare the pulsating sphere or a baffled piston against analytic solutions and BEM. Compare a rigid cavity against analytical modes and cavity FEM. Use the same physical source and units, not superficially similar numerical excitations.

Disagreement can arise from geometry approximation, source normalization, time convention, numerical dispersion, or boundary reflection. Change one cause at a time. A convergence table with separate columns for each mechanism is more informative than a single “FDTD error” label.

Do not commit an inverse crime by generating synthetic validation data with exactly the same grid and solver used for identification. Use independent discretization or analytic data where possible.

## Task sequence and gate

1. Derive compatible discrete operators and energy.
2. Verify plane-wave dispersion and stability.
3. Verify physical source normalization.
4. Verify rigid and resistive wall reflections.
5. Implement and test the chosen PML.
6. Quantify geometry errors.
7. Validate pressure/velocity observations.
8. Compare analytic, BEM, and FEM fixtures.
9. Add structural coupling with synchronized work.
10. Export reduced-transfer candidates only after convergence.

Acceptance requires stable long-time behavior, bounded reflection, accurate phase, correct energy injection, and geometry convergence. A visually smooth expanding wave is not enough. The runtime handoff should contain fitted physical transfer models and their uncertainty, not an unexplained impulse response obtained from an unverified absorbing box.
