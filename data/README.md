# Measurement data boundary

This repository currently provides protocols and schema examples, not a measured
setar corpus. Do not infer the existence of calibrated recordings, CAD, FRFs,
participant consent, or reference parameter sets from the presence of this
directory. The source ledger explicitly distinguishes public articles from
unverified underlying assets.

The [blank specimen template](specimen-template.json) contains four
**unmeasured** physical string placeholders and explicit `null` inputs. It is
not a validated asset and must never be relabeled as measured. Its safe blank
state is checked by `tools/audit_implementation.py`; the
[specimen completeness matrix](../docs/measurement/specimen-completeness-matrix.md)
explains how real measurements, uncertainties and permissions would fill a
*separate* admitted asset.

Recordings are reference and validation data. They are not sampled-note playback
assets for the synthesis engine. Physical parameters and justified reduced
models may be derived from measurements when provenance, rights, uncertainty,
and independent validation are preserved.

## Admission policy

Before adding real data, read
[`docs/measurement/data-contracts-and-provenance.md`](../docs/measurement/data-contracts-and-provenance.md)
and
[`docs/ethnomusicology/participants-consent-and-rights.md`](../docs/ethnomusicology/participants-consent-and-rights.md).
Each asset needs an identifier, checksum, format, units or channel definitions,
specimen/configuration linkage, acquisition provenance, quality status, and an
affirmative rights decision. Unknown rights mean no public distribution.

Keep raw assets immutable. Derived files name their inputs, transformation,
software version, settings, calibration, and output checksum. Corrections create
new metadata or derived versions instead of overwriting the only original.
Restricted participant video, signatures, contact information, and private
agreements do not belong in a public source repository.

## Proposed organization

The following paths describe a logical organization to implement when data exist;
their listing does not claim that the directories or assets are already present.

| Logical area | Contents | Distribution rule |
| --- | --- | --- |
| `schemas/` | Versioned machine-readable contracts | Project code/documentation license |
| `synthetic/` | Generated fixtures with known answers | Explicitly marked nonmeasurement |
| `manifests/` | Rights-cleared asset indexes and checksums | No private participant details |
| `calibration/` | Approved calibration records or public references | Check third-party terms |
| `derived/` | Approved physical parameters and reduced models | Preserve input restrictions |
| `validation/` | Frozen test manifests and approved examples | Protect independence and rights |
| External restricted storage | Raw participant data and private agreements | Enforced project access policy |

Large or restricted assets may be stored outside Git with stable references and
checksums. A pointer is not permission to download or distribute the target.
Document access requirements without embedding credentials, signed access URLs,
or personal information in the repository.

## Synthetic fixtures

Synthetic data must state the generator, parameters, random seed if any, units,
and expected result. Name them so dashboards and reports cannot confuse them
with instrument observations. They can test calibrations, parsers, spectral
estimators, solvers, and invalid-input handling. They cannot establish setar
authenticity or replace unavailable participant evidence.

Do not create plausible-looking fake measurements to satisfy a schema. A blank
template should use nulls and an explicit proposed status. A synthetic fixture
should be intentionally identifiable as such and tied to a numerical test.

## Release check

Before packaging an asset, validate its dependency graph, permissions, metadata,
checksums, and claim scope. Ensure that calibration and independent-test groups
remain separated and that repeated derivatives cannot leak across partitions.
Confirm that examples do not imply endorsement or reproduce an unapproved work.

The release record should say what the data support and what they do not. If
required metadata or permission is missing, quarantine the asset and continue
with an approved synthetic test or a narrower analysis. A clean repository with
honest missing-data gates is more useful than a complete-looking collection whose
measurements cannot be interpreted or lawfully shared.
