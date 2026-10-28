# Radiation measurements and acoustic reference capture

## Separate the instrument from the listening chain

Mechanical mobility describes motion per force. Radiated pressure describes the
sound field. The room, microphone, preamplifier, and digital chain then transform
that field into recorded samples. A useful physical-modeling dataset keeps these
layers distinguishable. Otherwise the engine may learn a microphone coloration
or room reflection and reproduce it as if it were a property of the setar.

This chapter proposes two linked acquisitions: controlled force-to-pressure
measurements for radiation modeling, and natural performance recordings for
validation and musical review. Neither uses recordings as note-playback synthesis
assets. The source ledger provides instrument-specific context, while the
protocol below defines original project requirements rather than asserting that
any published dataset is available.

## Define the acoustic output

For a mechanical force input at a specified port and pressure at a specified
microphone position, define `H_pF(omega)=P(omega)/F(omega)` in Pa/N. If pressure
is related to a measured velocity, specify which velocity and under what
excitation; the ratio is not automatically a universal radiativity independent
of the body's motion pattern. A modal radiation model may require multiple
mechanical coordinates rather than one local velocity.

Store microphone positions in both room and specimen frames, along with
orientation, distance definition, and uncertainty. A point described as “one
metre in front” is ambiguous without a reference origin and direction. Define
whether the distance is from the bridge, a body landmark, an estimated acoustic
center, or another explicit point. Do not move that origin to improve fit without
recording the change.

Distinguish absolute pressure, relative spectra, normalized directivity, and
perceptually loudness-matched listening examples. Each is legitimate for a
different purpose. Loudness normalization must not overwrite the calibrated raw
signal or be mistaken for absolute radiation efficiency.

## Acoustic calibration

Record microphone sensitivity, frequency response information, preamplifier gain,
channel scaling, and calibration uncertainty. Use an appropriate acoustic
calibrator or traceable reference procedure through the actual chain. Check before
and after acquisition groups as required by the uncertainty plan. A stable
calibrator reading does not establish the microphone's entire directional or
frequency response, so retain those limits.

Confirm polarity and relative channel delay for arrays or phase-sensitive work.
Use a known event or reference source appropriate to the system and account for
propagation distances. If separate recorders are used, estimate clock drift as
well as initial offset. A constant time shift cannot correct a rate mismatch
over a long recording.

Measure noise with the instrument present and with relevant electronics and
facility systems in their acquisition state. Record intermittent disturbances
such as ventilation, footsteps, or handling. Do not remove these from raw files;
flag affected intervals and document any derived denoising. A noise reduction
algorithm can alter decay and spatial cues, so validation should normally use
unaltered or explicitly justified processed references.

## Facility characterization

An anechoic room has a usable operating domain; its name does not guarantee a
reflection-free field at every frequency and position. Obtain or measure the
facility's relevant free-field characterization, background noise, and usable
volume. Record the instrument and microphone arrangement within that volume.
If the low-frequency region is not adequately controlled, label it accordingly.

In a nonanechoic room, impulse-response gating may isolate early sound over a
limited band, but the short window constrains frequency resolution. Do not claim
low-frequency radiation detail that requires a longer window than the reflection-
free interval. Room-response subtraction or deconvolution needs its own model
and validation; it is not an automatic way to recover anechoic truth.

Repeat selected measurements after changing source or microphone position to
diagnose room contributions. A resonance that remains fixed in the room frame
but changes relative to the instrument may be environmental. Use this as a
diagnostic alongside other evidence, not as an infallible classifier.

## Controlled radiation acquisition

Use a calibrated mechanical excitation and capture force and pressure
synchronously. If body velocity is also recorded, the combined dataset can test
whether a candidate mechanical reduction predicts acoustic output. Choose safe
excitation levels that remain in the intended linear regime. Record support and
string damping state because they alter both motion and sound.

For each angular position, obtain enough repeatability evidence to distinguish
directional differences from trial variation. A roving microphone can reduce
channel mismatch but introduces time and setup drift. A simultaneous array
captures one event consistently but requires interchannel calibration and may
perturb the field. Choose the tradeoff explicitly.

If the instrument is rotated, determine whether its support and string state
change with orientation. If the microphone is moved, preserve its pointing
direction and cable arrangement or record the changes. A rotation of the
instrument is not equivalent to a microphone move when gravity, player support,
or fixture contact changes the source.

