# P08 — Verify the elastic body and its reduced representation

**Status:** specification. **Prerequisites:** P07. **Exit evidence:** a converged,
traceable structural model and a reduced representation whose errors are bounded
for the intended ports and frequency range.

## Define the structural model before the mesh

Choose the continuum assumptions explicitly: shell, solid, beam, or a mixed
description; small or large displacement; prestressed or unstressed equilibrium;
linear or nonlinear constitutive behavior. The choice follows thickness, joint
behavior, loading, and observations rather than which element type is easiest
to click in a solver. Read [body modeling](../body/README.md),
[geometry](../geometry/README.md), and [numerics](../numerics/README.md).

Keep the structural and acoustic responsibilities distinct at this stage. A
structural displacement field is not a microphone signal. Its validity is
assessed through structural observations and work before P10 adds cavity and
radiation behavior.

## Entry routes and source map

The [source atlas](../research/source-atlas.md) records access, and
[Body 10](../body/10-synthetic-prototype-body.md) describes the actual executable
baseline. `BodyDesign` and `build_body()` in
[body_model.hpp](../../include/setar/instrument/body_model.hpp) and
[body_model.cpp](../../src/instrument/body_model.cpp) construct an analytic
rectangular orthotropic plate, a lumped cavity and a separate lateral oscillator.
They are not a full FEM/import pipeline. No specimen has supplied these defaults.

