# P19 — Maintain scientific reproducibility and long-lived instruments

**Status:** specification. **Prerequisites:** an approved P18 release. **Exit
evidence:** a sustainable procedure for regression triage, preserved evidence,
safe migrations, supported maintenance, and revalidation after significant
physical-model changes.

## A model remains an evolving scientific artifact

Measured specimens change with environment and setup. Software platforms and
plugin hosts evolve. New evidence may invalidate a parameter assumption while
the old code still compiles. Maintenance therefore has two responsibilities:
keep deployed artifacts reliable and update the underlying physical claims
honestly. Read [validation](../validation/README.md),
[research](../research/README.md),
[engineering](../engineering/README.md), and
[decisions](../decisions/README.md).

The roadmap has no calendar milestones. Maintenance actions are triggered by
new evidence, defects, platform changes, dependency vulnerabilities, or user
needs. A lack of a date is not permission to omit ownership or acceptance rules.

### Phase entry and evidence map

An approved P18 release remains this phase's prerequisite. The procedures can
be rehearsed against the present synthetic prototype without implying that a
product release, measured corpus or maintenance service exists. Start each
change with the P18 source/dependency/model/state identity, supported domain,
rights decisions and unresolved claims. Preserve the synthetic fixture while
measurement acquisition is blocked.

| Investigation | Research route and current boundary |
| --- | --- |
| Numerical or physical regression | `R-NUM-BILBAO-SMITH-2005` and `R-CONTACT-BILBAO-2014` in the [source atlas](../research/source-atlas.md); compare their assumptions with [the current modal-contact solver](../numerics/15-modal-contact-solver.md). A method proof does not certify a changed solver. |
| Changed data or calibration | `R-METROLOGY-JCGM-100`, the [data contract](../measurement/data-contracts-and-provenance.md) and [corpus partitions](../validation/02-reference-corpus-provenance.md). Existing metadata checks are narrower than calibrated physical validation. |
| Host, state or dependency change | `R-SDK-JUCE-AUDIOPROCESSOR` / `R-SDK-VST3-VALIDATOR`, exact-version official source, and [adapter limits](../plugin/08-prototype-juce-adapter.md). Headless CTests do not establish host compatibility. |
| Access, consent or claim withdrawal | `R-ETHICS-OHA` / `R-RIGHTS-WIPO-TCE` in the [evidence ledger](../research/evidence-ledger.md), [participant rights](../ethnomusicology/participants-consent-and-rights.md) and actual agreements; guidance is not permission. |

Use the [crosswalk](../../roadmap/phase-task-crosswalk.json) for work-order
identity and [coverage registry](../../roadmap/executable-test-coverage.json)
for current versus planned checks. Every handoff retains the observation,
raw/derived lineage where permitted, inspected source location, alternative
explanations, accepted/rejected/inconclusive/blocked decision and next owner.
These recipes describe future investigation; no runtime failure, migration,
dependency update or withdrawal operation is executed by this documentation.

## P19-T01: classify every reported issue

Start with a reproducible observation: build/version, platform/host, physical
asset, event sequence, sample rate, state, expected outcome, actual outcome,
and permission-safe evidence. Classify whether the issue is numerical,
model-physical, calibration, performance mapping, observation/recording,
real-time ownership, plugin lifecycle, usability, or distribution.

Do not turn every reported timbre difference into an equalizer request.
Disagreement may arise from tuning, contact trajectory, body mode, virtual
microphone, model simplification, or changed source data. Use the layer and
mechanism boundaries from the handbook to isolate the cause before changing
several controls at once.

Preserve the smallest failing fixture and add it to the relevant regression
suite when allowed. If it contains private performance recordings, create a
rights-safe minimized representation or synthetic reproduction as appropriate.
The original evidence may require restricted storage; do not upload it to a
public issue without consent.

### Execution recipe