## Angular and distance coverage

Choose measurement directions according to the intended output model and
anticipated spatial complexity. A few cardinal directions may support a coarse
directional hypothesis but not a high-order spherical representation. Determine
the required spatial bandwidth through pilot data and held-out directions.
Do not fit more angular degrees of freedom than the coverage supports.

Use a geometry that avoids accidental concentration of points in one region of
the sphere when estimating integrated quantities. Weight directions by their
associated solid angle or quadrature rule. Equal averaging of irregularly spaced
microphones is not generally an unbiased spatial average. Record missing
directions caused by supports, safety limits, or room constraints.

Test more than one distance where feasible. Far-field assumptions require
evidence for the source size and frequency band. Near-field pressure can contain
reactive components that do not scale as a simple outgoing spherical wave.
Do not infer acoustic power from one close microphone using an inverse-square
formula without establishing the assumptions.

## Phase and propagation

Preserve absolute timing where the acquisition makes it meaningful. If removing
propagation delay for analysis, use measured geometry and a documented sound-speed
estimate with uncertainty. The correction changes phase and can magnify position
errors at higher frequencies. Store corrected and uncorrected transfer functions
as separate derived assets.

Interpolate complex responses carefully. Interpolating magnitudes and wrapped
phases independently can create discontinuities or implausible cancellations.
A stable modal or state-space radiation representation may provide a better
structure, but it must be tested against withheld directions and frequencies.
Do not impose a zero-phase filter merely because phase is inconvenient; that
changes transient behavior and is not a physical propagation model.

## Pressure, intensity, and power

For an approximately progressive far-field wave, time-averaged intensity can be
related to RMS pressure by `I = p_rms^2/(rho_air*c)`. Integrating this over a
closed measurement surface can estimate radiated power under the relevant
assumptions. This relation is not valid as a universal near-field intensity
measurement. A pressure microphone alone does not measure particle velocity.

State whether complex spectral amplitudes are peak, RMS, one-sided, or two-sided;
otherwise factors of two can appear in energy and power calculations. Define
spectral density normalization and frequency integration explicitly. Acoustic
levels derived from pressure should state the reference pressure and bandwidth.
Do not compare a broadband RMS level with a single-bin magnitude as if they
were the same observable.

When the project only needs a perceptually plausible listening transfer, it may
choose not to estimate total power. That is acceptable if the claim is limited.
Do not add a nominal power number without the angular coverage and assumptions
needed to support it.

## Natural performance references

Capture performances with the instrument, player, and microphone arrangement
documented. Preserve a stable reference microphone and, if useful, additional
close or spatial channels. Mark which channels are intended for scientific
comparison and which for artistic presentation. Do not change gain between takes
without recording the change. Leave headroom based on a pilot that includes the
strongest approved articulations.

Record silence, tuning references, isolated gestures, transitions, and musically
meaningful phrases. Use player-reviewed material and permission covering the
composition or improvisation as well as the performance recording. A technically
excellent take may still be unusable for distribution if its rights are unclear.

Natural performances need not be mechanically identical to be useful. They test
whether the model covers meaningful variation and whether a control mapping feels
musically coherent. Use controlled plucks for tightly matched physical comparisons
and natural phrases for broader behavior. Do not pretend one protocol answers
both questions perfectly.

## Listening preparation

Create derived listening stimuli through a logged transformation: alignment,
trim, any loudness matching, fades, and format conversion. Keep raw calibrated
files immutable. Avoid processing that selectively improves the model or degrades
the reference. If reverberation is added, apply the same documented listening
condition when the question requires it.

Use hidden identities and randomized presentation where the study design needs
blinding. Include repeated items to estimate listener consistency and anchors
whose purpose is explicit. A preference result is not automatically a realism
result. Ask separate questions when needed: similarity to the reference,
plausibility as a setar, controllability, and musical usefulness can diverge.

## Acceptance

The radiation packet passes when force, pressure, timing, geometry, facility
limits, and uncertainty are traceable, and the proposed output representation
predicts withheld directions or conditions within its scope. Stop absolute
claims when calibration is missing, spatial integration when coverage is
inadequate, and phase claims when clocks are not aligned. A carefully labeled
relative acoustic dataset remains valuable without pretending to be more complete.

