# Release evidence, regression triage, and honest quality reporting

A release decision is a bounded statement about a particular artifact, not a
celebration of a green dashboard. This chapter specifies the evidence bundle and
review process for research snapshots and eventual instrument releases. It does
not authorize publishing, signing, uploading, changing repository protections,
or distributing licensed measurement data. Those actions require the owner's
explicitly approved workflow.

## Candidate identity

Freeze a candidate revision and build configuration. Record source commit,
uncommitted-diff status, dependencies, compiler, architecture, plugin identifiers,
state schema, model schema, and binary hashes. If the build includes generated
coefficients or fitted assets, record their source experiment and hashes too.
Changing a fitted body model without changing C++ source still changes the
instrument and invalidates evidence tied to the prior asset.

Reproducible builds and reproducible behavior are related but different goals.
Binary hashes can differ because of packaging metadata while behavior remains
equivalent; identical binaries can behave differently under different host or
device conditions. Preserve both artifact identity and execution context. Never
attach a successful result from one binary to a rebuilt binary simply because
their human-readable version strings match.

## Capability register

Maintain separate columns for implemented, numerically verified, locally tested,
cross-platform executed, host-qualified, empirically validated, and deferred.
For the initial synthetic modal scaffold, appropriate entries might say that a
reference model and report path are implemented while measurement calibration,
nonlinear contacts, and perceptual validation remain absent. Actual run records
determine execution columns; this example is not a substitute for those records.

Every visible feature should map to a supported contract. A tuning control may
exist without a validated physical retuning transition. A spectrum display may
work without a calibrated microphone comparison. A preset named after a playing
style does not establish practitioner approval. Include limitations in the
interface or accompanying documentation where a musician would reasonably infer
more capability than the evidence supports.

## Evidence bundle structure

The bundle has an index linking claims to runs and artifacts. It includes core
verification, convergence, admissible-domain checks, event semantics, real-time
instrumentation, controlled-hardware performance, host lifecycle, state migration,
GUI interaction, accessibility, and acoustic/perceptual evidence where claimed.
Missing categories are explicit. A research snapshot can be useful with missing
product evidence, provided it is not described as product-ready.

Each claim includes the criterion version, result, uncertainty or tolerance,
scope, limitations, reviewer, and supporting artifact hashes. The index should
let a new engineer locate the worst failing case quickly. Keep raw numeric data
beside plots, event logs beside rendered audio, and interaction assertions beside
screenshots. A screenshot is evidence of appearance, not of a successful state
restore, unless a separate assertion establishes the state.

## Severity and scope

Classify defects by consequence and affected domain. Memory corruption,
unbounded callback work, runaway output, persistent stuck notes, and unrecoverable
state loss require strong containment. A slight mislabeled axis may not crash
the plugin but can invalidate a scientific interpretation. Severity therefore
depends on whether the defect affects user safety, data integrity, execution,
musical behavior, evidence validity, or presentation.

Do not assign severity solely by how easy the fix is. A one-line unit conversion
can invalidate many fitted parameters. A rare crash during scanning can prevent
the host from discovering the plugin at all. Conversely, a large refactor may
have no user-visible defect if its verification remains sound. Record frequency,
reproducibility, affected configurations, and workaround separately from impact.

## Triage procedure

First preserve the failing input, binary identity, logs, and context. Reproduce
without changing several variables at once. Compare the same fixture in the
headless engine, lifecycle harness, and real host where relevant. This separates
physics errors from translation and lifecycle errors. A failure that disappears
when the editor is closed suggests a concurrency or telemetry path, but remains
a hypothesis until an intervention isolates it.

Use a known-good revision or asset as a control. Narrow the change set without
discarding user work or rewriting history. When a defect involves platform
differences, compare compiler flags, math library, architecture, denormal policy,
and event ordering. When it involves sound, compare intermediate mechanical
observables before changing output processing.

Write the regression test before or alongside the fix. The test should fail for
the old behavior and pass for the intended contract, not merely encode the new
output as a golden file. Explain why the oracle is independent enough. Add a
nearby case to prevent a narrow patch that fixes only one exact input.

## Baseline updates

A golden waveform, numeric table, or screenshot is a reviewed baseline, not an
authority immune to correction. Updating it requires an explanation of changed
behavior, affected claims, and supporting evidence. Review the difference and
the source values. Never approve a mass baseline update just because all files
were produced by the current code.

For acoustic baselines, distinguish intended model improvement from accidental
recalibration. For visual baselines, distinguish layout changes from rasterizer
or font changes. For state fixtures, never replace old-version files with new
serialization and claim migration still works. Historical fixtures must remain
representative of what earlier versions actually wrote.

## Exceptions and limitations

An exception records a failed or missing criterion, the affected scope, risk,
mitigation, owner, and condition for reconsideration. It does not relabel failure
as passing. Avoid vague statements such as “known issue, low priority” when a
musician might lose a session or receive unexpected output. If a configuration
is unsupported, reject it predictably and explain it instead of hoping it works.

Do not use an exception to bypass a safety-critical callback or numerical defect.
A research build may deliberately expose an experimental solver, but it needs
containment and unmistakable labeling. Audio protection is a last-resort product
mechanism, not proof the underlying model is stable; incidents must remain
visible in telemetry and evidence.

## Quality summary for different readers

The musician summary explains supported use, controller mappings, known audible
limitations, compatibility, and recovery behavior. The engineer summary gives
model revision, validity domain, tests, performance context, and unresolved
uncertainty. The research summary gives specimen and corpus scope, fitting
procedure, holdouts, effect sizes, and competing explanations. Reuse the same
claim identifiers so these summaries cannot quietly disagree.

Avoid absolute phrases such as “perfectly authentic,” “zero latency,” “all DAWs,”
or “fully deterministic” unless the precise bounded meaning is defensible.
For example, distinguish algorithmic latency from device and host latency, and
deterministic fixture behavior from cross-platform bitwise identity. Communicate
limits as useful operating information rather than hiding them in footnotes.

## Acceptance and next action

A candidate is ready for its declared distribution class when the evidence index
is complete, required gates have observed results, exceptions are explicit and
authorized, and reviewers can reproduce critical claims. Publication remains a
separate authorized action. The next action after review is either a bounded
release operation or a concrete experiment/fix resolving a named missing gate.
The handbook's size, the number of controls, and the attractiveness of plots do
not enter this decision as substitutes for evidence.
