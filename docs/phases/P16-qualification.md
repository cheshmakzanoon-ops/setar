# P16 — Qualify software, numerics, hosts, and evidence gates

**Status:** specification. **Prerequisites:** P15, with qualification fixtures
developed throughout earlier phases. **Exit evidence:** release-candidate
capability matrix backed by actual platform results, numerical acceptance
reports, host cases, and documented exceptions.

## Qualification is a connected argument

No single green test suite establishes correctness. A mathematical energy test
does not prove a real instrument model; an attractive acoustic match does not
prove thread safety; a plugin validator does not prove all host/device cases.
This phase assembles independent results into a bounded claim about the system.

Read [validation](../validation/README.md),
[real-time](../realtime/README.md),
[plugin](../plugin/README.md), and
[the quality contract](../governance/quality-contract.md).
The initial repository's headless GitHub checks cover a synthetic reference and
documentation tooling. Future production qualification needs additional jobs
and real-host evidence; those do not exist merely because these instructions do.

### Entry packet and source map

Freeze a candidate before qualification: source revision plus any dirty patch,
model/configuration hashes, dependencies, compiler/flags, planned claim and
permission-safe fixture identities. Begin with the actual
[prototype adapter](../plugin/08-prototype-juce-adapter.md),
[realtime evidence](../realtime/05-prototype-realtime-evidence.md) and
[coverage register](../../roadmap/executable-test-coverage.json).
The current core is synthetic. The optional custom JUCE editor/processor sources
are not a built or hosted product, and earlier container timings are not new
hardware qualifications. Preserve historical failures and their fixes without
reporting already-fixed defects as current failures.

