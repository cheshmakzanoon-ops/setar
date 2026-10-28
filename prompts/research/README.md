# Research and measurement developer promptbook

These prompts are bounded work requests, ordered by evidence dependency rather
than dates. Use them with the repository's phase gates and contributor rules.
They do not authorize contacting participants, changing an instrument, obtaining
paid services, publishing restricted data, or broadening permissions. The assigned
developer must inspect the actual repository and report what exists before
claiming implementation or measurement success.

Every prompt requires a final handoff containing changed files, commands actually
run, observed results, unresolved inputs, and the next evidence gate. A proposed
test is not a passed test. A synthetic fixture is not a setar measurement. Do not
fill missing scientific inputs with plausible values to make a pipeline run.

## R-PROMPT-01: Verify one source-to-claim chain

> Read `docs/research/evidence-ledger.md` and select the assigned claim, not a
> broad topic. Locate the original source through its DOI or primary host and
> record the exact edition and section. Separate what the author directly
> reports from our inference and from what remains unavailable. If full text is
> inaccessible, keep that status and do not substitute a secondary abstract.
>
> Produce a claim record with proposition, source location, specimen scope,
> boundary conditions, quantity definitions, counterevidence, and the model
> decision it affects. Extract only the minimum necessary numerical material.
> Inspect the original PDF or figure when signs, units, or axis labels could
> have been lost in text extraction. Have a second independent extraction for
> any value that will enter a production parameter set.
>
> Check whether associated CAD, raw responses, code, and rights are actually
> available. Use `not_verified_public` when availability has not been established.
> Do not say those assets do not exist. Draft a precise acquisition request if
> needed, but do not send it without separate authorization.
>
> Acceptance: another reviewer can locate the evidence and see exactly what it
> supports. Failure: an inferred value is labeled measured, an access failure is
> hidden, or public access is treated as a reuse license. Next: propose the
> smallest experiment that would resolve the most consequential remaining claim.

## R-PROMPT-02: Implement constitutive input admission

> Read `docs/research/published-parameter-audit.md` and the core's units contract.
> Implement an offline validator for the proposed orthotropic material schema.
> Keep physical values in SI units at the C++ model boundary. Document the
> Poisson-index convention, stress/strain ordering, engineering-shear convention,
> and local-to-global material frame. Do not insert guessed corrections for the
> quarantined published table.
>
> Validate finite values, positive principal and shear moduli, reciprocal
> compliance, symmetry, positive definiteness, conditioning, and proper rotation
> frames. Return structured diagnostics that identify fields and conventions.
> Validation occurs before real-time processing; do not add allocation or error
> formatting to the audio callback.
>
> Build independent fixtures: one admissible synthetic orthotropic material,
> the minimal published reciprocity contradiction, a symmetric indefinite block,
> a unit-scale error, swapped axes, an invalid rotation, and a near-singular
> candidate. Compare the matrix assembly against a separate small reference
> calculation. Test an isotropic rotation-invariance limit.
>
> Acceptance: admissible input is preserved without hidden changes and invalid
> input cannot enter the engine. Report tolerances and why they are appropriate.
> Do not silently symmetrize or clip eigenvalues. Next: connect approved material
> records to the geometry grain frame and run an uncertainty sensitivity study.

## R-PROMPT-03: Design the first specimen pilot

> Read `docs/measurement/reference-instrument.md`,
> `docs/measurement/geometry-and-thickness.md`, and the participant-rights chapter.
> Inspect the available specimen dossier. If authorization, condition review, or
> required handling information is missing, produce a blocker report and a
> proposed noninvasive pilot; do not invent a specimen or proceed with an
> unapproved intervention.
>
> Define a right-handed coordinate frame, stable landmarks, configuration
> identity, and a compact repeated measurement set. Select quantities whose
> uncertainty can change the first model decision. Include a reference-object
> calibration, a complete remounting repeat, and an explicit stop rule for
> instrument distress or state change. Distinguish loaded geometry from a
> stress-free model assumption.
>
> Produce an operator checklist, blank manifest, planned uncertainty budget,
> and processing path. Test the manifest and processing on clearly synthetic
> data before real acquisition. The plan must state what remains unobserved,
> especially thickness, contact locations, and grain directions.
>
> Acceptance: a qualified operator can execute the pilot safely and a different
> analyst can interpret its outputs. Failure: the plan depends on undocumented
> memory, assumes scanner accuracy from a product name, or treats an outer scan
> as complete internal geometry. Next: use pilot sensitivity and repeatability
> to decide whether dense acquisition is justified.

