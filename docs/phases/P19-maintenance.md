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

> Triage the supplied defect or new evidence against a named revision,
> physical asset, and canonical event fixture. Isolate its layer and preserve
> a rights-safe reproduction. Update independent numerical, realtime, host,
> and acoustic checks as the change requires. Version calibration and preset
> migrations rather than overwriting history. Report changed claims and
> compatibility implications explicitly, with no fabricated test success or
> arbitrary deadline.
