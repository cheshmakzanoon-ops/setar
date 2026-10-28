# Reproducibility, missing-data gates, and research handoff

## Reproducibility has several meanings

Bitwise repeatability of a deterministic executable is useful but narrower than
scientific reproducibility. A second analyst should be able to recompute a result
from preserved data and methods. A second acquisition should characterize the same
specimen state within the declared repeatability uncertainty. A second specimen
may legitimately differ. A second player may reveal a missing technique rather
than an error. Reports must state which form of reproducibility they demonstrate.

The project must preserve enough provenance to distinguish software drift from
instrument drift. Changes in string condition, support, humidity, tuning, nail
shape, or bridge position can alter observations without any code change. The
absence of a calendar deadline does not remove this need: every state transition
still requires a recorded cause and ordering.

## Research packet

Each accepted analysis packet contains immutable raw-data identifiers and checksums,
a machine-readable manifest, calibration records, consent and rights references,
specimen/configuration identifiers, source claims, transformation definitions,
software versions, numerical settings, random seeds, and the final report. Avoid
embedding personal participant identities in analysis filenames. Store the private
identity linkage separately under the approved access policy.

Preserve intermediate outputs when they are expensive or impossible to reproduce,
but do not confuse a cache with an authoritative input. A derived spectrum should
name its raw channels, window definition, averaging, sample rate, channel delays,
and correction chain. A mesh should name its scan, registration transform,
surface processing, thickness interpretation, and mesher settings. A reduced body
model should name the parent model or measured response, fitting objective, ports,
normalization, passivity evidence, and validity band.

The packet must not depend on a private notebook cell whose state exists only in
one analyst's process. Export the ordered procedure or an executable pipeline.
Document unavoidable manual actions, such as labeling a contact-release frame,
with decision criteria and inter-annotator checks.

## Reproduction layers

Start with schema and unit validation. Reject corrupt or ambiguous inputs before
performing expensive analysis. Next reproduce derived observables from raw data:
calibrated force, velocity, pressure, displacement, spectra, and event boundaries.
Then reproduce parameter fitting and its uncertainty analysis. Finally reproduce
the held-out predictions and the decision report.

Do not require bitwise equality when numerical libraries or hardware legitimately
change floating-point ordering, unless the implementation promises it. Define
tolerances in meaningful quantities and provide a strict deterministic path for
regression where useful. Tolerance selection must be tighter than the claimed
scientific effect while remaining above expected roundoff. A plot that “looks the
same” is not an adequate numerical comparison.

For each layer, maintain at least one tiny synthetic fixture with an independently
known answer. Examples include a scalar oscillator response, a known channel
delay, a controlled gain error, and a two-mode response with specified damping.
These fixtures test the analysis pipeline. Label them so they cannot be mistaken
for setar measurements in a dashboard or release note.

## Gate: source readiness

Pass when the source identity, relevant section, access status, and applicability
are recorded. Critical numerical extractions need unit and convention checks.
Unresolved contradictions remain explicit. Fail promotion if the only evidence is
a secondary abstract, if the intended source cannot be inspected, or if an
inconsistent table has been silently repaired.

Permitted work while blocked includes deriving equations, building validators,
testing generic synthetic fixtures, and preparing a precise acquisition request.
Blocked work includes labeling an unverified parameter set as measured, claiming
reproduction of unavailable CAD, or distributing source assets without rights.
This distinction keeps progress possible without weakening the claim boundary.

## Gate: specimen and rights readiness

Pass when ownership or lending authorization, handling restrictions, permitted
measurements, restoration expectations, and data uses are documented. Identify
which interventions require renewed approval: string changes, fret movement,
adhesive markers, fixtures, disassembly, or controlled environmental exposure.
Participant recording and distribution permissions are separate from instrument
access permission.

Stop when an intervention exceeds authorization, the instrument shows distress,
or the necessary consent scope is unclear. Do not substitute another person's
recording or scrape a public performance to fill the missing data. Continue only
with approved noninvasive work or synthetic tests until the scope is resolved.

## Gate: acquisition readiness

