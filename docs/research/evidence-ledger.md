# Evidence ledger and source governance

This registry records a bounded source inspection on 2026-10-28 and defines how
subsequent evidence is admitted. Access status describes that inspection, not a
guarantee of future availability. Source publication dates and access dates are
different fields; search-engine crawl dates are not publication dates. No external
paper, photograph, CAD file, or raw laboratory dataset is redistributed here.

## Initial source register

### R-SETAR-PEDRAMMEHR-2018

S. Pedrammehr, N. Jafarzadeh Aghdam, S. Pakzad, M. M. Ettefagh, and M. Homayoun
Sadeghi, *A study on vibration of Setar: stringed Persian musical instrument*,
Journal of Vibroengineering 20(7), 2680–2689 (2018).

- DOI: <https://doi.org/10.21595/jve.2018.19505>.
- Publisher HTML: <https://www.extrica.com/article/19505>.
- Publisher PDF: <https://www.extrica.com/article/19505/pdf>.
- Inspected locations: sections 3.1, 3.2, 4, 5; Table 1; rights notice.
- Status: primary full text accessible; Table 1 visually checked on PDF page 4,
  printed page 2683.
- Rights: the publisher identifies the article as CC BY 4.0. This does not verify
  availability or licensing of separate scans, simulation inputs, or datasets.
- Bounded evidence: the study combines structural modeling and modal experiments;
  the printed material table requires an admissibility audit before reuse.
- Decision served: investigate geometry, material axes, and mechanical responses,
  while quarantining unresolved numerical imports.

### R-SETAR-MANSOUR-2015

H. Mansour, *Modal Analysis of the Setar: A Numerical–Experimental Comparison*,
Journal of Vibration and Acoustics 137(6), article 061006 (2015).

- DOI: <https://doi.org/10.1115/1.4030863>.
- Status: bibliographic identity checked against the reference list in
  R-SETAR-PEDRAMMEHR-2018; direct DOI retrieval returned an access error.
- Full text: not verified in this inspection. A search-result abstract does not
  count as a reviewed methods section.
- Rights: no redistribution grant verified.
- Decision served: priority reading/acquisition request, not a source of imported
  parameter values in this repository.

Do not upgrade this record to `full_text_reviewed` because a secondary review cites
the paper. An authorized copy must actually be inspected and its definitions
checked. The accessible 2011 conference paper below is not the 2015 journal paper.

### R-SETAR-MANSOUR-2011

H. Mansour, *Numerical and Experimental Modal Analysis of the Setar*, Canadian
Acoustics 39, 136–137 (2011), as indexed by the author's laboratory.

- Author index: <https://caml.music.mcgill.ca/doku.php?id=publications>.
- PDF: <https://caml.music.mcgill.ca/lib/exe/fetch.php?media=publications:caa2011_mansour.pdf>.
- Inspected locations: sections 3–5.
- Status: primary two-page paper accessible; explicit redistribution license not
  verified.
- Bounded evidence: the experimental and numerical study specifies a constrained
  support condition.
- Decision served: match boundary conditions and distinguish structural agreement
  from a validated playing-condition acoustic model.

### R-SETAR-MANSOUR-SCAVONE-2012

H. Mansour and G. P. Scavone, *A comparison of vibration analysis techniques applied
to the Persian setar*, Proceedings of Acoustics 2012 Nantes, 1737–1742.

- PDF: <https://caml.music.mcgill.ca/lib/exe/fetch.php?media=publications:nantes2012_mansour.pdf>.
- Inspected locations: sections 3.1–3.4 and 4.1–4.2.
- Status: primary six-page paper accessible through the author laboratory; no
  explicit redistribution grant verified.
- Bounded evidence: the tested support, sensing, and excitation arrangements give
  different measured responses, including attachment-loading effects.
- Decision served: require project-specific loading and fixture controls before
  accepting a clean transfer estimate as an unperturbed instrument response.

This is not an unconditional ban on accelerometers or shakers. Their acceptability
depends on the target, attachment, frequency range, and required uncertainty. The
project must perform its own loading controls rather than import a verdict about a
different apparatus.

### R-SETAR-TABASSIAN

Kiya Tabassian, *The SETAR*, Atlas Ensemble performer/composer handout.

- Primary PDF: <https://www.atlasensemble.nl/assets/files/instruments/Setar/Setar%20by%20Kiya%20Tabassian.pdf>.
- Inspected locations: pages 1–2 of the three-page PDF.
- Status: accessible performer-authored source; publication date not established
  from the inspected document; no explicit reuse license verified.
- Bounded evidence: the handout describes variable tuning, movable frets,
  fingernail excitation, and resonance involving unplayed strings.
- Decision served: avoid equating one equal-tempered keyboard mapping with the
  instrument's complete performance specification.

The handout represents an identified performer's explanation. It does not establish
that every school, maker, and player uses identical terminology or technique.
Historical claims in a short handout need separate historical research if product
copy intends to repeat them; this engineering project need not resolve origin
stories to measure a contemporary instrument carefully.

### R-FRETTING-POORHAYDARI-2023

Kioumars Poorhaydari, *Empirical Evaluation of Intervals and Fretting Systems in
Persian Art Music*, Analytical Approaches to World Musics 11(2), 2023.

- HTML: <https://journal.iftawm.org/previous/2023-volume-11-no-2/poorhaydari/>.
- PDF: <https://journal.iftawm.org/wp-content/uploads/2023/12/Poorhaydari_AAWM_Vol_11_2.pdf>.
- Inspected locations: introduction, methodology, initial tuner tests, and abstract;
  use the article's paragraph numbering for precise references.
