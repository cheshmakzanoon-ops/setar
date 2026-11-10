# Independent research and mathematical review — 2026-11-10

## Scope, method and evidence boundary

Reviewer: `w8_b_research_review`, agent 23 of the 24-agent documentation campaign.
The [campaign ledger](2026-11-10-documentation-expansion.md) records the baseline
and delegated ownership. This review inspected the current working tree based on
`b4ae94e8cb1e77710a22d85eb707977d65f6c5a4`, including concurrent coordinator
corrections. Only this review file was written by this reviewer.

The review covered the [source atlas](../research/source-atlas.md),
[evidence ledger](../research/evidence-ledger.md),
[research-to-model workflow](../research/research-to-model-workflow.md), all four
[worked model decisions](../research/model-decision-examples.md), the
[finding](../../templates/research-finding.md) and
[promotion](../../templates/parameter-promotion.md) templates, and selected
high-risk acquisition and phase recipes. The selected recipes were P01-T01/T04/T06,
P04-T02, P08-T04/T05, P10-T04/T05, P12-T01/T04/T06, and the P17 listening-method
access boundary. The acquisition review concentrated on units/covariance, string
loss, bridge mobility, contact work, acoustic observation, manifest mechanics,
and promotion.

Independent arithmetic used Python's standard-library real and complex operations
in memory, without importing repository modules or executing its DSP. Source
comparison inspected current C++ definitions and the manifest validator. No build,
CTest, render, optimizer, measurement, source-author contact, new source-access
claim, parameter promotion, commit or push was performed. This is an analytic and
documentation review, not scientific validation or a rerun of historical evidence.

## Findings reported to the coordinator

| ID | Finding and consequence | Resolution inspected |
| --- | --- | --- |
| RR-01 | The revised critical/overdamped midpoint paragraph initially implied that the oracle always returned nonoscillatory fields. At exact critical damping with `gamma = Omega` and `gamma*dt/2 = 1`, both mapped roots are zero; `midpoint_damped_roots` rejects the zero spectral radius because its finite logarithmic decay cannot be represented. | The coordinator added the finite-positive-radius condition, named the exact rejection message, and distinguished the still-defined midpoint state update. Resolved by source comparison and algebra. |
| RR-02 | The historical identifiability paragraph inferred categorical impossibility and “reporting noise” from a nonzero synthetic profile difference of `5.8e-4`. That difference establishes weak separation for the stated fixture; practical nonidentifiability additionally depends on measurement, numerical and model uncertainty. It differs from the exact flexible-string invariance. | The coordinator preserved the historical `5.8e-4` and `28%` comparisons, made practical distinguishability conditional on propagated uncertainty, and separated the exact `EI = 0`, fixed-`T/mu` degeneracy. The revised paragraph was inspected. Resolved without a new experiment. |

No arithmetic correction was needed in the five worked examples. No unresolved
blocking error remained in this review's inspected scope after these corrections.

## Independent arithmetic: stiffness, tension and uncertainty

For the declared simply supported, uniform, linear synthetic string,

\[
T=4\mu L^2f_1^2-\frac{EI\pi^2}{L^2}
  =32.71372799791665\ \mathrm{N}.
\]

Using `L = 0.65 m`, `mu = 0.0004 kg/m`, `EI = 0.0002 N*m²` and
`f1 = 220 Hz`, direct substitution in the continuum characteristic equation gives:

| Mode | Independently calculated undamped frequency, Hz |
| --- | ---: |
| 1 | 220.0000000000000 |
| 2 | 440.0942341683486 |
| 3 | 660.3768694384681 |
| 4 | 880.9419384752351 |
| 6 | 1323.2944379704963 |
| 8 | 1767.8987930884746 |

These agree with the workflow's rounded table. The transformation
`z_n = (2*L*f_n/n)^2 = T/mu + EI*pi²*n²/(mu*L²)` has the stated units and
correctly exposes the information supplied by independently known length and
linear density. Reserved synthetic modes test that mathematical construction;
they do not become independent instrument measurements.

Differentiating the tension expression gives the four absolute standard-
uncertainty contributions below for the stated independent input uncertainties.

| Input | Absolute contribution to standard uncertainty of tension, N |
| --- | ---: |
| Linear density | 0.3271840000000000 |
| Length | 0.05034318769551286 |
| Fundamental frequency | 0.01487200000000000 |
| Bending rigidity | 0.0004672002083355909 |

Their root-sum-square is `0.33136867845857176 N`; density contributes about
`97.49%` of this illustrative variance. The displayed `u(T) ≈ 0.3314 N` and
rounded reporting of `32.71 N` with standard uncertainty `0.33 N` are correct.
The workflow also correctly withdraws the independence assumption if rigidity is
fitted from the same frequencies; then joint covariance is needed.

## Independent arithmetic: the four model decisions

