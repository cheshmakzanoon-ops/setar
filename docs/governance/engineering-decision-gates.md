# Engineering decision gates: what is specified, what is still unknowable

This is the project’s **honesty ledger**. It prevents the length and precision
of the handbook from being mistaken for a complete measured instrument or
frozen production implementation. Entries below are *open* until named
evidence and review actually resolve them. The exact current capability is
also described in [README](../../README.md) and the
[phase register](../../roadmap/phase-register.json).

## How to close a gate

For each gate, record its chosen option, rejected alternatives, physical scope,
source or experiment, independently calculated error/convergence, relevant
requirements/work orders, reviewer roles and the commit that implemented it.
If the necessary measurement is unavailable, the engineer may test a
**synthetic hypothesis**, but must not mark a measured-fidelity decision as
closed. A decision can be reopened after changed setup, new contradictory
evidence, higher-quality observations or different target hardware.

## Open scientific/implementation decisions

| ID | Open decision | Evidence needed before freezing it | Work orders / review |
| --- | --- | --- | --- |
| G-01 | Which exact Iranian setar(s) and setup are reference targets? | Specimen identity, access/consent, setup, practitioner target and limitations | ENG-P00-T01; scientific/cultural |
| G-02 | Which published physical constants are usable? | Original source audit, SI conversion, uncertainty and tensor reciprocity; inconsistent values quarantined | ENG-P01-T01; research/materials |
| G-03 | Which data may be retained, linked or distributed? | Owner and participant permissions, raw/derivative rights, public/private partition | ENG-P01-T02/ENG-P18-T01; rights |
| G-04 | Which string discretization is the offline reference? | FDTD/modal/FEM/implicit comparison, exact boundary, independent convergence and energy | ENG-P03-T01/T02; numerical |
| G-05 | What are the endpoint motion/force time levels? | Shared discrete work proof across two-mass and string-bridge fixture | ENG-P03-T03/ENG-P09-T01; numerical |
| G-06 | How many transverse/longitudinal DOFs are essential? | Held-out amplitude-dependent modes, cross-polarization and gesture evidence | ENG-P04-T02; physics/acoustics |
| G-07 | What nail/flesh force/friction/release law is defensible? | Contact movement/force observations, held-out strokes, passivity and convergence | ENG-P05-T01/T02; performance/numerical |
| G-08 | What fret/finger contact law and geometry are valid? | Fret positions, contact compliance, movable constraints and afterlength work | ENG-P06-T01; instrument/numerical |
| G-09 | What physical energy/work transition defines retuning? | Tension actuator model or disclosed nonphysical convenience map, transition tests | ENG-P06-T02; physics/product |
| G-10 | Which wood elastic tensor and humidity state are admissible? | Material orientation, SPD reciprocal coefficients, measured uncertainty | ENG-P07-T02; materials |
| G-11 | Which body continuum and boundary conditions fit the specimen? | Measured geometry, wooden soundboard/bowl, joints, FEM mesh/FRF convergence | ENG-P08-T01/T02; structural |
| G-12 | What passive bridge multiport order/rank is sufficient? | Complex measured FRFs, held-out directions, passivity and cost | ENG-P08-T03; acoustic/numerical |
| G-13 | What coupled solver preserves joint discrete power? | Monolithic reference and independent coupled energy, phase and zero-coupling tests | ENG-P09-T01/T02; numerical |
| G-14 | What cavity/radiation model is needed for observed directivity? | Surface/cavity mesh, calibrated microphone array and held-out angles | ENG-P10-T01/T02; acoustic |
| G-15 | How are actual gestures and four physical strings mapped from MIDI? | Practitioner-defined grammar, deterministic physical controls, ambiguity policy | ENG-P11-T01/T02; practitioner/host |
| G-16 | Which parameters can be estimated independently? | Sensitivity rank, covariance, synthetic recovery and new experiments for ambiguities | ENG-P12-T01/T02; identification |
| G-17 | Which reduced/multirate engine meets accuracy and CPU constraints? | Reference-vs-production Pareto on actual stress domain and host hardware | ENG-P13-T01/T02; numerical/realtime |
| G-18 | How are prepared assets adopted and reclaimed? | Single-writer proof, off-callback destruction, thread/overflow/fault tests | ENG-P13-T02; concurrency |
| G-19 | Which JUCE/SDK rights and plugin formats are authorized? | Explicit owner distribution/license decision and pinned dependencies | ENG-P14-T01/ENG-P18-T01; owner/rights |
| G-20 | Which exact host buses, events, state and migration semantics are supported? | Stable parameter IDs, host render differences, validator and actual DAW matrix | ENG-P14-T02/T03; host |
| G-21 | Which UI/plots and real screenshots qualify the product? | Working editor, frozen capture states, data/structural regression and user review | ENG-P15-T01/T02; UX/practitioner |
| G-22 | Which platform/host/hardware configurations pass? | Completed run links, binary hashes, real callback timings and exclusions | ENG-P16-T01/T02; QA |
| G-23 | What measured acoustic error constitutes acceptable fidelity? | Preregistered holdout, instrument variation, noise/uncertainty and reviewer judgment | ENG-P17-T01; acoustic/statistics |
| G-24 | What perceptual and expressive findings support musical quality? | Blind listening/playing protocols, participant consent, effect sizes/limits | ENG-P17-T02; performer/statistics |
| G-25 | What can legally be released and maintained? | Approved source/data licensing, reproducible package, old-state migration, retention | P18/P19; owner/QA |

No clock time appears in this ledger; these are evidence conditions, not a
schedule. None is presently claimed closed for the full product. Current
synthetic C++ functionality can legitimately satisfy *parts* of P02/P03/P04,
as reflected by the phase register and actual CTests, without closing the
integrated gates above.

## Narrowed-scope alternatives when data cannot be obtained

If complete reference-specimen measurements are unavailable, publish only
synthetic physics demonstrations with clear limits. If body FRFs are available
but nail motion is not, develop and validate a *string/body* transfer model
under a prescribed external force and defer physically measured excitation.
If only microphone recordings exist, test acoustic imitation as an explicitly
different hypothesis; do not claim correct bridge, wood or contact physics.
If a real-time reduced solver fails its physical error budget, keep the offline
reference and continue research instead of hiding the error with arbitrary
post-processing.

Where a genuine setar uses a wooden soundboard, do not import Iranian tar
skin-membrane mechanics merely because the instruments have similar names.
Instrument identity precedes the numerical model.

## Final documentation-completeness rubric

The documentation may be judged **implementation-ready for a bounded task**
when that task has: (a) authoritative input source or declared synthetic
fixture, (b) equations/units/physical boundary, (c) expected source/test
location and ownership, (d) detailed procedure, (e) independent oracle,
(f) failure/counterexample, (g) chosen tolerance rationale, (h) handoff
artifacts, (i) downstream dependencies, and (j) a named reviewer/evidence
gate. The work-order register and domain volumes jointly provide that layer.

The documentation must **never** be pronounced a physically complete digital
twin merely because the rubric is filled in. Missing source measurements,
design choices and validation studies are outside the power of documentation
alone. Their absence is a tracked research gate, not a drafting defect that
can be removed with more adjectives.

See [implementation specification](../engineering/implementation-specification.md),
[machine-traceable requirements](../../roadmap/requirements.json), and
[final fidelity protocol](../validation/09-final-fidelity-acceptance.md).
