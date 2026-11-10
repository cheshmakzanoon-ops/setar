# P01 — Audit sources, data, and instrument identity

**Status:** specification with an initial source audit in the research volume.
**Prerequisite:** P00. **Exit evidence:** a usable evidence register, admissible
parameter policy, acquisition plan, and defined held-out observations.

## Objective

Determine exactly what is known, what is merely asserted, and what is unavailable
before encoding physical constants or geometry. The literature establishes useful
methods and observations; it does not provide a complete measured digital twin
of an identified setar. A public paper's CAD illustration, a quoted wood species,
and a table of resonant frequencies cannot supply all model inputs.

Read [research](../research/README.md), [measurement](../measurement/README.md),
and [geometry](../geometry/README.md). Use the source ledger's original links,
access status, and exact page/table identifiers. Do not treat this roadmap as a
replacement for the publications.

## Entry checks and research routes

Start with the P00 claim envelope, the current synthetic configuration, and the
[source atlas](../research/source-atlas.md). Preserve the atlas's dated access
statements; a later researcher must record access actually achieved again. Use
the [research-to-model workflow](../research/research-to-model-workflow.md),
[finding record](../../templates/research-finding.md), and
[promotion record](../../templates/parameter-promotion.md) for the decisions below.
The recipes specify work to execute; they do not report new source retrieval,
specimen access, consent, measurements, or completed scientific gates.

| Decision | Primary route and exact starting location | Evidence still needed locally |
| --- | --- | --- |
| Construction and structural assumptions | `R-SETAR-PEDRAMMEHR-2018`, §2, §3.1–§3.2/Table 1, §4–§5; publisher links in the atlas | Exact specimen/configuration, numerical geometry, material provenance and experimental state |
| Apparatus and support effects | `R-SETAR-MANSOUR-2011`, §3; `R-SETAR-MANSOUR-SCAVONE-2012`, §3.1–§3.4 and §4.1–§4.2 | A project loading/remounting pilot with calibrated force and response |
| Unavailable journal detail | `R-SETAR-MANSOUR-2015`, DOI identity only in the current audit | Authorized full text, then an actual section/asset inspection; the 2011 paper is a different source |
| Playing configuration | `R-SETAR-TABASSIAN`, pp. 1–2; `R-FRETTING-POORHAYDARI-2023`, “Methodology and Initial Tests,” paragraphs [7]–[8] | Authorized specimen setup and contextual practitioner review |
| Uncertainty and identifiable inference | `R-METROLOGY-JCGM-100`, §4, §5 and §7 reading route; `R-IDN-RAUE-2009`, abstract/§2.3 excerpts only in the atlas inspection | Measurement equations/covariance; access to any additional method sections before citing them |

Before acquisition, identify missing permissions, calibration, accessible surfaces,
and independent observation groups. Before promotion, require their actual
evidence. P02's synthetic oracle and parser work can proceed while those inputs
remain blocked. Neither accessible literature nor a completed plan changes
`Provenance::Synthetic` in the current preset.

## P01-T01: build a source-to-claim map

For every important construction or acoustic claim, create a record containing
the claim, source identity, exact location, access status, evidence type, and
limitations. A publisher abstract supports a narrower claim than a full paper;
an author manuscript may differ from the final publication. Record the version
actually read. A dead link is not evidence that the paper does not exist, but
it does block verification of its unavailable details.

Distinguish direct measurement, method description, review statement, artist
practice, manufacturer specification, and your own proposed model. Mark whether
the source refers to Iranian setar, another long-necked lute, or a generic string
instrument. Transferable mathematics should not be presented as setar-specific
empirical validation. Preserve spelling variants for search while keeping one
canonical instrument identity in the project.

Review representative claims by independently locating their source paragraphs
or figures. If the source does not support the specificity of the claim, narrow
the claim. Do not add a citation and assume it fixes an overstatement.

### Execution recipe

