# From research to a physical-model change

## Scope and evidence boundary

This workflow turns a physical question into a reviewable finding, a model decision,
and an executable work order. It specifies future work; it reports no new instrument
measurement and does not implement a parameter importer or pass a phase. The current
engine remains a synthetic prototype. Read the [evidence ledger](evidence-ledger.md),
[data contract](../measurement/data-contracts-and-provenance.md),
[identifiability protocol](parameter-identifiability.md), and [source atlas](source-atlas.md).
Use the [finding template](../../templates/research-finding.md) and
[promotion template](../../templates/parameter-promotion.md).

The chain is: physical question → primary source → exact extraction → applicability
and uncertainty → discriminating experiment → scoped finding → equation/interface
change → independent verification → held-out prediction → versioned promotion.
A negative result is useful: rejecting an inadmissible material table or showing
that two loss mechanisms cannot yet be separated prevents false interpretations.

## 1. Frame the question and decision

Choose one observable, controlled change, and decision. For example: “For specimen
S in configuration C, does isolated-pluck amplitude shift early partial frequencies
beyond estimator uncertainty, and does that justify tension modulation?” Record
string/tuning, length, support, pluck position, amplitude range, environment,
frequency-estimator window, source commit, current model, and acceptance rule.
Fix the decision rule before inspecting decisive data.

List alternatives: actual tension rise, moving contact, termination changes,
polarization beating, and estimator bias. Link the existing requirement and work
order; this example can inform `REQ-STR-004` and `ENG-P04-T02`. Use the
[phase-task crosswalk](../../roadmap/phase-task-crosswalk.json): `P04-T02` and
`ENG-P04-T02` are different namespaces, not automatically the same task.
Classify prerequisites as checked, unaudited, requested, or missing. Without an
instrument, a synthetic estimator-bias test remains useful; a conclusion about
that instrument's actual pitch glide remains blocked.

## 2. Search and read with a stopping rule

Start with setar-specific primary sources in the atlas and ledger, then inspect
their methodological references. Adjacent-instrument literature may support a
mechanism or method without providing this specimen's coefficients. Log query,
site/database, date, source selected, inclusion/exclusion reason, and access
actually achieved. Search instrument plus mechanism, then mechanism independently.
Follow exact author/DOI identity to publisher or author repositories. An abstract
identifies a candidate; it does not establish a methods review.

Read in order: specimen/boundaries; apparatus/calibration; definitions/equations;
identification; results/uncertainty; limitations; supplementary assets. Follow a
decisive borrowed value to its original source. If unavailable, retain unresolved
provenance rather than upgrading an assumption to measurement. An access failure
is not evidence that the needed data do not exist.

For performer knowledge, use the [elicitation protocol](../ethnomusicology/elicitation-and-terminology.md).
Separate description, observed gesture, and engineering interpretation. Preserve
practitioner disagreement as variation. Preparing an asset/interview request is
separate from sending it, which needs appropriate authorization.

Stop searching when an inspected method, applicability assessment, counterevidence,
and remaining measurement/implementation gap support the next decision. If the
critical asset is missing, deliver a precise request and independent synthetic
work, rather than reading indefinitely or inventing parameters.

## 3. Extract claims with their physical meaning

Create one finding per proposition. Record source ID and exact inspected section,
equation, figure, or table. Preserve original notation/value, conversion expression,
converted SI quantity, and interpretation separately. A bridge compliance in m/N,
mobility in m/(N·s), impedance in N·s/m, and uncalibrated sensor ratio cannot share
an unexplained “response” field. Magnitude alone omits phase needed for coupling.

| Extraction field | Required distinction |
| --- | --- |
| Origin | Author-measured, fitted, assumed, borrowed, or project-digitized. |
| Configuration | Specimen, tuning, support/player contact, sensor attachment, input/output ports. |
| Convention | Unit, tensor axis, sign, angular/cyclic frequency, amplitude/energy. |
| Resolution | Sampling/window/bandwidth, repeats, fit interval, figure precision. |
| Uncertainty | Interval meaning, covariance/shared calibration, omitted systematic terms. |
| Applicability | Transferable method, provisional hypothesis, nontransferable specimen value. |