| Example | Recomputed result | Interpretation checked |
| --- | --- | --- |
| A: amplitude decay | `sigma = ln(1000)/1.5 = 4.605170185988091 s^-1`; slope `-40 dB/s`; `zeta = 0.003331525449451944`; `Q = 150.0813989226026`; damped frequency `219.9987790998121 Hz`. | The amplitude/energy factors, oscillator-Q convention and distinction from instantaneous mechanical energy are correct. The deliberately wrong doubled amplitude coefficient produces `T60 = 0.75 s`. |
| B: body mobility | At `Omega = 1884.955592153876 rad/s`, `Y11 = 1.3262911924324612`, `Y12 = -0.6631455962162306`, `Y22 = 0.3315727981081153`, all in m/(N s). | The Hermitian eigenvalues are `0` and `1.6578639905405765 m/(N s)`. The negative cross response is consistent with a passive rank-one realization. |
| B: supplied power and compliance | For peak force `(0.001, 0) N`, mean power is `6.631455962162305e-7 W`; static driving compliance is `1.4072386616991358e-5 m/N`. | The factor `1/2` matches peak phasors. Force `(0.001, 0.002) N` lies in the fixture's null direction and performs no modal excitation. |
| C: observation | `P/F1 = i*25 Pa/N`; a peak force of `0.001 N` gives pressure amplitude `0.025 Pa`. | The sign follows `exp(i*omega*t)` and `p = beta*eta_ddot`. Pressure/force is not a work-conjugate mobility and need not be positive real. This is a continuous analytic result, not an executed discrete output. |
| D: contact potential | `K = 0.05/(0.0005^1.5) = 4472.13595499958 N/m^1.5`; `Phi = F_ref*delta_ref/(alpha+1) = 1e-5 J`. | Differentiation yields `dPhi/ddelta = K*delta^alpha`. Stored potential is distinct from actuator work and dissipation; comparing exponents requires a common force/indentation reference. |

The body example's second-order receptance numerator is
`[[50, -25], [-25, 12.5]] kg^-1`. At the positive-imaginary pole
`s_p = -18.84955592153876 + i*1884.861342017956 s^-1`, its first-order residue is
that numerator divided by `2*s_p + 2*zeta*Omega`, with units s/kg. This correctly
distinguishes two quantities that are often both loosely called modal residues.

For nonzero dimensionless `c`, the simultaneous transformations
`psi' = c*psi`, `m' = c²*m`, `eta' = eta/c`, and `beta' = c*beta` preserve
receptance, mobility and pressure transfer. Direct complex-arithmetic substitutions
at `c = 2` and `c = -1` preserved the mechanical and observation transfers.
Changing the modal mass or observation scale alone would not preserve them.

For the flexible-string loss law, `omega_n² = (T/mu)*k_n²` makes the sensitivity
columns for `a2` and `b2` exactly proportional. The examples and selected P04/P12
recipes correctly retain the identifiable combination `a2 + (T/mu)*b2` and
require a supported condition change or another observable before claiming a
unique decomposition. Positivity bounds alone supply no missing information.

## Independent check of the midpoint correction

The revised [midpoint loss chapter](../numerics/14-damped-midpoint-loss-verification.md)
was compared with `midpoint_damped_roots` in
[numerical_prototype.cpp](../../src/numerical_prototype.cpp). For the positive-
imaginary root `s = -gamma + i*omega_d`, the Cayley transform gives

\[
|r|^2=\frac{(1-a)^2+b^2}{(1+a)^2+b^2},\qquad
\arg r=\operatorname{atan2}(b,1-a)+\operatorname{atan2}(b,1+a),
\]

where `a = gamma*dt/2` and `b = omega_d*dt/2`. The code's `log1p` expression is
algebraically equivalent to the displayed modulus. The two `atan2` terms retain
the correct quadrant when `1-a` is negative.

Independently expanding `log(r) = 2*atanh(s*dt/2)` near `dt = 0` gives
`log(r) = s*dt + s³*dt³/12 + O(dt⁵)`. Taking the negative real part divided by
`dt` therefore yields

\[
d_{\rm num}=\gamma+
\frac{\gamma^3-3\gamma\omega_d^2}{12}\,dt^2+O(dt^4).
\]

The leading correction changes sign at `gamma/Omega = sqrt(3)/2` within the
underdamped domain. The text correctly restricts `gamma/(1+b²)` to the
light-damping approximation rather than asserting a universal deficit.
For its synthetic `Omega = 1 rad/s`, `gamma = 0.99 s^-1`, `dt = 0.1 s`, direct
complex mapping gives `d_num = 0.9907602810505735 s^-1`. The equivalent modulus
ratio and code-style `log1p` expression agree within floating-point roundoff.
This confirms the rounded `0.990760281 s^-1` counterexample.

