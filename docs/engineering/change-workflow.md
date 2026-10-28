# Engineering changes: from bounded question to reproducible handoff

## Define the question before editing

A useful task begins with a falsifiable statement. “Improve realism” is an
aspiration, not an implementation contract. “Add a scalar force input whose
discrete work balance agrees with an independently solved driven oscillator” is
a bounded task. “Make the GUI nicer” leaves correctness undefined; “ensure the
synthetic warning remains visible at the minimum supported window size and can
be reached by a screen reader” has observable acceptance evidence.

Write the question, current limitation, proposed change, affected files, required
inputs, and expected outputs. Identify whether the task is mathematical
verification, software integration, measurement, perceptual evaluation, or a
mixture with distinct stages. State which conclusions are out of scope. This
prevents a useful result from being promoted into a much broader claim during a
handoff.

Read the existing implementation and relevant handbook section before adding a
parallel abstraction. The project already separates modal state, observation,
offline file generation, report generation, and optional host integration.
Change the layer that owns the behavior. Do not fix a core energy defect by
changing report scales, or solve an output-path problem by altering numerical
state.

## Establish the baseline and preserve unrelated work

Inspect repository status and reproduce the relevant baseline commands. Keep the
exact named tests and their outputs. If the baseline already fails, record that
fact before changing source. Distinguish the pre-existing failure from any new
one, and agree whether the task includes fixing it. A contributor should not
silently rewrite unrelated code to make a broad green result easier to obtain.

Use a separate build directory for compiler, sanitizer, or dependency
experiments. Keep generated outputs out of source directories. Do not commit
large transient build trees or binaries simply because they were needed for a
local test. Conversely, do not discard an expensive, irreproducible measurement
under the assumption that every generated file is disposable. Artifact retention
depends on provenance and reproduction cost, not just filename extension.

Parallel contributors should own nonoverlapping files or explicitly coordinate
shared interfaces. A public header change affects tests, renderer, documentation,
and optional boundaries. Announce such a change before another worker writes
against the old contract. Never independently force-push or rewrite shared
history as a way to reconcile overlapping edits.

## Design the independent expectation

For numerical work, choose an expected result before implementing the new
algorithm. Suitable expectations include a characteristic equation, analytic
forced response, exact impulse work, a symmetry, a conservation identity, a
known manufactured solution, or a converged independent solver. Explain why that
expectation is independent enough to catch the intended mistake.

Specify a norm and tolerance. State whether the comparison is absolute, relative,
energy-weighted, phase-aligned, or perceptually weighted. Specify the parameter
domain and the behavior near zero. If comparing different sample rates, align
physical time rather than array index. If comparing different spatial
representations, define the projection between them instead of treating unlike
coordinates as interchangeable.

Include at least one failure case relevant to the new behavior. For a force
input, wrong force sign should violate a known result. For serialization,
truncated input should be rejected without corrupting live state. For a new
plot, a non-finite source value should not become an attractive empty chart. The
test should make the failure legible enough that a future maintainer can diagnose
it without reconstructing the whole original discussion.

## Implement with explicit ownership and failure paths

Keep units at the interface. Prefer names that distinguish meters from fractions
of length and angular frequency from hertz. Avoid a single untyped “frequency”
parameter that means different quantities in the UI and solver. If an input is
converted, document the conversion and retain the original source unit where
provenance requires it.

Decide which operations may allocate, throw, block, or perform I/O. The current
render path has a narrower contract than preparation. Preserve that distinction
when adding state. A successful preparation should install a fully valid
candidate; a failed preparation should not leave half-updated coefficients and
old state dimensions. Tests should inspect the old usable state after a rejected
candidate.

Avoid “helpful” silent corrections of unsupported physics. Clamping a negative
mass, dropping high modes, renormalizing a growing state, or replacing a
suspicious material value can make a demo continue while invalidating the
experiment. Reject the request or explicitly record an authorized transformation
with its reason and consequences.

## Verify in layers

Run the smallest affected test first, then its neighboring numerical or software
contract, then the complete applicable baseline. Build an optimized configuration
as well as the normal debug configuration when a change could interact with
undefined behavior, floating-point optimization, or assertion removal. Use
sanitizers where the environment supports them, and preserve an explicit record
when a runtime restriction prevents a category of check.

Regenerate the report from the modified executable into a fresh directory. Read
the metadata and check values before looking at the curves. A visually plausible
waveform can coexist with the wrong units, mass normalization, or timestamp.
When presentation changes, capture and inspect the real rendered result. When
host integration changes, exercise a real named host rather than assuming that
headless tests cover it.

Separate evidence categories in the change record: compiled, numerically tested,
integration-tested, sanitizer-exercised, visually inspected, host-tested,
measured, and acoustically/perceptually validated. It is acceptable for many
columns to remain not applicable or not yet exercised. It is not acceptable to
collapse them into “works” and leave readers to infer the rest.

## Handoff record

A completed task should leave a concise summary with the implemented behavior,
files changed, rationale, input/output contract, exact commands, observed results,
artifact locations and hashes where useful, known limitations, and the next
unresolved question. Include an actual failure diagnosis when the task ends at a
valid blocker. A carefully established boundary can be a meaningful result even
when the desired feature is not yet accepted.

Review the documentation for accidental completion language. A newly derived
method is `proposed` until implemented and checked. A synthetic fixture can be
`verified` against its mathematical contract without being `validated` against a
real instrument. A workflow can be configured before it has a successful run.
These distinctions should survive the handoff and remain visible when an
artifact is opened by itself.

Finally, leave the repository in a state that another developer can reproduce.
Do not depend on an unrecorded local library patch, an invisible environment
variable, a private file copied into a build directory, or a manual step known
only to the original author. If such a dependency is unavoidable, name it and
make the missing access or evidence an explicit entry condition for the next
task. That is more useful than a nominally finished change that nobody can
verify.
