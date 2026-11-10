# P09 — Couple strings, bridge, and body without hidden energy

**Status:** specification. **Prerequisites:** P04 and P08; P05/P06 provide
interaction tests. **Exit evidence:** a verified multiport coupling whose
mechanical work, reciprocity, and coupled behavior remain correct under the
supported discretizations.

## Why coupling is its own phase

Independently convincing subsystems can become unstable or physically wrong when
connected. A string may exert a force while the body is driven with yesterday's
velocity; a bridge rotation may be confused with translation; or two different
normalizations may scale the same mode twice. These defects often sound like
resonance or extra richness before they become obvious instability.

Read the power-port contracts in [mathematics](../mathematics/README.md),
[body](../body/README.md), and [numerics](../numerics/README.md). The first product
of this phase is a written coupling contract, not a new preset.

## Entry routes and source map

The current connection already exists in
[instrument.cpp](../../src/instrument/instrument.cpp): `Instrument::Impl::assemble`
creates bridge spring/damper channels and `solve` solves their reactions together
with contacts. The richer rotational, distributed-foot and measured-port contracts
in [Body 03](../body/03-bridge-contact-and-multiport-coupling.md) remain extensions.
Use [the solver derivation](../numerics/15-modal-contact-solver.md) for the actual
three-level update; a midpoint reference proof does not automatically prove it.

