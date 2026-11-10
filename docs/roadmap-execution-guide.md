# Execute the roadmap: from research to an instrument change

Use this operating guide with the [20-phase roadmap](ROADMAP.md), one phase's
`Pxx-Tyy` task, its corresponding `ENG-Pxx-Tyy` work order, and the relevant
technical chapters. It does not change any scientific status. The existing
prototype uses synthetic parameters; better instructions are not new physical
measurement or validation evidence.

## Choose the next result, then choose the task

Start with an observable that is wrong, unknown, or unqualified. “More realistic”
must become a question such as: does partial decay vary with amplitude after
support and microphone effects are controlled? “Playable” must become a question
such as: which contact configurations exceed the block deadline, and which
approximation changes their physical prediction least?

Use the [baseline audit](engineering/2026-11-10-baseline-audit.md) to distinguish
existing code from proposed interfaces. Select the smallest task that can resolve
the question. The [phase-task crosswalk](../roadmap/phase-task-crosswalk.json)
maps all 119 scientific tasks to engineering work orders. `Pxx-Tyy` and
`ENG-Pxx-Tyy` are different namespaces, even when the numbers look similar.

| Immediate question | Start here | First reviewable result |
| --- | --- | --- |
| Which instrument and behavior are promised? | P00–P01 | Specimen/gesture/observation claim table, unknowns, exclusions |
| Which source contains a usable method or input? | P01, [source atlas](research/source-atlas.md) | Exact source location and applicability decision |
| Is the numerical result correct on a synthetic fixture? | P02–P04 | Independent oracle, error curve, work balance, validity domain |
| Why are attacks, stopping, releases, or buzzing wrong? | P05–P06 | Controlled contact experiment or discriminating synthetic fixture |
| What should replace the placeholder body? | P07–P09 | Geometry/material/FRF package with coordinates, uncertainty, and ports |
| How does motion become microphone pressure? | P10 | Acoustic observation model separated from mechanical feedback |
| How does a player's gesture change persistent state? | P11 | Reviewed event mapping and repeatable transition trace |
| Which parameters are identifiable? | P12 | Sensitivity decision and held-out prediction report |
| Which approximation meets the deadline? | P13 | Matched reference-error and deadline results on a frozen corpus |
| Does the engine survive host and user interaction? | P14–P16 | Binary-identified host/platform evidence packet |
| Does it reproduce the promised behavior? | P17 | Preregistered acoustic, expressive, and perceptual results |
| Can the exact artifact be distributed and maintained? | P18–P19 | Reconstructable provenance, permitted assets, migrations, regressions |

## Read every execution recipe as a physical argument

The phase chapters retain their original reasoning and add task-specific
execution recipes. Read the reasoning first: a checklist cannot repair an
incorrect assumption. Each recipe must resolve four practical questions.

1. **Question and research route:** what is falsifiable, which primary source or
   official contract should be inspected, which search terms help, and which
   experiment resolves missing evidence? Research ends with a decision or a
   named evidence gap, not a pile of references.
2. **Inputs and procedure:** which specimen/setup, raw records, calibration,
   coordinates, units, controlled variables, and ordered operations are needed?
   For numerical work, freeze boundaries, state, solver settings, and an
   independent expected answer.
3. **Finding and model translation:** does the finding change an equation term,
   contact law, physical parameter, observation operator, gesture contract,
   numerical approximation, or claim? Identify existing code and proposed
   replacements separately.
4. **Acceptance and handoff:** what observable, comparator, tolerance rationale,
   evidence package, rejecting counterexample, and downstream consumer determine
   the result? A rejected hypothesis can resolve a task without passing its phase.

The [research-to-model workflow](research/research-to-model-workflow.md) gives the
complete pipeline and a worked synthetic example. The
[worked model decisions](research/model-decision-examples.md) extend it with decay,
passive body response, observation/loading and nail-contact examples, including
calculations, competing explanations and explicit dispositions. The
[acquisition campaign](measurement/13-acquisition-campaign-execution.md) maps all
17 measurement packets to current fields or proposed physical-model extensions.
Record results using the
[research-finding template](../templates/research-finding.md) and use the
[parameter-promotion template](../templates/parameter-promotion.md) before moving
a quantity into an asset. Preserve exact citations and original reasoning;
do not copy papers into work records.

## Where research happens and how it stops

