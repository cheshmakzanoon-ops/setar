# Reference instrument selection and configuration control

## Define the intended claim

Select the reference instrument only after stating whether the project aims to
represent one identified specimen, a configurable family of setars, or a
performer-centered behavior set. These claims need different evidence. One
well-characterized instrument can support a specimen-specific model. It cannot
establish population variability or a universal construction. A collection of
poorly documented instruments does not automatically support the broader claim
either.

The initial recommendation is methodological: establish a carefully documented
reference specimen and retain separate specimens for generalization. This is a
proposed design, not evidence that any instrument has been obtained. Do not choose
solely on prestige, cost, visual symmetry, or one impressive recording. Choose an
instrument that knowledgeable players consider representative of the intended
musical use, whose owner permits the required non-destructive work, and whose
state can be tracked.

The source register R-SETAR-TABASSIAN and R-FRETTING-POORHAYDARI-2023 motivates
contextual performance documentation. R-SETAR-MANSOUR-SCAVONE-2012 motivates
attention to measurement disturbance. Their role is guidance, not certification
of the project's chosen specimen.

## Selection dossier

Create a dossier before any intervention. It includes the proposed claim,
ownership or loan authorization, maker attribution and its evidence, known repair
history, current condition, string and fret condition, player assessment, permitted
handling, and excluded actions. Unknown maker history must remain unknown. An
inscription, seller description, and expert attribution are different evidence
types and should not be collapsed into one confident field.

Record why this specimen was selected and which selection biases follow. A newly
made instrument may be easier to measure with matched witness wood but may not
represent older instruments. A frequently played instrument may offer valuable
performance experience while its exact construction materials are inaccessible.
A fragile historical specimen can be inappropriate for contact measurements even
if its cultural significance is high. The research question does not override
the owner's restrictions or conservation judgment.

Ask more than one qualified player to describe strengths, limitations, and
typical applications without first revealing which characteristics the modeling
team hopes to find. Preserve disagreements. The goal is to define a target domain,
not manufacture consensus. If the instrument is unusually resonant, muted, bright,
or difficult in a way relevant to the model, that can be documented as a specimen
trait rather than hidden as an inconvenient outlier.

## Condition and safety baseline

Photograph all accessible surfaces with scale references and neutral lighting.
Record cracks, repairs, finish wear, loose components, bridge placement, peg
condition, fret movement, and any existing buzzes. The photographs establish a
before state, not a diagnosis of structural integrity. A qualified maker or
conservator should review questionable conditions before the planned intervention.

Measure total mass with a suitable calibrated balance and document what is
included: strings, pegs, decorative elements, removable pickups, and any markers.
Repeat the reading after repositioning to assess handling variability. Do not
remove parts merely to obtain a cleaner mass number unless removal is approved.
The mass of an assembled instrument is useful even when component masses remain
unknown.

Define a handling surface, support locations, lifting method, and responsible
operator. Keep incompatible adhesives, solvents, unguarded clamps, and sharp
tools away from the instrument. A laser measurement may be mechanically
non-contact but still requires optical safety review and care with reflective
markers. “Non-destructive” is a goal to substantiate, not a label that makes all
methods safe.

Stop if a new crack, unexpected bridge movement, joint noise, unusual resistance,
or other distress occurs. Document the state and consult the authorized owner or
specialist. Do not continue merely to complete a planned grid. Do not attempt
repairs or retensioning beyond the approved procedure.

## Coordinate system and landmarks

Define a right-handed specimen coordinate frame from reproducible landmarks. One
reasonable project convention is a longitudinal axis along the nominal neck and
body centerline toward the nut, a transverse axis across the soundboard, and an
outward normal completing the frame. The actual choice may differ, but it must be
explicit and used consistently. Do not assume a curved or asymmetric instrument
has an exact plane of symmetry.

Give landmarks stable identifiers independent of mesh node numbers. Useful
landmarks include nut reference points, bridge reference points, body outline
points, string-holder locations, and neck/body junction references. Their
definitions must specify whether they refer to a geometric center, contact edge,
surface point, or fitted line. A bridge apex is not a single ideal point when the
contact is finite or worn.

Store the transformation from each acquisition frame to the specimen frame,
including its uncertainty and registration residual. The microphone array,
motion cameras, laser scanner, and modal measurement grid may each have distinct
frames. A photograph with arrows is useful for human review but does not replace
the numeric transform needed for analysis.

## Configuration identity

The specimen identifier remains fixed across ordinary setup changes. A
configuration identifier describes string installation, tuning state, bridge
position, fret map, nut and string-holder contact, support condition, and attached
equipment. Define which changes create a new configuration and which are repeated
observations of the same state.

At minimum, retuning beyond a declared tolerance, moving a fret, replacing a
string, changing bridge position, adding a sensor, or changing a support must be
recoverable in the configuration history. Some projects may encode sensor changes
in an acquisition-chain identifier rather than configuration; either design is
acceptable if the relationship is explicit and analysis cannot mix states by
accident.

Store intended tuning and measured pitch separately. A nominal note name is not
a tension measurement. Record the measurement window, excitation, temperature,
and whether the pitch was taken before or after a trial group. The source of a
tuning target may be a player instruction, a preset, or an ensemble reference;
retain that distinction.

## Reversible interventions

Prepare an intervention record before changing anything. State the research
question, expected effect, authorization, pre-state measurements, exact action,
safe reversal, and verification after reversal. Examples include temporary string
damping, approved reflective markers, a changed support, or a player-approved fret
adjustment. Do not assume that removing an object restores the original state:
adhesive residue, string creep, peg slip, and bridge settling can persist.

After a reversible change, repeat a compact reference measurement. Compare
selected pitches, bridge position, and a small mechanical response set to the
pre-intervention state. If the state does not return within the declared
repeatability domain, retain a new configuration instead of relabeling it as the
old one. This is especially important when calibration and independent validation
are intended to refer to the same specimen state.

## Specimen-family strategy

If the product will expose multiple instrument characters, distinguish measured
specimen presets from parameter variations. A preset derived by changing stiffness
or damping within a safe range is a designed variant, not a recording-backed
replica of an unmeasured instrument. Name and document it accordingly.

Select independent specimens to challenge generalization rather than merely add
more examples from the same workshop and setup. Differences in maker, construction,
age, setup, and player practice may be relevant, but do not collect sensitive or
unnecessary personal history. The selection rationale should connect to a
mechanical or performance hypothesis. A broader sample permits broader claims
only when the acquisition quality remains comparable.

Keep one specimen or configuration inaccessible to iterative calibration when
feasible. If all specimens are used to choose the model architecture, later tests
on those specimens are not fully independent. Report this honestly and acquire
new evidence before making a stronger generalization claim.

## Template and acceptance

```yaml
specimen_id: SPEC-PENDING
status: proposed
maker_attribution:
  value: null
  evidence: null
owner_authorization_id: null
reference_claim: null
condition_record_id: null
coordinate_frame_id: null
configuration_ids: []
permitted_interventions: []
prohibited_interventions: []
handling_approver: null
unresolved_questions:
  - No instrument has yet been accepted into this protocol.
```

The dossier passes when a new operator can identify the exact instrument, handle
it safely, reconstruct its relevant configuration, and determine whether a
proposed measurement is authorized. It fails when important state lives only in
memory, when a maker attribution is asserted without evidence, or when the
reference claim exceeds the selected population. The next action is a small
non-destructive repeatability pilot, not immediate dense acquisition.

