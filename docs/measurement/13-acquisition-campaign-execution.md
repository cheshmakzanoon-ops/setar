# Reference Iranian setar: specimen acquisition campaign and raw-evidence contract

**Current scientific status: no physical specimen was supplied or measured.**
This document is a work instruction, not synthetic measurement data.
Its [17-channel blank acquisition manifest](../../data/measurement-campaign-template.json)
is a distinct layer above the earlier
[per-instrument parameter template](../../data/specimen-template.json).
The first asks **what observations are needed and which raw files prove they
were taken**; the second expresses **candidate physical model inputs and
their uncertainty**. No field may be promoted from one to the other merely
because a plausible published number exists.

Read [specimen completeness](specimen-completeness-matrix.md),
[measurement protocols](README.md), [model admissibility](../body/01-orthotropic-material-audit.md)
and [inference and holdouts](../validation/05-identifiability-and-calibration.md) alongside this plan.
If a referenced volume is reorganized, update the linked evidence ledger at
the same time rather than leaving an orphan acquisition protocol.

This chapter connects acquisition decisions to the **current** prototype, not to
an assumed future importer. Use the [source atlas](../research/source-atlas.md)
to select and record an inspected method, the
[research-to-model workflow](../research/research-to-model-workflow.md#7-translate-findings-into-current-c-interfaces)
to classify its consequence, and the
[execution guide](../roadmap-execution-guide.md#phase-handoffs-what-the-next-developer-receives)
to assign the next owner. The operations below are proposed until a real session
and its review records exist. Preparing this chapter does not acquire any channel.

## Ownership, permission, reference identity

1. **Choose one particular Iranian setar.** Assign a stable, non-identifying
   specimen UUID before measurement. Separately record maker (if known),
   construction details, condition, string identities, current tension,
   bridge and fret setup, repair history, orientation and storage conditions.
   Photograph identifiable features only with permission. This specimen is
   the target; published average dimensions do not establish its geometry.
2. **Obtain informed owner and performer permission before any session.**
   Explain whether microphones, contact force sensors, optical scans,
   performer video, nail/finger measurements and any irreversible modifications
   are proposed. Non-invasive measurement is the default. The owner may
   prohibit access to the bowl interior, disassembly or identifying
   photographs; this creates a declared unknown, not an invitation to infer
   private information. Separate permission for public data release from
   permission to analyze privately.
3. **Freeze the initial setup.** Record location of strings, fret placements,
   tuning, bridge and contact points, humidity/temperature conditioning,
   playing support and recording geometry. Re-tuning or moving a bridge creates
   a new setup ID. If an impact measurement requires muting strings, label its
   boundary conditions as an altered setup, not the playing configuration.
4. **Define a physical coordinate frame.** Specify right-handed axes,
   origin on an identifiable stable body landmark, directions along the string
   scale, soundboard normal and lateral axis. Document transforms between
   photogrammetry, laser vibrometry, microphones and the mechanical bridge
   port. Preserve uncertainty in scale, orientation and registration.
5. **Protect raw evidence.** Create a permission-controlled folder or private
   dataset with immutable sensor originals, calibration files, metadata,
   processing scripts and local hashes. Do not commit identifiable performer
   video, proprietary instrument scans or restricted recordings to this
   public repository. The public template remains intentionally empty.

## The 17 independent acquisition packets

Each row below is a separately identifiable acquisition. One dataset may
support several packets, but a data reuse declaration must explain the shared
instrument and calibration errors, as well as any independence assumptions.

| Manifest channel | Specific capture and comparator | Invalid shortcut to avoid |
| --- | --- | --- |
| `geometry.external_scale` | Calibrated string nut–bridge lengths, four course locations, landmarks, neck section and body outer surface; repeat scale reference at several positions and publish registration error | A catalog photo with unknown lens/scale cannot determine millimeter geometry |
| `geometry.bowl_internal` | Permission-approved interior scan or non-destructive geometry inference, wall thickness sampling, ribs/joints, cavity opening and uncertain occluded regions | Treating exterior scan as an exact interior solid mesh |
| `geometry.wooden_soundboard` | Soundboard outline, local thickness, wood grain axes, edge/joint constraints, sound holes and bridge contact footprint | Substituting the tar's skin sound table; the Iranian setar has a wooden soundboard |
| `materials.orthotropic_compliance` | Separate density/moisture evidence, independent orientation-dependent stiffness measurements or bounded admissible priors, reciprocal compliance and SPD checks | Copying contradictory \(E,\nu,G\) values into a matrix that has negative energy modes |
| `strings.length_tension_density` | All four wire material identities, active/rest lengths, linear mass, tuning, tension estimation checked by another method, end support compliance | Assuming two visually similar strings are equal or treating tuning alone as tension |
| `strings.stiffness_losses` | Inharmonic partial frequencies and decay from repeated open-string and stopped-string captures, frequency/time resolution, dependence on humidity/tension and fit uncertainty | Fitting one scalar damping to both bending dispersion and frequency loss |
| `frets.contact_geometry` | Fret placement, tie geometry, material, height, compliant compression, afterlengths, approach and release positions; record changes after adjustment | Representing every fret as a fixed pitch number with instantaneous state reset |
| `bridge.multi_axis_mobility` | Calibrated complex bridge velocity/force FRFs in identified axes, sufficiently resolved phase, impulse/force and output sensor correction, coherence and repeats; include played support | Using a magnitude-only EQ or a clamped-body FRF as a complex playing-bridge impedance |
| `bridge.string_support` | Force direction, string break angle, static preload, contact patch and admissible bridge rocking/sliding dynamics under the current setup | Assuming a point-clamped string end when the bridge moves and shares energy |
| `excitation.nail_flesh` | Consented high-speed motion and synchronized contact/force observations over controlled nail angles, velocities, nail versus flesh engagement, detachment and restrokes | Calling a triangular initial displacement a measured nail force law |
| `gestures.performer` | Practitioner-approved gesture categories and annotated timing, damping, vibrato, ornaments, cross-string patterns and touch context; performer ID separated from public data | Mapping MIDI velocity directly onto a single generic pluck sample envelope |
| `cavity.acoustic_modes` | Cavity geometry and pressure transfer under a stated boundary condition, repeatable internal/external observation locations and environmental data | Using a generic Helmholtz peak to claim the full cavity response |
| `radiation.directivity` | Calibrated output pressure or velocity-to-pressure transfer versus known azimuth, elevation and distance in controlled acoustic conditions | Calibrating one on-axis microphone and claiming surrounding sound field fidelity |
| `observation.calibrated_mics` | Microphone response, spatial transform, distance, gain, room/array characterization, phase/time alignment, noise floor and source independence | Hiding missing radiation physics under room EQ or mastering gain |
| `environment.conditions` | Temperature/humidity records, conditioning before/after, soundboard and strings' state, stand/person support and storage interval | Treating one recorded condition as all-world instrument behavior |
| `interactions.cross_string` | Excite one string while retaining unstruck-string motion; sensor trace on each course and body bridge under multiple damping conditions | Layering independent sympathetic samples without a shared physical port |
| `inference.heldout_split` | Pre-register immutable train/validation/test partitions across gestures, excitation angles, mic locations and—where appropriate—sessions/specimens | Selecting pleasing examples after fitting, then presenting them as blind holdouts |

### Exactly what belongs in a raw acquisition packet

For **each** packet create a separate directory in an authorized dataset and
store the following. A table of pretty results is not a substitute for the
underlying sensor outputs.

1. **Acquisition protocol**: instrument/setup/session IDs; scientific question;
   device/model/firmware; sensors and axes; sampling rate and timestamp clock;
   stimulus or gesture; allowed safety/contact limit; environmental conditions;
   planned repetitions; explicit exclusion criteria.
2. **Calibration**: calibration operator, reference standard and expiry;
   device channel transfer function and phase/group delay, geometry scale,
   force/velocity gain, microphone response; before/after or drift checks;
   traceable evidence reference. A unitless exported WAV does not guarantee
   calibrated force in newtons.
3. **Raw file table**: byte-exact original file path, SHA-256, channel map,
   clock domain, sensor units, sample count, known dropouts/saturation and
   recording permissions. Keep unedited originals and derived copies distinct.
4. **Controlled repetitions**: repeat within setup and across relevant support,
   position/angle or instrument condition. Save runs that failed coherence or
   geometry registration with reasons; do not delete outliers post hoc.
5. **Processing provenance**: source hashes, scripts/versions, filter windows,
   deconvolution, clock alignment, averaging, parameter estimation,
   residuals and numerical settings. Cross-correlations and derived transfer
   functions must be reproducible from permissible retained raw inputs.
6. **Uncertainty**: standard or expanded uncertainty and the coverage factor,
   repeatability, instrument calibration, alignment, environmental drift,
   systematic bias and covariance across channels. Correlated orthotropic
   constants and multiple FRFs must not be treated as independent scalars.
7. **Independent checks**: geometric closure against known landmarks,
   sensor reciprocity where physically applicable, mass/density consistency,
   passive energy/stiffness admissibility, identical input reconstruction,
   noise floors and a *held-out* repeat not seen by fitting code.
8. **Review disposition**: calibrated/raw-data reviewer, acoustic/structural
   specialist, permission and rights reviewer, exact accepted/rejected
   measurements and unresolved questions. An SHA-256 proves byte identity;
   it cannot establish a sensor's accuracy or subject consent.

Use [NIST Technical Note 1297](https://www.nist.gov/pml/nist-technical-note-1297)
and NIST's [measurement uncertainty resources](https://www.nist.gov/itl/sed/topic-areas/measurement-uncertainty)
for an appropriate uncertainty model. Follow the actual sensor manufacturer's
calibration and measurement guides rather than guessing accuracy from a
resolution number. For complex frequency response, preserve the imaginary
component and coherence; magnitude-only screenshots cannot reconstruct a
passive multiport interface.

## Calibrated pilot: earn the next acquisition block

Before a full campaign, choose one decision and the smallest safe capture that
can resolve it. Specify the intended frequency/amplitude range, the effect to
detect, uncertainty budget, permitted interventions and a stop rule. These are
experiment-specific requirements, not universal sample-rate, coherence or force
thresholds. A pilot can end with a narrower valid band or an unidentifiable
parameter family; neither outcome authorizes filling missing manifest fields.

Use `R-SETAR-MANSOUR-SCAVONE-2012` in the
[apparatus/source card](../research/source-atlas.md#r-setar-mansour-scavone-2012--measurement-loading-and-supports)
to plan loading and support comparisons. Use `R-SETAR-MANSOUR-2011` for a
clearly identified restrained-body comparison, not as a substitute for the played
support. Use `R-METROLOGY-JCGM-100`, with the edition and sections actually
inspected, for the uncertainty calculation. The atlas records access limits:
the 2015 Mansour article remains an acquisition lead, not an inspected source
of additional specimen values. None of these papers supplies this campaign's
sensor calibration or setar coefficients.

| Pilot operation | Retained evidence and decision before expansion |
| --- | --- |
| Establish the unchanged reference | Record permission scope, setup photographs/landmarks, tuning, support, environmental state and noise-only channels. Predefine a reference trial that can be repeated between blocks. If bridge/fret position, support or tuning changes, assign a new setup identity instead of combining the trials as unchanged repeats. |
| Check the complete sensor chain | Record raw counts/volts, range, anti-alias filtering, gains, calibration transfer and polarity. Apply a permitted reference stimulus or calibration standard; repeat the check after the block. Choose range using the pilot's peak and noise floor. Saturation, dropout, unknown gain or drift beyond the declared uncertainty invalidates the affected band/trial; gain changes require a new calibration association. |
| Check spatial reconstruction | Measure a traceable scale in several views and reserve landmarks from registration. Remove/reposition the instrument only if authorized, then repeat registration. Compare held-out distances and contact coordinates, not just the fit residual of the landmarks used to solve the transform. Persistent mismatch stops scale-dependent parameter promotion. |
| Check loading and force/response quality | Under the same preload, compare the intended sensing/support arrangement with a less loaded or independently observed reference. Use a small permitted amplitude series and repeat the initial reference after it. Save force spectra, double impacts, response noise, complex FRFs and all rejected takes. An amplitude-dependent FRF or changed pole after attachment requires a loading/nonlinearity investigation before body or damping fitting. |
| Check synchronized contact capture | Record a shared timing event at the beginning and end of a block in every clock domain. Fit offset and clock-rate correction, inspect residuals, and retain original frame timestamps. Compare minimally instrumented and instrumented gesture trials with the performer. Occluded release, unresolved timing or changed technique narrows the contact claim rather than being repaired by aligning each take to the model. |
| Check acoustic comparability | Check microphone sensitivity/polarity before and after, record positions and reference force or motion, and characterize the available reflection-free interval and background. Repeat one position after moving the microphone. If the usable window cannot resolve the low-frequency question, change the facility/design or declare that band unavailable. |

An independent reviewer examines a pilot's raw-to-derived reconstruction before
authorizing its scientific expansion. The next block is selected to reduce the
largest consequential ambiguity: another force/response direction, improved
clock evidence, a safer loading control, or an independent length/density
measurement. Increasing repetitions only reduces appropriate repeatability
uncertainty; it does not remove shared scale, calibration or support bias.
See the specific
[support pilot](frf-and-modal-protocol.md#fixture-and-support-pilot),
[timing protocol](pluck-contact-and-motion.md#synchronization), and
[acceptance decisions](uncertainty-and-calibration.md#acceptance-decisions).

### Units, coordinates, signs and covariance at the handoff

The specimen frame and the engine frame must be connected explicitly. In
[config.hpp](../../include/setar/instrument/config.hpp), each string's `x`
runs from nut to tail anchor; `bridge_position_m` is the nut-to-bridge
speaking length and `total_length_m` includes the afterlength. Vertical
displacement is positive away from the soundboard; horizontal displacement is
positive toward physical string index 3. Store the right-handed acquisition
frame, named axis projections/permutations, landmark registration and uncertainty;
do not assume the order of camera coordinates matches the engine's `(v,h)`.
For orthogonal component transform \(R\), transform both force and velocity,
\(F_e=RF_s,\ v_e=Rv_s\), so the power product \(F_e^\mathsf{T}v_e\) is unchanged.
Positions additionally need the origin translation. Grain directions require
their own local material frames.

| Quantity crossing the boundary | Required convention |
| --- | --- |
| Geometry, motion and time | m, m/s, m/s² and s; preserve original sensor units and conversion. Angles are rad. Record whether a velocity is a material-point velocity or the derivative of a moving tracked contact coordinate. |
| Mechanical port | Force N entering the named subsystem and velocity m/s at the same point/direction; their product is W. Bridge force on the body is opposite the force on the string. Rotational ports, if proposed, additionally require torque N·m and angular velocity rad/s; they are not present bridge configuration fields. |
| FRF | State Fourier sign and spectral estimator. With \(e^{i\omega t}\), \(v=i\omega x\), so accelerance-to-mobility conversion divides by \(i\omega\); it is unusable at DC and ill-conditioned near the low-frequency noise floor. Preserve real/imaginary components, not only magnitude. |
| Elasticity and string dispersion | Density kg/m³, linear density kg/m, modulus Pa, compliance Pa⁻¹, string rigidity \(EI\) N·m² and plate rigidity N·m. Declare engineering versus tensor shear strain before comparing compliance matrices. |
| Decay and acoustic output | Amplitude decay rate s⁻¹ differs from dimensionless modal damping ratio. Pressure is Pa at the recorded observation position; a normalized digital audio sample has no absolute pressure meaning without the chain calibration. |

Use the [quantity and channel contracts](data-contracts-and-provenance.md#quantity-records).
Each derived vector records component order, units, standard versus expanded
uncertainty, coverage convention and covariance. For \(y=g(x)\), first-order
propagation is \(\Sigma_y=J\Sigma_xJ^\mathsf{T}\); use a justified nonlinear
propagation method where that approximation fails. Include uncertainty of
registration, calibration and clocks, rather than propagating only fitted
coefficients. A common length scale correlates four string lengths; a shared
force calibration correlates an entire FRF column. For complex data, specify
covariance of stacked real/imaginary components, including cross-port and
frequency correlations or the justified approximation used. Timing uncertainty
\(u_t\) contributes phase uncertainty of order \(2\pi f u_t\): choose timing
resolution from the required band and phase budget, not nominal frame rate.

### Raw-to-derived records without inventing an importer

The following are **logical records in an authorized dataset**, using the existing
[data contract](data-contracts-and-provenance.md#entity-graph). They are not new
fields understood by `validate_measurement_campaign.py` or a new public schema.

| Record | Minimum lineage and independent review |
| --- | --- |
| Raw acquisition | Specimen/setup/session/trial, original file hash, channel map, clock, units before calibration, sensor range, permissions and exclusion flags. Preserve a failed trial and its reason subject to retention permissions. |
| Calibrated view | Input hashes, calibration versions, conversion equations, axis/clock transforms, filtering and valid band; uncertainty and source revision. A second analyst should reproduce a representative conversion from the original data. Recalibration creates a new view and invalidates dependent fits. |
| Derived observable | Examples: contact-edge position with covariance, partial frequency/decay trajectory, complex mobility matrix or pressure/force transfer. Keep estimator/window choices, selected intervals, residuals, omitted bands, mode assignments and alternative interpretations. |
| Parameter candidate | Exact destination symbol, units, joint uncertainty, normalization, model assumptions and identifiability result. Label measured inputs, derived estimates, fitted effective values and priors separately. A candidate remains outside the default preset until the promotion decision. |
| Decision and prediction | Reviewer identity, accepted/rejected/inconclusive/blocked finding, supported domain, frozen dataset partition, held-out prediction, requirement/work order and next owner. Retain rejected candidates and why they failed; do not overwrite them with the optimizer's next answer. |

One raw acquisition can support multiple manifest channels, but all derivatives
retain the same source and calibration identifiers. Its copies are not independent
evidence. Use the [finding](../../templates/research-finding.md) and
[promotion](../../templates/parameter-promotion.md) templates for review records.
The inference packet's raw evidence can include the original split declaration
and trial inventory; a nonphysical record does not need a fabricated sensor
calibration. Its metadata must explain which actual acquisition/calibration
records it governs. The manifest's generic reference requirements do not replace
that semantic review.

## From the 17 channels to current physics

The current target types are in
[config.hpp](../../include/setar/instrument/config.hpp),
[body_model.hpp](../../include/setar/instrument/body_model.hpp) and
[gesture.hpp](../../include/setar/instrument/gesture.hpp).
The mappings below name representable candidates, not automatic imports.
An observation that exceeds these types' assumptions becomes a proposed extension
with an independent verification task, or a restricted claim.

### 1. `geometry.external_scale`

Following [registration](geometry-and-thickness.md#multi-view-registration),
retain calibrated landmark observations and raw images/scan points, then derive
each nut, bridge contact and tail anchor with joint registration uncertainty.
Project the actual path into the chosen string coordinate; compare with an
independent contact-to-contact distance. Candidate destinations are
`StringConfig.total_length_m` and `bridge_position_m`. Course spacing also
constrains `BodyDesign.bridge_point_m` after an explicit mapping to its rectangular
plate frame. A photographed bridge center is not automatically the string's
contact edge. Geometry alone identifies neither tension nor a compliant dynamic
termination. Curved paths, distributed contacts or a nonrectangular body require
a documented approximation or a proposed geometry/reference model. Route the
reviewed geometry to P07 before P04/P06/P08 use it.

### 2. `geometry.bowl_internal`

Use [safe geometry acquisition](geometry-and-thickness.md#safe-acquisition-plan)
and [thickness mapping](geometry-and-thickness.md#thickness-mapping), leaving
inaccessible regions explicitly unknown. Retain section observations, uncertainty
in inner surfaces, cavity openings and any closure assumptions. Derive volume
with sensitivity to occlusion and wall thickness; compare with an independently
authorized geometric estimate when available. `HelmholtzCavity.volume_m3` can
receive a reviewed volume for the lumped approximation. It cannot represent
bowl-wall vibration, rib/joint stiffness or higher cavity shapes. Those require
proposed P07/P08 structural geometry and P10 cavity models. An accurate exterior
mesh does not identify interior thickness or material, and a Helmholtz resonance
alone cannot uniquely recover volume and effective opening length.

### 3. `geometry.wooden_soundboard`

Retain outline, thickness locations, grain directions, soundhole boundaries and
bridge footprint in the specimen frame. Repeat high-sensitivity thickness
measurements with a calibrated method and report inaccessible locations.
The current `OrthotropicPlate` accepts one length, width and uniform thickness;
`BodyDesign.bridge_point_m` accepts four plate contact coordinates. These are a
simply supported rectangular approximation, not a mesh import. Mapping a real
outline to an equivalent rectangle is a separately evaluated reduction whose
residual belongs in model discrepancy. Thickness observations cannot establish
edge restraint or elastic constants by themselves. Hand the full fields and
uncertainty to P07/P08's proposed geometry/FEM route, retaining them even if the
first reduced candidate uses only a few effective dimensions.

### 4. `materials.orthotropic_compliance`

Use [material identity](materials-joints-and-environment.md#material-identity-levels),
[grain frames](materials-joints-and-environment.md#grain-frame-acquisition) and
the [orthotropic audit](../body/01-orthotropic-material-audit.md).
Record density, moisture/state, specimen or witness-sample association, load and
strain calibration, and independent directional tests. Current plate candidates
are `density_kg_per_m3`, `youngs_modulus_long_pa`,
`youngs_modulus_cross_pa`, `shear_modulus_pa` and
`poisson_long_cross` in `OrthotropicPlate`. Its
`plane_stress_compliance()` is a reciprocal in-plane reduction, not a general
three-dimensional wood tensor importer. Review the full proposed tensor and
its uncertainty for reciprocity and positive energy before deriving a reduction.
Density and modal frequencies alone do not identify all stiffnesses, joints and
support compliance. If admissibility or identifiability fails, quarantine the
candidate or retain correlated bounds; never silently repair a printed value.
P07 reviews constitutive assumptions before P08 fitting.

### 5. `strings.length_tension_density`

Keep each wire identity separate from the course grouping. Use the
[linear-density](strings-frets-and-setup.md#diameter-and-linear-density) and
[tension](strings-frets-and-setup.md#tension-inference-and-its-limits) protocols:
retain stock/witness mass and length, installation state, contact-edge distances
and low-amplitude frequency traces. Derive \(\mu=m/\ell\) with shared calibration
covariance. Map reviewed candidates to `StringConfig.linear_density_kg_per_m`,
`tension_n` and the two length fields. The plain-round-wire helpers in
[setar_preset.cpp](../../src/instrument/setar_preset.cpp) assume a homogeneous
solid cross-section; appearance does not establish that construction.
`tension_for_frequency()` is an inverse stiff-string calculation under ideal
ends, not a tension measurement. Compare it with independent evidence or retain
an inferred range after testing boundary sensitivity. A tuning frequency alone
cannot determine tension, mass density, stiffness and effective length. Send
that ambiguity to P12 before promoting the P04 string candidate.

### 6. `strings.stiffness_losses`

Retain repeated partial-frequency and amplitude-decay traces with fit windows,
noise floors, polarization, excitation amplitude, support/contact state and
calibrated clocks. Compare independent time-domain decay estimates and frequency
estimates; check beating, nearby body poles and sensor placement before attributing
all decay to the wire. The [string setup matrix](strings-frets-and-setup.md#setup-matrix)
and [identifiability chapter](../validation/05-identifiability-and-calibration.md)
govern condition selection.

The current `StringLossLaw` and
[engine preparation](../../src/instrument/instrument.cpp) use
\[
\sigma_n=a_0+a_2k_n^2+b_2\omega_n^2,\qquad
\omega_n^2=(T/\mu)k_n^2+(EI/\mu)k_n^4 .
\]
Here `decay_constant_per_s` is \(a_0\) in s⁻¹,
`decay_wavenumber_squared_m2_per_s` is \(a_2\) in m²/s and
`decay_frequency_squared_s` is \(b_2\) in s.
`bending_stiffness_n_m2` receives \(EI\) in N·m².
An amplitude-envelope slope \(s_A\) in dB/s gives
\(\sigma=-s_A\ln(10)/20\); exponential energy decays at \(2\sigma\).
Do not substitute a dimensionless body damping ratio or an FRF peak frequency
without its observation model.

**Length variation alone does not separate \(a_2\) and \(b_2\) in the flexible
limit.** When \(EI=0\) and \(c^2=T/\mu\) is unchanged,
\(\sigma=a_0+(a_2+c^2b_2)k^2\). Changing mode number or speaking length changes
\(k\), but the two sensitivity columns remain proportional. Identify the
combined coefficient and retain the family of admissible decompositions.
Resolved bending dispersion can add a \(k^4\) dependence; independently varied
tension can add different \(c^2\) values only if sharing loss coefficients across
those states is physically justified and tested. Neither strategy guarantees
practical identifiability when the effect lies below uncertainty or boundary
loss changes. Use sensitivity rank and parameter profiles, and seek an independent
loss observation if necessary. A bound-constrained fit is not proof of uniqueness.

The engine's basis uses `total_length_m`; measured speaking-segment partials
include bridge/body/contact coupling. Fit the applicable boundary model rather
than assigning those observations directly to uncoupled nut-to-tail modes.
Reject systematic amplitude-dependent residuals as evidence against the constant
linear-loss candidate, not as permission to hide them in observation gain.
P04 supplies the physical model; P12 decides which coefficient combinations the
data actually support.

### 7. `frets.contact_geometry`

Follow [fret geometry versus musical target](strings-frets-and-setup.md#fret-geometry-versus-musical-target).
Retain profiles, contact edges, string clearance, tie state and repeated finger
placements; preserve setup identity when a fret moves. Current
`FretConfig.position_m` stores one shared longitudinal location per fret.
`StringConfig.action_at_nut_m` and `action_at_bridge_m` define a linear
clearance line, and `fret_top_height_m()` gives its negative vertical height.
Check measured intermediate clearances against this line before adopting it.
Individual fret heights, finite profiles, different contact locations under
different strings and compression need a proposed extension if consequential.
Geometry does not identify `contacts.fret` stiffness/damping or the horizontal
stick spring. Controlled loading/release data are needed for those laws.
The current string model also omits pressing-induced tension rise. P06 must
retain that limitation when comparing played pitch with geometric predictions.

### 8. `bridge.multi_axis_mobility`

Use the [port contract](frf-and-modal-protocol.md#define-the-port-contract),
[multiport procedure](frf-and-modal-protocol.md#multiport-measurements) and
[modal identification](frf-and-modal-protocol.md#modal-identification).
Retain synchronized force and response, signs, line-of-sight corrections,
complex matrices, coherence, force spectra, support/preload and loading checks.
Compare swapped ports within uncertainty in the linear reciprocal domain.
Passivity concerns the Hermitian part of the complete collocated power-port
matrix; a negative real cross-transfer entry alone does not establish activity.

Current `BodyMode` candidates contain `frequency_hz` (undamped),
`damping_ratio`, `modal_mass_kg` and signed, dimensionless
`bridge_participation[string][polarization]`. The
`bridge_mobility()` implementation in
[body_model.cpp](../../src/instrument/body_model.cpp) exposes their modal sum.
FRFs constrain poles and second-order modal numerators
\(\psi_{ir}\psi_{jr}/M_r\), not a unique independent choice of modal mass and
shape normalization. Preserve that normalization and its covariance across
every mechanical and acoustic output. A frequency list or one magnitude FRF
cannot supply cross-port signs, modal masses and radiativity. The current
real-mode, diagonal-damping representation also lacks general residual terms
or an arbitrary complex state-space importer. If required, propose that extension
under P08 after an independently checked fit; P09 receives a passive port model
with its valid band, not an unexplained EQ curve.

### 9. `bridge.string_support`

Record break angles, contact edges, static preload, contact patch and motion
relative to both string and body under permitted loading. Use
[geometric mechanical checks](geometry-and-thickness.md#geometric-checks-tied-to-mechanics)
and the bridge port protocol. Separate string/bridge compliance from body
compliance using simultaneous motion or an independently constrained body model;
one total static deflection cannot identify every spring in series.
`BridgeConfig.stiffness_n_per_m` and `damping_n_s_per_m` supply one value per
transverse polarization, shared across all four string connections.
They are linear spring/damper candidates, not independent per-string preload,
break-angle, friction, rocking or torsional fields. Body modal participation
describes body-port motion, while these springs describe relative connection
motion; do not fit the same compliance twice. Observed slip or load-dependent
response requires a proposed connection law and work balance. P09 owns that
decision with P08's body model and P07's geometry evidence.

### 10. `excitation.nail_flesh`

Use [instrumentation](pluck-contact-and-motion.md#instrumentation-choices),
[contact annotation](pluck-contact-and-motion.md#contact-annotation) and the
work convention below. Retain synchronized force, nail and string motion,
contact-frame uncertainty, onset/release intervals and apparatus-disturbance
comparison. Fit compression/recovery and rate dependence over a declared domain;
reserve another permitted angle or speed for prediction. Current
`ContactConfig.nail` and `finger` use `ContactLaw`:
stiffness \(K\) in \(\mathrm{N}/\mathrm{m}^{\alpha}\), exponent \(\alpha\),
and damping in N·s/m. The current
[validator](../../src/instrument/config.cpp) admits finite positive stiffness,
finite `1 <= alpha <= 4`, and finite nonnegative damping for nail, finger and
fret laws. A supported fit outside those bounds remains a research candidate that
the current API cannot admit. Retain it for an explicit implementation-extension
decision rather than silently clipping its exponent. Mathematical admissibility,
current API admission and empirical support are separate checks.
Changing the exponent changes the stiffness unit. The current law is a
one-sided elastic potential plus gated viscous loss, not measured friction,
adhesion or a dynamically deforming nail.

`GestureKind::NailStroke` selects the `GestureEvent` action with prescribed
position, angle, constant speed, gap, force threshold and travel limit.
`position_m` uses the selected string's nut origin; current event admission in
`Instrument::Impl::valid_x` requires
`0 < position_m < strings[s].bridge_position_m`. An afterlength stroke is outside
this event contract even though the modal basis spans nut to tail. Its direction is
\((d_v,d_h)=(\sin\theta,\cos\theta)\); zero angle pushes toward positive
horizontal displacement. A measured force history is neither the
`force_n` release threshold nor an available force-file input. General trajectory
replay, distributed contact and friction require proposed interfaces.
Without calibrated force or an independently validated inverse model, retain a
family of plausible laws and supported release timing; do not promote unique
\(K,\alpha,R\) from audio alone. Hand contact identification to P05 and persistent
finger/fret implications to P06.

### 11. `gestures.performer`

Apply [mechanism versus performance capture](pluck-contact-and-motion.md#separate-mechanism-trials-from-natural-performance)
and [control mapping](pluck-contact-and-motion.md#mapping-to-real-time-controls).
Retain consent, operational descriptions, practitioner terminology, annotated
time intervals, initial ringing state and independent annotation disagreement.
Convert supported positions, directions and timing to physical events with a
declared sample-offset quantization error and stable equal-time sequence order.
[performance.hpp](../../include/setar/instrument/performance.hpp) and
[performance.cpp](../../src/instrument/performance.cpp) currently map scores/MIDI
to the events in `gesture.hpp` on persistent strings. They do not import
motion-capture files or infer intent from video. Two finger slots per string and
the prescribed motion laws limit the representable actions.

Gesture labels cannot identify a contact force law or establish that a MIDI
velocity mapping matches performer variation. Fit a proposed conditional mapping
only after contact and observation confounders are resolved, preserving
correlations rather than independently randomizing controls. P11 reviews
terminology, course grouping and event meaning; P05 checks the physical replay.

### 12. `cavity.acoustic_modes`

Combine internal geometry with calibrated pressure and structural-motion
references at documented ports. Record whether the instrument is muted,
restrained or played, and inspect multiple locations so a pressure node is not
mistaken for an absent mode. The
[acoustic output protocol](radiation-and-acoustic-capture.md#define-the-acoustic-output)
requires the excitation reference as well as pressure.
Current `HelmholtzCavity` represents one lumped opening/air-volume coordinate,
using volume m³, opening area m², effective length m, air density kg/m³ and sound
speed m/s. `build_body()` couples that approximation to the synthetic plate.
Pressure peaks alone cannot separate effective opening length, structural
compliance, cavity volume and damping. Geometry must constrain the inverse
problem, and independent pressure/motion responses must test it.
Higher cavity modes, spatial boundary motion and a compliant bowl require
proposed P10/P08 models. The equivalent opening in the current builder is not a
hole removed from plate stiffness; preserve that approximation in the comparison.

### 13. `radiation.directivity`

Use [controlled radiation acquisition](radiation-and-acoustic-capture.md#controlled-radiation-acquisition),
[angular/distance coverage](radiation-and-acoustic-capture.md#angular-and-distance-coverage)
and [phase/propagation](radiation-and-acoustic-capture.md#phase-and-propagation).
Retain calibrated pressure with force or surface-motion references, complete
observation geometry and usable reflection-limited band. Reserve positions from
fitting and compare complex transfer, not only normalized magnitude lobes.
Current `BodyMode.radiation_weight` contains `observation_count = 2` real
coefficients in Pa·s²/m, indexed `0` and `1`, multiplying modal acceleration
at the two observation channels. A pressure/force FRF cannot be copied into
that field: mechanical modal response,
normalization and the observation fit must first be separated.

These weights cannot express a general frequency-dependent complex directional
transfer, propagation delay or arbitrary microphone layout. `BodyObservation`
only defines the synthetic builder's simplified observation. Additional radiation
operators and acoustic feedback are proposed P10 work. Output weights never
feed force into the mechanics; fitting pressure does not establish radiation
loading or acoustic power. Do not count losses once in fitted mechanical damping
and again in a proposed loading model without reconciling the energy budget.

### 14. `observation.calibrated_mics`

Retain raw microphone channels, calibration tone/reference, response and delay,
gain settings, clock, spatial transform and room characterization under the
[acoustic calibration protocol](radiation-and-acoustic-capture.md#acoustic-calibration).
Derive Pa traces or calibrated transfers only over supported bandwidth; document
regularization and uncertainty of any response correction. Compare an independent
reference or repeat position to detect chain drift. Microphone calibration does
not identify the instrument's structural response, and a room-contaminated
spectrum cannot uniquely determine free-field radiation.

Current `ProcessSpec.output_gain_per_pa` is a fixed output scale, not a microphone
or room model. The [player](../../apps/setar_play.cpp) renders the synthetic
body observation through that gain; it does not ingest microphone calibrations.
Keep measured-chain correction and proposed virtual microphone/room transfer
outside mechanical state, with a declared comparison convention applied to both
signals. P10 owns calibrated observation; P12 owns comparable residuals.

### 15. `environment.conditions`

Follow [environment as state](materials-joints-and-environment.md#environment-is-part-of-the-state)
and [drift separation](materials-joints-and-environment.md#separate-environmental-sensitivity-from-drift).
Retain calibrated temperature, humidity, timestamps, conditioning, tuning and
support records before/during/after blocks. Convert temperature to K for derived
records while retaining original units; record relative humidity as a dimensionless
fraction or explicitly labelled percent. Revisit a reference condition and
counterbalance permitted changes so elapsed time is not confused with environment.
Humidity or temperature correlation alone does not identify a material law.

There is no general environment state or automatic humidity correction in
`InstrumentConfig`. The builder's air density/sound speed and material/geometry
fields can represent independently justified static configurations; their
environmental dependence is proposed research, not an existing runtime control.
Keep conditions as a sidecar validity domain and create new configuration
identities for supported changes. P07/P08/P10 consume the relevant evidence;
P12 must preserve session/state grouping during fitting.

### 16. `interactions.cross_string`

With the [setup matrix](strings-frets-and-setup.md#setup-matrix), excite one
identified string and record other strings' motion, body-port motion and the
input reference while controlling initial ringing, detuning and damping.
Repeat a reference condition between interventions. Separate physical transfer
from microphone cross-talk and direct accidental excitation using independent
mechanical observations. A velocity ratio between strings is dimensionless;
without calibrated force it is not a mobility in (m/s)/N, even though that is
the manifest's target unit. Retain useful ratio data with its actual quantity
and keep the force-normalized claim unresolved.

Current string/body/bridge configuration and the shared step in
[instrument.cpp](../../src/instrument/instrument.cpp) jointly predict sympathetic
motion. There is no independent sympathetic gain field. A complete-instrument
transfer is not the bare-body mobility from `bridge.multi_axis_mobility`;
fitting both independently can double-count the strings and bridge. Cross-string
data constrain coupling and detuning combinations but do not uniquely locate
every loss. P09 first checks a prediction from the independently constrained
parts, then investigates remaining body, boundary or contact alternatives.

### 17. `inference.heldout_split`

Follow [independent validation](independent-validation.md) and the
[fit/validation record contract](data-contracts-and-provenance.md#fits-and-validation-records).
Before fitting, retain an immutable inventory of trial IDs, their raw hashes,
roles, grouping rationale and decision criteria. Group overlapping windows,
repeat processing and derivatives of the same take together. Hold out meaningful
conditions or sessions for the claim being tested; a random split of samples from
one decay is not an independent prediction. Shared calibration still introduces
dependence across held-out sessions and belongs in uncertainty.

This channel controls evidence use; it has no runtime physics field. P12 must
record which data influenced parameter choice, model choice and stopping rules.
Once a supposed holdout guides those choices, record its reuse and preserve a
new independent test or narrow the claim. A split declaration alone cannot
establish identifiability or fidelity. P17 receives frozen candidates,
preregistered criteria and still-unseen permitted observations; it does not
receive a curated collection of the best-fitting takes.

## Instrument-specific data reduction sequence

**Geometry first.** Scan or measure without altering the instrument; calibrate
scale and register the origin, axes and surface landmarks. Repeat the
registration with a held-out landmark set. Create a geometry mesh with
occluded regions marked unknown. Distinguish solid wooden soundboard, bowl
and supporting neck/joints. Publish source mesh hashes and the uncertainty
on every high-influence string/bridge anchor.

**String parameter identification second.** Record individual string length,
mass/density, tension and tuning. Fit bending stiffness jointly from several
partial frequencies and estimate damping independently from decay trajectories
under controlled excitation. Repeat with at least one changed tension and
stopped length, when authorized, to test competing stiffness, termination,
observation and amplitude hypotheses. These changes do not automatically
identify each loss coefficient; apply the rank/degeneracy check above. If parameter
families cannot be uniquely identified, publish correlated ranges and add an
independent observation.

**Boundary/bridge and body third.** Measure structural mobility in multiple
force/velocity directions under clearly defined free, mounted and held
conditions. Preserve complex matrices \(Y_{ij}(\omega)\), phase, uncertainty,
coherence, force and velocity frames and preload. Check whether reciprocal
components agree within the actual passive/linear scope. Avoid declaring
passivity from a noisy real part at a single bin; derive physical conditions
and intervals. Compare these data with a converged FEM model using actual
geometry and admissible wood/joint assumptions.

**Nail/fret dynamics fourth.** For each consented performer gesture, capture
relative contact trajectory and synchronized force or a defensible alternate
inference method. Determine contact onset, normal/tangential components,
release, repeated engagement and force/velocity hysteresis. Work is
assigned to the named power port: force times string-point velocity gives string
work, whereas force times relative velocity describes contact energy exchange.
Use the separate actuator, stored-potential and dissipation terms below.
If direct force calibration is not possible, the law is underidentified;
report the inferred family and uncertainty rather than a unique measured
nail model. Protect performer-identifying source video.

**Acoustic observation fifth.** Calibrate the excitation, microphone response,
position, room and time base before comparing a synthesized pressure signal
with recordings. Compare directivity and distance over held-out positions,
not just one pretty sound from one mic. Test multiple expressive dynamics and
cross-string sympathetic response. Preserve the physical energy states even
when a downstream observation or room filter changes the output.

**Blind predictive validation last.** Lock the training/holdout split before
fitting. Recompute parameter uncertainty and sensitivity. Compare predicted
bridge mobility, decay, partial phase, pluck/release timing, acoustic spectra
and gesture response to genuinely held-out sessions. Report systematic
failures and mismatches. A good fit to training FRFs does not qualify a
physically equivalent setar.

## Contact work: distinguish the three measured integrals

For a fixed contact point and fixed scalar direction, let \(x_s\) be the string
displacement at that point, \(u\) the prescribed nail/finger surface coordinate,
and \(z=x_s-u\) the gap coordinate (m). Define \(F\) as the signed force on the
string along that direction (N). For the current elastic/viscous contact during
an interval with its damping gate fixed,
\[
F=-V'(z)-R\dot z,\qquad
W_s=\int F\dot x_s\,dt,\quad W_a=\int F\dot u\,dt,\quad
W_{\rm rel}=\int F\dot z\,dt .
\]
Thus
\[
W_s-W_a=W_{\rm rel}=-\Delta V-D,\qquad
D=\int R\dot z^2\,dt\ge0,\qquad W_a=W_s+\Delta V+D .
\]
All three integrals have units J, but they answer different questions.
\(W_s\) is work on the string; \(W_a\) is external work supplied through the
prescribed surface; \(W_{\rm rel}\) includes the change in contact potential
and dissipation. It is not by itself the injected string energy or the contact
loss. A closed loading/unloading loop estimates loss only under the stated
model with equal endpoint potential and no omitted internal states.
For the one-sided power law, \(V=K[-z]_+^{\alpha+1}/(\alpha+1)\).

Retain both motion channels and their synchronization covariance with force.
Report uncertainty in each integral and in endpoint compression. If the force
is inferred through a model, label the work as inferred and propagate that
model's uncertainty. An unresolved force gain, velocity projection or contact
endpoint prevents a calibrated dissipation claim even when audio looks plausible.
Changing longitudinal contact position or direction adds transport/actuator
terms: the derivative along a moving contact is not the same as the string
material velocity. The fixed-point identity above must then be extended.

The current [discrete solver derivation](../numerics/15-modal-contact-solver.md#4-energy-balance-of-the-coupled-step-including-moving-obstacles)
and [activation/release contract](../numerics/15-modal-contact-solver.md#5-activation-release-and-moving-contact-points-engine-contract)
specify a discrete balance with stored contact potential, damping and signed
prescribed-motion work. `EnergyLedger` in
[instrument.hpp](../../include/setar/instrument/instrument.hpp) separately
records `interaction_potential_j`, `external_work_j`, `dissipation_j` and
`switching_j`. The engine books remaining nail/finger potential removed at release
as dissipation, and contact relocation changes as actuator work. That is a
declared model policy, not evidence that a real nail dissipates all its stored
energy at detachment. Measured recovery into nail/finger/string motion would
motivate a proposed dynamic contact/release model under P05/P06, with its own
energy states and independent oracle.

## Manifest mechanics: enforced and explicitly not enforced

The Python [validator](../../tools/validate_measurement_campaign.py) is a
fail-closed **structure and file-integrity checker**, not an automated
laboratory. Run its default linter on the public blank template:

```sh
python3 tools/validate_measurement_campaign.py data/measurement-campaign-template.json
python3 tools/test_evidence_gates.py
```

It must say 17 channels, **0 acquired**, **0 review-ready** and
`scientific_truth_or_validity_proven_by_this_tool: false`. To qualify a real
private dataset, copy the template *into the authorized dataset directory*,
set `evidence_class` to `specimen_acquisition_manifest`, enter real global
IDs, and attach each raw file using a relative path and an actual lowercase
SHA-256. Keep original files beside that private manifest. Mark each channel
`acquired_unverified` only after an actual capture; `review_ready` requires
protocol, calibration, uncertainty, condition, permission and human reviewer
references. Rejected records remain `rejected` with a separately maintained
reason rather than being replaced by silence. A missing packet stays `missing`.

```sh
python3 tools/validate_measurement_campaign.py private-campaign/manifest.json
python3 tools/validate_measurement_campaign.py private-campaign/manifest.json --require-ready
```

The second command is **expected to fail on the published template**. It is
only a completeness gate requiring all 17 ready channels and explicit signed
human/material admissibility statuses; it still cannot prove that an owner
consented, a force sensor was calibrated or a wood tensor is physically real.
Manual independent source review and actual physical tests remain mandatory.
The unit vocabulary `SI_mixed` / `SI_tensor` is a container-level statement:
each result file must provide dimensions and covariance for every field;
the current validator does not parse such files or substitute for a physical
unit/admissibility audit.

The validator checks attached raw files for acquired/review-ready channels, but
does not resolve protocol/reviewer identifiers to real documents, interpret
result covariance, check passivity, or execute a human review. Its `rejected`
branch preserves the record without applying the acquired-file checks, so
rejected-data integrity must also be audited under the dataset's retention
policy. A syntactically acceptable reference string is not evidence that the
referenced procedure happened. Preserve the public template's null identifiers,
empty `raw_sources`, 17 `missing` channels, `not_evaluated` physical
admissibility and `not_performed` human review until a separate real campaign
exists; do not use demonstration bytes to make it look complete.

## Review, promotion and the next work order

### Review decisions are separate from manifest status

`acquired_unverified` means an actual capture is attached; `review_ready`
means the required references are supplied for review. Neither is a parameter
promotion or a fidelity verdict. Use a signed finding/promotion record alongside
the manifest for these outcomes:

| Scientific outcome | Required action |
| --- | --- |
| Accepted for stated scope | State the calibrated quantity, uncertainty, admissible domain and independent check that passed. A sound raw observation may be accepted while the proposed parameter mapping remains inconclusive. Promote only the quantity/model consequence actually supported. |
| Rejected | Preserve the raw/derived lineage and reason: calibration or provenance failure, physical inadmissibility, incompatible setup, or a failed model prediction. Distinguish rejected acquisition from a rejected hypothesis so valid observations are not discarded with a bad fit. Use the existing manifest `rejected` status when the acquisition itself is rejected. |
| Inconclusive | Retain correlated ranges, unresolved alternatives and the smallest additional observation that separates them. Do not choose a unique coefficient because the optimizer demands one. A computationally convenient choice remains an explicitly assumed/inferred model choice. |
| Blocked | Name the missing instrument access, permission, calibration, observable or implementation. Leave missing evidence missing. Independent synthetic verification or analysis design can continue, with its separate label. |

Before parameter promotion, the measurement reviewer checks raw/calibration
lineage; the domain reviewer checks interpretation, units, signs and admissibility;
the model reviewer checks identifiability and held-out predictions; and the rights
reviewer checks permitted use. Record which independent checks each person
performed. A failure in one layer is not cured by passing another.

### Current configuration boundary and proposed integration

`Instrument::prepare()` in [instrument.cpp](../../src/instrument/instrument.cpp)
accepts an in-memory `InstrumentConfig` on the control thread and calls
[config validation](../../src/instrument/config.cpp). It checks numerical and
structural constraints; it does not read the acquisition manifest, verify
units from a file, carry covariance, or decide whether values are measured.
[setar_play.cpp](../../apps/setar_play.cpp) calls
`synthetic_setar_config()`, while `build_body()` explicitly emits synthetic
provenance. There is no general measured-asset loading/publishing pipeline
implemented by these functions.

A future integration work packet therefore needs a reviewed offline conversion
contract, per-quantity lineage/covariance sidecar, explicit mapping and rejection
rules, independent fixture/prediction checks, version identity and rollback.
It must keep the existing synthetic preset as a regression fixture. Until that
work exists, deliver candidates and a field mapping for implementation review;
do not describe the manifest validator or specimen template as a runtime loader.
`Provenance` is one coarse enum for a whole configuration/body, not a
per-quantity evidence record. One measured length cannot justify marking
synthetic contacts, body modes and observation weights collectively `Measured`.
A mixed-package labeling policy remains a proposed prerequisite.

Current tests provide useful **synthetic verification comparators**, not measured
acceptance. The named cases are registered in [CMakeLists.txt](../../CMakeLists.txt);
inspect their assertions before reusing a tolerance:

| Existing tests and source | What the candidate still needs |
| --- | --- |
| `core.frequencies`, `core.wavenumber_loss` in [reference_tests.cpp](../../tests/reference_tests.cpp); `numerics.loss_decay_oracle` in [numerical_prototype_tests.cpp](../../tests/numerical_prototype_tests.cpp) | Independent measured string estimates, an identifiable loss model and a boundary-aware held-out prediction. |
| `solver.discrete_gradient_identity` in [numerics_tests.cpp](../../tests/instrument/numerics_tests.cpp); `contact.nail_passivity` and `contact.release_convergence` in [engine_tests.cpp](../../tests/instrument/engine_tests.cpp) | Force/motion calibration, empirically supported contact/release physics, and agreement over unseen trajectories. |
| `materials.reciprocity`, `materials.positive_energy`, `body.mobility_reciprocity`, `body.mobility_passivity` in [body_tests.cpp](../../tests/instrument/body_tests.cpp) | Real constitutive/FRF evidence, checked normalization and port coverage, uncertainty and out-of-band limitations. |
| `coupling.cross_string_transfer`, `coupling.uncoupled_limit` and `acoustics.observation_invariance` in [engine_tests.cpp](../../tests/instrument/engine_tests.cpp) | Calibrated sympathetic transfer and acoustic prediction. Observation invariance checks separation from mechanics, not correct radiated pressure. |

These are present test names, not a claim that they were executed during this
campaign or that the proposed measured-data tests already exist. A candidate
change must separately retain the exact commands, source/build identity and
executed results required by its work order.

### Dependency-ordered handoff

Assign these existing IDs using the
[engineering register](../../roadmap/engineering-tasks.json) and
[phase-task crosswalk](../../roadmap/phase-task-crosswalk.json).
The scientific `Pxx-Tyy` task and an `ENG-Pxx-Tyy` work order are separate
namespaces. The table names the next receiving work, not completed gates.

| Packet/decision | Receiving phase tasks and engineering work | Requirement and prerequisite |
| --- | --- | --- |
| Access, specimen identity, acquisition scope | P01-T03 → `ENG-P01-T02`; charter `ENG-P00-T01` | `REQ-GOV-001`, `REQ-GOV-002`, `REQ-GEO-001`; actual authorized specimen and declared claim |
| External/interior/soundboard geometry | P07-T01/P07-T02 → `ENG-P07-T01`; P08-T01 → `ENG-P08-T01` | `REQ-GEO-001`, `REQ-BOD-001`; calibrated registration, unresolved regions and convergence plan |
| Materials and environment-dependent bounds | P07-T03/P07-T05 → `ENG-P07-T02` | `REQ-GEO-002`; frames, covariance, reciprocal positive-energy constitutive model |
| String dispersion and loss candidates | P04-T01/P04-T02 → `ENG-P04-T01`; P04-T06 also → `ENG-P12-T01` | `REQ-STR-003`, `REQ-IDN-001`; independently constrained geometry/density, boundary model and rank decision |
| Nail/flesh identification and implementation | P05-T01 → `ENG-P05-T01`; P05-T02 → `ENG-P05-T02`; P05-T04 → `ENG-P05-T03` | `REQ-EXC-001`, `REQ-EXC-002`, `REQ-PRF-002`; synchronized force/motion, work and release-state evidence |
| Fret/action/contact consequences | P06-T01/P06-T02/P06-T03 → `ENG-P06-T01` | `REQ-FRT-001`; geometry plus controlled press/slide/release evidence; tension-rise extensions also require `ENG-P04-T02` / `REQ-STR-004` |
| Bridge FRFs and body reduction | P08-T04 → `ENG-P08-T02`; P08-T05 → `ENG-P08-T03` | `REQ-BOD-002`, `REQ-BOD-003`; complex calibrated ports, normalization, passive fit and held-out channels |
| String support and sympathetic coupling | P09-T01 → `ENG-P09-T01`; P09-T03/P09-T04 → `ENG-P09-T02` | `REQ-CPL-001`, `REQ-CPL-002`; accepted body/string candidates, signed power ports and independent cross-string prediction |
| Cavity, radiation and microphones | P10-T01/P10-T02/P10-T03 → `ENG-P10-T01`; P10-T04/P10-T06 → `ENG-P10-T02` | `REQ-ACO-001`, `REQ-ACO-002`; boundary/loading definition, calibrated geometry and observation |
| Performer events and course/gesture meaning | P11-T02/P11-T03/P11-T04 → `ENG-P11-T02` | `REQ-PRF-002`; practitioner review and physically supported event mapping |
| Identifiability, frozen fitting and independent validation | P12-T01/P12-T03 → `ENG-P12-T01`; P12-T02/P12-T04 → `ENG-P12-T02`; P17-T01/P17-T03 → `ENG-P17-T01` | `REQ-IDN-001`, `REQ-IDN-002`, `REQ-FID-001`; accepted observations, candidate freeze and unused validation conditions |

End each block with the manifest and derivative hashes, review decisions,
accepted and unresolved quantities, exact destination symbols, proposed
extensions, downstream owner and still-open gate. Partial campaigns may support
a restricted mechanism study; all 17 channels being review-ready is neither
necessary for every narrow experiment nor sufficient for a digital-twin claim.
The full `--require-ready` gate retains its existing completeness requirement.

## Nonnegotiable blockers before a digital-twin claim

- No accessible identified reference instrument or owner/performer permission:
  acquisition cannot begin. Do not simulate permission or fabricate a scan.
- No calibrated force/velocity bridge channel: a measured multi-axis bridge
  admittance claim is unavailable; fitting magnitude spectra is not a substitute.
- No nail-contact force data or identifiability proof: use a bounded hypothetical
  contact model with a declared experimental uncertainty, not "exact nail physics".
- No admissible wood material tensor or body boundary conditions: even a
  numerically converged FEM result cannot be called specimen-validated.
- No held-out acoustic/gesture observations: do not promote the physical
  model from synthetic or fitted to predictive validation.
- No rights to redistribute a measured file: retain an inaccessible source
  identifier and scope/reproducibility limitation, or obtain permission
  through the owner; never publish the file as an implicit byproduct.

**Handoff:** on completion of a *real* campaign, attach the private manifest's
hash, accepted physical parameter asset hashes, signed review references and
explicit admissible claim envelope to the model calibration change. Until
then, leave the main repository's default coefficients explicitly synthetic.
