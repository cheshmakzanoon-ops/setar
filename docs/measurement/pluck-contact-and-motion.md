# Pluck, fingernail contact, and gesture capture

## Measurement goal

The objective is to connect a player's intended gesture to physical excitation
without reducing performance to a velocity number. Relevant variables can include
contact location, approach direction, string displacement, release timing,
polarization, nail geometry, finger continuation, and interaction with neighboring
strings. Not every variable needs to be measured for every trial, but the model
claim must match what is observed.

R-SETAR-TABASSIAN provides performer-authored context for fingernail excitation and
ornamentation. It does not supply the project's force law or gesture statistics.
The protocols here are proposed mechanism and performance studies, with player
review and consent as prerequisites. Do not ask musicians to make harmful or
unnatural repetitions merely to fill a parameter grid.

## Separate mechanism trials from natural performance

A mechanism trial isolates a small question: how release time changes with
contact geometry, whether the string remains in contact after a particular
motion, or how polarization depends on approach direction. A natural performance
trial preserves musical intent and the player's preferred movement. Instrumented
mechanism trials can constrain a model while still being poor examples of normal
playing. Label them accordingly.

For each gesture family, ask the player to demonstrate the intended technique,
describe its context, and identify variants that matter. Record the player's
terminology and an operational physical description separately. Translation or
transliteration should not erase the distinction between related techniques.
If different players use the same term differently, retain that disagreement
instead of forcing it into one engineering label.

## Instrumentation choices

High-speed video can estimate contact and motion but suffers from occlusion,
projection, motion blur, frame timing, and rolling-shutter effects. Laser or
optical string sensing can provide high temporal resolution but may observe only
one projection or a limited spatial region. Force transducers can quantify
interaction while changing mass, stiffness, friction, and the player's sensation.
No sensor is neutral simply because its output has many digits.

Select instruments according to the question. To establish release timing, the
temporal uncertainty must be smaller than the effect being compared. To estimate
displacement, spatial calibration and viewing geometry must support the required
scale. To infer force from motion, the mechanical model and boundary conditions
must be identified independently enough that the inverse force is meaningful.

Pilot the complete arrangement with the player. Ask whether markers, lighting,
cameras, or sensor attachments alter technique. Record the response and, where
possible, compare instrumented and minimally instrumented trials. If the setup
changes the behavior, keep its data for the mechanism question and do not call it
unqualified natural-performance ground truth.

## Coordinate definitions

Express contact position along the string as both an absolute distance and a
normalized fraction of the current speaking length. The fraction alone can hide
a change in actual location when fretting changes the length. Define transverse
directions relative to the instrument and string, and track how those directions
map into camera coordinates.

Describe nail motion with a local contact frame when useful: normal approach,
tangential sweep, and out-of-plane movement. The frame is a model definition, not
necessarily an anatomically fixed coordinate system. If it is estimated from
images, store the estimation uncertainty. A two-dimensional trajectory cannot
uniquely recover a three-dimensional path without additional assumptions or views.

## Synchronization

Use a shared trigger or synchronized acquisition when available. Otherwise record
observable synchronization events in each modality and fit the relationship
between time bases. A useful affine model is `t_audio = a*t_video + b`, where
`b` is offset and `a` captures clock-rate mismatch. Test residuals against this
model; dropped frames or variable-rate capture may require piecewise mapping.

Store the synchronization events, fitted mapping, residual uncertainty, and
valid interval. Do not align every pluck independently to the model's preferred
release point, because that can erase the timing discrepancy being tested. A
single clap or flash can establish an offset but not long-term drift. Repeated
events or a shared clock provide stronger evidence.

The acquisition pipeline must preserve original timestamps and frame indices.
Transcoding video can change timing metadata, duplicate frames, or conceal
variable frame rate. Keep original files and verify any derived analysis format.
Do not use the nominal frames-per-second label as the only timing evidence.

## Contact annotation