For a separate analytic coarse-step example, `Omega = 1 rad/s`,
`gamma = 100 s^-1`, and `dt = 10 s` give mapped roots approximately
`0.95121832236836` and `-0.9980019480993543`. The continuous fast pole controls
the larger discrete modulus here. Thus comparing both mapped roots is necessary.
RR-01 records the distinct zero-radius critical exception and its corrected
description. These calculations use scalar formulas only; they add no solver
execution, convergence qualification or physical damping evidence.

Inspection of the chapter's working-tree diff confirmed that its historical
experimental numerical comparisons remain unchanged. The corrections address
the analytic statement, oracle boundary and interpretation of identifiability.

## Source-access and promotion boundaries

This review checked consistency of recorded access and downstream claims. It did
not repeat the external retrieval campaign or independently certify that an
earlier retrieval succeeded.

| Source | Recorded access retained | Downstream boundary checked |
| --- | --- | --- |
| Mansour 2015 | Bibliographic identity; failed direct retrieval; full text and assets uninspected. | P01 and acquisition recipes request an authorized copy and keep the accessible 2011 paper distinct. No new numerical constant is attributed to the unavailable article. |
| Raue 2009 | Publisher-indexed abstract and §2.3 excerpts; failed direct article retrieval. | P04/P12 and Example A keep uninspected methods as acquisition work. Profiling is a proposed investigation, not a newly reproduced full method or a source of specimen coefficients. |
| Bilbao 2009 book | Publisher identity/scope; full book not read in this update. | The atlas requires an authorized copy and exact section inspection before attributing a derivation. Book metadata is not promoted to a reviewed algorithm. |
| ITU-R BS.1534-3 | Official edition identity; PDF retrieval timeout. | P17 explicitly blocks method-conformance review if the needed text remains unavailable and requires declared adaptations. BS.1116 access does not establish BS.1534 review. |
| Collision preprint | Atlas's limited framework-level reading; a later retrieval attempt in the worked-example chapter failed. | Example D claims only independently differentiated contact arithmetic, not a fresh section-level review or measured nail coefficients. |

The following conceptual promotion cases were traced to actual fields and
source-level checks, without constructing fake acquired data or running importers:

- A source-access upgrade alone cannot populate `BodyMode`: the actual
  [configuration](../../include/setar/instrument/config.hpp) needs positive modal
  frequency/mass, damping convention, signed dimensionless bridge participation
  and separate observation weights. The current
  [mobility function](../../src/instrument/body_model.cpp) uses these quantities
  in the documented modal sum.
- A measured speaking length maps to `bridge_position_m`; the basis wavenumber
  in [Instrument::prepare](../../src/instrument/instrument.cpp) uses
  `total_length_m`. The reviewed guidance preserves afterlength and the coupled
  boundary distinction. Preparation stops at the first `sigma >= omega` and
  rejects an empty retained set; the direct modal coefficients reject critical
  as well as overdamped input.
- A calibrated pressure/force FRF is not itself a `radiation_weight`. There are
  two real observation coefficients per mode, indexed `0` and `1`, in Pa·s²/m.
  The engine applies a centered second difference and output gain. No current
  field supplies arbitrary complex observation filters or acoustic feedback.
- A brightness mismatch cannot identify `ContactLaw.stiffness` or its exponent.
  Current [gesture fields](../../include/setar/instrument/gesture.hpp) prescribe
  a straight stroke and force/travel release limits; they contain no physical
  patch-width, dynamic nail mass or friction-state field. The work/potential
  distinction and proposed extension boundary are accurate.
- A `review_ready` campaign channel supplies required references, not a scientific
  verdict. Inspection of [the validator](../../tools/validate_measurement_campaign.py)
  confirms its channel/status/unit-reference and raw-file integrity checks, the
  absence of acquired-file checks in its `rejected` branch, and its absence of calibration, covariance,
  passivity, rights or holdout-independence interpretation. The public
  [campaign template](../../data/measurement-campaign-template.json) remains 17
  missing channels with null identities and no raw sources; the
  [specimen template](../../data/specimen-template.json) remains unmeasured.
- Setting the coarse `Provenance` enum cannot supply per-quantity lineage or make
  a mixed package wholly measured. The workflow and promotion template require
  a sidecar, scoped acceptance, independent evidence and explicit migration or
  rollback. `Instrument::prepare()` validates an in-memory configuration and
  does not implement a measured-asset ingestion or promotion pipeline.

## Review disposition and limits

The five synthetic examples and revised midpoint algebra pass this independent
analytic check. Both reported corrections, RR-01 and RR-02, are resolved in the
inspected current text. No new source access, measured value, calibration, phase
acceptance or fidelity claim follows from this review. The historical solver
results retain their original execution scope and were not reproduced here.
Future empirical promotion still requires actual authorized observations,
uncertainty and identifiability evidence, independent predictions, and review
of the resulting field mappings.
