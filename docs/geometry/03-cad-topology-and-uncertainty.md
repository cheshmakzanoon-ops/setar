# Geometry 03 — Analysis topology and controlled simplification

## Physical region graph

This proposed, unimplemented task turns observed and inferred geometry into analysis regions. Inputs are surfaces, thickness, grain fields, and mechanical hypotheses. Outputs are a region/interface graph, meshing specification, parameterized uncertainty, and topology tests.

Identical outlines can represent different instruments if one model welds a bridge to the soundboard and another preserves contact. Define soundboard, bowl regions, neck, bridge, relevant masses, joints, internal air, radiating surfaces, and opening passages according to evidence. Do not invent braces or omit observed features because a generic example used a simpler construction.

Interfaces must be explicit:

| Type | Meaning | Required declaration |
|---|---|---|
| Shared continuum | Continuous displacement and traction | Region union |
| Bonded interface | No relative motion in the model | Bonded area and validity |
| Compliant joint | Relative motion with force law | Stiffness, loss, preload |
| Contact | Unilateral force and possible friction | Gap and contact law |
| Free surface | No traction except modeled loading | Boundary classification |
| Acoustic opening | Fluid connectivity or aperture law | Passage and end conditions |

Store neighboring regions and orientation. An interface normal from A to B reverses when viewed from B. A single global outward normal is inadequate for all internal interfaces.

## Boolean operations are modeling decisions

A union can convert a compliant joint into monolithic material. Retaining coincident but disconnected surfaces can create an unintended free body. Preserve pre-union components, document each operation, and count connected structural components before and after constraints.

Check duplicate faces, overlapping solids, inverted normals, nonmanifold edges, slivers, small gaps, and disconnected air regions. Classify defects before repair. Closing a reconstruction gap can be correct; closing a soundhole is not. An automatic operation cannot infer the distinction reliably from size alone.

Compare independently computed signed volume with the CAD kernel’s volume. Agreement is not proof of physical accuracy, but disagreement blocks analysis. Test acoustic connectivity from every opening to the intended cavity. Test the expected structural rigid-body nullspace before supports are added.

Maintain a synthetic topology fixture with two cavities and a small connecting aperture. The conversion chain must distinguish open and blocked versions. This catches repair routines that silently seal passages.

## Simplification follows observables

Small size does not establish negligible influence. Compare feature dimensions with bending wavelength, acoustic wavelength, wall thickness, contact footprint, and stress-concentration scale. A narrow aperture can have little volume yet appreciable inertance. Decorative details might be structurally unimportant but affect high-frequency close-field scattering.

Separate geometric simplification, constitutive simplification, and model-order reduction. Changing all three simultaneously makes discrepancies difficult to locate. Evaluate one category at a time using the same excitation, support, and observation definitions.

For each candidate simplification, state the mechanism it removes and the observables it could affect. Compare detailed and simplified fixtures. Record a frequency-dependent discrepancy, not merely one resonance difference. Retain the simpler model if it meets the declared budget, but preserve the comparison that justifies it.

## Meshing metadata

Assign stable physical groups to materials, contact patches, joints, acoustic boundaries, measurement regions, and ports. Gmsh’s official manual documents physical groups, high-order elements, and MSH exchange; export coverage can depend on physical-group membership: https://gmsh.info/doc/texinfo/. Verify behavior for the pinned version.

Colors and CAD layers are useful for inspection but may disappear during conversion. A separate manifest must identify groups and expected entity counts, areas, and volumes. Overlapping measurement and radiation groups can be legitimate; overlapping constitutive assignments require an explicit rule.

Preserve high-order node ordering and curved mappings. Dropping midside nodes changes geometry and approximation order. Reject unsupported element types rather than silently linearizing them. An apparently successful import can otherwise produce a different physical model.

Store semantic port regions independently of generated mesh indices. Verify regenerated interpolation weights, local axes, and force resultants after remeshing. A port that moves numerically during mesh refinement contaminates a convergence study with a changing problem.

## Uncertainty without invalid geometry

Use constructive parameters: thickness coefficients on a midsurface, constrained bridge position, bounded opening dimensions, or a joint-region extent. Independent random vertex displacement can create invalid elements and implausible roughness.

Each parameter needs units, evidence class, reference value, admissible range, correlation information, and affected regions. Dimensionless coefficients still require their basis functions. Otherwise the numbers cannot reconstruct geometry.

A perturbation that opens a crack, closes an aperture, or changes the bridge contact set is a topology change. Treat it as a separate hypothesis, not a smooth derivative for a local optimizer. Keep manufacturing variability distinct from uncertainty about one specimen.

For covariance propagation, retain correlations introduced by the measurement method. A global scaling error affects all lengths together. A shared segmentation threshold can affect an entire surface. Ignoring these correlations can make uncertainty intervals artificially narrow.

## Load-path fidelity

Preserve the finite neck-body attachment area when possible. A point constraint can distort rotational stiffness. If detailed joint geometry is unknown, compare explicit interface hypotheses and report the resulting spread. Do not hide this uncertainty by changing neck modulus.

A soundboard bonded to a flexible bowl is not a plate clamped to immovable ground. Rim flexibility, bowl motion, and adhesive response determine the effective boundary. At the bridge, preserve footprint and lever arms from string contacts to feet. A point force is a hypothesis requiring comparison with distributed contact.

The acoustic mesh must distinguish inner and outer surfaces even where a shell structural model uses a midsurface. Reusing the midsurface for both fluid domains may be adequate at low frequency, but it changes cavity volume and passage geometry. Quantify that approximation.

## Build procedure

1. Load immutable observations and an approved parameter manifest.
2. Construct named solid and air regions in SI units.
3. Apply simplifications in a reproducible order.
4. Build and verify the interface graph.
5. Reject invalid geometric parameter samples.
6. Export a documented neutral representation.
7. Generate structural and acoustic meshes with semantic groups.
8. Reimport through an independent inspection path.
9. Compare volumes, areas, ports, normals, and connectivity.
10. Hash inputs, instructions, outputs, and reports.

No proprietary CAD application is required by this contract. A selected alternative must demonstrate needed operations and export fidelity. Open-source availability does not prove a working installation. Neutral formats do not guarantee equivalent interpretation. Record versions and licenses before redistribution or integration.

## Acceptance evidence

Connect uncertainty to observables in an explicit table: thickness to bending behavior, opening geometry to fluid response, bridge position to residues, grain orientation to coupling, and joint extent to load transfer. These are testable hypotheses, not universal sensitivity rankings.

Allocate separate budgets for geometry, discretization, materials, supports, and measurement. A bridge-mobility budget can differ from a directivity budget. Refinement passes when target observables stabilize within the allocated geometric budget and unresolved uncertainty remains visible.

If two plausible interior reconstructions cannot be distinguished by current evidence, retain both. Selecting whichever fits one recording best does not resolve the ambiguity. The next tasks consume this semantic package for material auditing and FEM assembly; an unlabeled display mesh is not an adequate substitute.