Pass after channel identities, units, gains, polarity, latency, sample-clock
behavior, noise floor, saturation limits, and calibration validity are verified.
Demonstrate a small repeatable measurement after complete repositioning. Quantify
sensor loading or establish a justified upper bound. State the usable frequency
and amplitude domain.

Stop if any physical channel is uncalibrated but required for the claim, if units
cannot be recovered, if clipping affects the event, or if synchronization error
exceeds the event-time uncertainty budget. A microphone with an unknown absolute
gain may still support a carefully scoped relative analysis; it cannot support
absolute radiativity without further calibration. Record the narrowed scope rather
than discarding all possible value or pretending nothing is missing.

## Gate: data integrity and partition readiness

Pass when every file has a checksum, immutable raw-data location, rights status,
specimen/configuration linkage, and quality classification. Freeze calibration,
development-validation, and independent-test partitions by meaningful groups.
Keep repeated takes, derived versions, and overlapping segments linked so that
near-duplicates cannot cross partitions unnoticed.

Stop if the same gesture appears in both fitting and testing under different
filenames, if test identities have leaked into manual tuning, or if excluded trials
were removed without a reason. Retain excluded data with flags where rights and
storage policy permit. Exclusion is an analysis decision that should be reviewable,
not a deletion that makes inconvenient evidence disappear.

## Gate: parameter admissibility and identifiability

Pass when imported values are finite, dimensionally defined, physically admissible,
and traceable. Fitted values need sensitivity and confounding analysis sufficient
for the claim. Unidentified parameters must remain uncertain, fixed by declared
assumption, or removed from the exposed parameterization.

Stop if a fit requires nonpassive mechanical behavior without an actual active
source, if stability relies on hidden clipping, or if a parameter leaves its
justified domain. Do not cure this by renaming an invalid quantity an “artistic
control” while retaining scientific claims. Artistic controls can exist, but their
mapping must be separately designed and safely bounded.

## Gate: independent prediction

Pass a scoped validation claim only after the model and analysis are frozen and
independent data are evaluated according to the preregistered criteria. Report
both successful and unsuccessful conditions, confidence intervals, and the tested
population. Physical metrics and musical judgments answer complementary questions;
neither should erase a failure of the other without explanation.

Stop a broad fidelity claim if the model was evaluated only on fitted notes, one
familiar player, or synthetic references. The software may still be released as
an experimental research instrument with explicit limitations. A successful build,
a stable oscillator, and a pleasing demonstration are not evidence of validated
setar fidelity.

## Change control

Give every accepted parameter set a version and dependency fingerprint. A change
to geometry, calibration, channel sign, sample rate interpretation, or source
extraction can invalidate downstream fits even if their code has not changed.
Maintain a dependency graph so affected artifacts are marked stale. Never let a
cached report silently refer to an earlier calibration while showing the newest
model's name.

Classify changes by their evidential effect. Editorial clarification may not need
reanalysis. A corrected unit certainly does. A new test set cannot be merged into
the old results without preserving which model version saw which data. If a
released claim must be withdrawn, record the correction and its cause plainly.
Do not preserve a flattering claim merely because the implementation interface
has become inconvenient to change.

## Reproduction exercise

Assign a contributor who did not create the packet to reproduce one decisive
result in a clean environment. Provide the packet and documented commands, not
informal hints. Ask them to record ambiguities before receiving assistance. Then
resolve the ambiguities in the procedure itself. The exercise should include a
known failure fixture to confirm that the pipeline rejects bad data rather than
always producing a report.

For hardware replication, use the same specimen only when the original state is
recoverable and safe to reproduce. Otherwise state the difference and compare
appropriate distributions rather than demanding identical traces. Report fixture
reconstruction and coordinate-registration uncertainty as part of the replication.

## Final research handoff

The handoff begins with what is supported, what remains uncertain, and what must
not be claimed. Include the smallest accepted parameter package, its validity
domain, unresolved mechanisms, and the experiment most likely to change the next
architecture decision. Provide source and asset identifiers rather than copying
large datasets into prose.

The receiving implementation team must be able to validate the package without
contacting the original analyst. If it cannot, the packet is incomplete. The
next action is to repair the missing metadata or reproducibility path, not to
relax the scientific gate because integration work has already begun.
