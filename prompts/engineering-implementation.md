# Engineering implementation prompts tied to the work-order register

These are **bounded prompts for future human/AI-assisted development**, not
commands that retroactively implement the model or grant publication/asset
rights. Use the prompt associated with a `ENG-Pxx-Tyy` entry in
[engineering-tasks.json](../roadmap/engineering-tasks.json). Include the
affected [requirement IDs](../roadmap/requirements.json), current revision,
target fixture and owner-approved sources. Do not paste the entire handbook
into a tool context; give the exact relevant files/derivations and the one
falsifiable question that the work will resolve.

## Universal preflight to prepend to every task

> You are implementing one bounded component of a scientifically evidence-gated
> Iranian **setar**, primarily C++20. Start by checking git status and the
> actual existing interfaces; do not assume proposed classes are implemented.
> Read `AGENTS.md`, the selected work order `ENG-Pxx-Tyy`, its `read` list,
> dependent work orders, linked requirements, current source, and relevant
> mathematical derivations. Consult the phase-task crosswalk if the source
> phase uses a `Pxx-Tyy` heading; preserve these distinct task identities.
> State the physical question, SI units, signs,
> coordinate frame, time levels and implementation/validation status. Identify
> facts actually measured, literature priors, synthetic assumptions and
> unresolved inputs separately. Do not transform Iranian setar into tar,
> Indian sitar or generic plucked guitar by reusing their physical assumptions.
>
> Propose the smallest target files and tests; seek review where a public
> interface, scientific method or external asset/license decision is still
> open. Implement one independently verifiable behavior; preserve unrelated
> changes and make no unapproved external purchases, contacts or publication.
> Test analytic limits and adverse counterexamples before tuning for sound.
> Do not invent a tolerance or measured parameter to make a fixture pass.
> Run the full applicable headless CMake/CTest suite and documentation audits;
> ensure new runnable C++ tests are exercised in GitHub Actions on Linux,
> Windows and macOS. Record exact commands, results, graphs with physical units,
> input/asset hashes, source revision and limitations. If JUCE, licensed DAWs,
> a physical instrument or real GUI is unavailable, mark that verification
> unrun and keep a portable synthetic test route. Report what changed, what
> passed, what failed, and the next evidence gate.

## P-01: Source/measurement acquisition (ENG-P01-T01/T02, ENG-P07-T01/T02)

> Establish a specimen-specific data demand table; enumerate strings, bridge,
> wooden soundboard, bowl, joints, cavity, finger/nail contact, microphones and
> idiomatic setar gestures. For every value cite a primary trace, uncertainty,
> boundary condition, unit and permission; preserve contradictory sources
> outside accepted calibration. Design repeatable geometry, bridge FRF,
> pluck-motion and directivity experiments with calibration and holdouts.
> Build a blank validated manifest and quarantine tests, not synthetic values
> relabeled measured. Show that a negative/reciprocity-inconsistent material
> matrix is rejected. Deliver the permission-safe measurement plan, raw data
> hash scheme, unknowns and exact next acquisition method. Do not contact
> participants or alter an instrument without owner approval.

## P-02: Linear string spatial operator (ENG-P03-T01)

> Implement the string spatial mass, tension and bending operators with the
> reviewed boundary hypotheses. Before code, derive dimensions, endpoint
> closures, matrix symmetry/positivity and the weighted adjoint of any force
> injection interpolation. Add one explicit dense matrix action as an
> independent comparator to the optimized stencil. Run manufactured/analytic
> frequency and displacement tests across several resolutions, including
> smallest grids and endpoints. Deliberately perturb a boundary coefficient
> to demonstrate a test fails. Deliver source/tests, convergence CSV, energy
> derivation and supported stiffness/tension domain; no setar fidelity claim.

## P-03: Time update and power port (ENG-P03-T02/T03)

