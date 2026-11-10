# Worked findings that change a model decision

These examples show what can be accepted, rejected, or left unresolved after a
research calculation. They supplement the [research-to-model workflow](research-to-model-workflow.md)
and its existing stiffness/tension example. They do not report an acquired
instrument, fitted specimen, engine execution, or newly passing CTest.

**All example quantities and calculated results below are synthetic.** Extra
digits support arithmetic reproduction, not measurement precision. API indices,
source editions, work-order IDs, and software versions identify existing objects;
they are not experimental quantities. The equations are analytic calculations.
The acquisition and implementation checks are proposed and unrun.

Current source and test definitions were inspected at base commit
`b4ae94e8cb1e77710a22d85eb707977d65f6c5a4`, with concurrent documentation changes.
The arithmetic was independently recomputed with Python 3.12.14 using its standard
library. No DSP build, render, optimizer, laboratory measurement, contact with a
source author, or parameter promotion was performed for this chapter.

## Shared evidence and source boundary

Each example is a worked finding, not a new entry in the evidence registry. For
execution, assign an actual finding ID using the [finding record](../../templates/research-finding.md)
and attach a [promotion record](../../templates/parameter-promotion.md) only when a
model/package change is proposed. A rejected promotion can leave its physical
hypothesis `unresolved`; rejection of insufficient evidence is not proof that the
mechanism is absent.

