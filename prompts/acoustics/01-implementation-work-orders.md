# Bounded body and acoustic work orders

## How to use these orders

Each order is a starting contract for a human or coding agent. Names of future interfaces are proposals, not claims that those APIs exist. Inspect current repository code and phase evidence before choosing paths. Keep the core C++20 and independent of plugin frameworks. Offline scientific scripts may use a reviewed numerical stack, but no dependency is assumed installed or licensed.

Do not execute all orders as one unreviewable change. Each should leave one falsifiable result, tests, provenance, and a clear next dependency. If required measurements are absent, implement a synthetic verification fixture and label it accordingly; do not invent specimen data.

### Order A — Reject inadmissible orthotropic input

Read the material audit and geometry orientation chapters. Implement a material-input validator whose accepted representation fixes the Poisson-index convention, engineering/Kelvin shear convention, SI units, and independent constants.

Inputs are synthetic valid and invalid records plus a quarantined published-value test fixture. Preserve source values unchanged. Construct the normal compliance block, derive reciprocal partners only from an explicitly selected independent set, and check completeness, symmetry, positive energy, and conditioning.

Return structured diagnostics identifying the field and failed relation. Do not silently symmetrize inconsistent data or infer missing shear moduli through isotropic formulas. A caller can create a separate corrected hypothesis only with explicit provenance.

Tests include distinct anisotropic moduli, an isotropic limit, negative shear stiffness, a reciprocal sign conflict, a nearly singular normal block, unit conversion, and a rotated energy check. Compare direct compliance multiplication with an independent small linear solve.

Acceptance evidence is a test report and a short audit showing that the published inconsistency is detected for the stated convention. Passing this order verifies ingestion mathematics, not mulberry or walnut properties. Next work is coupon FE verification, not immediate specimen fitting.

Primary reading: USDA Chapter 5, equation 5–1, and the source-audit chapter’s exact publication pointer.

### Order B — Preserve semantic ports through remeshing

Read the specimen-coordinate and CAD-topology chapters. Implement an offline mapping from semantic contact patches to mesh interpolation/load-distribution operators. Inputs include an asymmetric synthetic surface, named patches, local axes, and several mesh resolutions.

The map must preserve the intended resultant force, moment about a declared origin, and work pairing. Use the same displacement interpolation and conjugate force transpose, including quadrature weights where required. Reject missing or ambiguous patch labels.

Do not choose the nearest node without a documented approximation. Store the physical patch definition and the generated numerical mapping separately. Export local-to-global transforms with handedness checks.

Tests translate and rotate the entire fixture, change node order, refine the mesh, and reverse one normal deliberately. Work and physical resultants should remain invariant under legitimate transforms; the malformed normal case should fail.

Acceptance requires a report of port locations, axes, resultants, moments, and refinement changes. No setar measurement is needed for this synthetic mapping test. The next dependency is structural matrix assembly using the same stable semantic ports.

### Order C — Establish an orthotropic FE reference

Read structural FEM, material admissibility, and eigenmode chapters. Choose one supported FE backend after verifying availability and license. Implement a small anisotropic coupon and plate reference before introducing instrument geometry.

Assemble mass and stiffness with explicit quadrature, material axes, constraints, and SI units. Export matrices and degree-of-freedom maps. Begin lossless so damping cannot conceal defects.

Verify rigid translation/rotation, uniaxial compliance, shear response, force/moment balance, symmetry, and positive energy. Use a rotated anisotropic specimen to reveal axis-order mistakes. Include at least three systematically related meshes and compare static compliance, frequencies, and port residues.

Acceptance is an independent numerical comparison with tolerances justified by precision and convergence. Do not claim validation from agreement between two scripts sharing the same element formula. A second formulation, analytic result, or independently derived matrix fixture is required.

The next dependency is a frozen structural reference package. A visually correct mesh or successful solver exit is not the output.

Primary implementation references are MFEM Example 2 or DOLFINx’s elasticity demonstration, used with the project’s additional orthotropic tests.

### Order D — Normalize and track modes

Read the eigenmode chapter. Build an adapter that imports M, K, constraints, and mode vectors, then emits mass-normalized modes and verification metadata.

Do not assume the solver’s default normalization. Compute modal mass, residuals, mass orthogonality, and projected port coefficients independently. Keep visualization scaling separate.

Implement matching across two meshes or parameter samples using a shared observation space and a global assignment with ambiguity reporting. For repeated clusters, compare subspaces rather than forcing individual vector identities.

