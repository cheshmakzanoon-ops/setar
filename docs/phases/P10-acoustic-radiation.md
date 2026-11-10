# P10 — Cavity, sound radiation, and the observation model

**Status:** specification. **Prerequisites:** P08; P09 supplies coupled structural
motion. **Exit evidence:** an acoustic observation model with verified units,
boundary conditions, coupling work, and a measured or explicitly synthetic
validity domain.

## Mechanical motion is not yet recorded sound

A displacement pickup at one string point is useful for numerical development.
It is not what a listener hears in front of a setar. Sound production involves
surface motion, cavity behavior, openings, surrounding air, and observation
position. This phase specifies which of those interactions are solved, reduced,
or deliberately omitted. Read [acoustics](../acoustics/README.md),
[body](../body/README.md), and [validation](../validation/README.md).

Keep the instrument's radiation model separate from an artistic room effect.
Adding reverberation can improve a demonstration while making a measurement
comparison less interpretable. The core validation path needs controlled
observation conditions before optional presentation effects are considered.

## Entry routes and source map

The [current synthetic body](../body/10-synthetic-prototype-body.md) already
contains a lumped plate/Helmholtz coupling through `build_body()` in
[body_model.cpp](../../src/instrument/body_model.cpp). It does not solve a
spatial cavity or exterior sound field. `BodyMode.radiation_weight` in
[config.hpp](../../include/setar/instrument/config.hpp) has units Pa·s²/m and
maps modal acceleration to output pressure. That observation is one-way;
it adds no exterior acoustic feedback. Frequency-independent weights do not
implement general frequency-dependent radiation, propagation or microphone
transfer. Declared modal damping currently includes an unseparated radiation-loss
allowance, which must be revisited before introducing an explicit radiation load.

