# Body 01 — Orthotropic material admissibility and source audit

## Scope and evidence boundary

This chapter specifies an unimplemented material-ingestion and verification pipeline for P07–P08. Its inputs are specimen geometry, grain fields, measured or published elastic data, and their uncertainty. Its outputs are admissible constitutive tensors, provenance records, rejected-data reports, and reproducible coupon tests. No parameter table in this chapter is a calibrated setar material asset.

The engineering problem is not merely choosing a density and a Young’s modulus. An orthotropic solid has distinct extensional and shear response in its material directions. Inconsistent Poisson ratios can violate reciprocity. A tensor that is numerically invertible can still store negative elastic energy. A plausible species label cannot rescue either defect.

Use the current specimen’s evidence whenever available. Literature values are priors whose species, moisture, specimen preparation, test method, loading frequency, and uncertainty may differ. A bending-derived modulus is not automatically identical to an axial dynamic modulus. A fitted effective modulus may represent unresolved geometry and joints rather than clear-wood properties.

## Define conventions before entering numbers

Use material directions L, R, and T as defined by the grain field. Define \(\nu_{ij}\) as minus transverse strain in direction j divided by axial strain in direction i under uniaxial stress in i. The first index is the loading direction.

For engineering strain ordering

\[
\epsilon_e=(\epsilon_{LL},\epsilon_{RR},\epsilon_{TT},
2\epsilon_{RT},2\epsilon_{LT},2\epsilon_{LR})^T,
\]

and stress ordering

\[
\sigma_e=(\sigma_{LL},\sigma_{RR},\sigma_{TT},
\sigma_{RT},\sigma_{LT},\sigma_{LR})^T,
\]

write \(\epsilon_e=S\sigma_e\). The normal block is

\[
S_n=
\begin{bmatrix}
1/E_L&-\nu_{RL}/E_R&-\nu_{TL}/E_T\\
-\nu_{LR}/E_L&1/E_R&-\nu_{TR}/E_T\\
-\nu_{LT}/E_L&-\nu_{RT}/E_R&1/E_T
\end{bmatrix}.
\]

The shear diagonal is \(1/G_{RT},1/G_{LT},1/G_{LR}\). All compliance entries have units Pa\(^{-1}\). Elastic energy density is \(\tfrac12\sigma_e^T S\sigma_e\), equivalently \(\tfrac12\epsilon_e^T C\epsilon_e\) with \(C=S^{-1}\), when the conventions are applied consistently.

Reciprocity requires

\[
\frac{\nu_{ij}}{E_i}=\frac{\nu_{ji}}{E_j}.
\]

Choose three independent Poisson ratios and derive their partners. Do not enter six independent values into an ideal reciprocal elastic model and hope the solver resolves the disagreement. Different commercial tools may choose different subsets or conventions; an input dialog accepting the numbers is not an admissibility test.

## Audit of the setar literature table

Pedrammehr et al., §3.2 and Table 1, define the first Poisson subscript as loading direction. As printed, mulberry gives \(E_L=6.073\) GPa, \(E_R=1.354\) GPa, \(\nu_{LR}=0.092\), and \(\nu_{RL}=-0.102\). Their reciprocity ratios are approximately \(0.01515\) and \(-0.07533\) GPa\(^{-1}\), so they cannot describe one reciprocal elastic tensor. The other printed pairs also disagree; the three required shear moduli are absent. Walnut’s printed transverse moduli exceed its longitudinal modulus, which merits verification rather than a guessed correction. Source: https://www.extrica.com/article/19505.

This is an algebraic ingestion failure, not a declaration that the underlying experiments are worthless. Preserve the publication record unchanged in a quarantined source record. Record the calculation, convention, and missing quantities. Request clarification or independent measurements. Do not silently change a minus sign, decimal point, axis label, or unit.

The negative Poisson value alone is not the reason for rejection. Some anisotropic materials can exhibit negative Poisson response while remaining stable. The opposite signs in a reciprocal pair with positive moduli are the decisive contradiction here. Likewise, a surprising modulus ratio is a reason to investigate, not a mathematical proof of invalidity.

## Positive energy is a separate requirement

After enforcing a justified reciprocal parameterization, require positive density, positive moduli, symmetric compliance, and positive definiteness. A robust implementation scales the matrix to reduce unit and anisotropy conditioning problems, then attempts a Cholesky factorization and evaluates eigenvalues as a diagnostic. Do not use an absolute eigenvalue threshold without relating it to matrix scale.

For the normal block, positive diagonal entries and positive principal minors provide useful independent checks. Positive pairwise minors are not sufficient alone; the full determinant matters. The shear entries must also be positive. Report the smallest normalized eigenvalue and condition estimate. Near-singularity can produce extreme sensitivity even when all inequalities technically pass.

