# Parameter or model promotion record

Use when research is proposed to change a model, configuration, or preset. A
completed form is not executable validation. Follow the
[workflow](../docs/research/research-to-model-workflow.md) and
[data contract](../docs/measurement/data-contracts-and-provenance.md).

## Identity and proposed scope

- Promotion ID/revision and linked findings:
- Phase/task, exact engineering work orders, and existing requirements:
- Current and proposed model/package/configuration versions:
- Source commit and analysis revision:
- Intended use: synthetic verification / research comparison / measured specimen /
  broader instrument family / distribution candidate:
- Specimen/configuration and validity domain:
- Decision: pending / accepted for scope / rejected / deferred:

State what changes and which prediction it improves. “Better tone” needs a
comparison protocol and does not replace a physical explanation.

## Quantity and model contract

| Quantity/component | Value/distribution | SI unit/frame/sign | Evidence kind | Source/raw record | Domain and uncertainty |
| --- | --- | --- | --- | --- | --- |
| To complete | Unknown stays null | Include frequency/tensor conventions | Synthetic/published/measured/inferred | Exact locator | Covariance reference |

- Governing equation, boundaries, assumptions, and approximation:
- Fixed, estimated, nuisance, and unidentified quantities:
- Uncertainty/covariance and propagation method:
- Admissibility constraints and rejection behavior:
- Mixed-evidence display/export policy:
- Old values retained for regression/rollback:

The current `Provenance` enum cannot hold per-quantity lineage/uncertainty. Name
the evidence sidecar. One measured length does not make synthetic contacts/body
measured.

## Provenance and rights

- Raw IDs, original hashes, immutable locations:
- Specimen/configuration/session/calibration/permission references:
- Source IDs, inspected locations, access status:
- Extraction/conversion/fitting/reduction revisions and reproduction commands:
- Input/output hashes:
- Permitted analysis, sharing, derivative, and distribution scope:
- Restricted inputs and public-package exclusions:

Public availability alone is incomplete rights evidence. Leave distribution
undecided if not established; preserve restrictions through derived packages.

## Code and runtime mapping

| Research quantity | Current file/symbol | Assignment/change | Conversion/constraint | Independent meaning check |
| --- | --- | --- | --- | --- |
| To complete | Existing or explicitly proposed | Exact field/interface | SI/frame/normalization | To complete |

- Speaking length versus total string/afterlength interpretation:
- Modal mass/normalization, port residues, observation interpretation:
- State migration, reset/transition, incompatible-package policy:
- Offline import/preparation and callback-safe installation plan:
- Failure reporting and rollback:

Inspect current implementation paths before creating a new subsystem. Resonance
frequencies alone do not establish causal/passive bridge behavior.

## Verification and validation

| Check | Independent reference/partition | Actual command/procedure | Result/artifact | Scope and unrun conditions |
| --- | --- | --- | --- | --- |
| Units/admissibility | To complete | To complete | To complete | To complete |
| Synthetic recovery/convergence | To complete | To complete | To complete | To complete |
| Energy/passivity/causality as applicable | To complete | To complete | To complete | To complete |
| Identifiability/uncertainty | To complete | To complete | To complete | To complete |
| Independent instrument prediction | To complete | To complete | To complete | To complete |
| Regression/runtime/host as applicable | To complete | To complete | To complete | To complete |

- Acceptance metrics, units, thresholds, and justification fixed before testing:
- Negative cases and expected rejection:
- Data reused for selection and new holdout required:
- Residual structure, alternatives, failed conditions:
- Reviewer assessment of independence:

Mark future tests planned and unexecuted checks unrun. Synthetic preset checks
establish their assertions, not measured specimen validity.

## Decision and downstream effect

- Accepted scope or exact rejection/deferral reason:
- Claims supported, unsupported, narrowed, or withdrawn:
- Scientific gates still open:
- Packages/fits/reports/presets/documentation made stale:
- Reanalysis and migration verification required:
- Rollback reference and compatibility limits:
- Reviewer/date and disagreement:
- Next measurement or implementation action:

Version changes to units, calibration, geometry, normalization, meaning, or valid
domain. Preserve earlier records. Synthetic verification can be accepted while
measured fidelity and release remain blocked; record outcomes separately.
