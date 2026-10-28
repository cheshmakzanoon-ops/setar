# Acoustics 06 — Solver exchange, portable assets, and evidence packages

## Purpose and implementation status

This proposed, unimplemented exchange contract makes a structural/acoustic result reproducible outside the application that created it. Inputs are approved geometry, solver configurations, matrices, modes, transfer data, and validation records. Outputs are a versioned evidence package and import tests.

A screenshot of a mode shape is not an exchange format. A proprietary project file may be useful evidence, but it is insufficient if another engineer cannot inspect units, constraints, port maps, and normalization without that application. Preserve neutral numerical data alongside native projects when rights permit.

The contract is deliberately solver-independent. It does not claim that every listed tool is installed, licensed, or interchangeable. Each adapter must demonstrate a round trip on controlled fixtures before handling specimen assets.

## Package layers

Separate four layers:

| Layer | Content | Purpose |
|---|---|---|
| Evidence | Raw observations, calibration, permissions, provenance | Establish what was measured |
| Reference model | Geometry, fields, matrices, solver settings | Reproduce the offline hypothesis |
| Reduced asset | Canonical states, ports, observation maps | Drive the physical runtime |
| Qualification | Tests, tolerances, results, failures | Bound the supported claim |

Do not bundle private owner details or restricted recordings into a distributable runtime asset. The runtime may need parameter values and provenance identifiers without needing the underlying personally identifying records.

Each layer has a content hash and explicit relationship to its inputs. Hashes establish identity and detect changes; they do not establish scientific correctness or permission.

## Manifest requirements

The manifest records schema version, specimen and state identifiers, evidence class, creation procedure, tool versions, units, coordinate frames, harmonic convention, numeric precision, and valid band.

It also records model hypotheses: shell versus solid, preloaded versus unloaded, support condition, joint laws, bridge contact state, material loss, cavity representation, exterior method, and observation definition. Without these, two matrices with the same dimensions may describe different physical problems.

Missing values remain missing. Use an explicit unknown status rather than a plausible default. A parser must distinguish unknown density from zero density and absent damping from a deliberate lossless model.

Include license and redistribution fields for geometry, code, measurements, and derived assets. A paper being accessible online does not automatically authorize redistribution of its figures or supplementary data.

## Degree-of-freedom map

Export a table mapping every row to semantic region, mesh entity, component, unit, constraint status, and coordinate frame. Preserve the mapping from original to retained degrees of freedom after elimination.

Nodal ordering can be interleaved xyz or component-blocked. High-order nodes may not coincide with familiar CAD vertices. Mixed models can include rotations, pressures, internal variables, or Lagrange multipliers. Do not infer ordering from array length.

If constraint elimination uses \(q=Tq_f+q_0\), export T and the offset/reference state or an equivalent reconstructible rule. Reduced matrices must correspond to that exact transformation. Applying one ordering to mass and another to stiffness can produce plausible eigenvalues with incorrect shapes.

Store local frames for rotational or directional coordinates. Mixed-unit matrices need per-coordinate units and scaling, not one misleading label such as “stiffness in N/m” for every entry.

## Matrix formats

For small and medium reproducibility fixtures, Matrix Market is a useful transparent exchange. Its official NIST specification distinguishes coordinate and array forms, numeric types, and symmetry storage: https://math.nist.gov/MatrixMarket/formats.html.

Declare one-based indices when using the format, preserve the header, and handle symmetric or Hermitian triangular storage correctly. Do not write a general matrix header while storing only one triangle. Duplicate entries need a documented accumulation or rejection policy.

Complex matrices must state the phasor convention. A complex symmetric matrix is not the same as Hermitian. Radiation and reciprocal frequency-response matrices often use transpose symmetry while power uses a Hermitian part.

For large assets, a structured binary format can be appropriate. Specify dataset names, shapes, scalar types, byte order where relevant, compression, and attributes. A binary file with undocumented layout is not portable merely because it has a familiar extension.

## PETSc and solver-native caveats

PETSc’s MatLoad documentation describes binary/HDF5 loading and backend-specific restrictions: https://petsc.org/main/manualpages/Mat/MatLoad/. Its documented HDF5 behavior includes format and transposition caveats; do not assume every MatView/MatLoad pairing is equivalent across versions and matrix types.

Pin the tested configuration and verify round trips numerically. If an adapter exports to Matrix Market through an intermediate sparse representation, confirm that constraints, block ordering, and complex values survive.

Native solver exports may use scaled mass or stiffness, suppressed constrained rows, or normalized eigenvectors. Read the actual export documentation and retain scaling metadata. Never recover physical units by adjusting a gain until one resonance amplitude matches.

## Structural numerical payload

The reference structural payload contains M, K, any explicit C or internal-variable realization, B, observation maps, constraints, equilibrium q0, and geometry/mapping hashes.

For modal exports, include eigenvalues, modes, normalization matrices, residuals, rigid-body treatment, and mode-family identifiers. Keep display-normalized shapes separate from mass-normalized computational shapes.

