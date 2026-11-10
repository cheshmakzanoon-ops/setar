# Research source atlas: from a question to an instrument decision

**Inspection date:** 2026-11-10. **Scope:** a reading and research execution guide,
not new measurements, an approved material library, or passed phase evidence.
The existing [evidence ledger](evidence-ledger.md) remains the record for its
original `R-` identifiers. The additional identifiers below provide a supplementary
source register; preserve them when recording a finding. Record the edition,
actual sections inspected, and access date again for a later experiment.

The central question is not “what can we cite?” It is “which observation or
argument could change this physical model, and what would falsify that change?”
A useful search ends with a scoped decision or a precise acquisition request.
It does not end with a bibliography or an undocumented value in a C++ preset.

## 1. Choose the research route before searching

| Unknown | First route | What to retrieve | Where the result enters the instrument |
| --- | --- | --- | --- |
| Which setar and which playing practice are represented? | Identified player/maker, authorized specimen, performer-authored material | String identity, course grouping, tuning context, gesture demonstration, permission and setup records | Specimen charter, physical string configuration, gesture vocabulary; P00/P01/P11 |
| Are string partials inharmonic and how do they decay? | Isolated string measurements plus mechanics/numerics sources | Length, linear mass, tension or independent bounds, partial frequencies and amplitude-decay traces | String dispersion and damping parameters; P03/P04/P12 |
| What excites a note? | Synchronized nail motion, string motion and available force measurements | Approach/release trajectories, contact region, direction, ringing initial state | Contact law and performance event contract; P05/P11 |
| How do frets and fingers change a ringing string? | Measured setup plus controlled press, slide and release | Fret coordinates, action, finger position, contact timing, pitch and decay trajectories | Moving constraints, contact damping and state transitions; P06 |
| Which structural details matter? | Specimen geometry, setar modal studies, calibrated bridge/soundboard response | Thickness/grain uncertainty, support state, complex FRFs, residues and damping convention | Body reference, passive reduction and bridge port data; P07–P09 |
| What reaches a microphone? | Calibrated acoustic capture and a separately verified radiation model | Surface motion, force reference, pressure, microphone coordinates, environment | Radiation loading versus observation transfer; P10 |
| Can the proposed parameters actually be identified? | Sensitivity study and independent observables before a large fit | Parameter profiles, correlations, bounds, held-out predictions | A reduced identifiable parameter set or a better experiment; P12 |
| Can the approximation run safely in a host? | Exact-version framework/SDK docs plus executable tests | Lifecycle/event contracts, worst-case timing, state compatibility and validator logs | Real-time approximation and adapter contracts; P13–P16 |
| Is the result convincingly setar-like for the stated use? | Preregistered acoustic, listening and playable evaluation | Blinded ratings, task completion, uncertainty, failure cases and scope | Qualified claims, limits and release notes; P17–P19 |

For every route, first write a question with a controllable input and an observable
output. For example: “With one string displaced and the others initially at rest,
does the measured bridge cross-mobility predict the second string's early response
without an extra sympathetic-resonance gain?” This is more useful than “research
resonance,” because it identifies a measurement, a mechanical connection and a
prediction that can fail.

## 2. Search method and stopping rule

### 2.1 Search in three passes

1. **Instrument pass.** Search the exact setar term, Persian spelling and author
   names below. Retrieve the original study or practitioner account. Record the
   actual specimen and whether the study concerns mechanics, musical practice,
   synthesis, or only history. Similar spellings do not establish identity.
2. **Mechanism pass.** Search the physical problem independently of instrument
   name: stiff strings, unilateral contact, modal mobility, acoustic radiation,
   inverse identification. A method from another instrument may supply an oracle
   or algorithm, while its geometry, coefficients and validation remain inapplicable.
3. **Implementation pass.** Once a method is chosen, inspect its exact numerical
   assumptions and the project's actual source. Search official software documentation
   for required interface behavior. Do not select physics because a convenient
   library happens to expose a matching class name.

Use an author/laboratory repository, publisher, conference archive or standards
body as the citation destination. A search index is a discovery tool. A repository
copy needs a title/author/version match; a similarly named conference paper does
not silently replace a later journal paper. Read references backward for foundations
and citations forward for corrections, critiques and later apparatus comparisons.
Record negative searches with the query, venue, date and outcome.

### 2.2 Queries that answer concrete questions

These are suggested searches, not claims that every query produces usable evidence.
Persian text may require both joined and separated spellings. Preserve the wording
used by a source instead of imposing a single transliteration.