**Question and route.** Which mechanism first diverges from the expected result,
and what smallest intervention can distinguish it from a recording or host
artifact? Use `ENG-P19-T01` / `REQ-MNT-001` and the release-evidence chapter's
"Triage procedure". For apparent body/decay changes consult
`R-SETAR-MANSOUR-SCAVONE-2012` §3.1–§3.4 and §4.1–§4.2 for apparatus confounders;
for host-only symptoms consult `R-SDK-JUCE-AUDIOPROCESSOR`. Search the precise
symptom plus mechanism, such as `bridge mobility sensor loading support` or
the pinned host's `state restore event order`, before proposing an EQ fix.

1. Preserve a permission-safe minimum reproduction: source/binary/model/state
   hashes, OS/host, sample rate in Hz, block sizes in samples, event sequence and
   offsets, gain, expected observable and actual output. A measured comparison
   also needs specimen/setup, calibration, microphone/force-port directions,
   coordinates in m, timing uncertainty and raw/derived references. Missing
   metadata makes the physical diagnosis inconclusive, not an estimated fact.
2. Compare the same input with the last accepted version and, where available,
   the headless engine and host adapter. Change one factor at a time and inspect
   intermediate motion, force/work, pressure observation and telemetry.

| Earliest disagreement | Candidate owner and discriminating comparison |
| --- | --- |
| Modal update, residual or energy ledger | [numerics.cpp](../../src/instrument/numerics.cpp) and [instrument.cpp](../../src/instrument/instrument.cpp); analytic oscillator or independent contact oracle before waveform matching. |
| Correct mechanics, changed output | `BodyMode.radiation_weight` or gain; compare the same mechanical state before and after observation changes. |
| Pitch, release or sympathy | `StringConfig`, contact law, tuning and bridge/body ports; hold observation fixed and isolate the suspected interaction. |
| Different events or host-only result | [performance.cpp](../../src/instrument/performance.cpp) versus `SetarProcessor::processBlock`; compare timed gesture traces and lifecycle calls. |
| Changed fitted values or unavailable evidence | Calibration/rights lineage; compare raw hashes, transforms and permissions before changing physics. |

Retain the minimal fixture, intervention table, rejected hypotheses and severity
by consequence, including state loss and invalid scientific claims. Existing
`engine.event_ordering`, `acoustics.observation_invariance` and
`coupling.uncoupled_limit` provide bounded synthetic controls, not a diagnosis of
every report. Independently reproduce the selected cause; reject a proposed fix
that only hides the symptom or changes several layers. Planned
`maintenance.golden_review` governs the eventual baseline decision. Hand the
classified case to P19-T02/T03/T04 or P19-T05, with the original physical claim
still open when measurements are insufficient.

## P19-T02: protect numerical contracts during change

For every solver or optimization revision, rerun independent analytic limits,
discrete energy/work identities, convergence studies, and reference/production
comparisons. Examine whether the revised method has changed order of accuracy,
stability domain, contact behavior, or mode normalization. A small WAV difference
can still reveal a serious energy defect; a large waveform difference can be
legitimate under an intentional physical-model correction.

Update equations and tests together, but keep at least one independent oracle
that cannot be made to pass by changing both sides of the same erroneous
formula. Retain previous known-good fixtures to detect behavior drift and
explain any intentional replacement of a golden dataset.

Reevaluate resource envelopes when mode counts, contact iteration, host APIs,
or dependency versions change. A physically improved model can violate
callback budgets. Treat this as a new qualification requirement, not an
excuse to silently drop critical physics under load.

### Execution recipe

**Question and route.** Does a solver change preserve the declared equations,
energy/work balance and admitted response domain, or intentionally introduce
new physics that needs requalification? Use `ENG-P19-T01` / `REQ-MNT-001`.
Read [convergence and golden data](../numerics/12-convergence-and-golden-data.md)
and the current solver derivation. Inspect `R-NUM-BILBAO-SMITH-2005` §II/§III
and the actual potential/discrete-work sections acquired from
`R-CONTACT-BILBAO-2014`; search `discrete gradient contact energy consistency
convergence`. These are methods references, not proof for the production update
or sources of setar contact coefficients.

1. Declare whether equations, discretization, floating-point evaluation,
   truncation or calibration changes. Freeze physical inputs, coordinates,
   damping conventions, gesture histories and observation gain. Compare old/new
   candidates at matched physical times, then refine time step and mode count
   independently. Changing sample rate can change the retained modal band;
   control that confounder before attributing a difference to time integration.
