# Reference setar digital-twin input completeness and acquisition matrix

**Status: acquisition specification. No physical specimen has been declared
fully measured by this repository.** A generic Iranian setar has considerable
construction, age, setup and performer variation. An ideal 1:1 reproduction
would require unbounded microscopic detail and perfect knowledge of conditions;
that is neither measurable nor a responsible product claim. The achievable
engineering target is **bounded predictive fidelity** for named instruments,
players/gestures, positions, environments and calibrated observation chains,
with uncertainty and a progressively extended validity domain.

**Instrument identity matters:** the Iranian setar has a wooden resonating
body/soundboard; do not import the skin-membrane soundboard model of the
Iranian **tar** by name similarity. The Smithsonian's account describes the
setar's wooden soundbox and movable tied frets; the tar's membrane construction
is a distinct instrument: [Smithsonian setar description](https://asia-archive.si.edu/podcast/master-of-persian-music-hossein-alizadeh-tar-and-setar/).

## Choose the actual target first

Give each accessible instrument a unique specimen ID and preserve the owner,
maker attribution *if known*, modifications, string installation/tuning,
fret state, instrument orientation, date/environment of measurement, and
permission/consent. If physical access is unavailable, leave the specimen
unselected; published average parameters are not a surrogate for a full
instrument scan. The core can still undergo synthetic mathematical verification.

Keep independent IDs for **specimen** (physical object), **setup** (strings,
frets, tuning and environment), **session** (instruments, calibration and
conditions), **gesture** (what a player did), **observation** (measurement
geometry/recording chain), **model** (scientific hypothesis), and **calibration**
(fitted parameters and train split). A session must not silently inherit a
setup from another day, and one microphone file does not identify a nail
trajectory without independent measurement.

The example [specimen template](../../data/specimen-template.json) shows a
**deliberately unmeasured** schema skeleton. Every unknown is explicit. It
cannot be loaded as a validated runtime instrument by simply changing its
status string.

## Physical data demand and the method of obtaining it

| Domain | Required fields and units | Practical acquisition and controls | Independent validation and likely ambiguity |
| --- | --- | --- | --- |
| Identity/assembly | `specimen_id`, maker/age if known, visible modifications, four-string layout, stable photos, consent | Interview owner; calibrated whole-instrument photography; inventory replaceable parts | Construction variants; maker attribution must not be guessed |
| Coordinate system | World/body/bridge/string origins and axes, transforms, dimensional units | Survey landmarks, calibrated scale bars, repeat registration | Forward/inverse transform residual, axis sign and scale test |
| String geometry | Open vibrating and total lengths (m), bridge/nut/peg contacts, spacing, gauges and afterlengths | Calipers, microscope/photography, controlled tension setup | Contact point not always a mathematical fixed support |
| String mass and stiffness | Linear density kg/m, axial tension N, flexural EI N·m², surface/winding and loss | Weigh calibrated segment where permitted; controlled frequency vs pitch/dispersion; multiple tensions | String composition or EI/T degeneracy, uncertain end corrections |
| String loss | Frequency-, amplitude- and environment-dependent decay/loss (s⁻¹ or model-specific) | Quiet decay repeated under changed fretting/tension, multi-sensor modal analysis | Body transfer, radiation and string internal damping can imitate each other |
| Fret structure | Count, positions (m), tied material/radius, fret height, compliance/friction, movable setup | Macro imaging, stylus/optical measurement, load/displacement when authorized | Effective stopping position changes with force and angle |
| Nail/flesh geometry | Fingernail shape/thickness, finger pad geometry, contact orientation | Consent-controlled high-speed imaging, calibrated scale and multi-angle synchronization | Visible trajectory does not automatically reveal force or internal friction |
| Nail gesture dynamics | Contact path (m), velocity (m/s), engagement/release, contact force (N), friction/compliance | High-speed camera and appropriate nonintrusive force/response probes, repeated stroke design | Sensor intrusion changes pluck; derive prior with uncertainty if direct force unavailable |
| Left-hand motion | Position, press/release speed, force, damping, ornaments and transitions | Motion/force observation synchronized with string state and acoustic outputs | Performer and technique-specific; maintain privacy and consent |
| Bridge mechanics | 3D position/orientation, local geometry/mass, string-force directions, attachment stiffness | Scaled imaging, controlled force/velocity FRFs at multiple axes, correct sensor mass | Strong directional behavior and support preload; compare force sign/frame |
| Body dimensions | Bowl curvature, opening, wall thickness map, wooden soundboard geometry, rim, neck joints | Calibrated photogrammetry/structured light; permitted thickness/ultrasound methods | Occluded internal thickness, seams and local defects; no fabricated interior |
| Wood/material | Density kg/m³, grain direction, orthotropic moduli Pa, shear moduli and Poisson terms, loss | Non-destructive ultrasonic/modal methods, coupon tests only with permission, controlled humidity | Orthotropic reciprocity/positive definiteness must be tested; literature tables can contradict |
| Wooden soundboard/joints | Plate thickness m, density kg/m³, directional bending stiffness, sound holes, glue/contact compliance | Survey and independent vibration/deflection, reversible fixtures | Interior thickness, brace/joint stiffness and local grain often partially unobservable without disassembly |
| Structural dynamics | Free/playing boundary condition, complex mobility (m/s)/N, mode shapes, damping, residues | Calibrated multi-input/multi-output hammer/shaker/velocity FRFs, repeated mount configurations | Clamp, sensor, temperature and playing support alter modes |
| Cavity/boundary air | Cavity geometry, openings, leaks, internal boundary impedance, air density/speed | Scan accessible geometry, measure resonances and leakage under controlled conditions | Losses and material damping trade off; soundhole/aero behavior is frequency-dependent |
| External radiation | Pressure Pa per controlled drive, directivity, mic positions/angles, phase reference | Anechoic/free-field where available or measured room response, microphone array with spatial registration | Room, near-field and mic response entangle source directivity |
| Observation chain | Capsule calibration, gain, preamp, clock, A/D, distance/orientation, room IR, processing | Calibration tones/known excitation, synchronized timestamps, raw float/PCM, no limiter | Acoustic level and phase cannot be compared without a calibrated reference |
| Dynamic performance | Repeated gestures across tuning/positions/dynamics/players, annotated ornaments | Consent-approved repeated and natural phrases, independent gesture/video/acoustic channels | Musical diversity; player fatigue and session variability |
| Environment | Air temperature °C, RH %, pressure Pa, wood moisture indicator, instrument conditioning | Calibrated sensors logged throughout session and replicate | Material, tuning and air sound speed drift between sessions |

These fields are *proposed acquisition demands*, not a claim that a single
paper, maker's plan, CAD file or frequency spectrum provides all of them.
For some irreversible operations—cutting coupons, disassembling a historic
instrument—use non-destructive measurement and a model-based uncertainty
distribution instead. Never presume the owner authorizes destructive inspection.

## Acquisition execution sequence: one session at a time

1. **Permissions and setup**: verify ownership, scientific intent, copying and
   distribution rights, storage access, consent/revocation, environmental
   controls, and whether cameras or identifiable performance recordings are
   allowed. Obtain authorization before contacting people or modifying gear.
2. **Calibrate instruments**: record traceable mic sensitivity, accelerometer or
   laser vibrometer transfer (including mass loading if relevant), force sensor,
   ruler/scale bars, optical frame timing, A/D gain and shared clock.
3. **Freeze coordinate registration**: survey bridge, nut, bowl, cavity opening,
   string/fret and mic/reference points; define positive port force and velocity
   axes. Take repeated registration shots to estimate frame uncertainty.
4. **Document the playing setup**: physical string identities, pitch/tuning,
   material/gauge, tension inference method, fret configuration, support,
   instrument temperature/RH, and any prior repair or tuning movement.
5. **Acquire isolated string/structure tests**: where safe, collect controlled
   plucks, fretted/open decays and bridge FRFs in different directions. Avoid
   assuming that one decay constant belongs only to the string.
6. **Acquire contact and dynamic gestures**: high-speed or otherwise calibrated
   pluck/fret trajectories with synchronized acoustic/mechanical measurements,
   repeated strokes and documented player variation. Preserve raw timestamps.
7. **Acquire observation/directivity**: controlled angular and distance sweeps
   and room/mic references, without changing instrument setup between related
   measurements unless logged.
8. **Capture uncertainty**: repeated acquisitions, calibration drift,
   Type-A/Type-B uncertainty estimates, parameter covariance and known hidden
   variables. Use a principled reporting method such as
   [NIST Technical Note 1297](https://www.nist.gov/pml/nist-technical-note-1297).
9. **Split before fitting**: freeze training versus independent holdout by
   specimen/setup/player/session/gesture/angle so a validation claim is not
   secretly trained on the same event.
10. **Publish a *redacted* manifest**: hash raw permitted files, store units,
    calibration/conditions, uncertainty, license and link/capability to
    reproduce. Do not commit unconsented identity, recordings or restricted
    source data to public GitHub.

Use [measurement protocols](README.md) for the instruments and signal
processing details. This page defines the all-domain handoff, not a replacement
for calibration of each sensor and operator.

## Data-contract example and semantic validation

A logical field is a record with `quantity`, `unit`, `value`, `uncertainty`,
`evidence_class`, `source_or_session_id`, `validity`, `status`. A number without
its method and uncertainty is insufficient. Correlated parameter sets (e.g.
orthotropic elastic constants) must retain a covariance or joint prior, not
independent standard-deviation labels that destroy physical admissibility.

The [JSON template](../../data/specimen-template.json) is safe to distribute
because it contains no actual recordings, private owner details or measured
values. Its missing values are `null` and status `unmeasured`. A distinct
[17-channel acquisition campaign schema and hash validator](13-acquisition-campaign-execution.md)
is now executable; it checks measurement *packet completeness*, not every
field or physical admissibility of the separate specimen-parameter template.
A future canonical specimen-asset validator and **separate** physical admissibility test
must reject a logically complete but non-SPD material tensor or inconsistent
port units. Even a correct schema cannot certify a sensor calibration or
consent document.

For each physically measured quantity, preserve a source reference and
file-level hash; for inferred quantities preserve the exact likelihood/objective,
training set hash, optimizer version and uncertainty; for priors preserve their
published origin and applicable boundary conditions. A copied figure with
axes visually inspected but no raw phase channel does not count as a complete
complex FRF. Record missing data as a **blocking unknown** or an explicitly
limited approximation, never as a fabricated default.

## Critical-path unknowns and independent experiments

1. **True contact compliance**: If no force channel exists, determine whether
   motion/shape and repeated acoustic responses identify a law. If they do not,
   represent family/uncertainty and design an independent force measurement;
   do not claim physically exact nail mechanics.
2. **Playing bridge admittance**: If a paper's modes are measured with a clamp,
   remeasure or model the playing support. A clamp-only fit has a different
   validity boundary.
3. **Bowl, wooden soundboard and joints**: If interior dimensions or moisture/preload
   are unknown, use ranges and sensitivity. Models that predict a held-out
   mode only after arbitrary hidden damping compensation need more data.
4. **External acoustic chain**: If there is no calibrated room or mic phase,
   state fidelity may be examined in mechanical port quantities first; do not
   report absolute radiation directivity or pressure validation.
5. **Cultural/playing coverage**: A physically plausible pluck of one open
   string does not cover idiomatic performance. Obtain practitioner-reviewed
   gesture and repertoire conditions before declaring expressive fidelity.
6. **Instrument variability**: Success on one specimen is evidence about that
   specimen and setup. Generalization to another maker/setar or humidity state
   needs new held-out measurements and error reports.

Complete specimen data are **relative to a declared model and claim envelope**:
when its parameters, measurement uncertainty and held-out predictions cover
all required effects at the chosen scale, the acquisition gate can be marked
passed *for that scope*. Do not use the absolute phrase “every atom documented”
or 1:1 physical identity as a scientific release claim.

Handoff tasks: `ENG-P01-T02` (acquisition plan), `ENG-P07-T01/T02` (geometry/materials),
`ENG-P08-T02` (mobility), `ENG-P05-T01` (nail/flesh), `ENG-P10-T02` (radiation) and
`ENG-P12-T01/T02` (identifiability/calibration). Their outputs must pass the
[requirements matrix](../../roadmap/requirements.json) before moving from
synthetic Q1/Q2 claims to measured Q3 claims.
