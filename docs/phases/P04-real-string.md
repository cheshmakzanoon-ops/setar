# P04 — Add stiffness, loss, polarization, and justified nonlinearities

**Status:** specification. **Prerequisite:** P03. **Exit evidence:** each added
mechanism has independent verification, admissible parameters, and an explicit
validity range. “More complex” is not an acceptance criterion.

## Reading and physical scope

Read the [string volume](../strings/README.md) and the exact
[numerical schemes](../numerics/README.md) before implementation. Use verified
primary research for physical mechanisms and parameter-identification methods.
Separate a property of a particular string from a convenient approximation
used by a model.

The first stiff-string reference uses bending rigidity EI and linear density
mu. For simply supported endpoints, its partial angular frequencies satisfy
omega_n squared = (T/mu)(n pi/L) squared + (EI/mu)(n pi/L) to the fourth power.
This analytic case is a test fixture, not proof that actual nut, fret, and
bridge boundaries are simply supported.

## Entry evidence and source route

Enter with P03's accepted physical problem, independent oracle, force/work
contract and numerical bandwidth. Keep that baseline as an ablation for every
extension. The current synthetic prototype already contains stiffness, three
nonnegative loss coefficients and two modal banks per string; shared geometric
tension, dynamic longitudinal motion and torsion are not implemented.
Their appearance in this roadmap does not close a scientific gate.

