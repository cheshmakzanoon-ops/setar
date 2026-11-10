# P18 — Prepare a defensible, reproducible release

**Status:** specification. **Prerequisites:** P16 and the claim-limited decision
from P17. **Exit evidence:** an approved release manifest, reproducible artifacts,
rights record, support statement, and rollback/recovery procedure.

## A release is a claim about a specific artifact

The project may eventually distribute a standalone instrument, one or more
plugin formats, a research dataset, or an engineering reference. Their rights,
tests, and audience may differ. A public repository containing the source is
not itself permission to redistribute third-party recordings, meshes, JUCE
components, SDK content, or signed binaries.

Read [rights and dependencies](../governance/rights-and-dependencies.md),
[plugin](../plugin/README.md),
[validation](../validation/README.md), and
[the capability contract](../governance/quality-contract.md). The initial
bootstrap intentionally contains no owner-selected open-source license; this
phase must not assume one has been approved.

### Phase entry and research map

These recipes specify future release work. Writing them creates no release
candidate, license decision, signed package, or publication authorization. The
current entry baseline is the [synthetic prototype](../engineering/11-playable-prototype.md)
and [source-only JUCE adapter evidence](../plugin/08-prototype-juce-adapter.md),
with the P16/P17 product and scientific gates still open. Manifest design,
rights inventories, and synthetic migration fixtures can proceed while specimen
acquisition is blocked; a measured-instrument claim cannot.

| Research decision | Primary route and handbook interpretation |
| --- | --- |
| Which dependency contract applies? | `R-SDK-JUCE-AUDIOPROCESSOR` and `R-SDK-VST3-VALIDATOR` in the [source atlas](../research/source-atlas.md); follow official upstream sources to the exact checkout's license, notices, bundled SDK and API files. Use [packaging evidence](../plugin/05-validator-host-and-packaging-evidence.md), not a remembered license summary. |
| Which recordings, coefficients and names may ship? | `R-ETHICS-OHA` and `R-RIGHTS-WIPO-TCE` in the [evidence ledger](../research/evidence-ledger.md), plus [participant rights](../ethnomusicology/participants-consent-and-rights.md). Guidance grants no project permission; actual agreements and owner decisions are separate inputs. |
| Which physical claims survive review? | `R-METROLOGY-JCGM-100`, `R-EVAL-ITU-BS1116-3` and `R-EVAL-ITU-BS1534-3` in the atlas; use exact inspected editions and [final fidelity acceptance](../validation/09-final-fidelity-acceptance.md). Methods documents supply no specimen validation. |
| What identifies the built instrument? | [Artifact contracts](../engineering/artifacts-and-reproducibility.md), [release evidence](../validation/08-release-evidence-and-failure-triage.md), current CMake/source, and the [task crosswalk](../../roadmap/phase-task-crosswalk.json). Separate shipped bytes from restricted supporting evidence. |

For each recipe retain an accepted, rejected, inconclusive or blocked decision,
its reviewer, exact source locations inspected, and evidence needed to reconsider
it. Missing approvals or measurements remain missing. Future release checks
below are not commands executed by this documentation expansion.

## P18-T01: specify the release's exact identity

Record source commit, dependency lock/revisions, asset versions, compiler and
build variants, plugin/standalone format, architecture, product version, and
expected platform support. Include the mathematical/physical feature set and
its evidence status. A version string alone is insufficient to reproduce a
numerically sensitive build.

Separate downloadable product contents from research evidence. A release can
reference measured validation summaries without bundling private raw recordings.
If assets are necessary at runtime, document their schema and installation
behavior. Do not put placeholder models or synthetic demonstration data into a
production preset under a specimen-derived name.

Make version identifiers stable across application UI, host plugin discovery,
state schema, and release records. A changed parameter meaning or physical
asset contract may require migration. Ensure a previously released preset
can be identified and interpreted without depending on the original author's
memory.

### Execution recipe