**Question and mapping.** Which construction, parameter, or behavior claim has
enough primary evidence to constrain this model, and which must remain an
assumption? This task maps to `ENG-P01-T01` and `REQ-GOV-002` in the
[crosswalk](../../roadmap/phase-task-crosswalk.json). Its effect is to prevent
unsupported lengths, losses, body modes, or playing rules from becoming apparent
facts in a preset.

1. Inventory the quantities in [config.hpp](../../include/setar/instrument/config.hpp),
   [body_model.hpp](../../include/setar/instrument/body_model.hpp), and
   [setar_preset.cpp](../../src/instrument/setar_preset.cpp). Make one claim row
   for each consequential value or rule: `StringConfig` geometry, tension, mass,
   rigidity and loss; fret/course/tuning choices; contact laws; body mass,
   participation and observation. Mark the current defaults synthetic. Separate
   an equation's mathematical source from the source of its coefficients.
2. Follow the atlas's instrument/mechanism/implementation passes. Log queries
   such as `"setar" "modal analysis"`, `"سه‌تار" ارتعاش`,
   `"setar" soundboard thickness`, and the exact Mansour 2015 title/DOI.
   Inspect the original publisher or author copy, then references for borrowed
   quantities and citing work for corrections. For each selected source, extract
   the locations in the table above in this order: specimen/boundaries,
   apparatus/calibration, definitions, results, limitations, supplementary assets.
   Log unsuccessful retrievals and the edition actually inspected; do not infer
   full-text access from an abstract or the accessible 2011 paper.
3. Preserve original symbol/unit, measured/fitted/assumed/borrowed origin,
   support/preload, frequency/amplitude range, and uncertainty next to the narrow
   paraphrase. Resolve a disputed sign against the PDF and surrounding definition.
   Record source access separately from claim support and asset rights. Put the
   source-to-claim dependencies in the **proposed** `data/source-ledger.json` and
   unresolved quantities in **proposed** `docs/research/uncertainty-register.md`,
   the destinations named by the work order; use the existing finding template
   for search/extraction evidence without copying whole publications.
4. Have a second reader reconstruct one decisive supported claim and one rejected
   input from their original locations. A structural frequency list can constrain
   a modal comparison but cannot fill `BodyMode.modal_mass_kg`,
   `bridge_participation`, damping, or `radiation_weight`. The latter maps
   acceleration to observation and supplies no acoustic feedback. Reject
   instrument misidentification, inaccessible-detail assertions, or a claim
   stronger than its source; retain unresolved applicability rather than averaging
   unlike specimens. Stop searching when a scoped decision or precise missing
   asset request is possible.

Retain accepted/rejected/inconclusive/blocked dispositions, dependency links and
review reasons. The work order also requires invalidating dependent assets when a
source is withdrawn. Planned `data.provenance_rejection` has the partial CTest
`data.acquisition_manifest_integrity`, checks manifest/file integrity, not this
claim graph or source interpretation. Hand quarantines to P01-T02, acquisition
gaps to P01-T03/T04, and supported method questions to P07/P08 with their claim
limits intact.

## P01-T02: quarantine inconsistent material data

Read the published-parameter audit before importing the 2018 setar material
table. For a linear orthotropic elastic model, convert all entries into a single
unit system and construct the compliance/stiffness representation using one
declared Poisson subscript convention. Check reciprocity, symmetry, finite
positive moduli, necessary positivity conditions, and positive definiteness.

Do not repair a negative sign, exchange axes, or change a decimal point simply
because the result becomes plausible. Preserve the raw transcription. Produce
an audit report showing which relation fails and the size of the discrepancy.
Possible explanations include reporting convention, transcription, units,
measurement uncertainty, or source error; the audit does not establish which
explanation is correct. Request clarification or obtain independent measurements.

Use synthetic admissible matrices to test the importer and solver while real
data are quarantined. Verify that the importer rejects a deliberately nonsymmetric
or indefinite example and reports the problematic field. A Cholesky failure
must not trigger silent diagonal regularization in the physical model.