| Decision | Exact primary-source route | Access and applicability limit |
| --- | --- | --- |
| Host lifecycle, buffers and state | `R-SDK-JUCE-AUDIOPROCESSOR`, [official AudioProcessor reference](https://docs.juce.com/master/classjuce_1_1AudioProcessor.html): `prepareToPlay`, `processBlock`, state methods, buses and suspension. | Public `master` page is readable. Inspect the exact authorized dependency source before applying a contract; the documented prototype syntax check used JUCE 8.0.10. It was not a host execution. |
| VST3 conformity | `R-SDK-VST3-VALIDATOR`, [official validator page](https://steinbergmedia.github.io/vst3_dev_portal/pages/What%2Bis%2Bthe%2BVST%2B3%2BSDK/Validator.html) and pinned SDK's version/license files. | The public page describes a conformity test host. A tool description, downloaded source or fabricated log is not an executed validator result or independent DAW result. |
| Mathematical correctness | `R-NUM-BILBAO-SMITH-2005`, [author manuscript](https://www.pure.ed.ac.uk/ws/portalfiles/portal/11222313/acustica_final.pdf), sections II–III/appendix; `R-CONTACT-BILBAO-2014`, [versioned preprint](https://arxiv.org/abs/1405.2589v1), potential/work/solve definitions. | The [atlas](../research/source-atlas.md) records partial inspection. Read the decisive derivation; a proof for a different discretization does not certify this modal solver. |
| Measurement and cultural claims | `R-METROLOGY-JCGM-100`, [official uncertainty guide record](https://www.bipm.org/en/doi/10.59161/jcgm100-2008e), sections 4, 5 and 7; `R-SETAR-TABASSIAN` and `R-EVAL-ITU-BS1116-3` / `R-EVAL-ITU-BS1534-3` through the atlas. | Methods and performer accounts supply no project measurements or listening results. BS.1534 full-method access remains unestablished in the atlas; exact edition review and authorized participants remain P17 work. |

Use bounded queries such as `site:docs.juce.com AudioProcessor processBlock`
or `site:steinbergmedia.github.io/vst3_dev_portal validator` to resolve a
specific contract. For a selected DAW, retain its official manual URL, version,
actual inspected lifecycle/state sections and access restrictions alongside the
run. Stop when the case expectation is justified or the missing source/license/
hardware is recorded. Synthetic preparation, metadata mutation checks and
independent oracles can continue while real-host acquisition is blocked.

## P16-T01: define the qualification matrix

List supported operating systems, architectures, compilers, deployment targets,
sample rates, block sizes, plugin formats, buses, host families, audio devices,
and expected physical-feature sets. For every cell, record implemented,
locally tested, CI tested, manually tested, acoustically validated, or not
supported. Include the exact revision and dependency versions used.

Keep the matrix small enough to support honestly. A platform added to marketing
without a corresponding reproducible test entry creates a false claim. If a
toolchain or host cannot be automated, document the manual evidence contract
and its reproducible steps rather than assuming it behaves like another host.

Define which variants share a common numerical core and which require separate
tests. SIMD paths, float/double precision, macOS CPU architectures, and plugin
wrappers may introduce real differences. A Linux headless pass cannot be copied
into the Windows VST3 column by analogy.

### Execution recipe

**Question and sources.** Which exact environment supports each claim about
persistent-state rendering, host survival and visible evidence? Read
[cross-platform evidence](../validation/07-cross-platform-ci.md),
[the 13-gate contract](../plugin/07-product-qualification-evidence-gates.md),
`R-SDK-JUCE-AUDIOPROCESSOR` and `R-SDK-VST3-VALIDATOR` through the source
map above. Resolve behavior against the pinned checkout and selected host's
official manual, not a different release's search snippet.

1. Copy the [qualification template](../../data/product-qualification-template.json)
   into a proposed evidence directory only when preparing an actual campaign.
   Retain all thirteen required cells: validator, independent real host,
   realtime and live UI for each of Linux/Windows/macOS, plus
   `global.perceptual`. The latter is a separate P17 result, not a prerequisite
   for preparing P16's software cases. Missing cells remain unexecuted; narrowing
   scope needs an explicit reviewed decision and compatible schema change,
   not deleting inconvenient rows.
2. For every case attach full source revision/dirty-patch identity, binary/bundle
   hash, dependency revisions, compiler/standard library/flags, observed OS image
   and architecture, host/version/format, device/driver, sample rate in Hz,
   block sequence in samples, model capacity and fixture hashes. Separate
   admission limits from executed coverage: `ProcessSpec`'s 22050–192000 Hz
   domain is not evidence that every rate/device combination has run.
   The template's single top-level binary hash cannot identify every OS bundle
   by itself; retain per-case bundle manifests and mark schema extensions as
   proposed until independently checked.
3. Inspect [engineering.yml](../../.github/workflows/engineering.yml) and actual
   retained job logs separately. Its headless/sanitizer lanes do not build the
   optional instrument plugin. [plugin/CMakeLists.txt](../../plugin/CMakeLists.txt)
   declares VST3 and Standalone; it does not establish AU support or universal
   macOS architecture coverage. Record execution, failure, blocked/skipped
   reason and supported feature set per artifact, never by analogy.

Retain the matrix, exact official source locations, commands, test XML/raw logs,
artifact hashes, exceptions and reviewer decisions. An independent reviewer
rejoins each claimed cell to its binary and run, including a deliberately missing
or mismatched artifact. Reject stale-revision reuse, inferred platform passes,
changed binaries and absent licensed hosts reported green.

`ENG-P16-T01` / `REQ-QLF-001` own this map; `REQ-LAB-002` supplies the
platform-isolation prerequisite. Current `data.acquisition_manifest_integrity`
only partially covers planned `qualification.matrix_integrity` through metadata
mutation checks. `qualification.host_rescan` remains planned. Hand the frozen
case matrix to P16-T02/T03/T04 and the still-empty perceptual cell to P17.


## P16-T02: automate independent scientific checks

Run analytic test problems, convergence suites, energy/work balances, geometry
and material admission checks, asset round trips, deterministic performance
replay, and reference/production comparisons. Keep tolerances tied to derived
bounds, reference precision, or measured uncertainty as appropriate. Do not
increase a tolerance to suppress an unexplained regression.

Add property and randomized tests for valid physical domains; preserve seeds
and minimal reproducing inputs. Include invalid conditions—NaNs, negative
moduli, malformed states, impossible contacts, nonconverged solves, missing
assets—and prove that they are rejected or contained as specified. Tests must
run in release-like configurations as well as debug instrumentation.

Use independent oracles. An implementation compared only with another code
path sharing the same formula and indexing error may pass for the wrong reason.
Where possible, test limiting cases, conservation identities, a different
discretization, and measured observations with separate error mechanisms.

### Execution recipe

**Question and research route.** Does the exact candidate satisfy independent
mechanical identities within its stated domain before its sound is interpreted
as an instrument result? Read [test/error-budget taxonomy](../validation/01-test-taxonomy-error-budgets.md),
`R-NUM-BILBAO-SMITH-2005` sections II–III and the relevant appendix, and
`R-CONTACT-BILBAO-2014`'s actual potential/discrete-work derivation. Search
`energy conserving nonlinear string boundary work` or
`collision discrete gradient scalar oracle` only for a missing derivation or independent comparator.
These are methods sources, not measured setar coefficients; unavailable decisive
text leaves that proof claim blocked.

Choose one frozen synthetic fixture and its failure counterexample before
running a sweep. Record coordinates, SI units, boundary conditions, initial
modal displacement, loss convention, contact law, sample rate, retained modes
and event ordering. Refine one numerical control at a time. Changing sample rate
also changes `min(max_mode_frequency_hz, 0.45*fs)` truncation in
[config.hpp](../../include/setar/instrument/config.hpp); hold retained physical
content fixed where possible or quantify that confounder. Distinguish the
instrument's nut-to-tail `total_length_m` and speaking `bridge_position_m`
from the isolated reference span.

Audit oracle independence in
[tests/support/instrument_oracles.hpp](../../tests/support/instrument_oracles.hpp),
[modal_crosscheck.hpp](../../tests/support/modal_crosscheck.hpp) and the
[numerics tests](../../tests/instrument/numerics_tests.cpp).
Use closed-form free response, independently solved scalar/coupled contact,
a different spatial discretization, uncoupled limits and observation invariance
as appropriate. Preserve seeds and minimized cases for randomized valid-domain
and invalid-input checks; deterministic fixtures need no invented random seed.
Derive new tolerances from truncation, arithmetic conditioning and reference
convergence. Measurement comparisons additionally need calibrated records and
`R-METROLOGY-JCGM-100` uncertainty; do not borrow a numerical tolerance as
an acoustic acceptance band.

Retain input/oracle hashes, source equations, raw error/energy/force tables,
convergence plots and accepted/rejected/inconclusive/blocked dispositions.
An independent reviewer must reproduce the comparator without importing the
production formula that is under test. Present CTests include
`solver.modal_exact_free_response`, `solver.newton_scalar_oracle`,
`body.plate_frequency_oracle`, `engine.config_validation`,
`contact.release_convergence` and `acoustics.observation_invariance`;
their synthetic scopes do not close planned `verification.oracle_independence`
or `realtime.reference_parity` in full. Reject tolerance widening without cause,
shared-oracle errors and fault-contaminated “successful” traces.
`ENG-P16-T01` / `REQ-QLF-001` aggregate this evidence, consuming
`REQ-LAB-001` and `REQ-RTS-002`. Return mechanism failures to their phase;
only supported numerical claims proceed to P16-T06/P17.


## P16-T03: verify runtime and concurrency boundaries

Instrument memory, allocation, and timing behavior under representative and
adversarial workloads. Exercise bounded event capacity, high mode counts within
support, simultaneous contacts, missing/malformed assets, repeated preparation,
and long decay tails. Check no blocking or allocation on the callback and no
premature asset reclamation across threads.

Use sanitizers, static analysis, and fuzzing for the interfaces they can test.
Document their limits and skip reasons. A sanitizer launch failure due to a
restricted environment is not a clean run; a remote successful sanitizer job
does not prove the absence of every race or lifetime error.

Collect timing distributions on named hardware and note measurement conditions.
Do not translate an arbitrary runner's elapsed build time into audio callback
latency. Qualification includes both the observed worst-case workload and the
error policy outside the supported envelope.

### Execution recipe

**Question and primary routes.** Can overload and concurrent lifecycle activity
be contained while the failure remains visible as invalid physics? Use
[callback bounds](../realtime/01-callback-contract.md),
[ownership/telemetry](../realtime/04-concurrency-state-and-telemetry.md),
`R-SDK-JUCE-AUDIOPROCESSOR`'s lifecycle/suspension methods, and the official
[ThreadSanitizer](https://clang.llvm.org/docs/ThreadSanitizer.html) and
[AddressSanitizer](https://clang.llvm.org/docs/AddressSanitizer.html) documentation
for the actual compiler version. Search only a named unsupported instrument or
thread contract. Instrumentation unavailable at launch is blocked evidence.

First map actual owners using
[Instrument](../../include/setar/instrument/instrument.hpp) and
[SetarProcessor](../../plugin/Source/SetarProcessor.cpp).
`prepare()` may allocate; `process/reset/snapshot/ledger` require one owner.
Review editor creation/destruction, state loads, the reprepare timer and host
thread identities. In particular, `engine_status_` can be written by
`prepareToPlay` while the editor reads it; the adapter chapter identifies this
potential race. Relaxed telemetry atomics do not prove compound-state coherence,
and a general immutable-asset/reclamation protocol remains proposed.

Freeze admissible capacities and inject bounded invalid events, exhausted event/
interaction capacity, forced Newton iteration limits, failed preparation and
repeated releases in a controlled harness. Include closing/stalling the editor
and shutdown during a pending change. Inspect actual `enqueue`: a full 256-entry
queue drops the incoming event and counts it; emergency-release priority is not
implemented merely because the roadmap requests it. The engine's
`NewtonNotConverged` reports use of the last iterate; a finite output is not
proof that the physical solve succeeded. Record counter/state transitions,
finite float output, energy removed by containment and successful recovery.

Measure release-like callback entry/exit with a characterized monotonic clock,
named CPU/OS/audio device, power policy, concurrent workload and instrumentation
cost. Store raw durations and deadline misses. Derive the proposed plugin budget
from `block_frames / sample_rate_hz` with declared host/driver margin; sanitizer
timings and historical shared-container averages are not deadline qualification.

Current `realtime.allocation_hook`, `realtime.max_work`,
`realtime.capacity_overflow` and `engine.newton_failure_reporting` exercise
bounded synthetic paths; they do not test every allocator, host, race or recovery
priority. Planned `qualification.fault_injection`,
`qualification.note_release_priority` and `realtime.asset_handoff` remain open.
An independent concurrency reviewer checks lifetime arguments, raw traces and
negative controls. Stop on callback waits/allocation, unbounded destruction,
lost release, unexplained nonfinite output or unaudited races. Retain failed and
blocked runs under `ENG-P16-T02` / `REQ-QLF-002` / `REQ-RTS-001`,
with `REQ-RTS-003` ownership gaps handed to P13/P14.


## P16-T04: run plugin-host and state qualification

Use recognized format validators and representative DAWs. Test discovery,
instantiation, event offsets, state save/restore, old-state migration, corrupt
state rejection, editor lifecycle, sample-rate changes, bypass, transport,
reset, rendering, and teardown. Keep the host-dependent expected behavior
explicit and reproduce issues with a small fixture.

Compare headless and plugin output for identical canonical event logs and
physical configuration. Preserve observed differences and determine whether
they come from event mapping, host bus format, state migration, precision, or
actual engine divergence. Do not add host-specific synthesis compensation
without recording why it is needed.

Protect credentials and release artifacts. Untrusted pull requests should
build/test without signing secrets, repository-write tokens, or publication
permissions. External dependencies and test tools need reviewed versions and
rights. Upload test evidence where authorized, but do not publish a final
redistributable plugin until P18's rights and release gate.

### Execution recipe

**Question and sources.** Does the actual host adapter preserve a declared
physical event/state contract across discovery, playback and restoration?
Read [the host playbook](../plugin/06-host-qualification-playbook.md),
[adapter limits](../plugin/08-prototype-juce-adapter.md),
`R-SDK-JUCE-AUDIOPROCESSOR` and `R-SDK-VST3-VALIDATOR`. Inspect exact-version
state, buffer, lifecycle and bus definitions, plus the selected host manual.
Stop at missing authorized toolchain/host access; continue headless fixtures
without inventing a hosted run.

Freeze the actual VST3 bundle hash and dependency manifest before validator
execution. Record full command, stdout/stderr, exit, tool/version and tested
class/format. Then use a separately identified real DAW: the validator cannot
also supply the independent-host cell. Drive fresh scan, load/unload, zero/one/
irregular blocks, same-sample event ordering, rate/bus changes, transport,
bypass/tail, repeated instances, editor cycling and offline bounce. Every case
states expected physical continuity or explicit reset, not just “no crash.”

Use [SetarProcessor.cpp](../../plugin/Source/SetarProcessor.cpp),
[performance.cpp](../../src/instrument/performance.cpp) and
[instrument.cpp](../../src/instrument/instrument.cpp) as the current paths.
A proposed differential harness must retain canonical sample times, sequence,
string, contact coordinates in metres and force/trajectory units. Compare with
headless execution at the same initial state and observation gain; do not align,
normalize or EQ away discrepancies. Begin with held gain, then test automation
separately because the adapter ramps observation gain per host block.

Test state-schema 1 independently: parameters, `tuningId` and `fretMapId`
round-trip, but ringing mechanical state is not serialized. The current code
ignores malformed/foreign/new-schema state; unknown IDs retain restored choice
indices, and selections can trigger a motion-resetting reprepare. Those facts
need negative fixtures, not a blanket rollback-safe or migration claim.
Future byte limits, transactional decoding and old-version migration are
proposed work. Preserve corrupt/truncated fixtures and prior accepted-state
identity without private host-project data.

An independent reviewer traces host inputs to event/state/PCM differences and
reproduces the worst failure. Existing `realtime.block_partition` and
`score.schedule_ordering` are headless checks; there are no current plugin
CTests for planned `plugin.state_roundtrip`, `plugin.malformed_state` or
`plugin.lifecycle_matrix`. Reject mismatched hashes, unexplained event loss,
partial invalid installation and validator-only host claims.
`ENG-P16-T01` / `REQ-QLF-001` aggregate P14's
`REQ-PLG-002` / `REQ-PLG-003` evidence, handing UI cases to P15 and any
distribution request to P18's separate gate.


## P16-T05: audit evidence and visuals

Review every claimed capability against tests and artifacts. Plots, screenshots,
and listening files must name their data provenance, software revision,
observation chain, conditions, and limitation. A graph generated from synthetic
inputs remains synthetic even if it looks indistinguishable from a measurement.

Check that results can be reproduced from manifests and allowed source data.
If a proprietary specimen recording cannot be published, preserve a permission-
safe redacted metadata record and a separate public synthetic fixture. Do not
upload confidential raw records in order to make a test artifact easier to click.

Use an issue/exception register with severity, affected domain, reproduction,
workaround if applicable, and disallowed claims. A known failure may permit a
narrower release scope, but it must not disappear from the evidence packet.

### Execution recipe

**Question and source route.** Can an independent reader reconstruct a claimed
result and distinguish physical data from presentation without accessing
unauthorized records? Read
[artifact identity](../engineering/artifacts-and-reproducibility.md),
[release evidence](../validation/08-release-evidence-and-failure-triage.md)
and `R-METROLOGY-JCGM-100` sections 4, 5 and 7. For structural-measurement
claims, revisit `R-SETAR-MANSOUR-SCAVONE-2012`'s support/sensor comparisons
through the atlas: neither an attractive FRF nor a published mode list
establishes the project's calibration or modal residues.

For each capability select one artifact and follow claim → case criterion →
actual run/binary → model/configuration → raw input → allowed source.
Retain source location, unit/axis/sign convention, sensor/calibration or
synthetic generator, uncertainty, event timeline, analysis revision and every
derived hash. In [setar_play.cpp](../../apps/setar_play.cpp) /
[analyze_render.py](../../tools/analyze_render.py), the prototype label is
`synthetic_unvalidated_prototype_render`; the older reference report uses
`synthetic_not_measured_setar`. Neither may become measured by editing a title.
`BodyMode::radiation_weight` maps acceleration to synthetic observation and
does not implement acoustic feedback or calibrated radiated power.

Reconstruct plotted values from retained tables, checking time origins, fixed
observation gain, spectrum reference/window, block-end cadence, logarithmic
floors and omitted intervals. Inspect failures and maxima as well as nominal
plots. Compare a genuine capture with the running permitted candidate and its
semantic state; [capture_report.py](../../tools/capture_report.py) is an HTML
report route, not native-host evidence. Preserve unretouched images and a
separate annotation/diff derivative. Missing raw data, inaccessible private
records or unavailable host capture produce explicit access-limited or blocked
findings; use an identified public synthetic fixture for reproducibility without
substituting it for the private measurement.

Run the existing metadata checks when reviewing a future packet:
[validate_product_qualification.py](../../tools/validate_product_qualification.py)
checks referenced hashes and kind-specific fields, while
[test_evidence_gates.py](../../tools/test_evidence_gates.py) exercises rejection.
A PNG signature, dimensions or synthetically structured validator log cannot
authenticate execution. Independent review must test that chain.

Current `integration.report`, `integration.setar_play` and
`data.acquisition_manifest_integrity` provide limited executable checks;
planned `qualification.matrix_integrity` and `ui.screenshot_regression` are
not completed host/visual gates. Reject false provenance, irreproducible
transforms and concealed failures. `ENG-P16-T01` / `REQ-QLF-001` receive
the accepted/rejected/inconclusive/blocked claim index, exception severity,
affected domain and smallest resolving action; hand it to P16-T06 and P17.


## P16-T06: conduct independent gate review

Assign reviews to numerics, acoustics/measurement, real-time C++, plugin/host,
and practitioner/perceptual roles appropriate to the claim. Reviewers should
challenge shared assumptions and identify untested mechanisms, not merely
approve code style. Resolve critical errors or narrow the supported claim.

The exit package consists of a capability matrix, actual CI and manual-host
results, source/asset manifests, numerical and acoustical evidence, open risks,
and a review decision. The next phase evaluates sound and playability; no
qualification pass automatically proves commercial-level fidelity.

> Construct a release-candidate qualification matrix from actual evidence.
> Separate implemented, CI-tested, manually tested, and acoustically validated
> fields. Exercise independent scientific references, hostile input cases,
> cross-platform builds, sanitizers, callback ownership, host state lifecycles,
> and canonical-event differential renders. Preserve failures and skipped
> cases with reasons. Recommend a bounded supported domain rather than filling
> empty cells with assumptions.

### Execution recipe

**Question and research route.** What bounded claim survives independent
challenge of both the physical model and its execution evidence? Read the
[quality contract](../governance/quality-contract.md),
[decision gates](../governance/engineering-decision-gates.md) and
[release triage](../validation/08-release-evidence-and-failure-triage.md).
Assign decisive source checks by question: `R-CONTACT-BILBAO-2014` for a
collision/work argument, `R-METROLOGY-JCGM-100` for calibrated uncertainty,
`R-SDK-JUCE-AUDIOPROCESSOR` / `R-SDK-VST3-VALIDATOR` for host contracts,
and `R-SETAR-TABASSIAN` for the bounded performer account. Review actual
sections and applicability; unavailable source text or participant evidence
cannot be replaced by another reviewer's confidence.

Freeze a review packet with the candidate/binary/configuration identities,
matrix, source-access log, source/raw/derived manifests, independent-oracle
lineage, per-case tolerances and dispositions, faults, skips and unsupported
claims. Arrange numerical, measurement, realtime C++, host and practitioner/
access review appropriate to that claim when authorized; documentation work
creates the assignments and scripts, not participant contact or fictitious
signatures. Record each reviewer's contribution/conflict and separate the
implementation author from the decisive independent check.

Have reviewers select a worst failing case and its negative control, reproduce
a decisive result from raw records, and trace one musician action to event,
mechanics and observation. Ask whether a competing mechanism, changed support,
gain, truncation or controller mapping could explain the result. Recheck
lifetime/reclamation and recovery after failure, not just steady playback.
An acoustic-looking plot cannot answer a concurrency question, and a green
numerical test cannot certify practitioner approval. Preserve unresolved
disagreements instead of averaging incompatible physical or musical assumptions.

Produce a dated review decision with reviewer identifiers where permitted,
accepted scope, rejected claims, blockers, owner and next discriminating action.
Critical unexplained numerical/concurrency faults or untraceable artifacts stop
acceptance within the affected domain; narrowing a claim must retain the failure
and its exclusion. Missing access yields blocked review, while independently
verified synthetic work can still be accepted within its stated scope.

`ENG-P16-T01` / `REQ-QLF-001` and `ENG-P16-T02` /
`REQ-QLF-002` / `REQ-RTS-001` own the review. Existing metadata/allocation/
capacity CTests remain partial; planned `qualification.matrix_integrity`,
`qualification.fault_injection` and `qualification.note_release_priority` need
their full evidence. Pass the limited candidate and open gates to P17 for
evaluation readiness. The template's `--require-qualified` must still reject a
packet lacking `global.perceptual` or another required pass; P16 review does
not manufacture that P17 result, authorize distribution, or close full product
qualification.

### Phase handoff checklist

- Rejoin every accepted platform/host claim to the exact binary, source, model,
  official contract and observed execution; keep unexecuted cells visible.
- Supply independent scientific comparators, raw numerical/timing traces,
  retained fault/negative cases, concurrency ownership rationale and recovery
  evidence, with scope and uncertainty.
- Supply authentic visuals and provenance/permission records; distinguish report
  captures, native editor captures, metadata validation and human review.
- Record the independent bounded decision, exceptions and unresolved P13–P15
  work. P17 receives evaluation inputs and unfilled empirical gates; P18 alone
  handles later rights/release decisions. The prototype remains synthetic and
  the phase remains a specification until its actual exit evidence exists.
