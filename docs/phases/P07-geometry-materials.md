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

> Build an analysis-ready geometry/material package from the authorized records.
> Preserve raw evidence and coordinate transforms. Audit units, topology,
> orthotropic conventions, reciprocity, and positive definiteness. Identify
> hidden dimensions and unsupported constants instead of inventing them.
> Produce a sensitivity-driven measurement request and a clearly labeled
> synthetic alternative for numerical work that must proceed without the data.
