# Physical string specification and parameter provenance

## Measure the object before naming its coefficients

A string model needs more than a note name. Its speaking length, mass per length,
bending behavior, tension, terminations, surface condition, and loss mechanisms
jointly determine the response. Several combinations can produce nearly the same
fundamental frequency. Therefore, fitting the fundamental alone cannot identify
the string. This chapter defines a measurement and inference sequence that keeps
those ambiguities visible.

Begin with an instrument and string inventory approved by the owner or maker.
Assign stable IDs to the instrument, each installed string, each replacement
specimen, and every measurement session. Record whether a string is plain,
composite, or wound if known, but do not infer its alloy from colour. A shop label
is evidence about the supplied product, not a metallurgical analysis. If strings
have been mixed or replaced, preserve that uncertainty.

Photograph the instrument with permission and include a scale only when the scale
lies in the relevant plane. A perspective photograph is not a reliable diameter
measurement. Record speaking contact points rather than assuming the visible
edge of the nut or bridge is the vibration endpoint. The effective endpoint may
shift with contact geometry, load, and frequency. Keep geometric length and
acoustically inferred effective length as different fields.

## A parameter record that can survive revision

Every scalar record should contain a quantity name, SI value, uncertainty or
interval, provenance class, object ID, method ID, observation date if known, and
validity conditions. An unknown is represented explicitly rather than by zero.
Zero bending rigidity is a meaningful ideal-string experiment; it must not also
mean that rigidity was never measured.

For example, a synthetic fixture may declare `mu = 0.0004 kg/m`, with the status
`synthetic`, a rationale such as numerical conditioning, and no claim that the
value describes any real setar string. This is an illustrative value, not a
recommended string specification. An input validator can accept it for a test
while refusing to label the resulting preset as measured. A derived tension
references the exact length, frequency, density, and model revision used to
calculate it.

Store dimensional information in field names or strong quantity types. A GUI may
display millimetres, but the engine boundary receives metres. A cents offset may
be useful musical metadata; it is not a substitute for a physical frequency
ratio or a fret position. Audit conversions at serialization boundaries, because
unit mistakes often survive visually plausible plots.

## Length, diameter, and mass per length

Measure the unloaded or lightly loaded specimen length under a documented
condition. If weighing a spare specimen is permitted, measure a sufficiently
long segment that the instrument's balance resolution is useful. Remove or
separately account for loops, knots, windings, packaging, and end fittings.
Report the measured mass divided by measured length with propagated uncertainty.
Do not cut an owner's installed string without explicit permission.

Diameter measurements require a controlled measuring force and several
locations. A micrometer can flatten soft material or bridge over a winding.
Report the instrument resolution, force method, number and positions of samples,
and whether the cross-section appears circular. A diameter spread can indicate
manufacturing variation, wear, corrosion, or measurement problems. None should
be silently converted into a single perfect cylinder.

For a homogeneous circular string, a geometric estimate uses

\[
A=\frac{\pi d^2}{4},\qquad I=\frac{\pi d^4}{64},\qquad \mu=\rho A.
\]

These relations are model assumptions, not proofs that a supplied string is
homogeneous. The fourth-power dependence of `I` makes a small diameter error
important for bending rigidity. For wound or composite strings, a simple
outer-diameter substitution can be badly misleading. Prefer a measured effective
dispersion fit, together with a clear statement of its frequency and tension
range, until a more detailed construction model is justified.

## Tension and the inverse-frequency trap

For an ideal flexible string with fixed endpoints,

\[
f_1=\frac{1}{2L}\sqrt{\frac{T}{\mu}}.
\]

Using this equation to infer tension is reasonable only within its assumptions.
It does not independently verify `mu` or `L`. If those inputs were chosen to make
the frequency fit, the resulting tension is not a measurement. Report it as
derived and retain the dependency graph.

For a uniform stiff string with simply supported endpoints, the modal relation
is

\[
\omega_n^2=\frac{T}{\mu}k_n^2+\frac{EI}{\mu}k_n^4,
\qquad k_n=\frac{n\pi}{L}.
\]

Thus a candidate tension from a target fundamental is

\[
T=\mu\left(\frac{\omega_1}{k_1}\right)^2-EI k_1^2.
\]

A negative result is not a parameter to clamp to a small positive number. It
signals incompatible inputs, invalid assumptions, or a target outside the model's
feasible region. Real contact conditions require their own frequency calculation.
A compliant termination can make the simply supported inverse formula an
inadequate estimator even when every input is positive.

Direct tension measurement, when safe and authorized, gives stronger separation
of parameters but may disturb the installed condition. Document the fixture's
alignment, calibration, added compliance, and whether the string is the same
specimen later recorded. The goal is not to use the most elaborate equipment; it
is to know what the equipment actually measures.