### Execution recipe

**Question and mapping.** Can a reported material law store positive elastic
energy and obey reciprocity under its stated axes and units? An inadmissible law
can alter body resonances or destabilize the physical system before any sound
comparison. The direct mapping is `ENG-P01-T01` / `REQ-GOV-002`; the material
implementation consumer is `ENG-P07-T02` / `REQ-GEO-002`.

1. Re-extract `R-SETAR-PEDRAMMEHR-2018` §3.2/Table 1 from the
   [publisher HTML](https://www.extrica.com/article/19505) and
   [PDF](https://www.extrica.com/article/19505/pdf), PDF page 4, printed page 2683.
   Read the [published audit](../research/published-parameter-audit.md) and
   [constitutive chapter](../body/01-orthotropic-material-audit.md), including its
   USDA Wood Handbook Chapter 5, “Elastic Properties,” equation 5–1 reference.
   Search the exact DOI with `Table 1`, `erratum`, `orthotropic`, and `Poisson`
   for an explicit correction; absence of a found correction does not resolve
   the intended values. Handbook relations are methods, not specimen constants.
2. Keep a raw transcription and a separate SI worksheet: GPa × `1e9` gives Pa;
   density is kg/m³; Poisson ratios are dimensionless. Define first Poisson index
   as loading direction, second as contraction, with `L/R/T` axes and engineering
   strain ordering `[epsilon_LL, epsilon_RR, epsilon_TT, gamma_RT, gamma_LT, gamma_LR]`
   with `gamma_ij=2*epsilon_ij`, and corresponding stress ordering. Then `S_ii=1/E_i`,
   `S_ji=-nu_ij/E_i`, and shear diagonals are `1/G_RT`, `1/G_LT`, `1/G_LR`.
   Compute every `nu_ij/E_i - nu_ji/E_j` in Pa⁻¹, its scale-normalized residual,
   and uncertainty or displayed-rounding bounds. Preserve missing shear values
   as null; the printed normal block cannot establish a complete 3D law.
3. A second analyst recomputes the mulberry LR pair and all other reported
   reciprocal pairs independently. The printed mulberry pair's opposite signs
   with positive moduli contradict reciprocity; a negative Poisson ratio alone
   is not the rejection rule. Check finite positive density/moduli, then symmetry,
   then positive principal minors and a scaled Cholesky/eigenvalue diagnostic for
   a complete candidate. Retain the smallest normalized eigenvalue, conditioning,
   uncertainty sensitivity and grain-frame definition. Derive tolerances from
   arithmetic precision and the documented uncertainty; do not reuse a synthetic
   unit-test tolerance as a laboratory error bar. Never symmetrize, swap axes,
   clip eigenvalues or infer missing shear moduli to make the source pass.
4. Retain raw/SI tables, residual tables, missing-field report and a quarantine
   disposition linked to the source ledger. An admissible replacement needs an
   author clarification with actual input conventions, authorized independent
   measurements, or an explicitly synthetic prior. Any corrected candidate gets
   a new record and the [promotion form](../../templates/parameter-promotion.md).
   A modal fit may identify effective stiffness while leaving thickness, joints
   and support confounded; agreement with frequencies does not clear the tensor.

The present [body_model.cpp](../../src/instrument/body_model.cpp) exposes
`reciprocal_poisson_pair`, `plane_stress_compliance`, and `flexural_rigidity` for
the two-direction `OrthotropicPlate`; it is not a 3D source importer. Existing
`materials.reciprocity` and `materials.positive_energy` in
[body_tests.cpp](../../tests/instrument/body_tests.cpp) exercise synthetic plate
constants and negative cases. Full published-tensor, shear, rotated-frame and
uncertainty tests remain planned. Use an admissible synthetic tensor plus
nonsymmetric/indefinite counterexamples for that future importer work; retain the
current synthetic body. Hand the audit and unresolved measurements to P07-T03
and P08's constitutive-model selection, with rejected values barred from assets.

## P01-T03: select a specimen and acquisition strategy

Decide whether to seek the original research specimen/data, characterize a new
instrument, or maintain a research-only synthetic model until acquisition is
possible. Record the decision and its effect on claims. Combining one maker's
geometry with another specimen's damping and a third player's recordings creates
a composite model, not an exact reconstruction of any of them.

Assign an immutable specimen identifier before measurements. Record maker/owner
information only with the appropriate permission. Attach strings, bridge,
frets, repairs, finish, and setup as versioned configuration records. Changing
strings or moving a bridge changes the measurement state; it is not a new file
name for the same experiment. Use the measurement protocols to distinguish
noninvasive observations from modifications that need additional permission.

### Execution recipe

**Question and mapping.** Which accessible physical state can anchor the intended
sound and response claim? Under `ENG-P01-T02`, `REQ-GOV-002` and `REQ-GEO-001`,
write the selection decision before interpreting a good recording as evidence of
an identified instrument. Use [reference selection](../measurement/reference-instrument.md)
and the [acquisition campaign](../measurement/13-acquisition-campaign-execution.md).

1. Compare three routes in a decision record: obtain the original research
   specimen/assets with matching experimental state; characterize a newly
   authorized specimen; or keep a research-only synthetic configuration. Search
   `"Numerical and Experimental Modal Analysis of the Setar"` in the author
   laboratory index and `"setar" bridge mobility` to locate the original methods.
   Read `R-SETAR-MANSOUR-2011` §3 and `R-SETAR-MANSOUR-SCAVONE-2012`
   §3.1–§3.4/§4.1–§4.2 through the atlas links. Their support/loading comparisons
   define questions for a project pilot; they do not establish access to either
   instrument. Use `R-SETAR-TABASSIAN` pp. 1–2 and
   `R-FRETTING-POORHAYDARI-2023` paragraphs [7]–[8] to identify setup questions,
   separating player observations from proposed scales.
2. For each candidate, record intended claim, access evidence, permitted handling,
   condition, configuration repeatability and measurement coverage. A real route
   needs an opaque specimen ID, owner authorization reference and an initial
   condition record; missing evidence remains null. Plan calibrated nut/bridge/tail
   landmarks, string identity and mass-per-length route, fret/action positions,
   support and sensor state, tuning before/after trials, temperature and humidity.
   Specify device calibration, repeat/reposition operations and uncertainty rather
   than assigning an unsupported universal repeat count. Do not infer hidden
   thickness from an exterior photograph.
3. Define a right-handed specimen frame and explicit transforms into each string's
   runtime frame: nut at `x=0`, bridge at `bridge_position_m`, tail at
   `total_length_m`. Record longitudinal distances in m and sensor directions as
   unit vectors. The specimen frame may run in a different longitudinal direction;
   its transform must make that visible. Version string replacement, fret/bridge
   movement, retuning, damping, support and attachment changes. A clamped,
   string-muted test is a distinct state from playing support.
4. Populate a **separate proposed** `data/specimens/<specimen-id>/measurement-plan.json`
   and `experiments/specimen-campaign.md` from the
   [blank specimen](../../data/specimen-template.json) and
   [campaign](../../data/measurement-campaign-template.json) templates; leave the
   public originals blank. Record a noninvasive calibration/repeatability pilot,
   excluded interventions and the safe stop conditions in the selection protocol.
   No contact, paid access, invasive measurement or participant session follows
   merely from this plan.

An independent reviewer must reconstruct the candidate/setup and explain each
unknown model input using the [completeness matrix](../measurement/specimen-completeness-matrix.md).
Reject a composite asset presented as one specimen; block acquisition without
authorization or on evidence of instrument distress. If no specimen is available,
accept only the synthetic-work decision. Planned `data.specimen_identity` and
`data.geometry_registration` are not current CTests; manifest integrity is only
partially checked. The handoff to `ENG-P07-T01` supplies frames and geometry
demands; P04/P06 receive string/fret demands. Later accepted measurements may
replace the corresponding `StringConfig`/`FretConfig` fields, while the existing
synthetic preset remains a regression fixture and unrelated body/contact fields
retain their own evidence labels.

## P01-T04: request useful research files

Prepare a specific request listing raw point clouds, coordinate system, interior
thickness information, editable CAD/mesh, material axes, mesh element definitions,
boundary constraints, preload state, raw force/response recordings, sensor
calibration, fitted modes/residues, and redistribution terms. Ask for what exists
without implying the authors are obliged to provide it. A processed image is
useful background but does not replace those numerical inputs.

Do not send messages automatically from this repository. An owner-authorized
person may contact the researchers. Log the technical outcome without publishing
private correspondence. If materials are offered only for private research,
separate access controls from the public repository and ensure CI can still
exercise synthetic fixtures. Never commit restricted data to make a test green.

### Execution recipe

**Question and mapping.** What minimum missing file package would make a source's
body or geometry result reproducible and usable for a physical-model comparison?
The crosswalk maps this task to `ENG-P01-T01` and `ENG-P01-T02`, covering
`REQ-GOV-002` and `REQ-GEO-001`. The output here is an unsent technical draft and
a gap table, not evidence that any researcher replied.

1. Start from `R-SETAR-PEDRAMMEHR-2018` §3.1–§3.2 and §4–§5 and the
   `R-SETAR-MANSOUR-2015` DOI lead in the [atlas](../research/source-atlas.md).
   Search each exact title/DOI with `supplementary`, `dataset`, `mesh` and
   `repository` at the publisher and author laboratory. Log actual asset access
   and terms. The 2015 full text remains uninspected in the existing record;
   request an authorized copy before attributing its detailed method. Read
   [FRF/modal acquisition](../measurement/frf-and-modal-protocol.md) and
   [mode normalization](../body/05-eigenmodes-normalization-and-tracking.md)
   to turn a vague “send data” request into the following scope.
2. Draft: “For the identified paper and experiment, please indicate which of the
   following files already exist and may be shared for the stated analysis.”
   Specify the exact specimen, setup, support and string/preload state. Request
   original scaled point clouds with landmarks and frame transforms; accessible
   interior/thickness observations and occlusion masks; editable CAD or node and
   element connectivity with units, element formulation, material/grain frames,
   joints, constraints and preload; and the actual material inputs with Poisson
   convention, missing shear constants, provenance and any corrections.
3. Request untouched force and response channels with channel map, digital-count
   or voltage units, sample rate/count, clock/trigger and delay information,
   sensor calibration including phase, orientations/positions, support/attachment
   details, repeats and rejected impacts. Ask for complex FRFs with frequency
   grid, units, estimator/window/averaging settings and coherence; fitted poles,
   damping convention, complex residues, modal mass/normalization and uncertainty;
   and processing scripts/version/arguments. Ask which original files may be
   retained, analyzed, shared, and used for derived parameter/public releases.
   Article permission and raw-asset permission require separate answers. Accept
   that requested files may never have been produced.
4. Keep the draft and an available/requested/unavailable/rights-unresolved table
   in the **proposed** `experiments/specimen-campaign.md` handoff, with private
   correspondence excluded. If authorized materials later arrive, hash originals,
   inspect contents against the request, and use P01-T06 ingestion before analysis.
   Do not enter plausible missing values or equate a mesh image with mesh topology.

Have an independent analyst check whether the requested inputs could reconstruct
`M*(eta''+2*zeta*Omega*eta'+Omega²*eta)=sum(psi*F)` and its bridge ports in
[`BodyMode`](../../include/setar/instrument/config.hpp). Missing mass/residue,
direction, phase or normalization blocks a measured coupling asset even when
frequencies are available; unclear rights block the affected use. Record partial
utility separately, such as a geometry comparison with unresolved material data.
Current `data.acquisition_manifest_integrity` can verify declared files and hashes,
not source completeness, licensing or FRF physics; `data.provenance_rejection`
remains only partially covered. Hand geometry to `ENG-P07-T01`, material queries
to `ENG-P07-T02`, and calibrated FRF demands to `ENG-P08-T02`/`ENG-P08-T03`.
If blocked, continue synthetic body/port verification without importing the gaps.

## P01-T05: design independent validation before fitting

Create separate calibration and validation partitions before running parameter
optimization. Consider partitions by string, fret, gesture, dynamic range,
player, support condition, and specimen, depending on the intended claim.
Randomly splitting adjacent samples from one note is not meaningful independence.
Keep a record of every dataset used to select parameters or model structure,
including informal listening during development.

Choose observables that identify different mechanisms. String partial spacing
can constrain stiffness; amplitude decay can constrain losses; bridge mobility
can constrain body coupling; radiation observations constrain output mapping.
One microphone recording rarely identifies all these uniquely. Design controlled
experiments that reduce ambiguity rather than compensate it with extra knobs.

### Execution recipe

**Question and mapping.** Which unseen physical condition would distinguish a
predictive parameter/model choice from compensation between excitation, strings,
body and observation? The exact mapping is `ENG-P01-T02` plus `ENG-P12-T02`:
`REQ-GOV-002`, `REQ-GEO-001` and `REQ-IDN-002`. Sensitivity work is handed to
`ENG-P12-T01` / `REQ-IDN-001` rather than presumed complete by creating a split.

1. Read [independent validation](../measurement/independent-validation.md) and
   [identifiability/calibration](../validation/05-identifiability-and-calibration.md).
   Use `R-METROLOGY-JCGM-100` §4–§5 and §7 for measurement equations, correlation
   and reporting. Search `profile likelihood practical identifiability experimental
   design dynamical models` to locate `R-IDN-RAUE-2009`; the atlas records only
   abstract/§2.3 excerpt access, so obtain the actual method before claiming its
   implementation. These sources guide inference; neither supplies setar data
   nor a universal holdout percentage.
2. Define the generalization claim and measurands before fitting: partial
   frequencies in Hz/cents across declared windows; amplitude decay in s⁻¹;
   complex bridge mobility in `(m/s)/N`; pressure in Pa at calibrated positions;
   and contact timing in s where observed. Freeze tuning, support, amplitude,
   sensor/microphone calibration, processing windows, bandwidth and noise-floor
   rules or explicitly identify them as controlled variations/nuisance variables.
   For example, new tuning/stopped length challenges stiffness versus termination,
   a withheld bridge port challenges modal residues, and a withheld microphone
   direction challenges observation. A same-specimen test cannot establish
   unseen-specimen generalization.
3. Build a raw-asset ancestry table. Group every event, segment, repeated transform
   and derivative together; then group by the relevant specimen/configuration,
   session, player, gesture family and acquisition condition. Assign whole groups
   to calibration, development and independent test. Record overlap constraints,
   rationale, allocation seed if randomized, planned coverage, excluded groups and
   access history in **proposed** `data/splits/<split-id>.json`. Do not populate
   nonexistent acquisition IDs or hashes. With no data, deliver a proposed group
   allocation policy with empty memberships and an explicit blocked freeze.
4. Before optimization, have a reviewer independently follow hashes/ancestry
   across partitions, freeze the model/metric selection process, and check that
   scaling, feature selection, early stopping and informal listening use only
   permitted development data. Plan a sentinel test: changing a held-out datum
   must not change fitted parameters, normalization or stopping. If test evidence
   later motivates a change, preserve the failed result, reclassify it as
   development and reserve new evidence for the revised claim.

Retain the split manifest, mechanism/observable table, calibration covariance,
preregistered tolerances and their rationale, leakage review and later residual
plots by group. Too few independent groups or uncertainty larger than a justified
decision margin yields inconclusive/blocked validation, not a relaxed threshold.
Rejected leakage cannot be repaired by renaming files. Accepted predictions may
support `StringLossLaw`, `BodyMode` residues or an observation change through the
promotion record; split creation alone changes none of them. In particular, keep
modal damping ratio distinct from string amplitude-loss coefficients and keep
microphone transfer from absorbing mechanical loss. The current string law is
`sigma_n=a0+a2*k_n^2+b2*omega_n^2`, with coefficient units s⁻¹, m²/s and s;
`BodyMode.damping_ratio` is dimensionless. Planned
`identify.heldout_reproduction` and `identify.leakage_guard` have no current CTest
mapping; deterministic synthetic replay proves no empirical independence. Hand
the frozen policy/data, when available, to P12 and the later P17 study designer.

## P01-T06: prepare data contracts

Use a manifest with units, coordinate axes, sample rate, clock information,
sensor orientation, calibration factors, uncertainty, configuration identifier,
permissions, source version, and raw-data hashes. Preserve raw measurements and
derived processing as separate immutable records. Include sign conventions for
force and velocity, because a reversed channel can invalidate a passivity check.

Reject missing or ambiguous units at the import boundary. Test wrong sample rate,
inconsistent channel lengths, clipped force impulses, invalid hashes, duplicate
identifiers, and mismatched specimen configuration. A warning that is easy to
ignore is inappropriate when the missing field changes the physical meaning.
Provide a synthetic example manifest that exercises the schema without imitating
real participant data.

### Execution recipe

**Question and mapping.** Can another analyst reconstruct a measured quantity's
physical meaning and every transformation before it reaches a prepared model?
The crosswalk maps this task to `ENG-P02-T02` and `ENG-P01-T02`, carrying
`REQ-LAB-001`, `REQ-LAB-002`, `REQ-GOV-002` and `REQ-GEO-001`. A sign, clock or
unit error can produce false pitch, negative apparent port resistance or an
incorrect pressure level while the file itself remains readable.

1. Read the [data contract](../measurement/data-contracts-and-provenance.md),
   [campaign mechanics](../measurement/13-acquisition-campaign-execution.md)
   and [uncertainty procedure](../measurement/uncertainty-and-calibration.md).
   Use `R-METROLOGY-JCGM-100` §4, §5 and §7 through its official atlas links.
   For each sensor, find the manufacturer's exact model/firmware calibration
   procedure by model number plus `sensitivity phase calibration`; record the
   inspected version. A generic uncertainty method supplies no device accuracy.
2. Preserve the distinction between
   [measurement-campaign-template.json](../../data/measurement-campaign-template.json)
   (17 observation packets) and
   [specimen-template.json](../../data/specimen-template.json)
   (candidate parameter provenance). In an authorized dataset, create a separate
   campaign only when observations exist. Enter actual global specimen/session,
   frame, permission, calibration and split IDs; add relative raw-file paths with
   computed lowercase SHA-256. Keep `missing`, `acquired_unverified`, `review_ready`
   and `rejected` distinct. Review-ready channels need protocol, calibration,
   uncertainty, conditions, permission and reviewer references; an ID string alone
   does not prove any of those facts.
3. Attach per-trial/channel sidecars for configuration/setup identity, raw counts
   or volts, SI conversion and offset, phase/delay correction, sample rate/count,
   timestamp clock, orientation/location/frame, force/velocity signs, clipping and
   dropout flags, uncertainty/covariance, rights and source version. Record every
   transform's input/output hashes, code revision, arguments and quality decisions.
   Keep raw files immutable and retain failed trials/reasons as permitted. Derived
   FRFs need real/imaginary components, frequency units and estimator conventions;
   `SI_mixed` or `SI_tensor` in the campaign is not a per-field unit definition.
4. Run the existing linter from the repository root:

   ```sh
   python3 tools/validate_measurement_campaign.py data/measurement-campaign-template.json
   python3 tools/test_evidence_gates.py
   ```

   The blank template must remain 17 channels, zero acquired and zero review-ready;
   `--require-ready` must reject it. For a real authorized campaign, run the same
   validator on its private manifest; require-ready is a later completeness gate,
   not a way to bypass missing acquisition. Inspect
   [validate_measurement_campaign.py](../../tools/validate_measurement_campaign.py)
   and [test_evidence_gates.py](../../tools/test_evidence_gates.py): they enforce
   expected channel units/IDs, referenced-file hashes and containment, status and
   required references. They do not parse sample arrays, establish calibration
   truth, detect clipping, validate tensor physics or prove partition independence.

Specify those missing semantic checks for **proposed** `data/evidence.schema.json`,
`tools/validate_evidence.py` and `tests/evidence/`, the `ENG-P02-T02` destinations.
An independent analyst should reproduce one synthetic force/velocity conversion
and predict rejection of a time axis inconsistent with sample rate, unequal channel
lengths, duplicate trial identity, undeclared polarity/orientation reversal,
clipping, invalid hash, missing calibration
and configuration mismatch. Separate checks already executed by the present
`data.acquisition_manifest_integrity` CTest from planned extensions to
`data.provenance_rejection`, `verification.oracle_independence` and
`ci.matrix_coverage`; do not claim the latter requirements from schema success.

Retain ingest reports, negative-case expectations and a field-to-model map:
speaking length maps to `StringConfig.bridge_position_m`, afterlength requires
`total_length_m`, calibrated modal normalization feeds `BodyMode`, and pressure
calibration feeds observation. The current `Provenance` enum and
[`validate(InstrumentConfig)`](../../src/instrument/config.cpp) cannot carry or
verify this lineage. Use a versioned sidecar before any future import; reject or
quarantine ambiguous physical meaning. Hand the contract to P02 evidence tooling,
P07 acquisition and P12 fitting with the semantic gaps explicitly open.

## Outputs, review, and handoff

The output is a source ledger, parameter quarantine list, acquisition decision,
measurement plan, schema examples, and held-out validation design. Independent
review should reconstruct one accepted fact and one rejected parameter from the
original sources. The reason for rejection must be technical and reproducible,
not simply that the value differs from a preferred answer.

> Audit the supplied source or dataset under P01. Report access and license
> status, exact claims supported, units and conventions, internal consistency
> checks, missing metadata, and whether it can enter calibration or validation.
> Do not invent missing values or copy copyrighted material into the repository.
> If unusable, propose the smallest measurement or clarification that resolves
> the blocking uncertainty. Keep the original record unchanged.

P02 may proceed with synthetic fixtures. P07 may proceed when specimen access and
geometry/material permissions exist. Neither inherits a claim of exact setar
reproduction merely because the source audit is complete.

Before handing off P01, a reviewer should be able to check all of the following:

- Every decisive claim has an exact inspected source location or an explicit
  access blocker, a separate rights decision, a model consequence and an owner.
- The published material contradiction is reproducible without modifying the
  transcription; any replacement has its own provenance and admissibility scope.
- The selected acquisition route names the physical state and permissible pilot,
  or records that specimen-dependent gates remain blocked while synthetic work
  continues. Draft requests are identified as unsent.
- The split policy identifies the intended independent experimental unit and
  every leakage route; empty memberships are not called a frozen dataset.
- The two existing templates remain visibly blank, proposed schema/import paths
  are labeled proposed, and the difference between structural validator success
  and scientific acceptance is explicit.
- Each finding ends accepted for scope, rejected, inconclusive or blocked with
  retained reasons, an independent review method, and the next ENG work order.
  No source audit alone admits a new measured preset or closes fidelity gates.
