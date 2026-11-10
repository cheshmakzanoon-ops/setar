# P05 — Model fingernail excitation and physical release

**Status:** specification. **Prerequisites:** P04 and verified contact numerics.
**Exit evidence:** bounded contact behavior, correct work transfer, reproducible
release, and gesture-dependent predictions evaluated against relevant evidence.

## What makes this a setar task

The setar's right-hand nail interaction is central to its attack and articulation.
A generic random burst into a delay line is not a measured model of that contact.
At the same time, prescribing an arbitrary elaborate nail geometry without data
does not make the result accurate. Begin with a limited model whose parameters,
observables, and failure modes can be tested.

Read [contacts](../contacts/README.md), [performance](../performance/README.md),
and [measurement](../measurement/README.md). Consult practitioner descriptions
and controlled recordings to establish stroke direction, contact sequence, and
which strings are intentionally contacted. Do not infer the entire gesture from
the audio waveform alone.

## Entry checks and research map

The current prototype is a useful synthetic baseline: point nail contact at a
fixed longitudinal position, a prescribed straight transverse path, a one-sided
power potential, and force-threshold or travel-limit release. Its removal of
remaining contact potential is booked as release dissipation. These are explicit
model choices, not measured nail geometry or a verified geometric slip-off law.
There is no nail friction state or dynamic finger/nail mass. Read the current
[gesture contract](../../include/setar/instrument/gesture.hpp),
[configuration](../../include/setar/instrument/config.hpp), and
[modal-contact derivation, §§2–5](../numerics/15-modal-contact-solver.md)
before proposing replacements.