2. Use independent limits from [instrument_oracles.hpp](../../tests/support/instrument_oracles.hpp)
   and [numerics_tests.cpp](../../tests/instrument/numerics_tests.cpp): closed-form
   modal response, forced response, discrete-gradient identity and independent
   coupled-contact minimization. Add the smallest counterexample to the changed
   mechanism as future implementation work. For full coupled comparisons, state
   that a converged independent production/reference pair is still an open gate.
3. Compare raw state and ledger traces in m, m/s and J, normalized residuals,
   convergence curves, pitches, release times and force/velocity port power.
   Preserve `StringLossLaw` coefficients in 1/s, m²/s and s separately from
   `BodyMode.damping_ratio`; changes of modal normalization require transforming
   masses, participation and observation consistently. Derive tolerances from
   roundoff, convergence and physical uncertainty before observing the result.
4. Reprofile affected workloads using [setar_benchmark.cpp](../../apps/setar_benchmark.cpp)
   and actual hardware/host traces when relevant. Lower average time cannot
   excuse worse callback deadlines, lost events or silently reduced physics.

Existing `solver.modal_exact_free_response`, `solver.newton_coupled_oracle`,
`engine.lossless_energy_ledger`, `contact.nail_passivity` and
`realtime.allocation_hook` cover selected synthetic contracts. They are not the
planned `maintenance.golden_review` or `maintenance.corpus_migration` gates.
Retain old/new manifests, raw errors and timing distributions, reviewed baseline
diffs and failure cases. An independent reviewer must reject unexplained energy
creation, lost convergence or changing both code and oracle to the same formula.
Proposed `tools/compare_evidence.py` and
`docs/governance/model-migration-policy.md` receive the eventual protocol; hand
accepted numerical scope or a rejected candidate to P19-T04 and P18 requalification.

## P19-T03: version data and calibration honestly

Maintain the links among specimen identity, source records, derived geometry,
material assumptions, fitted parameters, uncertainty, reduction assets, and
validation partitions. A new measurement can correct an earlier estimate,
but the former values and evidence should remain identifiable so old renders
and user presets can be interpreted.

Do not quietly retrofit a previously published result with a different
material tensor or microphone correction. Assign new asset identities and
document migration rules. Keep sensitivity to climate/setup and specimen
variation visible; do not turn a specimen-derived calibration into a universal
setar constant through repeated reuse.

Periodically review source access and licenses as practical without assuming
all URLs remain available. Preserve bibliographic metadata and provenance
within permission boundaries. An unreachable paper must not be replaced by a
plausible unattributed value. Leave its access status and unresolved questions
in the evidence ledger.

### Execution recipe

**Question and route.** Can a new calibration or a withdrawn source be traced
through every affected coefficient, render and claim without falsifying the
historical record? Use `ENG-P19-T02` / `REQ-MNT-002`. Read the data contract's
"Derived transforms" and "Rights propagation", the corpus transformation graph,
and `R-METROLOGY-JCGM-100` §4, §5 and §7 for uncertainty/correlation. Search
`calibration correction correlated uncertainty propagation` in that official
edition. Use `R-ETHICS-OHA` and the actual agreement for withdrawal review;
neither a methods source nor accessible PDF grants retention rights.

1. Identify the change trigger: sensor correction, specimen setup, geometry,
   material assumption, fit method, partition change or permission restriction.
   Retain immutable raw hashes where permitted and assign new calibration,
   transform, parameter and model identities. Record channel conversion, polarity,
   timing, coordinate-frame mapping, SI units, covariance and validity domain.
   Separate a corrected microphone observation from a changed mechanical loss.
2. Build the dependency report from raw assets through fits, model versions,
   presets and release claims. Recompute affected derivatives and compare old/new
   values with propagated uncertainty; reserve independent conditions for
   prediction. Data reused to select the correction become development evidence.
   Keep failures and stale results visible instead of replacing the historical
   plots with newly favorable ones.