Begin with the [evidence ledger](research/evidence-ledger.md) and source atlas.
For instrument claims, prioritize original experiments and identified
practitioners. For mathematics, use the original derivation or author manuscript.
For a host/build contract, use official documentation for the pinned version.
Record the sections actually read. An abstract can identify a candidate paper;
it cannot establish an unread procedure, coefficient, or dataset license.

Search backward through references for original methods and forward through
citing work for corrections and limitations. Include spelling variants and
Persian terminology where useful, while keeping Iranian setar distinct from
Indian sitar. Broaden to other plucked instruments for transferable methods;
a guitar experiment can motivate a decay-identification procedure without
providing measured setar losses.

Write the decision that evidence must resolve before searching. Stop when an
admissible input answers it or the search log establishes a bounded unresolved
gap. Record queried repositories, terms, access failures, alternatives, and the
next acquisition action. “Not found” does not mean “does not exist.” Prepare
precise data requests when useful; sending them needs contact authorization.

If sources disagree, retain both statements and compare specimen, setup,
definition, units, band, and uncertainty. Do not average incompatible quantities.
Quarantine physically inconsistent inputs. The [published-parameter audit](research/published-parameter-audit.md)
demonstrates why plausible wood properties still require reciprocal and
positive-energy checks.

## Phase handoffs: what the next developer receives

These are required future outputs, not a list of acquired assets.

| Phase | Research or experiment | Translation into the instrument | Reviewable handoff |
| --- | --- | --- | --- |
| P00 | Specimen, musical use, supported conditions | Model domain and permitted claims | Charter, exclusions, observables, reviewers |
| P01 | Sources and missing data | Admitted, quarantined, or deferred inputs | Source/finding records, acquisition plan, split policy |
| P02 | Mathematical cases and portable tools | Independently testable mechanisms | Oracle, command, environment, artifact manifest |
| P03 | String boundaries and discretization | Verified motion and force/velocity representation | Equations, convergence, work balance, invalid cases |
| P04 | Dispersion, loss, polarization, tension change | Separately identifiable string mechanisms | Parameter evidence, limits, coupled-state tests |
| P05 | Nail motion, compliance, friction, release | Stroke-to-contact force and transferred work | Contact law, force/travel traces, attack comparisons |
| P06 | Finger/fret contact, afterlength, damping | Moving constraints on persistent state | Setup map, transition traces, energy/residual evidence |
| P07 | Geometry, thickness, grain, materials | Coherent geometric/constitutive fields | Coordinate-aware geometry and uncertainty package |
| P08 | Modes, FRFs, supports, reduction | Justified body representation | Modal or mass/stiffness asset, damping, port residues |
| P09 | Bridge driving/cross-port mobility | Reciprocal string/body interaction | Signed ports, passive coupling, sympathetic response |
| P10 | Cavity, radiation, directivity, microphones | Acoustic feedback and observation | Boundary definition, loading model, calibrated output |
| P11 | Tunings, courses, strokes, ornaments | Gestures applied to persistent strings | Reviewed terminology, event semantics, sequence fixtures |
| P12 | Confounding, fitting, generalization | Supported parameters/model complexity | Split manifest, fit diagnostics, uncertainty/residuals |
| P13 | Reduction and worst-case cost | Bounded engine with quantified error | Reference comparison, capacity policy, deadline distributions |
| P14 | Host lifecycle, timing, state | Plugin around the same engine | Exact binary, validator/host/event/state records |
| P15 | Musician tasks and observability | Meaningful controls and diagnostics | Usability, unit mappings, accessibility, visuals |
| P16 | Platform/host/hardware stress | Support for tested configurations | Exact-source matrix, artifacts, failures, exclusions |
| P17 | Held-out acoustic/perceptual evaluation | Bounded fidelity claims | Preregistered design, effects, uncertainty, claim decision |
| P18 | Rights, reproducibility, packaging | Distributable qualified artifact | Provenance, notices, package checks, release decision |
| P19 | Drift, migration, new hypotheses | Evidence-preserving evolution | Impact analysis, versioned assets, regression, rollback |

## Parallel work and current priorities

The dependency graph specifies evidence needed to pass gates. Independent
synthetic verification, profiling, host experiments, UI research, acquisition
planning, and test design can proceed while specimen access is unavailable.
Keep their results separate from a claim about an actual instrument.

