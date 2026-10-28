# Materials, grain, joints, and environmental state

## Scope and caution

The objective is to establish a mechanically admissible, traceable description
of the reference instrument at a stated environmental condition. It is not to
assign a species name to every component by appearance or to recover every
microscopic property from a few resonances. Wood, joints, finish, and contact
conditions form a coupled structure. Many useful model parameters will initially
be effective quantities with bounded uncertainty.

Read R-SETAR-PEDRAMMEHR-2018 section 3.2 together with the project's
[published-parameter audit](../research/published-parameter-audit.md). That source
does not authorize blindly copying its table into a constitutive model. Read
R-SETAR-MANSOUR-SCAVONE-2012 for the motivation to control measurement attachment
and support effects. The procedures here are proposed experiments for the
project's own specimen.

## Material identity levels

Use separate fields for maker-reported species, visual identification, laboratory
identification, and effective material class. A maker's record can be valuable
evidence without being a laboratory test. If the actual species is unknown, a
model may still use a passive orthotropic parameter family. Its name should
reflect the uncertainty rather than imply taxonomic certainty.

Document component construction: solid, laminated, ribbed, repaired, filled, or
otherwise assembled, where observations support the description. Do not infer
internal construction from an external silhouette. Record finishes and repairs
as observed layers or regions without assuming their composition or thickness.
If a coating's mechanical influence is not identifiable, represent it in an
effective surface property or uncertainty term and state the limitation.

## Witness specimens

Where the maker can provide offcuts or matched stock with permission, assign
separate witness-specimen identifiers. Record their relationship to the instrument:
same board, same batch, claimed same species, or merely similar stock. These are
different levels of representativeness. An offcut may share growth structure yet
have a different moisture history or local defects.

Measure witness dimensions, mass, grain direction, conditioning state, and
defects before testing. Use appropriate mechanical test standards or specialist
procedures when available, recording the exact method and deviations. Do not
invent a claim of standards compliance from a homemade bending rig. A research
fixture can still provide useful data if its load path, calibration, and
uncertainty are documented.

The instrument itself must not become a destructive material coupon. Cutting,
drilling, sanding, or removing the soundboard requires explicit separate
authorization and specialist review. If that authority does not exist, use
non-destructive observations and report the resulting limits.

## Grain frame acquisition

For each component or region, establish longitudinal, radial, and tangential
directions to the extent they can be observed. Photograph visible grain and growth
rings with registered coordinates. A clear surface-grain direction may constrain
the longitudinal axis while leaving the radial/tangential distinction uncertain.
Do not fill that ambiguity with an arbitrary normal vector without recording it.

Represent the grain field with proper orthonormal frames and an uncertainty model.
On a curved component, interpolate orientations in a way that preserves valid
rotations. Averaging matrix entries can create nonorthogonal frames. At joints,
retain distinct component frames rather than smoothing across a material boundary
that physically changes orientation.

Test how plausible grain deviations affect the predicted modes and port
responses. If a local grain uncertainty is negligible in the current band, report
that sensitivity result and avoid expensive overmeasurement. If it dominates,
plan a better observation or narrow the model claim. More precise geometry cannot
compensate for a completely wrong stiffness orientation.

## Elastic and inertial quantities

For an orthotropic model, document the independent normal moduli, shear moduli,
Poisson convention, density, and the derivation of reciprocal terms. Require
symmetry, positive definiteness, finite values, and acceptable conditioning. The
constitutive importer should reject incompatible values before any simulation.
Never let a solver's willingness to run substitute for an admissibility check.

Mass and geometry can constrain average density, but uncertainty in inaccessible
volume, finish, and hardware must be included. Do not fit density to total mass
and then claim each component's density was measured independently. Likewise,
static or dynamic bending tests constrain combinations of modulus and geometry;
their interpretation depends on fixture and beam/plate assumptions.

When using dynamic material tests, report the frequency and amplitude regime.
A frequency-dependent complex modulus is not interchangeable with a static
Young's modulus. If the synthesis uses a simpler effective law, show how it
approximates the observations within the required band and preserve the residual
as model discrepancy.