## R-PROMPT-04: Build a calibrated FRF analysis path

> Read `docs/measurement/frf-and-modal-protocol.md` and
> `docs/measurement/uncertainty-and-calibration.md`. Implement an offline pipeline
> that accepts force and response channels with explicit quantity, units,
> calibration, location, direction, and time-base metadata. Preserve raw assets.
> Define Fourier and cross-spectrum conventions before coding estimators.
>
> Produce mobility in m s^-1 N^-1. If the input response is acceleration, retain
> the measured accelerance and document conversion to mobility, including the
> low-frequency exclusion policy. Implement force-spectrum checks, clipping and
> double-impact flags, window metadata, H1 estimation, coherence diagnostics,
> and repeated-trial uncertainty. High coherence alone must not grant acceptance.
>
> Verify against synthetic oscillators with known poles and residues. Add known
> channel delay, gain error, noise, truncated decay, and inadequate force-band
> fixtures. Show which problems the diagnostics detect and which require external
> calibration. Test that zero padding does not alter claimed information content.
>
> Acceptance: units, phase, amplitude, and uncertainty agree with independent
> references within declared tolerances; invalid bands remain flagged. Do not
> claim instrument validation without real authorized data. Next: run a small
> calibrated pilot and a sensor-loading control before fitting a body model.

## R-PROMPT-05: Fit a physically admissible body reduction

> Read `docs/research/parameter-identifiability.md` and the body-model interface.
> Inspect the supplied FRF packet for calibration, port definitions, support,
> preload, usable band, and rights. Stop the calibrated-fit claim if these are
> missing. You may continue with a synthetic recovery study labeled as such.
>
> Define the candidate model, loss function, frequency weighting, residual terms,
> constraints, and normalization before optimization. Preserve collocated
> power-port structure and passivity where required. Use several initializations
> and model orders, record failed runs, and inspect residuals by port and band.
> Do not select modes only by the smallest training error.
>
> Analyze parameter sensitivity and confounding. Report which quantities are
> physically identified and which are effective fit parameters. Predict held-out
> ports or configurations without refitting. Test the exported model in the
> intended coupling interface, not only as an isolated frequency plot.
>
> Acceptance: the reduction is stable, admissible, traceable, and predictive in
> its declared band. Failure: hidden gain adjustments absorb errors, passivity
> relies on clipping, or test data guide model selection. Next: connect radiation
> through a separately validated output map and document unresolved mechanisms.

## R-PROMPT-06: Establish radiation-data validity

> Read `docs/measurement/radiation-and-acoustic-capture.md`. Inspect the pressure,
> force, microphone geometry, calibration, facility, and timing records. Define
> whether the task estimates absolute Pa/N transfer, normalized directivity, or
> a relative listening response. Do not claim all three from incomplete data.
>
> Implement the selected transform with explicit spectral normalization and
> propagation convention. Preserve uncorrected transfer functions alongside any
> distance or phase correction. Quantify microphone-position and channel-delay
> effects. If using an angular representation, choose its order from coverage
> and held-out directions rather than desired visual smoothness.
>
> Add synthetic tests for known delays, directional nulls, missing directions,
> and irregular angular quadrature. Verify that a close-microphone pressure
> measurement is not automatically converted into total acoustic power.
> Document the room's usable band and limitations of time gating.
>
> Acceptance: the chosen representation predicts withheld directions or
> conditions and retains honest calibration limits. Failure: a room reflection
> is fitted as instrument radiation or absolute claims use unknown gain. Next:
> produce rights-cleared listening stimuli through a logged, symmetric
> preparation process and retain the calibrated originals.

## R-PROMPT-07: Create a gesture annotation and control study

