# Test taxonomy, oracles, and error budgets

One elaborate render can hide cancellation between incorrect excitation,
incorrect bridge response, and incorrect output equalization. Divide evidence
into interpretable tests so failure suggests a useful next experiment. Inputs
are a mathematical model, declared observable, and parameter domain. Outputs are
a test inventory, error budget, and acceptance records with explicit threshold
status. This chapter supplies proposed project procedures, not measured limits.

## Choose the oracle first

An oracle is the independent basis for deciding correctness. An isolated linear
mode may have an analytic solution. Contact may need a converged offline solver
with a different numerical structure. Event scheduling may use a deliberately
simple semantics model. Screenshots need layout constraints as well as reviewed
images. For a real setar, the reference is an experimental observation with
uncertainty, not a single exact waveform.

A second wrapper around production code is not independent. Shared coefficient
formulas and indexing bugs can make two implementations agree incorrectly.
Independence may come from another derivation, discretization, symbolic solution,
analytic limiting case, or measurement. State the kind and shared code. Reference
implementations may be slow and high precision; they should be easy to inspect.

| Family | Setar-project example | Main diagnostic value |
| --- | --- | --- |
| Dimensions | Tension, line density, and wave speed have compatible units | Mapping defects |
| Analytic case | Isolated mode has stated frequency | Algebra and stepping |
| Invariant | Unforced passive subsystem cannot create energy | Signs and coupling |
| Convergence | Error decreases with refinement | Discretization |
| Metamorphic | Block partitions preserve an event stream | Scheduler state |
| Adversarial | Malformed state and event bursts stay bounded | Robustness |
| Regression | Fixed fret-transition defect stays fixed | Reintroduction |
| Empirical | Held-out decay trajectories agree within uncertainty | Model adequacy |
| Perceptual | Controlled stimuli reveal audible differences | Salient mismatch |
| Product | Reopened session restores tuning and parameter identity | Host integration |

Numerical features need multiple families. Energy tests miss a stable solver with
wrong pitch. Frequency tests miss a boundary injecting a little energy at every
contact. Comparing only with yesterday's implementation cannot establish
convergence when yesterday contained the same bias.

## Budget uncertainty by origin

For observable (q), separate model discrepancy, parameter uncertainty,
measurement uncertainty, discretization error, floating-point effects, and
feature-extraction uncertainty. They are not automatically independent random
variables. Do not take a root-sum-square merely because it looks scientific.
With unknown correlation, use a conservative bound or sensitivity analysis over
dependence assumptions. Frequency and decay fitted from one short record can
be correlated.

Numerical uncertainty should be smaller than the physical difference being
resolved. If shifting the extraction window moves frequency by two cents, a
claimed half-cent improvement is not credible. If pluck amplitude varies greatly
between repeats, one normalized take cannot establish force-to-loudness behavior.
Improve the experiment or weaken the claim, rather than add decimal places.

Use absolute and relative tolerances where meaningful. Near zero, use an
absolute floor in physical units. A decay-rate ratio can be more interpretable
than subtracting times, provided both estimates have adequate signal-to-noise.
For onset timing, distinguish seconds from samples and state sample-rate behavior.
Dimensionless error still needs a declared normalization.

## Threshold registry

Every threshold has a value, units, domain, rationale, status, revision, and
evidence identifier. Initially it is proposed. Reference precision and
sensitivity can justify a numerical regression tolerance; they do not establish
a perceptual threshold. Perceptual relevance needs an experiment whose stimulus,
playback, listener population, and uncertainty match the intended claim.

For illustration, a team could propose a one-cent isolated-solver pitch budget,
a one-decibel envelope comparison band in a particular measurement regime, or a
subpixel layout tolerance with a fixed rasterizer. These are candidate choices,
not established project facts or automatic defaults. A reviewer must assess their
rationale and whether measurements can resolve them before adoption.

Strict isolated-solver tolerances can coexist with wider empirical intervals
reflecting specimen variation. A musician-facing tuning display may round more
coarsely than internal analysis. Display resolution is not tolerance, and neither
is measurement accuracy. Preserve threshold revisions with runs so future policy
changes do not rewrite history.

## Convergence procedure

Fix physical parameters and excitation in physical units. Select a reference
quantity: modal frequency, integrated loss, bridge impulse, or trajectory norm.
Refine time and space independently where allowed, then together under the
stability condition. Resample outputs to a common observation grid after solving.
Do not retune damping or material parameters to make a coarse solution match.

Use enough levels to distinguish a trend from an accidental crossing. Plot error
against refinement with axes appropriate to the claimed order. For contact,
analyze event times and energy changes as well as waveform error. A small timing
shift can dominate a samplewise norm. Reduced order at a transition may follow
from reduced regularity; investigate that possibility without using “nonlinear”
as a blanket excuse for nonconvergence.

Stop when rounding, reference uncertainty, or extraction noise dominates and
label the floor. Extrapolation outside the resolved regime requires justification.
Preserve internal-step counts and parameter settings so speed claims can be read
alongside errors. Show unstable boundaries when they inform the admissible domain,
not only a handpicked stable operating point.

## Meaningful generated cases

Randomized cases should sample physically admissible regions, including boundary
cases. Uniform raw-parameter sampling is often poor. Logarithmic loss ranges,
correlated geometry, and stability constraints need explicit samplers. Store the
algorithm, seed, and generated case: a library upgrade can change a seed's meaning.

Properties express assumptions. Passive energy cannot rise without external work,
but moving fingers and changing tension can perform work. Unconditional monotonic
energy would reject legitimate physics. Linear sign symmetry is broken by
unilateral contact. Tag each property with its valid regime, and test invalid-
input rejection separately from admissible behavior.

Minimize failures without deleting the defining mechanism. Removing every contact
event can hide the bug by removing its cause. Keep the parent fixture identifier
and explain reduced dimensions. Where practical, introduce a known-bad mutant to
demonstrate that the test detects its stated defect. A test that passes both
correct and deliberately incorrect implementations provides little confidence.

## Composite scores and acceptance

Composite acoustic scores help rank candidates but are dangerous sole gates.
Better steady-state spectrum can hide worse attack. Report components, physical
meaning, weights, and distributions across stimuli. Hard guards protect safety
and essential behavior; quality measures show tradeoffs. An average cannot excuse
a stuck drone, broken note-off, or unstable fret transition.

Freeze weights before held-out evaluation. Version newly introduced metrics and
rerun comparisons transparently. Keep defective candidates for sensitivity tests:
wrong tension, excessive damping, absent coupling, wrong radiation view, and
quantized gesture timing. A metric unable to detect its target defect is not
ready to enforce it.

Acceptance artifacts are the family map, independence note, budget table,
threshold registry, raw results, convergence plots, and counterexamples. A
reviewer must distinguish proposed thresholds, numerically justified thresholds,
and empirical thresholds. Complete one observable's budget end to end before
adding dozens of unexplained assertions to a render comparison.
