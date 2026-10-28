# Research and measurement volume

This volume specifies how the project earns physical and musical claims. Its
protocols are proposed work, not completed measurements. No measured reference
setar, calibrated response matrix, geometry scan, or performance corpus is supplied
by these instructions. Synthetic fixtures can verify software behavior but cannot
establish that a model represents a particular instrument.

The ambition is a convincing, deeply playable physical model. That ambition is not
a promise of exact replication or a claim of parity with Pianoteq. The research
deliverable is a chain from a question, through a controlled observation, to a
parameter or falsifiable prediction. An attractive recording without that chain
is artistic reference, not automatically calibration evidence.

## Navigation

| Decision | Chapter | Required output |
| --- | --- | --- |
| What does a source support? | [Evidence ledger](evidence-ledger.md) | Scoped claim and source records |
| Can published numbers be imported? | [Parameter audit](published-parameter-audit.md) | Admissibility and provenance decision |
| Which observations matter? | [Research questions](research-questions.md) | Falsifiable prediction matrix |
| Can parameters be inferred? | [Identifiability](parameter-identifiability.md) | Confounding and sensitivity report |
| What blocks progression? | [Reproducibility](reproducibility-and-stop-gates.md) | Evidence-based gate decision |
| How is evidence acquired? | [Measurement volume](../measurement/README.md) | Calibrated, reproducible protocols |
| How is musical context represented? | [Ethnomusicology volume](../ethnomusicology/README.md) | Player-reviewed performance specification |
| How is work assigned? | [Research prompts](../../prompts/research/README.md) | Bounded developer work requests |
| Where do data belong? | [Data contract](../../data/README.md) | Rights-aware asset organization |

## Evidence language

Use four explicit labels: **observed**, **derived**, **proposed**, and **synthetic**.
Observed means a traceable measurement or directly inspected source statement.
Derived means a calculation from named inputs with an available method. Proposed
means work or a decision not yet executed. Synthetic means an intentionally
constructed fixture whose values are not observations of a real setar. Attach
these labels to quantities rather than entire projects. A geometry model can
contain measured outer surfaces and inferred inner surfaces; calling the whole
file `measured` hides that distinction.

Source identifiers begin with `R-`. Laboratory assets use independent identifiers
such as `SPEC-`, `SESSION-`, `CAL-`, `TRIAL-`, and `FIT-`. A source identifier is not
a specimen identifier. A paper can motivate an experiment without identifying the
instrument eventually represented by the plugin.

## Evidence sequence

Secure an authorized instrument and define the claims before collecting a large
corpus. Check calibration, units, and data integrity on benign reference objects.
Acquire enough geometry and string-state data to define the first mechanical
model. Repeat a small response set after complete repositioning. Expand coverage
only after repeatability and pilot sensitivity results justify the additional work.

Fit restricted subsystems before the complete instrument. Test partial frequencies
before body equalization can conceal string errors. Test bridge admittance before
interpreting microphone data as a body-only property. Compare supported and
player-held conditions before assigning a laboratory decay to performance. Freeze
independent test material before iterative listening makes every example familiar.

Gates are evidence-based, not calendar-based. A low-amplitude mobility model may
advance while nonlinear contact remains experimental. An uncertain grain field
may permit a measured passive reduction to be studied, but prevents claims that
the reduction uniquely identifies the wood's elastic constants.

## Implementation boundary

Recordings are measurement and validation objects, not note-playback synthesis
assets. Measured parameters and physically interpretable reduced models may inform
the engine when provenance, rights, and validity limits are explicit. Do not call
a measured transfer response a first-principles calculation. Equally, a model is
not a sample player merely because its physical parameters were measured.

Every handoff needs SI units, coordinate frames, signs, uncertainty, validity
limits, and failure states. Frequencies without residues, damping conventions,
mode shapes, and excitation geometry do not constitute a bridge model. Pitch
tables without fret positions, string state, and player context do not constitute
a performance specification. Incomplete handoffs should fail visibly.
