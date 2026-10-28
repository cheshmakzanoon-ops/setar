# Fret buzz, intermittent contact, and distinguishing physics from chatter

## A bright transient is not automatically a correct collision

Intermittent contact can generate rich high-frequency content. Numerical chatter,
aliasing, and unstable penalty forces can also generate bright or noisy sound.
The ear alone cannot distinguish the mechanisms reliably enough for verification.
This chapter defines tests that separate a modeled string–obstacle interaction
from artifacts that happen to sound instrument-like.

The target remains Iranian setar. Do not import a jawari bridge model from the
Indian sitar because both instruments can exhibit complex attacks. The relevant
obstacles here are the measured or synthetic frets, neck, finger surfaces, and
other explicitly identified parts of the chosen setar configuration. Any new
contact surface needs geometric and empirical justification.

Prerequisites are a verified unilateral contact reference, full-string geometry,
and an observation chain with known bandwidth. Inputs are obstacle profiles,
contact laws, initial or driven string states, and refinement settings. Outputs
are contact maps, forces, penetration, energy/work residuals, spectra, and
convergence reports. Audio is one output among several, not the sole criterion.

## Contact geometry and clearance

Record clearance between the equilibrium string and each potential obstacle.
Small clearance errors can change whether a collision occurs at all. A model
fitted to a buzzing recording may be highly sensitive to action height, fret
profile, or static tension. Preserve those uncertainties instead of reporting a
precise contact stiffness that compensates for unknown geometry.

A one-dimensional obstacle profile along `x` is useful for a string moving in
one normal direction. In a two-axis model, the string can move laterally relative
to a finite fret or finger. Decide whether the obstacle extends across the whole
transverse region or has finite support. A scalar height profile that assumes
infinite lateral extent can create contacts that the actual geometry would miss.

Prepare candidate obstacles outside the audio callback. The broad-phase search
can use conservative bounds, but those bounds must not omit a physically
reachable contact. Test the maximum declared motion and configuration changes.
An efficient search that silently misses collisions is not a valid reduction.

## Compliant contact and rigid limits

A power-law potential can represent deformability or serve as a penalty
approximation to a rigid obstacle. These interpretations are different. In the
first, penetration represents physical compression under an effective model. In
the second, it is approximation error that should shrink under a controlled
stiffness sequence.

Do not choose an enormous penalty coefficient and assume the problem is solved.
Higher stiffness can require smaller timesteps for accuracy, more difficult
nonlinear solves, and a larger represented bandwidth. A stable implicit solver
can still smear impact timing or misrepresent a stiff transient at a coarse
timestep. Report contact-time and force convergence, not just absence of NaNs.

