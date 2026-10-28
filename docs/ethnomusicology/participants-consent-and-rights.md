# Participants, consent, data rights, and stewardship

## Scope and professional review

This is an engineering and research workflow, not a jurisdiction-specific legal
opinion or a ready-to-sign consent form. Obtain qualified institutional, ethics,
privacy, and legal review where applicable to the project, participants, and
intended distribution. The workflow's purpose is to prevent missing permissions
from being discovered only after the model and marketing depend on the data.

Primary guidance is indexed as R-ETHICS-OHA and R-RIGHTS-WIPO-TCE. The Oral
History Association's official ethics page is
<https://oralhistory.org/oha-statement-on-ethics/>. WIPO's official overview is
<https://www.wipo.int/en/web/traditional-knowledge/traditional-cultural-expressions/index>.
These sources motivate careful participation and rights review; they do not
grant permission for this project or resolve every applicable legal issue.

## Distinct permissions

Separate permission to handle an instrument, permission to record a person,
permission to analyze the data, permission to share internally, permission to
publish recordings or images, and permission to distribute derived model assets.
Also separate use of a participant's name, likeness, biography, and endorsement.
A single informal “yes” to a recording session should not be stretched into all
of these uses.

The underlying musical work, arrangement, performance, recording, images, and
technical measurements can involve different interests and agreements. A
traditional repertoire label does not establish that a particular recording or
arrangement is unrestricted. A participant's approval may not cover every
third-party work they perform. Resolve the actual intended material and use
through the appropriate rights process before public release.

Do not assume a transformation removes restrictions. A parameter set fitted from
a restricted performance, a gesture distribution, or an annotated video excerpt
may need an explicit derivative-use agreement. The project should ask for the
uses it actually needs in understandable language, not rely on a technical
argument after collection that “only coefficients remain.”

## Plain-language explanation

Explain the physical-modeling goal, what will be recorded, what sensors or markers
will be used, what tasks are requested, and what the data will influence. Clarify
that the synthesis engine is intended to generate sound through modeled physics,
not replay the participant's notes, while acknowledging that recordings may be
used for calibration, comparison, demonstrations, or other agreed purposes.

Describe known limitations and possible future uses precisely enough for an
informed decision. Do not say the data are “anonymous” if identifiable voice,
face, hands, instrument details, or performance style remain. Do not promise
unlimited withdrawal after unrestricted public distribution if the project cannot
retrieve copies. Agree on what can be changed or withdrawn at each stage and
document the practical limits.

Use a language the participant understands and provide competent translation
when needed. Allow questions and time to consider the agreement without pressure.
Check understanding through conversation rather than treating a signature as
proof that every technical implication was clear.

## Participation and comfort

Design tasks around participant comfort and safe instrument handling. Permit
breaks, refusal of particular gestures, and stopping the session without pressure.
Avoid requesting painful repetition, unusual force, or personal grooming changes
as if they were routine technical requirements. If a measurement needs a
substantial intervention, explain it separately and obtain explicit agreement.

Do not condition respectful treatment or agreed compensation on producing
favorable evaluations. Participants should be able to criticize the model,
decline a public example, or identify an unrepresentative take. Preserve those
responses as part of the research record under the agreed privacy conditions.

If institutional review is required, obtain it before the relevant activity.
This repository cannot declare an exemption on behalf of an institution. The
project lead should document which review process applies and who has authority
to approve the study.

## Compensation and credit

Discuss compensation, expenses, attribution, access to results, review roles,
and any continuing collaboration before work begins. A research honorarium,
licensing fee, consulting agreement, and co-development arrangement are different
relationships. Record the actual arrangement and avoid vague promises of future
visibility or commercial success.

Credit should describe the contribution accurately. A musician who supplies
reference gestures is not automatically an endorser. A scholar who reviews
terminology is not responsible for every model claim. A maker who lends an
instrument is not necessarily approving modifications or use of their name in
preset branding. Seek specific approval for public attribution and wording.

## Data minimization

Collect only personal information needed for the research and its accountability.
Use participant identifiers in analysis data and keep identity linkage in a
restricted record. Avoid including addresses, contact details, payment records,
or unrelated biography in the measurement repository. Background information
should be collected at the resolution needed to interpret expertise and coverage,
not as an open-ended personal profile.