- Status: primary full text accessible; reusable dataset rights not established.
- Bounded evidence: the study uses an online survey with 24 participants and
  pictorial fret analysis, and proposes theoretical standardizations.
- Decision served: distinguish observed intervals from a proposed scale and require
  contextual provenance for the project's tuning presets.

Do not turn a theoretical proposal into a universal Iranian tuning law. Do not
use interval variability as an excuse for inaccurate measurement. Structured
performance variation and measurement uncertainty are distinct quantities.

### R-METROLOGY-JCGM-100

Joint Committee for Guides in Metrology, *Evaluation of measurement data—Guide to
the expression of uncertainty in measurement*, JCGM 100:2008.

- Official DOI: <https://doi.org/10.59161/JCGM100-2008E>.
- Official publications index: <https://www.bipm.org/en/committees/jc/jcgm/publications>.
- Status: official record verified; the current index also lists subsequent
  material and amendments. Record the exact edition used by each procedure.
- Decision served: uncertainty reports need a measurement model, input
  quantities, correlations, and a defined meaning for reported intervals.
- Limitation: using these principles does not claim accredited calibration.

### R-ETHICS-OHA

Oral History Association, *OHA Statement on Ethics* and *Principles and Best
Practices*, official online guidance.

- Ethics page: <https://oralhistory.org/oha-statement-on-ethics/>.
- Suite: <https://oralhistory.org/principles-and-best-practices-revised-2018/>.
- Inspected location: preparation and communication section, accessed 2026-10-28.
- Status: official guidance accessible; distinguish adoption dates from later
  compiled PDF publication dates.
- Bounded evidence: the guidance emphasizes informed participation, clear project
  purposes and risks, review, and promises the project can actually keep.
- Decision served: a respectful interview and participant-review workflow, not
  legal advice or automatic institutional ethics approval.

### R-RIGHTS-WIPO-TCE

World Intellectual Property Organization, *Traditional Cultural Expressions*,
official explanatory page.

- URL: <https://www.wipo.int/en/web/traditional-knowledge/traditional-cultural-expressions/index>.
- Status: official page inspected 2026-10-28.
- Bounded evidence: traditional cultural expression and contemporary performance
  raise distinct rights questions; customary status does not justify assuming
  every recording or adaptation is unrestricted.
- Decision served: require project-specific rights review rather than treating
  traditional repertoire as blanket permission. Applicable law and agreements
  require qualified review in the relevant jurisdictions.

## Two independent status systems

Source status describes access and provenance: `discovered`, `identity_verified`,
`abstract_reviewed`, `full_text_reviewed`, `asset_verified`, `rights_verified`, or
`superseded`. Claim status describes reasoning: `candidate`, `supported_with_scope`,
`contradicted`, `unresolved`, or `withdrawn`. These are not a single progress ladder.
A fully reviewed paper may have unresolved applicability to a player-held specimen.

A useful claim is precise enough to be wrong. “Wood matters” is not sufficient.
“Changing the measured thickness field within its uncertainty changes the fitted
bridge response beyond the declared tolerance” can be investigated once its
specimen, support, and tolerance are defined. Store the proposition, source
location, project interpretation, counterevidence, and an observation that would
change the decision.

```yaml
claim_id: CLAIM-ATTACHMENT-001
statement: Attachment loading may bias body identification.
evidence_kind: proposed_project_test
source_ids: [R-SETAR-MANSOUR-SCAVONE-2012]
source_scope: Published apparatus comparison, not our apparatus.
specimen_id: null
configuration_id: null
protocol_id: PROTOCOL-SENSOR-LOADING
project_result: null
status: unresolved
promotion_blocker: No project sensor-on versus sensor-off comparison.
```

Stable identifiers survive URL changes. A replacement URL needs an identity check;
a similar title does not prove an identical edition. Preserve corrections as
versioned records with reasons, rather than invisibly rewriting the evidence trail.

## Extraction procedure

For a published quantity, record its printed name, units, axes, support state,
specimen description, and whether it was measured, assumed, fitted, or borrowed.
Read surrounding prose and footnotes. Inspect the original PDF if HTML could have
lost a minus sign or exponent. A figure-digitized value needs the axis calibration,
digitization method, and estimated uncertainty. Report no more precision than the
figure supports. Check logarithmic versus linear axes explicitly.

Copy only what is needed and permitted. Prefer a small original consistency audit
to reproducing a table or paraphrasing a whole paper. Missing shear moduli,
unreported fit bounds, or undocumented damping conventions remain missing; a
familiar software screenshot does not supply those inputs. For decisive values,
have a second contributor extract them independently before comparing assumptions.
Resolve disagreements by returning to definitions, not averaging incompatible
transcriptions.

## Missing assets and permissions

This inspection does not verify public complete CAD, raw FRF matrices, calibrated
acoustic data, or a redistributable performance corpus corresponding to the cited
studies. Record `not_verified_public`, not `does_not_exist`. Public downloadability
does not establish permission to redistribute, use commercially, or release a
derived parameter set. An article license is not automatically a separate asset
license.

Prepare requests identifying the desired files, experimental state, metadata,
intended uses, and licensing needs. Sending requests is a separate authorized
action; no author or musician should be contacted automatically because this
chapter exists. If assets arrive, preserve the grant and verify that the specimen,
configuration, and permitted derivatives match the planned use.

## Review and acceptance

The ledger passes when a reviewer can reconstruct why each decisive source applies,
locate it, separate author statements from project inferences, and identify missing
inputs. It fails when citations decorate unrelated claims, secondary summaries
masquerade as experiments, access failures disappear, or unknown rights become
assumed permission. Deliver a source-to-claim graph and the smallest useful next
experiment for each unresolved decision. Literature justifies an investigation;
it does not validate the finished synthesizer.
