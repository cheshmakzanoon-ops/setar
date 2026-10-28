# P01 — Audit sources, data, and instrument identity

**Status:** specification with an initial source audit in the research volume.
**Prerequisite:** P00. **Exit evidence:** a usable evidence register, admissible
parameter policy, acquisition plan, and defined held-out observations.

## Objective

Determine exactly what is known, what is merely asserted, and what is unavailable
before encoding physical constants or geometry. The literature establishes useful
methods and observations; it does not provide a complete measured digital twin
of an identified setar. A public paper's CAD illustration, a quoted wood species,
and a table of resonant frequencies cannot supply all model inputs.

Read [research](../research/README.md), [measurement](../measurement/README.md),
and [geometry](../geometry/README.md). Use the source ledger's original links,
access status, and exact page/table identifiers. Do not treat this roadmap as a
replacement for the publications.

## P01-T01: build a source-to-claim map

For every important construction or acoustic claim, create a record containing
the claim, source identity, exact location, access status, evidence type, and
limitations. A publisher abstract supports a narrower claim than a full paper;
an author manuscript may differ from the final publication. Record the version
actually read. A dead link is not evidence that the paper does not exist, but
it does block verification of its unavailable details.

Distinguish direct measurement, method description, review statement, artist
practice, manufacturer specification, and your own proposed model. Mark whether
the source refers to Iranian setar, another long-necked lute, or a generic string
instrument. Transferable mathematics should not be presented as setar-specific
empirical validation. Preserve spelling variants for search while keeping one
canonical instrument identity in the project.

Review representative claims by independently locating their source paragraphs
or figures. If the source does not support the specificity of the claim, narrow
the claim. Do not add a citation and assume it fixes an overstatement.

## P01-T02: quarantine inconsistent material data

Read the published-parameter audit before importing the 2018 setar material
table. For a linear orthotropic elastic model, convert all entries into a single
unit system and construct the compliance/stiffness representation using one
declared Poisson subscript convention. Check reciprocity, symmetry, finite
positive moduli, necessary positivity conditions, and positive definiteness.

Do not repair a negative sign, exchange axes, or change a decimal point simply
because the result becomes plausible. Preserve the raw transcription. Produce
an audit report showing which relation fails and the size of the discrepancy.
Possible explanations include reporting convention, transcription, units,
measurement uncertainty, or source error; the audit does not establish which
explanation is correct. Request clarification or obtain independent measurements.

Use synthetic admissible matrices to test the importer and solver while real
data are quarantined. Verify that the importer rejects a deliberately nonsymmetric
or indefinite example and reports the problematic field. A Cholesky failure
must not trigger silent diagonal regularization in the physical model.

## P01-T03: select a specimen and acquisition strategy

Decide whether to seek the original research specimen/data, characterize a new
instrument, or maintain a research-only synthetic model until acquisition is
possible. Record the decision and its effect on claims. Combining one maker's
geometry with another specimen's damping and a third player's recordings creates
a composite model, not an exact reconstruction of any of them.

Assign an immutable specimen identifier before measurements. Record maker/owner
information only with the appropriate permission. Attach strings, bridge,
frets, repairs, finish, and setup as versioned configuration records. Changing
strings or moving a bridge changes the measurement state; it is not a new file
name for the same experiment. Use the measurement protocols to distinguish
noninvasive observations from modifications that need additional permission.

## P01-T04: request useful research files

Prepare a specific request listing raw point clouds, coordinate system, interior
thickness information, editable CAD/mesh, material axes, mesh element definitions,
boundary constraints, preload state, raw force/response recordings, sensor
calibration, fitted modes/residues, and redistribution terms. Ask for what exists
without implying the authors are obliged to provide it. A processed image is
useful background but does not replace those numerical inputs.

Do not send messages automatically from this repository. An owner-authorized
person may contact the researchers. Log the technical outcome without publishing
private correspondence. If materials are offered only for private research,
separate access controls from the public repository and ensure CI can still
exercise synthetic fixtures. Never commit restricted data to make a test green.

## P01-T05: design independent validation before fitting

Create separate calibration and validation partitions before running parameter
optimization. Consider partitions by string, fret, gesture, dynamic range,
player, support condition, and specimen, depending on the intended claim.
Randomly splitting adjacent samples from one note is not meaningful independence.
Keep a record of every dataset used to select parameters or model structure,
including informal listening during development.

Choose observables that identify different mechanisms. String partial spacing
can constrain stiffness; amplitude decay can constrain losses; bridge mobility
can constrain body coupling; radiation observations constrain output mapping.
One microphone recording rarely identifies all these uniquely. Design controlled
experiments that reduce ambiguity rather than compensate it with extra knobs.

## P01-T06: prepare data contracts

Use a manifest with units, coordinate axes, sample rate, clock information,
sensor orientation, calibration factors, uncertainty, configuration identifier,
permissions, source version, and raw-data hashes. Preserve raw measurements and
derived processing as separate immutable records. Include sign conventions for
force and velocity, because a reversed channel can invalidate a passivity check.

Reject missing or ambiguous units at the import boundary. Test wrong sample rate,
inconsistent channel lengths, clipped force impulses, invalid hashes, duplicate
identifiers, and mismatched specimen configuration. A warning that is easy to
ignore is inappropriate when the missing field changes the physical meaning.
Provide a synthetic example manifest that exercises the schema without imitating
real participant data.

## Outputs, review, and handoff

The output is a source ledger, parameter quarantine list, acquisition decision,
measurement plan, schema examples, and held-out validation design. Independent
review should reconstruct one accepted fact and one rejected parameter from the
original sources. The reason for rejection must be technical and reproducible,
not simply that the value differs from a preferred answer.

> Audit the supplied source or dataset under P01. Report access and license
> status, exact claims supported, units and conventions, internal consistency
> checks, missing metadata, and whether it can enter calibration or validation.
> Do not invent missing values or copy copyrighted material into the repository.
> If unusable, propose the smallest measurement or clarification that resolves
> the blocking uncertainty. Keep the original record unchanged.

P02 may proceed with synthetic fixtures. P07 may proceed when specimen access and
geometry/material permissions exist. Neither inherits a claim of exact setar
reproduction merely because the source audit is complete.