3. Map accepted parameters to [config.hpp](../../include/setar/instrument/config.hpp)
   and versioned sidecars. The coarse `Provenance` enum changes no engine behavior
   and cannot encode per-quantity permissions or uncertainty. Do not mark a whole
   synthetic body/contact package measured because one string length is measured.
   The proposed `data/retention-register.json` must link permissions, derivative
   dependencies, permitted retention and responsible access owner.
4. For a future withdrawal, follow the approved agreement/review process: exclude
   affected inputs from new packages, assess derivatives, downgrade or withdraw
   dependent claims, and retain only permitted metadata/tombstones. Do not promise
   recovery of every already-distributed copy or secretly retain prohibited raw
   data. Historical identity and current availability are different fields.

Retain the version graph, correction/withdrawal rationale, permitted manifests,
comparison tables and affected-claim list. Another authorized maintainer should
replay still-permitted results and demonstrate exclusion using a synthetic
revocation fixture. `tools/validate_measurement_campaign.py` and current
`data.acquisition_manifest_integrity` check limited schema/hash conditions;
planned `maintenance.provenance_replay` and `maintenance.revocation_path` require
the fuller process. Reject unidentified transformations or rights substitution;
block reconstruction when necessary inputs are unavailable. Hand migrations to
P19-T04 and revised notices/claims to P18, including proposed
`docs/governance/claim-withdrawal.md` rather than changing public claims silently.

## P19-T04: maintain user-state compatibility

Keep stable parameter IDs and explicit state versions. Test supported preset
migrations across changes in solver and physical assets. When an older state
cannot be represented exactly, choose and document a safe conversion, retained
compatibility path, or actionable unsupported-version message. Do not silently
reinterpret physical units or discard user edits.

Test restoration in multiple real hosts and after editor lifecycle changes.
Preserve stop/release/reset semantics and exact event ordering. A compiler or
framework update may change serialized defaults or runtime timing despite an
unchanged user interface; rerun differential event renders accordingly.

Maintain installation/uninstallation behavior and signing continuity for the
supported platforms. Where release processes require credentials, use approved
protected workflows and least privilege. Never ask contributors to commit
tokens or private signing keys to reproduce a build failure.

### Execution recipe

**Question and route.** Does an old session retain the intended tuning, gesture
semantics and physical configuration after a supported update, or fail with a
recoverable explanation? Use `ENG-P19-T01` / `REQ-MNT-001` and the dependent
`REQ-PLG-002` contract. Read [parameter/state compatibility](../plugin/02-parameters-state-and-compatibility.md),
`R-SDK-JUCE-AUDIOPROCESSOR` and the exact pinned wrapper/state implementation.
Search official JUCE and host documentation for `parameter ID normalized range
state restore` and `sample offset ordering`; capture inspected version/sections
when interpreting a changed contract.

1. Inventory supported historical state bytes, schema, parameter IDs, choice
   order, defaults, normalized mappings and model/asset hashes. The current
   [SetarProcessor.h](../../plugin/Source/SetarProcessor.h) defines
   `state_schema_version = 1` and four IDs: `outputGainDb`, `dynamicsScale`,
   `tuning`, `fretMap`. Preserve those meanings independently of display labels.
   No real released-state corpus or version-2 migration is established here.
2. Compare original and restored values, then drive both with the same timed
   canonical gestures. In [SetarProcessor.cpp](../../plugin/Source/SetarProcessor.cpp),
   `tuningId`/`fretMapId` are resolved after restoring the parameter tree; unknown
   IDs keep the restored index. Treat reordered or removed presets as a specific
   ambiguity requiring a proposed deterministic conversion or actionable rejection,
   not evidence of existing migration safety.
3. Test round trip, oldest supported version, unsupported future version,
   malformed state, canceled preparation and changed model identities. Keep
   original fixtures immutable. Define whether a migration preserves physical
   values exactly or deliberately selects a compatibility path; record units
   and uncertainty for any calibrated conversion. Configuration state does not
   serialize ringing modal motion. Current tuning/fret selection re-prepares
   and silences the engine, so exact sounding-state continuation is not promised.
