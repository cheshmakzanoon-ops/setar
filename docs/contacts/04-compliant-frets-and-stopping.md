# Compliant tied frets and stopping an existing string

## Stopping is a boundary interaction

A stopped note is not obtained physically by multiplying the open-string pitch.
The player changes contact conditions on an existing string. The fret provides
support, the finger supplies a load and damping, the string's path can lengthen,
and motion may persist on both sides of the contact. A pitch-scaled oscillator
can be a useful musical approximation, but it cannot establish those mechanics.

The proposed reference retains the full physical string and represents fret and
finger contacts separately. A reduced real-time model may use an effective
termination if it is derived and validated over a stated range. The distinction
between reference and reduction should remain visible in configuration and
evidence, especially for transitions and repeated plucks.

Prerequisites are measured or synthetic fret geometry, the finite-contact
conventions, and a verified nonlinear integration framework. Inputs are the
current string state, fret profile, finger trajectory or load, and compliance
records. Outputs are contact forces, realized speaking behavior, tension changes
if included, dissipation, and continued motion of retained segments.

## Geometry of a tied fret

Use the [shared setup-state measurement
contract](../measurement/strings-frets-and-setup.md) for fret identifiers,
installation conditions, and geometric observations. Keep a model-specific
contact profile linked to that record so changing the profile approximation does
not overwrite the measured geometry.

Record the fret's contact surface rather than treating its name or index as a
complete specification. Relevant quantities include its longitudinal coordinate,
height above the neck reference, transverse variation, radius or local profile,
and possible flattening under load. A tied gut fret is not automatically a rigid
metal ridge. Its effective compliance depends on its construction and support,
which must be measured or explicitly approximated.

The string's equilibrium height and action determine whether it reaches the fret
under a given finger command. A contact profile uses the same coordinate system
as the string and neck. If `z` points away from the soundboard, a simple lower
obstacle gap may be written as string-surface height minus fret-surface height.
The string radius and local geometry must be included consistently rather than
being counted once in each surface.

An angled fret can intersect different strings at different longitudinal
positions. A one-coordinate-per-fret representation is a reduction. Test whether
its error matters for the intended tuning claim. Do not overwrite a measured
angle with a mathematically convenient perpendicular line and still label the
configuration measured.

## Finger and fret are separate contacts

The finger typically acts from the opposite side of the string from the fret.
Its position behind or near the fret, its shape, and its compliance influence
loading. A model that places the finger exactly at the fret and applies an ideal
zero-displacement condition has already discarded those effects. State that
reduction openly if it is used.

Use distinct contact IDs and material laws. The fret can transmit reaction to a
neck or body representation, while the finger may dissipate energy and be driven
by a prescribed or compliant actuator. Their forces should not be collapsed into
an arbitrary shortening of length unless the resulting reduced impedance is
derived and tested.

Simultaneous contacts can overconstrain a naive solver. A prescribed finger path
that drives the string through a rigid fret demands incompatible geometry unless
the finger or fret yields. Choose a consistent compliance or constraint
formulation and report excessive penetration or force. Do not resolve the
conflict by silently reducing the user's pressure command.

## Compliance and support mobility

A local fret model may use a spring, dashpot, mass, or passive internal variables.
The simplest credible choice depends on the observation band. A pure spring
stores energy but does not dissipate it; a dashpot dissipates but does not store
energy. A viscoelastic fit can do both. Its parameters need units, provenance,
and a stated range of load and frequency.

If the neck's measured mobility already includes the installed fret behavior,
adding a separate explicit fret compliance may double-count it. Decide which
physical effects live in the contact element and which live in the support
model. Document the measurement boundary: where was force applied, where was
velocity measured, and what was attached during acquisition?

An effective compliance fitted from one note may include finger behavior,
string stiffness, and support motion. It should not be advertised as the material
modulus of gut. Use a name such as effective contact compliance and preserve the
conditions of the fit. This naming discipline prevents later contributors from
reusing a convenient coefficient as a universal material property.

## Realized pitch and pressure

