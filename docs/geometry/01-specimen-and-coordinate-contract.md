# Geometry 01 — Specimen identity and coordinate contract

## Purpose, inputs, and evidence status

This is a proposed, unimplemented acquisition contract, not a claim that a setar has already been measured. A recognizable pear-shaped mesh, a plausible resonator, and a reconstruction of an illustration are different products. None establishes the geometry of a particular instrument. An accepted package identifies the specimen, its measurement state, the parts actually observed, and the parts inferred.

The goal is not a universal setar shape. A reference specimen provides one defensible starting point; later population modeling can represent differences between makers and instruments. Averaging exterior geometry can remove asymmetries that couple modes. Averaging thickness can change stiffness more than the exterior outline suggests.

Prerequisites are measurement permission, safe handling, calibrated length references, a mass measurement, and immutable storage for observations. Permission to photograph does not imply permission to remove finish, disassemble joints, move the bridge, or change tension. Obtain specific authorization for invasive or configuration-changing work.

## Specimen identity and configuration

Assign an opaque specimen identifier. Keep ownership details outside distributable assets unless permission includes them. Record maker attribution, species identification, age, and repair history with their evidence. A visual guess at mulberry is not anatomical identification.

Give each physically distinct configuration a state identifier. Include strings, diameters, tuning, tensions or tension estimates, bridge position and orientation, frets, pegs, covered openings, attached sensors, support contacts, and environmental readings. Photographs must show the configuration used for the corresponding measurement. Moving the bridge creates a different port map even if exterior CAD remains unchanged.

Use field-level evidence labels:

| Label | Meaning | Permitted interpretation |
|---|---|---|
| Measured | Direct observation with method and uncertainty | Specimen evidence within its conditions |
| Inferred | Model-based deduction from observations | Conditional estimate |
| Published | Value from another study or object | Prior, not specimen truth |
| Synthetic | Deliberately constructed fixture | Verification only |
| Unknown | No adequate evidence | Missing input or explicit hypothesis |

Calling an entire CAD file measured because its exterior was scanned hides the status of an invented interior. Preserve mixed provenance at the region and field level.

## Coordinate frames

Choose a reproducible right-handed specimen frame. To agree with the string volume, put the origin at a documented bridge reference point, positive x along the nut-to-bridge direction (continuing toward the tail from that origin), positive z approximately normal to the soundboard toward the strings, and positive y completing the right-handed frame. This is a convention, not anatomy. Describe exactly how another operator reconstructs it; do not infer y from a string-numbering order unless that order has been mapped explicitly.

“Bridge center” is insufficient. Use reproducible feature intersections or a fitted frame from documented landmarks. Where safe markers are prohibited, use photographed natural features and record localization uncertainty. Keep the unloaded geometry frame distinct from a resolved tensioned geometry.

Every dataset carries a transform:

\[
\mathbf x_s=R_{a\rightarrow s}\mathbf x_a+\mathbf t.
\]

Store matrix entries, translation units, vector convention, and whether the transform actively moves vectors or passively changes coordinates. Check orthogonality and determinant. A reflection must not silently repair a left-handed export. If reflection is necessary, account for surface winding and the distinction between polar vectors, such as force, and axial vectors, such as torque.

Use an asymmetric calibration fixture with three unequal dimensions, a named side marker, and a known torque direction. A sphere hides many frame errors. Transform a force and displacement together and verify unchanged work. Round-trip landmarks through every supported importer and exporter.

## Features are not node numbers

Create semantic identifiers for nut contacts, bridge string contacts, bridge feet, tail attachments, neck-body interfaces, soundhole loops, soundboard perimeter, bowl seams, and interior registration landmarks. Each carries a geometric definition, uncertainty, evidence link, and valid state.

Do not define a port solely as node 1034. Node numbers change after remeshing. Store its physical region and interpolation rule; regenerate and retain the numerical mapping. A port normal derived from one coarse triangle can rotate during refinement. Prefer a geometric patch normal or documented averaging procedure.

Measure speaking lengths between actual contact regions, not decorative endpoints. Rounded contacts may have direction-dependent effective positions. Record both the surface profile and the point-contact approximation used by the string model. An inferred contact location remains inferred even when its pitch prediction is excellent.

## Independent mass accounting

Weigh the instrument in its modeled state where practical. State whether strings, pegs, bridge, frets, sensors, adhesive, and cables are included. Total mass cannot separate density from thickness, but it detects many compensating errors.

\[
m_{\mathrm{model}}=\sum_r\int_{\Omega_r}\rho_r\,dV+\sum_k m_k.
\]

The second sum contains declared lumped components, not a hidden correction that forces agreement. Report mass by region. If a thin adhesive is omitted geometrically but included as mass, identify its location and its mechanical role.

Where safely authorized, several support measurements may constrain center of mass. Include support compliance and friction in uncertainty. Do not balance a valuable instrument precariously. Center-of-mass disagreement can expose missing peg mass or an incorrect neck density even when total mass matches.

## Reference geometry is not measured geometry

Pedrammehr et al., §3.1, describe exterior scanning and CAD processing, but the inspected article does not release complete raw CAD or establish interior thickness accuracy: https://www.extrica.com/article/19505. Use it as a methodological reference, not a downloadable specimen definition.

A development geometry should have a visibly synthetic identifier and parameterized construction. Identify illustrative dimensions and literature-derived proportions separately. A later specimen can reuse the pipeline without inheriting synthetic provenance. Likewise, visible pegs in a rendering do not prove their mass or stiffness is included in the analysis.

## Acquisition task and evidence gate

1. Record the untouched state and permitted operations.
2. Establish the frame and independent length references.
3. Acquire overlapping exterior observations.
4. Repeat selected views independently to expose registration bias.
5. Acquire permitted interior and thickness observations.
6. Measure mass and accessible dimensions independently.
7. Register data without overwriting original records.
8. Build features and overlay them on observations.
9. Report complete, partial, inferred, and absent coverage.

Separate scanner calibration, registration, and reconstruction uncertainty. Their errors may be correlated. A fitted cloud residual measures agreement with the cloud used in fitting; it does not automatically measure accuracy against the instrument.

Acceptance requires another engineer to locate every model port from the record. Evidence includes transform round trips, handedness and unit checks, landmark overlays, dimensional comparisons, and mass accounting. Tolerances follow target observables and measurement capability, not a convenient scanner specification.

Include a deliberately incorrect millimeter-to-meter conversion in negative tests. It must fail before a solver runs. Include mirrored geometry and swapped force-axis fixtures. A successful importer should reject or explicitly convert them, not produce plausible-looking but physically inconsistent files.

Missing specimen evidence blocks specimen-fidelity claims, not synthetic algorithm development. Keep that distinction in all handoffs. For uncertainty terminology, consult JCGM 100, Chapters 4–6: https://www.bipm.org/en/doi/10.59161/jcgm100-2008e. The guide supplies reporting principles, not setar-specific tolerances.