> Read `docs/measurement/pluck-contact-and-motion.md` and
> `docs/ethnomusicology/elicitation-and-terminology.md`. Identify one gesture
> contrast that matters to the intended instrument. Preserve performer terminology
> separately from physical descriptors. Confirm that the acquisition and any
> imagery are within the participant's agreed scope.
>
> Define observable contact events, timing uncertainty, coordinate frames, and
> synchronization. Use intervals for occluded or between-frame events. Build an
> annotation guide and independently annotate a subset before trusting automated
> labels. Compare instrumented and minimally instrumented behavior where feasible.
>
> Fit or propose a compact control representation using supported variables:
> contact location, direction, displacement, duration, and state dependence.
> Preserve correlations rather than adding independent random jitter. Compare
> a simple initial-condition model with finite-contact behavior only where the
> data can distinguish them.
>
> Acceptance: held-out gestures test the mapping, uncertainty is propagated, and
> apparatus disturbance is documented. Failure: sub-frame precision is asserted
> without evidence, inferred force is called measured force, or a natural
> performance claim relies on a substantially altered technique. Next: test the
> mapping with a player who did not supply calibration gestures.

## R-PROMPT-08: Validate tuning and fret representation

> Read `docs/ethnomusicology/performance-and-tuning.md` and
> `docs/measurement/strings-frets-and-setup.md`. Build a representation that keeps
> physical string identity, course grouping, absolute reference, intended interval,
> measured fret position, predicted pitch, and measured pitch distinct. Unknown
> values remain unknown; do not insert a universal quarter-tone scale.
>
> Implement round-trip tests for ratios, cents, and frequencies, preserving
> original labels and provenance. Test physical fret geometry separately from
> an electronic pitch mapping. Define what host tuning messages change and how
> the physical state transitions safely. A retune must not silently teleport
> energy or invalidate the body preload assumption.
>
> Prepare a player-review task with isolated notes and contextual phrases. Record
> target and realized pitch trajectories with the estimator and uncertainty.
> Preserve disagreements as contextual alternatives instead of averaging them
> into a false authority. Review preset names and contributor attribution.
>
> Acceptance: the representation can express the approved examples and clearly
> labels designed approximations. Failure: notation is confused with measured
> mechanics or a single player's setup is presented as universal. Next: evaluate
> sympathetic response and damping under held-out tuning configurations.

## R-PROMPT-09: Freeze and audit independent partitions

> Read `docs/measurement/independent-validation.md` and the data-contract chapter.
> Inventory all raw events and derivatives, then construct linked groups by
> specimen, participant, session, configuration, and gesture family. Choose the
> independent axis that matches the intended claim and state which axes remain
> shared because of limited data.
>
> Implement leakage checks for duplicate raw events, overlapping segments,
> processed copies, shared calibration decisions, and manually reviewed test
> examples. Produce a partition manifest and access history. Freeze the model,
> parameter package, metrics, normalization, exclusions, and acceptance rules
> before opening the independent test.
>
> Report group-aware uncertainty and condition-specific failures. Distinguish
> implementation verification, physical validation, listening plausibility,
> preference, and active playability. Do not combine a physical failure with a
> preference success into an unexplained overall pass.
>
> Acceptance: every claim identifies truly unseen information and its population
> scope. Failure: randomized note splitting leaks the same session into all sets
> or thresholds change after results are seen. Next: preserve failed test results,
> reclassify reused data as development evidence, and acquire new independent
> evidence for revised claims.

## R-PROMPT-10: Reproduce a complete research packet

> Work as a reviewer who did not create the packet. Read
> `docs/research/reproducibility-and-stop-gates.md`, then reproduce one decisive
> result from raw identifiers, manifests, calibration, and documented commands.
> Record ambiguities before receiving informal assistance. Do not copy private
> credentials or participant information into the report.
>
> Validate checksums, schema, units, timing, source status, and rights first.
> Recompute derived observables, fitting, uncertainty, and held-out predictions.
> Use the documented numerical tolerances and test a known invalid fixture to
> confirm that the pipeline can reject bad data. Distinguish bitwise variation
> from scientifically meaningful variation.
>
> Check that all claimed outputs correspond to the same calibration and model
> versions. Mark stale downstream assets when an input changes. Review release
> language against the actual evidence: a successful build or synthetic
> convergence test is not measured setar fidelity.
>
> Acceptance: the result is independently reproducible or its blocker is precisely
> located. The handoff lists supported claims, failed claims, unknowns, changed
> files, actual commands, and next actions. Do not manufacture a favorable
> result to complete the report. Next: repair the packet itself so the next
> reviewer needs fewer private explanations.