Video may reveal more than the intended hand motion. Review framing, background,
audio conversation, and visible documents before recording. Create authorized
cropped or blurred derivatives for analysis or release when appropriate, while
recognizing that de-identification can be incomplete. Do not promise that a hand
video or distinctive instrument cannot be recognized.

## Access and storage policy

Define who may access raw recordings, interviews, consent records, derived
annotations, and public examples. Use storage controls that enforce those
permissions. A filename marked `private` is not access control. Keep restricted
assets out of public source repositories and build artifacts.

Specify retention, backup, deletion, and breach-response procedures in a form the
project can actually operate. A research packet can preserve reproducibility
through restricted access rather than public release of every raw file. If data
cannot be retained under the agreed terms, document what analyses remain
reproducible and what limitations follow.

Do not upload participant data to an external analysis or generative service
unless the agreed scope and institutional policy permit that use. A convenient
tool integration does not create permission to transfer recordings to another
processor. Record approved processing locations and services as part of the
data-flow review.

## Rights ledger

Create a permission record linked to every relevant asset. It should identify
the agreement, covered participants and works, permitted purposes, access groups,
distribution conditions, attribution requirements, review requirements,
restrictions, and contact route for questions. Store the agreement itself in
the appropriate restricted location; public manifests can reference its
identifier without exposing signatures or personal details.

Use explicit status values such as `pending_review`, `internal_analysis_only`,
`approved_public_derivative`, and `approved_public_raw` where they reflect the
actual agreement. Do not infer a broader status from a narrower one. Packaging
tools should require an affirmative release decision for each asset and its
dependency chain.

```yaml
permission_id: PERMISSION-PENDING
status: pending_review
participant_ids: []
covered_asset_ids: []
permitted_uses: []
prohibited_uses: []
public_attribution_text: null
review_required_before_release: null
derivative_parameter_distribution: null
commercial_distribution: null
agreement_record_location: null
```

This template grants nothing. Null fields are unresolved requirements. Never
fill them with defaults that convert silence into permission.

## Review before release

Prepare the exact materials proposed for release: audio excerpts, video crops,
interview quotations, screenshots, preset names, contributor credits, and model
descriptions. Review them against the agreement and with participants where the
process requires it. Approval of one excerpt should not automatically cover a
different edit or context.

Check whether an edit changes meaning. A quotation criticizing one prototype
should not be shortened into apparent praise for the product. A demonstration
of an undesirable technique should not be presented as the participant's normal
performance. Keep an approved-excerpt record with the exact version and intended
use so later packaging cannot accidentally substitute an earlier draft.

## Changes of purpose

If the project changes from internal research to commercial distribution, from
audio analysis to public video, or from specimen modeling to performer-style
emulation, reassess permission. Do not assume the original scope covers a
materially different purpose. Seek renewed agreement or restrict the use to what
was actually authorized.

Likewise, a new collaborator or external processor may change the data-access
scope. Update the ledger and review before transfer. The research team should
not share restricted files informally to solve an engineering bottleneck.

## Handling withdrawal or restriction

Follow the agreed and applicable process when a participant requests withdrawal,
correction, or narrower use. Identify affected raw and derived assets through the
provenance graph. Stop new distribution while the request is reviewed. Do not
promise that every already-distributed copy can be recovered; explain practical
limits through the agreed process and qualified advice where needed.

If deletion or restriction affects a published result, update the reproducibility
statement without exposing private reasons unnecessarily. A tombstone can record
that an asset is unavailable under its rights policy. Do not retain prohibited
content secretly to preserve a convenient benchmark.

## Acceptance and stop conditions

The gate passes when participation is informed, scope is documented, data flows
are approved, access controls exist, and each intended release has a traceable
permission decision. Stop acquisition or release when scope is ambiguous,
participants are uncomfortable, a third-party work is unresolved, or a requested
use exceeds the agreement. Continue with synthetic fixtures or newly authorized
material instead of pressuring participants to remove the blocker.

Respectful stewardship is not separate from technical quality. It determines
whether the evidence can be retained, reviewed, shared, and used in the final
instrument at all. The next action is to prepare project-specific documents for
qualified review, not to treat this chapter as a signed legal instrument.
