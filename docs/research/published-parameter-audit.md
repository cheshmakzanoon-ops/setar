# Published orthotropic-parameter audit

## Decision and inspected source

Table 1 of R-SETAR-PEDRAMMEHR-2018 is not an approved material preset. Its printed
normal-compliance pairs do not satisfy reciprocity under the source's stated
Poisson-index convention. A complete elastic tensor also requires shear inputs not
supplied by that table. Preserve extracted values as audit evidence; never silently
repair them or promote them to production defaults.

Source: <https://www.extrica.com/article/19505>, section 3.2 and Table 1, and
<https://www.extrica.com/article/19505/pdf>, PDF page 4, printed page 2683. HTML
and the rendered PDF were checked. The negative mulberry reverse
longitudinal/radial entry and the walnut transverse modulus magnitudes appear in
the PDF, so an HTML extraction error alone does not explain them. The intended
underlying values remain unresolved.

This is an original consistency audit, not a finding that every result in the
study is invalid. Its simulation could have used another internal representation
or additional inputs. The available publication does not resolve that question.
Conversely, reported frequency agreement does not make an inconsistent table safe
for another implementation.

## Fix the convention first

Let `L`, `R`, and `T` denote the material directions. Define `nu_ij` as minus the
strain in direction `j` divided by the strain in direction `i` under uniaxial stress
in `i`: first index loads, second index contracts. For the normal compliance block,

\[
\boldsymbol\epsilon=S_n\boldsymbol\sigma,\qquad
(S_n)_{ii}=1/E_i,\qquad (S_n)_{ji}=-\nu_{ij}/E_i.
\]

Reciprocal linear elasticity requires symmetric compliance and therefore

\[
\nu_{LR}/E_L=\nu_{RL}/E_R,\quad
\nu_{LT}/E_L=\nu_{TL}/E_T,\quad
\nu_{RT}/E_R=\nu_{TR}/E_T.
\]

Software using another index convention needs a documented adapter. Explicitly
write its stress and strain vectors, shear ordering, and whether shear strain is
engineering or tensorial. Test uniaxial loading along all three material axes.
Factor-of-two shear mistakes can remain hidden in an unrotated plate test.

## Minimal extraction and independent results

The printed mulberry longitudinal/radial inputs are `E_L = 6.073 GPa`,
`E_R = 1.354 GPa`, `nu_LR = 0.092`, and `nu_RL = -0.102`. Their compliance
ratios are `0.0151490` and `-0.0753323 GPa^-1`. With positive moduli, opposite
signs cannot satisfy reciprocity. Negative Poisson ratios are not categorically
forbidden in anisotropic elasticity; the reciprocal pair must nevertheless obey
the symmetry relation.

The walnut normal moduli print as `7.96`, `8.10`, and `9.12 GPa` in `L`, `R`,
and `T`. The longitudinal/radial Poisson pair `0.495`, `0.052` produces
`0.0621859`, `0.00641975 GPa^-1`. The difference exceeds rounding at the
displayed precision. Do not assume missing decimal points or interchange axes to
make the numbers look familiar.

The following are derived audit values using all printed normal entries, not new
measurements:

| Row | Pair | First ratio, GPa^-1 | Reciprocal ratio, GPa^-1 |
| --- | --- | ---: | ---: |
| Mulberry | LR / RL | 0.0151490 | -0.0753323 |
| Mulberry | LT / TL | 0.0724518 | 0.1049383 |
| Mulberry | RT / TR | 0.5051699 | 0.7407407 |
| Walnut | LR / RL | 0.0621859 | 0.00641975 |
| Walnut | LT / TL | 0.0793970 | 0.00383772 |
| Walnut | RT / TR | 0.0886420 | 0.0414474 |

These discrepancies justify quarantine, not guessed replacements. A sign error,
axis swap, column transcription, specimen mismatch, or decimal error would each
require different evidence and a different correction.

## Executable small audit

This self-contained example uses only the minimal pair necessary to demonstrate
the problem. It calculates in inverse pascals and prints a diagnostic rather than
constructing a production material. It requires Python's standard library only.

