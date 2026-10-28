# Friction, flesh transitions, and stroke direction

## Direction is geometry before it is timbre

A reverse stroke changes the path of the nail relative to the string array. It
can change encounter order, surface orientation, contact duration, and the
relative contribution of nail and flesh. It should not be implemented solely as
a brighter or darker preset selected by a sign bit. Such a tone switch can be a
useful controller approximation, but it is not a physical explanation.

The first implementation task is to define stroke direction in instrument
coordinates and let a practitioner map that definition to their terminology.
Avoid assuming that a camera's left/right, a player's inward/outward, and a
musical stroke name are interchangeable. The same visual motion can appear
reversed from another viewpoint. Store a reference frame and a verified label
mapping with the performance profile.

Prerequisites are the finite-contact gap convention and two-axis string motion.
Inputs are a surface path, local normal and tangent frame, contact parameters,
and current state. Outputs include normal/tangential forces, slip state where
modeled, dissipated work, and a record of surface transitions. All parameter
values without measurement remain synthetic or inferred.

## A minimal dissipative friction law

Let `v_rel,t` be the relative tangential velocity of the string surface against
the nail surface. A regularized kinetic-friction candidate in one tangential
direction is

\[
F_t=-\mu_f F_n\tanh(v_{rel,t}/v_\epsilon),
\]

where `mu_f` is nonnegative and dimensionless, and `v_epsilon` is a positive
velocity scale. The
force opposes relative motion, so `F_t * v_rel,t` is nonpositive when `F_n` is
nonnegative. The associated lost energy must be included in the ledger.

This regularization is an engineering choice. It does not reproduce ideal static
friction exactly, and the smoothing velocity can affect slow capture and release.
It must therefore be tested as a parameter, not selected only to make a solver
converge. A vector tangential law needs a norm and direction treatment that is
well-defined at zero velocity; extending a scalar formula independently to axes
can introduce unintended directional bias.

For a model with torsion, surface relative velocity includes rotational motion
through the string radius or lever arm. The same geometry transforms tangential
force into torque. If torsion is absent, state that the law acts on translational
surface motion under a reduction. Do not claim that omitted torsion has been
measured to be irrelevant merely because the model runs.

## Static friction and stored tangential energy

If stick–slip behavior is important, a tangential compliance or constrained stick
state can represent it. Such a state stores information and possibly energy.
Its update, yield criterion, reset at separation, and interaction with a changing
normal force must all be explicit. A variable named `stick` is not a constitutive
law.

A tangential spring model needs a rule for what happens when the contact patch
moves or the local frame rotates. Reusing an old displacement vector in a new
frame can create an artificial impulse. Rotating the state covariantly may be
appropriate for one material interpretation; resetting it may dissipate stored
energy. Either choice must follow a model and be recorded in the ledger.

If the normal load falls toward zero, the admissible friction force usually
changes. A release algorithm must handle this transition without dividing by a
vanishing load or leaving a tangential force after separation. Tests should
approach zero load from several slip and stick states, not just from rest.

## Nail and flesh are different model regions

A finite fingertip representation can include a relatively stiff nail region,
a more compliant flesh region, and a geometric transition between them. That
does not require a detailed anatomical mesh. A small number of physically
interpretable surface regions can be a useful intermediate model if their
geometry and effective compliance are identified or openly synthetic.

Do not blend material coefficients solely as a function of MIDI velocity. The
transition should be associated with which surface region is in contact. A
controller may influence the path that selects that region, but the causal chain
must remain visible. If an empirical controller curve directly changes the
mixture, label it as an expressive mapping rather than a measured biomechanical
law.

At a region boundary, discontinuous stiffness or friction can produce a real
model discontinuity and a difficult numerical event. Decide whether the physical
surface warrants a smooth transition or a sharp one, then test refinement.
Smoothing is not automatically more accurate; it is a regularization whose scale
needs justification and sensitivity analysis.

## Texture and noise should have a mechanism