| Research route | Exact source and reading location | Use and boundary |
| --- | --- | --- |
| Intended action | `R-SETAR-TABASSIAN`, [performer handout](https://www.atlasensemble.nl/assets/files/instruments/Setar/Setar%20by%20Kiya%20Tabassian.pdf), pp. 1–2 | Elicitation of direction, string selection and tuning context; no contact coefficients or measured trajectories. |
| Normal contact and reference numerics | `R-CONTACT-BILBAO-2014`, [versioned preprint](https://arxiv.org/abs/1405.2589v1), §2 and §4; `R-CONTACT-BILBAO-TORIN-2014`, [DAFx paper](https://dafx.de/paper-archive/2014/dafx14_stefan_bilbao_numerical_simulation_of_s.pdf), §§2–3 | Potential/work and independent discretization methods; other-instrument examples do not identify setar material laws. |
| Uncertainty and fitting | `R-METROLOGY-JCGM-100`, [official GUM](https://www.bipm.org/documents/20126/2071204/JCGM_100_2008_E.pdf), §§4, 5, 7; `R-IDN-RAUE-2009`, [publisher record](https://academic.oup.com/bioinformatics/article/25/15/1923/213246), abstract and §2.3 acquisition lead | Measurement equation, correlated errors, identifiability; preserve the atlas's limited access status for unread methods. |
| Specimen evidence | [Pluck/contact/motion protocol](../measurement/pluck-contact-and-motion.md), especially “Synchronization,” “Contact annotation,” and “Nail and finger geometry”; [campaign packets](../measurement/13-acquisition-campaign-execution.md) | Future `excitation.nail_flesh`, `gestures.performer`, and `inference.heldout_split` acquisition; none is supplied by the synthetic preset. |

For each recipe freeze the source revision, configuration, retained modes, sample
rate, initial ringing state, event timeline, observation position and numerical
limits. Vary temporal resolution at a common retained bandwidth before separately
varying modal resolution. Set tolerances from the intended observable, measurement
uncertainty and independent reference error; newly chosen thresholds are proposed
acceptance budgets, not published setar facts. Each packet records accepted,
rejected, inconclusive or blocked findings, including excluded trials and reasons.
Missing specimen access blocks empirical promotion while synthetic verification
continues. These instructions authorize no participant contact or new acquisition.

## P05-T01: define the contact geometry and kinematics

Specify the string coordinate, nail trajectory, normal direction, contact gap,
and local tangential motion. Decide whether the nail trajectory is prescribed,
driven by a compliant finger model, or measured as an input for a controlled
experiment. Those are different boundary conditions and should be selectable
test configurations rather than mixed implicitly.

Declare how a finite contact region is represented. A point-contact limit is
useful for verification, but the contact width may affect high-frequency
excitation. If an interpolation kernel spreads force across a grid, distinguish
numerical support width from a physical contact patch. Verify that observing
velocity and distributing force are adjoint operations under the chosen inner
product.

Define release geometrically or mechanically. A note-on envelope is not a
physical release rule. The transition must avoid injecting an arbitrary
discontinuity into string state. Store enough contact state to replay the event
deterministically and to distinguish a nail passing over a ringing string from
exciting a silent one.

### Execution recipe

**Question and route.** Determine the smallest geometry that predicts engagement
and release position across a declared stroke range. Read
`R-SETAR-TABASSIAN` pp. 1–2 for action context and
`R-CONTACT-BILBAO-2014` §4 for a distributed interaction method, then
[finite nail contact](../contacts/01-finite-nail-contact.md), “Define a signed gap
once” through “Moving interpolation carries work.” Search `"setar" fingernail
plucking`, `"سه‌تار" مضراب ناخن`, and `nail string contact geometry release`.
Record which descriptions are observed, proposed or inaccessible; none determines
nail curvature or stiffness for this specimen.

1. Establish nut-origin `x` in metres, vertical positive away from the soundboard,
   horizontal positive toward string 3, and the calibrated camera-to-instrument
   transform. Capture permitted scaled views of natural nail geometry and motion,
   or use a declared mechanical surrogate. Retain edge curvature, surface region,
   orientation in radians, uncertainty, frame timestamps and occluded intervals.
   Fit clock offset/drift from synchronization markers, not from desired release.
2. In the current point model write `z = d·u(x,t) - u_n(t)` with
   `d = (sin(angle_rad), cos(angle_rad))`; `z > 0` means separation. Reconstruct
   approach, contact and release intervals independently of audio. Compare a
   straight prescribed path with a measured path before adding finger dynamics.
3. For a **proposed** finite patch use normalized `w_a` in 1/m and
   `B_i = integral(w_a*phi_i dx)`, dimensionless. Hold physical width `a` in metres
   fixed as quadrature/mode count changes. Compare against the point limit; test
   `F*(B*q_dot) = (B^T*F)·q_dot` and include `B_dot*q`/rotating-normal work.
   A numerical interpolation footprint is not a measured contact patch.

**Translation and acceptance.** Map supported point-path quantities to
`GestureEvent::{position_m,angle_rad,speed_m_per_s,gap_m,travel_m}` and
`Instrument::Impl::apply`/`assemble` in
[instrument.cpp](../../src/instrument/instrument.cpp). Current `NailStroke`
admission requires `0 < position_m < strings[s].bridge_position_m` from the
selected string's nut origin; an afterlength stroke needs a new event contract.
Curved surfaces, migrating
patches and geometry-based release require proposed state/interface extensions;
do not encode them as unexplained changes to `force_n`. Retain raw views,
transforms, annotated gap/path plots, normalized-kernel checks and the model-choice
record. A second annotator and an independently evaluated analytic plane/string
intersection must agree within the calibration/time budget. Use a geometric miss,
reversed normal and rotated symmetric fixture as negative controls. Stop when
occlusion prevents resolving the claimed separation. `contact.gesture_replay`
checks existing synthetic determinism, not recovered geometry. Handoff through
`ENG-P05-T01` (`REQ-EXC-001`, `REQ-EXC-002`) to P05-T02 and P05-T06.

## P05-T02: establish a passive normal contact reference

Choose a nonpenetration or compliant penalty model with an explicit stored
potential and, if used, dissipative term. Derive force from the potential with
the declared gap sign. Test no-contact, first contact, increasing compression,
unloading, and separation. Penalty stiffness is not merely a brightness knob;
it affects penetration, numerical stiffness, and solver requirements.

Implement the reference solver following the verified numerical contact chapter.
Check the discrete work balance and the small-contact/rigid-contact limits
appropriate to that scheme. Separate numerical convergence from the physical
choice of penalty law. A solver that stays stable by excessive numerical damping
may erase the very attack behavior being investigated.

Include deliberately difficult trajectories and rejected parameter combinations.
The reference run must report residuals and convergence state. The real-time
containment policy is a later engineering requirement; it must not redefine a
failed solve as a normal attack.

### Execution recipe

**Question and route.** Can the selected normal law transfer the prescribed work
and converge in force, penetration and release state without hidden energy?
Inspect `R-CONTACT-BILBAO-2014` §2, especially energy balance and numerical
integration, and the repository's
[solver equations C1, N1 and E2](../numerics/15-modal-contact-solver.md).
Search `energy conserving unilateral contact power law penetration release` for
independent methods, keeping their schemes distinct from this modal solver.

1. Declare `z = B*q - u_n`, penetration `eta = max(0,-z)` in metres and
   `V = K*eta^(alpha+1)/(alpha+1)` in joules. `K` is in N/m^alpha,
   `alpha >= 1` for the mathematical family considered here; damping `R` is in
   N·s/m. The current `ContactLaw` validator in
   [config.cpp](../../src/instrument/config.cpp) admits only finite
   `1 <= alpha <= 4`, finite positive `K` and finite nonnegative damping. A fitted exponent
   outside that implementation range requires a separately verified law/solver
   extension or a justified restriction of the candidate domain; do not clip it
   and retain the original fit claim. Changing the exponent changes the
   coefficient's units. Fit physical compression only from a suitable force/gap
   experiment; treat a rigid-obstacle penalty as a numerical approximation.
2. Start with a lumped mass/obstacle and independently integrate the conservative
   orbit using energy quadrature. Then use an isolated flexible string (`EI=0`)
   with fixed endpoints and a known point-compliance comparator
   `u_release = F_release*x*(L-x)/(T*L)` in the slow limit. Here `L` is that
   isolated span. The present instrument test approximates this with a stiff
   grounded bridge and `L=bridge_position_m`; its finite bridge stiffness,
   nonzero bending rigidity, modal truncation and stroke speed require a separate
   discrepancy budget. Removing body loss alone does not impose fixed endpoints.
   Include separation, grazing, loading, unloading, repeat contact and zero damping.
   Sweep stiffness and step size separately; hold the physical law and mode set
   fixed when assessing temporal convergence.
3. Compare test-side potential differences with `numerics::channel_force`,
   `channel_potential_j` and `SmallNewtonSolver::solve` in
   [numerics.cpp](../../src/instrument/numerics.cpp). Record gap/force/impulse,
   release displacement and velocity, `NewtonResult::residual_m`, iteration cap
   and the ledger residual `E-E0-W+D-S` in joules. The step force spans two time
   levels; a separated endpoint alone does not justify zeroing interval work.

**Decision and handoff.** The present `NailPhase` transition removes contact at
`force_n` or `travel_m` and books residual potential as dissipation. Preserve this
synthetic release model as a regression fixture; test a proposed geometric release
separately. Independent scalar bisection and a converged time-domain reference
must support penetration, impulse and release-state budgets. Reject tensile
separated behavior inconsistent with the chosen law, unexplained work, or a
nonconverged solve; deliberately cap Newton as a failure control and retain the
trace. Keep law/version manifests, force–gap loops, convergence curves and each
disposition. Present CTests `solver.discrete_gradient_identity`,
`solver.newton_scalar_oracle`, `contact.nail_passivity`,
`contact.release_convergence` and `engine.newton_failure_reporting` are scoped
synthetic checks, not a measured finite-patch qualification. Route the packet
under `ENG-P05-T02` / `REQ-EXC-001` to P05-T03/T04 and P06-T02.

## P05-T03: introduce friction only with defined scope

If tangential friction is needed, state whether it represents nail/string
sliding, a simplified effective contact, or another mechanism. Specify static
and sliding regimes, regularization, and the corresponding dissipation. Do not
reuse bowed-string friction laws without evaluating whether their assumptions
apply to a brief nail interaction.

Verify the sign of frictional power and behavior near zero relative speed.
Discontinuities or regularizations can create stiffness and affect attack
spectra. Use energy accounting and trajectory-controlled experiments to isolate
those effects. A parameter sweep should show continuous physical transitions
where the underlying model predicts them, not discontinuities caused by a
branch condition or solver tolerance.

### Execution recipe

**Question and route.** Does tangential interaction explain a repeatable residual
after geometry, normal loading and observation are fixed? Use
`R-CONTACT-BILBAO-2014` §2 for the normal-energy reference and
`R-SETAR-TABASSIAN` p. 1 for direction elicitation; neither supplies a friction
coefficient. Read [friction, flesh and direction](../contacts/02-friction-flesh-direction.md),
“A minimal dissipative friction law” and “A controlled comparison design.” Follow
its Pavlidou–Richardson guitar and Chadefaux–Le Carrou–Fabre harp primary-source
leads, searching exact titles plus `nail tangential force slip release`. Retrieve
authorized full methods before using them; a failed download or abstract remains
an acquisition lead, and other-instrument coefficients remain inapplicable.

1. Freeze the P05-T01 geometry, normal law, string parameters and mechanical
   observation. With an authorized surrogate first, prescribe tangential motion
   in m/s under independently calibrated normal load in newtons. Measure both
   force components with bandwidth/cross-axis calibration; reverse velocity and
   vary load and speed separately. Record humidity, surface condition and sensor
   disturbance as possible confounders. Nail and flesh trials need separate
   geometry/region labels.
2. Compare the frictionless hypothesis with the **proposed** kinetic candidate
   `F_t = -mu_f*F_n*tanh(v_rel,t/v_epsilon)`, where `mu_f >= 0` is dimensionless
   and `v_epsilon > 0` is m/s. For nonnegative `F_n`, require
   `F_t*v_rel,t <= 0` in watts and integrate the lost work. Test zero load,
   zero speed, reversals, separation and vanishing regularization under refinement.
   Relative velocity must use the same moving/rotating geometry as the gap.
3. Only add static stick state if resolved hysteresis/capture defeats the kinetic
   model. Specify stored tangential energy, yield, frame rotation and release
   disposal before implementation; compare against an independent constrained
   lumped reference. Without torsion, label the translational surface reduction.

**Translation and gate.** Current `ContactLaw`, `ChannelLaw` and `NailState`
have no tangential friction state. The `ContactConfig` fret stick spring is a
separate synthetic surrogate, not nail friction. A proposed coupled tangential
channel must extend state ownership, solver/Jacobian and `EnergyLedger`; changing
`nail.damping_n_s_per_m` is not that implementation. Retain calibrated force/slip
traces, power plots, held-out load/speed predictions, parameter uncertainty and
accepted/rejected/inconclusive alternatives. Independently recompute friction
work; reject positive passive power or nonconvergent regularization effects.
`contact.nail_passivity` currently verifies only normal-contact behavior; friction
acceptance is a proposed extension under `ENG-P05-T02` / `REQ-EXC-001`, handed to
P05-T04/T06 and P11. If geometry and friction remain confounded, retain the simpler
model and identify the missing observation.

## P05-T04: validate the excitation spectrum

Start with an ideal triangular-displacement pluck as an analytic test of string
observation and spatial excitation. Expected harmonic cancellations depend on
pluck position. Use that limiting case to reveal indexing, normalization, and
boundary errors before interpreting a nail simulation. Do not label the
triangular fixture a realistic fingernail model.

Then compare physical contact outcomes across pluck positions, depths, speeds,
directions, and initial ringing states. Track delivered work, initial modal
distribution, attack time, and high-frequency content. Keep microphone/body
effects out of the isolated-string test. Once coupled, examine whether the same
contact parameters predict held-out attacks rather than refitting each note.

### Execution recipe

**Question and route.** Which changes in attack follow from pluck location and
contact work, and which require a different mechanism? Read
`R-NUM-BILBAO-2009` through the [source atlas](../research/source-atlas.md), using
an authorized copy's ch. 7, “Linear bar and string vibration,” and recording the
exact modal-excitation subsection inspected; read `R-CONTACT-BILBAO-2014` §4 for
finite-duration excitation methods.
Search `triangular pluck Fourier sine coefficients harmonic cancellation` and
`nail pluck position contact duration measured spectrum`; use the
[motion protocol's excitation descriptors](../measurement/pluck-contact-and-motion.md)
for specimen comparisons.

1. For an isolated flexible string of length `L`, height `a` and pluck coordinate
   `x_p`, independently integrate the triangle to obtain
   `A_n = 2*a*L^2*sin(n*pi*x_p/L)/(n^2*pi^2*x_p*(L-x_p))` in metres.
   At `x_p = L/2` even coefficients vanish; at `L/3` every third does.
   Compare quadrature and modal reconstruction, including a slightly displaced
   pluck as a control. Distinguish an excitation zero from a sensor located on
   a modal node by using multiple observation positions.
2. Exercise `reference::ModalString::pluck` in
   [reference.hpp](../../include/setar/reference.hpp) against test-side quadrature,
   not against its own formula. Here `StringParameters.length_m` is the complete
   isolated span. A sharp triangle has divergent continuum bending energy when
   `EI > 0`; use `EI = 0` for that energy limit or declare a smoothed finite-width
   profile. The coupled instrument's speaking length is `bridge_position_m`,
   not `total_length_m`.
3. Replace initialization with `GestureKind::NailStroke` on the persistent
   instrument. Sweep `position_m`, speed, direction, force/travel release and
   controlled prior phase/amplitude while keeping string/body/observation fixed.
   Save input work, release state, attack duration, partial energies and defined
   in-band spectral metrics. Compare equal supplied work where appropriate;
   retain physically scaled output separately from listening-level matching.

**Acceptance and translation.** Preserve raw mechanical/acoustic channels,
calibration and synchronization, window/filter choices, mode/sample-rate sweeps
and uncertainty in the attack metrics. Independent quadrature and a converged
contact reference must explain the synthetic result before held-out measured
positions/speeds are evaluated without refitting. `core.pluck_projection` and
`core.convergence` in [reference_tests.cpp](../../tests/reference_tests.cpp)
check the initializer; `contact.release_convergence` is a limited point-release
check. `contact.strike_speed_envelope` remains a planned acceptance ID. Reject
observation-node, body-damping or bandwidth confounds before changing
`contacts.nail` or `PerformanceSettings`. Record the decision under
`ENG-P05-T03` (`REQ-EXC-002`, `REQ-PRF-002`) and hand residuals to P05-T06/P12,
with the isolated fixture retained for P09 coupling regression.

## P05-T05: multiple strings and directional strokes

A sweep may contact several strings at distinct instants and depths. Derive
those times from a trajectory/geometry model or a declared performance
abstraction. Do not fire identical independent note-ons at the same sample and
call that a strum. The shared physical body continues to couple strings even
when only one is directly touched.

Test an inward stroke, outward stroke, a missed string, a partial sweep, and a
repeated pluck while a previous vibration remains. Verify that string state is
not reset on every contact. A contact may add or remove energy depending on
relative motion; the work ledger must explain the outcome. Randomization, if
later added, must represent a documented distribution of gesture variability
and be reproducible from a seed.

### Execution recipe

**Question and route.** Can one documented sweep explain encounter order and
different work delivered to four already-existing strings? Read
`R-SETAR-TABASSIAN` pp. 1–2, then
[multi-string sweeps](../contacts/03-multi-string-sweeps.md), “Geometry establishes
order,” “Timing at and between samples,” and “Energy accounting across encounters.”
Search `"سه تار" راست چپ`, `"setar" paired course stroke`, and
`multi string plucking trajectory contact timing`. Acquire geometry and timing
through the campaign's `excitation.nail_flesh` and `interactions.cross_string`
packets when authorized, retaining uncertain or unintended encounters.

1. Build a synthetic flat-array oracle with string lateral coordinates `y_i` in
   metres and a straight nail path `y(t)=y_0+v*t`. Predict candidate encounter
   times `(y_i-y_0)/v` in seconds, then verify finite-surface intersections and
   gap signs independently. Reverse direction; change one height/spacing; include
   a missed string, partial sweep, near-simultaneous pair and invalid path.
   Propagate spacing and clock uncertainty to order/timing intervals.
2. Freeze per-string properties and start with body coupling absent. Give one
   string a known ringing state before the sweep and vary its phase. Compare
   per-string displacement/velocity, contact force and signed work; a valid
   restrike may remove energy. Then restore the same passive shared body to
   distinguish direct nail work from later sympathetic transfer.
3. Replay the absolute event timeline with different block partitions, including
   a sweep spanning boundaries. Record `sample_offset`, `sequence`, rejected
   events and channel-capacity faults. Test a declared contact-capacity exceedance
   separately from a geometric miss; neither may silently become a successful
   physical sweep.

**Current mapping and handoff.** `PerformanceSettings::course_sweep_delay_s`,
`parse_score` and `schedule` in
[performance.cpp](../../src/instrument/performance.cpp) implement a deterministic
scheduling abstraction; they do not solve a measured global nail/array geometry.
`Instrument` owns one nail state per physical string. Shared surface geometry,
parent-gesture identity and a reactive nail are **proposed** extensions, with their
own total work/state contract. Retain the array/path manifest, independently
predicted intersections, order/force/work plots and approximation-error table.
Present `score.course_sweep`, `contact.gesture_replay`, `realtime.block_partition`,
`performance.restrike_memory`, `coupling.cross_string_transfer` and
`coupling.uncoupled_limit` cover synthetic subsets; they do not qualify measured
encounter timing. Accept only order/work errors within the declared uncertainty
and numerical budget; reject state resets or unreported order-dependent energy.
Handoff through `ENG-P05-T03` and `ENG-P11-T01` (`REQ-EXC-002`, `REQ-PRF-002`,
`REQ-PRF-001`, `REQ-CPL-002`) to P06/P11 and the P12 held-out sweep study.

## P05-T06: data and acceptance

Use synchronized gesture/force observations where feasible, with clear limits
on what the measurement can resolve. Avoid inferring nail stiffness from an
audio spectrum while simultaneously changing body damping and microphone EQ.
That inverse problem may be underdetermined. Use separate experiments and
sensitivity analysis to choose identifiable parameters.

Accept the component when contact mechanics, work transfer, release, repeated
interaction, and directional behavior pass their tests within a declared domain.
Authenticity needs practitioner and measurement evidence; mathematical contact
verification alone is insufficient.

> Implement the specified nail-contact experiment without resetting the string
> or substituting prerecorded attacks. State the gap/force sign, contact law,
> trajectory, solver, and release rule. Show work and energy before, during,
> and after contact; include no-contact and failed-solve cases. Compare spectra
> at several physical pluck positions and label all synthetic assumptions.
> Do not add uncontrolled humanization to make an invalid model sound natural.

Handoff contact-state and gesture interfaces to P06 and P11. Preserve isolated
tests so later body coupling cannot hide an excitation regression.

### Execution recipe

**Question and route.** Which contact parameters are supported by independent
measurements, and over what stroke/initial-state domain? Read
`R-METROLOGY-JCGM-100` §§4, 5 and 7 and the accessible
`R-IDN-RAUE-2009` identifiability material, acquiring the full method before
claiming its reproduction. Use [parameter identifiability](../research/parameter-identifiability.md)
and the [acquisition campaign](../measurement/13-acquisition-campaign-execution.md).
Search `nail string force motion synchronized calibration` and
`contact stiffness release threshold profile likelihood`; record missing raw data,
not just missing publications.

1. Specify specimen/setup/session IDs, permitted uses, calibrated force/motion
   axes, sensor transfer functions and timestamps. Allocate geometry and time
   uncertainty before choosing a camera or fitting contact. Pilot sensor loading
   and compare minimally instrumented conditions. Annotate engagement/release as
   intervals with an independent reviewer; retain occlusion, clipping, clock drift
   and excluded trials. Do not contact participants or collect data under this
   documentation task; if access is absent, deliver the precise acquisition packet.
2. Freeze string parameters, body/observation treatment and P05-T01 geometry.
   Use normal force versus penetration to constrain `K, alpha`, speed-varied
   force/displacement loops for loss, and independent detachment observations for
   release hypotheses. Integrate compatible signed force/velocity to estimate
   work with calibration covariance. Inferred force remains an inverse-model
   quantity, never a direct measurement.
3. Scale sensitivities by input/observation uncertainty and compare competing
   combinations of stiffness, patch width, path and release threshold. Reserve
   complete sessions or stroke conditions before fitting; repeated windows from
   one take stay together. Test recovery on a declared synthetic fixture, a
   frictionless null case and intentionally ambiguous audio-only observations.
   If parameter directions remain flat or held-out errors exceed the stated
   budget, retain intervals/unknowns and choose the next discriminating channel.

**Reviewable result.** Deliver the proposed
`data/gestures/<session-id>/manifest.json` and
`experiments/nail-contact-campaign.md` packets with raw hashes, calibration,
source sections, annotations, split manifest, fitted covariance, residual plots
and each disposition. Map supported quantities to `ContactConfig::nail`,
`GestureEvent` and the proposed geometry/friction extensions; retain synthetic
presets and use a per-quantity provenance sidecar. Independent reviewers must
recompute one force–work result and evaluate reserved attacks without parameter
changes. Present `contact.nail_passivity`, `contact.release_convergence` and
`contact.gesture_replay` establish only their synthetic properties;
`contact.strike_speed_envelope` and practitioner
`performance.gesture_matrix`/`performance.repertoire_holdout` remain planned.
Route `ENG-P05-T01` and `ENG-P05-T03` (`REQ-EXC-001`, `REQ-EXC-002`,
`REQ-PRF-002`) to P06/P11, with identifiability/calibration issues handed to P12.

## Exit and handoff checklist

- Preserve one independently checked gap/force/work convention and the exact
  synthetic baseline, including its threshold/travel release limitation.
- Deliver separate geometry, normal-law, friction-hypothesis, spectrum and sweep
  decisions; unsupported coefficients remain unknown or explicitly synthetic.
- Keep raw/derived lineage, units, uncertainties, failed controls and reserved
  predictions with the executed test names and results; a planned test is not a pass.
- Hand P06 the continued string/contact state and admissible simultaneous-contact
  domain; hand P11 the event/control semantics and P12 the unresolved parameter
  directions. Numerical acceptance can proceed while the empirical phase gate
  remains open.
