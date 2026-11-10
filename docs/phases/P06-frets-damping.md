# P06 — Frets, stopping, damping, and changing speaking length

**Status:** specification. **Prerequisites:** P04 and P05. **Exit evidence:**
contact and stopping mechanisms preserve the declared mechanical balance and
predict a defined set of left-hand gestures without resetting the instrument.

## Establish the physical question

A fret is a contact constraint, not simply a command to shorten a delay line.
The string can remain in contact, lose contact, touch more than one support, or
transfer energy into a finger. Behind-fret segments can also retain motion.
Which effects matter depends on geometry, action, tension, displacement, and
the player's gesture. This phase determines which mechanisms are necessary for
the target specimen and how their limits are represented.

Read [contact mechanics](../contacts/README.md),
[performance](../performance/README.md), and
[numerical boundary treatment](../numerics/README.md). The reference problem
must identify whether the fret is modeled as rigid, compliant, lossy, or moving.
Do not introduce all four options before the simplest relevant case is verified.

## Entry checks and research map

The current [instrument](../../include/setar/instrument/instrument.hpp) retains
four nut-to-tail modal strings, two transverse polarizations and two finger-pad
slots per string. Synthetic point frets, finger pads, lateral finger damping and
a fret stick spring are implemented. Frets are stationary configuration objects;
finger positions can move. An active-slot `FingerPress` already preserves its pad
state and moves at the event speed, as specified in
[gesture.hpp](../../include/setar/instrument/gesture.hpp) and exercised by
`controls.sweep_continuity`. The historical teleportation defect is
[recorded as fixed](../realtime/05-prototype-realtime-evidence.md#defects-found-by-these-tests);
do not reopen it without a new failing reproducer. Physical tension retuning,
finite contact patches, measured fret compliance and Coulomb slip remain absent.

| Research route | Exact source and location | Decision it can inform |
| --- | --- | --- |
| Fret placement and musical context | `R-FRETTING-POORHAYDARI-2023`, [journal full text](https://journal.iftawm.org/previous/2023-volume-11-no-2/poorhaydari/), “Methodology and Initial Tests,” paragraphs [7]–[8]; `R-SETAR-TABASSIAN`, [handout](https://www.atlasensemble.nl/assets/files/instruments/Setar/Setar%20by%20Kiya%20Tabassian.pdf), pp. 1–2 | Separate observations and intended intervals from theoretical scale proposals; neither supplies this specimen's geometry or compliance. |
| Barrier and finger mechanics | `R-CONTACT-BILBAO-TORIN-2014`, [DAFx paper](https://dafx.de/paper-archive/2014/dafx14_stefan_bilbao_numerical_simulation_of_s.pdf), §§2.2–2.4, §3 and §4.5 | Independent barrier/finger and moving-contact methods, adapted explicitly from a single-polarization model. |
| Penalty/contact verification | `R-CONTACT-BILBAO-2014`, [versioned preprint](https://arxiv.org/abs/1405.2589v1), §2 and §6 | Conservative collision and distributed-obstacle references; no measured Iranian setar fret law. |
| Setup, uncertainty and acquisition | `R-METROLOGY-JCGM-100`, [GUM](https://www.bipm.org/documents/20126/2071204/JCGM_100_2008_E.pdf), §§4, 5, 7; [strings/frets/setup](../measurement/strings-frets-and-setup.md); [campaign](../measurement/13-acquisition-campaign-execution.md) | Future `frets.contact_geometry`, `strings.length_tension_density` and `gestures.performer` packets with calibration and covariance. |

Freeze setup, initial state, contact parameters, numerical bandwidth, observation
and event order before each experiment. Separate a same-mode-count sample-rate
study from a modal-resolution study. Use the actual
[modal-contact energy convention](../numerics/15-modal-contact-solver.md), not a
continuous energy formula substituted into its discrete ledger. Derive proposed
acceptance tolerances from contact/pitch/decay observables, acquisition uncertainty
and independent reference error. Each task retains accepted, rejected,
inconclusive or blocked findings and the reason. Synthetic verification can
continue while specimen acquisition or practitioner review is blocked; none of
these instructions supplies permission for contact, apparatus changes or recording.

## P06-T01: inventory stopping geometry

Build a specimen-referenced coordinate description of nut, bridge, tied frets,
finger contact region, and string rest position. Include string radius and
whether a reported height is measured to its center or surface. Record the
uncertainty and loading state of each measurement. A fret position measured on
an unstrung neck need not equal the effective contact point under playing load.

Specify which string segments are retained in the model. If the finger damps a
segment that is omitted, explain the reduced boundary impedance that substitutes
for it. An ideal fixed endpoint is a useful limiting case; it is not evidence
that the omitted segment has no audible effect. Keep a richer offline model to
test the simplification under the most demanding intended gestures.

For tied frets, distinguish the nominal fret center, contact crown, and variable
contact width. Include the mounting condition in the uncertainty record rather
than assigning a universal setar fret geometry. Avoid equating a visual fret
number with one immutable pitch across tunings, specimens, and practices.

### Execution recipe

**Question and route.** Which measured geometry controls contact onset, realized
speaking length and residual segments for the selected setup? Read
`R-FRETTING-POORHAYDARI-2023`, “Methodology and Initial Tests” [7]–[8], and
`R-SETAR-TABASSIAN` pp. 1–2 as context, then
[compliant frets](../contacts/04-compliant-frets-and-stopping.md), “Geometry of a
tied fret” and “Segments that do not disappear.” Search `"setar" tied fret
action measurement`, `"سه‌تار" پرده بندی کوک`, and `string fret crown effective
contact length`. Treat photographs and theoretical ratios as different evidence
from loaded geometry and played pitch.

1. Establish a nut-origin coordinate system and a common scale in metres. Record
   actual nut, bridge and tail contact locations, string radius, rest centerline,
   fret crown/profile, tie width, finger region and per-string action. Preserve
   whether each height refers to surface or center, loaded/unloaded state,
   instrument support, tuning, environment and calibration. Repeat landmarks
   independently; include perspective and common scale errors in covariance.
   Do not move frets or alter action to obtain a convenient grid.
2. Tabulate the nut-side stopped segment, speaking segment and bridge-to-tail
   afterlength separately. Map `StringConfig::total_length_m` to nut–tail and
   `bridge_position_m` to nut–bridge. Current `FretConfig::position_m` assumes
   one shared longitudinal coordinate per fret, and `fret_top_height_m` derives
   a linear effective clearance from `action_at_nut_m`/`action_at_bridge_m`.
   Explicit string radius, curved crowns and string-dependent fret intersections
   require a **proposed** geometry extension; do not subtract radius twice or
   label a fitted linear clearance as the measured surface.
3. Compare intended interval, measured coordinate and measured played frequency
   separately. Use `x=L*(1-2^(-c/1200))` only as the ideal flexible-string check.
   Its sensitivity `dc/dx=1200/(ln(2)*(L-x))` explains the position contribution
   to cents uncertainty; include length/tension/contact uncertainty as well.
   Preserve unequal fret spacing and practitioner context.

**Acceptance and handoff.** Retain scaled raw views, landmark/crown tables,
coordinate transforms, retained-segment diagram, uncertainty and the mapping
decision in the `frets.contact_geometry` packet. A reviewer must reconstruct
clearances from raw scale references and compare a second measurement route.
Swapped center/surface conventions, an angled-fret synthetic case and a
speaking/total-length swap are negative controls. Reject a calibrated-contact
claim if clearance uncertainty exceeds the onset difference of interest.
`preset.fret_positions` checks synthetic maps and `engine.fretted_pitch` a
synthetic stopped-string limit; neither measures geometry. Route
`ENG-P06-T01` / `REQ-FRT-001` to P06-T02/T05, with richer geometry requirements
and intended-interval records handed to P07/P11.

## P06-T02: verify a stationary unilateral support

Start with one fixed obstacle and a single string. Test an initial state that
never touches it, a small isolated collision, sustained contact, and repeated
collisions. With a lossless obstacle/contact formulation, the energy balance
must account for reversible contact storage. If loss is present, distinguish
contact dissipation from bulk string damping.

Compare contact timing and impulse under step refinement. Plot penetration or
constraint violation in physical units; a visually smooth displacement trace can
hide an unacceptable overlap. If numerical regularization is used, show how
its parameter affects timing and spectral content. Do not reduce the step until
the waveform looks plausible and omit the convergence study.

Add simultaneous contacts only after the single-contact tests pass. Include a
case where contact activates at neighboring frets and a case where the finger
prevents a rebound. The active-set or nonlinear iteration policy must be bounded
for a production implementation and inspectable in the reference implementation.

### Execution recipe

**Question and route.** Does one stationary fret produce convergent unilateral
contact without unexplained work, before finger motion is introduced? Read
`R-CONTACT-BILBAO-TORIN-2014` §§2.2, 2.4 and 3 and
`R-CONTACT-BILBAO-2014` §2 for transferable verification methods. Use
[contact potentials](../mathematics/05-contact-potentials-and-solvers.md) and
[modal-contact solver §§2–5](../numerics/15-modal-contact-solver.md).
Search `string stationary barrier collision impulse penetration convergence`;
published guitar/sitar examples supply no setar penalty coefficient.

1. Freeze one synthetic string, one lower obstacle and zero external motion.
   With vertical displacement positive upward, use the current effective gap
   `z=u_v(x_f)-h_f`, penetration `eta=max(0,-z)`, and force on the string upward.
   Set `V=K*eta^(alpha+1)/(alpha+1)` in joules; `K` has units N/m^alpha.
   Separate a compliant interpretation from a rigid-obstacle penalty, with a
   declared admissible compression or approximation-error budget respectively.
2. Independently solve the static small-system force balance, then perturb its
   sustained-contact equilibrium and compare linearized frequencies. Specify the
   applied static load in newtons and include its work; free-impact controls use
   zero load. Also run a contact-free trajectory, one collision and repeated
   collisions. Disable string
   and contact losses for the conservative case; add each known loss separately.
   Compare gap, impulse in N·s, event timing and potential storage under timestep
   and modal refinement, not merely output amplitude.
3. Inspect `Instrument::Impl::add_fret_channel`, `assemble` and the post-solve
   activation check in [instrument.cpp](../../src/instrument/instrument.cpp),
   with `ChannelLawKind::OneSidedPower` and `SmallNewtonSolver` in
   [numerics.hpp](../../include/setar/instrument/numerics.hpp). A previously
   omitted obstacle that penetrates must enter the repeated solve. For two
   neighboring contacts compare a separately coded small active-set or converged
   FDTD reference, including nearly redundant constraints and a pad preventing
   rebound. Record capacity and iteration bounds.

**Acceptance and handoff.** Retain the geometry/law manifest, independently
computed equilibrium and impulse, force/gap plots, energy residual and separate
temporal/modal convergence curves. At zero drive the ledger must close with
nonnegative stated dissipation; reject unresolved penetration, missed obstacles,
capacity overflow or Newton failure as qualified behavior. A deliberate wrong
normal and too-small iteration cap must fail visibly. Existing
`solver.discrete_gradient_identity`, `solver.newton_coupled_oracle`,
`solver.point_spring_eigen_oracle`, `engine.lossless_energy_ledger` and
`engine.fretted_pitch` check building blocks or synthetic limits; the point-spring
test is bilateral and is not a unilateral-impact oracle. Full stationary-fret
qualification is a proposed extension of `ENG-P06-T01` / `REQ-FRT-001` feeding
P06-T03/T04/T05. Set its tolerances from the independent error curves before
interpreting measured buzz or fret compliance.

## P06-T03: represent finger compliance and losses

A damping finger is not necessarily a fixed endpoint. Define whether the model
represents transverse impedance, normal contact, tangential loss, or a lumped
effective combination. Give each parameter a measurable role and unit. A
frequency-independent dashpot may be an initial approximation; assess its
validity against the measured decay and contact-duration range before adding
more elements.

Use prescribed finger motion as an external work input. If the finger model has
its own mass and compliance, include that stored energy. Verify that reducing
the damping coefficient approaches the corresponding lossless model, and that
removing the finger restores the uncontacted response without a hidden state
reset. A negative damping estimate is a diagnostic failure, not a permissible
tone-shaping adjustment.

Separate intentional left-hand muting from emergency numerical containment.
Musical damping should act through a documented physical or performance model.
A safety mute after nonfinite state must remain visible in diagnostics and must
not be counted as a successful damping gesture.

### Execution recipe

**Question and route.** Can a local passive finger impedance explain
location-dependent decay and release while preserving the underlying string?
Read `R-CONTACT-BILBAO-TORIN-2014` §§2.3–2.4 for a finger/energy method and
`R-METROLOGY-JCGM-100` §§4–5 for the uncertainty model, then
[finger damping and release](../contacts/05-finger-damping-and-release.md),
“A stationary distributed viscous benchmark” through “Elastic contact and
release.” Search `finger string contact impedance decay measurement` and
`local string damper node antinode attenuation`. Obtain permitted touch trials
through the setup and gesture packets, not by inferring pressure from loudness.

1. Fix string/body losses from independent evidence or a synthetic fixture.
   Apply a stationary lumped dashpot `F=-R*v_rel`, `R` in N·s/m, so passive
   power is `-R*v_rel^2`. For a distributed density `c(x)` the force density
   is `-c(x)*u_t` and `c` is N·s/m². State which representation is used and
   hold physical width fixed under refinement.
2. Initialize separate modes and place the damper at a node and antinode;
   independently compute the damped linear system response and compare decay.
   Then add normal elastic contact, time-varying engagement and prescribed motion
   one at a time. For the present pad `z=u_pad-u_v`; compression pushes downward.
   Include pad work and contact storage, then inspect release without a new pluck.
   Test `R=0`, removed contact, touch–release–touch and pre-existing motion.
3. Compare consented repeated touch locations, durations and loads with calibrated
   motion/force channels where available. Separate finger loss from support,
   bridge and string damping; retain force uncertainty and observation bandwidth.
   Reserve a location/duration or session to test a common coefficient before
   introducing frequency dependence or separate tissue elements.

**Translation and gate.** Current `ContactConfig::finger`,
`finger_lateral_damping_n_s_per_m`, `FingerPress` and `FingerRelease` act through
`Instrument::Impl::assemble`; `fret_stick_damping_n_s_per_m` belongs to another
contact mechanism. Finger mass, finite width and viscoelastic internal states are
**proposed**, with their stored energy and release policy required explicitly.
Retain raw/derived manifests, per-mode decay/force–displacement plots, work/loss
partitions, fitted intervals and the simplest-supported-model decision. An
independent linear-system solution must reproduce the synthetic node/antinode
contrast; reserved measurements must distinguish the candidate from unchanged
bulk loss within uncertainty. Reject negative damping, unbooked release storage
or a global state reset. `engine.lossy_energy_ledger` and
`contact.release_afterlength` are present synthetic checks; measured finger
impedance and spatial-loss qualification remain proposed under `ENG-P06-T01` /
`REQ-FRT-001`. Handoff the identified domain to P06-T04/T06 and P12, keeping
`GestureKind::Silence` containment separate from musical damping.

## P06-T04: move a contact without unexplained energy

Sliding or repositioning a finger changes the constraint and may do mechanical
work. Establish the difference between moving a physical contact and instantly
selecting a new numerical domain. If grid interpolation, remeshing, or modal
projection is required, test displacement, velocity, work, and interpolation
error across the transition. A crossfade can hide a click while still changing
the physics; label any such production approximation explicitly.

Exercise slow and rapid motion, motion during silence, and motion during strong
ringing. Compare different host block partitions with identical sample-stamped
events. A gesture should not change merely because a DAW uses a different buffer
size. Include endpoints, direction reversal, and an invalid position outside the
physical domain. Reject or safely constrain impossible geometry according to a
published input contract, not an undocumented clamp.

### Execution recipe

**Question and route.** Does a moving contact account for its external work
while preserving displacement/velocity, and what additional contract would make
tension retuning physical? Read `R-CONTACT-BILBAO-TORIN-2014` §2.4 and §4.5,
then [moving frets and retuning](../numerics/04-moving-frets-and-retuning.md),
equations R1–R4. Search `moving finger distribution string external power` and
`time varying string tension energy state transfer`. These sources motivate
derivation; they do not validate the current control trajectories.

1. For the proposed simultaneous moving-map derivation use compression
   `s=b-G*q`, with exact endpoint identity
   `Delta s=Delta b-G_bar*Delta q-Delta G*q_bar` and control work
   `W=F_bar*(Delta b-Delta G*q_bar)` in joules. Verify the identity on unrelated
   endpoint data before dynamics. Include a stationary deformed string with a
   moving observation point: material velocity is zero while geometric work
   need not be. Translate signs explicitly to the current gap `z=B*q-u`.
2. Audit the existing split update: `FingerMove` and active-slot `FingerPress`
   preserve pad state, `move_toward` limits travel by speed times step, and
   `book_channel_changes` books changed-map potential as `external_work_j`.
   It is already implemented; test its temporal error against a separately
   integrated continuous-motion reference. Exercise slow/fast motion, reversal,
   fret crossing, simultaneous pluck and ringing states. Repartition identical
   sample-stamped events and compare pad location, physical probes and work.
3. Keep tension change separate. The current `GestureKind` has no retuning event
   and `prepare()` is not a state-preserving actuator. For a **proposed** fixed-domain
   tension transition derive `V_T=T*q^T*A_s*q/2` and parameter work
   `W_T=mean(q^T*A_s*q)*Delta T/2`. Check increasing/decreasing tension with
   known slope energy, coefficient admissibility throughout the ramp, and any
   basis-transfer energy defect. Do not replace this with final-audio resampling.

**Acceptance and handoff.** Retain endpoint algebra checks, continuous-reference
traces, event/config hashes, displacement/velocity and work error curves. A
stationary control must add no geometric work; out-of-domain positions, moving an
inactive slot and unsupported retuning must follow explicit rejection policies.
Reject unexplained jumps even if output sounds smooth. Current
`controls.sweep_continuity` includes finite-speed re-press, and
`contact.fret_crossing_work` checks ledger accounting; `realtime.block_partition`
does not qualify every host-driven control case. `controls.retune_energy_ledger`
remains planned. Route `ENG-P06-T02` / `REQ-FRT-002` to P06-T06/P11 and P13
with separate accepted movement and blocked/proposed retuning dispositions.

## P06-T05: investigate buzz and residual segments

Contact chatter can be a physical phenomenon, numerical instability, or both.
Identify it through step refinement, work balance, and controlled variation of
action and amplitude. Do not declare every high-frequency transient authentic
fret noise. Compare contact force histories, spectral changes, and measured
onset conditions rather than only one pleasing recording.

Evaluate whether behind-fret and nut-side motion matters to the target domain.
Perform an ablation: retain the segment, replace it with an impedance, then
remove its dynamics under an explicitly ideal boundary. Compare controlled
responses and perception with the same main-string parameters. Select the
simplest representation that satisfies the declared error budget, keeping the
ablation artifacts so future extensions can revisit the decision.

### Execution recipe

**Question and route.** Is repeated high-frequency contact a predicted physical
effect within the setup uncertainty, and do omitted string segments change it?
Read `R-CONTACT-BILBAO-2014` §6 and
`R-CONTACT-BILBAO-TORIN-2014` §§2.2, 3 and 4 for barrier methods, then
[buzz validation](../contacts/06-buzz-and-contact-validation.md), “Chatter has
diagnostic signatures,” “Bandwidth and aliasing,” and “Experimental
discrimination.” Search `string fret buzz onset clearance amplitude convergence`
and `behind fret afterlength damping measurement`. Indian sitar jawari behavior
is not a setar fret-law justification.

1. Freeze string tension, loss, contact law, pluck position/direction, observation
   and initial work. Sweep synthetic clearance around first contact; separately
   sweep amplitude with geometry fixed. Predict the no-contact limit from the
   free string and compare the contact-onset boundary. Plot gaps in metres,
   impulse in N·s, contact duration, iteration/residual traces and energy/work.
   Refine timestep, retained modes and solver tolerance independently while
   keeping physical patch width fixed if a proposed finite patch is used.
2. Compare a high-rate reference after a documented common-band low-pass filter
   with runtime output. Retain unfiltered mechanics as well: output filtering
   cannot repair incorrect internal work. Compare band-limited force or impulse
   when sensor bandwidth cannot resolve peak force. A change with iteration cap,
   capacity or discrete resolution is a numerical warning, not evidence of buzz.
3. Probe both `0 < x < x_fret` and
   `bridge_position_m < x < total_length_m` using
   `Instrument::string_displacement_m`/`string_velocity_m_per_s`. The current
   modal domain retains both regions. A **proposed offline ablation** compares
   that model with a passive replacement impedance and an ideal eliminated
   segment, matching the main segment, pre-transition state and work as closely
   as their different domains permit. Quantify discarded/projection energy;
   arbitrary muting is not an equivalent initial condition.

**Evidence and decision.** Retain the clearance/amplitude contact map, modal/time
convergence plots, segment-motion traces, filtered spectra, raw setup observations
and ablation errors. Use contact-disabled and large-clearance controls; compare
against an independently converged obstacle solver before consented near-onset
instrument trials. Do not adjust a real instrument's action under this plan.
Reject unstable residuals, unexplained loss/growth, hidden contact dropping or
alias-sensitive onset; classify unresolved geometry versus physical uncertainty
as inconclusive. `contact.release_afterlength` demonstrates retained nut-side
motion/open-pitch recovery in its fixture, not measured tail-afterlength decay
or a buzz threshold. `engine.fretted_pitch` likewise is a limited synthetic check.
Route the qualified obstacle/segment decision through `ENG-P06-T01` /
`REQ-FRT-001` to P06-T06, P09 and the P13 reduction/aliasing study.

## P06-T06: qualify left-hand gestures

Build a gesture matrix with practitioner input: stopped notes, release to an
open string, damping, slides where relevant, repeated articulation, and contact
on a previously ringing string. Specify which actions are in scope rather than
pretending every named ornament is one standard MIDI sequence. Record the
physical control trajectory separately from its eventual keyboard mapping.

Acceptance needs both mechanical verification and instrument-specific evidence.
Show that failure cases are detectable, contact state survives block boundaries,
and changing the fingering does not silently erase unrelated strings. Hand off
event definitions, contact state ownership, and admissible geometry to P11.

> Implement one stopped-string experiment with an explicit fret/finger geometry.
> Document the gap convention, retained segments, moving-boundary work, and
> numerical constraints. Test no contact, sustained contact, release, and motion
> during ringing. Compare step refinement and event block partitions. Report
> unexplained energy or chatter as a defect, not as realism. Do not invent a
> specimen's fret positions or infer a universal tuning from equal temperament.

### Execution recipe

**Question and route.** Which left-hand actions can this persistent contact
system express and predict, and which remain controller approximations? Use
`R-SETAR-TABASSIAN` pp. 1–2 and
`R-FRETTING-POORHAYDARI-2023` “Methodology and Initial Tests” [7]–[8] to frame
questions, then [gesture verification](../performance/01-gesture-verification.md)
and [performance validation](../performance/05-performance-validation.md).
Search `"setar" left hand damping release ornament` and
`"سه‌تار" انگشت گذاری`. Preserve player terminology and disagreement; a
published technique name is not a calibrated trajectory or universal MIDI map.

1. Prepare a bounded matrix of stopped note, light touch, damping, release to
   open string, slide/reversal, repeated press, overlapping fingers and contact
   during ringing. Include another string already sounding and both members of
   a paired course. Record intended action separately from physical string/slot,
   position/height in metres, speed in m/s, onset/release intervals and setup.
   Practitioner sessions require existing permission and review; otherwise keep
   this an engineering matrix with the empirical gate blocked.
2. Replay physical `GestureEvent` sequences first, then compare their
   `parse_score`/`MidiGestureMapper` translations in
   [performance.cpp](../../src/instrument/performance.cpp). The existing
   `finger_position_for_fret` policy keeps a pad between neighboring frets;
   evaluate that approximation for the observed placement. Two pad slots per
   string are the present capacity, not evidence that every ornament fits.
   `FingerRelease` raises a pad; `Silence` clears state. Document the mapper's
   note-off damping policy separately from both.
3. Inspect continued state and work before/during/after each gesture. Repartition
   equal-timestamp events across blocks without changing absolute order; test
   repeated release, invalid position/slot, move on an inactive pad, and capacity
   exhaustion. Compare a pluck into an existing stop with engagement during
   motion. Contact removal must preserve unrelated string state and book any
   discarded interaction energy.

**Acceptance and handoff.** Retain the matrix, source/annotation locations,
permissions, raw trajectory/audio hashes, event traces, state/work plots,
uncertainty and per-gesture accepted/rejected/inconclusive/blocked decisions.
An independent reviewer reconstructs one transition and compares reserved
sessions or gestures against preregistered timing/pitch/decay criteria; mechanical
verification and practitioner endorsement remain separate. Present
`controls.sweep_continuity`, `contact.release_afterlength`,
`performance.persistent_state`, `engine.event_ordering`, `score.parse_valid`
and `score.midi_mapper` check synthetic implementation behavior.
`performance.gesture_matrix`, `performance.repertoire_holdout` and
`controls.retune_energy_ledger` remain planned, with tension retuning unsupported.
Route `ENG-P06-T02` and `ENG-P11-T02` (`REQ-FRT-002`, `REQ-PRF-002`) to P11
with state ownership, admissible geometry, unexpressible gestures and explicit
host mappings; send mechanical-contact failures back to `ENG-P06-T01` /
`REQ-FRT-001` rather than concealing them in the mapper.

## Exit and handoff checklist

- Deliver the specimen/setup geometry and its uncertainty independently from
  intended tuning, fitted effective boundaries and synthetic regression maps.
- Retain stationary-contact, damping, motion and buzz/reference comparisons,
  including failure controls and the supported range of penetration, speed and
  contact count. Keep the full nut-to-tail state or document reduction error.
- Separate prescribed-motion work, contact/string dissipation, switching energy
  and explicit resets. Preserve the implemented finite-speed re-press behavior;
  physical retuning remains its own open work package.
- Give P11 exact event meanings and contact ownership, P12 unresolved parameter
  tradeoffs, and P13 the retained reference fixtures/error budgets. No synthetic
  test result closes practitioner or measured-specimen qualification by itself.