4. In a future built adapter, test actual host save/reopen, editor closure,
   transport changes, stop/release/reset ordering and sample-rate changes.
   Compare event traces at physical time, not array index across unequal rates.
   Recheck P18-T04 installation/rollback for every supported transition.

Retain input/output state hashes, expected semantic values, event/render diffs,
host logs and recovery instructions. Existing `engine.event_ordering`,
`realtime.block_partition` and `contact.gesture_replay` cover core ordering/replay,
not serialization or real-host restoration. `plugin.state_roundtrip`,
`plugin.malformed_state` and `maintenance.corpus_migration` remain planned.
An independent reviewer rejects silent unit/range reinterpretation, lost edits,
partial loads or undocumented sonic changes. A blocked host case stays blocked;
hand the supported conversion/rejection policy and immutable fixtures to P18
and the next maintenance revision.

## P19-T05: maintain security, dependencies, and CI evidence

Track dependency revisions, license implications, supported platforms, and
known security issues. Review changes before automatic upgrades modify audio,
threading, or binary packaging behavior. A dependency update deserves tests
commensurate with its affected boundary. If an optional GUI dependency cannot
be upgraded under current rights, keep headless core maintenance independent.

Preserve GitHub Actions logs and reference artifacts according to retention
policy, while maintaining enough portable fixtures inside the repository to
reproduce core claims. An expired artifact link is not a permanent archive.
Store checksums, manifest schema, exact commands, and rights-safe inputs so
new engineers can regenerate evidence.

Audit documentation links, branch/release capability matrices, and actual
test statuses. Do not copy a green badge from a previous commit onto a new
architecture. If a runner becomes unavailable, mark the qualification gap
and investigate a supported alternative without inventing a pass.

### Execution recipe

**Question and route.** Does a dependency, runner or toolchain change alter the
instrument's behavior, rights or reproducibility, and which evidence must be
renewed? Use `ENG-P19-T01`, `ENG-P19-T02`, `REQ-MNT-001` and `REQ-MNT-002`.
Consult the rights chapter's dependency inventory, [testing and CI](../engineering/testing-and-ci.md),
and `R-SDK-JUCE-AUDIOPROCESSOR` / `R-SDK-VST3-VALIDATOR`. Follow the exact upstream
repositories to official release notes, license files and security advisories;
search `<dependency> <pinned revision> advisory` and the changed API. Record
which affected-version statements were inspected, whether the component is
shipped or build-only, and unresolved applicability. Do not infer exposure or
clearance from a package name alone.

1. Compare the accepted dependency graph with the proposed update, including
   transitive/bundled SDKs, compiler flags, platform runtime and licenses. Review
   changes to threading, allocation, floating-point math, state parsing and
   bundle contents before choosing qualification scope. Keep a headless path
   independent when optional JUCE rights/build access remains unresolved.
2. Inspect [engineering.yml](../../.github/workflows/engineering.yml): it pins
   actions by commit, uses `contents: read`, and retains uploaded evidence for
   14 days. These are current configuration facts, not assurances that a later
   run succeeded. Record actual workflow revision, action revisions, runner
   image/architecture, commands, test discovery and result logs for each change.
   A replacement runner or architecture needs new evidence under its own identity.
3. Preserve portable source, permitted synthetic fixtures, schemas, hashes and
   regeneration commands beyond temporary CI artifact links. Keep calibrated
   records in approved retention storage. Test a replay from retained inputs
   with another maintainer; if a tool/source is unavailable, report the precise
   missing dependency and downgrade affected reproducibility claims.
4. After a separately authorized update, run the smallest affected checks plus
   required baseline/platform gates. Schema/link audits and
   `documentation.executable_coverage_integrity` do not establish audio, host
   or physical equivalence. Renew canonical-event, numerical-oracle and package
   comparisons where those boundaries changed; preserve failures and run identity.

Retain the dependency/license diff, impact table, advisory applicability decision,
actual logs and old/new comparison artifacts. Independent review rejects an
unexplained behavior change, new unresolved shipped rights, absent test discovery
or recycled CI evidence. Planned `maintenance.golden_review` and
`maintenance.provenance_replay` remain broader acceptance gates. Hand runtime
issues to P19-T01/T02, rights/access restrictions to P19-T03 and release-scope
changes to P18. No dependency, workflow permission, secret or publication setting
is changed by this recipe.

