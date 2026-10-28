# Geometry 02 — Surfaces, interior thickness, and grain fields

## Scope

This proposed pipeline consumes the specimen contract and produces exterior and interior surfaces, thickness, orientation fields, and confidence maps. None is claimed implemented or measured. An exterior scan does not directly reveal concealed joints, wall thickness, internal roughness, or three-dimensional grain direction.

These omissions matter differently. Thickness changes mass and bending stiffness; an opening changes fluid connectivity; grain orientation changes directional stiffness. For a thin bending-dominated plate, thickness enters stiffness cubically but mass linearly. That scaling motivates careful metrology without pretending that a curved orthotropic bowl is a flat isotropic plate.

Preserve four representations: raw observations, a fitted metrology surface, analysis geometry, and a display mesh. A smooth display mesh cannot substitute for the analysis geometry. Raw scanner noise should not become elastic corrugation merely because it was measured. Reconstruction regularization must have a stated spatial scale and an observable-sensitivity justification.

## Thickness methods and permissions

Select methods according to access, calibration, risk, spatial resolution, and authorization. Calipers constrain accessible edges, not arbitrary interior points. Magnetic gauges require a safe follower and curvature calibration. Ultrasound requires justified anisotropic wave speed, interpretable echoes, and an approved coupling method. Tomographic imaging introduces resolution, segmentation, and artifact uncertainty. None of this equipment is assumed available.

Do not prescribe liquid couplant, adhesive, or finish removal without approval. Equipment availability is not permission to use it on a particular specimen. If only partial observations are possible, fit a parameterized thickness field and mark unobserved regions explicitly. Smooth interpolation is an inference, not a new measurement.

Record measurement direction. Global-axis distance differs from local-normal thickness on a bowl. Specify whether thickness is shortest surface separation, normal offset from a midsurface, or distance along a manufacturing direction. A shell formulation must consume a compatible definition.

## Interior reconstruction

Register interior and exterior surfaces through shared landmarks or a common acquisition frame. Do not align them by minimizing thickness variation unless uniform thickness is independently justified; that would remove the variation being sought.

Trace midsurface normals toward the opposite surface and reject ambiguous intersections near rims, holes, or tight curvature. Compare with nearest-surface distance as a diagnostic. Strong disagreement may indicate that a single shell thickness is inadequate locally.

Use solid regions where shell assumptions fail, especially thick transitions or complex joints. Shell-solid coupling requires a validated kinematic relation; coincident nodes alone do not prove correct moment transfer. Check offsets for self-intersection and collapsed openings. Exterior rendering can conceal a negative internal volume.

Do not automatically fill all holes. Distinguish soundholes, cracks, access gaps, reconstruction defects, and intended interfaces before repairing topology. An acoustic opening and a scanner hole may look similar to a generic mesh tool.

## Grain as a field

Assign a local orthonormal material basis \((\mathbf e_L,\mathbf e_R,\mathbf e_T)\), not one longitudinal direction for the whole instrument. Longitudinal follows fibers; radial and tangential relate to growth rings. The USDA Wood Handbook, Chapter 5, “Orthotropic Nature of Wood” and “Elastic Properties,” is the primary definition reference: https://research.fs.usda.gov/download/treesearch/62244.pdf.

Construction determines the field. Separate bowl pieces can have discontinuous orientations; a carved bowl may require a different model. Observe the construction rather than importing it from a generic description. Surface grain constrains a tangent direction but may not determine fiber inclination. Store unresolved out-of-plane angle as uncertainty instead of calling zero inclination measured.

Fit orientations as rotations. Interpolating nine matrix entries independently can destroy orthogonality. Numerical reorthogonalization removes drift but cannot correct anatomically wrong axes. Do not smooth grain direction across a glue seam unless homogenization is deliberate and validated.

Rotate a fourth-order elasticity tensor by all four indices:

\[
C^g_{ijkl}=R_{ia}R_{jb}R_{kc}R_{ld}C^m_{abcd}.
\]

Engineering strain and stress vectors do not share an arbitrary identical six-dimensional rotation rule: engineering shear strain includes a factor of two. A Kelvin representation can simplify energy-preserving transformations, but every exchange file must state its convention.

## Orientation tests

Test uniaxial loading along each material axis, then rotate the whole experiment and compare physical strain energy. Include a ninety-degree longitudinal/radial swap on an anisotropic synthetic coupon. The stiff and soft directions should swap. Isotropic fixtures cannot detect many orientation errors.

Visualize axis glyphs at seams, curved regions, and representative interior points. Distinguish each axis clearly. Some axis sign reversals leave linear orthotropic elasticity unchanged; document equivalent choices so later directional loss or nonlinear constitutive work is not confused.

Export orientation values at quadrature points or through a reconstructible field definition. Merely storing axes at mesh vertices is insufficient unless interpolation and normalization are defined. Different interpolation choices can alter the assembled stiffness.

## Geometric uncertainty

Registration error moves an entire scan coherently; it is not independent noise at every vertex. Segmentation bias shifts an interior surface. Gauge offset shifts many thickness readings. Grain-angle uncertainty can be correlated along a board. Model these mechanisms rather than independently jittering all nodes.

Start with interpretable parameters: global scale, mean soundboard thickness, longitudinal gradient, bridge-region thickness, bowl-piece offset, and grain-angle offset. Add spatial degrees of freedom only when observations support them. Keep thickness positive by construction; log-thickness is one option, but document the resulting distribution. Silent clamping changes the ensemble.

Estimate observable sensitivities using perturbations larger than solver noise and smaller than the scale of nonlinear change. Compare central differences at two perturbation sizes. If the derivative changes wildly, inspect remeshing, mode swapping, topology changes, and insufficient solver accuracy before trusting an optimizer.

Thickness and modulus can compensate each other. Independent mass and geometric measurements reduce this ambiguity. Do not claim material identification from modal frequencies alone when the thickness field remains largely unknown.

## Reconstruction procedure and gate

1. Import observations with explicit units and transforms.
2. Verify independent reference dimensions.
3. Label observed and inaccessible surfaces, seams, openings, and contacts.
4. Fit surfaces with documented regularization.
5. Evaluate residuals on observations withheld from fitting.
6. Construct thickness or solid regions and reject ambiguity.
7. Assign per-piece grain fields and uncertainty.
8. Compute volume, cavity volume, contact geometry, and mass prediction.
9. Compare independent measurements.
10. Export analysis geometry with an evidence overlay.

The withheld observations prevent a flexible surface from merely fitting noise. Retain a simpler candidate geometry for comparison. If added detail does not change target observables beyond uncertainty, extra complexity may be unjustified.

Acceptance requires positive thickness, valid volumes, correct opening connectivity, consistent normals, reproducible grain frames, and explicit unresolved regions. No-warning CAD output is insufficient.

If mass is too large, investigate density, thickness, duplicate solids, omitted cavities, and unit conversions before reducing density. If bending frequencies are too high while mass matches, inspect constraints, grain direction, thickness distribution, and joint stiffness. If radiation disagrees while structural mobility agrees, inspect fluid geometry and observation conditions before altering elastic constants.

The next dependency is topology-preserving analysis geometry. Material fitting should not absorb geometry errors that the acquisition record could have exposed.
