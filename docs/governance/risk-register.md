# Risk register and decision procedure

This register is the starting map of scientific, engineering, rights, and
product risks. It is not evidence that the risk has been resolved. Each entry
must be reviewed when new data or a new component changes its likelihood or
impact. The owner of a bounded task records actual mitigation evidence in the
task's experiment and review packet, not only by checking a box here.

## Evaluation rules

Do not treat guessed probability percentages as scientific estimates. Use
qualitative likelihood when frequency data do not exist, distinguish severity
from detectability, and state the consequences that matter. Mark a risk
`open`, `under investigation`, `mitigated in stated domain`, `accepted with
explicit limitation`, or `retired with evidence`. No status implies global
proof. A serious unknown may block a claim even if a demonstration seems good.

Use these questions in review:

1. What observable failure would reveal this risk?
2. Which present evidence supports the concern, and what is still inferred?
3. Which input, solver, or asset boundary contains it?
4. What independent experiment would discriminate among explanations?
5. What user-facing claim or supported configuration must be withheld meanwhile?
6. What artifact and reviewer can change the risk's status?

## R01 — Missing complete specimen data

**Status: open.** Public literature and performer resources do not constitute
a complete measured digital twin of one target instrument. Hidden thickness,
material tensors, joints, bridge contact, and calibrated multi-point response
remain unresolved. A synthetic reference may be verified, but specimen
fidelity cannot be claimed. Mitigation is consented, calibrated measurement
with source identity, uncertainty, and independent validation partitions;
see P01, P07, and [measurement](../measurement/README.md).

## R02 — Inadmissible published material properties

**Status: quarantined input; open research question.** The printed wood
properties in a relevant published setar FEM table fail reciprocal normal
compliance checks under the stated convention and lack a complete admissible
shear specification. Copying them into a solver is unsafe. Preserve the
original table as a source claim, seek clarification or new measurements,
and use clearly synthetic admissible alternatives only for controlled solver
experiments. See [the primary-source audit](../research/published-parameter-audit.md).

## R03 — Nonpassive discrete coupling

**Status: open for full engine.** String, bridge, body, cavity, and contact
solvers can each appear stable while the connected update injects energy.
Mitigation requires explicit effort/flow signs, adjoint maps, work identities,
time-integration analysis, and limiting-case tests. Delayed feedback must be
examined rather than assumed passive. See P09 and the numerical coupling
contracts. No full coupled engine is implemented in the bootstrap.

## R04 — Inverse-problem ambiguity and overfitting

**Status: open.** Similar recordings may arise from different combinations of
pluck, material, damping, microphone, and structural parameters. Mitigation
uses mechanism-isolating experiments, sensitivity/identifiability analysis,
groupwise holdouts, and uncertainty-aware fitting. Do not claim uniquely
measured wood parameters from a good tone match; see P12 and P17.

## R05 — Unbounded or blocked audio callback

**Status: open for production engine.** Nonlinear contact solves, asset
installation, UI telemetry, MIDI floods, denormals, and model replacement
can violate realtime constraints. The synthetic reference provides a bounded
render path but is not a complete host event engine. Mitigation uses explicit
capacities, bounded iteration/failure behavior, immutable preparation, safe
reclamation, hardware-qualified profiling, and plugin-host stress tests.

## R06 — Cross-platform and plugin qualification gaps

**Status: headless synthetic baseline tested; full product open.** A configured
workflow proves nothing until its jobs actually pass. Even successful Linux,
Windows, and macOS headless jobs do not validate an optional JUCE VST3 or
real DAW lifecycle. Maintain a per-revision capability matrix and real-host
fixtures. Do not transplant evidence between different source revisions or
formats. See P14 and P16.

## R07 — Copyright, licensing, consent, and redistribution

**Status: open owner decisions.** No source redistribution license has been
selected for this repository. JUCE and VST3 have version-specific terms;
measurements, performances, images, and participant feedback can carry separate
rights. Keep public synthetic examples rights-safe, record exact dependency
sources, and request owner decisions before licensing or binary publication.
Do not assume the ability to read research equals permission to redistribute it.

## R08 — Misleading fidelity or cultural claims

**Status: controlled by documentation; validation open.** A setar is not an
Indian sitar, a generic guitar, or one immutable tuning template. A convincing
render may still lack representative fret, nail, or performance behavior.
Mitigation includes practitioner review, culturally specific terminology,
model/measurement distinction, and claim-limited blind/expressive testing.
“Pianoteq-level” is an aspiration, not an objective certification or affiliation.

## R09 — Numerical errors hidden by audio presentation

**Status: partly mitigated in synthetic bootstrap; full engine open.** Silent
normalization, clipping, strong filtering, arbitrary damping, or reverb can
make a defective solver sound benign. Require raw physical variables and
energy/work diagnostics, fixed documented observation gain, and independent
limiting-case tests. Keep presentation effects outside the scientific
comparison chain unless intentionally modeled and measured.

## R10 — Broken provenance, data leakage, and nonreproducible results

**Status: open as the project expands.** Model assets may outlive source code,
test logs may expire, and private raw recordings must not enter public CI
artifacts. Use manifests, hashes, input rights, exact commands, schema versions,
release records, and reviewed storage boundaries. A graph without its
transform and data identity is not an adequate evidence packet.

## R11 — Unsupported user-state migration

**Status: open for product.** Changes to physical parameter meaning, tuning,
reduction assets, or host state schema may silently alter old presets. Stable
identifiers, versioned migrations, validation, failure recovery, and real-host
round-trip fixtures are required before public preset compatibility promises.

## R12 — Uncontrolled complexity

**Status: ongoing design risk.** Solving every continuum field at maximum
resolution can exceed practical resources without improving identified
predictions. Conversely, simplifying too early can remove important attacks,
sympathetic coupling, or radiation mechanisms. Maintain offline richer
references, ablations, error budgets, and task-specific parameter identifiability.
Choose complexity based on demonstrated explanatory value and realtime cost.

## Updating this register

When a new risk appears, add a unique ID with mechanism, current evidence,
testable failure, affected claims, mitigation plan, status, and reviewer.
Never delete a resolved concern merely to make a report look green: retain a
short summary or link to the decision and evidence that retired it.

Use [the task template](../../templates/task.md) and
[experiment template](../../templates/experiment.md). A review should state
which release/product claims remain blocked by each high-severity open risk.