| Method route | What was inspected for this chapter | Boundary on its use |
| --- | --- | --- |
| `R-NUM-BILBAO-SMITH-2005`, `R-IDN-RAUE-2009` | Existing [source-atlas cards](source-atlas.md), [loss chapter](../strings/03-loss-and-dispersion.md), and [identifiability protocol](parameter-identifiability.md) | Primary-text access is inherited only as documented in the atlas. In particular, the Raue full method remains an access-limited lead; no newly reproduced profile-likelihood method is claimed. |
| `R-SETAR-MANSOUR-SCAVONE-2012` | Atlas apparatus/support reading map and [FRF procedure](../body/06-measured-frfs-and-modal-identification.md) | Its loading comparisons motivate a specimen-specific control. They supply no numerical loading correction here. |
| SINTEF Vector Fitting, [Passivity](https://www.sintef.no/en/software/vector-fitting/passivity/) | The primary page's “Passivity” subsection was retrieved on 2026-11-10; it separates stable poles from passivity | The mechanical power pairing and the modal proof below are derived here. An electrical fitting method or software example does not supply a setar body parameter. |
| `R-ACO-DELEON-2008` | Atlas access limits and [radiation reduction chapter](../acoustics/05-radiation-reduction-and-runtime-transfer.md) | Full acoustic formulation and perforation assumptions require further inspection before adoption. No exterior solver or measured radiation asset is introduced. |
| `R-CONTACT-BILBAO-2014`, `R-SETAR-TABASSIAN` | Atlas cards and [finite-contact chapter](../contacts/01-finite-nail-contact.md) | A fresh retrieval of the [versioned collision preprint](https://arxiv.org/pdf/1405.2589v1) failed. No new section-level primary review is claimed. The potential below is independently differentiated; neither an adjacent-instrument collision example nor a performer description supplies nail coefficients. |

Missing full text or raw data should produce a precise acquisition request, not
invented values. Record requested edition, apparatus state, calibration, port
frame, raw complex channels, normalization, and permitted uses. Such requests
remain drafts until sending is authorized.

## Example A — A decay time does not identify the owner of the loss

### Question, inputs, and analytic result

**Decision:** may a fitted amplitude decay be installed as intrinsic string loss
while retaining a damped, coupled body? This follows scientific `P04-T02` and
`P04-T06`, mapped to `ENG-P04-T01` and `ENG-P12-T01`, under `REQ-STR-003` and
`REQ-IDN-001` in the [crosswalk](../../roadmap/phase-task-crosswalk.json).

Assume one isolated underdamped mode,

\[
\ddot q+2\sigma\dot q+\omega_0^2q=0,\qquad A(t)=A_0e^{-\sigma t}.
\]

Choose the **synthetic** amplitude `T60 = 1.5 s` and undamped frequency
`f0 = 220 Hz`. Amplitude falling by `60 dB` means `A/A0 = 10^-3`, so

\[
\sigma=\ln(1000)/T_{60}=4.605170185988091\ {\rm s^{-1}}.
\]

The **synthetic** amplitude slope is `-40 dB/s`. The squared-amplitude energy
envelope falls to `10^-6` at the same time: use `20 log10` for amplitude and
`10 log10` for energy. Do not identify every instantaneous damped kinetic-plus-
potential energy sample with this envelope; its phase-dependent factor differs.
With `omega0 = 2*pi*f0`, the **synthetic** damping ratio is
`zeta = 0.003331525449451944`, the conventional oscillator
`Q = omega0/(2*sigma) = 150.0813989226026`, and the damped sinusoidal frequency is
`219.9987790998121 Hz`. This `Q` is calculated from the oscillator convention;
it is not a measured half-power-bandwidth estimate or an FRF peak frequency.

**Negative arithmetic control:** using `ln(10^6)/T60` as the amplitude coefficient
gives the **synthetic** wrong value `9.210340371976182 s^-1` and an amplitude
`T60 = 0.75 s`. This catches the amplitude/energy factor-of-two error without
running the engine.

### Why fitting more coefficients cannot assign the mechanism

The current [StringLossLaw](../../include/setar/instrument/config.hpp) is

\[
\sigma_n=a_0+a_2k_n^2+b_2\omega_n^2.
\]

Its fields are `decay_constant_per_s`,
`decay_wavenumber_squared_m2_per_s`, and `decay_frequency_squared_s`, with units
`s^-1`, `m²/s`, and `s`. For the flexible-string limit,
`omega_n² = c²*k_n²`, so decay only constrains `a0` and `a2 + c²*b2` at fixed
`c² = T/mu`. The sensitivity columns for `a2` and `b2` are proportional.
Nonnegative bounds select an admissible family; they do not identify its members.
Nonzero stiffness can break exact dependence, but weak curvature and noisy decay
estimates can leave practical ambiguity. A changed tension helps only if its
independent value and the assumption of shared loss coefficients remain valid.

Likewise, one acoustic envelope cannot distinguish intrinsic string loss, bridge
damper loss, body loss, player contact, or observation cancellation. Energy may
leave one string while remaining in another mechanical subsystem. A sum of
beating partials can have an apparent decay unlike any one pole. A body-mediated
effective decay fitted into the string and then reproduced by the coupled body
counts the same explanation twice.

### Disposition and operational handoff

**Accepted for synthetic analytic scope (`supported_with_scope`):** the conversion
between the declared amplitude envelope and `sigma`. A proposed isolated fixture
may use `a0 = 4.605170185988091 s^-1`, `a2 = 0`, `b2 = 0`, all synthetic.
**Rejected promotion:** treating that value as a measured intrinsic string
property. **Inconclusive (`unresolved`):** which physical subsystem owns the loss.

Before fitting, compare synchronized mechanical string motion, bridge motion, and
pressure with documented support/contact states. Isolate partials and check
window/noise-floor sensitivity; retain beats and excluded windows. Compare a
string-only reference, controlled termination states, and the coupled body model.
Freeze an independent session or condition for prediction. If the added channels
still cannot distinguish candidates within the propagated uncertainty, retain a
scoped effective law and stop causal parameter promotion.

The implementation uses `k_n = n*pi/StringConfig.total_length_m`, not the speaking
span `bridge_position_m`. In [Instrument::prepare](../../src/instrument/instrument.cpp),
the string loop truncates at the first mode with `sigma >= omega` and fails if no
mode remains; the [modal bank](../../src/instrument/numerics.cpp) rejects an
overdamped mode supplied directly. Preserve the retained mode count in a future
comparison instead of silently changing the fitted band.

Existing `core.wavenumber_loss` in [reference_tests.cpp](../../tests/reference_tests.cpp)
and `solver.modal_exact_free_response` in [numerics_tests.cpp](../../tests/instrument/numerics_tests.cpp)
provide synthetic numerical checks. They were inspected, not executed here, and
do not assign specimen losses. Retain envelope definitions, raw channels, excluded
windows, calibration covariance, candidate profiles, and both loss-owner models.
Hand the unresolved loss budget to `ENG-P04-T01`/`ENG-P12-T01` before P09 coupling
or P10 explicit radiation loading reuses it.

## Example B — A passive body can have a negative cross response

### Question and declared ports

**Decision:** should a body fit with negative cross-mobility be rejected, or should
its residues be retained? The relevant scientific tasks are `P08-T04` and
`P08-T05`, mapped to `ENG-P08-T02`/`ENG-P08-T03`, under `REQ-BOD-002`,
`REQ-BOD-003`, and the coupling consequence `REQ-CPL-001`.

Use force entering the body and velocity in the same direction as conjugate
variables. Rows are velocity outputs; columns are force inputs. Define peak
phasors with `exp(i*omega*t)`. The **synthetic** single mode has mass
`m = 0.02 kg`, undamped frequency `f = 300 Hz`, damping ratio `zeta = 0.01`, and
dimensionless port participation `psi = (1, -0.5)`. For a future fixture, map its
ports to `bridge_participation[0][0]` and `bridge_participation[1][0]`, where the
second index denotes `Polarization::Vertical`; all other slots are synthetic
zeros. The first indices identify physical strings, not course labels.

\[
D=\Omega^2-\omega^2+i2\zeta\Omega\omega,\qquad
H_x=\frac{\psi\psi^T}{mD},\qquad Y=i\omega H_x.
\]

At the **synthetic** resonance `omega = Omega = 1884.955592153876 rad/s`,

\[
Y=\begin{bmatrix}
1.326291192432461&-0.6631455962162305\\
-0.6631455962162305&0.3315727981081152
\end{bmatrix}\ {\rm m/(N\,s)}.
\]

The Hermitian part `Y_H = (Y + Y*)/2`, with `*` denoting conjugate transpose, has
**synthetic** eigenvalues `0` and `1.657863990540576 m/(N s)`. More generally,

\[
Y_H=\frac{2\zeta\Omega\omega^2}
 {m[(\Omega^2-\omega^2)^2+(2\zeta\Omega\omega)^2]}\psi\psi^T\succeq0.
\]

This is an analytic passivity result for the declared damped modal realization,
including its stable poles; a frequency-grid check alone is weaker. For the
**synthetic** peak force `(0.001, 0) N`, the velocity is
`(0.001326291192432461, -0.0006631455962162305) m/s` and mean supplied power is
`F* Y_H F / 2 = 6.631455962162305e-7 W`. The **synthetic** force
`(0.001, 0.002) N` has `psi^T F = 0`: it excites no mode in this rank-one fixture.
Neither a negative off-diagonal entry nor this null direction implies activity.

### Residues and normalization are part of the finding

The **synthetic** second-order receptance numerator is
`psi*psi^T/m = [[50, -25], [-25, 12.5]] kg^-1`. It multiplies
`1/(s²+2*zeta*Omega*s+Omega²)` in the Laplace-domain receptance, with `s` in s⁻¹.
At a simple pole `s_p`, the first-order residue is instead
`(psi*psi^T/m)/(2*s_p+2*zeta*Omega)`, in s/kg. Static driving compliance is
`H_x,11(0) = 1.407238661699136e-5 m/N`. Frequencies and damping alone supply
neither that compliance nor the modal numerator. A normalized shape image
supplies no calibrated force scale.

If the mode shape is multiplied by a nonzero dimensionless `c`, use
`psi' = c*psi`, `m' = c²*m`, and modal displacement `eta' = eta/c`. If pressure is
`beta*eta''`, transform `beta' = c*beta` too. These transformations preserve both
mechanical transfer and observation; a mode-sign reversal is the special case
`c = -1`. Changing mass without participation changes physical loading. Mass and
shape scale are not separately identifiable from these port FRFs until a
normalization convention or independent constraint fixes that freedom.

**Negative controls:** replacing the negative cross numerator by its absolute value
can remain passive while predicting the wrong cross-port phase. Positive driving
diagonals alone also fail as a test: a symmetric matrix can have cross terms large
enough to give a negative eigenvalue. Preserve calibrated phase and the full
matrix; never clip individual negative entries to “repair” passivity.

### Disposition and handoff

**Accepted for synthetic analytic scope (`supported_with_scope`):** retain the
negative cross response of this passive fixture. **Rejected:** importing a
frequency list as a complete body or correcting cross signs without evidence.
**Inconclusive (`unresolved`):** whether any real specimen has these residues.

Map the candidate to existing `BodyMode.frequency_hz`, `damping_ratio`,
`modal_mass_kg`, and `bridge_participation` in [config.hpp](../../include/setar/instrument/config.hpp).
The current [bridge_mobility](../../src/instrument/body_model.cpp) evaluates this
modal form. Existing `body.mobility_reciprocity` and `body.mobility_passivity` in
[body_tests.cpp](../../tests/instrument/body_tests.cpp) inspect synthetic behavior;
the latter samples a frequency grid. No new fixture or test result is claimed.

The next empirical packet needs calibrated complex force/motion channels, paired
sensor/support controls, shared poles, residue covariance, and a normalization
ledger. Reserve a cross-port channel or separate state for prediction. Stop
promotion if reciprocity/phase discrepancies exceed the predeclared uncertainty
budget or depend on sensor/support changes. Retain failed fits, unmodified
responses, passivity corrections, and withheld predictions. `ENG-P08-T02` owns
acquisition and `ENG-P08-T03` owns reduction; proposed work-order paths such as
`src/body/reduction.cpp` are not a second existing implementation. Acoustic
observation and unique material inversion remain separate gates.

## Example C — An observation weight cannot implement an acoustic load

### Question and calculation

**Decision:** can a microphone mismatch be corrected with
`BodyMode.radiation_weight`, and would doing so establish correct radiation loss?
Scientific `P10-T04`/`P10-T05` route through `ENG-P10-T02`, with `REQ-ACO-002`;
feedback also involves `ENG-P10-T01`, `REQ-ACO-001`, and `REQ-CPL-001`.

Reuse the explicitly **synthetic** mode from Example B and choose the
**synthetic** observation coefficient `beta = 0.01 Pa*s²/m`. In the continuous
target relation,

\[
p=\beta\ddot\eta,\qquad
\frac{P}{F_j}=-\frac{\omega^2\beta\psi_j}{mD}.
\]

For its first port at the **synthetic** resonance, `P/F1 = i*25 Pa/N`. A
**synthetic** peak force `0.001 N` therefore gives pressure amplitude `0.025 Pa`
under this chosen observation. Those are analytic outputs, not calibrated
microphone data. The pressure-to-force transfer is not a conjugate driving
mobility; its real part need not be nonnegative. Apply passivity to the physical
loading ports, not to this microphone transfer.

The current [engine observation](../../src/instrument/instrument.cpp) uses the
modal second difference divided by the squared time step, multiplied by
`radiation_weight` and `ProcessSpec.output_gain_per_pa`. It does not use this
continuous frequency expression exactly at every sample rate. A future discrete
comparison must account for the second-difference frequency response and modal
time stepping, then demonstrate refinement. Analytic arithmetic here is not
evidence that a render attains the displayed pressure.

### Which hypothesis the experiment can decide

The **proposed** experiment keeps the instrument, excitation, support, and
mechanical sensing fixed while changing microphone position or its calibrated
observation chain. Preserve force-to-velocity and force-to-pressure responses
with synchronized clocks. If mechanical responses agree within uncertainty and
pressure differs, an observation change is a candidate. This does not by itself
identify which radiation, microphone, room, or placement term caused it.

If the actual structural load changes, changed poles, mechanical decay, or
mobility cannot be repaired by changing a pressure weight. Test a separately
justified loading hypothesis. Record any apparatus disturbance when moving a
microphone so that the supposedly observation-only control remains interpretable.
Matching one pressure spectrum while mechanical residuals persist rejects the
proposed observation-only explanation of those residuals.

| Current coefficient can represent | It cannot generally represent |
| --- | --- |
| A fixed signed real observation coefficient for each retained mode at each existing output channel | Arbitrary frequency-dependent phase or propagation delay |
| Modal cancellation and a limited calibrated gain relation at declared positions/band | Continuous microphone motion, general frequency-dependent directivity, or room/microphone transfer without additional states/operators |
| Different outputs from unchanged physical state | Acoustic reaction force, cavity loading, radiation impedance, or a changed mechanical decay rate |

Negative weights can express observation cancellation. Altering them can change
the apparent envelope of a modal mixture while its mechanical state is unchanged.
That is another reason not to assign intrinsic loss from a pressure envelope.

### Disposition, extension boundary, and handoff

**Accepted (`supported_with_scope`):** the analytic distinction between
observation and loading. **Rejected:** promoting a fitted pressure weight as a
radiation-damping parameter. **Inconclusive (`unresolved`):** specimen observation
calibration, because no calibrated pressure/motion data exist in this example.
No weight is installed.

Existing `acoustics.observation_invariance` in
[engine_tests.cpp](../../tests/instrument/engine_tests.cpp) changes weights/gain
and checks unchanged mechanical ledger/state with changed output. It is a current
synthetic test definition, not a new execution or acoustic validation. Proposed
negative controls are zero weights with nonzero mechanics, a sign reversal, and
an observation-only change that must leave the force/velocity transfer unchanged.

If constant weights fail held-out complex pressure responses, propose a causal,
stable observation filter with explicit delay, units, location, calibration, and
state-transition rules under the [radiation reduction contract](../acoustics/05-radiation-reduction-and-runtime-transfer.md).
Keep feedback absent in that observation extension. A separate acoustic load
extension needs conjugate ports, a passive realization, and a coupled work ledger.
It also requires reassessing body damping already used as an effective loss
allowance, or explicit radiation can be counted twice. Neither extension is a
present field in `BodyMode`; `src/acoustics/radiation.cpp` is a proposed work-order
path, not the current implementation.

Stop a fit when pressure calibration, phase, position, or boundary state cannot
be reconstructed. Retain original complex responses, rejected observation/load
candidates, calibration covariance, and a separately reserved microphone position
or session. Hand `ENG-P10-T02` the observation finding and `ENG-P10-T01`/P09 the
unresolved loading claim; no microphone match closes both gates.

## Example D — A bright attack does not uniquely identify nail stiffness

### Question and rival predictions

**Decision:** does an overly bright attack justify changing the nail law?
Scientific `P05-T01`/`P05-T02` map to `ENG-P05-T01`/`ENG-P05-T02`, under
`REQ-EXC-001` and `REQ-EXC-002`. Identification and held-out decisions also require
`ENG-P12-T01`/`ENG-P12-T02`, `REQ-IDN-001`/`REQ-IDN-002`.

Define brightness through a preregistered partial-energy ratio or spectral moment
with a declared band, window, release reference, and noise-floor policy. Keep
mechanical and pressure descriptors distinct. No universal numerical brightness
threshold, safe force, camera rate, or repetition count is supplied here.

| Rival cause | Discriminating observation and control | Decision consequence |
| --- | --- | --- |
| Position or physical patch width | Measure longitudinal contact position and patch geometry while matching trajectory; compare released modal motion | A systematic spatial excitation mismatch motivates geometry/patch work, not an arbitrary increase of stiffness |
| Approach speed or contact duration | Measure force, relative gap, nail motion, release interval, and string motion; vary speed while preserving observed geometry | Fit the normal law only if it predicts force-gap behavior and withheld release states |
| Tangential friction or changing normal | Resolve tangential motion/force or explicitly qualified inverse estimates; compare controlled direction/angle changes | Reproducible unexplained tangential work motivates a friction/geometry extension; brightness alone is insufficient |
| Body/observation or estimator | Simultaneous mechanical and pressure response; fixed mechanical input with alternative observation; vary analysis window | A pressure-only difference routes to P08/P10 or analysis, not automatically to nail coefficients |
| Previous string motion | Record approach-time displacement/velocity and phase; compare initially resting and ringing states | Preserve state-dependent interaction rather than resetting the string to reproduce an onset |

### Proposed discriminating protocol

1. Freeze specimen/setup, string/tuning, support, nail condition, intended action,
   and permission scope using the [contact/motion protocol](../measurement/pluck-contact-and-motion.md).
   Use a mechanical surrogate for controlled geometry changes where appropriate;
   surrogate success does not establish natural playing validity.
2. Calibrate force, motion axes, length scale, and microphone channels. Record
   force-sensor and fixture loading. Synchronize original clocks with a retained
   offset/drift model and uncertainty; use additional views when one view cannot
   resolve the contact plane. Never align each take to the model's predicted
   release to hide timing error.
3. Acquire matched position, speed, and direction contrasts in counterbalanced
   order. Preserve actual rather than merely intended trajectories and initial
   string state. Keep the observation fixed during mechanical contrasts; keep
   mechanics fixed for the observation control. A direct force record and a
   force inferred from string motion have different evidence labels.
4. Annotate contact/release as intervals when visibility or temporal resolution
   requires it. Compare signed actuator work, force-gap relation, release timing,
   and released displacement/velocity before judging pressure brightness. Fit
   only candidates the observed channels can distinguish.
5. Reserve whole trajectories and a separate session where feasible; do not split
   neighboring frames or overlapping onset windows across fit and validation.
   Predict a withheld speed/angle/position condition with frozen coefficients.
   Choose uncertainty-based acceptance and the minimum relevant effect before
   inspecting that condition. Selection on a held-out result makes it development
   data and requires a new holdout or a narrower claim.

Stop physical stiffness claims if force is unavailable and inverse assumptions
remain unchecked. Stop timing claims below the acquisition uncertainty, and stop
natural-performance claims when instrumentation changes the action. Retain
occluded, clipped, and failed takes with exclusions under their permission scope.
No-contact motion, a resting-versus-ringing comparison, unchanged mechanics with
altered observation, and timestep/patch-resolution refinement are negative
controls; they are proposed, not executed here.

### Synthetic contact arithmetic and current implementation boundary

For separated gap `g > 0`, set indentation `delta = max(0, -g)`. With
`F = K*delta^alpha` and `Phi = K*delta^(alpha+1)/(alpha+1)`, choose the
**synthetic** `alpha = 1.5`, `delta_ref = 0.0005 m`, and `F_ref = 0.05 N`.
Independent arithmetic gives the **synthetic**
`K = 4472.13595499958 N/m^1.5` and `Phi(delta_ref) = 1e-5 J`.
This is stored potential, not automatically actuator work or dissipated release
energy. Comparing exponents should hold force at a reference indentation fixed;
their differently dimensioned `K` values cannot be compared as one stiffness.

Current [ContactLaw](../../include/setar/instrument/config.hpp) has `stiffness`,
`exponent`, and `damping_n_s_per_m`. [GestureEvent](../../include/setar/instrument/gesture.hpp)
has `position_m`, `angle_rad`, `speed_m_per_s`, `force_n`, `gap_m`, and `travel_m`.
It drives a point contact along a prescribed straight path, releasing at force or
travel limits; removed contact potential is booked as release dissipation. It has
no physical patch-width parameter, dynamic nail mass, nail-friction state, or
geometric slip-off law. Those are proposed extensions requiring their own work
and convergence derivation. Existing `solver.discrete_gradient_identity`,
`contact.nail_passivity`, and `contact.release_convergence` in the
[numerics tests](../../tests/instrument/numerics_tests.cpp) and
[engine tests](../../tests/instrument/engine_tests.cpp) check synthetic numerical
properties, not identified human contact.

**Accepted for synthetic analytic scope:** the potential and unit conversion.
**Rejected promotion:** inferring physical nail stiffness from brightness alone.
**Inconclusive (`unresolved`):** the winning attack mechanism until the protocol
has evidence. Retain raw synchronized channels, annotation disagreement,
calibrations, fitted alternatives, and failed predictions. `ENG-P05-T01` owns
the minimum discriminating acquisition; `ENG-P05-T02` receives a law/geometry
change only after that decision, with old synthetic behavior preserved as a
regression fixture.

## Retained evidence and closure

For these worked examples, retained evidence is the declared inputs, equations,
computed values, inspected source/test paths, and stated decisions in this file.
There are no raw specimen assets or calibration hashes to populate. Future runs
must preserve immutable raw identities, derived-input/output hashes, actual
commands and analysis revisions, failed candidates, partitions, and reviewer
disposition under the [data contract](../measurement/data-contracts-and-provenance.md).
Keep restricted recordings and permissions outside public evidence packages.

Every example leaves the measured specimen gate open. A future promotion must
name the changed fields/equations, loss owner, modal normalization, observation
meaning, validity domain, uncertainty, and rollback package. The current coarse
`Provenance` enum cannot encode this per-quantity lineage; retain a versioned
evidence sidecar, and do not relabel a mixed synthetic/measured configuration as
wholly measured. Changing calibration, sign, normalization, geometry, or meaning
invalidates downstream fits until recomputed.

Documentation checks for this chapter consist of analytic arithmetic, local
link/identifier verification, and whitespace review. Source/test inspection
establishes where a future check belongs. It is not a record that those checks,
the proposed acquisitions, or an engine comparison were run.
