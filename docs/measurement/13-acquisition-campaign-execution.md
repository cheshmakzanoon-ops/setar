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
stopped length to separate material stiffness, termination effects, observation
position and amplitude dependence. If parameter families cannot be uniquely
identified, publish correlated ranges and add an independent observation.

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
\(\int F\cdot v_{\rm relative}\,dt\) under a defined sign convention.
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