Require an independent extraction for decisive stability/admissibility/fidelity
values. Resolve disagreement against original definitions, not by averaging.
Use the [published-parameter audit](published-parameter-audit.md) for suspect
elasticity. Source access and claim support remain separate; full text does not
prove specimen applicability, and article rights do not automatically cover raw assets.

## 4. Preserve evidence before fitting

Follow specimen → configuration → session → trial → raw asset → derived asset →
result. Transformations record input hashes, analysis revision, arguments,
environment, and output hashes. Raw files remain unchanged; recalibration produces
a new derived version. Restricted identity/permission records stay separate from
public analysis. Check channel signs/units, calibration, clocks, clipping,
synchronization, support changes, and valid frequency range at ingestion.
Preserve excluded trials and reasons where retention permits.

An uncertainty record specifies a measurement function, input quantities,
correlations, and interval meanings. Use the official
[JCGM publication index](https://www.bipm.org/en/committees/jc/jcgm/publications)
to identify the GUM edition and applicable amendments used. This does not claim
accredited calibration. Keep repeatability, calibration, specimen variation,
numerical error, and model discrepancy separate. Repeated biased readings do not
remove bias; repeating one mesh does not demonstrate convergence.

## 5. Separate explanations experimentally

These are proposed discriminating designs, not completed measurements:

| Mismatch | Alternatives | Discriminating action | Resulting model decision |
| --- | --- | --- | --- |
| Upper partials sharp | Bending stiffness, termination, length, estimator bias | Multiple partials across known lengths/tunings; synthetic estimator check; termination sensitivity. | Revise dispersion/termination rather than arbitrary per-mode shifts. |
| Decay too fast | String loss, bridge radiation, body damping, finger contact | Mechanical/acoustic channels with support/contact controlled. | Assign loss to its supported subsystem without double counting. |
| Attack too bright | Contact duration, position, nail geometry, observation | Synchronized contact/motion and isolated-string evidence. | Change contact only after observation/body alternatives are checked. |
| Sympathy too weak | Cross-mobility, detuning, course map, observation cancellation | Mechanical transfer ports and alternative tuning/observation. | Change coupling/configuration rather than unexplained output gain. |
| Body modes shift | Thickness, elasticity, mass, support/preload | Independent thickness/mass and support comparison. | Constrain geometry/materials before adding fit freedom. |

Freeze partitions by meaningful experimental units. Repeated takes from one
session, overlapping windows, and reprocessed versions remain together. Data
used to choose a model become development data; preserve a new independent
partition or narrow the claim after reuse. Record hypotheses that failed and
why, rather than retaining only the optimizer's best run.

## 6. Classify the finding and its consequence

Outcomes are accepted for stated scope, rejected, inconclusive, or blocked. Map
these explicitly to existing claim vocabulary: a scoped accepted finding may be
`supported_with_scope`; inconclusive/blocked claims remain `unresolved`. Neither
changes source-access status nor completes a phase automatically.

| Consequence | Required handoff |
| --- | --- |
| No change | Evidence current mechanism is adequate within stated domain/budget. |
| Configuration | Values/distributions, units, covariance, specimen, field mapping, validation. |
| Equation | Assumptions, states, energy/work, limits, independent oracle, validity. |
| Numerical method | Discrete scheme, convergence/stability, failure handling, fixed-physics comparison. |
| Observation | Mechanical-state-to-signal mapping, calibration/location, separation from feedback. |
| New measurement | Ranked ambiguity, minimum added conditions/channels, decision they resolve. |

Inspect residuals by frequency, amplitude, direction, support, and session. If
all admissible candidates fail similarly, investigate missing physics or
calibration before adding optimizer freedom. Do not mask a failed mechanism with
an unexplained gain/EQ correction while retaining the original physical claim.

## 7. Translate findings into current C++ interfaces

Inspect source before implementation. The engineering register includes proposed
future paths as well as actual implementation-evidence paths. Do not create a
duplicate engine just to match a proposed filename.

| Research output | Current target | Integration constraint |
| --- | --- | --- |
| String properties | `StringConfig` in [config.hpp](../../include/setar/instrument/config.hpp) | `total_length_m` is nut-to-tail; `bridge_position_m` is speaking length. Include afterlength/bridge interaction. |
| Modal amplitude loss | `StringLossLaw` in the same header | Coefficients have units 1/s, m²/s, and s for constant, wavenumber-squared, and angular-frequency-squared terms. Convert dB/s explicitly. |
| Body poles and residues | `BodyMode` in the same header | Frequencies/damping alone lack mass normalization and bridge residues. `radiation_weight` maps acceleration to observation, not acoustic feedback. |
| Contact law | `ContactLaw`, [numerics.cpp](../../src/instrument/numerics.cpp), [solver derivation](../numerics/15-modal-contact-solver.md) | Exponent changes stiffness units to N/m^alpha. Derive potential and work. |
| Isolated string oracle | `StringParameters` in [reference.hpp](../../include/setar/reference.hpp) | `length_m` is the chosen complete simply-supported span. A speaking-span comparison uses `bridge_position_m`; an uncoupled nut-to-tail basis comparison uses `total_length_m`. Neither alone includes the internal bridge's effect on coupled modes. |
| Gesture meaning | [gesture.hpp](../../include/setar/instrument/gesture.hpp), [performance.cpp](../../src/instrument/performance.cpp) | Timed interactions on persistent strings; do not reset ringing state to match attacks. |

The coarse `Provenance` enum cannot encode per-quantity uncertainty, mixed
evidence, or fitting lineage. Keep a versioned sidecar under the data contract.
One measured length cannot justify labeling a synthetic body/contact package
`Measured`; establish a reviewed mixed-package import/label policy first.

Name existing symbols, proposed changes, and independent checks in each work
order. Inspect test assertions before citing coverage: `preset.tension_inverse`
checks a synthetic calculation, not tension inferred from an actual instrument.
Measured-data validation needs its own dataset, criterion, and scope.

## 8. Worked example: synthetic stiffness and tension

### Question and oracle

**Every value in this example is synthetic.** No string was measured or corpus
ingested. The arithmetic illustrates future analysis/verification under
`REQ-STR-003`, `REQ-IDN-001`, `ENG-P04-T01`, and `ENG-P12-T01`.
The question is whether observations distinguish stiffness from tension and which
fields would receive the result. Substitute sine modes into the repository's
[continuous equation](../mathematics/01-string-continuum-and-power.md):

\[
\omega_n^2=(T/\mu)(n\pi/L)^2+(EI/\mu)(n\pi/L)^4.
\]

Julius O. Smith's author-written [The Stiff String](https://www.dsprelated.com/freebooks/pasp/Stiff_String.html)
also explains the stiff-string equation and dispersion; it supplies none of this
example's parameter values. Assume a uniform small-displacement string with
fixed simply-supported ends, positive density/tension, nonnegative rigidity,
and no contacts/body. `f_n = omega_n/(2*pi)` denotes **undamped** frequency.
Damped sinusoidal frequency and FRF peak frequency require separate observation
models and cannot be substituted silently.

Choose `L = 0.6500 m`, `mu = 0.000400 kg/m`, `EI = 0.000200 N*m²`, and an undamped
fundamental of `220 Hz`. Derive the tension:

\[
T=4\mu L^2f_1^2-EI\pi^2/L^2=32.7137279979\ \mathrm N.
\]

| Mode | Synthetic frequency (Hz) | Demonstration role |
| --- | ---: | --- |
| 1 | 220.000000 | Fit input |
| 2 | 440.094234 | Fit input |
| 3 | 660.376869 | Fit input |
| 4 | 880.941938 | Reserved synthetic prediction |
| 6 | 1323.294438 | Reserved synthetic prediction |
| 8 | 1767.898793 | Reserved synthetic prediction |

Extra digits support arithmetic checks, not measurement precision. A future
fixture must retain its analytic generator/revision and synthetic identity.

### Identifiability and fitting

One frequency constrains a combination of `T`, `mu`, `L`, and `EI`; it cannot
identify all four. With independently known length/density, define:

\[
z_n=(2Lf_n/n)^2=a+b n^2,\qquad
 a=T/\mu,\qquad b=EI\pi^2/(\mu L^2).
\]

Regress `z_n` against `n²` and recover `T = mu*a` and `EI = mu*L²*b/pi²`.
`z_n` and its intercept have units m²/s². In actual measurements, propagate
length, density, frequency, and mode-assignment uncertainty jointly: shared
length/calibration induces correlations. Unweighted least squares is only a toy
starting point here. Compare a tension-only (`b=0`) candidate with stiffness,
checking resolved slope and reserved prediction improvement. A negative
unconstrained slope is a diagnostic for noise/wrong modes/model discrepancy;
retain it and investigate rather than silently clamp and claim measurement.

### Uncertainty calculation

For a separate illustrative calculation assign independent standard uncertainties
`u(L)=0.0005 m`, `u(mu)=0.000004 kg/m`, `u(f_1)=0.05 Hz`, and
`u(EI)=0.000020 N*m²`. These are chosen inputs, not laboratory estimates.
First-order propagation for the tension equation gives:

\[
u^2(T)=(4L^2f_1^2u(\mu))^2+
[(8\mu Lf_1^2+2EI\pi^2/L^3)u(L)]^2+
(8\mu L^2f_1u(f_1))^2+[\pi^2u(EI)/L^2]^2.
\]

The result is approximately `u(T)=0.3314 N`; density dominates this illustrative
budget. Report about `32.71 N` with standard uncertainty `0.33 N`, under these
assumptions. This calculation assumes an independently supplied rigidity. If
rigidity was fitted from the same frequencies, use joint covariance instead of
treating it as an independent uncertain input.

### Implementation and empirical handoff

1. Build a separate synthetic recovery fixture with known parameters, controlled
   noise, a single-mode underdetermined case, and a wrong-mode negative case.
2. Recover parameters with analysis independent of the forward engine; inspect
   sensitivity and uncertainty before accepting recovery.
3. Populate the isolated reference `StringParameters.length_m`, `tension_n`,
   `linear_density_kg_per_m`, and `bending_stiffness_n_m2`. Specify loss, mode count,
   and sampling separately; retain the synthetic label.
4. Compare analytic frequencies and decay conventions with the implementation
   over appropriate resolution/truncation refinements.
5. Before transferring to the full instrument, establish `total_length_m`,
   `bridge_position_m`, bridge compliance, and afterlength. Coupled eigenfrequencies
   need not equal the isolated formula.
6. Preserve the existing synthetic preset for regression. A measured specimen
   package receives its own identity, version, and evidence record.

Reserved modes here test a synthetic analysis pipeline, not empirical
independence. Real validation requires held-out conditions such as another
acquisition session, tuning, or measured speaking length, with boundary state
recorded. Failure prompts investigation of termination, polarization, or
identification before adding unexplained corrections.

Only after the proposed checks actually run may the result say: “The estimator
recovers the declared synthetic fixture within tolerance and recognizes the
single-mode case as underdetermined.” It cannot say that setar strings have been
measured or validated. Until execution, even the narrower statement is an
acceptance criterion, not an observed result.

## 9. Promote and hand off without losing limits

A promotion packet includes finding IDs, source/raw lineage, model/analysis
versions, values/covariance, units, domain, verification/validation reports,
requirement/work-order links, old/new configuration IDs, reviewer decision, and
rollback reference. Checks name actual commands/results/artifacts, not only
planned test names. Version changes to calibration, geometry, notation, units,
normalization, meaning, or valid domain; mark downstream fits/reports stale and
recompute affected evidence. Preset meaning changes need migration or rejection.

A numerical implementation can pass a synthetic verification gate while acoustic
validation remains blocked. An admissible measured value can describe one specimen
without supporting a class-wide preset. Keep those outcomes separate.

End each task with the supported decision, negative results, inspected/acquired
evidence, exact proposed files/symbols, executed versus planned checks, unresolved
assumptions, smallest next experiment, and still-open phase gate. Use
`ENG-P01-T01` for source audit, domain work orders for mechanisms, and
`ENG-P12-T01`/`ENG-P12-T02` for identifiability/calibration where applicable.
When data/full text are missing, deliver a precise request plus independent
synthetic work. Leave unknown values null. The handoff must remain understandable
without private conversation history.
