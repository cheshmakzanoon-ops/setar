# Reference corpus, provenance, and leakage-resistant splits

Measurements describe an instrument and experiment, not a bag of audio to
optimize against. This chapter defines the corpus supporting acoustic and gesture
validation. It does not authorize collecting, redistributing, or repurposing
someone else's recordings. Use permitted recordings and measurements, documenting
rights separately from technical quality. Recorded notes are references, never
the synthesis engine.

## Sampling unit and identifiers

The natural unit is usually a take within a session on a specimen, performed by
a player under a documented setup. Windows from one take are not independent
performances. Multiple microphones on one pluck share excitation and environment.
A thousand extracted frames are not a thousand independent acoustic observations.

Assign opaque identifiers for specimen, session, player, setup, take, channel,
gesture, and derivative. Keep private identity mappings only where consent or
equipment management requires them. Public artifacts use relevant characteristics
without unnecessary personal details. Construction descriptions include
uncertainty; inferred material constants are not direct measurements.

Declare whether the target is an individual specimen or an instrument population.
With one specimen, make a specimen-level claim and leave population evaluation
unmet. Build, player, string-history, and tuning differences are part of the
domain, not nuisances to average away indiscriminately.

## Acquisition metadata

Each take records string/course identity, tuning before and after, fret/contact
location, sympathetic strings allowed to ring, excitation direction and position,
gesture instructions, damping conditions, and microphone geometry. Measured force
or displacement needs sensor calibration and mounting effects. If input force is
not measured, say so. A MIDI velocity assigned later is not a force measurement.

Record sample rate, encoding, channel order, preamplifier gain, microphone model,
polar pattern where applicable, distance, angle, room description, and reference
level calibration. Include relevant temperature and humidity without pretending
one reading describes wood moisture. Document whether the instrument was held,
supported, or clamped; supports can change mechanical and radiative response.

Retain failed-take notes. Clipping, handling noise, incorrect fingering, unstable
tuning, and sensor dropout need explicit reasons and an exclusion policy. Raw
takes remain immutable unless permissions require removal. A removed file's
manifest records that fact without retaining prohibited content.

## Transformation graph

Raw audio is captured data. Conditioned audio has declared transformations such
as channel selection, DC removal, synchronization, or calibrated conversion.
Features are extracted quantities. Give each layer distinct identifiers and
hashes. Do not overwrite raw audio with normalized audio while calling it raw.

Transformations record input hashes, algorithm revision, parameters, and output
hash. Filters need coefficients or exact design recipes, padding, phase behavior,
and edge treatment. Resampling needs ratio, implementation, and delay handling.
Alignment needs its permitted shift range and criterion. Level adjustment needs
a measurement window and gain. Such processing can erase the behavior under study.

Independent per-partial alignment can manufacture phase agreement the model never
produced. Per-note normalization can conceal defective dynamic response. Denoising
changes the noise floor and may bias decay estimates. These transforms can support
narrow experiments, but the claim must describe them. Preserve an unconditioned
comparison as a diagnostic.

## Grouped partitions

Separate development data for construction, calibration data for estimation,
validation data for declared model selection, and final held-out data for the
generalization claim. The split follows the question, not a ritual percentage.
Group correlated observations before splitting.

Hold out complete sessions for unseen-session claims, players for unseen-player
claims, and instruments for instrument generalization. Frame-level splits leak
nearly identical content. Take-level splits can still leak room response, tuning,
or player-specific attacks when those are the generalization axes being tested.

A reviewed split manifest states group keys, selection rule, random seed if used,
intended claim, and allowed access. Integrity metadata may be inspected without
sealed waveforms if that boundary is declared. If waveform inspection is required
to diagnose corruption, log it and consider whether fresh holdout data are needed.
Sealing is a workflow discipline, not a cryptographic claim. Repeated model
selection on final-test results spends the holdout even without an optimizer.

## Coverage matrix

Cover open and stopped strings, pair interactions, register, excitation position,
dynamics, attack direction, damping, and ornaments. Mark cells `measured`,
`synthetic only`, `missing`, or `excluded`. Many easy open notes do not fill an
unmeasured fret-contact or drone-interaction cell.

Controlled gestures identify mechanisms; musical passages test their composition.
Controlled plucks reveal decay and coupling but not the experience of repeated
ornaments. Free performance reveals musical mismatch but complicates causal
alignment. Both belong with different interpretations. Synthetic fixtures test
extractors and pipelines and remain labeled synthetic in manifests and graphs.

Include negative controls: damp a sympathetic string, change microphone angle,
perturb tuning slightly, or move the pluck. These help establish metric
sensitivity. Do not alter several factors and attribute the result to one.
Each coverage cell describes what was manipulated and what could not be fixed.

## Repeats and missing information

Collect repeats before deciding how small an error is meaningful. Estimate
within-session and, when possible, between-session variation. Show individual
points or intervals, not only an average curve. A narrow confidence interval for
the mean does not make an individual performance predictable to that precision.

Distinguish response repeatability from input variability. Measured force supports
transfer estimation only where linearity and measurement quality are adequate.
For nonlinear gestures, preserve input trajectories and compare conditional
responses. Label unobserved excitation uncertainty rather than allowing fitting
to attribute it entirely to string or body parameters.

Missing metadata are `unknown`, not plausible guesses. Explain consequences and
restrict analysis if needed. Unknown microphone distance may permit some decay
analysis but defeats absolute radiation-level comparison. Unknown string history
may require a wider uncertainty model instead of a precise material-loss claim.

## Integrity tests and migration

Ingestion checks hashes, decodability, duration, channels, sample rate, finite
samples, clipping flags, duplicates, references, and permission state. Detect
duplicate takes under different names and derivatives in inconsistent splits.
Near-duplicate detection supports review; it does not authorize automatic deletion.

Test extraction on synthetic tones with known drift, close modes with controlled
beats, exponential and multi-slope decays, calibrated noise, and clipping. An
extractor must report uncertainty or failure for unresolved partials. A confident
number for every input is a defect. Store negative-control outputs as evidence.

Migration tests cover renamed identifiers, changed channel conventions, and
revised segmentation. These can invalidate comparisons without changing audio
bytes. Version schemas and derivatives, retain prior manifests, and regenerate
declared descendants. Reports identify corpus revision as precisely as code.

## Acceptance and the next experiment

A reviewer checks permissions, provenance, grouping, coverage, exclusions, and
reproducible transforms. Acceptance is a manifest and audit report, not a folder
of recordings. Missing coverage blocks its associated empirical claim without
blocking legitimate synthetic verification.

Target the largest ambiguity with the next measurement. If damping and radiation
are confounded, add a mechanically or spatially informative view. If gesture
variability dominates, improve input observation or controlled excitation. Corpus
growth should answer questions rather than accumulate files. Coordinate apparatus
details with the measurement volume; this chapter specifies evidence use rather
than replacing acquisition protocols.