## Estimating bending rigidity and losses

Measure multiple partial frequencies under low-amplitude excitation where
nonlinear shifts and fret collisions are minimized. A fixture with well-defined
terminations can help identify string properties separately from body effects.
Fit dispersion jointly with uncertainty in effective length and tension rather
than treating every residual as bending stiffness. Plot residuals by mode number.
A curved residual pattern or localized outlier can reveal missing termination
physics or mistaken peak association.

Do not estimate frequency-dependent damping from a single broadband decay slope.
Extract individual partial envelopes where they are sufficiently isolated, state
the fitting interval, and preserve confidence limits. Beating between close
components can mimic non-exponential decay. Body coupling can produce different
decay behavior near body resonances. A fitted string-loss law should not absorb
all of that behavior if the body is later modeled explicitly.

Mansour, Woodhouse, and Scavone's [2016 plucked-string
paper](https://doi.org/10.3813/AAA.919021), especially its model-development and
experimental-evaluation sections, is a primary example of combining measured
string and body behavior in a time-domain model. Its validation concerns its
tested cello system. It does not supply setar parameters or establish that the
same loss fit is appropriate for metal strings on a setar.

## Temperature, installation, and aging

Record environmental conditions when they are available and relevant. If they
were not measured, write that they are unknown. Do not invent room temperature
because a formula expects a number. Record time since installation, tuning
history, visible wear, and whether the string was recently stretched or played.
These factors are candidate explanatory variables, not excuses to discard an
inconvenient measurement.

Repeat a small subset of observations after a controlled retune or reinstallation
when permission and resources allow. If the inferred parameter changes, decide
whether the string changed, the contact changed, or the estimator is sensitive.
A model can legitimately use condition-specific parameter sets. Averaging
incompatible conditions into a single preset may be less honest and less useful.

## Sensitivity before exhaustive acquisition

Use synthetic perturbations to determine which measurements matter most for the
current claim. Vary `mu`, `T`, `EI`, length, and loss parameters independently
within declared uncertainty, then vary physically linked combinations. A larger
diameter changes both mass and rigidity for a homogeneous string; treating those
as independent is useful for identifiability analysis but not a manufacturing
scenario.

Report sensitivity in the output domain: partial-frequency changes, decay changes,
bridge-force changes, and perceptual features selected in advance. A parameter
that barely changes the current output may not merit an elaborate acquisition
campaign. Conversely, a small geometric uncertainty near a contact threshold can
dominate attack behavior even when its effect on fundamental frequency is tiny.

Use this analysis to define tolerances, not to backfill desired conclusions.
If every plausible parameter set gives nearly identical audio, the experiment
cannot discriminate them. If several combinations fit equally well, preserve an
ensemble or interval rather than publishing one falsely precise estimate.

## Acquisition procedure and deliverables

Use the shared [strings, frets, and setup-state measurement
protocol](../measurement/strings-frets-and-setup.md) for acquisition records and
the [measurement data contract](../measurement/data-contracts-and-provenance.md)
for identifiers and provenance. This chapter specifies how those observations
enter string-model parameters; it does not define a competing raw-data format.

The prerequisites are permission to measure, calibrated or characterized tools,
an inventory, and a written endpoint definition. First collect geometric and
specimen observations. Then acquire low-amplitude string-only or minimally
coupled responses where feasible. Next acquire installed responses with documented
body and contact conditions. Finally fit parameters with the numerical model
fixed, retain residuals, and perform a withheld-condition prediction.

Outputs include raw observations, calibration records, parameter records,
analysis scripts, fitted alternatives, uncertainty estimates, and a concise
claim ledger. Keep recordings as reference data, subject to their permissions;
they are not the synthesis engine. The physical model generates sound through
state evolution and radiation, not by replaying the measured note.

Acceptance requires traceability from every shipped physical parameter to a
measurement, derivation, literature record, or openly synthetic choice. It also
requires a negative test: deliberately swap a unit or supply an impossible
tension and verify that validation fails informatively. A fit passing its own
training data is insufficient; at least one withheld amplitude, tension,
termination, or note condition must be predicted with stated error.

## Failure handling and next action

Common failures include nominal diameter mistaken for measured diameter,
manufacturer material names mistaken for measured elasticity, installed decay
mistaken for intrinsic loss, and an uncertainty interval suppressed because the
preset format accepts only one number. Correct the data model before continuing.
Do not compensate with undocumented EQ, pitch correction, or a noise layer.

Once the inventory and provenance records are coherent, implement the linear
loss-and-dispersion experiments in the next chapter. If measurements are not yet
available, run the same procedure on a clearly synthetic fixture and label its
conclusions as numerical. The procedure remains valuable without pretending that
a synthetic fixture is a measured instrument.
