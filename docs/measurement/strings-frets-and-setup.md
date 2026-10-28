# Strings, frets, and setup-state measurement

## Scope

The synthesis model needs a physical state, not just note names. Each string has
material, cross-section, linear density, bending behavior, speaking length,
afterlengths, tension, contacts, and loss. Each fret has position, shape, compliance,
and a relationship to the player's finger. Setup changes can alter several of
these quantities simultaneously. This chapter defines how to keep those effects
separate.

The external performance sources are indexed as R-SETAR-TABASSIAN and
R-FRETTING-POORHAYDARI-2023. They justify treating tuning and fret layout as
context-dependent, not a single universal equal-tempered map. The acquisition
procedures below are original project proposals. No string gauges, exact fret
positions, or measured tensions are asserted for an actual specimen here.

## String identity and installation record

Give every installed string an identifier independent of its nominal role. Record
manufacturer and product when known, claimed material, measured construction,
installation state, visible corrosion or wear, winding if present, and source of
each assertion. A package description is not an alloy analysis. Appearance alone
does not establish material composition. If composition is unknown, report a
bounded effective physical model rather than inventing a grade.

Photograph the path through peg, nut, bridge, and holder. Record contact edges,
wrapping, local bends, and any contact with neighboring strings. Distinguish the
speaking segment from afterlength segments and note which segments can vibrate.
The string's total installed length is not the same as its speaking length.

Do not remove a string or cut a sample without authorization. Where approved,
retain an unused piece from the same stock for mass-per-length and mechanical
tests. Link the stock sample to the installed string with uncertainty about batch
variation. A witness sample is more informative than a generic catalog value, but
is still not a measurement of every point on the installed string.

## Diameter and linear density

For an accessible uniform string, measure diameter at multiple positions and
orientations using a method whose contact force does not flatten or damage it.
Record resolution, calibration, repeatability, and whether the string is under
tension. For a noncircular or wound construction, a single diameter is an
incomplete description. Store the observed geometry or an effective cross-section
model with its limits.

Where a witness length is available, estimate linear density from mass and length:

\[
\mu=m/\ell.
\]

Measure the length under a defined state and account for end effects, packaging
residue, and balance resolution. Avoid using a very short piece whose mass is
near the balance's uncertainty. Repeated readings of the same piece estimate
measurement repeatability; several pieces estimate stock variability. Keep those
variance components distinct.

The circular-solid relation `mu = rho*pi*d^2/4` is a consistency check only when
its assumptions apply. Disagreement can indicate diameter error, unknown alloy,
coating, nonuniformity, or construction mismatch. Do not force the measured mass
to agree by choosing a convenient density without disclosing the inference.

## Speaking length and termination

Define the actual contact locations under the relevant preload, not merely the
distance between nominal component centers. A rounded nut or bridge can create a
finite contact region and an effective termination that differs from a simple
geometric point. Record the chosen geometric reference and uncertainty. If an
effective acoustic length is fitted, label it separately from measured length.

Measure string height and clearance at locations needed for the contact model.
Use safe noncontact or very low-force methods and account for perspective if
photogrammetry is used. A ruler in an oblique photograph does not provide a
uniform scale unless the geometry is corrected. Store camera calibration or
restrict the claim to what the image can support.

Check whether bridge motion, nut compliance, and afterlength resonance are likely
to matter in the current band. A fixed-end test can establish a numerical baseline
but should not be mistaken for the actual termination. Preserve the distinction
between a measured geometric end and a fitted dynamic boundary.

## Tension inference and its limits

For an ideal flexible string with fixed ends, the fundamental relation gives

\[
T\approx 4\mu L^2 f_1^2.
\]

This is a derived estimate under stated assumptions, not a direct tension
measurement. Bending stiffness, compliant boundaries, amplitude-dependent tension,
and uncertain pitch estimation can bias it. Use measured `mu`, `L`, and a defined
low-amplitude frequency estimate, then propagate their covariance. If a direct
tension measurement would require intrusive hardware, assess its disturbance and
authorization before adding it.

