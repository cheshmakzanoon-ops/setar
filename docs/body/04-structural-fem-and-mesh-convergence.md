# Body 04 — Structural FEM and mesh convergence

## Question and evidence status

This proposed, unimplemented P08 task asks whether a specified elastic assembly predicts its governing equations accurately enough to support instrument comparisons. A fine mesh does not establish realism. Geometry, material, joint, support, and discretization errors remain distinct.

Inputs are semantic geometry, admissible tensors and grain fields, density, equilibrium, ports, and a target frequency band. Outputs are assembled matrices, independent verification fixtures, convergence records, and a structural reference with a numerical error budget. Choose the band from the intended observables: low-frequency bridge mobility, attack radiation, and local contact stress can require different resolution.

## Weak form and dimensional contract

For displacement \(\mathbf u\), density \(\rho\), and stress \(\sigma=C:\epsilon(\mathbf u)\),

\[
\int_\Omega\rho\,\mathbf w\cdot\ddot{\mathbf u}\,dV+
\int_\Omega\epsilon(\mathbf w):C:\epsilon(\mathbf u)\,dV
=\int_\Omega\mathbf w\cdot\mathbf b\,dV+
\int_{\Gamma_t}\mathbf w\cdot\mathbf t\,dS.
\]

Body force density is N/m\(^3\), traction N/m\(^2\). With translational nodal coordinates, mass has units kg and stiffness N/m. Mixed translational/rotational coordinates require a generalized unit map. The assembled equation is \(M\ddot q+C\dot q+Kq=Bf\).

Initially omit damping while verifying stiffness and inertia. Damping must not conceal spurious modes or an unstable interface. Check normalized matrix symmetry before constraints, after constraints, and after reduction. Small roundoff asymmetry is different from a systematically nonsymmetric assembly.

## Element choice

Thin shells efficiently represent bending, but their shear treatment, curvature approximation, and thickness integration must fit the region. Solids avoid some shell assumptions but can lock in bending with unsuitable low-order elements. More elements do not necessarily repair a formulation defect.

Choose shell, solid, or mixed representations through benchmarks. Use an orthotropic plate with exactly matched boundaries, then a curved-shell fixture. Compare against analytical solutions where available and an independently refined formulation otherwise.

A shell may predict global mobility while failing local contact stress. Declare the intended output rather than treating every discrepancy as equivalent. Mixed shell-solid interfaces must transfer translations and rotations without artificial stiffening; test bending and torsion across a synthetic interface against a single-formulation reference.

Quadrature must resolve spatially varying orientation as well as basis functions. Underintegration can create zero-energy modes. Overintegration cannot fix a wrong tensor rotation. Check Jacobians at integration points; curved elements can invert internally despite acceptable corners.

Mass lumping is a model change. Compare consistent and lumped mass on frequencies, residues, and forced response before using it for an explicit integrator. Do not attribute lumping error to uncertain wood properties.

## Loads and boundaries

Preserve force and moment under mesh refinement. Fixed total force over a changing patch is different from fixed traction. The experimental excitation determines which quantity is held constant. Store patch geometry and quadrature rather than relying on whichever node is nearest.

A free connected three-dimensional body has six rigid motions in small-strain elasticity. Extra near-zero modes can indicate disconnected components or underintegration; missing ones can indicate unintended constraints. Test analytically constructed translations and rotations directly.

Do not fix arbitrary nodes to invert a free-free stiffness and then call the result free-free. Use nullspace handling or a documented projection. If a reference constraint is used, prove that it leaves the relevant elastic response unchanged.

Inspect support reactions in constrained models. Their resultant and moment must balance loads. A boundary applied to the wrong physical group can still produce a numerically converged, visually plausible solution.

The official DOLFINx elasticity demo demonstrates rigid-body near-nullspace construction: https://docs.fenicsproject.org/dolfinx/main/python/demos/demo_elasticity.html. It is an implementation reference, not an adopted setar solver.

## Convergence outputs

Track total mass, static port compliance, eigenfrequency clusters, mode subspaces, driving-point mobility, cross-mobility, strain-energy distribution, and surface normal velocity projected into acoustic basis functions.

Frequency convergence alone is insufficient. A mode shape can remain inaccurate near the bridge, producing wrong residue magnitude. Radiation can remain wrong after bridge mobility converges because the surface phase pattern is unresolved.

Use at least three systematically related discretizations. Refine size, order, or both while preserving physical geometry and ports. State whether curved geometry approximation changes too. Separate field and geometry error where possible.

Two points do not establish asymptotic order. Nonmonotonic results can arise from mode crossing, changing geometry, solver tolerances, or competing errors. Investigate before fitting a convergence exponent.

## Experiment procedure

1. Freeze one physical hypothesis and boundary state.
2. Declare outputs and allocated numerical tolerances.
3. Generate coarse, intermediate, and fine meshes.
4. Verify groups, volumes, mass, and port resultants.
5. Assemble identical constitutive assumptions.
6. Solve with algebraic errors below mesh differences.
7. Match mode subspaces rather than frequency ranks alone.
8. Evaluate forced response on a shared adaptive frequency grid.
9. Refine where output sensitivity indicates unresolved behavior.
10. Publish trends, failures, and nonmonotonic cases.

The finest mesh is a reference approximation, not truth. If discretization dominates uncertainty, the gate remains open. A larger parameter-fitting tolerance cannot turn unresolved numerical error into physical uncertainty.

## Algebraic solver controls

Residual tolerance controls the linear solve, not the underlying physical approximation. Ill-conditioned systems can have small residuals but significant state errors. Compare selected cases with a direct or independently configured solve and inspect output sensitivity.

Near resonances, forced-response systems become sensitive. Use adequate damping interpretation and frequency resolution. A sparse grid can miss narrow peaks, making two different models appear to agree.

Shift-invert eigenanalysis has inner linear and outer eigenvalue tolerances. Record both, along with preconditioners and convergence histories. Do not report only an iteration count.

Double precision is the baseline reference. Mixed precision is an optimization experiment requiring separate comparison. Bitwise distributed-solver identity may be unrealistic, but physical invariants and target outputs must satisfy declared tolerances.

## Failure diagnosis

Long thin elements can degrade bending behavior; slivers can worsen conditioning without useful resolution; abrupt transitions near contact can distort response. A mesh size based only on acoustic wavelength ignores direction-dependent structural bending wavelengths.

Aspect ratio is not a complete error estimator. Anisotropic refinement may be useful when aligned with solution structure. Validate actual approximation error rather than applying a single geometric score mechanically.

A point load can cause nonconvergent local stress. Use a physical footprint for contact questions. For global mobility, compare point and distributed loading as distinct hypotheses and report their band of agreement.

Check mesh and parameter changes separately. A different port interpolation, accidental material-group loss, or altered constraint can masquerade as convergence. Export comparison manifests so these changes are visible.

## Acceptance and handoff

Require element tests, nullspace checks, force/moment balance, mesh trends for target outputs, and reproducible matrix export. High element count, a successful solve, and attractive mode animation are not evidence substitutes.

MFEM’s annotated Example 2 supplies a concrete vector-elasticity weak-form starting point: https://mfem.org/annotated/ex2/. Gmsh’s official manual documents high-order and exchange details: https://gmsh.info/doc/texinfo/. Pin actual versions before scripting their options.

The modal task receives \(M,K,C,B\), constraints, coordinates, mesh hashes, nullspace policy, and convergence results. It must not infer these from undocumented solver binaries. The acoustic task additionally receives physical surface geometry and normal-motion maps, not just mode frequencies.