Surface roughness can be modeled through a geometric perturbation, a fluctuating
friction parameter, or a more detailed contact law. Each choice implies a
different source of work and bandwidth. A random noise burst mixed into the
audio output may be artistically useful, but it does not establish physical
roughness synthesis.

If stochastic roughness is explored, define its spatial or temporal correlation,
amplitude, seed, and dependence on sliding speed. A spatial surface sampled by a
moving contact is not equivalent to time-white noise independent of speed. The
same surface traversed twice should have the intended repeatability. Record
whether the random state belongs to a specimen surface, a performer profile, or
an experiment.

Do not fit high-frequency noise until the deterministic contact is converged and
the observation chain is understood. Aliasing, numerical chatter, microphone
self-noise, and fret buzz can otherwise be mislabeled as nail texture.

## Reading and empirical limits

Pavlidou and Richardson's [classical-guitar string–finger
study](https://www.ioa.org.uk/system/files/proceedings/m_pavlidou_and_be_richardson_the_string-finger_interaction_in_the_classical_guitar_theoretical_model_and_exper.pdf)
is a primary example of treating nail friction and multi-directional string
motion as part of the interaction. Chadefaux, Le Carrou, and Fabre's [A Model of
Harp Plucking](https://pubmed.ncbi.nlm.nih.gov/23556609/) illustrates measurement-
informed finger interaction in a different instrument context. Read them for
experimental and modeling ideas, not for transferable setar coefficients.

For the target instrument, obtain practitioner-reviewed examples of the intended
stroke classes and document nail shape, contact region, and uncertainty where
possible. Do not ask a performer to adopt an uncomfortable or unsafe motion for
the sake of a convenient model. Their voluntary description of the gesture is
part of the evidence, and they may decline any measurement or attribution.

## A controlled comparison design

Use a symmetric synthetic surface and isotropic string first. Reverse a path
under conditions where the physical symmetry predicts an equivalent transformed
response. This catches arbitrary direction-dependent gains. Then introduce one
asymmetry at a time: nail curvature, flesh region, string-array geometry, or
bridge orientation. The resulting difference has an interpretable cause.

For friction tests, prescribe relative motion with known normal load in a lumped
fixture and verify nonpositive friction power. Test the low-velocity limit,
velocity reversal, separation, and large but admissible slip speeds. Then couple
to the string and repeat under refinement. A pass at one timestep can hide a
friction-induced numerical instability.

For a stick-state model, test loading, unloading, rotation, patch migration, and
normal-force collapse. Compare with an independent complementarity or highly
resolved reference where appropriate. Record solver residuals as well as audio;
a waveform can sound smooth while the internal state violates its constraints.

## Mapping and observability

Expose a small number of meaningful controls: path depth, path speed, nail
orientation, and contact-region preference where supported. Do not expose every
unidentified physical coefficient as an equally credible musician control.
Engineering views may expose them for experiments with clear provenance labels.

Controller mappings should be invertible enough for debugging. Given a rendered
gesture, a trace should identify which path and material profile were used and
which controller values produced them. This does not require a mathematically
invertible function; it requires reproducibility and visibility of policy.

Do not interpret a user selecting more flesh as a claim about their actual
anatomy. It is an instrument-control metaphor unless measured input supports a
more literal interpretation. Optional physiological sensing belongs in a
separate, consent-based adapter and must not be required for basic playability.

## Acceptance and next action

The work passes when friction signs and units are verified, dissipation is
accounted for, region transitions converge, symmetry tests behave as predicted,
and the controller trace explains the selected path. Acoustic validation requires
held-out real strokes and practitioner review; a plausible reverse-stroke EQ
curve is not enough.

If the model cannot distinguish geometry from material effects, reduce the
claim and acquire a more discriminating observation. The next chapter extends
the same single contact trajectory across multiple persistent strings. It must
retain these work and state conventions rather than replacing each encounter
with an unrelated note trigger.
