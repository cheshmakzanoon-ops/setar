# P10 — Cavity, sound radiation, and the observation model

**Status:** specification. **Prerequisites:** P08; P09 supplies coupled structural
motion. **Exit evidence:** an acoustic observation model with verified units,
boundary conditions, coupling work, and a measured or explicitly synthetic
validity domain.

## Mechanical motion is not yet recorded sound

A displacement pickup at one string point is useful for numerical development.
It is not what a listener hears in front of a setar. Sound production involves
surface motion, cavity behavior, openings, surrounding air, and observation
position. This phase specifies which of those interactions are solved, reduced,
or deliberately omitted. Read [acoustics](../acoustics/README.md),
[body](../body/README.md), and [validation](../validation/README.md).

Keep the instrument's radiation model separate from an artistic room effect.
Adding reverberation can improve a demonstration while making a measurement
comparison less interpretable. The core validation path needs controlled
observation conditions before optional presentation effects are considered.

## P10-T01: define acoustic variables and boundaries

State pressure, particle velocity, density, speed of sound, coordinate frame,
and units. Distinguish velocity potential from pressure when reading sources;
their scaling and time derivatives are not interchangeable. Define normal
directions consistently at structural/acoustic interfaces and openings.

Describe the cavity domain, openings, exterior domain, and boundary conditions.
An ideal rigid wall, prescribed vibrating surface, and fully coupled elastic
wall answer different questions. Specify whether a radiation impedance loads
the structure or whether pressure is only computed afterward. A one-way
observation model cannot claim to include radiation damping in the mechanical
state unless that loss is represented elsewhere with an explicit accounting.

Record environmental properties used for air. Keep uncertainty appropriate to
the measurement context; do not present a many-digit speed of sound as a fixed
universal constant when temperature and humidity were not controlled.

## P10-T02: verify elementary acoustic cases

Before importing the instrument, reproduce suitable analytic or high-confidence
reference problems: an enclosed cavity mode, a simple radiating source, and a
boundary/interface test. Choose cases that exercise the intended discretization
and units, not merely problems a software package solves by default.

Evaluate phase, amplitude, distance behavior, and conservation or radiated power
where applicable. State the far-field or near-field assumptions. A formula
valid far from a compact radiator cannot be applied arbitrarily close to a
complex surface without analysis. Use mesh and frequency refinement with
error measures appropriate to complex-valued fields.

For an exterior truncated domain, quantify reflection from the numerical
boundary. For a boundary-element formulation, evaluate singular quadrature,
conditioning, and formulation-specific resonance issues. A visually smooth
pressure map is not evidence that the exterior problem is correct.

## P10-T03: couple cavity and structure

Define how pressure traction loads the structure and how surface normal
velocity drives the acoustic field. Verify the interface work identity and
normal sign. When different meshes are used, construct interpolation/projection
operators that preserve the required pairing; nearest-node matching alone may
not preserve power.

Compare uncoupled and coupled cases to isolate the acoustic loading effect.
Check whether retained structural/acoustic modes are sufficient near the
resonances of interest. Avoid adding an empirical cavity resonance on top of a
fully coupled cavity that already contains the same mechanism.

If the initial production model uses a reduced cavity or radiation impedance,
derive it from a verified reference or an identified measurement with stated
limits. Check causality, stability, and passivity where the representation acts
as a load. A fitted frequency curve that matches magnitudes can still have an
unphysical phase or active impedance.

## P10-T04: define microphone and listener observations

Specify microphone positions and orientations relative to the instrument,
including distance and uncertainty. Keep microphone calibration, frequency
response, directivity, and room effects separate from the intrinsic instrument
model. A free-field prediction should not be fitted directly to a reverberant
room recording without accounting for the observation chain.

Choose whether the production instrument exposes a fixed virtual microphone,
several measured/derived observations, or a spatially interpolated field. Verify
the interpolation with held-out positions. Do not create arbitrary stereo width
and describe it as measured directivity. Stereo presentation may be useful,
but its origin and limits must be explicit.

For comparisons, preserve absolute level when the question concerns efficiency
or delivered work. Use level matching only when the perceptual question calls
for it, and report the applied gain separately. Automatic normalization can
erase evidence of a wrong coupling scale.

## P10-T05: reduce for real-time observation

Factor frequency-domain or modal radiation maps into a runtime representation
with a declared causal/stable realization. Compare impulse response, complex
frequency response, and position dependence with the offline reference. Keep
the valid frequency range and sample-rate behavior in the asset schema.

Assess truncation, interpolation, and discretization separately. A production
filter's error must not be attributed to the underlying body physics without
an isolation experiment. Check changes in microphone position or radiation
asset during ringing for safe state transitions and predictable work/output
behavior. Perform expensive fitting and factorization outside the callback.

## P10-T06: assess radiation against measurements

Use calibrated structural and acoustic observations where possible to separate
wrong surface motion from wrong radiation. Compare several positions and
gestures, not a single equalized spectrum. Evaluate low-frequency response,
resonance/antiresonance structure, directional changes, phase where measurable,
and uncertainty from the room and microphones.

The exit report contains actual pressure/directivity plots, units, scale,
observation geometry, calibration chain, and held-out cases. If data remain
unavailable, record the verified synthetic acoustic solver and the unvalidated
instrument prediction as separate achievements.

> Implement the specified acoustic reference and observation chain. Identify
> whether coupling is one-way or two-way, prove/check the interface work signs,
> and verify elementary cavity/radiation cases before using the instrument
> geometry. Preserve complex response and level information. Compare reduced
> and full predictions and do not count room reverb, EQ, or stereo decoration
> as evidence of a correct setar radiation model.