| Research question | Primary source and inspected/required location | Repository method chapter |
| --- | --- | --- |
| Which geometry and test boundary are being compared? | `R-SETAR-PEDRAMMEHR-2018`, [article](https://www.extrica.com/article/19505), §3 and §4–§5; `R-SETAR-MANSOUR-2011`, [author paper](https://caml.music.mcgill.ca/lib/exe/fetch.php?media=publications:caa2011_mansour.pdf), §3 | [Structural FEM](../body/04-structural-fem-and-mesh-convergence.md), [joints/preload](../body/02-joints-finish-and-preload.md) |
| What can support and sensor loading change? | `R-SETAR-MANSOUR-SCAVONE-2012`, [author paper](https://caml.music.mcgill.ca/lib/exe/fetch.php?media=publications:nantes2012_mansour.pdf), §3.1–§3.4 and §4.1–§4.2 | [Measured FRFs](../body/06-measured-frfs-and-modal-identification.md), [damping](../body/07-damping-and-causal-materials.md) |
| How should error and ambiguity be reported? | `R-METROLOGY-JCGM-100`, [official record](https://www.bipm.org/en/doi/10.59161/jcgm100-2008e), §§4–5/§7; `R-IDN-RAUE-2009` access-limited methods lead in the atlas | [Identifiability](../body/09-parameter-identifiability-and-experiment-design.md) |
| How does a smaller model preserve physical loading? | [NASA Craig–Bampton primer](https://femci.gsfc.nasa.gov/craig_bampton/Primer_on_the_Craig-Bampton_Method.pdf); [SINTEF passivity documentation](https://www.sintef.no/en/software/vector-fitting/passivity/), inspect the chosen method before use | [Normalization/tracking](../body/05-eigenmodes-normalization-and-tracking.md), [passive reduction](../body/08-passive-reduction-and-residual-flexibility.md) |

Begin numerical verification with an admissible synthetic geometry/material
package, explicit equations, ports and independent comparator. A specimen FEM
claim additionally needs P07's calibrated geometry, admissible material alternatives
and matched boundary state. A calibrated measured multiport FRF can support a
passive response model while wood inversion remains unresolved: document this
bounded alternative under `ENG-P08-T02`/`ENG-P08-T03`, preserve the still-open
`REQ-BOD-001` FEM gate, and make no unique wood/interior or untested-support claim.
No source's frequency table supplies modal masses, residues, damping partition
or radiation calibration by itself.

Each recipe produces an accepted, rejected, inconclusive or blocked finding with
raw/derived lineage, exact inspected source sections, comparator, uncertainty,
failure reasons and next owner. Proposed file paths below are future work; their
names and planned acceptance IDs do not imply existing code or passing tests.

## P08-T01: verify the element and assembly pipeline

Use independent elementary problems before the instrument geometry. Check rigid
translations and rotations, constant-strain or bending patch behavior where
appropriate, mass totals, and known beam/plate frequencies within each element's
assumptions. State the constraints and analytical reference; do not compare two
solvers that share the same undocumented boundary mistake.

Audit element orientation, numerical quadrature, thickness interpretation,
constitutive rotations, and assembly indexing. Check matrix symmetry and the
expected nullspace. A free body has rigid-body modes; eliminating them by adding
arbitrary weak springs changes the problem. If stabilization is necessary,
quantify its effect and keep it out of claimed physical damping.

Exercise deliberately invalid elements and materials. Negative Jacobians,
degenerate cells, nonpositive density, and inadmissible elasticity should produce
specific diagnostics before a modal solution is accepted. A solver's successful
exit code is insufficient evidence of an admissible model.

### Execution recipe

**Question and research.** Does the selected element implement the stated energy,
mass and boundary work before specimen uncertainty is introduced? This is
`ENG-P08-T01` / `REQ-BOD-001`. Read
[Body 04](../body/04-structural-fem-and-mesh-convergence.md) and
[the finite-element reference](../numerics/06-finite-elements.md); the latter's
Hermite beam is an elementary comparator, not a setar shell formulation. Use
`R-SETAR-PEDRAMMEHR-2018` §3 for the modeling questions, then inspect the selected
formulation's original derivation and official implementation documentation, such
as [MFEM Example 2](https://mfem.org/annotated/ex2/) for weak-form assembly.
Search `orthotropic shell bending patch test shear locking rigid body modes`;
record the element/version actually examined and do not infer verification from
software reputation.

1. Specify shell/solid/beam kinematics, tensor rotation, thickness integration,
   quadrature, consistent mass and supported coordinate units. For translational
   coordinates use metres, mass kg, stiffness N/m, body force N/m³ and traction
   N/m²; document separate rotation/moment units in mixed elements.
2. Derive expected strain energy and load work independently on an anisotropic
   coupon. Apply each normal/shear loading, reverse element orientation with the
   correct coordinate mapping, rotate the material frame, and compare against
   direct compliance calculations. Include distinct moduli so isotropy cannot
   hide an axis swap.
3. Verify rigid translations/rotations, constant-strain patches and supported
   bending patches appropriate to the element. Check free connected solid
   nullspace, matrix symmetry, total mass and force/moment resultants before and
   after constraints. Compare beam/plate frequencies with an independent analytic
   or converged different-formulation oracle using identical boundaries.
4. Deliberately supply inverted/degenerate elements, missing groups, invalid
   material frames and indefinite tensors. Reject before solving; inspect
   quadrature-point Jacobians, underintegration modes, shell locking and mixed-
   interface moment transfer rather than hiding defects with damping or springs.

**Retention and translation.** Deliver equations, element/version choice, fixture
manifests, independently derived expectations, matrices and residual/convergence
plots to proposed `src/body/offline_fem/`, `tests/body/fem_patch_tests.cpp` and
`experiments/mesh-convergence.md`. Present `build_body()` solves a small analytic
modal assembly; it does not assemble these elements. Its current
`body.plate_frequency_oracle` and `body.cavity_coupling_oracle` in
[body_tests.cpp](../../tests/instrument/body_tests.cpp) remain useful independent
synthetic checks with their existing scope.

**Independent gate.** Planned `fem.patch_test` and `fem.mesh_convergence` require
the separate weak-form/reference path, scale-aware residual tolerances derived
from precision/conditioning, and observable error trends. Reject wrong nullspaces,
nonphysical energy or a comparator sharing the implementation's element matrices.
Passing the present plate oracle cannot close either FEM acceptance ID. Hand the
verified element/assembly contract and unresolved formulations to P08-T02/T03.

## P08-T02: establish equilibrium and supports

Represent gravity, string loads, contact preload, and mounting conditions only
where they belong to the intended experiment. Reproduce the supports used in
the measurement session. A freely suspended modal test, a player-held instrument,
and a bench-mounted object have different boundary conditions.

If a prestressed analysis is needed, converge the static equilibrium first and
verify reaction forces and deformation. Linearize about that state using a
documented tangent operator. Do not add a geometric-stiffness correction with
an unexplained sign. Check that reducing the load approaches the corresponding
unstressed model and that the selected equilibrium is physically meaningful.

### Execution recipe

**Question and research.** Which equilibrium and support state produces the
tangent response being compared, and can boundary error masquerade as wood or
damping error? Use `ENG-P08-T01` / `REQ-BOD-001`,
[Body 02](../body/02-joints-finish-and-preload.md), `R-SETAR-MANSOUR-2011` §3
and `R-SETAR-MANSOUR-SCAVONE-2012` §3.1–§3.4/§4.1–§4.2. Extract neck restraint,
string vibration suppression versus retained preload, and sensing/support changes
as separate experimental variables. Search `prestressed modal analysis string
instrument bridge downbearing support compliance`; source conditions guide a
pilot and do not become this specimen's support constants.

1. Freeze the P07 geometry configuration, string tensions with uncertainty,
   bridge contact patches, gravity direction, nut/tail attachments, supports and
   sensor/cable masses. Identify whether the scan represents loaded or unloaded
   shape; avoid applying the same deformation twice.
2. Form each contact load by summing tension vectors pointing away along adjacent
   string segments. Use newtons and measured break angles, include balancing
   anchor loads, and check net force/moment. Full axial tension is not vertical
   bridge downforce. Model supports over their actual finite regions and retain
   compliance uncertainty instead of imposing an unexplained rigid clamp.
3. Solve the stated static residual to a budget below the displacement/force
   uncertainty, record reactions and pressure distribution, and verify the active
   contact set. A resting bridge must not need tensile normal contact. Investigate
   incompatible geometry or load signs before changing stiffness.
4. Linearize about this equilibrium with documented material, geometric, contact
   and follower-load terms. Independently compare tangent action with small
   positive/negative perturbations of the nonlinear residual at multiple sizes.
   Reduce loads toward zero and compare with the corresponding unstressed model;
   vary support within measured bounds to identify sensitive modes.

**Retention and translation.** Keep force diagrams with units, equilibrium mesh
and state hashes, support patches, residual/reaction histories, contact pressures,
tangent checks and a validity envelope for small vibration. Record accepted,
rejected and unresolved support hypotheses in proposed
`experiments/mesh-convergence.md`. Current `BodyDesign.bridge_mass_kg` supplies
inertia only, and `BridgeConfig` supplies dynamic string-to-body spring/damper
coupling; neither represents this preload solve. The proposed FEM must supply
`M`, tangent `K`, constraints and ports to P08-T03/T05.

**Independent gate.** A separate force/moment balance and tangent perturbation
calculation must agree within preregistered numerical/measurement budgets.
Stop on unstable equilibrium, unexplained negative stiffness, tensile resting
contact or a changing active set outside the declared linear domain. Present
`body.cavity_coupling_oracle` does not verify preload/supports; these remain part
of planned `fem.patch_test` / `fem.mesh_convergence` evidence. Deliver matched
boundary identities to P08-T04 and P09 so fitted losses do not absorb a different
test fixture. Missing safe loaded-state evidence keeps the specimen claim open.

## P08-T03: converge the observables that matter

Refine mesh and approximation order while holding the physical model fixed.
Compare not only natural frequencies but mode shapes, port residues, and bridge
mobility. A frequency can converge while a local force/velocity response near a
small footprint remains inaccurate. Treat the bridge loading patch as a
physical region, not a single convenient mesh node whose meaning changes under
refinement.

Track mode correspondence using appropriate shape/mass metrics rather than
frequency sorting alone. Near-degenerate modes may rotate within a subspace;
evaluate the subspace and its port response instead of declaring arbitrary
eigenvector differences a failure. Preserve sign-invariant comparison where
eigenvector sign is physically arbitrary.

Document the selected band and residual effect of omitted higher modes.
Convergence within that band does not justify unlimited sample-rate output.
Check static compliance and low-frequency response as well as resonance peaks;
a truncated model can reproduce visible peaks yet have incorrect off-resonance
behavior.

### Execution recipe

**Question and research.** Is numerical error small enough in the bridge response
and surface motion that a discrepancy can meaningfully test a physical hypothesis?
Use `ENG-P08-T01` / `REQ-BOD-001`,
[Body 04](../body/04-structural-fem-and-mesh-convergence.md) and
[Body 05](../body/05-eigenmodes-normalization-and-tracking.md).
Read `R-SETAR-PEDRAMMEHR-2018` §4–§5 for its comparison conditions, without
equating published frequency agreement with a converged local mobility.
Search `modal assurance criterion repeated eigenvalue subspace convergence
bridge mobility` and inspect the selected eigensolver's residual definition,
for example [SLEPc EPS](https://slepc.upv.es/release/documentation/manual/eps.html).

1. Freeze geometry, material alternatives, equilibrium, supports, damping and
   semantic force/response patches. Declare frequency band and separate budgets
   for numerical error, measurement uncertainty and model discrepancy before
   evaluating the decisive case. Preserve force resultant and moment as patches
   are remeshed; changing nearest nodes changes the problem.
2. Generate at least three systematically related discretizations to expose
   trends rather than a two-point coincidence. Refine size/order and, separately,
   geometry representation. Keep algebraic solver error below observed mesh
   differences, then tighten a representative solve to verify that assumption.
3. Export eigenpair residuals and mass normalization. Match modes through a
   common integration/sensor space with declared weights, and compare repeated
   clusters by subspace angles and aggregate port response. Eigenvector sign or
   frequency rank alone must not decide identity.
4. Compare static elastic compliance where defined, complex driving/cross mobility,
   residues, strain-energy localization and projected surface-normal velocity.
   Refine a common frequency grid around resonances and antiresonances; use
   absolute error near response zeros and phase only above a meaningful signal
   floor. For free-free models remove/project documented rigid modes before a
   static-compliance comparison; do not invert singular stiffness implicitly.

**Retention and translation.** Keep every mesh/matrix/configuration hash, port
resultants, eigenpair/subspace assignments, error curves, ambiguous matches and
out-of-band residual estimate in proposed `experiments/mesh-convergence.md`.
Current `body.plate_frequency_oracle` verifies independent finite-difference
convergence for the synthetic rectangle's first modes. Varying
`BodyDesign.coupling_basis_factor` or `max_frequency_hz` investigates its modal
basis/truncation, not a real-body FEM mesh. Do not attribute either experiment to
the other.

**Independent gate.** Planned `fem.mesh_convergence` needs stable target
observables within their allocated budget and an independently refined reference
or a justified analytic limit. Reject drifting port maps, unexplained missing
modes, unresolved nonmonotonic trends or apparent convergence dominated by
damping/frequency-grid smoothing. A further numerical refinement is meaningful
only for a concrete remaining error. Hand the accepted approximation and its
uncertainty to P08-T04/T05; retain measured validation as a separate comparison.

## P08-T04: identify and constrain losses

Introduce damping through a representation with stated assumptions and evidence.
Modal decay coefficients, proportional damping, and frequency-dependent
constitutive loss are not interchangeable. Compare their predicted decays and
frequency response over the intended amplitude and environment range.

Keep structural dissipation separate from acoustic radiation and joint losses
until their combined identification is defensible. Fitting total measured decay
as structural damping and then adding a radiation model can double-count energy
loss. Record what each coefficient includes and which conditions it was fitted
under. Reject negative dissipative parameters unless a genuinely active physical
subsystem has been explicitly modeled, which is not the default here.

### Execution recipe

**Question and research.** Which decay model explains calibrated motion in the
declared support/amplitude state without double-counting radiation or inventing
spatial wood losses? The crosswalk assigns this task to `ENG-P08-T02` /
`REQ-BOD-002`. Read [Body 06](../body/06-measured-frfs-and-modal-identification.md)
and [Body 07](../body/07-damping-and-causal-materials.md); use
`R-SETAR-MANSOUR-SCAVONE-2012` §3.1–§3.4/§4.1–§4.2 to design a support/sensor
loading pilot and `R-METROLOGY-JCGM-100` §§4–5 for its uncertainty. Search
`modal damping exponential window correction overlapping modes force velocity
FRF`; inspect the chosen estimator's assumptions before using its result.

1. Acquire authorized repeated force/motion records with calibrated gain, polarity,
   timebase and registered axes, maintaining tension/support/environment. Record
   safe excitation levels and reject double hits, clipping or support movement.
   Preserve raw and windowed data, force spectra, coherence and sensor-loading
   comparisons; high coherence alone does not establish gain or phase accuracy.
2. With `exp(i omega t)`, distinguish receptance `X/F` in m/N, mobility `V/F` in
   m/(N·s), and accelerance `A/F` in m/(N·s²). Fit complex channels jointly with
   common poles and physical residues, then compare time-domain decays. Keep
   whole sessions/ports for validation, not adjacent bins from the same fit.
3. For an underdamped viscous mode, `sigma = zeta*Omega` is amplitude-decay rate
   in s⁻¹ and `Omega` is undamped rad/s. Convert amplitude dB/s slope `b < 0`
   by `sigma = -b*ln(10)/20`; distinguish damped frequency and energy decay.
   Check beating, finite-window decay and noise-floor bias with independent
   known-pole synthetic records before interpreting nonexponential loss.
4. Compare effective modal damping with any justified Rayleigh/internal-variable
   alternative on held-out data. Identify which radiation, joint, support and
   fixture losses each coefficient already includes. If decomposition is
   unresolved, retain an effective total loss for that state, not a measured
   wood viscosity. A future explicit radiation load requires re-identification
   or an evidenced partition.

**Retention and translation.** Proposed `experiments/bridge-mobility.md` and
`tests/body/impedance_tests.cpp` receive calibration/FRF manifests, fitted poles,
uncertainty, decay/phase residuals and disposition of rival explanations.
`BodyMode.damping_ratio` maps to `sigma = zeta*2*pi*frequency_hz` in
[instrument.cpp](../../src/instrument/instrument.cpp). Present `build_body()`
projects damping diagonally and drops modal cross terms; a necessary
nonproportional or frequency-dependent law needs a proposed passive realization,
not independent guessed `zeta` values.

**Independent gate.** Present `body.mobility_reciprocity` and
`body.mobility_passivity` test synthetic modal mobility, including a sampled
full 8×8 Hermitian-matrix check in the current source; they contain no measured
calibration or decay evidence. For `REQ-BOD-002`, require reserved complex FRFs,
reciprocity within uncertainty and a realization-level passivity assessment.
Reject negative loss, phase corrections without calibration, or passivity
enforcement larger than the justified error budget. Send an accepted effective
model to P08-T05 even if wood identification remains inconclusive, preserving
its restricted band/state and open FEM gate.

## P08-T05: derive a port-preserving reduced model

Choose physical force/velocity ports before reduction. Normalize modes with the
correct mass convention and retain the corresponding force projection and
velocity observation. For a second-order structural model, the input and output
maps must preserve their power pairing. Arbitrary rescaling of one map changes
coupling even if isolated frequency plots look unchanged.

Compare the reduced model with the full model on impulses, sweeps, static or
quasi-static compliance where meaningful, and multiport reciprocity. Evaluate
truncation error at each intended bridge/cavity port, not just a global norm.
Any residual flexibility or high-frequency correction must have its own
stability/passivity argument and tests.

Store reduction provenance: full-model identity, basis, normalization, retained
band, damping treatment, input/output maps, and validation cases. A list of
frequencies without masses/residues and port definitions is not an installable
body model.

### Execution recipe

**Question and research.** Can fewer states reproduce the mechanical loading that
governs string decay and sympathetic transfer, with bounded off-resonance error?
Use `ENG-P08-T03` / `REQ-BOD-003`,
[Body 08](../body/08-passive-reduction-and-residual-flexibility.md) and
[modal reduction](../numerics/05-modal-reduction.md). Use
`R-SETAR-MANSOUR-2011` §3 to retain the measured port/boundary meaning;
inspect the NASA and SINTEF primary routes in the source map for the selected
projection or rational realization. Search `passive mechanical multiport reduction
residual flexibility Craig Bampton`. Methods provide a construction, not measured
setar residues or a universal retention cutoff.

1. Freeze either converged `M,C,K,B` with its physical domain or calibrated
   complex multiport FRFs with uncertainty and held-out channels. Define force
   exerted on the body in newtons along each declared positive port axis and
   conjugate velocity in m/s along that axis, giving input power `fᵀv`.
   Separate microphone/surface observation maps from
   this mechanical power pair.
2. For projection `q = V q_r`, form `M_r = VᵀMV`, `C_r = VᵀCV`,
   `K_r = VᵀKV`, `B_r = VᵀB` and `v = B_rᵀ qdot_r`. Verify positive mass,
   nonnegative stiffness on the intended elastic subspace and dissipation.
   For a measured route, fit shared poles and reciprocity, then establish
   passivity of the actual rational realization; stable poles alone are insufficient.
3. Record normalization explicitly. Current `BodyMode` uses modal coordinate in
   metres, mass in kg and dimensionless `bridge_participation[s][p]`; its
   second-order receptance numerator is `psi_c*psi_d/M` in kg⁻¹. A first-order
   pole residue divides this numerator by the Laplace denominator's derivative
   evaluated at that pole. Convert mass-normalized FEM shapes consistently
   rather than attaching an arbitrary mass to already scaled
   numerators. Rescaling a dimensionless shape by nonzero `a` requires mass by `a²`,
   participation/observation by `a`, and coordinate by `1/a`.
4. Sweep retained rank and interface/static enrichment. Compare full and reduced
   complex mobility, static elastic compliance, cross-port phase, impulses and
   integrated work using independent response calculations. Evaluate narrow
   resonances and zeros as well as averages; require passive residual flexibility
   and out-of-band behavior. Keep acoustic observation error separately bounded.

**Retention and translation.** Deliver source identity, basis/normalization,
port order, signed residues, retained/valid bands, damping partition, residual
realization, error-versus-cost and held-out results to proposed
`src/body/reduction.cpp` / `tests/body/reduced_model_tests.cpp`. Existing
`BodyConfig` in [config.hpp](../../include/setar/instrument/config.hpp) can carry
real independent underdamped modes across eight translational bridge ports.
It cannot automatically carry general complex modes, off-diagonal damping or
rotational ports. `radiation_weight` has units Pa·s²/m and multiplies modal
acceleration for observation; it supplies no acoustic feedback or calibration.

**Independent gate.** Planned `body.reduction_heldout_frf` and
`body.reduction_passivity` need a genuinely independent high-order/measured
comparator, reserved port/state prediction and a passivity argument plus numerical
checks. Present `body.mobility_reciprocity` and `body.mobility_passivity` verify
the synthetic sum; no full FEM reduction is qualified. Reject missing
masses/residues, one-sided map rescaling, unsupported state transfer, or corrections
that hide failed held-out phase. Deliver accepted passive assets to
P08-T06/P09/P13 and motion maps to P10; report unresolved wood interpretation
without blocking a justified response model.

## P08-T06: define safe asset loading

Design the reduced-model asset schema for offline validation and bounded runtime
use. Check dimensions, units, finite values, symmetry where required, mode counts,
energy-related positivity, version, and hashes before installation. Preparation
must leave the current valid model unchanged if a new asset fails validation.

Treat malformed or incompatible files as data errors with actionable messages.
Never deserialize arbitrary executable content. Keep expensive eigenanalysis,
mesh loading, and matrix factorization outside the audio callback. Document the
memory/cost envelope of the accepted reduced representation for P13.

### Execution recipe

**Question and research.** Can a reviewed body package reach processing without
changing its physical meaning, exceeding capacity or replacing valid state with
an invalid candidate? This links `ENG-P08-T03` / `REQ-BOD-003` and
`ENG-P13-T02` / `REQ-RTS-001` / `REQ-RTS-003`. Read
[modal preparation/runtime](../numerics/05-modal-reduction.md#preparation-and-runtime)
and [Body 08](../body/08-passive-reduction-and-residual-flexibility.md).
Revisit `R-SETAR-PEDRAMMEHR-2018` §3.2/Table 1 and the published-parameter audit
as a concrete prohibited promotion case, and `R-METROLOGY-JCGM-100` §7 for
metadata that must survive export. Search the selected serialization library's
official schema, bounds and versioning contract only when choosing an importer;
no dependency or format is assumed adopted here.

1. Specify a proposed immutable asset schema with version, model/source hashes,
   provenance sidecar, units, force/velocity frames, port ordering, normalization,
   damping semantics, mechanical and observation bands, residual realization and
   calibration/validation references. Declare which representation fits current
   `BodyMode` and reject unsupported representations explicitly.
2. Validate complete candidate data away from processing: finite positive masses
   and frequencies, permitted damping, array dimensions, signed finite maps,
   count/byte/work limits and energy/passivity conditions appropriate to the
   realization. Validate material provenance and quarantine disposition upstream;
   hashing data identifies bytes, not physical admissibility or permission.
3. Independently recompute selected mobility/work cases before and after
   serialization and sample-rate preparation. Check Hz versus rad/s, kg versus
   grams, port permutations and eigenvector sign/scaling applied consistently
   to every map. Account for the current preparation cutoff
   `min(max_mode_frequency_hz, 0.45*sample_rate_hz)`; a reduced retained set needs
   its own response-error check at each supported rate.
4. Design transactional publication: prepare and validate an entire candidate,
   adopt at a documented boundary, preserve the previous valid asset on failure,
   and reclaim old storage off the audio thread. Specify state projection and
   discarded energy, or a declared transition policy, before switching a ringing
   body. Test failed preparation and bounded-resource rejection as data cases.

**Retention and translation.** Store schema examples, rejection diagnostics,
round-trip traces, capacity sheet, ownership/state-transition diagram and error
reports with proposed `src/body/reduction.cpp` and
`src/engine/asset_publication.cpp`. Current
[config.cpp](../../src/instrument/config.cpp) validates constructed modal fields;
`Instrument::prepare()` builds banks/maps off the processing path. Neither is
the proposed file importer or transactional live publication system. The current
empty `BodyConfig.modes` meaning is rigidly grounded bridge contacts, so a failed
load must not silently turn into an empty body.

**Independent gate.** Present `body.invalid_design`, `realtime.allocation_hook`,
`realtime.max_work` and `realtime.capacity_overflow` test their synthetic/current
paths; they do not qualify asset handoff. Planned `realtime.asset_handoff`,
`realtime.telemetry_snapshot`, `body.reduction_heldout_frf` and
`body.reduction_passivity` require the new path's actual traces and independent
response checks. Reject unknown units/version, inconsistent normalization,
unsupported ports, capacity excess or changed live state on failure. Hand a
reviewable bounded loader/publication contract to P13, keeping runtime and
specimen validation statuses separate.

## Exit review

Require separate reviews of physical assumptions, numerical convergence, and
reduction accuracy. Attach actual mesh/shape plots with units, deformation scale,
boundary markings, and synthetic/measured labels. A dramatic modal animation
does not establish correct material properties.

The handoff checklist must distinguish the route that was actually completed:

- **Physical evidence:** specimen/configuration and support/preload identities,
  calibrated inputs, uncertainty, competing explanations and authorized asset
  use are traceable. An accepted measured FRF response does not resolve the
  geometric/material inverse problem by implication.
- **Numerical evidence:** independent element/equilibrium/convergence reports
  accompany an FEM route; a measured-response route retains the open FEM gate.
  Synthetic oracle results stay identified separately from either specimen route.
- **Reduced interface:** signed force/velocity ports, modal mass convention,
  damping partition, residual states, observation scaling and valid bands survive
  independent export/reimport and held-out response/work comparisons.
- **Runtime handoff:** accepted capacities, sample-rate truncation, installation
  failure behavior and state-transition policy are explicit. List actual executed
  checks and artifacts separately from planned acceptance IDs; retain every
  blocked or inconclusive scientific gate for P09, P10, P12 and P13.

> Construct and verify the specified body model without importing quarantined
> elastic constants. Demonstrate element-level checks, boundary equilibrium,
> mesh convergence, mode/subspace matching, and bridge-port response. Derive a
> reduced asset with explicit mass normalization and power-paired maps. Compare
> full and reduced responses, disclose omitted physics, and reject invalid
> assets before they can reach real-time processing.

Hand the validated port contract to P09 and the surface-motion representation
to P10. Preserve full-model fixtures so later optimization remains testable.