In a simple rigid-stop limit, the speaking length is geometrically determined.
In a compliant model, the effective termination can be frequency-dependent and
the static path can alter tension. Pressure therefore may affect more than
damping. The model should predict the realized spectrum from its state rather
than force it to a preselected note frequency after the fact.

Compare four quantities explicitly: intended interval, fret coordinate, predicted
frequency, and measured frequency. A mismatch can reveal geometry error,
compliance, tension change, or an inadequate pitch estimator. It does not justify
silently moving the fret or applying output pitch correction. Those are separate
operations with separate user-facing meaning.

The tuning chapter's ideal length ratio is a useful limiting test. As fret and
finger compliance approach the selected ideal boundary under compatible
conditions, the model should approach that limit. Infinite stiffness is not a
practical numerical setting; use a controlled sequence and report convergence
and conditioning.

## Segments that do not disappear

The nut-side segment can retain energy after a stop. Depending on finger damping
and contact, it may be weakly audible, transfer energy, or decay rapidly. The
reference model should retain it when studying stopping transients. A reduced
model may eliminate it only with a documented energetic and acoustic error.

If an effective speaking segment is created, map displacement and velocity from
the prior full-string state. The mapping should preserve the relevant physical
field where possible and account for discarded energy. Initializing the new
segment at rest creates an artificial mute; copying an unrelated waveform can
create an artificial excitation. Both can sound superficially acceptable while
failing repeated-note behavior.

Release of the stopping finger reverses neither operation exactly. Contact
forces vanish or change according to geometry, and the full string continues
from its current state. A newly open note need not begin with the same state as
an isolated open-string pluck. This is one reason note-triggered voice banks are
an inadequate reference for the task.

## Slides, rolls, and changing contact location

A moving finger can slide along the string, change load distribution, or transfer
support between neighboring frets. The geometric contact point can move without
the physical fret moving. Separate those quantities in the event schema.
Dragging a tied fret to a new position is a configuration operation, not the
same as a finger slide during performance.

Moving interpolation carries the geometry terms described in the nail chapter.
Work from the finger actuator and frictional loss must remain distinguishable.
A glide produced by continuously scaling an oscillator frequency can be useful
as an assisted mapping, but it should not be confused with a verified sliding
contact model.

When multiple left-hand fingers are active, their contacts can overlap. Define
which constraints are physically present, not merely which note was most recent.
A lower-position finger may remain on the string when a higher-position finger
is added. Removing the higher finger can expose a previously established stop.
That behavior needs persistent finger identities and state, not a single global
fret number.

## A task-specific verification sequence

Begin with a static equilibrium test: prescribed finger load, a compliant fret,
and no vibration. Compare force balance and deformation with an independent
small-system reference. Then excite a small perturbation about that equilibrium
and compare modal frequencies and decay with a linearized reference. This
separates static-geometry errors from dynamic-contact errors.

Next engage the finger while the string is already moving. Vary phase and speed
of engagement, then release it without a new pluck. Track energy in both string
segments, contact storage, dissipation, and actuator work. Repeat under spatial
and temporal refinement. A correct steady stopped pitch is not enough if the
transition injects unexplained energy.

Finally add two fingers and test add/remove order. Include a paired-course
condition where a finger affects one or both members according to geometry.
Compare with the intended practitioner gesture only after the mechanical
invariants pass. Practitioner disagreement about a mapping is evidence to
preserve, not a reason to rewrite terminology without consultation.

## Evidence and exit decision

Deliver a fret-geometry schema, contact/support boundary statement, equilibrium
solver evidence, transition traces, and a comparison of full-domain and reduced
models if both exist. The gate requires correct ideal limits, bounded and
convergent contact behavior, no unexplained disappearance of string energy, and
explicit treatment of overlapping fingers.

If only a pitch-scaling approximation is currently implemented, state that
limitation plainly and keep this chapter as an unimplemented design contract.
The next chapter treats damping and release in more detail, because a convincing
stopped pitch with an unrealistic decay or impossible note-off behavior is not
yet a playable setar model.