**Question and route.** Can a maintainer reconstruct the same physical model and
event interpretation from a candidate's identity without relying on its display
name? This task maps to `ENG-P18-T02` / `REQ-REL-002`. Read the release-evidence
chapter's "Candidate identity" section and `R-SDK-JUCE-AUDIOPROCESSOR` for state
and lifecycle boundaries. Search pinned upstream sources for `AudioProcessor
getStateInformation`, `juce_add_plugin VERSION` and plugin identifiers; record
the actual revision and inspected definitions, not an assertion about `master`.

1. Freeze source commit, dirty-diff digest if applicable, compiler/standard
   library, architecture, floating-point flags, CMake cache/options, dependency
   revisions and generated-asset inputs. Reconcile `PROJECT_VERSION` in
   [CMakeLists.txt](../../CMakeLists.txt) with the product, bundle and plugin codes
   in [plugin/CMakeLists.txt](../../plugin/CMakeLists.txt); do not assign a new
   identity during the inventory.
2. Record the fully expanded `InstrumentConfig` and `ProcessSpec`, including
   string/body mode limits, contacts, losses, bridge participation, observation
   weights, sample rate in Hz, event ordering and state schema. Identify
   `synthetic_setar_config()` in [setar_preset.cpp](../../src/instrument/setar_preset.cpp)
   and its `build_body(synthetic_setar_body_design())` dependency. Preserve SI
   meanings: nut-to-tail `total_length_m` differs from speaking
   `bridge_position_m`; `BodyMode.radiation_weight` observes acceleration and
   supplies no acoustic feedback. Future measured configurations additionally
   need specimen, calibration, uncertainty and permitted raw/derived lineage.
3. Inventory package paths, hashes, formats, architecture and runtime assets
   independently from evidence-only records. The current
   [setar_play summary](../../apps/setar_play.cpp) records useful preset/render
   metadata but is not a complete canonical serialization of contacts and body
   modes. A complete release manifest and canonical configuration digest remain
   proposed under `tools/release/manifest.py`.

Retain a field-by-field identity table, input/output hash manifest and canonical
event fixture. Have a second maintainer compare declared configuration with
source-generated values and artifact contents. `integration.setar_play` checks
the present synthetic renderer path; planned `release.rebuild_trace` is not a
present CTest. Reject ambiguous defaults, stale model hashes or unit/schema
changes without migration; block unavailable asset reconstruction. Hand accepted
identity and unresolved fields to P18-T02 through P18-T06 and P19; equal version
strings imply neither byte nor acoustic equivalence.

## P18-T02: complete legal and rights decisions

Obtain the repository owner's decision on source and binary licensing before
publishing under a license. Review JUCE's exact current terms for the specific
distribution model, the actual pinned VST3 SDK license, and any other dependency
or bundled tool. Keep the source-code license decision separate from ownership
and consent for instrument measurements, performance recordings, photographs,
and participant statements.

Inventory all redistribution permissions. An author-hosted PDF may be lawful
to read and cite without being lawful to copy into the repository. A specimen
owner may authorize measurement while a performer retains separate rights in
a recording. License compatibility, contributor consent, trademarks, and
data-protection requirements need explicit review suited to the jurisdictions
involved; this handbook is an engineering checklist, not legal advice.

Do not download or execute a commercial dependency in a way that implies the
owner accepted unfamiliar terms. Build tools should make optional dependencies
explicit. Record source checksums and applicable notices for shipped copies.

### Execution recipe