If damping is modal rather than physical-matrix based, say so. Export modal rates with their evidence and coupling assumptions. Do not fabricate a unique material damping matrix from a set of observed decays unless a specific reconstruction is justified.

## Acoustic numerical payload

Export pressure and velocity bases, boundary quadrature weights, normals, interface matrices, air properties, and the definition of any generalized source coordinates.

Frequency-domain data need frequency units, sorting, complex convention, source/receiver ordering, and whether propagation factors have been removed. A frequency array in Hz must not be consumed as rad/s.

For radiation impedance, include the effort/flow pairing and the sign of power entering the fluid. For microphone transfers, include coordinates, orientation, near/far-field status, reference distance, and calibration gain.

Store aperture pressure/volume-flow ports separately from solid surface force/velocity ports. Their units differ even when both are represented by numeric arrays. A unit-aware importer should reject an attempted interchange.

## Synthetic round-trip fixture

Use this deliberately synthetic two-coordinate system:

\[
M=\begin{bmatrix}2&0\\0&3\end{bmatrix},\quad
K=\begin{bmatrix}5&-1\\-1&4\end{bmatrix},\quad
C=\begin{bmatrix}0.2&0\\0&0.3\end{bmatrix},\quad B=I.
\]

Coordinates are meters, masses kilograms, stiffness N/m, damping N·s/m. Static compliance is

\[
K^{-1}=\frac1{19}
\begin{bmatrix}4&1\\1&5\end{bmatrix}.
\]

At \(q=(0.1,-0.2)^T\) and \(v=(0.3,-0.4)^T\), elastic energy is 0.125 J and kinetic energy 0.33 J. Dissipation rate is 0.066 W. With \(f=(0.5,0.25)^T\), input power is 0.05 W, so instantaneous total-energy rate is \(-0.016\) W.

These numbers are exact consequences of the synthetic fixture, not instrument measurements. Every adapter should reproduce them after export/reimport. Also permute coordinate order and verify invariance. Intentionally swap one mass row without the corresponding column; the symmetry and energy tests must fail.

## Geometry and mesh payload

Export neutral geometry where feasible, analysis meshes, semantic groups, material fields, and port definitions. Include a display mesh only as a convenience.

Check high-order element nodes and orientation on reimport. A converter may preserve connectivity but change ordering or discard curved nodes. Compare volume, area, group counts, and port locations against the original.

Preserve an evidence map identifying measured and inferred geometry. A neutral export should not erase uncertainty or make an inferred interior look measured.

## Alternatives to proprietary pipelines

An explicit candidate stack is: a geometry kernel or CAD tool with verified neutral export; Gmsh for meshing; MFEM or DOLFINx for custom FE assembly; PETSc/SLEPc for linear and eigenvalue solves; Bempp for homogeneous exterior boundary integrals; and an independently verified time-domain acoustic solver for cross-checks.

This is a set of alternatives, not a promised turnkey chain. Orthotropic constitutive laws, shell choices, contact, coupling matrices, and export adapters still require implementation and tests. Some packages expose Python interfaces while the final runtime remains C++.

k-Wave can serve as an independent acoustic reference under its documented method and licensing conditions. Its MATLAB-oriented workflows may require an available compatible environment; do not claim a license-free path without checking the selected implementation.

If a proprietary application is available, it can remain one reference solver. Export enough neutral data to reproduce the essential results elsewhere. Do not require collaborators to purchase software merely to inspect a basic mass matrix or normalization convention.

## Reproducible execution record

Record exact commands, solver options, source revisions, numeric libraries, precision, parallel decomposition, and environment dependencies. Avoid secrets and private credentials in logs.

A lockfile or version string is not sufficient if solver options are changed interactively without being recorded. Capture the actual configuration used for the reported output.

Separate deterministic identity checks from tolerance-based numerical checks. Distributed reductions can change roundoff without changing the physical result. Specify which outputs require exact hashes and which require invariant tolerances.

## Failure handling

Importers reject missing units, incompatible schema versions, invalid dimensions, nonfinite values, inconsistent hashes, and unsupported element types. They must not silently invent defaults or truncate matrices.

Numerically suspicious data can be quarantined with a report rather than deleted. Preserve the original and the failed checks. A reviewer should be able to reproduce why the asset was rejected.

If a file is partially written or an export is interrupted, do not publish it as ready. Use atomic publication of completed validated assets in the implementation. The audio callback only receives a fully prepared immutable object.

## Package acceptance

1. Reimport in an independent path.
2. Recompute mass, energy, static compliance, and residuals.
3. Verify coordinate and constraint maps.
4. Reconstruct selected FRFs and pressure transfers.
5. Check passivity and reciprocity with declared scaling.
6. Reproduce synthetic fixtures and negative tests.
7. Verify permissions and distribution scope.
8. Publish the evidence manifest and known limitations.

The gate passes only when another engineer can reproduce the claimed behavior without hidden GUI state or guessed units. Portable data do not make an unvalidated model true; they make its assumptions and failures inspectable.