| Purpose | Search strings | Screening question |
| --- | --- | --- |
| Setar structural studies | `"setar" "modal analysis"`; `"Persian setar" vibration`; `"سه تار" "تحلیل مودال"`; `"سه‌تار" ارتعاش` | Is it an Iranian setar, what support/string state was used, and are original methods available? |
| Geometry and construction | `"setar" soundboard thickness`; `"سه تار" ضخامت صفحه`; `"سه‌تار" ساخت خرک` | Are dimensions measured on an identified object, described by a maker, or merely repeated in sales copy? |
| Playing action | `"setar" fingernail plucking`; `"سه‌تار" مضراب ناخن`; `"سه تار" راست چپ` | Is the account from a named practitioner, and can the described action be demonstrated and synchronized with sound? |
| Frets and tuning | `"setar" fretting intervals`; `"sitār" "Persian" fretting`; `"سه‌تار" پرده بندی کوک` | Does it report observations, propose a theoretical tuning, or describe a specific performance context? |
| Loss/dispersion | `stiff string partial decay identification physical modelling`; `string bending stiffness frequency dependent damping measurement` | Are amplitude and energy decay distinguished, and can boundary/body loss be separated? |
| Contacts | `finite difference string barrier collision energy penalty`; `physical modelling nail string pluck release` | Is the law physically calibrated, numerically convenient, or both? Is the excitation prescribed or dynamically coupled? |
| Bridge/body | `setar bridge mobility laser Doppler`; `multiport mobility modal residues passive reduction` | Are force/velocity directions, complex phase and normalization supplied? |
| Radiation | `musical instrument acoustic radiation impedance cavity soundhole BEM`; `structure acoustic power coupling soundboard` | Does the model include acoustic feedback, or only map motion to an output microphone? |
| Identification | `profile likelihood practical identifiability experimental design dynamical models` | Can the proposed observations distinguish parameters, rather than only reduce an aggregate residual? |
| Host/release | `site:docs.juce.com AudioProcessor processBlock`; `site:steinbergmedia.github.io/vst3_dev_portal validator` | Does the page match the pinned dependency version and the behavior actually used by this repository? |

Do not globally exclude `sitar`: some Persian scholarship uses `sitār` for the
Iranian instrument. Instead inspect geography, construction, player, diagrams and
terminology. Conversely, a paper about Indian sitar bridge buzzing is not evidence
that the same bridge law belongs in this setar. Preserve the distinction in the
source record and any model hypothesis.

### 2.3 Stop when a decision is possible

For a bounded task, the search is sufficient when it has an inspected primary
method, an applicability check, a counterexample or rival mechanism, and a precise
remaining-data list. If no suitable primary source is found, record the gap and
propose a controlled experiment. Do not search indefinitely for a number that will
make an underdetermined fit appear physical. Do not replace unavailable full text
with a confident summary of a search snippet.

A request for missing data should name the specimen, configuration, raw and
processed formats, calibration, coordinate frames, preprocessing, material
conventions, and allowed uses required. Drafting such a request is research work;
sending it to a person requires separate authorization. No author, institution or
performer has been contacted by this atlas.

## 3. Setar-specific source cards

“Selected sections reviewed” means those sections were inspectable during this
update; it does not claim a new exhaustive peer review. Older inspection details
remain in the evidence ledger. The extraction and model actions in each card are
**project instructions**, not additional conclusions attributed to the authors.

### R-SETAR-PEDRAMMEHR-2018 — construction and structural modal analysis