Tests include sign reversals, coordinate permutations, repeated eigenvalues, near crossings, missing modes, and a vector with a deliberately corrupted component. Verify that basis-invariant response remains unchanged when a repeated subspace is rotated.

Acceptance includes residual tables, normalization matrices, matching decisions, and a reconstructed FRF comparison. The output must remain meaningful if an alternative eigensolver chooses different signs.

Next work is measured-shape comparison through actual sensor operators. Do not use high MAC as evidence of correct absolute coupling.

### Order E — Identify a reciprocal multiport FRF model

Read the measured-FRF chapter. If real calibrated data are absent, generate a clearly synthetic passive two- or three-port reference and contaminate it with declared noise and calibration perturbations.

Fit all channels with shared poles. Preserve complex phase, row/output and column/input ordering, and physical units. Include a controlled residual-flexibility term when the finite fitting band requires it.

Compare recovered poles, residues, cross-channel signs, and static behavior against the hidden synthetic truth. Deliberately fit each channel independently as a diagnostic comparison, not as the accepted architecture.

Hold out complete transfer channels or excitation directions. Estimate uncertainty from repeated synthetic datasets while separately exposing systematic gain/phase errors. Test reciprocity before imposing it, and report how passive enforcement changes fit error.

Acceptance requires correct physical scale and held-out multiport prediction, not just resonance frequencies. The next dependency is a passive load realization compatible with string ports.

If measured data are used, require calibration and permission records. This order does not authorize new physical measurements or redistribution.

### Order F — Prove string/body work exchange

Read the bridge chapter and the numerics energy-port contract. Implement a synthetic string endpoint coupled to a small passive body using the approved synchronized time discretization.

Begin with one body oscillator and then two strings sharing it. Use identical port force and conjugate velocity at the same work time level. Record string, body, damping, and external-input energy separately.

Compare against an independently assembled small coupled linear system. Test rigid termination, heavy-body limit, weak coupling, lossless exchange, damping, and symmetric/antisymmetric string combinations.

Add a deliberately delayed feedback version only as a negative or comparison fixture. It must not inherit the passive claim of the synchronized version. Quantify its stability and energy defect if retained for research.

Acceptance includes coupled poles, energy residual convergence, cross-string transfer, and block-size independence. A one-way body postfilter does not satisfy this order.

Next work adds bridge inertia and contact only after the linear connection is verified. Do not combine nonlinear friction with an unproven linear port map.

### Order G — Verify a cavity/structure interface

Read the cavity chapter. Implement a rigid rectangular cavity and a single moving piston or elastic boundary with explicit normals. Use either a first-order acoustic formulation or a pressure-only formulation with a separately derived physical energy reconstruction.

Construct the interface matrix from shared quadrature. Apply its conjugate transpose consistently. Record pressure force in N, normal velocity in m/s, and volume flow in m\(^3\)/s.

Tests include constant pressure on a closed surface, uniform piston volume flow, rigid-cavity eigenfrequencies, closed lossless energy exchange, and passive wall loss. Reverse a normal deliberately and require detection.

Compare the long-wavelength coupled response with an independently derived lumped mass–spring–acoustic-compliance model. Do not delete the constant pressure mode without explaining whether mean compression is physically constrained.

Acceptance is work cancellation and converged coupled response. The next dependency is an opening with a defined exterior termination. A zero-pressure opening remains an explicit comparison boundary, not a claim of realistic radiation.

### Order H — Compute passive exterior radiation

Read the exterior BEM chapter and the selected backend’s exact operator documentation. Implement a pulsating sphere before instrument geometry.

Match harmonic convention, Green-function sign, and boundary normal. Compute surface impedance, pressure at several radii, radiated power, and a generalized radiation matrix for multiple surface patterns.

Test low and high dimensionless frequency, near-boundary observation quadrature, matrix reciprocity under proper weighting, and nonnegative radiation resistance. Investigate formulation irregularities near known interior resonances.

Use dense assembly first if practical. Add acceleration only after the baseline passes, then quantify its additional approximation error.

Acceptance includes analytic amplitude/phase agreement, boundary-to-far-field power balance, and convergence in mesh and quadrature. Do not accept magnitude-only agreement.

Next work couples aperture and structural source bases. A microphone transfer is not a substitute for the loading matrix.

Primary readings are Bempp’s Boundary Operators and Potential Operators sections; verify installed APIs rather than assuming examples from another version run unchanged.