| Primary route in the [source atlas](../research/source-atlas.md) | Inspect and extract |
| --- | --- |
| `R-NUM-BILBAO-SMITH-2005`, [author manuscript](https://www.pure.ed.ac.uk/ws/portalfiles/portal/11222313/acustica_final.pdf) | §II assumptions, §III scheme/energy and the appendix before transferring loss; distinguish linear, global-tension and discrete models. |
| `R-CONTACT-BILBAO-TORIN-2014`, [conference paper](https://dafx.de/paper-archive/2014/dafx14_stefan_bilbao_numerical_simulation_of_s.pdf) | §2 model and energy, §3 numerical construction; its examples are not measured setar coefficients. |
| `R-SETAR-MANSOUR-SCAVONE-2012`, [author paper](https://caml.music.mcgill.ca/lib/exe/fetch.php?media=publications:nantes2012_mansour.pdf) | §3.1–§3.4 and §4.1–§4.2 apparatus/support comparisons to design loading controls. |
| `R-METROLOGY-JCGM-100`, [official PDF](https://www.bipm.org/documents/20126/2071204/JCGM_100_2008_E.pdf) | §§4–5 and §7 for measurement equations, correlated uncertainty and reporting. |
| `R-IDN-RAUE-2009`, [publisher record](https://academic.oup.com/bioinformatics/article/25/15/1923/213246) | Obtain an authorized full method before reproducing profile likelihood; the atlas inspected only the abstract/§2.3 excerpts and records an access failure. |

For every task use the [finding](../../templates/research-finding.md) and
[parameter-promotion](../../templates/parameter-promotion.md) records. Retain
exact inspected locations, raw/derived identities, units, calibration, uncertainty,
independent comparator and actual commands/results. End with accepted for stated
scope, rejected with reasons, inconclusive when alternatives remain, or blocked
with the missing input. The
[acquisition campaign](../measurement/13-acquisition-campaign-execution.md)
routes empirical work through `strings.length_tension_density`,
`strings.stiffness_losses` and relevant bridge packets. Those packets are
currently blank; synthetic mechanism tests can proceed independently.

## P04-T01: stiffness and dispersion

Add bending stiffness without changing the existing flexible-string limit.
Derive the spatial operator and the required boundary terms. Verify units of
Young's modulus, area moment, and bending rigidity; wire diameter enters the
area moment strongly, so a diameter/radius mistake causes a large error.
Record how diameter and linear density are measured or specified.

Test zero-stiffness convergence to P03, exact stiff-mode frequencies, and boundary
conditions. Compare multiple resolutions across the usable partial band. Do
not compensate numerical dispersion by fitting a different physical stiffness
unless the approximation and compensation are explicitly separated. Otherwise
an inferred wire material becomes a hidden solver-error parameter.

If an all-pass or waveguide approximation is explored, compare its phase/group
delay to the same continuum target and preserve the reference model for
validation. That alternative is a separate decision, not an undocumented swap
inside the same class name.

### Execution recipe

**Question and mapping.** Is upper-partial sharpening physical bending, a
termination effect or numerical dispersion? These alternatives affect perceived
pitch, brightness and beating differently. Work under `ENG-P04-T01` and
`REQ-STR-003`. Read `R-NUM-BILBAO-SMITH-2005` §II,
`R-CONTACT-BILBAO-TORIN-2014` §2, and
[the physical specification](../strings/02-physical-specification.md).
Search `stiff string bending rigidity partial frequencies termination compliance`;
record assumptions before extracting a relation.

1. Freeze the P03 flexible fixture and independently derive
   `mu y_tt=T y_xx-EI y_xxxx` with simply supported conditions
   `y=y_xx=0`. Record `E` in Pa, `I` in m⁴ and `EI` in N m².
   For an explicitly homogeneous circular wire, `I=pi d⁴/64`;
   distinguish diameter from radius. A wound/composite string needs an effective
   model, not an unsupported use of that formula.
2. Substitute sine modes and compare `omega_n²=(T k_n²+EI k_n⁴)/mu` with an
   independently assembled eigenproblem. Start at zero rigidity, increase
   synthetic rigidity while holding all other physics fixed, and refine space
   and time separately using P03's limits. Inspect endpoint rows as well as
   frequencies; substituting clamped conditions changes the problem.
3. For future measured inference, obtain calibrated diameter/orientation repeats,
   mass per known witness length where already authorized, speaking span and
   termination records from
   [strings/setup measurement](../measurement/strings-frets-and-setup.md).
   Preserve stock-to-installed-string uncertainty. Compare multiple low-amplitude
   partials and known tunings/lengths; hold support, sensor, excitation position
   and observation definition controlled. Do not remove or cut strings under
   this roadmap instruction.
4. Fit and compare zero-rigidity, rigidity and plausible boundary-compliance
   hypotheses. Refine the solver before interpreting fitted `EI`. Separate
   numerical frequency error from measurement uncertainty and model discrepancy;
   the diameter contribution to rigidity uncertainty scales approximately as
   `4 u(d)/d` when other assumptions hold.

**Acceptance and translation.** Retain mode/residual tables, dispersion plots,
boundary alternatives and covariance. `core.frequencies`,
`numerics.dispersion_grid_refinement` and `preset.tension_inverse` are present
synthetic checks, not material identification.
`StringParameters.bending_stiffness_n_m2`, `StringGrid.bending_stiffness_n_m2`
and `StringConfig.bending_stiffness_n_m2` already exist; the work-order path
`src/strings/stiff_loss.cpp` is proposed. Accept the analytic limit within a
derived rounding/refinement budget; measured acceptance needs uncertainty-aware
held-out partials. Reject rigidity that compensates grid error or impossible
material inference, record unresolved boundary compensation as inconclusive,
and absent specimen measurements as blocked. Hand physical/numerical separation
to T02/T06, P06 termination work and P12 fitting.

## P04-T02: physical and empirical loss models

Classify losses by mechanism and evidence: air interaction, internal material
loss, termination loss, bridge/body energy transfer, and finger contact should
not all be folded into one exponential unless the limited model is declared.
Some loss mechanisms can be represented with causal frequency-dependent
operators or fitted modal decay rates. Identify which representation is being
used and how it will couple to the body later.

For the chosen damped PDE, derive the loss discretization together with the
wave update. A backward-time frequency-dependent loss term can change the
overall temporal order even when the wave terms are centered. Use the bound
and energy expression belonging to that exact scheme. Do not cite a centered
scheme's second-order accuracy for a different implementation.

Test nonnegative dissipation, analytic modal decay where available, the lossless
limit, and behavior at the parameter-domain edges. An experimentally fitted
negative damping coefficient must be rejected or interpreted as a failed
identification, not used to simulate an active string. Keep termination loss
separate enough to avoid counting it twice when body coupling is enabled.

### Execution recipe

**Question and route.** Which loss law predicts partial envelopes without
counting body leakage twice or mistaking numerical loss for material damping?
This controls sustain and the changing spectral balance of a decay. The crosswalk
maps this task to `ENG-P04-T01` and `REQ-STR-003`, not to the similarly numbered
nonlinearity work order. Read `R-NUM-BILBAO-SMITH-2005`'s loss appendix and
[the actual midpoint loss derivation](../numerics/14-damped-midpoint-loss-verification.md).
Search `string amplitude decay wavenumber squared stiffness proportional damping`.

1. Freeze the convention
   `q_n''+2 sigma_n q_n'+omega_n² q_n=0` with
   `sigma_n=a0+a2 k_n²+b2 omega_n²`. The respective coefficient units are
   s⁻¹, m²/s and s; `sigma_n` is the underdamped amplitude-envelope decay in s⁻¹,
   while modal damping ratio is `zeta_n=sigma_n/omega_n`. Positive decay in dB/s is
   `20 sigma_n/ln(10)` and `T60=ln(1000)/sigma_n` for an isolated exponential.
   Energy-envelope decay has exponent `2 sigma_n`. Use undamped angular
   frequency in the law, not the observed damped sinusoidal frequency. The current
   instrument applies this law to nut-to-tail basis modes with
   `k_n=n*pi/total_length_m`; bridge-coupled observed partials can mix these modes,
   so their decay is not automatically a direct observation of one `sigma_n`.
2. Test each coefficient alone, combinations, zero loss and a deliberately wrong
   sign/term. For the grid use the stated damping operator and its quadratic
   dissipation; compare simulated projected modes with independent Cayley roots,
   then with continuous decay under refinement. The current midpoint grid admits
   nonoscillatory modes; `reference::ModalString` rejects critical/overdamped
   retained modes. `Instrument::prepare` stops at the first `sigma_n >= omega_n`
   and rejects preparation if no mode remains. Record the retained count/cutoff;
   it does not simply skip that mode and resume above it. `LeapfrogStiffString`
   rejects all nonzero loss. For critical/overdamped grid cases compare the full
   response or poles `-sigma_n +/- sqrt(sigma_n²-omega_n²)`; a single exponential
   envelope with decay `sigma_n` is not the general expected response.
3. Verify the decay estimator on known damped pairs, changing gain, window,
   noise and nearby-mode interference. For authorized acquisition preserve raw
   motion, clock/gain calibration, support, pluck amplitude, bridge state and
   window/noise-floor decisions. Use the 2012 setar source's apparatus comparisons
   to plan loading controls; do not import its values as this string's loss.
4. Check identifiability before fitting: with `EI=0`,
   `omega_n²=(T/mu)k_n²`, so only `a2+b2 T/mu` is identifiable at one tension.
   More partials or changed length alone cannot remove this exact degeneracy.
   Prefer a reduced law or justified independent prior; changed tension can
   discriminate only if coefficient transfer across conditions is justified.
   Resolved stiffness-sensitive observations can help, but weak separation
   below uncertainty remains inconclusive.

**Evidence and handoff.** Retain per-partial decay/pole tables, estimator residuals,
loss allocations and alternatives. Present `numerics.loss_law_passivity`,
`numerics.loss_decay_estimator`, `numerics.loss_decay_oracle`,
`numerics.loss_modal_reference_crosscheck` and `core.wavenumber_loss` cover
synthetic cases. They do not identify measured intrinsic loss. Derive numerical
tolerances from estimator conditioning/refinement; use measured uncertainty for
empirical residuals. Reject negative admitted coefficients, unexplained gain or
body leakage absorbed twice. Accepted parameters map to `StringLossLaw` in
[config.hpp](../../include/setar/instrument/config.hpp) with a provenance sidecar;
unavailable measurements remain blocked. T06/P12 receive correlations and P09
receives the loss-versus-port-work separation.

## P04-T03: two transverse polarizations

Introduce two transverse displacement components with a declared physical basis.
For an ideal isotropic string with identical boundaries, rotating the basis
should not change observable energy or equivalent motion. Use that symmetry as
an independent test. Actual bridge/fret geometry can make the two directions
respond differently; document where the anisotropy enters.

Excite each direction separately and together. Verify that a change in pluck
direction maps into physically meaningful components rather than arbitrary
left/right stereo panning. Radiation is a later observation problem. Couple
the components only through explicitly modeled mechanisms; otherwise apparent
beating may be an accidental mismatch in duplicated parameter arrays.

Prepare the interface for directional bridge admittance. Force/velocity vectors
must use the same coordinate frame as the body ports. Test frame transforms
with orthogonal matrices and verify that dot-product power is invariant.

### Execution recipe

**Question and mapping.** Does changing pluck direction rotate mechanical motion
and delivered work correctly before directional bridge/contact effects are added?
This determines response to stroke angle and prevents artificial beating.
Use `ENG-P04-T02` and `REQ-STR-004`. Read
`R-NUM-BILBAO-SMITH-2005` §II–§III for energy assumptions, then
[two-axis numerics](../numerics/03-two-polarizations.md) and
[polarization hypotheses](../strings/04-polarizations-and-extensions.md).
Search `nonplanar string rotation covariance directional bridge mobility`.

1. Record an explicit basis map. Handbook notation may call lateral motion
   `y` and normal motion `z`; the current `Polarization` enum is
   `Vertical=0`, positive away from the soundboard, and `Horizontal=1`,
   positive toward string index 3. Store the transform to measurement/body
   coordinates, units and origin instead of copying array order from a diagram.
2. Construct an isotropic contact-free fixture with identical mass, tension,
   rigidity and loss in both axes and symmetric terminations. Excite each axis,
   then an oblique combination at fixed physical energy. Compare each with the
   independently verified scalar solution. Rotate state, force and observation
   together with an orthogonal matrix; transformed trajectories must agree and
   `F^T v` and total energy must remain invariant.
3. Add one declared anisotropy at a time, initially a known directional bridge
   or symmetric positive-semidefinite stiffness/damping matrix. Transform
   matrices as well as vectors. Compare eigenpairs and port power independently;
   positive diagonal damping alone does not prove full matrix passivity.
   Deliberate axis swaps, a force-only rotation and asynchronous updates must
   fail the covariance/work checks.
4. For empirical doublets, seek synchronized two-axis displacement/velocity with
   calibrated orientation and timing; compare pluck direction, low amplitude,
   neighboring-string damping and support conditions. The 2012 setar apparatus
   study supplies a loading-control method. A mono microphone doublet cannot
   alone distinguish polarization, neighboring strings, body modes or estimator
   leakage; preserve these alternatives and the original trial traces.

**Current boundary and acceptance.** `Instrument::prepare` in
[instrument.cpp](../../src/instrument/instrument.cpp) prepares two identical
modal banks per string; nail/bridge/body interactions can couple directions.
It has no shared nonlinear-tension term or arbitrary constitutive anisotropy.
`engine.lossless_energy_ledger` checks coupled synthetic work, but is not the
planned `numerics.polarization_decoupled_limit` test. Proposed
`src/strings/multi_axis.cpp` and its tests require an implementation decision.
Accept rotation/scalar agreement within a derived arithmetic and truncation
budget; reject basis-dependent physics. Retain frame matrices, axis energies,
mode/spectral plots and comparison results. Label unresolved doublets
inconclusive or unavailable directional data blocked. Hand accepted vector
ports to P05/P06/P09 and the common-axis energy contract to T04.

## P04-T04: nonlinear tension and longitudinal effects

Decide what observation requires additional nonlinearity: pitch rise at strong
excitation, amplitude-dependent frequency, polarization exchange, or another
measured effect. Write the associated assumptions about longitudinal motion,
quasi-static stretching, tension, and boundary work. Do not add nonlinear terms
only because high-fidelity instruments are expected to be complicated.

Derive the stored energy and gradient of the selected extension. Verify the
small-amplitude limit against the linear model. Use controlled amplitude sweeps
to compare predicted frequency shift and energy behavior. Distinguish an actual
amplitude-dependent frequency shift from estimator bias caused by a changing
spectrum or short observation window.

A nonlinear solver needs a bounded, observable convergence policy. Record
residual definitions, scaling, iteration limits, and failure behavior. The
offline reference may use robust implicit methods that are too expensive for
the eventual callback; that is acceptable if the interface and comparison plan
are explicit. Never silently continue with an unconverged iterate.

### Execution recipe

**Question and sources.** Does a repeatable amplitude-dependent pitch trajectory
require geometric stretching after contact, boundary and estimator effects are
controlled? Work under `ENG-P04-T02` and `REQ-STR-004`. Read
`R-NUM-BILBAO-SMITH-2005` §II–§III and
`R-CONTACT-BILBAO-TORIN-2014` §2–§3, then
[stretching equations N1–N11](../mathematics/04-nonlinear-stretching.md) and
[the tension experiment](../strings/05-nonlinear-tension.md).
Search `Kirchhoff Carrier string amplitude frequency shift axial boundary compliance`.

1. State the candidate reduction: fixed axial endpoints, small transverse slope,
   rapid axial equilibration, positive axial stiffness `EA` in N.
   For `r=(y,z)` derive
   `DeltaT=EA/(2L) integral |r_x|² dx` and
   `Vstretch=EA/(8L)(integral |r_x|² dx)²`. Both axes share the same tension.
   Keep externally commanded tuning, static fretting extension and dynamic
   extension separate; they have different work channels.
2. Independently differentiate the discrete potential using P03's derivative
   and quadrature. Verify its two-point gradient by
   `gradient_bar^T Deltaq=DeltaV`, including coincident states and rotated
   two-axis inputs. Ordinary midpoint applied to a quartic force does not
   automatically conserve this potential. For a pure fundamental sine in the
   ideal global-tension model, derive the reduced Duffing coefficient and use its
   weak-amplitude frequency shift as an independent comparator.
3. Sweep calibrated initial displacement or injected work, retaining slope,
   strain, tension and phase histories. Hold observation, mode count and
   force/contact history fixed. Compare against a separately converged offline
   solve; refine space, time and nonlinear residual separately. Verify vanishing
   amplitude and `EA -> 0` limits, and validate the pitch estimator on a
   known fixed-frequency signal with changing partial amplitudes.
4. Specify force-residual units, scaling and a work-defect bound
   `|r_force^T Deltaq|`, finite iteration capacity and rejected-step policy.
   Derive a proposed small-slope domain from the neglected arc-length terms;
   it is not a measured safe-playing limit. Dynamic axial waves require added
   longitudinal state, boundaries and a new stability/accuracy analysis.
   Admit that extension only if its predictions distinguish an unresolved
   observation from this quasistatic reduction.

**Decision and handoff.** Retain energy-gradient algebra, amplitude-frequency
plots, solver/refinement traces and all failed solves. Current
`StringConfig.tension_n` is constant during prepared string evolution;
`engine.lossless_energy_ledger` and `solver.discrete_gradient_identity`
verify existing mechanisms, not this proposed stretching law.
`numerics.nonlin_tension_work` remains planned. Accept the numerical mechanism
only within its derived error/domain budget; measured pitch-glide acceptance
needs T06 uncertainty and held-out conditions. Reject separate per-axis tensions,
double-counted fretting stretch or unconverged updates. Estimator ambiguity is
inconclusive; missing amplitude/axial data is blocked. Send the explicit equation
and proposed state/solver changes to P05/P06/P12/P13 without enabling a new claim.

## P04-T05: decide whether torsion belongs in scope

Torsional motion may matter for particular contacts or strings, but adding it
requires excitation, boundary, coupling, and measurement evidence. Define a
test that can distinguish its contribution from missing damping or contact
geometry. If there is no identifiable observable in the current domain, record
it as a research extension instead of adding uncalibrated degrees of freedom.

For any accepted torsional model, verify rotational inertia units, torque/angular
velocity power, wave speed, termination conditions, and coupling to nail/fret
friction. Do not connect a torque directly to a translational force port.

### Execution recipe

**Question and mapping.** Is there an observable contact response that a
translational model cannot explain and a torsional model can independently
predict? This might concern surface slip or an attack feature, but extra state
alone is not evidence of improvement. Use `ENG-P04-T02` and `REQ-STR-004`.
Read [the torsion decision section](../strings/04-polarizations-and-extensions.md).
Use `R-NUM-BILBAO-2009` via its
[publisher record](https://onlinelibrary.wiley.com/doi/book/10.1002/9780470749012)
as an authorized-copy methods lead; the atlas does not establish a reviewed
torsion derivation. Search `torsional string vibration friction contact surface velocity torque`,
retrieve an original derivation and record its exact inspected section before
adopting a model. Missing full text is a recorded gap, not an invented citation.

1. Define competing predictions before acquisition: torsion versus nail/contact
   width, surface friction, polarized translation, fret impacts and numerical
   aliasing. Specify a controlled change in contact orientation/lever arm and
   synchronized surface/contact observation that could separate them. Record
   string construction, radius, tension, support, amplitude, sensor loading and
   acquisition bandwidth. Existing nail/flesh and string packets provide a
   route, but torsional angle/torque channels would be a proposed addition.
2. If a model is justified, define angle `theta` in rad, polar inertia per
   length `Irot` in kg m, torsional rigidity `Ctor` in N m² and applied torque
   density in N m/m. Independently derive
   `Irot theta_tt=Ctor theta_xx` and energy
   `integral(Irot theta_t²+Ctor theta_x²)/2 dx`.
   For a declared homogeneous circular wire, `Irot=rho J` and `Ctor=G J`;
   these identities do not identify a wound string's effective properties.
3. Verify wave speed `sqrt(Ctor/Irot)`, termination eigenmodes, zero-torque
   response and finite-band convergence with an independent scalar-wave oracle.
   Couple through the same lever arm: `v_surface=r omega` and `tau=r F`,
   so `F v_surface=tau omega`. Include signs, rotation frame and any other
   translational work; mismatched lever arms must fail the energy check.
4. Compare torsion-enabled and excluded models on a reserved observable at
   matched translational/contact parameters. Derive tolerances from angular/
   timing calibration, estimator uncertainty and numerical convergence.
   If predicted differences lie below that budget or can be absorbed by
   unidentifiable friction parameters, retain the simpler model.

**Evidence and exit.** Record a justified inclusion or exclusion, source-access
log, sensitivity/ablation plots and the smallest missing measurement.
No current `StringConfig` field, instrument state or CTest implements torsion.
The planned `numerics.nonlin_tension_work` is not a torsion acceptance test;
new tests would need an explicit future registry change. Reject unit/power
inconsistency, mark nondiscriminating data inconclusive and unavailable evidence
blocked. An accepted exclusion is scoped to the tested domain, not proof that
torsion never matters. Hand the decision to P05 friction and P12 identifiability.

## P04-T06: identify parameters without confounding mechanisms

Use the measurement plans to isolate string properties before full-body fitting.
Record mounting compliance and environmental conditions. Fit partial frequencies
and decay envelopes with uncertainty and inspect residual patterns. A fit that
works for one note but predicts impossible material values or fails at another
length is not a verified physical explanation.

Run sensitivity experiments: vary one admissible parameter around its estimate,
observe which metrics change, and identify combinations that the current data
cannot distinguish. Report those identifiability limits to P12. Store parameter
uncertainty and provenance with the preset/model asset rather than as a detached
spreadsheet that cannot be traced to the engine.

### Execution recipe

**Question and route.** Which combinations of length, tension, density, rigidity
and loss are determined by observations well enough to predict another setup?
This prevents a convincing note fit from becoming a falsely physical preset.
Work under `ENG-P04-T01`/`ENG-P12-T01`, `REQ-STR-003` and `REQ-IDN-001`.
Read `R-METROLOGY-JCGM-100` §§4–5/7, the accessible material and access limits
for `R-IDN-RAUE-2009`, and
[string identification experiments](../strings/07-identification-experiments.md).
Search `stiff string tension rigidity identification partial covariance profile likelihood`;
obtain the full identification method before claiming its reproduction.

1. Freeze specimen/setup/session IDs, physical string identity, speaking and
   total lengths, support/bridge state, environment, calibrated observation axes
   and clock. Preserve raw traces, repeated setups, uncertainty/covariance,
   mode assignments and exclusions. Separate direct diameter/mass/length
   observations from inferred tension and fitted effective rigidity. Missing
   acquisition blocks empirical promotion while synthetic recovery proceeds.
2. Verify estimation with independent synthetic frequencies/decays, controlled
   noise and known wrong-mode/underdetermined cases. With independently known
   `L,mu` and simply supported undamped frequencies, regress
   `(2 L f_n/n)² = T/mu + [EI pi²/(mu L²)] n²`.
   Propagate shared length/clock errors and uncertain mode assignment;
   do not treat frequency peaks, damped frequencies and natural frequencies as
   interchangeable observations. Retain negative unconstrained slopes as
   diagnostics before imposing physical bounds.
3. Form scaled sensitivities of frequency, decay and directional response to
   candidate parameters; sweep finite-difference steps, inspect singular values
   and profiles while reoptimizing the other parameters. Fundamental pitch
   alone cannot determine `T,mu,L,EI`. Apply T02's exact flexible-string loss
   degeneracy: more partials at one tension do not identify all three
   coefficients. Choose identifiable combinations, a reduced law or an
   independently justified prior; disclose dependence on that choice.
4. Reserve an actual changed tuning, speaking length or session before fitting.
   Predict its partials and decay without refitting. Separate mounting
   compliance, body transfer, polarization beating and amplitude shift from
   intrinsic string parameters; changing stopped length also changes contact
   conditions. Rank the added measurement that most reduces a remaining
   ambiguity instead of fitting every subsystem to one microphone trace.

**Acceptance and model translation.** Retain fit residuals by mode/condition,
sensitivity/profile plots, covariance, rejected hypotheses and held-out results.
Numerical recovery tolerances derive from the generator/estimator budget;
empirical thresholds require measurement uncertainty plus a declared predictive
error allocation. `preset.tension_inverse` and the loss CTests verify synthetic
arithmetic, not the planned `identify.synthetic_recovery` or
`identify.sloppy_direction` requirements. Their general identification tools
under `tools/calibration/` remain proposed. Accepted values map to current
`StringConfig`/`StringLossLaw` with a versioned provenance sidecar;
speaking length maps to `bridge_position_m`, not `total_length_m`.
Reject impossible or resolution-dependent parameters. Mark correlated unresolved
families inconclusive and absent calibration blocked. P12 receives the identifiable
set, and P05/P06/P09 receive the boundary/loss assumptions they must preserve.

## Completion and implementation prompt

The component gate requires limiting cases, independent mode/decay tests,
energy/dissipation evidence, frame-invariance tests, convergence studies, and
parameter-domain rejection. Every extension must improve a stated prediction
or enable a later verified interaction; unused complexity should remain optional.

> Add only the specified physical extension to the verified string baseline.
> Explain the observation motivating it, derive its units and energy/work
> contribution, and identify the numerical scheme used. Preserve the previous
> limiting case. Add tests that fail if the extension is accidentally doubled,
> omitted, or has its sign reversed. Report identifiability limits and do not
> adjust body or output EQ to conceal a string-model discrepancy.

Handoff directional string ports to P05/P06 and P09. Handoff admissible parameter
ranges and unresolved identification questions to P12.

Before handing off, verify that each mechanism has its own bounded finding:

- Stiffness and loss preserve the P03 limits, boundary assumptions and numerical
  bandwidth; fitted coefficients do not absorb unresolved grid or bridge errors.
- Axis transforms, shared-tension assumptions and any torsional exclusion have
  explicit energy/work implications and discriminating tests.
- Accepted records retain SI units, covariance, calibration/source lineage and
  the exact configuration field; unknown physical inputs remain unknown.
- Actual CTests and proposed acceptance IDs have separate outcomes, including
  negative cases and missing evidence; synthetic success does not close validation.
- P05/P06/P09 receive ports and contact/boundary assumptions, P12 receives
  identifiable combinations and held-out decisions, and P13 receives the domain
  against which a faster approximation must be compared.
