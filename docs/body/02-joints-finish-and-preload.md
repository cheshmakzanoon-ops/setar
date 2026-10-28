# Body 02 — Joints, finish, supports, and static preload

## The assembled instrument is not a collection of free plates

This chapter defines proposed, unimplemented assembly tasks for P08–P09. Inputs are geometry, admissible material fields, string configuration, observed joints, and support conditions. Outputs are a static equilibrium, a tangent dynamic model, joint hypotheses, and tests separating material behavior from assembly behavior.

A soundboard attached to a flexible bowl does not have the boundary condition of a plate clamped to an immovable frame. The neck carries string loading and interacts with the body through a finite joint. A resting bridge transfers force through a preload-dependent contact. Finish and glue can affect mass, stiffness, and loss. Omitting these mechanisms may be a useful approximation, but each omission needs a declared observable and a comparison.

Start with the simplest assembly whose load paths are correct. A detailed mesh of the wrong boundary condition is less informative than a modest mesh with explicit uncertainty about the joint. Do not force frequency agreement by adjusting wood moduli before checking whether the assembly is constrained correctly.

## Joint representations

A bonded interface imposes equal displacement between corresponding points. It can be implemented with conforming nodes, a consistent constraint, or a validated nonmatching-mesh method. A penalty connection approximates the constraint and introduces a stiffness parameter that can affect conditioning and high-frequency modes. “Very large” is not a reproducible parameter choice.

A compliant interface can use a relative displacement \(\boldsymbol\delta\), with traction

\[
\mathbf t=K_j\boldsymbol\delta+C_j\dot{\boldsymbol\delta}.
\]

Here \(K_j\) has units N/m\(^3\) when traction is Pa and displacement is m; \(C_j\) has units N·s/m\(^3\). For a lumped joint force, the corresponding matrices instead have units N/m and N·s/m. Mixing surface and lumped coefficients produces an area-dependent error that may hide during one mesh configuration.

Require symmetric positive-semidefinite dissipative and stiffness contributions when modeling a passive linear joint. Cross-coupling terms can be physically meaningful, especially in a local joint frame, but they need admissibility tests. A diagonal spring model is a simplification, not an automatic description of glue.

If the interface includes rotational degrees of freedom, define moments and angular velocities explicitly. A rotational spring coefficient has units N·m/rad. Do not combine translations and rotations in a matrix without documenting the generalized coordinates and their scaling.

## Neck and rim boundaries

Preserve the finite neck-body connection area. A point attachment can eliminate relevant moment transmission or introduce artificial stiffness depending on its constraints. Compare at least a monolithic limit, a compliant interface, and the evidence-supported nominal model. The comparison is an uncertainty study, not permission to fit arbitrary joint parameters.

At the soundboard rim, distinguish local adhesive compliance from flexibility of the bowl itself. If both are represented by one effective spring, label it as an assembled boundary model. Its fitted values may not transfer to a different bowl geometry.

Measure or bound joint extent where possible. Joint area, material stiffness, and layer thickness can be strongly confounded. An effective surface stiffness may be identifiable when the individual adhesive properties are not. Report that effective quantity rather than claiming microscopic material identification.

Check whether repairs or cracks create nonlinear or amplitude-dependent response. A linear joint calibrated at small excitation may fail under a strong gesture. Separate tests at multiple safe excitation levels can expose such behavior without immediately introducing an elaborate damage model.

## Finish and thin layers

Model finish only at the level supported by evidence. Options include explicit layers, distributed surface mass and loss, or a bounded omitted-effect hypothesis. A thin layer can be small in mass yet influence bending because its distance from the neutral surface matters.

For a layered plate, integrate stiffness through thickness with the correct lever arms. A surface mass changes inertia without supplying bending stiffness. A complex modulus supplies a frequency-domain loss model but does not automatically define a causal time-domain state update.

Do not infer a finish material from appearance alone. Record unknown composition and thickness. Avoid using an arbitrary damping multiplier to compensate for missing finish and then naming it a physical finish parameter.

If before-and-after construction data are available with permission, compare changes under consistent support and environmental conditions. The difference can include moisture changes, curing, added mass, and altered boundaries. Attribution to one mechanism requires control measurements, not merely chronological ordering.

## Static string loading