| Research question | Primary source route and access boundary | Project route |
| --- | --- | --- |
| Which exterior formulation handles these surfaces and holes? | `R-ACO-DELEON-2008`, [author-laboratory thesis](https://caml.music.mcgill.ca/lib/exe/fetch.php?media=publications:ma_deleon_2008.pdf); inspect §§2.1–2.2, §3.1.2, §3.2 and §4. The atlas records only abstract/contents/§1.4 inspection and flags limited perforated-boundary treatment. | [BEM/loading](../acoustics/02-exterior-bem-and-radiation-impedance.md), explicit cavity/exterior aperture continuity |
| Which variables and discrete work are being checked? | `R-NUM-BILBAO-2009`, [publisher record](https://onlinelibrary.wiley.com/doi/book/10.1002/9780470749012); obtain an authorized copy and record actual sections before attributing a derivation. | [Cavity/interface](../acoustics/01-cavity-and-structural-acoustic-coupling.md), [FDTD/PML](../acoustics/03-fdtd-boundaries-and-pml-validation.md), independent analytic fixtures |
| Which measured pressure can be compared with a model? | `R-METROLOGY-JCGM-100`, [official record](https://www.bipm.org/en/doi/10.59161/jcgm100-2008e), §§4–5/§7; [NIST free-field calibration](https://www.nist.gov/publications/importance-free-field-calibration-microphones) and [complex microphone calibration report](https://www.nist.gov/system/files/documents/calibrations/eacous.pdf) | [Radiation acquisition](../measurement/radiation-and-acoustic-capture.md), [microphones/room](../acoustics/04-microphones-room-and-directivity.md) |

Synthetic analytic verification can start with assumed air and simple geometry.
Specimen predictions additionally require P07 inner/outer geometry and hole
connectivity, P08/P09 motion with normalization/ports, and matched calibrated
acoustic data. Do not substitute equal total opening area for verified perforation
physics. The [source atlas](../research/source-atlas.md) records access, not an
imported acoustic asset. Every recipe produces a manifest and scoped accepted,
rejected, inconclusive or blocked finding; missing data never become calibrated
defaults. The phase's general field solvers and runtime transfer assets remain
proposed even though the lumped prototype and observation test exist.

## P10-T01: define acoustic variables and boundaries

State pressure, particle velocity, density, speed of sound, coordinate frame,
and units. Distinguish velocity potential from pressure when reading sources;
their scaling and time derivatives are not interchangeable. Define normal
directions consistently at structural/acoustic interfaces and openings.

Describe the cavity domain, openings, exterior domain, and boundary conditions.
An ideal rigid wall, prescribed vibrating surface, and fully coupled elastic
wall answer different questions. Specify whether a radiation impedance loads
the structure or whether pressure is only computed afterward. A one-way
observation model cannot claim to include radiation damping in the mechanical
state unless that loss is represented elsewhere with an explicit accounting.

Record environmental properties used for air. Keep uncertainty appropriate to
the measurement context; do not present a many-digit speed of sound as a fixed
universal constant when temperature and humidity were not controlled.

### Execution recipe

**Question and research.** Which air domain, boundary and source quantity does
the claimed pressure represent, and which acoustic terms actually load the
mechanics? Use `ENG-P10-T01` / `REQ-ACO-001`,
[Acoustics 01](../acoustics/01-cavity-and-structural-acoustic-coupling.md) and
[the exchange contract](../acoustics/06-solver-exchange-and-evidence-packages.md).
Start `R-ACO-DELEON-2008` with §§2.1–2.2 and §3.1.2, recording what was
actually read; search `perforated cavity exterior acoustic pressure volume flow
interface normals` and `setar soundhole thermoviscous end correction`. Its limited
perforation treatment is a reason to verify an alternative aperture formulation,
not permission to adopt its approximation for setar holes.

1. Freeze inner/outer geometry, wall faces, hole connectivity, passages and
   supports with P07 uncertainty. Define pressure `p` in Pa, particle velocity in
   m/s, density in kg/m³, sound speed in m/s and volume flow `Q` in m³/s.
   A velocity potential with `u=grad(phi)` has units m²/s and
   `p=-rho*phi_dot` under the stated linear convention; it is not a pressure
   sample. Keep frequency in Hz distinct from angular frequency in rad/s.
2. Draw both fluid domains and their outward normals. With solid normal `n_s`
   into fluid, its neighboring fluid normal is `-n_s`; pressure traction on the
   solid is `-p n_s`. Specify rigid, pressure-release, prescribed-motion,
   impedance or fully coupled conditions per patch and opening. For a cavity
   outlet, declare exterior termination and the sign of `p Q` power in W.
3. Define the small-signal, stationary-air regime and measure temperature,
   humidity and ambient pressure when acquiring data. Derive or justify air
   properties and propagate their correlated uncertainty with dimensions and
   aperture geometry. Separate passage length from effective end correction;
   list thermoviscous, leakage and nonlinear-flow terms as included or omitted.
4. Audit constant-pressure/normal resultants on a closed surface, piston volume
   flow `Q=A v_n`, and pressure-force units. Retain the sealed cavity's mean
   compression mode where wall displacement changes volume. Intentionally flip
   one face or treat volume flow as particle velocity to establish diagnostic
   sensitivity before meshing a specimen.

**Translation and gate.** Present `HelmholtzCavity` in
[body_model.hpp](../../include/setar/instrument/body_model.hpp) uses one effective
opening and no resolved perforated wall; its hole is not removed from plate
stiffness/mass. A proposed spatial boundary schema must retain face/aperture
identity, normal, basis, quadrature and environment, feeding proposed
`src/acoustics/offline_cavity.cpp`. Deliver a domain diagram, boundary table,
unit/normal checks, geometry/environment uncertainty and unresolved apertures to
T02/T03. Existing `body.cavity_coupling_oracle` checks the lumped fixture;
planned `acoustics.interface_adjoint` is not yet its distributed-interface test.
Reject ambiguous normals, missing aperture termination or untraceable units;
accept a synthetic domain only with its explicit validity limits.

## P10-T02: verify elementary acoustic cases

Before importing the instrument, reproduce suitable analytic or high-confidence
reference problems: an enclosed cavity mode, a simple radiating source, and a
boundary/interface test. Choose cases that exercise the intended discretization
and units, not merely problems a software package solves by default.

Evaluate phase, amplitude, distance behavior, and conservation or radiated power
where applicable. State the far-field or near-field assumptions. A formula
valid far from a compact radiator cannot be applied arbitrarily close to a
complex surface without analysis. Use mesh and frequency refinement with
error measures appropriate to complex-valued fields.

For an exterior truncated domain, quantify reflection from the numerical
boundary. For a boundary-element formulation, evaluate singular quadrature,
conditioning, and formulation-specific resonance issues. A visually smooth
pressure map is not evidence that the exterior problem is correct.

### Execution recipe

**Question and research.** Does each selected acoustic solver reproduce known
complex fields and power before geometry or fitting can conceal a formulation
error? This is `ENG-P10-T01` / `REQ-ACO-001`. Read
[Acoustics 01](../acoustics/01-cavity-and-structural-acoustic-coupling.md),
[02](../acoustics/02-exterior-bem-and-radiation-impedance.md) and
[03](../acoustics/03-fdtd-boundaries-and-pml-validation.md). Inspect
`R-ACO-DELEON-2008` §3.2 and §4 for integration/reference methods and use
`R-NUM-BILBAO-2009` only after acquiring the relevant numerical sections.
Search `Helmholtz BEM pulsating sphere irregular frequency singular quadrature`
and `acoustic staggered grid PML reflection angle late time stability`; obtain
the actual chosen formulation's primary equations rather than mixing examples
from different schemes.

1. Generate an analytic rigid rectangular cavity with dimensions in m and
   `f_n=(c/2)*sqrt((n_x/L_x)^2+(n_y/L_y)^2+(n_z/L_z)^2)`.
   Compare several modes and degenerate subspaces, and handle the constant
   pressure mode explicitly. Add a pressure-release or impedance-wall case to
   exercise a different boundary; refining the mesh must leave physical volume
   and source strength fixed.
2. Use a pulsating sphere with radius `a`, outward velocity in m/s and peak
   phasors under `exp(i omega t)`. Independently evaluate
   `Z_spec=rho*c*(i k a)/(1+i k a)`, outgoing phase and `1/r` pressure.
   Compare surface pressure, several near/far observers, and input versus
   enclosing-surface power with consistent peak/RMS factors. Sweep `ka` so a
   single accidental amplitude match cannot pass.
3. For BEM, refine surface basis, singular/near-singular quadrature and solve
   tolerance separately; test near known interior resonances for formulation
   nonuniqueness. For FDTD, derive compatible gradient/divergence work and the
   chosen CFL limit, then assess axis/diagonal dispersion, voxelized geometry,
   source normalization and pressure/velocity space-time alignment.
4. Compare truncated/PML domains with an independently enlarged domain before
   return reflections arrive. Resolve reflection versus frequency/angle,
   grazing incidence, corners and late-time auxiliary-state growth. A deliberately
   reflective boundary and reversed outgoing phase must fail even if magnitude
   images look smooth. Cross-check BEM/FDTD on the same analytic physical source,
   using independent source normalization and refinement paths.

**Retention and gate.** Save analytic generators, equations/source locations,
meshes, complex fields, source work, power flux, reflection spectra and separate
error curves. Set proposed tolerances from the downstream phase/level/power
budget and demonstrated convergence, rather than an unexplained cells-per-wave
count. The proposed `tests/acoustics/cavity_tests.cpp` must implement
`acoustics.cavity_mode_convergence`; no present CTest verifies a spatial cavity,
exterior BEM or PML. `body.cavity_coupling_oracle` is an independent two-degree
plate/Helmholtz check and cannot substitute. Reject nonconvergent phase/power or
late growth, retaining the cause or blocked-method decision. Hand verified
operators and valid frequency/geometry envelopes to T03; actual specimen
acoustic measurement remains a separate acquisition, not an analytic benchmark.

## P10-T03: couple cavity and structure

Define how pressure traction loads the structure and how surface normal
velocity drives the acoustic field. Verify the interface work identity and
normal sign. When different meshes are used, construct interpolation/projection
operators that preserve the required pairing; nearest-node matching alone may
not preserve power.

Compare uncoupled and coupled cases to isolate the acoustic loading effect.
Check whether retained structural/acoustic modes are sufficient near the
resonances of interest. Avoid adding an empirical cavity resonance on top of a
fully coupled cavity that already contains the same mechanism.

If the initial production model uses a reduced cavity or radiation impedance,
derive it from a verified reference or an identified measurement with stated
limits. Check causality, stability, and passivity where the representation acts
as a load. A fitted frequency curve that matches magnitudes can still have an
unphysical phase or active impedance.

### Execution recipe

**Question and research.** Does pressure change structural motion by the work
predicted at the same interface, and can the retained states reproduce hybrid
resonances without duplicating cavity or radiation losses? Use `ENG-P10-T01` /
`REQ-ACO-001`, [Acoustics 01](../acoustics/01-cavity-and-structural-acoustic-coupling.md)
and [Body 10](../body/10-synthetic-prototype-body.md).
Inspect `R-ACO-DELEON-2008`'s formulation and aperture limits, then the
[official acoustic–structure boundary reference](https://doc.comsol.com/6.3/doc/com.comsol.help.aco/aco_ug_multiphysics_couplings.14.03.html)
for its specific normal conventions; these are methods, not a ready specimen
model. Search `nonmatching structural acoustic interface work adjoint pressure
traction` and `multiple soundhole radiation impedance coupling`. Acquire
synchronized force, surface motion and pressure for a matched support state
before identifying a specimen load.

1. Form a shared surface quadrature and mapping from structural velocity to
   fluid normal velocity. Derive pressure traction through its work adjoint;
   verify `P_s=-integral(p v_s dot n_s dS)` and the opposite acoustic input
   power independently. For pressure FEM retain the paired `C_sp p` and
   `C_sp^T q_ddot` terms with their derived signs, rather than forcing the
   mixed-unit block system to look conventionally symmetric.
2. Test a closed piston/cavity in the long-wavelength limit against a separately
   derived mass/compliance model, with `C_a=V/(rho*c^2)` in m³/Pa. Preserve mean
   pressure and reconstruct physical fluid energy in J from pressure/velocity;
   a pressure-only quadratic numerical invariant is not automatically joules.
   Refine structure, air and interface quadrature independently.
3. Compare zero coupling, prescribed one-way motion and full bidirectional
   loading while holding all other terms fixed. Track hybrid poles, mechanical/
   fluid energy partition and bridge mobility as coupling increases. Flip one
   interface sign and break one transpose as negative fixtures; require detected
   work error. Add losses individually and check nonnegative accounted loss.
4. For openings/exterior loading, use conjugate pressure/volume-flow or integrated
   traction/velocity ports. Check the full Hermitian radiation-resistance matrix
   under simultaneous source patterns, with causality and a passivity argument
   appropriate to its realization. Negative cross terms can represent passive
   interference. Compare matched measured motion/pressure with uncertainty;
   insufficient observability leaves loss partition inconclusive.

**Translation and gate.** `build_body()` already incorporates a lumped cavity
into `BodyMode` poles/residues. Replacing it with a spatial cavity must remove or
explicitly exclude that prior contribution. Likewise, current modal damping
absorbs radiation loss: adding a derived radiation impedance requires a justified
structural-versus-radiation loss partition, not adding both or blindly subtracting
a fitted damping number. Exterior added mass must not be counted again through
`opening_effective_length_m`. `radiation_weight` remains observation-only and
cannot supply this feedback. Retain interface matrices, state/loss ownership,
energy and pole convergence, invalid fixtures and the scoped disposition for
proposed `src/acoustics/offline_cavity.cpp` / `tests/acoustics/cavity_tests.cpp`.
Present `body.cavity_coupling_oracle` is a useful lumped comparator; planned
`acoustics.interface_adjoint` and `acoustics.cavity_mode_convergence` remain open.
Reject unresolved work or duplicated mechanisms; hand an accepted load and its
separate observation source basis to T04/T05 and P09.

## P10-T04: define microphone and listener observations

Specify microphone positions and orientations relative to the instrument,
including distance and uncertainty. Keep microphone calibration, frequency
response, directivity, and room effects separate from the intrinsic instrument
model. A free-field prediction should not be fitted directly to a reverberant
room recording without accounting for the observation chain.

Choose whether the production instrument exposes a fixed virtual microphone,
several measured/derived observations, or a spatially interpolated field. Verify
the interpolation with held-out positions. Do not create arbitrary stereo width
and describe it as measured directivity. Stereo presentation may be useful,
but its origin and limits must be explicit.

For comparisons, preserve absolute level when the question concerns efficiency
or delivered work. Use level matching only when the perceptual question calls
for it, and report the applied gain separately. Automatic normalization can
erase evidence of a wrong coupling scale.

### Execution recipe

**Question and research.** What pressure should a specified microphone record,
and can changes in direction, distance or room be distinguished from changes in
the instrument? Use `ENG-P10-T02` / `REQ-ACO-002`,
[Acoustics 04](../acoustics/04-microphones-room-and-directivity.md) and
[radiation acquisition](../measurement/radiation-and-acoustic-capture.md).
Use `R-METROLOGY-JCGM-100` §§4–5/§7 plus the NIST calibration references in
the source map; record whether calibration is pressure, free-field or diffuse-field
and which frequency/phase data are available. Search `microphone complex
sensitivity free field orientation acoustic center calibration` and `musical
instrument directivity room gating spatial holdout`. A one-frequency sensitivity
check is not a broadband directional calibration.

1. Fix specimen and room frames, microphone acoustic centers in m, orientations
   in rad, source/reference point, distances, channel order and position
   uncertainty. Record force in N, surface velocity in m/s and pressure in Pa
   synchronously where possible. Calibrate the complete microphone/preamp/ADC
   gain, polarity, relative delay and drift; preserve noise and pre/post checks.
2. Compute pressure from the accepted complex field at the actual observer,
   followed by microphone response and electrical/digital gain. Retain the
   unprocessed Pa signal and the explicit dimensionless-output gain. Compare
   multiple radii before asserting far-field `1/r` behavior; propagate position,
   sound-speed and timing uncertainty into phase, including correlated errors.
3. Characterize or bound room reflections with a reference acquisition and
   repeatable placements. Record time gates and their low-frequency resolution
   limit. Separate player/support mechanical loading from player/room scattering.
   Fit only an instrument-only or explicitly room-modeled transfer; moving a
   room null into the body model is a failed interpretation.
4. Define absolute pressure, normalized directivity and integrated power as
   different outputs. Use solid-angle quadrature for far-field integration;
   verify it on a monopole and a known directional pattern. Reserve microphone
   angles, radii and orientations before spatial fitting. Include channel swaps,
   polarity reversal and wrong propagation delay as negative cases; preserve
   absolute level so normalization cannot conceal a calibration error.

**Translation and gate.** Current `BodyObservation.distance_m` and
`dipole_weight_per_m` prepare two synthetic monopole/dipole acceleration weights
via `build_body()`. They are not a measured directivity field or arbitrary
microphone-placement API. `Instrument::Impl::step` applies `radiation_weight` and
`ProcessSpec::output_gain_per_pa`; proposed `src/acoustics/radiation.cpp` must
represent the richer pressure/propagation/calibration chain without modifying
mechanical loading. Existing `acoustics.observation_invariance` in
[engine_tests.cpp](../../tests/instrument/engine_tests.cpp) checks selected
mechanical states and ledger terms under changed weights/gain, not calibrated
positions. Planned `acoustics.directivity_holdout` needs reserved-position
predictions within measurement/numerical budgets. Retain geometry, calibration,
raw/derived transfers, room limits, gains and rejected trials in proposed
`experiments/directivity-sweep.md`. Missing absolute calibration blocks absolute
pressure claims; inadequate clocks block phase claims. Hand scoped observations
and remaining gaps to T05/T06 and P17.

## P10-T05: reduce for real-time observation

Factor frequency-domain or modal radiation maps into a runtime representation
with a declared causal/stable realization. Compare impulse response, complex
frequency response, and position dependence with the offline reference. Keep
the valid frequency range and sample-rate behavior in the asset schema.

Assess truncation, interpolation, and discretization separately. A production
filter's error must not be attributed to the underlying body physics without
an isolation experiment. Check changes in microphone position or radiation
asset during ringing for safe state transitions and predictable work/output
behavior. Perform expensive fitting and factorization outside the callback.

### Execution recipe

**Question and research.** Can a compact causal observation reproduce the
offline complex field over the declared band and microphone region while any
separate acoustic load remains passive? This is `ENG-P10-T02` / `REQ-ACO-002`
and `ENG-P13-T01` / `REQ-RTS-002`. Read
[Acoustics 05](../acoustics/05-radiation-reduction-and-runtime-transfer.md) and
[the exchange package](../acoustics/06-solver-exchange-and-evidence-packages.md).
Use `R-ACO-DELEON-2008` for the reference formulation inspected during the task
and [SINTEF's original fitting/passivity resources](https://www.sintef.no/en/software/vector-fitting/)
for distinct rational-fit and passivity methods. Search `causal acoustic radiation
transfer delay vector fitting matrix passivity source basis`; verify the selected
method/version and do not treat a stable scalar fit as a passive matrix load.

1. Freeze offline loading and observation as separate assets with source basis,
   units, normal orientation, geometry hash and band. A source coefficient may be
   velocity in m/s or volume flow in m³/s; derive the associated pressure-map
   units. Reserve frequencies, directions and radii before selecting filter or
   spatial order. Keep numerical-reference uncertainty distinct from fit error.
2. Fit the complex observation, isolating only a justified propagation delay.
   Compare candidate FIR/IIR/state-space and spatial bases by impulse onset,
   long tail, magnitude, phase and reserved-position error. Preserve relative
   phase among sources and add their complex pressures before magnitude. A
   magnitude-only/minimum-phase reconstruction is a separate approximation.
3. If reducing feedback too, project through work-conjugate ports and assess the
   full load's Hermitian resistance and rational passivity, including between
   sampled frequencies and outside the accuracy band. Microphone pressure is
   not a conjugate load, so its transfer need not be positive real. Document
   any passivity enforcement's effect on the fit and on coupled bridge motion.
4. Prepare rate-specific coefficients and fixed capacities off the callback.
   Sweep supported sample rates and block partitions against the offline
   reference, accounting for discretization warping, delay and sharp-contact
   out-of-band input. Test silent start, ringing-state microphone changes and
   asset replacement with explicit observation-memory mapping/crossfade rules.
   Wrong-rate coefficients, erased propagation memory and an unstable fitted
   pole are rejecting negative fixtures.

**Translation and gate.** The existing equation is
`p_o=sum_j radiation_weight[o,j] * eta_j_ddot`; a constant coefficient in
Pa·s²/m cannot encode a general transfer's poles, zeros, delay or spatial phase.
Current `Instrument::Impl::step` obtains acceleration by a centered second
difference, with the internal output timing documented in
[instrument.cpp](../../src/instrument/instrument.cpp). New dynamic observation
memory belongs in a proposed extension of `src/acoustics/radiation.cpp`, not in
unexplained modal gains; loading states require a separately verified P09 update.
Retain canonical realizations, delay conventions, stability/passivity evidence,
rate-specific errors, order-versus-cost and transition traces. Existing
`acoustics.observation_invariance` covers the static synthetic output;
`realtime.reference_parity` and `acoustics.directivity_holdout` remain planned
acceptance scope. Reject active loads, noncausal onset, unexplained pressure
scaling or failure on reserved positions. Hand the scoped accepted/rejected
asset, preparation bounds and uncertainty to P13 and T06; a missing calibrated
reference permits only synthetic reduction claims.

## P10-T06: assess radiation against measurements

Use calibrated structural and acoustic observations where possible to separate
wrong surface motion from wrong radiation. Compare several positions and
gestures, not a single equalized spectrum. Evaluate low-frequency response,
resonance/antiresonance structure, directional changes, phase where measurable,
and uncertainty from the room and microphones.

The exit report contains actual pressure/directivity plots, units, scale,
observation geometry, calibration chain, and held-out cases. If data remain
unavailable, record the verified synthetic acoustic solver and the unvalidated
instrument prediction as separate achievements.

### Execution recipe

**Question and research.** Does the frozen model predict calibrated pressure at
unseen positions and gestures, and can any mismatch be assigned to motion,
radiation or the observation chain? Use `ENG-P10-T02` / `REQ-ACO-002`,
[radiation acquisition](../measurement/radiation-and-acoustic-capture.md) and
[gesture/radiation validation](../validation/04-gesture-and-radiation-validation.md).
`R-SETAR-MANSOUR-SCAVONE-2012` §3–§4 motivates support/sensor controls, and
`R-METROLOGY-JCGM-100` §§5/7 governs uncertainty propagation/reporting.
Search `instrument force velocity pressure transfer calibrated directivity
held out measurement` and `setar sound radiation soundhole measurement`; record
whether retrieved studies actually provide force references, spatial metadata
and usable raw assets. Source plots alone are not this specimen's calibration.

1. Freeze model/asset revisions and the development/holdout split by acquisition
   session, position and gesture before fitting. Keep repeated takes and derived
   versions together. Use controlled, authorized force excitation for matched
   transfers and separately documented musical gestures for broader behavior;
   preserve tuning, support, player contact, environment and sensor loading.
2. Acquire synchronous force, multiple relevant surface/bridge motions and
   microphone pressure with calibrated gains, clocks, locations and uncertainty.
   Retain raw records, rejected overloads/double hits and usable input bands.
   Compare `p/F` in Pa/N and `v/F` in m/(N·s) independently. One local `p/v`
   ratio is not a universal radiation operator when surface mode mixtures vary.
3. Predict reserved angles/distances and a reserved session without further EQ
   or gain fitting. Report complex transfer error, pole/zero structure, attack
   timing, directional nulls, absolute pressure and spatially integrated power
   only where the field/coverage support it. Use absolute error near nulls;
   display measurement, numerical and reduction uncertainty separately.
4. Isolate alternatives: feed measured/reconstructed surface motion into the
   acoustic reference to test radiation conditional on motion; compare model
   motion against structural measurements; repeat observation placements to
   expose room/calibration sensitivity. Include deliberately mislocated
   microphones or wrong gains as negative analysis controls. A room-colored or
   normalized fit must not pass the absolute free-field criterion.

**Translation and gate.** A supported finding changes a named boundary,
`BodyMode` residue/loss partition, or proposed radiation observation realization;
it does not justify retuning every layer against one spectrum. Current
`BodyMode.radiation_weight` and `ProcessSpec::output_gain_per_pa` remain
uncalibrated synthetic parameters. `acoustics.observation_invariance` verifies
their one-way mechanical independence, whereas planned
`acoustics.directivity_holdout` requires the reserved measurements and a frozen
criterion derived from uncertainty and the stated acoustic use. Reusing a
holdout to select the model makes it development data and requires a new test
or a narrower claim. Deliver pressure/directivity plots with units and geometry,
calibration and split manifests, raw/derived hashes, residual diagnosis and
accepted/rejected/inconclusive/blocked decisions through proposed
`experiments/directivity-sweep.md` and `tests/acoustics/observation_tests.cpp`.
Stop absolute, phase or power claims when their inputs fail qualification;
retain any defensible relative result. Hand the calibrated domain to P12/P17,
with synthetic-solver success and unvalidated instrument prediction explicitly
separate when acquisition is blocked.

> Implement the specified acoustic reference and observation chain. Identify
> whether coupling is one-way or two-way, prove/check the interface work signs,
> and verify elementary cavity/radiation cases before using the instrument
> geometry. Preserve complex response and level information. Compare reduced
> and full predictions and do not count room reverb, EQ, or stereo decoration
> as evidence of a correct setar radiation model.

## Exit and downstream handoff

- Deliver domain/boundary and source/receiver definitions, actual source sections
  read, air/geometry uncertainties, independent analytic benchmarks and failed
  formulations. A cavity or pressure-field plot alone does not close the gate.
- Export acoustic loading separately from pressure observations, with units,
  work adjoints, normalization, complex convention, delay and state ownership.
  Identify removal/replacement of existing lumped cavity, aperture end correction
  and radiation-loss allowances before adding new physics.
- Supply the offline/reduced comparison and canonical/runtime assets with
  calibrated microphone geometry, room limits, gains, held-out cases and declared
  frequency/spatial/rate domain. Keep raw Pa output available for validation.
- Give P09 any changed feedback law, P13 prepared realization/capacity/error
  budgets, and P12/P17 the untouched validation partition. Report exactly which
  existing CTests ran and which acoustic acceptance IDs remain planned; neither
  this documentation nor a synthetic verification result changes specimen status.
