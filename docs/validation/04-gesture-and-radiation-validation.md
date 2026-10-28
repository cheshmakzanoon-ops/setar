# Gestures, coupling, and radiation as separate validation axes

An instrument is more than a collection of isolated notes. The setar model must
eventually represent controlled excitation, movable-fret interactions, ringing
strings, and receiver-dependent sound. This chapter specifies experiments that
separate those mechanisms. It assumes verified subsystems and a corpus whose
permissions and provenance are known. None of the experiments is implied to
exist merely because a synthetic reference renderer is available.

## Gesture representation and ground truth

Represent a gesture as a sequence or trajectory of physical controls with units,
coordinate conventions, and uncertainty. A fingertip position estimate is not a
normal contact force. A controller aftertouch value is not automatically either.
Maintain three layers: observed player action, inferred physical trajectory,
and product controller mapping. Comparing two layers without naming the mapping
creates false precision.

For each gesture choose observable checkpoints: first contact, increased normal
force, sliding interval, nail release, damping onset, and final release. Sensors
may not observe all checkpoints directly. Give inferred events uncertainty
intervals and specify how they are inferred. Video can help identify event order,
but its frame rate and synchronization limit timing claims. Do not interpolate
a detailed force profile from sparse imagery and call it measured.

Use a simple, repeatable gesture before a compound ornament. A stopped note with
a controlled contact change can expose a boundary-work error that a musical
phrase obscures. Compound gestures then test whether individually credible
mechanisms remain credible when they overlap. Preserve the distinction between
an isolated-mechanism claim and a complete-performance claim.

## Contact-transition experiment

Fix string state and excitation. Compare a free segment, a controlled contact
approach, sustained contact, and release. Observe pitch trajectory, transient
energy, high-frequency content, contact force where measurable, and residual
ringing. The model should account for external work performed by moving contact,
not assert passive energy decay during every movement.

Use an offline reference with tighter numerical controls to distinguish physical
transients from discretization artifacts. Refine contact timing relative to the
sample grid. If moving the same event by a fractional step changes energy
dramatically, the numerical treatment needs investigation before acoustic fitting.
Repeat the test with the string initially at different oscillation phases;
contact is phase-dependent, so a single convenient phase is inadequate.

The acceptance packet contains input trajectories, state/energy traces, event
times, acoustic output, convergence results, and uncertainty. A smooth waveform
alone does not demonstrate correct contact: an arbitrary crossfade may eliminate
the artifact while deleting the physical energy transfer. Compare the physical
reference and any product-safe fallback as separate models.

## Repeated attacks and ornaments

Repeated plucks can interact with residual motion. Test attacks at different
phases and spacings, including near a strong body or string beat period. Compare
the history-dependent response with a reset-between-notes control. A model that
resets all state on every note may sound consistent but cannot support a claim
about continuous physical interaction.

For ornament evaluation, work with knowledgeable players to define the gesture
and terms. Do not assume that one keyboard pitch-bend pattern represents every
practice. Ask which finger changes position, which string remains ringing, where
excitation occurs, and which acoustic cues identify a successful gesture. Record
practitioner judgments separately from generic audio-quality ratings.

Use blinded comparison for sound character and unblinded interaction evaluation
when testing playability. These answer different questions. A performer needs to
see controls during usability work; that session cannot simultaneously be
presented as a visual-bias-free timbre test. Preserve the experimental context
and label observations accordingly.

## Sympathetic and paired-string coupling

Excite one string while monitoring its companion and bridge response. Repeat
with selected strings damped, and with small controlled detuning. Compare energy
transfer, beat structure, and decay changes. A simple sum of independently
triggered voices may reproduce two pitches but not the transfer mechanism.

Instrument coupling can be direction-dependent through geometry and observation.
State which force and velocity components are compared. Reciprocity expectations
apply only under the relevant linear, passive, time-invariant assumptions and
consistent coordinates. Nonlinear contact or changing constraints can invalidate
a simple reciprocity test; document the regime rather than quietly discarding
the result.

Do not infer coupling strength from one microphone alone if radiation weighting
can explain the same change. Add a mechanical observation or multiple receiver
views when possible. Fitting independent gains for each take can erase the
cross-string amplitude relation that should identify coupling. Preserve a
shared calibration wherever the claim depends on absolute or relative energy.

## Radiation and receiver geometry

The modeled source and receiver must share a declared coordinate system. Record
distance, orientation, channel order, and any room treatment. A single close
microphone is an observation of one view, not the entire radiation field.
Distinguish near-field behavior, free-field approximation, and room-contaminated
measurements. A body mode's amplitude in one direction cannot by itself establish
its total radiated power.

Evaluate multiple receiver positions with common calibration. Compare amplitude
and phase when synchronization supports phase claims, otherwise compare
appropriate magnitude or energy features and state the limitation. Use
frequency-dependent directivity plots with identical scale and orientation.
Show missing angular coverage, not a smooth interpolated sphere that implies
unmeasured precision.

Keep room response separate from instrument radiation. If room convolution is
used for a listening experiment, apply a controlled and declared treatment to
both candidates. If the measured reference already contains a room, convolving
only the model with a guessed room can confound model and room errors. Prefer
an experiment designed to isolate the intended factor, or limit the claim to
the complete recorded scene.

## Stereo and spatial presentation

Stereo width is not a free physical constant. Define whether channels represent
two microphone positions, a binaural receiver, or an artistic rendering. Test
channel identity, polarity, mono compatibility, and behavior when the host asks
for mono. A mono fold-down should follow the declared output representation,
not silently change the physical model.

A visually attractive rotating instrument control needs a documented acoustic
meaning. If rotation only changes an artistic pan law, label it accordingly.
If it changes source/receiver geometry, the radiation operator and measurement
coverage must support that interpretation. Validate interpolation between views
and avoid unexplained energy jumps at angular boundaries.

## Separating failure hypotheses

When a phrase sounds too bright, possible causes include nail-release duration,
pluck position, boundary contact, excessive high-mode energy, body mobility,
radiation direction, or receiver coloration. Use targeted interventions. If
bridge motion agrees but microphone spectrum does not, investigate the observation
chain before rewriting string damping. If isolated attacks agree but repeated
attacks fail, investigate state history and gesture scheduling.

Maintain a hypothesis table listing predicted changes under each intervention.
An experiment is valuable when hypotheses predict distinguishable outcomes.
Collecting more examples of the same ambiguous condition may reduce random noise
without resolving the structural ambiguity. This is the bridge between validation
and the identifiability analysis in the next chapter.

## Gate and next action

Acceptance requires verified event semantics, declared physical coordinates,
controlled or characterized input, independent observations, and feature-specific
criteria. A compound-gesture result names the tested gesture family and controller
mapping. A radiation result names receiver coverage and room assumptions. Neither
generalizes automatically to all playing styles or listening positions.

The next action is the smallest experiment that distinguishes an uncertain
mechanism: a phase-shifted repeated attack, a damped-string control, or an
additional receiver view. Keep proposed tests visible when acquisition is
unavailable; do not replace absent measurements with invented curves.