### Order I — Qualify an absorbing time-domain boundary

Read the FDTD/PML chapter. Implement a single documented acoustic update and one absorber formulation. Keep source, physical domain, and PML regions explicit.

Derive the actual stability bound and staggered energy. Test dispersion along multiple grid directions before adding geometry. Validate source normalization with an analytic field.

Measure reflected waves against a larger-domain reference across frequency, incidence angle, and PML parameters. Include grazing incidence, corners, low-frequency content, and late-time zero-input evolution. Do not place sources or sensors inside the absorber.

Tests must distinguish physical absorption from numerical boundary removal. Record energy in auxiliary variables where the chosen formulation requires it.

Acceptance is a bounded reflection map and no unexplained long-time growth. An expanding-wave animation is not evidence. Next work compares with BEM on the same physical source and geometry.

If k-Wave is selected as an independent reference, identify it correctly as a k-space/pseudospectral route and verify its environment and licensing.

### Order J — Calibrate a pressure observation

Read the microphone/directivity chapter. Implement a virtual observation map with explicit coordinates, orientation, propagation delay, and Pa-to-digital gain.

Use a synthetic monopole and directional pattern to verify \(1/r\) behavior, phase delay, angular quadrature, and power integration. Distinguish near-field evaluation from far-field asymptotics.

Add a microphone calibration transfer only when its provenance and field type are known. Keep room response separate. A test normalization gain must be stored and must not overwrite the calibrated render.

Hold out microphone positions and directions from interpolation. Test wrapped-phase behavior, channel permutations, and frame rotations. Require failures for missing units or ambiguous acoustic centers.

Acceptance includes calibrated pressure, spatial prediction, and correct digital scaling. The next dependency is a runtime observation realization that preserves these properties.

This order does not authorize purchasing microphones, entering a laboratory, or publishing recordings. If measurements are unavailable, the output remains synthetic observation verification.

### Order K — Reduce load and observation separately

Read passive body reduction and radiation-runtime chapters. Start from one accepted full reference package, not an unverified frequency table.

Build a congruence-projected mechanical model with consistent ports. Add static/interface enrichment when modal truncation misses low-frequency compliance. Independently compress the pressure observation while retaining phase and delay.

Fit a passive load realization and a causal stable observation. Do not impose scalar entrywise passivity corrections on a matrix. Check extrapolation outside the fitting band.

Compare full and reduced responses near resonances, antiresonances, and cross-port cancellations. Test simultaneous inputs and long zero-input decay. Evaluate state mapping when switching assets or sample rates.

Acceptance requires separate mechanical-loading and acoustic-observation error reports plus bounded runtime work. CPU benchmarks must be measured with hardware/compiler/workload metadata, not inferred from mode count.

Next work integrates the immutable asset into the approved runtime lifecycle. Fitting and allocation remain outside the audio callback.

### Order L — Build an auditable exchange adapter

Read the solver-exchange chapter. Implement one importer/exporter pair with a versioned manifest, exact units, coordinate maps, constraints, and provenance.

Start with the synthetic two-coordinate fixture in that chapter. Reproduce its static compliance, 0.125 J elastic energy, 0.33 J kinetic energy, and power balance after round trip.

Test one-based sparse indices, symmetry storage, duplicate entries, complex conventions, nonfinite values, wrong dimensions, unsupported schema, and a coordinate permutation. Malformed files must fail with actionable diagnostics, not guessed defaults.

For a solver-native format, verify the actual version’s scaling and transposition behavior. Store neutral matrices and maps alongside native projects when allowed.

Acceptance is reproduction through an independent reader. A matching checksum alone proves identity, not interpretation. The next dependency is a complete evidence package for one accepted reference model.

## Review prompt for every completed order

Review the change as a skeptical scientific maintainer. Identify the exact physical claim, evidence class, units, sign conventions, validity range, and independent reference. Check whether tests could pass with a wrong normalization, swapped axis, omitted area factor, or hidden output normalization.

Separate mathematical verification from specimen validation and real-time qualification. List missing evidence without converting it into a fabricated result. Confirm that no recording became a note-playback engine, no proprietary dependency was assumed available, and no unbounded work entered the audio callback.

Return a concise acceptance decision with reproducible commands and artifacts. If the order fails, preserve the failed case and state the smallest next investigation. Do not broaden the implementation automatically to make a demo appear complete.
