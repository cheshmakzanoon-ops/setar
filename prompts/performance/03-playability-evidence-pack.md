# Work brief: sequence-level playability and fidelity evidence pack

## Prompt to give the contributor

Audit the current instrument's performance behavior and produce a reproducible
evidence pack. Read `AGENTS.md`, `docs/START_HERE.md`,
`docs/performance/01-gesture-verification.md`,
`docs/performance/05-performance-validation.md`, and the relevant string/contact
chapters. Inspect the actual source, tests, and available measurement permissions.
Do not infer implemented capability from the handbook's proposed architecture.

Your goal is to identify what repeated gestures, ringing, coupling, and left-hand
transitions actually do, then connect each claim to evidence. This is an audit
and test task, not authorization to redesign the engine, acquire private data,
contact musicians, publish recordings, or add effects to improve an audition.
Request additional authority if one of those actions becomes necessary.

## Build the capability matrix

List four persistent strings, paired-course targeting, finite nail contact,
friction/flesh transitions, compliant frets, finger damping, nonlinear tension,
body coupling, deterministic sample offsets, and MIDI/MPE mapping. For each,
record implemented behavior, numerical tests actually run, measured validation
if available, practitioner review if available, and explicit limitations.

Use separate status labels. A synthetic test can establish a numerical property
without establishing instrument fidelity. A successful Linux build does not
establish Windows or macOS execution. A configured CI workflow does not establish
that a particular run passed. A positive listening comment applies to its
conditions, not every possible gesture.

## Construct a compact sequence corpus

Create a direct-event corpus using only supported actions. Include isolated
pluck controls, repeated plucks at controlled phases, note release without a
damper, explicit damping, independent paired-course targeting, and a course sweep
if implemented. Add a lower-finger/higher-finger release sequence only if the
engine can actually represent those contacts.

Every synthetic fixture needs a manifest with physical units, provenance,
configuration revision, sample rate, event stream, observation definition, and
expected invariant. Do not choose values that look like measurements and omit
their synthetic status. Do not reuse a recorded note as the synthesis engine.

For unsupported actions, write a skipped test specification with its missing
dependency rather than fabricating an output. The pack should make absence easy
to see. A report containing only successful capabilities encourages later
overclaiming.

## Mechanical and scheduling checks

Compare continuous-state repeated plucks with an independent reference and an
intentionally reset-state control. Verify that note-off does not clear unrelated
ringing. With coupling disabled, verify slot isolation; with passive coupling
enabled, verify the total energy ledger while allowing individual strings to
gain transferred energy.

Render the canonical event stream under several block partitions and compare
event traces before audio. Test exact block-boundary offsets, same-frame actions,
channel or owner reuse, and stale configuration references. Include a capacity
failure and verify its documented outcome.

Inspect quiet-state optimizations. A dormant string must still be able to receive
bridge work under the supported model. If an optimization suppresses sympathetic
response, quantify the error and restrict the claim rather than calling it
inaudible without evidence.

## Acoustic comparison where data exist

Use only measurements whose provenance and permissions allow the analysis.
Record specimen, tuning, fret setup, excitation conditions, sensor chain, and
uncertainty. Keep raw and processed data separate. State filtering, alignment,
windowing, and normalization; preserve physical amplitude for dynamics analysis.

Choose held-out conditions before fitting the candidate model where possible.
Compare relevant observables such as partial frequencies, decay, attack timing,
contact behavior, and selected spectral features. Do not hide a failed contact
prediction inside one aggregate score dominated by steady-state pitch.

If no suitable measurements exist, produce the acquisition specification and
mark acoustic validation unavailable. Do not invent benchmark numbers, fabricate
screenshots, or convert an informal online recording into a calibrated reference.

## Practitioner-review preparation

Prepare neutral questions tied to specific behavior: intended string selection,
stroke contrast, alternating texture, stopping continuity, and controllability.
Use the verified glossary and preserve alternate terminology. Do not ask a
practitioner to certify numerical properties they cannot observe.

Separate permission for participation, recording, redistribution, attribution,
and quotation. If no practitioner session is authorized or available, provide a
review packet rather than imaginary feedback. The packet can include clearly
labeled synthetic examples and the current capability matrix.

Avoid declaring a uniform 24-tone temperament or universal technique execution.
Keep intended intervals, physical fret coordinates, predicted pitches, and
measured pitches distinct. Any mapping that uses an assisted nonphysical pitch
operation must be labeled separately from physical stopping or tension change.

## Final evidence handoff

Deliver the corpus, manifests, commands, trace and metric reports, failure cases,
and a claim ledger. For each conclusion, identify exactly which artifact
supports it and what remains outside its scope. Include a concise list of the
smallest next experiments that would distinguish unresolved explanations.

The pack is acceptable when another contributor can reproduce its supported
claims, see its missing evidence, and understand its limitations without the
original conversation. It is not required to show that the model is already
high fidelity. An honest, reproducible failure that identifies the next physical
question is a valuable completed result.
