# Finger damping, partial contact, and release transients

## Damping is a mechanical operation

A finger that touches a string can remove energy without establishing a rigid
stop. It may alter decay selectively, suppress some motion, or create a transient
as contact begins or ends. Replacing all of this with a global exponential
amplitude envelope loses spatial and state dependence. That envelope can be a
useful emergency presentation tool, but it is not the physical damping model.

This work package distinguishes a passive stationary damper, a moving driven
finger, a compliant stop, and an emergency reset. They have different energy
balances and controller semantics. The engine should not overload one command
such as `noteOff` to mean all four without an explicit adapter policy.

Inputs are the string state, contact location and extent, finger motion or
equilibrium, and a passive material law. Outputs include dissipated energy,
contact force, stored contact energy if present, and the continued string state.
The basic numerical tests can be entirely synthetic; acoustic claims require
instrument observations and practitioner review.

## A stationary distributed viscous benchmark

A simple benchmark adds a nonnegative spatial damping density `c(x)` with force
density `-c(x) u_t`. Its instantaneous power is

\[
P_d=-\int_0^L c(x)|u_t|^2\,dx\le0.
\]

For this distributed representation, `c` has units consistent with force density
divided by velocity. Do not confuse it with a lumped dashpot coefficient in
newton seconds per metre. If the damper is represented through a normalized
interpolation port instead, derive the corresponding lumped force and mass
weighting explicitly.

This benchmark is useful because its sign is unambiguous. It does not model all
finger tissue. A real contact can store elastic energy, change geometry, and
move. Those effects are added only after the passive benchmark is correct.
Time-varying a nonnegative pure damping coefficient changes the rate of loss but
does not itself create a stored elastic energy term in this idealized law.

## Spatial selectivity

A narrow damper near a modal node affects that mode differently from a damper
near an antinode. A broad contact averages over a region and can produce another
pattern. These are direct predictions to test. If the implementation applies
the same envelope to every mode regardless of location, it has not implemented
spatial damping.

Use a physical width in metres and demonstrate convergence under grid refinement.
A contact that always spans one node changes its physical width as the mesh
changes. That can make a damping coefficient appear resolution-dependent. The
same issue arises when fitting a fingertip from a coarse grid and then moving to
a finer one without refitting or transforming the model.

For two polarizations, a finger may damp directions differently. Begin with an
isotropic positive-semidefinite damping matrix and test rotation covariance.
Introduce anisotropy only with a declared frame and evidence or a synthetic
experiment. A matrix with positive diagonal entries can still be indefinite if
cross terms are wrong, so test the full quadratic form.

## Moving fingers and work

A moving finger is not necessarily passive from the string's perspective. It can
take energy away or put energy in. Define relative velocity from the actual
contact kinematics. For a lumped coordinate `q_c = I(t)u`, its derivative includes
the moving-interpolation term. For a resolved geometric contact, differentiate
the gap and tangential coordinates consistently instead of importing an
incompatible velocity formula.

A damper force opposing relative motion dissipates energy in the combined
string–finger system, while the prescribed finger actuator can supply work.
Record both quantities. Observing a temporary rise in string energy does not
prove failure if the moving finger supplied that energy. Conversely, calling the
element a damper does not excuse unexplained growth.

If the finger has its own mass and compliance, include their energy. If its
motion is prescribed, include actuator work. Do not integrate a free finger
state while simultaneously overwriting its position from a controller without
accounting for the resulting work.

## Elastic contact and release

A soft finger contact may combine a restoring potential with dissipation. At
release, any stored elastic energy has to go somewhere: into motion, into modeled
dissipation, or into an explicitly recorded reset loss. Removing the contact
object and forgetting its potential creates an untracked energy change.

A unilateral contact can naturally separate as geometry changes. A reduced
contact state may need a release trajectory. In either case, distinguish the
command to release from the instant at which mechanical contact ends. A host
note-off can initiate the former; it does not directly establish the latter.