> Do not select a scheme solely because it seems fast. Derive its actual
> amplification matrix, temporal order, stability envelope and discrete
> energy for the chosen damping and boundary operators. Implement one reference
> and one independent analytic or higher-accuracy oracle. Connect two masses
> with a spring using the intended force/velocity port and validate equal-and-
> opposite discrete work; include a deliberately delayed port that should
> fail. Run temporal/spatial refinement separately. Report whether the
> specified method is appropriate for stiff contacts and whether an implicit
> variant is required; present alternatives with error-vs-cost evidence.

## P-04: Nail/flesh contact and release (ENG-P05-T01/T02/T03)

> Begin with a synthetic scalar unilateral potential and a defined performer
> trajectory, not a recorded transient or arbitrary ADSR. Derive force,
> energy, relative work, tangential dissipation and physical release
> conditions. Implement engagement/slip/release state transitions with bounded
> nonlinear iteration or explicit offline rejection. Test grazing, zero
> velocity, high stiffness, repeated recontact and deliberate nonconvergence
> against a separate potential evaluator or converged solver. Preserve raw
> penetration/force/work traces. When empirical evidence is available,
> preregister unseen players/angles; do not fit a contact law on held-out
> recordings. Return the final law's *validity range*, not a claim of perfect
> every-player nail behavior.

## P-05: Moving fret, finger, retuning (ENG-P06-T01/T02)

> Implement frets and moving left-hand contacts as constraints or compliant
> contacts on persistent string state. Write a work account for an external
> moving constraint and any tension actuator. Preserve afterlength and
> sympathetic state; do not delete a string segment at a fret. Run full and
> released constraints, sliding, same-sample controls, rapid reversals and
> contact overlap. Compare energy and position with a separate reference.
> Explicitly label convenience pitch transforms nonphysical. Deliver state
> migration/transition tests and a counterexample that an ordinary MIDI
> note-to-frequency reset would fail.

## P-06: Structural body and bridge (ENG-P08-T01/T02/T03)

> Use measured **wooden** setar geometry and admissible orthotropic material
> priors. Derive FEM weak form, material-frame transforms and boundary/joint
> assumptions; implement patch/mesh convergence before calibration. Measure
> or ingest calibrated complex multi-axis bridge mobility with forces and
> velocities in matched frames. Reduce only after verifying eigenmode tracking,
> complex FRF accuracy and passive port behavior over a withheld band and
> direction. Supply a synthetic structural fixture if real geometry is
> unavailable and name that limit. Deliver meshes, eigenmode/FRF plots,
> reduction rank-vs-cost and quarantine decisions for invalid tensors.

## P-07: String–body and acoustic coupling (ENG-P09-T01/T02, ENG-P10-T01/T02)

> State every conjugate force/velocity and pressure/volume-flow interface,
> signed axes and discrete time levels. Connect string and body through one
> joint solve (or a derived passive partition), then verify a two-mass limit,
> zero-coupling limit, total energy and unstruck-string transfer. Test an
> intentionally one-sample-lagged feedback to show why it is unacceptable
> without a separate approximation bound. Model cavity/radiation as a
> structural-acoustic interaction with registered geometry, then separate
> microphone/room observation. Produce held-out angle/position comparisons;
> do not claim directivity from one EQ-fitted on-axis recording.

## P-08: Four-string state and musical gestures (ENG-P11-T01/T02)

> Design a single-writer `InstrumentState` containing four persistent physical
> strings, contact histories and one shared body. Create a normalized gesture
> grammar independent of MIDI. Translate host commands only at a dedicated
> adapter; define ambiguous string choice and all-notes-off versus physical
> damping. Demonstrate re-striking one string while old modes are still
> active, unstruck-string sympathetic response and afterlength behavior.
> Include practitioner review of Iranian setar gestures and microtonal tuning
> without inventing consensus where performers disagree. Record state and
> event traces and prove block-partition invariance for fixed input.

## P-09: Parameter identification and holdouts (ENG-P12-T01/T02)