For each task distinguish **prerequisites for doing the work** from
**prerequisites for promoting its claim**. A synthetic fixture permits event-order
tests but cannot certify playing technique. A measured bridge FRF may support
a passive response model while leaving wood constants unidentified. An
allocation-hook test establishes only the paths that its instrumentation covers;
it leaves other allocation paths, locks, hosts, and deadlines unqualified.

### Concrete starting packages at the audited revision

Choose one row as a bounded work package. These are proposed next results,
selected from the inspected synthetic prototype and its open evidence. Recheck
the current source and register before assigning them; later work may close or
change a gap. The scientific phase names alone do not determine whether work can
start.

| Start with these tasks | Available starting point | First deliverable and decision | Effect on the instrument or its claims |
| --- | --- | --- | --- |
| P00-T01/T05, P01-T03/T06 | The charter recipes, specimen completeness matrix, and blank 17-channel campaign template | A specimen/gesture/observation scope, access and calibration inventory, grouped split plan, and a small repeatability pilot specification. If access is missing, identify the exact owner, missing permission or apparatus, and next admissible acquisition step. | Defines which physical instrument and playing conditions future parameters represent; supplies no measured coefficients until the acquisition is actually performed. |
| P03-T03/T04 and P09-T02/T06 | Present reference string, modal-contact derivation, synthetic body and partial coupling checks | An independently derived small connected fixture, its signed force/velocity and connector-storage balance, a convergence/comparison plan, and a deliberately wrong-sign or delayed-port control. Retain a failing comparison as a numerical defect report. | Tests whether connecting otherwise plausible subsystems changes motion and work correctly; does not establish that the synthetic body matches a setar. |
| P12-T01/T03/T04 | Known synthetic parameter configurations and explicit current loss/observation equations | A sensitivity/identifiability report, a synthetic recovery design with nuisance variables, and acquisition groups reserved before fitting. If two parameters are confounded, fix one from independent evidence or reduce the parameterization before collecting more of the same observable. | Determines which model coefficients a future experiment can support and prevents one mechanism from compensating for another. |
| P13-T01/T03/T04 | Existing benchmark, callback paths, allocation-hook checks and historical dense-pattern timing failures | An exact-source workload/capacity inventory and controlled-hardware timing report, paired with a frozen reference-error corpus. Attribute expensive work before proposing a reduction. A missed deadline remains a failed workload, even if average CPU is low. | Produces an explicit supported performance envelope and a justified optimization target; a faster result needs renewed physical/numerical comparison. |
| P14-T01/T02/T05 | Optional JUCE adapter, lifecycle/event recipes and planned host matrix | A reproducible optional-plugin build and exact-binary validator/host packet covering timing, state, buses and lifecycle. Record source-inspected concerns separately from observed failures; retain absent tools/hosts as unexecuted matrix cells. | Establishes which wrapper contracts and host combinations have evidence; successful headless tests alone leave these claims open. |

Within a row, prerequisites can be an output of the same bounded investigation.
For example, write and review a synthetic fixture contract before implementing
its independent comparator. Do not wait for a physical specimen to test an
algebraic work identity; do not substitute that result for the unavailable
specimen experiment. Route any proposed implementation to the exact work order
in the [crosswalk](../roadmap/phase-task-crosswalk.json), whose dependencies and
acceptance scope still apply.

These streams can proceed together with non-overlapping ownership. Their plans
do not authorize participant contact, purchases, invasive specimen changes,
public releases, or repository permission changes.

## Leave a usable handoff

End with the answered question, actual procedure, observations, uncertainty,
decision, changed model meaning, and exact next task. Attach source/configuration
hashes to derived artifacts. Retain failed trials and reasons for exclusions.
If a finding invalidates a parameter, identify every affected asset, preset,
test, and claim before replacement.

Run the existing structural audits after editing:

```sh
python3 tools/audit_docs.py
python3 tools/audit_implementation.py
python3 tools/audit_executable_coverage.py
```

They check local file links, registry consistency, and executable-test mappings
within their implemented scope. They do not establish scientific completion;
the phase gate still needs its specific evidence and review.

The [documentation expansion record](reviews/2026-11-10-documentation-expansion.md)
links the independent integrity, mathematical and contributor-walkthrough reviews
for these instructions. Their acceptance concerns the documented execution path;
each instrument claim retains its own scientific or product gate.