## Damping conventions

Choose a damping representation because it predicts observable behavior, not
because it has a convenient single knob. Modal damping ratio, loss factor,
frequency-dependent complex modulus, viscous coefficient, and decay rate have
different units and implications. A fitted exponential envelope can describe a
mode locally while failing a coupled or nonlinear transient.

Estimate damping with methods appropriate to modal overlap and signal-to-noise
ratio. A half-power bandwidth estimate requires assumptions that fail when
resonances overlap or the background is strongly sloped. A decay fit can be
biased by beating, support motion, or the noise floor. Compare more than one
method on synthetic signals and selected real trials before adopting a default.

State whether the reported damping includes support, sensor, radiation, and
player contact losses. “Material damping” should be reserved for a justified
mechanism separation. Effective body damping is a legitimate model quantity,
but its context must travel with the parameter.

## Joint characterization

Map accessible joints and interfaces: neck/body connection, bowl seams,
soundboard perimeter, bridge contact, nut contact, and repairs. Distinguish bonded
joints from unilateral frictional contacts. A bridge held by preload is not
equivalent to a glued connection. However, the exact contact behavior of the
reference specimen must be observed rather than assumed from a generic diagram.

Begin with noninvasive response comparisons and local deformation observations.
If a joint flexibility is suspected, ask which modes or transfer paths would
change under a small safe perturbation. Avoid applying arbitrary forces to an
instrument merely to reveal a joint. Use specialist-approved load limits and stop
criteria. A failed attempt to observe motion does not prove a perfectly rigid
joint; it establishes an upper bound within the measurement resolution.

In a reduced model, a joint may be represented by effective translational and
rotational stiffness and damping. Those parameters require defined relative
coordinates and power-conjugate forces. Check reciprocity and passivity where
the interface is passive. If contact opens, slips, or changes with preload, a
linear joint fit is limited to its identified state.

## Environment is part of the state

Record ambient temperature and relative humidity with calibrated or characterized
sensors near the instrument, not only a building-wide thermostat reading. Note
sensor location, response time, uncertainty, and acquisition timing. If wood
moisture content is measured, record the method and its applicability to the
species, geometry, finish, and depth. Ambient humidity is not an instantaneous
measurement of internal moisture content.

Log the instrument's relevant recent environment when available, including
transport or abrupt changes. Do not invent a conditioning history if the owner
does not know it. A nominally constant room can coexist with a specimen still
equilibrating from previous conditions. Observe stability through repeated safe
reference measurements rather than assuming a universal equilibration duration.

Do not deliberately cycle humidity or temperature beyond owner-approved safe
conditions. The research goal does not justify damaging wood, glue, finish, or
strings. If environmental sensitivity is important, a witness specimen or existing
natural variation may provide safer evidence. Any controlled conditioning study
needs specialist review and explicit authorization.

## Separate environmental sensitivity from drift

When conditions vary, record tuning, geometry references, and mechanical responses
alongside climate. A correlation between humidity and a resonance shift is not
automatically a causal material law: tuning, support, and elapsed settling can
covary. Use repeated reference conditions and controlled comparisons where safe.
Model specimen-specific sensitivity before extrapolating across instruments.

The eventual plugin may expose a designed “dryness” or “wood character” control.
That control should not be marketed as a calibrated humidity simulation unless
measurements support the mapping. An artistic control can be useful while being
honestly separated from physical environmental state.

## Handoff and acceptance

Deliver material evidence by component, grain frames with uncertainty, admissible
constitutive candidates, damping definitions, joint hypotheses, environmental
records, and sensitivity results. List all inferred or effective quantities. The
modeling team should know which values can be varied independently and which are
correlated by measurement or identification.

Pass when the chosen parameterization is physically admissible and adequate for
the scoped prediction, even if some microscopic values remain unidentified. Stop
material-specific claims when uncertainty is dominated by missing grain,
thickness, or joint information. The next experiment should target that dominant
ambiguity; collecting another familiar resonance frequency may add little.