> List each parameter with physical units, prior source, uncertainty and
> observation sensitivity. Compute scaled Jacobians and singular values;
> include a deliberately unidentifiable synthetic parameter pair. Freeze
> training and held-out specimen/session/gesture/observation IDs *before*
> optimizer development. Fit only training data, record initial guesses,
> alternative optima and residual patterns, and evaluate untouched conditions
> with uncertainty. Do not compensate wrong nail force with arbitrary
> microphone gain or declare a non-identifiable fitted number measured.
> Deliver parameter posterior/interval, source hashes and withheld failures.

## P-10: Real-time reduction and publication (ENG-P13-T01/T02)

> Profile an accepted offline model and identify actual work costs. Compare
> candidate modal truncation, filter reduction, multirate stepping and SIMD
> on identical gestures, keeping physical port/state errors and attack
> behavior visible. Derive any resampling and bridge feedback phase effect.
> Freeze a supported numeric/capacity envelope and implement preallocated
> bounded callbacks, transactional immutable asset handoff and deferred
> destruction. Instrument allocations, locks, iterations, denormal tails and
> stress callbacks on real platform hardware; report observed maxima and
> misses rather than invented CPU percent. Exercise invalid replacement,
> saturated event queue, nonconverged contact and safe containment. Report
> reference error versus measured cost and rejected optimizations.

## P-11: JUCE/VST3 host boundary (ENG-P14-T01/T02/T03)

> Start only with an authorized/pinned JUCE source/license choice. Confirm
> headless core still builds without JUCE. Implement host callbacks as bounded
> translation to the physical engine, stable parameters and reversible
> versioned state. Test host/headless differential events, zero/irregular
> blocks, same-sample events, malformed state, repeated prepare/bypass and
> sample-rate switches. Run the official Steinberg VST3 validator on the
> compiled plugin when installed, plus approved real hosts. Preserve actual
> logs, versions, hashes and screenshots; report unrun environments as unrun.
> Do not represent the optional one-note synthetic diagnostic as finished VST.

## P-12: UI, visual reports and fidelity evaluation (P15–P17)

> Build a musician editor and engineering inspector sharing a stable physical
> control dictionary, with Persian/English terminology reviewed by practitioners.
> Separate model parameters from observation/creative effects and accessible
> graphs from complete offline evidence. Plot displacement, energy/work,
> contact force, modal dispersion, bridge FRF, microphone pressure and
> realtime cost with correct units/uncertainty. Capture actual rendered
> interface/screenshots under pinned viewports and platform metadata; do not
> retouch a wireframe into a fake screenshot. Preregister held-out acoustic,
> blind-listening and playable-gesture experiments; distinguish physical
> verification, measured prediction and listener preference. Deliver claims
> bounded to actual specimens/players/hosts and explicit failure examples.

## Reviewer adversarial prompt (all phases)

> Act as an independent numerical acoustics + C++ reviewer. Ignore the
> author's success narrative and inspect source/test implementation, physical
> port units/time levels, raw fixtures and expected formulas. Identify where
> an expected result shares the same buggy helper as production. Construct
> one counterexample involving a boundary, nonlinear release, frame rotation,
> sample delay, asset lifetime, data leakage or missing license. Explain the
> expected pass/fail, how to reproduce it, and which requirement/claim is
> invalidated. Verify actual GitHub Actions results for the named revision;
> never equate a configured workflow with a completed run. Return a prioritized
> list of falsifiable issues, not a generic sound-quality score.

## Deliverable skeleton to append after running a prompt

```text
Work order / requirements:
Source revision / dirty state:
Claim class (synthetic / numerical / measured / product):
Model equation / port and units / frames / time level:
Assumptions and unresolved input values:
Changed files and API reasons:
Independent comparator and deliberate negative case:
Test commands / platforms and actual results:
Input, output and figure hashes / raw evidence locations:
Tolerance derivation / numerical and measurement uncertainty:
CPU/latency/capacity evidence (if relevant):
Reviewer disciplines and unresolved objections:
What the result cannot yet establish:
Next admissible work order / scientific decision gate:
```

Keep this output short enough to be read in review, and attach full diagnostic
artifacts separately. A prompt cannot grant permission to publish, alter
branch protection, contact players, purchase software or choose a license;
those decisions remain with the project owner.