Calculate string forces from actual geometry and tensions. At a bridge contact, the net force is the vector sum of tensions in adjacent segments, with directions pointing away from the contact along each segment. A small break angle can make the normal component much smaller than the axial tension. Do not apply full string tension vertically to the soundboard.

Include tail and nut reactions and the path by which they enter the structure. A model that applies bridge downforce but omits the balancing loads can impose an unintended net force or moment. In a free instrument, internal string forces must be balanced across their attachment system.

Solve equilibrium:

\[
\mathbf r(\mathbf q_0)=\mathbf f_{\mathrm{internal}}(\mathbf q_0)
-\mathbf f_{\mathrm{external}}(\mathbf q_0)=0.
\]

Then linearize about the equilibrium if small-signal dynamics are intended. The tangent stiffness can contain material, geometric, contact, and follower-load contributions. State which are included. A modal solve around an unloaded shape is a different hypothesis from a preloaded tangent solve.

If only a linear static model is used, quantify the resulting approximation. Compare deformation with thickness, contact gap, and geometric feature scales. A small absolute displacement can still alter bridge contact distribution significantly.

## Contact preload and operating envelope

The bridge’s normal load establishes which parts of its footprint remain in contact and the available friction force. Static equilibrium must not require tensile normal contact from a resting foot. If it does, inspect the geometry, force directions, bridge mass, and contact model.

Evaluate the range of dynamic force over the intended gesture envelope. A linearized contact is valid only while its assumed active set and sticking state remain appropriate. If the predicted pressure becomes negative, the model has left that validity region; clipping pressure after the solve does not preserve the original energy balance.

When nonlinear contact is needed, choose a unilateral formulation and a documented regularization or complementarity solver. Coordinate with the numerical contact contract so normal elastic storage and tangential dissipation are counted correctly. Contact parameters must not become arbitrary “tone” controls without a mapping back to physical assumptions.

## Supports during measurement and performance

Free-free suspension, laboratory support, and playing support are different boundary conditions. A soft suspension introduces low-frequency support modes and damping. A player’s hand, torso, or clothing adds distributed compliance and loss. Model and measure the actual condition used for validation.

Do not set every contacted point to zero displacement unless that rigidity is justified. A compliant support may be represented by a passive impedance over a finite patch. Its parameters should be measured or bounded, and the patch geometry should follow the specimen state.

Use support perturbation experiments to identify sensitive modes. If moving a soft support changes a measured resonance substantially, the mode is not well approximated as free-free under that setup. Record the effect instead of averaging it away.

## Assembly verification tasks

1. Verify each isolated component’s mass and rigid-body modes.
2. Connect a synthetic two-body fixture with a known joint.
3. Check static force and moment balance.
4. Check reciprocal displacement response for linear passive joints.
5. Check nonnegative joint dissipation under prescribed relative motion.
6. Solve the unloaded and preloaded assemblies.
7. Inspect contact pressure and active regions.
8. Compare tangent predictions with small dynamic perturbations of the nonlinear reference.
9. Repeat under declared support conditions.
10. Archive equilibrium residuals and sensitivity results.

For a penalty interface, refine both mesh and penalty parameter. A result that changes with penalty stiffness has not reached a justified constrained limit. Excessive penalty can produce poor conditioning and high-frequency artifacts; insufficient penalty creates unwanted compliance.

For nonmatching interfaces, verify work conservation. If one side interpolates displacement with matrix \(T\), the conjugate force transfer must use the appropriate transpose and integration weights. Independent interpolation of force and displacement can create or destroy energy.

## Acceptance evidence

The assembly gate requires force and moment equilibrium, stable tangent behavior in the declared operating region, correct interface units, and agreement between reduced joint hypotheses and more detailed fixtures within an allocated budget. It also requires clear support labels on every modal or FRF comparison.

Primary implementation reading: MFEM’s annotated linear-elasticity example, especially its weak-form traction treatment and material/boundary attributes, provides a concrete assembly starting point: https://mfem.org/annotated/ex2/. It is an isotropic example, not a finished orthotropic setar solver. Extending it requires the tensor and joint tests above.

The next dependency is bridge multiport modeling. Supply the equilibrium, local contact geometry, tangent matrices, and validity bounds. A bridge model cannot infer these from a final audio recording alone.