Keep the tension estimate and target tuning as distinct fields. Retuning changes
preload on the body and bridge, not just the string frequency. When comparing
body responses across tunings, record that altered preload. If the experiment
needs the same preload with suppressed string vibration, temporary damping must
be controlled for its own mechanical loading.

## Fret geometry versus musical target

Give tied frets stable identifiers and measure their positions along the actual
string path or a defined neck reference. Record width, height, cross-section,
number of wraps when observable, material claim, wear, and mobility. A fret's
center is not necessarily the sounding contact location. Store both the measured
profile and the model's contact convention.

For a simple ideal string, a target ratio `r` implies a remaining speaking length
`L/r`; the distance from the nut is `x=L(1-1/r)`. In cents `c`,

\[
r=2^{c/1200},\qquad x=L(1-2^{-c/1200}).
\]

This is a geometric design relation, not a complete prediction of played pitch.
Pressing the string changes its path length and tension, contact geometry may be
finite, and finger placement can alter damping and pitch. Use the relation to
check gross consistency, then measure actual pitch under documented playing
conditions. Do not move a player's frets to match the equation merely because
the ideal formula disagrees.

A local position sensitivity under the same ideal assumptions is

\[
\frac{\partial c}{\partial x}=
\frac{1200}{\ln 2}\frac{1}{L-x}.
\]

Thus the same position uncertainty has a larger cents effect as the remaining
length decreases. This supports position-dependent uncertainty reporting and
careful high-fret measurements. It does not prove that high notes are inherently
less musically controlled; it describes one geometric contribution.

## Pitch measurement protocol

Record raw audio or an appropriate motion signal, not only a tuner's displayed
number. Store the pitch estimator, window, confidence, harmonic selection,
analysis interval, and handling of beating or inharmonicity. A plucked note can
have an evolving fundamental estimate. Define whether the reported pitch refers
to attack, early decay, late decay, or a robust trajectory summary.

Measure repeated notes with the finger maintained, then with the finger lifted
and replaced. These answer different repeatability questions. Also repeat after
retuning or remounting when those operations occur in the intended workflow.
Treat uncontrolled string ringing as either an intentional performance condition
or a confound to suppress, and document the choice.

Use a frequency-reference check through the actual acquisition chain to detect
sample-rate and estimator bias. A nominal sample rate in a file header is not an
independent measurement of clock accuracy. For comparison across devices, account
for their time bases before interpreting small interval differences.

## Setup matrix

Construct a compact matrix that varies one mechanism at a time where feasible:
open versus fretted; low versus higher amplitude; other strings muted versus
ringing; selected alternate tunings; finger placement within a defined region;
and relevant support conditions. Then include a smaller set of natural combined
gestures to test whether conclusions survive realistic interaction.

Randomize or counterbalance condition order to avoid confusing gradual drift
with a treatment effect. Record tuning before and after blocks. If drift exceeds
the predefined state tolerance, split the block, repeat the reference condition,
or exclude it according to the declared rule. Never retune silently in the middle
of a nominally unchanged configuration.

## Outputs and gate

Deliver a per-string physical record, fret geometry table, intended and observed
tuning records, contact conventions, setup photographs, and uncertainty budget.
Each field has a source kind: observed, manufacturer claim, literature prior,
derived estimate, fitted effective value, or unknown. This prevents an inferred
tension from being mistaken for a load-cell measurement.

Pass when the model can reconstruct the relevant string path and distinguish
target pitch from actual played pitch. Stop a calibrated-contact claim if fret
position, clearance, or finger state is unobserved at the required resolution.
Continue with a restricted open-string experiment if appropriate. The next action
should reduce the dominant uncertainty, not add more nominal note labels to an
already ambiguous setup.