```python
from math import isclose, isfinite

def audit_pair(E_i_pa, E_j_pa, nu_ij, nu_ji):
    values = (E_i_pa, E_j_pa, nu_ij, nu_ji)
    if not all(isfinite(v) for v in values):
        raise ValueError("Nonfinite constitutive input")
    if E_i_pa <= 0 or E_j_pa <= 0:
        raise ValueError("Principal moduli must be positive")
    left = nu_ij / E_i_pa
    right = nu_ji / E_j_pa
    scale = max(abs(left), abs(right), 1.0e-30)
    residual = abs(left - right) / scale
    return left, right, residual

left, right, residual = audit_pair(6.073e9, 1.354e9, 0.092, -0.102)
print("compliances Pa^-1:", left, right)
print("relative reciprocity residual:", residual)
assert not isclose(left, right, rel_tol=1e-3, abs_tol=1e-18)
```

The numerical assertion tolerance is a regression-test choice, not a claim about
experimental uncertainty. A scientific admissibility decision should compare
residuals to an uncertainty model. Here the sign contradiction is already enough
to reject blind import under the chosen convention.

## Positive definiteness is not reciprocity

Passive small-strain elasticity requires positive energy for every nonzero strain:

\[
W=\tfrac12\boldsymbol\epsilon^T C\boldsymbol\epsilon>0.
\]

For a symmetric constitutive tensor this is positive definiteness. Check symmetry
before using a symmetric eigensolver or Cholesky factorization. Positive real parts
of eigenvalues from a nonsymmetric matrix do not establish elastic admissibility;
neither does a positive diagonal. Orthotropic admissibility requires a positive
normal block and appropriate positive shear terms in a consistent convention.

A diagnostic symmetrization of the printed normal-compliance blocks happens to
give positive eigenvalues for both rows. That does not validate the source table.
Symmetrization changes the material law, provides no uncertainty-based rationale,
and does not recover missing shear constants. It must not be labeled an
author-approved correction. The project must distinguish “one altered matrix is
positive” from “the reported material is established.”

For an approved candidate, scale the matrix by meaningful reference quantities,
report its smallest eigenvalue and condition number, and test robustness across
the parameter uncertainty region. A nearly singular positive matrix can be
numerically unusable. Thresholds should reflect precision and uncertainty rather
than arbitrary absolute numbers in whichever units were entered.

## Units, rotation, and damping

Convert GPa to Pa exactly once at the model boundary. Density is in kg/m^3;
compliance is in Pa^-1. A number labeled merely `damping` is insufficient. A
dimensionless modal damping ratio, loss factor, decay constant in s^-1, and Rayleigh
coefficient are different quantities. Their relationships depend on the assumed
model and frequency regime. The importer must reject an undefined convention.

Rotate local material tensors through an explicit grain frame. A geometrically
correct mesh with incorrectly oriented stiffness can still yield plausible-looking
mode shapes. Test isotropic rotational invariance, a known right-angle orthotropic
rotation, and random proper rotations. Energy positivity should survive rotation.
Validate orthonormality and positive determinant of every frame. A curved bowl's
surface normal is not automatically the growth-ring radial direction; a component's
long axis is not proof of its grain direction.

## Valid replacement routes

One route is clarification from the authors plus actual simulation inputs and
appropriate permission. Another is new measurements on associated witness wood
or conservatively identified instrument parameters. A third is a declared generic
passive material for numerical-method development. The third route is legitimate
but cannot support a claim of measured setar fidelity. Keep its labels distinct.

A constrained inverse fit may parameterize a symmetric positive tensor through a
factorization. This ensures admissibility, not unique physical identification.
Thickness, density, grain, joint flexibility, and supports may compensate for
elastic changes. Report confounding, profile likelihoods or posterior uncertainty,
and held-out spatial responses. Many fitted digits from a few resonance frequencies
are not evidence of many independently known material properties.

If a fit needs constants outside independently justified bounds, investigate
geometry and model discrepancy before broadening the bounds. If regularization
changes a material significantly, give the fit a new identifier and report the
perturbation. Never silently clip eigenvalues or modify input fields during loading.

## Regression and acceptance

Test importer rejection of nonfinite numbers, nonpositive moduli, absent shear
terms, incompatible reciprocal entries, invalid rotations, symmetric-indefinite
blocks, and excessive conditioning. Include unit-scale mistakes, swapped axes,
and a deliberately admissible synthetic material. Use the published discrepancy
as a named negative fixture, not a real-world preset. All validation must occur
outside real-time processing; an invalid record must never reach an audio callback.

The audit passes when input provenance, conventions, calculations, and quarantine
are reproducible, and every replacement is justified. Unresolved publication
intent need not stop unrelated solver research. It must stop unresolved constants
from acquiring authority through repeated copying. The next deliverable is a
schema validator and independent compliance test, not an assertion that this
chapter measured the elastic properties of any setar.