**Question and route.** Which exact bytes and derived physical parameters may
be included in the proposed distribution, under which recorded conditions?
Use `ENG-P18-T01` / `REQ-REL-001`. Start with the rights chapter's "Dependency
inventory" and "Measurement participants and makers" sections. From
`R-SDK-JUCE-AUDIOPROCESSOR`, use the
[official JUCE repository](https://github.com/juce-framework/JUCE) to locate the
license and notices for the actual checkout. From `R-SDK-VST3-VALIDATOR`, inspect
the [official SDK repository](https://github.com/steinbergmedia/vst3sdk) and the
license/FAQ routes linked in the packaging chapter. Search `JUCE <pinned tag>
license distribution` and `VST3 <pinned revision> bundled license`; current web
terms cannot replace the files accompanying an older bundled SDK.

1. Start from P18-T01's package inventory. For each source dependency, runtime
   library, tool, font, image, mesh, recording, calibration and derived parameter
   asset, record upstream identity, version/hash, build use, whether shipped,
   exact terms location, required notices and unresolved interpretation. Separate
   permission to run a validator from permission to redistribute it. The adapter's
   recorded JUCE 8.0.10 configuration is historical evidence, not a
   repository-enforced dependency lock or approved release license.
2. Trace measured-data dependencies through the [data contract](../measurement/data-contracts-and-provenance.md).
   Record agreement IDs separately for instrument handling, performance,
   recordings, images, identification, public derivatives, names and withdrawal.
   Use `R-ETHICS-OHA` and `R-RIGHTS-WIPO-TCE` as guidance; neither establishes
   ownership or grants rights. Keep private agreements outside distributable
   manifests. Uncertain derivative rights remain unresolved even if only fitted
   coefficients would ship.
3. Prepare the owner's concrete source/binary licensing decision and any
   jurisdiction-specific questions for qualified review. Record intended
   distribution and attribution text. Do not select a license, accept terms,
   contact rights holders, or publish while drafting this packet.

Retain an itemized permitted/excluded/pending ledger and proposed notices under
the work order's **proposed** `docs/governance/release-rights-checklist.md` and
`third_party/notices.md`. An independent reviewer compares notices and approvals
with every packaged item; missing provenance blocks that item or its dependent
release. `data.acquisition_manifest_integrity` checks limited metadata/hash rules,
not legal permission. `release.license_audit` and `release.asset_rights` remain
planned acceptance IDs. Hand decisions and exclusions to P18-T03/T06; technical
admissibility and redistribution approval remain separate findings.

## P18-T03: produce release artifacts through controlled builds

Use a protected release workflow when the repository owner has configured and
approved one. Separate untrusted pull-request testing from signing and
publication. Pin actions and dependencies; use least-privilege permissions
and avoid exposing secrets in logs or artifacts. Do not have a test job that
publishes artifacts merely because all local tests happened to pass.

Run clean builds for the supported platform/architecture matrix and retain
relevant configure, compile, test, and artifact metadata. Compare hashes where
reproducible builds are feasible, and document nondeterministic components such
as signing or timestamps if byte identity is not achieved. Preserve debug
symbols and symbol mapping according to privacy and distribution needs.

Create a software bill of materials and dependency provenance at the level
appropriate for the release. Inspect bundled content for accidental recordings,
private paths, credentials, test-only assets, and unlicensed resources. Reject
a build that passes tests but includes unexpected binary payloads.

### Execution recipe

**Question and route.** Does a clean rebuild reproduce the approved model and
package contents, including generated coefficients, without silently picking up
local files? Use `ENG-P18-T02` / `REQ-REL-002`, the artifact-contract chapter's
"Hashes and provenance boundaries", and `R-SDK-VST3-VALIDATOR` for exact
tool/plugin identity. Consult the installed CMake's version-matched official
manual for `CMakePresets` and `install`; search upstream release notes for the
pinned compiler/SDK's reproducibility controls. Preserve inspected sections and
observed differences; no build reproducibility is established here.

1. After rights review, define an isolated build per admitted OS, architecture
   and format with fixed source, toolchain, dependency hashes and environment
   inputs. [CMakePresets.json](../../CMakePresets.json) supplies headless presets;
   [engineering.yml](../../.github/workflows/engineering.yml) configures headless
   Linux, Windows and macOS checks plus Linux sanitizers. Neither is a release
   packaging/signing workflow. Record both JUCE switches explicitly: the reference
   diagnostic and `SETAR_ENABLE_JUCE_INSTRUMENT` are different optional targets.
2. In a later authorized execution, retain configure/build commands and logs,
   actual CTest results, expanded physical configuration, generated assets and
   package manifest. Compare a second independently prepared build with the first
   before signing. Compare sorted paths, hashes and embedded metadata; investigate
   timestamps, absolute paths and toolchain differences individually. If signing
   or packaging prevents byte identity, retain unsigned comparisons and explain
   residual differences without claiming identical binaries.
3. Inventory transitive shipped components and notices from the built package,
   then test outside the build tree. Use a frozen synthetic score and fixed
   sample rate, mode limits, gain and events to detect changed behavior. Keep
   numeric differences separate from measurement uncertainty; tolerances come
   from the applicable numerical/physical error budget, not an arbitrary
   allowance chosen after a mismatch.

Retain an SBOM, source/dependency/model digests, logs, inventory diff, raw comparison
tables and reasons for each nondeterministic component. Existing
`integration.report`, `integration.solver_evidence` and `integration.setar_play`
exercise bounded synthetic paths. Planned `release.rebuild_trace` and
`release.package_smoke`, and work-order paths `cmake/Packaging.cmake`,
`tools/release/manifest.py` and `tests/package/smoke_tests.py`, are not implemented
release machinery. Reject unreviewed assets, unexpected payloads, missing notices
or evidence attached to another hash. Hand the identified package to P18-T04;
publication and credentials remain separate future actions.

## P18-T04: qualify installation and migration

Test fresh installation, update from prior supported versions, rollback where
allowed, uninstall, rescanning by hosts, preset migration, missing asset
handling, and multiple installed plugin formats. Use clean user profiles or
machines where possible and record the exact environment. An installer
demonstration on the developer's machine is not sufficient.

Verify code-signing and notarization behavior where required, but do not store
signing keys in source or broad CI job permissions. If a platform workflow
cannot be exercised, mark its release support accordingly. Test failure
handling: interrupted asset download if offered, corrupted preset, revoked
permission, and incompatible prior state.

Provide a preservation path for user presets and calibration edits. Installing
a new physics engine must not silently reinterpret old parameters or overwrite
the only copy of a user's performance configuration. Document what migration
is automatic, what requires confirmation, and what cannot be preserved.

### Execution recipe

**Question and route.** Can a musician open a supported saved configuration
after installation or rollback without changing its physical meaning or losing
their only preset? Use `ENG-P18-T02` / `REQ-REL-002`, with state-boundary dependency
`REQ-PLG-002`. Read [state compatibility](../plugin/02-parameters-state-and-compatibility.md)
and `R-SDK-JUCE-AUDIOPROCESSOR`'s state/lifecycle methods; inspect the pinned
JUCE `AudioProcessorValueTreeState` source and official platform installation
documentation. Search `state restore schema unknown parameter ID` and the actual
host's `plugin scan cache installation` documentation. Verify platform requirements
when the real candidate exists.

1. Freeze package hash, OS/host versions, architectures, install destinations,
   prior supported versions and original preset bytes. Use isolated accounts or
   clean machines, with backups of user edits. No released installer or historical
   released-state corpus is established by this repository; source-generated
   prototype fixtures must be labeled accordingly.
2. Inspect [SetarProcessor.cpp](../../plugin/Source/SetarProcessor.cpp):
   `getStateInformation()` stores `SetarPrototypeState`, `schemaVersion = 1`,
   `tuningId` and `fretMapId`; `setStateInformation()` ignores malformed,
   foreign or other-version state, and an unknown preset ID keeps the restored
   choice index. That behavior is a migration risk, not an implemented future
   schema conversion. Record meanings of `outputGainDb`, `dynamicsScale`,
   `tuning` and `fretMap` from [SetarProcessor.h](../../plugin/Source/SetarProcessor.h).
3. Specify and later execute install, rescan, reopen, upgrade, rollback and
   uninstall cases for each supported format. Compare expected parameter values,
   model/asset identity and canonical-event renders before and after. Hold sample
   rate, gain, tuning, gesture offsets and analysis windows fixed. A future asset
   loader also needs missing/corrupt/revoked-asset cases; the present adapter
   rebuilds embedded synthetic presets and does not establish that loader.
4. Define migration as parse, versioned conversion, full validation, preparation
   and deliberate installation; keep originals and recovery instructions. Record
   any approved sonic change with units and rationale. Reject silent substitution,
   lost edits, partial loads or an unacknowledged reset of sounding strings.

Retain immutable state fixtures, installation logs, before/after parameter and
render diffs, failures and rollback evidence. An independent tester must reproduce
the result in the named host. `realtime.block_partition` and
`contact.gesture_replay` check core fixtures, not installer/state migration;
`plugin.state_roundtrip`, `plugin.malformed_state` and `release.package_smoke`
remain planned acceptance IDs. Hand supported transitions and blocked platforms
to P18-T05/T06 and P19-T04.

## P18-T05: write truthful release communication

State exactly which features are available and their physical/measurement
status. “Physically modeled” can describe a validated method with specified
components; “1:1 perfect recreation” would require a far stronger, generally
unattainable claim across specimens and gestures. Avoid unsupported claims of
commercial parity or affiliation with third-party brands.

Publish system requirements, supported hosts/platforms, known limits, asset
rights, accessibility information, state compatibility, and support routes.
Distinguish an experimental laboratory build from a production plugin. Real
screenshots should identify the build and show actually implemented features;
concept wireframes must remain labeled concepts.

Release notes should map major numerical or physical changes to tests and
audible consequences. Where calibrated assets change, explain whether the
change represents new measurements, corrected equations, a new specimen, or
an artistic option. This maintains scientific traceability and user trust.

### Execution recipe

**Question and route.** Would a reader infer a stronger instrument, host or
fidelity claim than the identified artifact has demonstrated? This task maps
to `ENG-P18-T01`, `ENG-P18-T02`, `REQ-REL-001` and `REQ-REL-002`. Use the
release-evidence chapter's "Quality summary for different readers" and the P17
claim-limited decision. For uncertainty language consult `R-METROLOGY-JCGM-100`
§7; for listening claims inspect the actual protocol and access limits under
`R-EVAL-ITU-BS1116-3` / `R-EVAL-ITU-BS1534-3`. Search the exact recommendation
edition plus `test conditions reporting`; neither recommendation certifies the
project or supplies a setar-specific equivalence threshold.

1. Build a sentence-level claim table: feature, candidate/model hash, physical
   domain, evidence class, exact test or measured-study record, uncertainty,
   failures, reviewer and allowed wording. Distinguish implemented, synthetic
   verification, host execution and specimen validation. Consult the actual
   [coverage registry](../../roadmap/executable-test-coverage.json), not a planned
   test name as evidence of a completed result.
2. Reconcile each sound or response claim with its mechanism. Current
   [config.hpp](../../include/setar/instrument/config.hpp) observation weights
   map body acceleration into a synthetic pressure observation; that is not
   calibrated spatial radiation or acoustic feedback. Preserve the synthetic
   preset label and adapter's uncompiled/unhosted status. A future measured claim
   must name specimen/setup, gesture range, microphone coordinates in m,
   calibration, held-out partition and uncertainty; recordings used for fitting
   cannot independently validate the same claim.
3. Prepare musician and engineering release notes from the same table. Explain
   audible changes, migration consequences, unsupported hosts and recovery,
   including negative and inconclusive study results. Match images and audio to
   the candidate; retained raw/derived manifests must reproduce plotted results.
   Draft attribution and third-party comparisons only within the rights and
   empirical decisions from P18-T02/P17.

An independent reader should trace each consequential sentence to its evidence
and identify a counterexample that would invalidate it. The current
`tools/validate_product_qualification.py` checks metadata/files, and
`data.acquisition_manifest_integrity` tests selected rejection rules; neither
authenticates a host run or listening outcome. Planned `release.asset_rights`
and `release.rebuild_trace` remain separate release gates. Reject unsupported
scope, absent calibration or aggregates hiding failed gestures; retain blocked
claims as exclusions. Hand the reviewed draft and claim table to P18-T06 without
publishing either.

## P18-T06: require explicit release approval

Assemble the release packet: claim matrix, final CI runs, host/device tests,
measurement validation summary, licenses/consents, manifests/checksums,
installation tests, issue exceptions, security review, and an owner approval.
Do not equate a signed binary with scientific qualification; signing asserts
publisher identity/integrity, not physical accuracy.

If a critical gate fails, narrow scope, repair the defect, or postpone release
without attaching a calendar deadline. Preserve the rejected candidate's
reproduction record and avoid quietly changing a published artifact in place.

### Execution recipe

**Question and route.** Does the evidence packet support this specific
distribution decision, and can a reviewer find every blocker before approval?
Use `ENG-P18-T01`, `ENG-P18-T02`, `REQ-REL-001` and `REQ-REL-002`. Revisit
`R-SDK-VST3-VALIDATOR` for what format conformance can establish,
`R-METROLOGY-JCGM-100` for reported uncertainty, and the
[13-gate product contract](../plugin/07-product-qualification-evidence-gates.md).
Research unresolved claims by exact artifact, source edition and failed gate;
do not restart a broad search to find favorable wording.

1. Assemble an index joining candidate source/dependency/model/package hashes
   to P18-T01 identity, rights decisions, clean builds, installation/migration
   results, P16 qualification and P17 claim dispositions. Include known defects,
   recovery procedures, reviewers, actual commands and original logs. Measured
   evidence needs permitted raw locations, calibration and derived lineage;
   public summaries must not expose private participant records.
2. Have reviewers independently sample packaged bytes, a reconstruction, a
   migration, a worst failing physical case and each consequential rights
   decision. Cross-check units, gain and observation conditions before accepting
   an acoustic comparison. An approved exception records its exact domain and
   consequence; it does not turn failure into a pass or waive an unsafe boundary.
3. In a later evidence review, run the existing
   `tools/validate_product_qualification.py` on the real record and use
   `--require-qualified` for its complete declared matrix. The blank
   [template](../../data/product-qualification-template.json) must fail that
   qualification request. A narrower research distribution needs a reviewed
   scope decision and compatible versioned evidence contract; deleting rows from
   the current template does not qualify it. Structural validity still needs
   independent execution and scientific review.

Record accepted, rejected, inconclusive or blocked disposition for the candidate.
`release.license_audit`, `release.asset_rights`, `release.rebuild_trace` and
`release.package_smoke` are planned gates; present CTest metadata checks do not
close them. A changed binary, dependency, model asset or claim reopens affected
decisions. Retain rejected candidates where rights permit and identify their
replacement rather than mutating their record.

**Handoff/stop checklist.** Require matching hashes, independently reviewed
claims/rights, actual supported-platform results, recoverable user state and a
named maintenance owner. A missing item yields a concrete next work order or
narrower proposed scope. The deliverable here is a reviewable packet. Licensing
adoption, signing, notarization, upload and external distribution are distinct
future actions requiring the owner's explicit artifact-specific approval and
approved workflow. After an approved release, hand its immutable identity and
limitations to P19; this recipe grants no such approval.

> Prepare a release-candidate evidence packet for the exact named artifacts.
> Resolve source, dependency, recording, mesh, participant, and binary rights
> independently. Build from a known revision, run the supported platform/host
> matrix, audit packaged bytes, and test installation plus preset migration.
> Require explicit owner approval for licensing, signing, publication, and
> external distribution. Make claims only within the validated domain and
> record all unsupported cases.