Positive definiteness applies to the constitutive law in its intended strain space. Plane-stress reduction is not accomplished by deleting arbitrary rows and columns. Partition the constitutive relation and eliminate out-of-plane stresses or strains according to the actual assumption. Verify that the reduced law preserves the appropriate energy and symmetry.

An orthotropic shell requires a through-thickness model. If grain or material changes across thickness, integrate the corresponding extensional, coupling, and bending stiffnesses. A single averaged three-dimensional modulus cannot generally reproduce all three. Unsymmetric layers can couple extension and bending even if each layer is individually orthotropic.

## Uncertainty parameterization

Sampling independent moduli and Poisson ratios from broad distributions can generate inadmissible tensors. Rejecting many samples after the fact distorts the intended uncertainty model. Prefer a parameterization that remains admissible, such as a structured Cholesky representation of the symmetric normal compliance block with positive shear compliances. Preserve orthotropic zero structure.

Such a parameterization is a numerical device, not a physical prior by itself. Map its implied distributions back to moduli and Poisson ratios and inspect them. A mathematically convenient broad Gaussian on Cholesky entries may imply implausible material behavior.

Keep separate uncertainty sources: specimen variability, measurement calibration, moisture state, frequency dependence, grain-angle error, and model discrepancy. If an effective tensor is fitted to assembled-instrument modes, do not label it a direct wood measurement. Report which independent measurements constrain it and which parameters remain correlated with thickness or joint stiffness.

Use a sensitivity matrix for candidate observables and parameters. Singular values reveal poorly constrained combinations. A narrow optimizer convergence criterion does not imply a narrow physical uncertainty interval. If two parameter combinations fit equally well, retain that ambiguity and seek a discriminating measurement.

## Coupon-level verification

Before assembling a setar, verify the constitutive implementation on synthetic coupons. Apply one normal stress at a time and check axial and transverse strains against the compliance matrix. Apply pure shear in each material plane and verify the engineering-shear factors. Rotate the coupon and repeat using transformed tensors.

Use an isotropic special case as a regression, but do not rely on it alone. Isotropy can hide axis swaps and shear-order errors. An anisotropic fixture with all three extensional and shear moduli distinct is necessary.

For each fixture, compare direct matrix evaluation, an independently assembled finite-element solution, and an energy calculation. Check mesh convergence, not merely one displacement value. A rigid translation should create no strain energy; a rigid rotation in small-strain linear elasticity should likewise be in the nullspace to discretization precision when represented exactly.

Verify work reciprocity with two load vectors:

\[
\mathbf f_a^T\mathbf u_b=\mathbf f_b^T\mathbf u_a
\]

for a symmetric, properly constrained linear system. This catches asymmetric assembly and inconsistent boundary transformations. It does not validate a nonlinear contact model, where the tangent and loading path require separate interpretation.

## Material-ingestion task

1. Parse source values without converting or correcting them invisibly.
2. Attach unit, convention, provenance, and evidence status to each field.
3. Convert units into SI with explicit tested factors.
4. Check completeness for the selected material model.
5. Evaluate reciprocal consistency before constructing the tensor.
6. Construct the accepted parameterization or reject it with reasons.
7. Check positive energy and numerical conditioning.
8. Rotate into the geometric material field.
9. Run coupon tests and archive their residuals.
10. Export a versioned material asset with a stated validity domain.

Missing shear moduli are not filled by the isotropic relation \(G=E/[2(1+\nu)]\) for an orthotropic wood unless isotropy is an explicit alternative hypothesis. An unknown modulus can be assigned a prior for sensitivity analysis, but the resulting model remains partly assumed.

## Acceptance evidence and limitations

The gate requires exact provenance, unit tests, reciprocal checks, positive-energy checks, orientation tests, and independent coupon convergence. The accepted material asset must state whether it is static, dynamic, effective, frequency-dependent, moisture-specific, or synthetic.

A successful material gate does not prove the instrument model. Geometry, joints, supports, bridge contact, and radiation remain separate uncertainties. Conversely, a fitted modal match does not excuse a nonphysical tensor. A reciprocal passive model is a structural constraint on the explanation, not an optional aesthetic preference.

Primary reading: USDA Wood Handbook, Chapter 5, “Elastic Properties,” equation 5–1, for the engineering constants and their relation: https://research.fs.usda.gov/download/treesearch/62244.pdf. Use its species data only with their stated conditions; it is not a specimen calibration service.

The next task is assembly modeling. Pass the full accepted tensor and orientation definition, not merely a species name or a screenshot of a material dialog.
