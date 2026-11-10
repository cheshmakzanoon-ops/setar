# P07 — Acquire defensible geometry and material descriptions

**Status:** specification. **Prerequisites:** P01; measurement infrastructure from
P02 supports the work. **Exit evidence:** a traceable geometric/material model
with uncertainty, usable boundary definitions, and no quarantined values hidden
inside a simulation input deck.

## The specimen is part of the model

The instrument class alone does not define a reproducible finite-element model.
Wall thickness, wood directions, joints, moisture, repairs, string loading, and
support conditions can change a response. This phase turns a named physical
specimen into a versioned set of observations and modeling assumptions. When a
specimen is unavailable, the output must remain a synthetic design study.

Read [geometry](../geometry/README.md), [body](../body/README.md), and
[measurement](../measurement/README.md). Consult the source audit before using
published dimensions or elastic constants. In particular, a paper's material
table is not exempt from reciprocity and positivity checks.

## Research map and entry conditions

Use the [source atlas](../research/source-atlas.md) for access status and the
[research-to-model workflow](../research/research-to-model-workflow.md) for finding
records. The following are acquisition and analysis instructions, not newly
acquired specimen evidence.

| Decision | Primary route and exact reading target | Transfer limit |
| --- | --- | --- |
| Exterior, interior and material provenance | `R-SETAR-PEDRAMMEHR-2018`, [publisher article](https://www.extrica.com/article/19505), §2, §3.1 and §3.2/Table 1 | Describes a study; does not release this project's specimen geometry or justify importing its contradictory material table. |
| Loaded geometry and measurement boundaries | `R-SETAR-MANSOUR-2011`, [author paper](https://caml.music.mcgill.ca/lib/exe/fetch.php?media=publications:caa2011_mansour.pdf), §3; `R-SETAR-MANSOUR-SCAVONE-2012`, [author paper](https://caml.music.mcgill.ca/lib/exe/fetch.php?media=publications:nantes2012_mansour.pdf), §3.1–§3.4 and §4.1–§4.2 | Apparatus and support comparisons motivate a specimen-specific pilot; they supply no universal support correction. |
| Wood directions and elasticity conventions | [USDA Wood Handbook](https://research.fs.usda.gov/download/treesearch/62244.pdf), Chapter 5, “Orthotropic Nature of Wood” and “Elastic Properties”; [published-parameter audit](../research/published-parameter-audit.md) | Definitions and priors do not identify wood species, moisture state or constants on an intact setar. |
| Correlated measurement uncertainty | `R-METROLOGY-JCGM-100`, [official record](https://www.bipm.org/en/doi/10.59161/jcgm100-2008e), §§4–5 and §7 | Supplies an uncertainty method; tolerances must follow this task's observable and calibration budget. |

Before acquisition, require a P01 specimen/configuration identity, authorized
operations, calibrated instruments actually available, a raw-record destination,
and intended P08/P09/P10 observables and bands. Before numerical work, a declared
synthetic package is sufficient. Keep a separate status for each route; missing
interior access blocks a measured interior claim, not a synthetic mesh exercise.
Do not contact participants, buy access or alter an instrument through this plan.

The current [body builder](../../src/instrument/body_model.cpp) consumes
`BodyDesign`/`OrthotropicPlate` from [body_model.hpp](../../include/setar/instrument/body_model.hpp),
not CAD, thickness fields or full three-dimensional tensors. Its rectangle,
constant grain, rigid bowl and synthetic coefficients remain regression fixtures.
The specimen files and general material importer named below are **proposed**.
For every recipe retain an accepted, rejected, inconclusive or blocked disposition,
with evidence and reasons; acceptance always states its limited domain.

## P07-T01: define the acquisition contract

Identify the instrument, owner permissions, condition, setup, strings, tuning,
temperature, humidity, and whether measurements are loaded or unloaded. State
which interventions are allowed. Removing a bridge, applying an accelerometer,
or using an internal probe can change the object or its response; permission
and a risk review precede those operations.

Specify the coordinate frame using physical landmarks that can be recovered in
future sessions. Record transformation conventions for scanner, photograph,
microphone, and simulation coordinates. Include a scale reference and estimate
registration uncertainty. A mesh with many triangles is not precise merely
because its file is large.

Choose methods by the unknown they resolve. External photogrammetry can recover
visible shape but does not directly measure hidden wall thickness or wood
moduli. Internal thickness observations need a suitable non-destructive method
and uncertainty assessment. Do not infer an inaccessible interior by mirroring
the outside and then label that interior measured.

### Execution recipe

**Question and research.** Can another operator recover the geometry and loaded
state that determine bridge alignment, bending stiffness and cavity volume?
Under `ENG-P07-T01` / `REQ-GEO-001`, read
[Geometry 01](../geometry/01-specimen-and-coordinate-contract.md) and
[Geometry 02](../geometry/02-surface-interior-thickness-and-grain.md), then extract
acquisition and missing-interior assumptions from `R-SETAR-PEDRAMMEHR-2018` §3.1.
Search `"setar" soundboard thickness`, `"سه تار" ضخامت صفحه` and the actual
available gauge's calibration guide. Record whether each candidate method measures
surface location, local-normal thickness or only a visible edge. The paper's
scanning method is a lead, not a dimensional input.

1. Freeze specimen, strings, tuning/preload, bridge seating, support, attached
   sensors and environment. List permitted observations and unobservable regions
   before taking data. Use existing authorized records if access is blocked.
2. Establish the right-handed specimen frame from named landmarks: bridge origin,
   positive longitudinal axis toward the tail, positive normal toward the strings,
   and the completing transverse axis. Store each acquisition-to-specimen rigid
   transform, metres, matrix/vector convention and landmark uncertainty. Keep the
   current plate-corner frame and nut-origin string coordinate as separate frames.
3. Acquire scale references and independent accessible lengths, then repeat a
   subset without reusing the first registration. Choose a permitted thickness
   method only after checking curvature, direction, calibration and uncertainty;
   unobserved interior thickness remains unknown or bounded inference. Do not use
   photographic resolution as dimensional accuracy.
4. Reserve landmarks and dimensions for checking, separate scale error from
   registration error, and estimate correlated uncertainty using
   `R-METROLOGY-JCGM-100` §§4–5. Preregister tolerances from the effect of geometry
   error on the intended mobility/contact observable, not a scanner brochure.

**Retention and translation.** Deliver raw calibration/observation hashes,
permission references, frame drawings, coverage maps, repeated and withheld
residuals, and a proposed `data/specimens/<specimen-id>/geometry.json` manifest
plus `experiments/geometry-registration.md`. An eventual adapter maps actual
contact distances to `StringConfig`: `bridge_position_m` is nut-to-bridge speaking
length; `total_length_m` is nut-to-tail. No measured surface directly populates
`BodyMode`, and `BodyDesign.bridge_point_m` requires its own frame conversion.
Map `Polarization::Vertical` away from the soundboard and `Horizontal` toward
physical string index 3 explicitly; the latter need not equal the specimen's
completing transverse axis.

**Independent gate.** A second reader must recover scale and withheld landmarks
from raw records and reproduce force/displacement work after frame transformation.
Mirrored frames, millimetre/metre mistakes, or unexplained loaded/unloaded mixing
reject promotion; unavailable interior observations produce a bounded gap.
`data.geometry_registration` and `data.specimen_identity` are planned acceptance
IDs, not present specimen CTests. Hand the coverage and uncertainty contract to
P07-T02 and the measurement campaign; synthetic registration fixtures may proceed.

## P07-T02: preserve raw and derived geometry

Keep raw acquisition records immutable, subject to consent and storage rights.
Derived surfaces carry their registration, smoothing, hole repair, decimation,
and thickness-assignment history. Store transformation scripts and parameters
where possible. Human edits need an explicit operation log and visual diff.

Check for self-intersections, reversed normals, disconnected fragments, open
boundaries, and unit errors before meshing. Some openings are intentional acoustic
ports, not defects to close automatically. Mark semantic regions such as bridge
footprint, soundboard, bowl, neck joint, cavity openings, and supports. Their
identities must survive a mesh refinement; relying on arbitrary vertex numbers
alone makes experiments brittle.

Separate geometric fidelity from solver suitability. A detailed scan may require
a simplified analysis geometry, especially around cracks, ornament, or very thin
features. Preserve both representations and demonstrate the effect of each
simplification on relevant modes or coupling quantities. Do not erase a feature
because meshing is inconvenient without documenting its likely mechanical role.

### Execution recipe

**Question and research.** Does reconstruction preserve the physical load paths,
openings and bridge footprints closely enough for the chosen structural and
acoustic observations? Use `ENG-P07-T01` / `REQ-GEO-001`,
[Geometry 03](../geometry/03-cad-topology-and-uncertainty.md), and
`R-SETAR-PEDRAMMEHR-2018` §3.1 to distinguish the described CAD workflow from
unavailable raw geometry. Search `mesh reconstruction thickness uncertainty
soundhole topology` and inspect the selected tool's official versioned physical-
group/export documentation; the [Gmsh manual](https://gmsh.info/doc/texinfo/) is
a candidate reference, not an installed dependency or approved import pipeline.

1. Preserve four identities: raw observations, fitted metrology surfaces, analysis
   geometry and display mesh. Rebuild each derived version from input hashes,
   unit conversions, transforms, fitting scales and ordered edits. Retain human
   corrections as explicit before/after regions.
2. Register interior/exterior observations through measured landmarks. Construct
   local-normal thickness only where intersections are unambiguous; compare
   nearest-surface distance diagnostically. Mark extrapolated thickness and
   competing interior reconstructions separately. Avoid choosing registration
   merely to make thickness uniform.
3. Classify every gap before repair: soundhole, crack, seam, unobserved patch or
   numerical defect. Check signed volume, duplicate/overlapping regions, normals,
   self-intersection, positive thickness and intended air connectivity. Preserve
   semantic bridge, rim, neck-joint and support regions through remeshing.
4. Hold material and boundary hypotheses fixed while comparing one simplification
   at a time. Compare area/volume, mass prediction and bridge-port response; use
   reserved observations to assess reconstruction error. A future converged
   P08 calculation can bound a simplification's mechanical effect, while geometric
   checks can proceed now. Do not relabel an uncomputed effect negligible.

**Retention and translation.** Store a transformation graph, topology report,
observed/inferred overlays with scale, region areas/volumes, uncertainty fields,
and accepted/rejected alternatives in the proposed specimen package and geometry
experiment. The current `build_body()` has no surface importer and does not remove
the equivalent opening from plate stiffness or mass; changing
`HelmholtzCavity.opening_area_m2` is not a geometric-hole validation. Preserve that
limitation when handing neutral geometry to proposed `src/body/offline_fem/`.

**Independent gate.** Reimport through an independent inspection path, recalculate
volume and withheld dimensions, and verify that a known open/blocked synthetic
aperture and an asymmetric scale fixture remain distinguishable. Reject silent
hole sealing, lost groups, unsupported element conversion or unexplained mass
changes. The planned `data.geometry_registration` / `data.specimen_identity`
checks need these fixtures; `body.plate_frequency_oracle` presently checks a
synthetic rectangular model and cannot certify them. Deliver stable regions to
P07-T04, thickness/volume uncertainty to P07-T05, and analysis geometry to P08.

## P07-T03: construct admissible constitutive data

Define the orthotropic axes from observed wood orientation, not global mesh
coordinates. State the convention for each Poisson ratio, engineering shear
strain, stiffness/compliance matrix ordering, and tensor rotation. The convention
must travel with the data file so another solver can interpret it correctly.

Check reciprocal compliance and positive definiteness for every proposed elastic
parameter set. Preserve the original published entries even when they fail;
store a quarantined record rather than silently symmetrizing or replacing them.
If an admissible approximation is introduced for sensitivity analysis, mark it
synthetic or inferred and state the optimization or projection used to obtain
it. Numerical admissibility does not prove that the approximation describes
the actual wood.

Estimate density and elastic behavior through non-destructive characterization
or justified literature priors with uncertainty. Do not claim nine independent
orthotropic constants are known when only a few resonant frequencies have been
measured. Check identifiability and use a reduced material description when the
data cannot support the full parameterization.

### Execution recipe

**Question and research.** Which elastic description can store positive energy
and is actually supported by observations, without making fitted tone depend on
an impossible tensor? Work under `ENG-P07-T02` / `REQ-GEO-002`. Read
[Body 01](../body/01-orthotropic-material-audit.md), the
[published audit](../research/published-parameter-audit.md), and
`R-SETAR-PEDRAMMEHR-2018` §3.2/Table 1 alongside the USDA Chapter 5 definitions
in the research map. Search `orthotropic wood dynamic elastic constants moisture
Poisson reciprocity` and follow decisive values to their original experiment.
The 2015 Mansour article remains an access-limited lead in the atlas; do not
attribute unread material methods to it.

1. Extract each published or measured quantity unchanged, with source section,
   specimen/species evidence, moisture/temperature, frequency, method and interval.
   Seek permitted nondestructive characterization or existing associated witness-
   wood records; never prescribe cutting a specimen. Keep missing shear constants
   null rather than filling them using an isotropic relation.
2. Define `L,R,T` from grain observations, a proper orthonormal frame, and
   `nu_ij = -epsilon_j/epsilon_i` under loading in `i`. Declare stress ordering
   and engineering shear factors. Convert moduli to Pa, density to kg/m³ and
   compliance to Pa⁻¹. Check `nu_ij/E_i = nu_ji/E_j` before any symmetrization.
3. Preserve contradictory printed pairs as quarantined negative fixtures. An
   admissible inferred alternative gets a new identity, an explicit constrained
   fit/projection and the change from the original. Then test scaled symmetry,
   positive density, full positive definiteness and conditioning across its
   uncertainty domain. Derive plane-stress elimination consistently if choosing a
   shell; passing a two-dimensional check does not certify a full wood tensor.
4. Rotate a deliberately anisotropic coupon through known and general proper
   rotations. Independently compare uniaxial/shear strains, complementary energy
   and reciprocal load work. Estimate which material combinations are identifiable
   before fitting; few modal frequencies do not determine nine constants.

**Retention and translation.** Keep raw extraction, SI conversion, tensor/frame
definitions, rejected records, eigenvalue/conditioning tables, coupon plots and
uncertainty bounds. Present functions `reciprocal_poisson_pair()`,
`plane_stress_compliance()` and `flexural_rigidity()` in
[body_model.cpp](../../src/instrument/body_model.cpp) cover only the synthetic
plate's in-plane law. Proposed `src/assets/material_validation.cpp` and
`tests/assets/material_tests.cpp` must cover full tensors, grain fields and
quarantine ingestion before any specimen material asset is accepted.

**Independent gate.** Use direct compliance calculations independent of the
implementation, a rotated anisotropic fixture, and the published contradictory
pair. Reject nonfinite/incomplete inputs, reciprocal inconsistency, indefinite
energy or unsupported correction; an unidentifiable fit remains inconclusive.
Present `materials.reciprocity` and `materials.positive_energy` in
[body_tests.cpp](../../tests/instrument/body_tests.cpp) exercise the synthetic
plane-stress subset only. Hand admissible alternatives and unresolved quantities
to P07-T05/P08, leaving specimen fidelity open even when algebraic checks pass.

## P07-T04: represent spatial variation and assembly

Decide whether thickness, density, grain direction, and damping are constant by
region or spatially varying. Additional fields require observations or a clear
uncertainty model. An unconstrained per-element fit can reproduce measured modes
while having no predictive value. Prefer a small number of interpretable
parameters before introducing fine spatial flexibility.

Document joints, adhesives, contact areas, bridge seating, and neck attachment.
Classify each as bonded, compliant, frictional, or unresolved. State how a joint
approximation changes the possible motion. A perfectly tied interface imposes
more information than an uncertain physical joint; its use must be deliberate.

Include prestress and assembly sequence where they matter. String tension can
alter geometry, contact, and tangent stiffness. A linear model about the loaded
equilibrium is not the same as a linear model about an unstressed scan. Keep the
equilibrium solve and the small-vibration analysis distinct and verifiable.

### Execution recipe

**Question and research.** Does observed variation or joint compliance alter the
force path and bridge response enough to require more than one uniform plate?
Use `ENG-P07-T01`, `ENG-P07-T02`, `REQ-GEO-001` and `REQ-GEO-002`. Read
[Geometry 02](../geometry/02-surface-interior-thickness-and-grain.md),
[Geometry 03](../geometry/03-cad-topology-and-uncertainty.md), and
[Body 02](../body/02-joints-finish-and-preload.md). Extract component/assembly
assumptions from `R-SETAR-PEDRAMMEHR-2018` §2–§3 and loaded-state distinctions
from `R-SETAR-MANSOUR-2011` §3. Search `setar soundboard rim neck joint compliance`
and `wood grain orientation shell stiffness`; retain uncertain construction as
alternative hypotheses, not facts inferred from a generic illustration.

1. Build a region/interface graph from authorized images, thickness observations
   and assembly records. Separate wood pieces, finish, glue, bridge seating and
   supports; record measured area/extent and unobserved interfaces. A CAD union
   must not silently change a compliant joint into continuous material.
2. Start with regional thickness/density and piecewise grain frames. Add gradients
   only where observations or a declared bounded uncertainty model support them.
   Surface grain constrains a tangent direction, not necessarily the radial axis
   or out-of-plane fibre angle. Preserve discontinuities across seams and verify
   the frame at quadrature points after interpolation.
3. Compare bonded, passive compliant and unresolved-contact hypotheses at the
   same geometry, load and support state. For a surface law `t = K_j delta`,
   stiffness is N/m³; a lumped force spring uses N/m. Declare rotational moments
   and angular coordinates separately. Include uncertain finish as an explicit
   layer or bounded omitted effect, not a hidden damping multiplier.
4. Compute candidate static load paths from actual tension vectors and break
   angles, with balancing nut/tail loads. Pass the assembly sequence and contact
   state to P08-T02 for equilibrium and tangent verification. Do not use a modal
   fit to absorb unsupported bridge seating or rim constraints.

**Retention and translation.** Deliver field definitions and correlation lengths,
frame glyphs, interface normals, joint units, mass accounting, hypothesis matrix
and predicted-response comparison plan. `BodyDesign.bridge_mass_kg` currently
adds a lumped point mass; `BridgeConfig` describes the string-to-body spring,
not distributed glue or bridge-foot compliance. Spatial tensors, bowl flexibility,
finish and preload belong to proposed `src/body/offline_fem/` and cannot be
encoded by adjusting the current plate Poisson ratio.

**Independent gate.** Check an anisotropic two-piece fixture, rigid and compliant
joint limits, total force/moment transfer and nonnegative joint energy/dissipation.
Reject invalid frames, double-counted mass, missing load paths or changes caused
only by mesh-dependent interface area. Planned `data.geometry_registration` and
material acceptance checks cover the relevant contracts; existing
`materials.reciprocity` and `materials.positive_energy` CTests do not test this
assembly. Send accepted bounded hypotheses
and inconclusive alternatives to P07-T05 and P08-T01/T02, with P09 port regions.

## P07-T05: propagate uncertainty before fitting

Identify geometry/material variables to perturb and the observations to inspect:
mode frequencies, mode shapes, bridge mobility, radiated response, and contact
alignment. Use controlled sensitivity studies to rank measurement priorities.
If thickness uncertainty dominates a result, refining mesh resolution alone
will not resolve the scientific uncertainty.

Report correlated uncertainties where warranted. Thickness and elastic modulus
can trade off in bending behavior; independently selecting their best-looking
values may understate uncertainty. Do not collapse a plausible parameter family
to one precise number without a reason. Reserve measurements for validation
rather than fitting every available observation.

### Execution recipe

**Question and research.** Which unresolved geometric/material quantity limits
prediction most, and which added observation would separate it from competing
explanations? Under `ENG-P07-T02` / `REQ-GEO-002`, use
[Body 09](../body/09-parameter-identifiability-and-experiment-design.md),
`R-METROLOGY-JCGM-100` §§4–5/§7 and the `R-IDN-RAUE-2009` source card.
The latter's abstract/§2.3 excerpts motivate profiling; acquire authorized full
methods before relying on uninspected details. Search `thickness modulus density
identifiability plate mobility` and `profile likelihood experimental design
mechanical parameters`. Published methods establish no specimen uncertainty.

1. Construct a parameter table with units, admissible bounds, evidence class and
   correlated calibration effects: scale, thickness basis, density, grain angle,
   joint extent/compliance, bridge position and support. Keep specimen variation
   separate from uncertainty about this specimen. Treat topology/contact-set
   changes as separate hypotheses rather than smooth perturbations.
2. Freeze the observation vector and validation split before fitting. Include
   independent mass/thickness, complex driving/cross mobility and registered
   spatial shapes where available. Do not count FRFs and modes extracted from
   those same FRFs as independent measurements without joint covariance.
3. Verify finite-difference sensitivities at two perturbation sizes above solver
   noise; compare isolated-eigenvalue derivatives with
   `phiᵀ(dK - lambda dM)phi` under mass normalization. Track clustered subspaces
   and include derivatives of port position/direction. Weight real and imaginary
   responses by uncertainty rather than comparing raw Pa and metre columns.
4. Inspect singular directions, profiles and admissible ensembles. Run the
   chapter's synthetic thin-plate counterexample: for `a > 0`, scaling thickness
   by `a`, density by `1/a` and in-plane material moduli by `1/a³` preserves
   bending stiffness and areal mass with fixed planform, boundaries and Poisson
   structure under its stated thin-plate assumptions. Then add an
   independent thickness observation and test whether the ambiguity narrows.

**Retention and translation.** Retain input covariance/bounds, random seeds where
used, derivative checks, singular-value/profile plots, failed samples, held-out
predictions and a ranked measurement request. Current `BodyDesign` supports
uniform synthetic sensitivity fixtures; it cannot represent the full spatial
ensemble. Pass accepted material ranges to the proposed material validator and
geometry package; send identifiable effective combinations, not fictitious unique
wood constants, to P08 and P12.

**Independent gate.** Require a separate analyst to recover the constructed
ambiguity and demonstrate which proposed observation changes the prediction
interval. Reject derivative results dominated by remeshing or mode swaps and
claims narrower than calibration/model error. `materials.reciprocity` and
`materials.positive_energy` presently screen synthetic admissibility, not
identifiability; the broader analysis remains planned. If wood inversion remains
inconclusive, a calibrated passive multiport FRF model may progress through
P08-T04/T05 for the measured state/band, with no claim to uniquely identified
wood, hidden geometry or changed-support prediction.

## P07-T06: deliver an analysis-ready package

The package contains coordinate definitions, specimen manifest, geometry
provenance, semantic regions, admissible material alternatives, unresolved
properties, units, and solver-independent checks. Include a visual report with
scale, region labels, and distinction between measured and inferred surfaces.
A photograph is evidence of appearance, not a complete dimensional specification.

Exit only when a second developer can reconstruct the model inputs and identify
every assumption that is not directly supported. P08 may proceed on a synthetic
package to verify its solver, but must retain that label until a specimen-backed
package passes this gate.

### Execution recipe

**Question and research.** Can a second developer rebuild the exact admissible
inputs and distinguish every measured region from a hypothesis before attempting
a sound-producing body replacement? This is `ENG-P07-T01` / `REQ-GEO-001`.
Use [the acquisition packet contract](../measurement/13-acquisition-campaign-execution.md#exactly-what-belongs-in-a-raw-acquisition-packet),
[Geometry 03](../geometry/03-cad-topology-and-uncertainty.md), and the research-map
sources. Revisit `R-SETAR-PEDRAMMEHR-2018` §3.1–§3.2 to check what a paper alone
cannot supply; use `R-METROLOGY-JCGM-100` §7 for reported uncertainty. Search the
chosen neutral format's official unit, topology and physical-group specifications
only to resolve actual export questions, and pin the inspected version.

1. Assemble a versioned proposed `data/specimens/<specimen-id>/geometry.json`
   package with specimen/configuration IDs, rights references, raw hashes,
   transforms, semantic regions, metrology and analysis surfaces, material/frame
   references, support/load states, uncertainty, processing revision and status.
   Unknown values remain explicit. Keep private identity data outside distributable
   assets; include only records whose use is authorized.
2. Add dimensioned views, thickness coverage, grain glyphs, interfaces, openings,
   contact patches and normals. State deformation/display scaling and distinguish
   measured, inferred, published and synthetic content field by field. Include
   rejected geometry/material records without making them selectable inputs.
3. Reconstruct from an empty derived-output directory using the recorded workflow,
   then independently reimport and compare landmarks, mass/volume, groups, normals
   and coordinate round trips. Check port force/displacement work and compare
   one admissible synthetic coupon across the exchange boundary.
4. Publish a consumer table: P08 receives geometry/material alternatives and
   boundary states; P09 receives bridge geometry and signed port frames; P10
   receives cavity/opening geometry and distinct inner/outer surfaces; P12 receives
   uncertainty, unresolved alternatives and the locked acquisition split.

**Model translation.** Include a current-versus-proposed mapping: `BodyDesign`
contains only a uniform analytic surrogate, `BodyConfig` contains modal dynamics,
and neither imports this geometric package today. A measured dimension cannot
promote `synthetic_setar_body_design()` or the whole `Provenance` enum to measured.
An eventual asset adapter must preserve the sidecar's mixed provenance and P08
normalization contract; quarantine must survive conversion.

**Independent gate and exit checklist.** A reviewer who did not reconstruct the
geometry must reproduce units/scale, locate each physical port, explain hidden
interior assumptions, rerun admissibility checks and identify the remaining
measurement request. Record accepted/rejected/inconclusive/blocked files and
the reasons in `experiments/geometry-registration.md`. The required
`data.geometry_registration` and `data.specimen_identity` acceptance IDs remain
planned; existing body CTests cannot close this handoff. Missing provenance,
lost topology or unsupported constants stop specimen promotion. A complete
synthetic package can exit into P08 numerical verification with its label and
scientific gate explicitly open.

> Build an analysis-ready geometry/material package from the authorized records.
> Preserve raw evidence and coordinate transforms. Audit units, topology,
> orthotropic conventions, reciprocity, and positive definiteness. Identify
> hidden dimensions and unsupported constants instead of inventing them.
> Produce a sensitivity-driven measurement request and a clearly labeled
> synthetic alternative for numerical work that must proceed without the data.
