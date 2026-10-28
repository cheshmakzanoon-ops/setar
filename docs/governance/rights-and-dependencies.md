# Rights, dependencies, and distribution decisions

## No license decision is implicit

The repository owner has requested engineering work and a push to this repository.
That authorizes the requested changes, not an arbitrary open-source license,
commercial contract, publication of third-party recordings, or adoption of a
copyleft dependency. This bootstrap does not select a redistribution license for
the project's original work. The owner must make that decision explicitly before
inviting reuse under specific terms or distributing products.

This is an engineering checklist, not legal advice. Questions about a dependency
license, sample consent, attribution, patents, trademarks, or redistribution
should be resolved with the relevant rights holder or qualified advice. Record
the decision and exact license text for the pinned version; do not rely on a
summary in an old tutorial.

## Source material

Link to papers, DOI records, author manuscripts, and institutional repositories.
Do not copy complete papers into the source tree merely because a PDF is
downloadable. Open-access availability and a license permitting redistribution
are different facts. Preserve author attribution and version identity when a
licensed figure or dataset is legitimately used. This repository's engineering
prose should be original, with short factual references and independently
developed procedures rather than a substitute for the source publications.

For each numerical table used as data, record source, table/page, transcription,
units, license/permission status, uncertainty, and transformations. A technically
usable value may still lack permission for a particular redistribution. Conversely,
a legally reusable table may be physically inconsistent and require quarantine.
Rights review and scientific review are separate gates.

## Dependency inventory

Maintain a lock record for each external dependency with upstream URL, exact
version or commit, checksum, license location, build purpose, runtime linkage,
redistributed components, and update procedure. Separate development-only tools
from shipped binary dependencies. A header-only library can still impose license
obligations. A statically linked dependency is not automatically exempt from
notice requirements.

JUCE is optional at the core level. Before enabling or shipping a JUCE target,
review the licensing options applicable to the selected version and intended
distribution. Do not assume that the availability of source code authorizes every
commercial use. VST3 SDK licensing must likewise be checked at the exact chosen
revision; historical articles may describe arrangements that no longer match
current upstream files. The plugin volume records official sources and a
version-specific audit procedure.

## Measurement participants and makers

Obtain consent for recording a performer and for the intended uses of those
recordings: private calibration, public examples, model training/identification,
redistribution, or commercial demonstration are not interchangeable. Identify
the instrument owner and whether destructive material sampling or modification
is permitted. Do not treat lending an instrument as permission to CT-scan, alter,
publish, or retain every resulting dataset indefinitely.

Use pseudonymous participant identifiers in public test manifests. Keep contact
details, payment records, signatures, and private communications outside the
public repository. Published contact information may be used to request research
materials with authorization; it is not permission to imply a collaboration.
Never send requests or offer compensation automatically as part of a build script.

## Product claims and third-party names

The project aims for a high standard of physical-modeling fidelity and
responsiveness. Mentions of Pianoteq explain the desired quality aspiration only.
Do not reproduce its proprietary internals, presets, marketing assets, or sound
libraries; do not imply endorsement, equivalence, or an affiliation. Comparative
claims require a fair documented test and a reviewed description of the tested
conditions.

The identity of a luthier or performer should not be used in a preset name unless
the permission and accuracy implications have been addressed. A measured
instrument model may be named using a neutral specimen identifier while those
issues are unresolved.

## CI and publication safety

Use the minimum token permissions needed for checkout, tests, and artifact upload.
Never put credentials or raw consent forms into reports, screenshots, build
logs, archives, or workflow artifacts. Do not run untrusted fork code with
repository secrets through `pull_request_target`. Signing and publication jobs
require a separately approved design and should not run on arbitrary pull
requests. The initial workflows test and upload diagnostics; they do not publish
a commercial plugin or change repository administration.

Before release, audit the actual artifact contents rather than only source
licenses. Check transitive libraries, embedded fonts/images, example MIDI, test
audio, model parameter provenance, notices, and installer scripts. Produce an
inventory that another maintainer can reproduce from the exact release commit.
