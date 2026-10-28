# Geometry, thickness, and coordinate registration

## Measurement objective

The geometry task must produce a model whose uncertainty is known where it affects
mechanics. A visually convincing outer scan is not enough. Thin-structure bending
depends strongly on thickness, local curvature, joints, and material orientation.
The internal bowl, soundboard underside, bridge contact, nut contact, and fret
profiles may remain partly inaccessible. Mark that incompleteness explicitly.

R-SETAR-PEDRAMMEHR-2018 section 3.1 provides a published example of a geometry-to-
structural-model workflow, but no complete corresponding reusable CAD asset was
verified for this repository. The acquisition and quality controls below are
proposed project procedures. They must be adjusted to the authorized specimen and
available metrology without claiming that a particular scanner guarantees the
required accuracy.

## Define geometry quantities before scanning

List the quantities needed by the intended mechanical model: outer surfaces,
internal surfaces or thickness field, bridge footprint, string path, support
locations, neck shape, fret contact geometry, soundhole shape, and joint regions.
For each quantity, record the measurement method, expected coverage, uncertainty
target, and fallback if it cannot be observed safely.

Distinguish geometric detail needed for mechanics from detail needed only for
visual presentation. An ornamental engraving may be visually important and
mechanically negligible for the current band. A subtle thickness gradient can be
visually invisible and mechanically important. Allocate resolution through a
sensitivity argument, not solely by the scanner's maximum point count.

Define whether thickness is measured along a local surface normal, between paired
surfaces, or through a specific instrument axis. These are not identical on curved
regions. A shell model usually requires a midsurface and a thickness assigned
normal to it; a solid model needs a consistent volume. The conversion from the
observed surfaces to either representation is a derived transformation with its
own assumptions.

## Safe acquisition plan

Review any need for surface treatment, targets, internal probes, disassembly, or
contact pressure with the owner and relevant specialist. Do not coat the
instrument, remove its soundboard, or press a thickness gauge through an opening
without explicit approval. Small soundholes do not make internal probing safe by
default. Avoid laser or illumination exposure outside the instrument and operator
safety review.

Perform a reference-object check across the intended measurement volume. The
check should test scale, orientation, and local distortion, not merely confirm
that the scanner starts. If the instrument requires multiple views, test the
registration procedure on a benign object of comparable spatial extent. Record
calibration validity, environmental conditions, and the exact acquisition mode.

Support the instrument so that it remains stable without imposing an unrecorded
deformation. If geometry is measured under string preload, record tuning and
support. If geometry is measured without strings, that is a different state.
Neither is automatically the stress-free geometry required by a simulation.
Inferring an unstressed configuration from a loaded scan is an inverse mechanics
problem, not a cosmetic mesh operation.

## Multi-view registration

Acquire overlapping views with independent landmarks or targets where approved.
Do not use a single low residual from an iterative alignment algorithm as proof
of correct registration. Symmetric regions, smooth surfaces, occlusion, and
uneven point density can yield plausible wrong alignments. Check withheld
landmarks and dimensions that were not used to fit the transform.

Store each raw point cloud and its acquisition frame. Store rigid transforms
separately from the transformed clouds. Use a proper rigid transform with a
positive-determinant rotation; an accidental reflection can preserve distances
while reversing grain or microphone directions. Record whether any nonrigid
registration was used. Nonrigid alignment can hide physical deformation and is
not appropriate merely to make overlapping views coincide.

Report registration residuals spatially, not just as one mean number. A small
global RMS can conceal a large error near the bridge or neck junction. Inspect
residual distributions, outlier locations, and uncertainty along poorly observed
directions. Repeat a subset of views after repositioning to separate scanner
precision from full-workflow repeatability.

## Thickness mapping

Choose a safe method appropriate to access and material. Contact gauges, optical
internal scans, ultrasonic methods, or tomographic approaches each have different
calibration and interpretation requirements. No method is presumed available or
accurate enough by name alone. An ultrasonic time-of-flight measurement, for
example, depends on wave speed and propagation direction; using a generic speed
without uncertainty can turn material uncertainty into apparent thickness detail.