Define observable events before annotating: first apparent contact, visible
deflection, maximum observed displacement, release interval, possible recontact,
and damping contact. Use intervals when an event lies between frames or is
occluded. A missing observation is not proof that contact did not occur.

Have at least a subset annotated independently by another reviewer. Compare
agreement on event existence and timing, not just label spelling. Disagreements
should reveal ambiguous definitions or insufficient resolution. Revise the
annotation guide and preserve its version. Do not adjust labels solely to make
the physical model fit better.

Link annotations to unprocessed frame references and channel times. If an
automated detector is trained on hand labels, partition by player and session
where generalization is claimed. Report detector errors and propagate timing
uncertainty into contact-model fitting. A confident detector score is not a
substitute for measured event accuracy.

## Excitation descriptors

Avoid defining every pluck by a single scalar such as MIDI velocity. Store a
compact physical descriptor vector with units and uncertainty: position,
direction, displacement, duration, release speed or another supported proxy,
string identity, fretting state, and intended articulation. Some fields may be
unknown. Preserve unknowns instead of filling them with population averages that
look measured.

For a simple string initialized by displacement, the modal excitation depends on
the spatial displacement profile, not merely its maximum. A finite contact can
also introduce velocity and distributed force. When reducing observations to an
initial-condition model, document what is discarded and compare predictions
against a finite-duration model on held-out gestures.

Energy provides a useful consistency check. If force and contact velocity are
measured with compatible signs, input work is the time integral of their product.
If only displacement is observed, energy inferred through a string model inherits
that model's assumptions and parameter uncertainty. Do not present an inverse
energy estimate as a direct force measurement.

## Nail and finger geometry

Record only the geometry needed for the research question and approved by the
participant. Nail edge curvature, protrusion, contact orientation, and a
nonidentifying reference image may be relevant. Detailed biometric imagery or
unnecessary personal information is not required for a physical model. Store
identifiable video under restricted access and publish only authorized derivatives.

Do not impose a nail shape or grooming change as a casual experimental variable.
Such a change affects the musician beyond the session and needs explicit
agreement. Prefer observing natural variation or using a separate mechanical
surrogate for controlled contact experiments. A surrogate can test a contact law
but does not establish the full range of human technique.

## Repetitions and variability

Use repeated gestures to estimate within-player variation, with breaks and task
changes chosen around comfort and data quality rather than an arbitrary quota.
Observe between-session and between-player variation separately. Thousands of
nearly identical strokes by one player do not establish a broad performance
distribution.

Counterbalance gesture order to reduce fatigue, adaptation, and tuning drift
confounding. Record player comments about unusual takes. Include negative or
failed attempts only with clear labels and consent; they may illuminate model
boundaries without representing intended performance. Never present them as
examples of the player's competence in public materials.

## Mapping to real-time controls

The research output should propose a low-dimensional control mapping whose
variables correspond to observed meaningful variation. It need not expose every
measured degree of freedom. Separate deliberate gesture controls from stochastic
variation. A randomizer that changes pitch or contact independently at every
sample can destroy physically and musically meaningful correlations.

Fit distributions conditionally on articulation, string state, and player context
where supported. Preserve correlations between location, direction, and release
behavior. When data are sparse, use a simpler deterministic mapping with declared
limits rather than a complicated statistical model trained on a handful of takes.
Test the mapping with players who did not supply its calibration gestures.

## Acceptance and failures

The gesture packet passes when timing, geometry, annotations, consent, and
instrumentation disturbance are documented, and the chosen excitation model
predicts held-out observations within its declared domain. Stop force-law claims
if force is unobserved and inverse assumptions are untested. Stop sub-frame
timing claims when the camera cannot support them. Stop natural-performance
claims when the apparatus materially changes the gesture.

The next action should address the specific ambiguity: another camera view,
better synchronization, an independent motion channel, a less intrusive setup,
or a narrower model. More annotated frames cannot recover information that the
acquisition never captured.