Siamak Pedrammehr and coauthors, *A study on vibration of Setar: stringed Persian
musical instrument*, Journal of Vibroengineering 20(7), 2680–2689 (2018).
[Publisher full text](https://www.extrica.com/article/19505),
[DOI](https://doi.org/10.21595/jve.2018.19505).

**Access:** publisher HTML available; selected construction, modelling and
material sections reviewed. Read §2, §3.1, §3.2/Table 1, then the modal experiments
and comparisons in §4–§5. The article connects scanned geometry, structural
simulation and experimental vibration. Its printed material constants require
the repository's [existing admissibility audit](published-parameter-audit.md).

**Extract:** specimen/support/string state, component topology, local wood axes,
measurement origin of each quantity, and what geometry/raw data are absent.
**Translate:** motivate P07 geometry/material records and P08 reference experiments;
retain suspicious constants as negative import fixtures. **Reject:** importing a
published frequency list as a complete body preset, or changing a printed value
without an explicit correction record. A body preset also needs mode normalization,
port participation, damping and a declared operating state.

### R-SETAR-MANSOUR-2011 — a compact apparatus-to-FEM example

Hossein Mansour, *Numerical and Experimental Modal Analysis of the Setar*,
Canadian Acoustics 39, 136–137 (2011).
[Author-hosted paper](https://caml.music.mcgill.ca/lib/exe/fetch.php?media=publications:caa2011_mansour.pdf).

**Access:** two-page paper available; apparatus and method (§3) inspected. The
paper specifies neck restraint and damping the strings while retaining preload,
with bridge excitation and distributed soundboard response measurements.

**Extract:** excitation direction/location, support, string state, sensor,
measurement coordinates and modal extraction conventions. **Translate:** build
an experiment configuration that distinguishes geometry, preload and vibration
suppression. Replicate a defined boundary condition before claiming numerical
agreement. **Limit:** a clamped structural experiment does not by itself validate
player-held decay, radiation, nail contact, or the complete synthesized instrument.
Use it to design P07/P08 evidence, not as a universal setar response.

### R-SETAR-MANSOUR-SCAVONE-2012 — measurement loading and supports

Hossein Mansour and Gary P. Scavone, *A comparison of vibration analysis techniques
applied to the Persian setar*, Acoustics 2012 Nantes, 1737–1742.
[Author-hosted paper](https://caml.music.mcgill.ca/lib/exe/fetch.php?media=publications:nantes2012_mansour.pdf).

**Access:** six-page paper available; §3.1–§3.4 and §4.1–§4.2 inspected. It compares
support, excitation and sensing choices and reports apparatus-dependent loading.

**Extract:** the paired conditions and measured quantity in every comparison;
separate its setar and violin observations. **Translate:** add a project sensor
loading pilot and support-repeatability comparison before accepting FRFs. Keep
fixture/sensor effects out of material damping fits. **Limit:** no universal
accelerometer prohibition follows. The project's allowable sensor mass, attachment
and support must be established for its own specimen and frequency band.
A cleaner trace can still be a more perturbed physical system.

### R-SETAR-MANSOUR-2015 — priority acquisition lead

Hossein Mansour, *Modal Analysis of the Setar: A Numerical–Experimental Comparison*,
Journal of Vibration and Acoustics 137(6), 061006 (2015).
[DOI](https://doi.org/10.1115/1.4030863).

**Access:** bibliographic identity retained from the existing ledger; direct DOI
retrieval again returned an access error in this update. Full text and underlying
assets were not inspected here. **Next action:** obtain an authorized publisher or
author copy, verify the title/edition, then extract support, geometry, joints,
preload and comparison definitions. Treat it as a lead, not a source of new
constants. Keep the 2011 conference paper's identity separate; access to that paper
does not establish the contents of the 2015 article.

### R-SETAR-TABASSIAN — performer-authored instrument and tuning description

Kiya Tabassian, *The SETAR*, Atlas Ensemble handout; publication date not established.
[Primary PDF](https://www.atlasensemble.nl/assets/files/instruments/Setar/Setar%20by%20Kiya%20Tabassian.pdf).

**Access:** PDF available; pages 1–2 inspected. It describes fingernail actions,
variable tuning, movable frets, course organization and sympathetic resonance.
**Extract:** original names, diagrams, relative versus absolute pitch, and the
specific action each description denotes. **Translate:** create elicitation tasks
for a practitioner: demonstrate both stroke directions, selective damping and
retuning in context; connect the resulting trajectories to P05/P06/P11.
**Limit:** a single performer's handout does not certify every school or a fixed
concert-pitch preset. It also supplies no nail stiffness, contact friction,
calibrated sound recording, or reusable gesture dataset.

### R-FRETTING-POORHAYDARI-2023 — observations versus tuning proposals

Kioumars Poorhaydari, *Empirical Evaluation of Intervals and Fretting Systems in
Persian Art Music*, Analytical Approaches to World Musics 11(2), 2023.
[Journal full text](https://journal.iftawm.org/previous/2023-volume-11-no-2/poorhaydari/).

**Access:** full HTML available; abstract and “Methodology and Initial Tests,”
including numbered paragraphs [7]–[8], inspected. The study combines a performer
survey and pictorial fret analysis with proposed theoretical scale models.
**Extract:** which entries are participant observations, geometric inference,
historical citations or the author's proposals; retain the population/context.
**Translate:** design a versioned fret-position and intended-interval record,
then compare measured played pitches under controlled stopping conditions.
**Limit:** use primary historical texts for historical assertions and project
measurements for a specimen preset. A proposed parent scale is not a universal
mandatory tuning or evidence that finger action has no pitch effect.

## 4. Numerical and acoustic methods: transfer the method, audit the assumptions

### R-NUM-BILBAO-SMITH-2005 — nonlinear string energy and discretization

Stefan Bilbao and Julius O. Smith III, *Energy-conserving finite difference schemes
for nonlinear strings*, Acta Acustica united with Acustica 91(2), 299–311 (2005).
[University record](https://www.research.ed.ac.uk/en/publications/energy-conserving-finite-difference-schemes-for-nonlinear-strings/),
[author manuscript](https://www.pure.ed.ac.uk/ws/portalfiles/portal/11222313/acustica_final.pdf).

**Access:** manuscript available; §II and the §III/appendix reading map inspected.
It treats linear and Kirchhoff–Carrier equations, discrete energetic behavior,
stability restrictions and boundary/loss issues. **Read:** §II for model assumptions,
§III for the actual discrete scheme, especially boundary handling; inspect the
appendix before transferring a loss formulation. **Translate:** derive a unit-checked
continuous energy and the distinct discrete invariant for the chosen P03/P04
scheme. Add convergence and nonlinear amplitude cases. **Limit:** a proof for
one discretization does not certify the current modal-contact solver or a hybrid
scheme with added delay, damping or moving constraints.

### R-CONTACT-BILBAO-2014 — collision potentials and energy accounting

Stefan Bilbao, Alberto Torin and Vasileios Chatziioannou,
*Numerical Modeling of Collisions in Musical Instruments*, 2014 preprint.
[Versioned record](https://arxiv.org/abs/1405.2589v1),
[PDF](https://arxiv.org/pdf/1405.2589).

**Access:** author preprint and abstract available; this card identifies its
collision/potential framework without claiming every derivation was checked.
**Extract during the task:** gap sign, potential, force evaluation, discrete work,
nonlinear solve and the assumptions behind any stability statement.
**Translate:** a no-contact/contact/separation oracle and penetration/residual
study for P05/P06. **Limit:** its examples span other instruments. The Indian
sitar example does not establish an Iranian setar bridge or fret law; numerical
penalty stiffness is not automatically an identified physical contact stiffness.
A later journal version, if used, requires its own edition check.

### R-CONTACT-BILBAO-TORIN-2014 — distributed fretboard contact

Stefan Bilbao and Alberto Torin, *Numerical Simulation of String/Barrier
Collisions: The Fretboard*, DAFx-14 (2014).
[Conference paper](https://dafx.de/paper-archive/2014/dafx14_stefan_bilbao_numerical_simulation_of_s.pdf).

**Access:** eight-page paper available; introduction and §2 model entry inspected.
The paper describes a single-polarization string/barrier problem, tension
modulation, finger stopping and a finite-difference energy approach.
**Read:** §2 for the physical model and energy, §3 for discretization/solve,
§4 for numerical examples. **Translate:** design controlled obstacle fixtures,
separate physical contact width from numerical interpolation width, and specify
what P06 requires beyond the source model. **Limit:** tied movable setar frets,
two polarizations and measured finger behavior need explicit adaptation and
independent validation; the published guitar example is not that validation.

### R-ACO-DELEON-2008 — a boundary-element method and its useful limits

Simon de Leon, *The Boundary Element Method for Linear Acoustic Systems*,
McGill University MA thesis (2008).
[Author-laboratory PDF](https://caml.music.mcgill.ca/lib/exe/fetch.php?media=publications:ma_deleon_2008.pdf).

**Access:** thesis available; abstract, contents and §1.4 inspected.
**Read next:** §2.1–§2.2 for formulation, §3.1.2 for normals, §3.2 for integration,
and §4 for reference problems. Its stated treatment of perforated boundaries is
limited; this matters directly to setar soundholes.
**Translate:** use explicit normals, pressure/velocity conventions and an analytic
acoustic reference before trying real geometry. **Limit:** do not copy its opening
approximation into P10 as validated radiation impedance. Select and verify a
formulation that represents the project's actual coupled cavity/exterior boundary.
A thesis's software description is not proof that its old download still builds
or meets current licensing and runtime requirements.

### R-NUM-BILBAO-2009 — deeper finite-difference foundation

Stefan Bilbao, *Numerical Sound Synthesis: Finite Difference Schemes and Simulation
in Musical Acoustics* (Wiley, 2009).
[Publisher record and DOI](https://onlinelibrary.wiley.com/doi/book/10.1002/9780470749012).

**Access:** publisher identity and scope verified; the full book was not read in
this update. Use an authorized copy to study finite-difference operators,
energy/stability analysis and the component relevant to the task. Record exact
chapter/section from that copy before citing a derivation. **Translate:** strengthen
P02/P03 reference work and P04–P10 numerical assumptions. **Limit:** the book is
a methods source, not a measured setar parameter collection. Do not invent page
numbers or imply every proposed algorithm has been independently reproduced.

## 5. Measurement, identification and evaluation sources

### R-METROLOGY-JCGM-100 — uncertainty as part of the quantity

JCGM 100:2008, *Evaluation of measurement data — Guide to the expression of
uncertainty in measurement*.
[Official record](https://www.bipm.org/en/doi/10.59161/jcgm100-2008e),
[official PDF](https://www.bipm.org/documents/20126/2071204/JCGM_100_2008_E.pdf).

**Access:** PDF available; contents and §4.1 inspected. Read §4 for input
uncertainties, §5 for combination including correlation, and §7 for reporting.
**Translate:** define the measurement equation before calculating an uncertainty
bar; carry calibration, repeatability and shared errors through derived tension,
FRFs and fitted parameters. Record edition and coverage convention. **Limit:**
following a guidance document does not make the project an accredited laboratory,
remove unmodelled bias or justify treating correlated records as independent.

### R-IDN-RAUE-2009 — whether a fit determines its parameters

Andreas Raue and coauthors, *Structural and practical identifiability analysis of
partially observed dynamical models by exploiting the profile likelihood*,
Bioinformatics 25(15), 1923–1929 (2009).
[DOI](https://doi.org/10.1093/bioinformatics/btp358),
[publisher page](https://academic.oup.com/bioinformatics/article/25/15/1923/213246).

**Access:** publisher-indexed abstract and §2.3 excerpts reviewed; direct article
retrieval encountered an access error. Treat uninspected methods as acquisition
work. The article concerns biological dynamical models; its identifiability
reasoning motivates a method transfer, not acoustic parameter values.
**Translate:** before P12 fitting, freeze observables and residual assumptions,
then profile suspect parameters while re-optimizing the others. Record flat or
bound-limited directions and design an added measurement. **Limit:** a bounded
optimizer result is not proof of identifiability, and the likelihood/noise model
must be justified for the project's acoustic data.

### R-EVAL-ITU-BS1116-3 and R-EVAL-ITU-BS1534-3 — choose the listening question

Official recommendations:
[ITU-R BS.1116-3](https://www.itu.int/rec/R-REC-BS.1116-3-201502-I/en),
*Methods for the subjective assessment of small impairments in audio systems*;
[ITU-R BS.1534-3](https://www.itu.int/rec/R-REC-BS.1534-3-201510-I/en),
*Method for the subjective assessment of intermediate quality level of audio systems*.

**Access:** official edition records verified; BS.1116 PDF retrieved; BS.1534 PDF
retrieval timed out, so no new full-method review is claimed. **Translate:** select
a protocol suited to the expected error and evaluation question, then read and pin
the exact edition before preregistration. P17 also needs instrument-specific
playability tasks, not merely an audio-quality rating. **Limit:** an adapted test
must state its deviations. A pleasant result, failure to reject a difference, or
an underpowered comparison does not establish indistinguishability or professional
instrument parity. These two recommendations are not interchangeable score scales.

## 6. Official implementation sources and version discipline

### R-SDK-JUCE-AUDIOPROCESSOR — application boundary behavior

[JUCE AudioProcessor reference](https://docs.juce.com/master/classjuce_1_1AudioProcessor.html).
**Access:** official class page inspected. Begin with `prepareToPlay`,
`processBlock`, bus layout, state methods and resource lifecycle for the actual
adapter work. The `master` documentation can describe behavior or layout newer
than this repository's pinned JUCE version; inspect the matching source checkout
before changing a contract.

**Translate:** enumerate host calls and event ordering as P14 tests, keep the
physics in the independent engine, and retain separate validator/host execution
records. **Limit:** a declaration in documentation, successful configuration or
syntax inspection does not establish a compiled, loadable, qualified plugin.

### R-SDK-VST3-VALIDATOR — plugin conformity evidence

[Steinberg validator documentation](https://steinbergmedia.github.io/vst3_dev_portal/pages/What%2Bis%2Bthe%2BVST%2B3%2BSDK/Validator.html),
[official SDK repository](https://github.com/steinbergmedia/vst3sdk).
**Access:** validator page inspected; it identifies a command-line conformity test
host with source. **Translate:** record the SDK revision, validator binary,
plugin hash, command, complete result and supported platform in P14/P16 evidence.
Check current official version/license files when the task executes. **Limit:**
a passing validator does not prove every DAW interaction, callback deadline or
setar fidelity. Do not silently broaden platform claims beyond executed jobs.

## 7. Turn a reading into an admissible finding

Use the existing [data/provenance contract](../measurement/data-contracts-and-provenance.md)
and [parameter identifiability chapter](parameter-identifiability.md). The following
is an editorial checklist for one finding, not a new executable schema.

| Finding field | What the researcher must write | Why the model developer needs it |
| --- | --- | --- |
| Question and task | Exact physical question, phase, `ENG-Pxx-Tyy`, relevant `REQ-AAA-NNN` | Prevents unrelated literature from driving an implementation |
| Source and inspected location | Stable `R-` ID, DOI/URL, edition, section/table/figure, access level | Lets a reviewer reproduce the extraction |
| Original statement | Small paraphrase, original symbols/units and measured/assumed/fitted status | Separates evidence from project interpretation |
| Applicability | Specimen, support, preload, amplitude/frequency, environment and boundary conditions | Defines where the result can constrain the model |
| Transformation | SI conversion, axis/sign mapping, uncertainty and every derivation | Prevents hidden changes between a paper and a parameter |
| Competing explanations | Alternative mechanism, missing observable and discriminating experiment | Prevents tuning one subsystem to conceal another's error |
| Model consequence | Configuration field, equation/port, numerical approximation or test affected | Gives a concrete implementation handoff |
| Acceptance | Independent oracle, measured comparison, tolerance rationale and failure action | Makes the claim falsifiable |
| Disposition | Candidate, supported within scope, unresolved, contradicted or withdrawn | Avoids promoting a plausible hypothesis into a preset |

A publication's DOI belongs in a source record, not in place of a specimen ID.
A model consequence can be “retain the simpler model and collect another
measurement.” Recording that decision may be more valuable than adding an
unidentifiable coefficient.

### Worked research decision: an extra peak in bridge response

This is a **proposed diagnostic scenario**, not a measured project result.
Suppose a future FRF contains a peak absent from the current body reduction.
First identify whether the response is mobility, accelerance, displacement/force
or pressure/force, and reproduce the units and support. Check repeated acquisitions,
force quality, sensor attachment and phase. Read the 2012 setar apparatus study to
plan a paired loading check; its result must not substitute for this check.

If the peak follows the sensor or support, correct the experiment/model boundary
and document the rejected calibration candidate. If it is repeatable and physical,
compare spatial response at several ports, then estimate whether a missing body
mode can explain both diagonal and cross responses. Fit the mode with consistent
residue signs, normalization and damping, test passivity, and reserve a port or
support condition for prediction. Do not add an arbitrary post-EQ peak: it would
change the sound without establishing the mechanical energy exchange.

The handoff goes to `ENG-P08-T02` and `ENG-P08-T03`, with `REQ-BOD-002`,
`REQ-BOD-003` and the P09 coupling implications (`REQ-CPL-001`). It contains the
raw/configuration references, analysis method, candidate mode record, uncertainty,
held-out result and rejection criteria. Actual file locations and executable
checks must come from the current
[engineering work orders](../../roadmap/engineering-tasks.json) and
[coverage registry](../../roadmap/executable-test-coverage.json); proposed evidence
is not a present CTest.

## 8. Research completion and handoff

A completed research task delivers the question, inspected sources, a finding
record, a decision with uncertainty, and an explicit next owner. It does not need
to deliver a positive answer. Before closing, a second reader should be able to:

1. Locate the decisive source and see exactly what was inspected.
2. Reproduce conversions and distinguish measured, published, fitted and synthetic
   quantities without asking the original author of the task.
3. Explain the physical state/equation/port affected and the mechanism expected to
   change in the sound or playing response.
4. Identify the experiment that could reject the interpretation.
5. Find the destination phase/work order and its still-open gate.

For current project work, use this atlas with the
[phase task crosswalk](../../roadmap/phase-task-crosswalk.json),
[measurement campaign instructions](../measurement/13-acquisition-campaign-execution.md)
and [scientific decision gates](../governance/engineering-decision-gates.md).
No source inspection in this document changes the synthetic status of the
prototype or closes a specimen, contact, body, radiation or fidelity gate.