| Question | Primary research route | Required project input |
| --- | --- | --- |
| Which signed bridge ports represent the experiment? | `R-SETAR-MANSOUR-2011`, [author paper](https://caml.music.mcgill.ca/lib/exe/fetch.php?media=publications:caa2011_mansour.pdf), §3; `R-SETAR-MANSOUR-SCAVONE-2012`, [author paper](https://caml.music.mcgill.ca/lib/exe/fetch.php?media=publications:nantes2012_mansour.pdf), §3.1–§3.4 and §4.1–§4.2 | P07 coordinates and preload; P08 calibrated complex FRFs and normalization |
| What makes the entire connection passive? | `R-NUM-BILBAO-SMITH-2005`, [author manuscript](https://www.pure.ed.ac.uk/ws/portalfiles/portal/11222313/acustica_final.pdf), §II–§III and boundary/loss appendix; `R-CONTACT-BILBAO-2014`, [versioned preprint](https://arxiv.org/abs/1405.2589v1), inspect the particular potential/work derivation before use | [Energy-port equations](../mathematics/03-energy-and-passive-ports.md), independent coupled fixture |
| Which bridge response can be reduced or identified? | Smith's author-written [Bridge Modeling](https://www.dsprelated.com/freebooks/pasp/Bridge_Modeling.html); [SINTEF passivity methods](https://www.sintef.no/en/software/vector-fitting/passivity/); `R-METROLOGY-JCGM-100`, [official record](https://www.bipm.org/en/doi/10.59161/jcgm100-2008e), §§4–5/§7 | [FRF acquisition](../measurement/frf-and-modal-protocol.md), [passive reduction](../body/08-passive-reduction-and-residual-flexibility.md) |

These sources supply experimental or mathematical methods, not specimen bridge
coefficients. Follow the [atlas's access limits](../research/source-atlas.md) and
record the exact sections actually inspected. Synthetic port, solver and state
tests can proceed before specimen acquisition; a measured coupling claim requires
the matched specimen, setup, calibrated input/output directions and uncertainty.
Each recipe retains a manifest, raw/derived lineage, rejected trials, and an
accepted, rejected, inconclusive or blocked decision. Proposed paths are work
targets, not existing implementations or completed acceptance evidence.

## P09-T01: define mechanical ports

For every interface, state coordinate frame, orientation, force sign, velocity
sign, and power pairing. Translational ports use force and velocity; rotational
ports use torque and angular velocity. A normal/tangential basis must be
transformed consistently on both effort and flow variables. Test that a frame
rotation leaves computed physical power invariant.

Describe the bridge footprint and each string's contact location. Decide whether
the bridge is rigid, elastic, lumped, or included in the full structural mesh.
If the bridge is represented both in an effective body mobility and as explicit
mass/stiffness, investigate double-counting. Distinguish the empirical response
of the assembled instrument from the response of a bare component.

Define force distribution and velocity observation as an adjoint pair under the
model's inner product. A spatial interpolation rule is not correct merely
because it is smooth. Write the discrete power identity and test it with
independent random finite vectors, including rotated and multi-contact cases.

### Execution recipe

**Question and research.** Can every string force do exactly the opposite work
on the declared bridge/body coordinates, including simultaneous contacts? A wrong
map changes decay and sympathetic response even when individual modes look right.
Use `ENG-P09-T01` / `REQ-CPL-001`, [Body 03](../body/03-bridge-contact-and-multiport-coupling.md)
and the [power contract](../mathematics/03-energy-and-passive-ports.md).
Extract actual excitation/sensing directions and support state from
`R-SETAR-MANSOUR-2011` §3 and `R-SETAR-MANSOUR-SCAVONE-2012` §3–§4.
Search `setar bridge mobility force velocity collocated ports` and `rigid bridge
wrench twist virtual work`; route missing coordinates or residues to the P08 FRF
campaign instead of inferring them from a microphone spectrum.

1. Freeze the specimen or synthetic fixture, nut/tail/bridge coordinates in m,
   contact footprints, break angles in rad, preload in N and support state. Label
   bare-body versus assembled-bridge FRFs. Audit whether bridge inertia or foot
   compliance is already included before adding it explicitly.
2. Define force positive into each subsystem and velocities in the same physical
   directions. For a rigid kinematic map `v_s = T v_b`, derive
   `f_b = -T^T f_s`, so summed internal power is zero. Include moments in N·m
   and angular velocities in rad/s when lever arms matter. For a compliant
   connector use relative displacement `r`, spring energy `r^T K_c r/2` in J
   and dissipation `r_dot^T C_c r_dot` in W; its two port works need not cancel
   without those storage/loss terms.
3. Construct velocity interpolation first, then its quadrature/metric adjoint for
   forces. Independently sum force/velocity products before and after projection.
   Exercise multiple unequal simultaneous forces, non-axis-aligned frames,
   distributed patches and a nonorthogonal scaled map, for which inverse and
   transpose differ. Verify force and moment resultants separately.
4. Propagate contact-location/direction and calibration uncertainty to the map and
   measured power. Keep numerical roundoff bounds separate. Reverse one effort
   sign only, omit an area weight, and use a wrong transpose in negative fixtures;
   each must fail the relevant work/resultant check.

**Translation and gate.** Current `BridgeConfig` in
[config.hpp](../../include/setar/instrument/config.hpp) has two translational
stiffness/damping values; `BodyMode.bridge_participation` supplies eight signed
modal contact shapes. `assemble` uses string coefficient `+1` and body `-1`.
There are no explicit bridge rotation or foot-slip states. Preserve
`StringConfig.total_length_m` as nut-to-tail and `bridge_position_m` as speaking
length. Deliver the signed port table, maps, independently computed work defects,
uncertainty and bridge ownership decision to P09-T02 and proposed
`tests/coupling/port_energy_tests.cpp`. Existing `coupling.port_power_balance`
checks a midpoint two-mass fixture, not these general transforms. Extending that
acceptance ID requires precision/conditioning-derived bounds and detected negative
controls; reject unexplained gains or unmatched ports before coupling real assets.

## P09-T02: choose a coupled time update

Derive the full connected update before splitting code into components. Identify
which variables are simultaneous and which, if any, are delayed. A one-sample
feedback delay can change passivity and resonances even when each isolated
component is passive. Do not infer stability of the connection from isolated
unit tests.

For an implicit solve, specify matrix structure, factorization reuse, residual
criteria, and failure policy. For a passivity-preserving formulation, show the
actual discrete energy/work balance including contact, damping, and port terms.
For a controlled approximation, bound its error and explicitly limit the
supported step/band/parameter range. Avoid combining two independently proved
schemes and asserting that the composite proof follows automatically.

Start with a string connected to one passive resonator with an analytic or
independently assembled reference. Verify the fixed-boundary limit, soft-support
limit, and zero-coupling limit. These tests localize sign and normalization
defects before the body has many modes.

### Execution recipe

**Question and research.** Does the simultaneous update reproduce the connected
mechanics without an artificial feedback delay or unexplained source of energy?
Use `ENG-P09-T01` / `REQ-CPL-001`. Read the actual equations E1, C1 and N1 in
[the modal-contact solver](../numerics/15-modal-contact-solver.md), then compare
the independent midpoint argument in [energy and passive ports](../mathematics/03-energy-and-passive-ports.md).
`R-CONTACT-BILBAO-2014` supplies a potential/work research route and
`R-NUM-BILBAO-SMITH-2005` §III a discretization/boundary route; neither certifies
this composite solver. Search `discrete gradient simultaneous string body contact
energy time staggering` and log the exact source time levels before adaptation.

1. Start with independently assembled masses, springs and damping in kg, N/m and
   N·s/m. Solve their continuous linear state equation analytically or with a
   separately converged dense reference. Next replace one mass with a finite
   string basis whose masses and shapes are derived independently. Freeze initial
   displacement/velocity, ports and connector stiffness while refining the step.
2. Write the prototype channel equation `s - b - M_c F(s) = 0`, with `s,b` in m
   and compliance `M_c = B G B^T` in m/N. Record how exact elimination of linear
   channels and the nonlinear solve share the same forces. Its work uses
   `F^n (q^(n+1)-q^(n-1))/2`, with both subsystems at that centered level;
   do not substitute a midpoint or end-step velocity into its ledger.
3. Compare poles, force histories and port motion against the independent
   reference. Recompute each scheme's discrete ledger independently; do not
   equate its modified energy to continuous mechanical energy at finite step.
   Sweep passive connector stiffness and finite body
   mass toward soft/fixed-support limits. Test zero coupling in the independent
   fixture; current `BridgeConfig` requires positive stiffness, so an illegal zero
   is not a supported engine limit. Zero body participation instead removes the
   shared path while leaving each bridge spring grounded.
4. Repeat with reversed subsystem ordering and simultaneous contacts. Intentionally
   delay one reaction and reverse one reaction sign in separate diagnostic
   fixtures. Require the work and response checks to detect the modifications,
   including cases that remain audibly bounded. Tighten solve tolerance and step
   independently to separate residual error from discretization error.

**Translation and gate.** Inspect `Impl::solve`, `scatter` and `step` in
[instrument.cpp](../../src/instrument/instrument.cpp), and `SmallNewtonSolver` in
[numerics.cpp](../../src/instrument/numerics.cpp). A finite unconverged iterate is
currently used and counted; nonfinite containment resets state. Neither outcome
qualifies a scientific render. Existing `coupling.two_mass_oracle`,
`coupling.port_power_balance` and `engine.lossless_energy_ledger` cover different
synthetic subsets; the independent coupled-string comparison remains proposed.
Retain matrices, time-level table, force/work traces, convergence curves and
failure cases in the `ENG-P09-T01` dossier, targeting proposed
`src/physics/coupled_step.cpp` only if a reviewed extension needs it. Set bounds
from reference convergence and force-times-coordinate-residual work estimates,
not an arbitrary audible-noise threshold. Hand the accepted update/domain or
rejected scheme and reason to P09-T03/T06; measured FRFs still gate specimen claims.

## P09-T03: introduce all physical strings

Represent the target instrument's physical strings as persistent state, with
course membership and tuning described separately. A paired course does not
mean two identical oscillators with a stereo spread. It can have distinct string
parameters and contact timings while sharing bridge/body motion.

Excite one string and observe the others through the common coupling. Test
sympathetic transfer, energy exchange, and beating for controlled detuning.
With losses removed, the declared closed system's total energy should follow
the expected balance. With losses added, energy transfer between strings must
not be mistaken for global energy growth or decay.

Check invariance under reordering of string storage and equivalent port ordering.
A vector index must not accidentally determine a different physical delay. Use
test cases with unequal strings so a symmetric implementation error cannot
hide behind identical parameters.

### Execution recipe

**Question and research.** Does an unstruck physical string acquire the motion
predicted by shared bridge mobility, with detuning changing exchange and beating
without a sympathetic audio gain? This links `ENG-P09-T02`, `ENG-P11-T01`,
`REQ-CPL-002` and `REQ-PRF-001`. Read [string identity](../strings/01-identity-and-state.md)
and [Body 03](../body/03-bridge-contact-and-multiport-coupling.md).
`R-SETAR-TABASSIAN`, [performer handout](https://www.atlasensemble.nl/assets/files/instruments/Setar/Setar%20by%20Kiya%20Tabassian.pdf),
pages 1–2, motivates unplayed-string resonance and variable tuning, not numerical
transfer strengths. Search `setar sympathetic strings bridge cross mobility
detuning`; obtain calibrated string/bridge motion through the P08/P09 measurement
route before attaching a specimen claim.

1. Specify four separate strings, their nut-to-tail/speaking lengths in m, tension
   in N, linear mass in kg/m, rigidity in N·m², both polarizations, course labels,
   initial states and shared-body ports. First use a symmetric two-string fixture
   with analytically separable symmetric/antisymmetric modes; then use unequal
   masses, lengths and port signs so symmetry cannot conceal indexing defects.
2. Displace or strike only one string. Record each string's displacement in m,
   velocity in m/s and energy in J, signed bridge force in N, body energy and
   connector potential. Use mechanical probes before microphone output: a
   radiation null can hide actual sympathetic motion. Compare against a separately
   assembled coupled linear reference before adding nonlinear plucks.
3. Sweep controlled detuning in cents with tension changes derived from the
   declared string model. Predict splitting, beat frequencies and transfer maxima
   from the coupled eigenproblem rather than assuming globally monotonic transfer.
   Add string/contact loss one mechanism at a time; distinguish redistribution
   among strings from loss of the entire connected system.
4. Remove one unstruck string's body participation, then all cross paths; it must
   remain at rest when initially silent. Permute strings, event targets and both
   axes of the port matrix together and unpermute results. Repeated strikes must
   preserve ringing state. A deliberately copied output layer must fail the
   mechanical-motion criterion even if it sounds sympathetic.

**Translation and gate.** `Instrument` already owns four persistent two-polarization
states; `snapshot`, `string_displacement_m` and `string_velocity_m_per_s` expose
the relevant existing probes in [instrument.hpp](../../include/setar/instrument/instrument.hpp).
Current `coupling.cross_string_transfer` checks a synthetic unison pair against
one differently tuned string and disconnects one body's participation;
`coupling.uncoupled_limit`, `performance.persistent_state` and
`performance.restrike_memory` provide additional bounded checks. They do not
constitute a continuous detuning study or measured transfer validation.
Retain initial-state tables, modal/FRF predictions, exchange envelopes, raw
synchronized measurements with tuning/calibration uncertainty, and the decision
in proposed `experiments/sympathetic-transfer.md` and
`tests/coupling/cross_string_tests.cpp`. Reject transfer surviving disconnection,
storage-order dependence beyond a declared numerical budget, or unexplained
global energy. Hand accepted physical histories and unresolved ports to P11 and
P09-T04; blocked acquisition leaves the synthetic finding separate.

## P09-T04: compare response functions, not only audio

Compute driving-point and transfer mobilities at the bridge ports. Inspect
resonance splitting, antiresonances, phase, reciprocity where the physical model
requires it, and low-frequency compliance. Report the reference and sampling
conventions. Magnitude-only agreement can conceal a phase error that changes
feedback behavior.

Compare full-body and reduced-body couplings at the same ports. A reduced model
qualified in isolation may fail after connection if omitted flexibility becomes
important. Repeat truncation studies in the connected configuration and retain
the coupled error budget. Do not tune string damping to conceal reduction error.

### Execution recipe

**Question and research.** Does the connected model predict complex bridge
responses and simultaneous-force power over the promised band, including weak
antiresonances and strongly coupled strings? Use `ENG-P09-T02` / `REQ-CPL-002`,
[measured FRFs](../body/06-measured-frfs-and-modal-identification.md) and
[passive reduction](../body/08-passive-reduction-and-residual-flexibility.md).
Inspect `R-SETAR-MANSOUR-SCAVONE-2012` §3.1–§3.4/§4.1–§4.2 for apparatus
comparisons and `R-METROLOGY-JCGM-100` §§4–5 for correlated uncertainty.
Search `multiport mechanical mobility Hermitian passivity modal residues residual
flexibility`; follow Smith/SINTEF links in the source map for methods, not setar
port values. Acquire matched force/velocity records through the
[FRF protocol](../measurement/frf-and-modal-protocol.md).

1. Freeze support, string preload/tuning, bridge representation, coordinates and
   the full/reduced normalization. Measure or compute all needed driving and
   cross-port entries in m/(N·s), preserving phase, force spectrum, timing,
   coherence and repeatability. Convert accelerance using `Y=H_a/(i omega)` only
   in a supported nonzero band; retain the original acceleration data.
2. Compare complex `Y_ij` against `Y_ji` for reciprocity, without conjugation.
   Assess passivity using `H_Y=(Y+Y^H)/2` and
   `P_mean=F^H H_Y F/2` for peak phasors. Negative real cross entries can be
   passive; positive diagonals alone are insufficient. For mixed rotational and
   translational ports declare work-preserving scaling before eigenvalue/error
   comparisons. An incompletely measured matrix cannot certify its missing
   directions.
3. Refine frequency near poles/zeros and vary body order, residual flexibility
   and string resolution independently. Compare low-frequency compliance, pole
   splitting, absolute complex error and relative error only away from response
   zeros. Predict reserved port responses and multiple simultaneous force
   directions, including the least-dissipative eigenvector of `H_Y`.
4. Use an independent dense mechanical model or reserved measured acquisition
   as comparator. Add a channel-delay negative fixture and a symmetric matrix
   with positive diagonals but an indefinite Hermitian part; both must be
   detected. Retain a passive matrix with negative cross terms as a control
   against incorrectly rejecting harmless interference.

**Translation and gate.** Current `bridge_mobility` in
[body_model.cpp](../../src/instrument/body_model.cpp) sums `BodyMode` residues;
it is the body mobility before attaching dynamic strings, so a proposed connected
FRF harness must include the complete string/bridge system. Existing
`body.mobility_reciprocity` and `body.mobility_passivity` in
[body_tests.cpp](../../tests/instrument/body_tests.cpp) check a synthetic body.
The latter checks eigenvalues of the full eight-port Hermitian part on a frequency
grid, not just its diagonals; this is neither a measured certificate nor a global
passivity proof for an arbitrary fitted rational model. Retain complex matrices,
calibration covariance, frequency refinements, full/reduced errors and rejected
fits in the `ENG-P09-T02` transfer dossier. Bound discrepancies by independent
numerical error and measurement uncertainty; unexplained nonpassivity blocks
loading promotion. Hand accepted port residues/domain to P10/P13 and remaining
identifiability questions to P12; `coupling.cross_string_transfer` still needs
the reserved complex-response acceptance evidence.

## P09-T05: handle preparation and state changes

Specify what happens when tuning, body asset, bridge position, or sample rate
changes while the instrument rings. Some changes represent external work; some
are reconfiguration operations that require a declared state transition. They
must not silently reuse incompatible mode coordinates or factorization data.

Prepare new immutable coefficients and validated assets away from the callback.
Define a safe ownership transfer and retirement policy. If a state projection
or crossfade is used, quantify its consequences and label it a production
transition rather than an exact mechanical event. The old valid model remains
available if preparation fails.

### Execution recipe

**Question and research.** Can a ringing instrument adopt a new tuning, basis or
sample rate with a declared state/work transition and bounded ownership? This
joins `ENG-P13-T02` and `ENG-P09-T01`, under `REQ-RTS-001`, `REQ-RTS-003` and
`REQ-CPL-001`. Read [state ownership](../architecture/02-physical-ports-and-state-ownership.md),
[asset publication](../realtime/04-concurrency-state-and-telemetry.md) and
[reduction/state mapping](../body/08-passive-reduction-and-residual-flexibility.md).
Use `R-NUM-BILBAO-SMITH-2005` §II to identify parameter-dependent stored energy,
then derive this transition separately; its fixed-model proof is not a live
retuning algorithm. Search `passive model reduction state transfer changing
basis energy` and record the exact realization assumed. For physical retuning
claims, require calibrated tension/motion histories from the approved acquisition
route; a UI parameter jump is not such a measurement.

1. Classify each request: physical parameter evolution with external work;
   basis-only representation change; or a production reset/crossfade. Record
   old/new asset hashes, sample rates in Hz, port maps, contact anchors and both
   stored modal time levels. State which persistent histories must survive.
2. For a basis-only change, reconstruct the same physical displacement/velocity
   and project using the declared mass metric. Test identity, permutation and
   mode-rescaling transforms first. Compare bridge motion and stored energy
   independently; retain energy discarded by a smaller basis. Reconstruct the
   three-level history at a changed step instead of reusing old coefficients or
   treating the prior displacement as a new-step sample.
3. For fixed-coordinate stiffness change, use the independently calculated
   energy difference, such as `q^T (K_new-K_old) q/2`, to identify parameter
   work; derive a discrete counterpart for the selected update. Bridge movement
   additionally changes maps and contact geometry. A reset or observation
   crossfade needs its own named transition accounting, not physical damping.
4. Prepare capacities, coefficients and maps away from processing; validate all
   before publication. Exercise invalid replacements, mode-count changes,
   stale snapshots and sample-rate changes during decay in a proposed ownership
   harness. Verify old-state survival on failure, no use-after-retirement, and
   no allocation/destruction in the processing path. Deliberately retain stale
   coefficients or swap modal coordinates without projection as negative cases.

**Translation and gate.** Present `Instrument::prepare` in
[instrument.cpp](../../src/instrument/instrument.cpp) builds a new `Impl`
transactionally, but successful preparation installs zero state. It cannot run
concurrently with `process`; `reset` also clears mechanics and the ledger.
Neither implements ringing-state asset adoption or physical retuning.
`engine.config_validation` checks failed preparation rollback;
`realtime.allocation_hook` covers existing processing/reset probes, not a future
publisher. Planned `realtime.asset_handoff` and `realtime.telemetry_snapshot`
remain open for proposed `src/engine/asset_publication.cpp` and its harness.
Retain transition diagrams, old/new state traces, energy/work differences,
roundoff/projection error budgets and fault results with a scoped disposition.
Reject unexplained energy jumps or ambiguous lifetime rules. Hand the selected
policy and unsupported transitions to P13/P14 and the P09-T06 stress corpus.

## P09-T06: qualify the connected system

Use long decays, repeated plucks, simultaneous contacts, retuning, and extreme
admissible parameter combinations. Track individual subsystem energy and the
global work residual. Include the deliberately wrong coupling sign in a test
fixture or independent diagnostic experiment to demonstrate that the energy
test can detect the relevant failure.

Acceptance requires bounded supported behavior, understood energy exchange,
convergence or controlled approximation evidence, and reproducible coupled
responses. It does not establish radiated setar fidelity until the acoustic and
measurement phases also pass.

### Execution recipe

**Question and research.** Which connected operating envelope has independently
bounded motion, work defect and response error, and which combination fails
first? Use `ENG-P09-T01` / `REQ-CPL-001` and `ENG-P09-T02` / `REQ-CPL-002`.
Read [verification and benchmarks](../numerics/11-verification-and-benchmarks.md)
with [the energy ledger](../mathematics/03-energy-and-passive-ports.md).
`R-CONTACT-BILBAO-2014` directs the contact/work audit and
`R-METROLOGY-JCGM-100` §§5/7 the uncertainty report. Search `coupled string body
passivity long time nonlinear contact solver residual` for counterexamples and
limitations of the chosen method. Specimen acceptance additionally consumes the
calibrated FRFs and sympathetic measurements from T03/T04.

1. Freeze the accepted port/update contract, parameter ranges, initial energies
   in J, external gesture trajectories in m/s, sample rates, mode counts and
   iteration/channel capacities. Choose boundary combinations through sensitivity
   analysis: weak/strong bridge compliance, clustered body/string modes, strongly
   detuned strings, both polarizations and maximum supported simultaneous
   contacts. Do not silently expand the domain after a failure.
2. Run zero input, long decay, repeated plucks and contact sequences. Retain
   string/body/connector energies, prescribed-motion work, dissipation, switching
   work and solver faults. Compare absolute residual in J and a fixed initial/
   injected-energy-normalized residual; dividing by a vanishing decay is invalid.
   A string may gain energy while the total closed system remains passive.
3. Recompute work independently from signed force/motion traces, and compare
   selected cases with the T02 dense/high-resolution reference. Refine time step,
   string/body order and nonlinear tolerance separately. For state changes,
   exercise only the declared current reset behavior or the separately verified
   proposed T05 policy; missing live-retuning support is an explicit gap.
4. Inject wrong reaction sign, one-sided delay and nonadjoint map separately.
   Include an active multiport combination despite positive driving-point real
   parts. Require detection before accepting the diagnostic. Derive residual and
   response budgets from independent reference convergence, conditioning and
   measurement uncertainty; bounded sound alone does not meet them.

**Translation and gate.** Use current `EnergyLedger`, `ProcessSummary` and probes
from [instrument.hpp](../../include/setar/instrument/instrument.hpp).
`engine.lossless_energy_ledger`, `engine.lossy_energy_ledger`,
`engine.newton_failure_reporting`, `coupling.cross_string_transfer` and
`coupling.uncoupled_limit` are present synthetic checks. The named
`coupling.port_power_balance` / `coupling.two_mass_oracle` acceptance IDs have
partial two-mass implementations; this recipe extends their connected scope.
Any unexplained growth, missed negative control, nonfinite containment or solver
failure rejects the affected scientific run; preserve the trace instead of
clipping or adding loss. Deliver commands, revisions, parameter manifest,
independent recomputation, worst cases and accepted/rejected/blocked domain to
P10, P13 and P17. Reproducible synthetic qualification leaves specimen fidelity
and any unavailable measured transfer explicitly open.

> Derive and implement the specified string/bridge/body connection with explicit
> effort/flow signs and adjoint maps. Verify a one-resonator reference, limiting
> cases, multi-string transfer, and coupled reduction error. Show the discrete
> work balance and any effect of time staggering. Do not hide instability with
> clipping, extra decay, or an undocumented output filter. Deliver a coupling
> diagram and evidence that a second reviewer can reproduce from the manifest.

## Exit and downstream handoff

- Supply signed/located ports, modal normalization, bridge ownership and the
  complete simultaneous work equation, including connector storage and losses.
- Attach independent connected-reference comparisons, matrix-level passivity,
  symmetric/asymmetric transfer, refinement and detected fault fixtures; identify
  the actual CTests executed separately from still-planned acceptance scope.
- State supported configurations and transition policies with parameter,
  numerical and measurement uncertainty kept separate. Preserve failed and
  inconclusive cases, not just the smallest residual.
- Give P10 structural surface/port motion and the loss partition, P11 persistent
  string/course semantics, and P13 full/reduced error budgets. An absent specimen
  or calibration packet blocks the measured claim while synthetic verification
  can continue. No phase status changes follow from completing this document.