Avoid setting velocity to zero at a finger location as a casual damping rule.
Such a projection can remove energy, but it is an impulsive constraint operation
with a specific numerical meaning. It may also create discontinuities or alter
neighboring modes. If used as a controlled approximation, derive its impulse and
energy effect and compare it with a compliant reference.

## Partial stops and harmonic-like responses

A light contact can selectively attenuate modes without producing the same
boundary as a firm stop. The resulting spectrum depends on location, width,
force, and duration. Do not implement every light touch by selecting a canned
harmonic oscillator. The persistent string state should determine which motion
survives under the declared contact model.

Whether a particular touch is part of the intended setar performance vocabulary
must be established with practitioners. The engine can support a physical
interaction without labeling it as a standard technique. Conversely, a verified
technique name does not determine a unique force trajectory. Keep vocabulary and
mechanics connected through an evidence-backed mapping rather than equating them.

Tests can initialize individual modes, apply a stationary local damper, and
compare attenuation by mode. Then use a broadband pluck and inspect the evolving
spectrum. A successful synthetic test establishes selectivity, not the
authenticity of a named ornament.

## Note-off, mute, and panic

Define separate engine commands for ending a controller ownership claim,
releasing a finger contact, applying a damping gesture, and clearing all state.
A performance adapter may map note-off to one of these under a named mode. The
mode should be visible in presets and event traces.

A panic command is allowed to clear state abruptly for safety or user control,
but it should be described as a reset. A short output fade can prevent an
unpleasant click during that reset; the fade is a presentation measure, not a
mechanical simulation. Record a reset counter so automated tests can detect
unexpected recovery behavior.

An all-notes-off message should release the notes owned by the relevant adapter
according to its policy. It should not necessarily erase sympathetic ringing or
contacts owned by another source. An all-sound-off or explicit panic may have
broader semantics. Preserve those distinctions even if a simple UI offers one
button that intentionally invokes the broadest action.

## Repeated touches and residual state

Test touch–release–touch sequences before the string decays. The second touch
must act on the continued state. Vary spacing, contact location, and phase. A
global envelope can accidentally pass a single isolated damping test while
failing these sequences because it has no representation of local motion.

Overlap a damping finger with a pluck and with a fret stop. The combined solver
must respect all active contacts. Do not apply damping after synthesis as a
separate amplitude multiplier and claim that the pluck was physically damped.
The nail's interaction can change when the string is restrained during loading.

For a paired course, test independent damping of each member and a broad finger
that contacts both. The result should follow geometry and ownership. It should
not depend on which member happens to be stored first in an array.

## Verification and measurement procedure

Start with the stationary viscous benchmark and verify nonincreasing energy in
an unforced closed string. Compare modal attenuation with an independent linear
reference. Then add time-varying contact strength, moving position, and elastic
storage one at a time. Each addition changes the expected ledger and therefore
needs its own test.

For measurements, document finger location, contact intent, visible geometry,
and the repeatability limits of force. If force is not measured, do not infer a
precise pressure value from the sound alone. Use multiple trials and retain
their differences. A performer may intentionally vary the touch in ways a simple
sensor does not capture.

Compare the model on withheld touch locations or durations after fitting. If a
single effective coefficient works only for one location, investigate width,
support, and observation effects before adding arbitrary per-note envelopes.
The goal is a predictive interaction, not a table of unrelated decay curves.

## Acceptance and limitations

The deliverables are contact-law definitions, unit checks, energy traces,
spatial-selectivity tests, note-off policy documentation, and repeated-touch
regressions. Acceptance requires no unexplained energy creation in passive
fixtures, accounted work in driven fixtures, and no accidental clearing of
unrelated string states.

The next chapter addresses intermittent fret and neck contact. A damping model
that makes every aggressive gesture quiet may hide buzz rather than explain it.
Retain the ability to inspect contact forces and spectra so numerical failures,
physical collisions, and intended damping remain distinguishable.