Bilbao, Torin, and Chatziioannou's [2015 collision
framework](https://doi.org/10.3813/AAA.918813) is a primary reading for potential-
based treatment and numerical energy arguments. Bilbao and Torin's [articulated
string/fretboard
work](https://www.research.ed.ac.uk/en/publications/numerical-modeling-and-sound-synthesis-for-articulated-stringfret/)
is directly relevant to combined plucking, stopping, and intermittent contact.
The project must still derive and test its own selected discretization.

## Chatter has diagnostic signatures

Numerical chatter may change drastically with timestep, contact tolerance, or
iteration cap. It may occur at a grid-related frequency, alternate contact state
every update, or inject unexplained energy. Physical repeated contact can also
be rapid, so no single signature is decisive. Use a combination of refinement,
energy, force continuity where expected, and independent reference comparison.

Plot signed gap, normal force, relative velocity, and solver residual around a
representative event. A force that remains attractive after separation, a
negative dissipated-energy increment under a supposedly passive law, or a
residual plateau at the iteration cap points to an implementation problem.
Do not hide these traces behind an output limiter.

Event-log hysteresis may be useful to summarize contact episodes, but the physical
solver should not use an arbitrary debounce interval without a modeled reason.
A debounce can suppress real recontact and alter energy. If regularization is
necessary, state its physical or numerical role and test sensitivity to its scale.

## Bandwidth and aliasing

Impacts can excite frequencies above the output Nyquist limit. A model with
bounded energy can still produce aliased audio if those components fold into the
output band. Energy stability and spectral accuracy are separate requirements.
The output observation and resampling path need explicit anti-alias treatment.

An oversampled reference can help quantify aliasing, but oversampling alone is
not proof of convergence. Increase both temporal and spatial resolution where
needed, maintain the same physical contact width and material law, and compare
the band-limited output after a documented filter. Otherwise changing the mesh
also changes the physical interaction being compared.

Softening a contact to reduce high-frequency energy changes the model. That may
be a justified effective compliance, but it should not be presented as a purely
numerical anti-alias filter. Similarly, filtering the final audio can remove
out-of-band artifacts while leaving wrong internal work and force. Verify both
mechanics and observation.

## Contact topology changes

A moving string may contact one fret, then another, or span a region with
multiple simultaneous contacts. The active set can change rapidly. A solver
must handle those changes without depending on incidental iteration order.
For small fixtures, compare all plausible active sets or use an independent
constraint formulation to check the result.

If a reduced model permits only one fret contact at a time, define the selection
rule and identify the regime in which it is valid. A maximum-displacement rule
may choose a different fret than a minimum-gap rule. Neither is automatically
equivalent to the coupled mechanics. The error must be measured in aggressive
gestures, where multiple contacts are most likely.

When capacity is exceeded, do not drop an arbitrary contact and continue while
claiming the same physical model. Reject the prepared configuration, use a
validated bounded reduction, or enter a documented safe failure mode. Record the
event so automated tests and users can distinguish it from ordinary buzz.

## Experimental discrimination

Use a controlled synthetic obstacle first. Vary clearance while holding the
initial work fixed and identify the onset of contact. Then vary amplitude while
holding geometry fixed. The resulting contact/no-contact boundary should converge
under refinement. This is more informative than listening to one aggressively
plucked example.

For an instrument experiment, document action, fret condition, string tuning,
pluck location and direction, and whether the finger is stopping the string.
Acquire repeated examples near the onset of buzz if the performer is willing.
Do not request modifications that risk damaging the instrument; geometry changes
belong with the owner or qualified maker's approval.

Compare a contact-enabled model with a contact-disabled ablation using the same
string and body parameters. If a noise layer is needed only to hide a discrepancy,
report that as a failed mechanistic explanation. A separate artistic effect can
remain available, but it should not contaminate the physical-validation dataset.

## Evidence reports that expose failure

A useful report includes the geometry manifest, parameter provenance, contact
law, numerical scheme, refinement levels, work ledger, and a set of short
representative traces. Show a contact-free case, a single-contact case, and a
repeated-contact case. Include at least one rejected or failed configuration and
its diagnostic behavior.

Report force peaks cautiously. They can be highly sensitive to compliance,
bandwidth, and sensor response. An integrated impulse or band-limited force may
be better identified than an instantaneous maximum. State which quantity the
measurement supports. Do not compare an unfiltered numerical peak with a
band-limited sensor peak as though they were identical observables.

For listening evidence, keep level-matched and physically scaled versions
separate. A louder attack can be perceived as brighter. Preserve individual
practitioner judgments and confidence, and ask about specific qualities rather
than only whether the sound is authentic.

## Exit criteria and handoff

Acceptance requires contact onset and release convergence, bounded and explained
penetration, correct energy accounting, independent reference agreement, and an
aliasing assessment over the intended output band. It also requires a clear
distinction among physical buzz, numerical failure, and optional artistic noise.

The full contact volume can then hand a bounded, documented mechanical interface
to the performance interpreter. If any of those criteria remain unmet, the
interpreter may still be developed against synthetic fixtures, but it must not
present the unfinished interaction as validated setar behavior. The next stage
is to test whether meaningful performer commands produce these verified physical
actions consistently and expressively.