## P19-T06: preserve a culture of skeptical review

Use issue reports, reproducible experiments, decision records, and practitioner
feedback to refine priorities. Invite technical challenge to cherished
assumptions, especially published material tables, solver passivity claims,
and acoustic comparisons with large nuisance effects. Update the evidence
status when a claim is overturned; do not hide corrected science to preserve
a marketing sentence.

Keep substantial new work in explicit work packages with an answerable
question, prerequisites, sources, implementation path, tests, acceptance,
failure rules, and handoff. Extend the handbook when a genuinely new
mechanism or experimental method is needed, not to increase its page count.
The useful outcome is a reproducible, improving instrument and a repository
in which new developers can trust what has actually been demonstrated.

### Execution recipe

**Question and route.** What observation would overturn an accepted explanation,
and can the project retain that result without losing provenance or overstating
the replacement? Use `ENG-P19-T02` / `REQ-MNT-002` and
[research-to-model workflow](../research/research-to-model-workflow.md) §§2–7.
Start with a contested claim and its original source. For a material claim,
`R-SETAR-PEDRAMMEHR-2018` §3.2/Table 1 and the
[published-parameter audit](../research/published-parameter-audit.md) provide an
admissibility case. For an apparently unique fit, `R-IDN-RAUE-2009` motivates
identifiability investigation, but its atlas access limits remain explicit.
Search the exact DOI/title with `correction`, `erratum`, `replication` and the
mechanism; record negative searches without declaring that no contrary evidence
exists. An inaccessible paper is an acquisition lead, not an inspected method.

1. Give a reviewer the original claim, permitted source/raw records, parameter
   transformations, model/analysis versions, units, uncertainty and held-out
   partition history. Ask them to reconstruct the argument and name a rival
   mechanism: for example, sensor loading versus body damping, or observation
   gain versus force calibration. Agree on a discriminating comparison before
   choosing a new parameter or acceptance threshold.
2. Retain accepted, rejected, inconclusive and blocked findings with reasons.
   A synthetic counterexample can refute a numerical claim without establishing
   a specimen value. Conversely, a pleasing recording cannot repair a failed
   work balance. Check exact `StringConfig`, `BodyMode` or event changes against
   the [current architecture](../architecture/01-current-code-and-proposed-engine.md)
   before proposing a new abstraction. Keep unmeasured quantities unknown.
3. Prepare an explicit work package with source access, controlled inputs,
   independent comparator, error budget, proposed files, failure rules and owner.
   If the claim changes, supersede the relevant ADR with links to the prior
   decision and model/migration consequences. Preserve old evidence only within
   retention rights and use P19-T03's withdrawal path where required. Practitioner
   feedback needs the existing authorized process; drafting this review sends
   no participant requests.

Retain a review dossier with source excerpts/paraphrases, reproducible reasoning,
counterexamples, disagreements, claim-dependency diff and smallest next experiment.
Existing `documentation.implementation_traceability` checks registry structure;
it does not certify the review's scientific conclusion. Planned
`maintenance.provenance_replay` and `maintenance.revocation_path` require an
independent maintainer to reconstruct still-supported claims and identify
withdrawn dependencies. Reject an unsupported golden replacement or a new claim
based solely on inaccessible evidence; report inconclusive findings as such.

**Handoff checklist.** Identify the affected model/requirement, supported domain,
retained evidence and access owner, migration/rollback implications, independent
reviewer and next acceptance gate. Send numerical work to P19-T02, data work to
P19-T03 and changed release statements to P18. This closes a bounded review
decision; it does not close the project's outstanding scientific gates.

> Triage the supplied defect or new evidence against a named revision,
> physical asset, and canonical event fixture. Isolate its layer and preserve
> a rights-safe reproduction. Update independent numerical, realtime, host,
> and acoustic checks as the change requires. Version calibration and preset
> migrations rather than overwriting history. Report changed claims and
> compatibility implications explicitly, with no fabricated test success or
> arbitrary deadline.
