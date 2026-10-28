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
