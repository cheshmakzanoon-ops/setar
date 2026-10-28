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

## P18-T06: require explicit release approval

Assemble the release packet: claim matrix, final CI runs, host/device tests,
measurement validation summary, licenses/consents, manifests/checksums,
installation tests, issue exceptions, security review, and an owner approval.
Do not equate a signed binary with scientific qualification; signing asserts
publisher identity/integrity, not physical accuracy.

If a critical gate fails, narrow scope, repair the defect, or postpone release
without attaching a calendar deadline. Preserve the rejected candidate's
reproduction record and avoid quietly changing a published artifact in place.

> Prepare a release-candidate evidence packet for the exact named artifacts.
> Resolve source, dependency, recording, mesh, participant, and binary rights
> independently. Build from a known revision, run the supported platform/host
> matrix, audit packaged bytes, and test installation plus preset migration.
> Require explicit owner approval for licensing, signing, publication, and
> external distribution. Make claims only within the validated domain and
> record all unsupported cases.