For paired surfaces, compute thickness with a defined correspondence method and
flag regions where normals intersect ambiguously or curvature makes nearest-point
distance misleading. For sparse contact readings, store the original points and
their contact direction before interpolation. Interpolated values are derived,
not measured at every mesh element. Preserve an observation-density map and a
confidence mask.

Write the thickness model as `h(x)` with a spatial covariance or a practical
regional uncertainty approximation. A scalar uncertainty repeated independently
at every vertex can create unrealistically rough perturbations. Scanner scale,
surface registration, and interpolation biases create correlated errors. Use
perturbation fields that reflect those mechanisms when evaluating sensitivity.

Do not assume left-right symmetry to fill missing regions unless it is a declared
model hypothesis. Test its influence against accessible measurements. Similarly,
do not use a uniform soundboard thickness merely because only one reading exists.
Use an interval or a family of plausible fields and state that the current data
cannot choose among them.

## Mesh preparation without erasing evidence

Preserve raw geometry and create a derived processing branch. Record outlier
removal, hole filling, smoothing, decimation, surface fitting, and topology repair
as separate transformations. A watertight mesh is convenient but may include
invented surfaces across unobserved regions. Mark those faces by provenance.

Quantify how processing changes distances, curvature, area, and volume. Use
mechanical sensitivity to decide whether the change matters. Smoothing can erase
a real thickness transition or soften a joint. Conversely, leaving scanner noise
as corrugation can create artificial stiffness or acoustic scattering. The right
choice is neither maximum detail nor maximum smoothness; it is a geometry model
with justified bandwidth and uncertainty.

Check normal orientation, nonmanifold edges, self-intersections, disconnected
components, and element quality. Retain named anatomical regions and landmark
mapping through remeshing. Mesh node identifiers are not stable measurement-point
identifiers. A response measured at a physical point should map to a continuous
surface location or interpolation operator, with mapping uncertainty, rather than
the nearest arbitrary node.

## Geometric checks tied to mechanics

Compare total modeled volume times assigned density with measured mass, allowing
for unmodeled components and density uncertainty. A mismatch can reveal geometry,
material, or component-accounting problems; it does not uniquely identify which
one is wrong. Do not rescale the entire mesh or density merely to force agreement
without examining the cause.

Check string-clearance geometry, bridge-foot contact, soundhole openings, and
support contact against photographs and direct dimensions. A tiny geometric error
near a contact can matter more than a larger error on an inactive surface. Use
separate local refinement for contact and global structural convergence when
appropriate.

For bending behavior, a useful sensitivity reminder is that ideal plate bending
stiffness scales with the cube of thickness under fixed material assumptions.
This is a model relation, not a claim that a real curved orthotropic setar plate
is a uniform isotropic plate. It explains why a small thickness uncertainty may
deserve more attention than a visually obvious surface feature.

## Uncertainty handoff

Deliver nominal geometry, raw observations, region provenance, registration
transforms, coverage masks, and an uncertainty description. Include a few
physically plausible perturbed geometries for downstream sensitivity tests only
if their construction is reproducible and their purpose is clear. They are
uncertainty realizations, not additional measured specimens.

The handoff should let the modeling team ask: which dimensions are directly
observed; which were inferred; which cannot be identified; which errors are
correlated; and which regions dominate the predicted response uncertainty? If the
answer is only “the scan is accurate,” the handoff is incomplete.

## Acceptance and stop conditions

Pass when required regions have sufficient documented coverage for the scoped
model, independent dimensions agree within uncertainty, repeated registration is
consistent, and mesh processing has not hidden unsupported geometry. Stop a
geometry-specific fidelity claim when thickness or contact regions are unknown
at a level that dominates the target response. Continue with a deliberately
generic geometry or a measured reduced response if that route serves a narrower
claim, clearly labeled.

If safe access is insufficient, do not expand the intervention without approval.
Record the missing region, its likely influence, and the least invasive experiment
that could reduce the uncertainty. The next action may be a witness component,
an authorized imaging service, or a model sensitivity study rather than a more
aggressive measurement on the reference instrument.
