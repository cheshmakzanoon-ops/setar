# Five evidence-gate handoff prompts for future C++/DSP and laboratory engineers

**Purpose:** copy exactly one bounded prompt for the next authorized engineer
or coding agent. Replace bracketed inputs with approved sources and report
missing inputs honestly. These are not grants to collect a private instrument,
invite research participants, download JUCE, choose a commercial license or
spend money without owner authorization. No deadlines are assigned.

## Gate 1 — numerical architecture: compare actual algorithms

> You are the numerical lead working on the Iranian setar physical-modeling
> project at revision `[REVISION]`. Read
> `docs/architecture/07-experimental-solver-evidence-and-choice.md`,
> `docs/architecture/05-discrete-solver-selection.md`,
> `docs/numerics/07-time-integration.md`,
> `docs/mathematics/03-energy-and-passive-ports.md` and the exact `ENG-P03`/
> `ENG-P04`/`ENG-P09`/`ENG-P13` work orders. Reproduce the existing 60-row
> C++ solver sweep and all named CTests before changing any method. The existing
> leapfrog/midpoint string prototypes and analytic modal reference are
> *synthetic, fixed-end linear* baselines, not setar measurements.
>
> Begin by writing the physical state, boundary data, mathematical operators,
> admissible parameter range, canonical SI units, time levels, discrete energy
> and port work identity for exactly one additional model feature: `[FEATURE]`.
> Construct an independent analytic/manufactured/high-precision oracle and
> failure cases. Implement a minimally complex C++20 prototype with no JUCE;
> keep preparation separate from stepping. Compare explicit FDTD, implicit
> integration and a modal benchmark on identical initial/boundary/contact
> conditions. For nonlinear contacts add a discrete-gradient or independently
> converged nonlinear oracle, record solver iteration/residual and refused
> steps. For body studies add a separate calibrated FEM or analytic patch/
> convergence fixture; do not claim an FEM implementation exists without
> actual assembled mesh calculations. For multirate coupling derive and test
> reciprocal work across internal rates and intentional one-sample lag.
>
> Sweep grid/time resolution, local contact stiffness, mode truncation,
> damping and relevant parameter uncertainty separately; report frequency
> and phase, external work, stored energy, defect, contact release, allocated
> memory and measured CPU under a precisely identified platform. Do not
> select a production solver from one voice demo or a fabricated runtime
> estimate. Output changed source, CTests (including failing negative cases),
> raw CSV, actual labelled SVG/HTML plots, exact SHA-256s and a candidate
> matrix stating advantages, counterexamples and remaining physical data.
> Update `roadmap/executable-test-coverage.json` only for actually executable
> subsets. Ask for new authority instead of inventing parameters. Stop
> when a missing measured boundary or calibration makes the next inference
> underdetermined; log that as a scientific gate, not a silent pass.

## Gate 2 — physical data: measured one-specimen campaign

> You are the measurement and metrology lead for a **specific** Iranian
> setar `[SPECIMEN_ID]`. Read
> `docs/measurement/13-acquisition-campaign-execution.md`,
> `docs/measurement/specimen-completeness-matrix.md`,
> `docs/measurement/uncertainty-and-calibration.md`,
> `docs/body/01-orthotropic-material-audit.md` and the relevant P01/P07/P08/
> P05/P10/P12 phase tasks. Confirm signed owner/performer permission, allowed
> observation types and data storage before any measurement. If not approved,
> provide a session plan and blocker; do not imitate a real dataset with
> synthetic values.
>
> Make a private copy of `data/measurement-campaign-template.json`. Capture
> all 17 required packets at the exact named coordinate frame and setup:
> calibrated geometry, wooden soundboard and interior bowl, admissible
> orthotropic materials, four string geometry/tension/density/stiffness/loss,
> movable fret geometry, complex multiaxis bridge mobility, support/contact,
> nail/flesh force and release, performer gestures, cavity/radiation, microphone
> transfer, environment and cross-string response. Preserve device clock,
> sensor calibration/phase, source hash, channel units, uncertainty/covariance,
> repetition and authorization separately from derived model fits. Avoid
> invasive measurements without explicit approval.
>
> Freeze held-out sessions before fitting; test physical admissibility and
> identifiability against independent observations. When a sensor is absent,
> mark a channel missing and propose what independent experiment could
> resolve it. Run `tools/validate_measurement_campaign.py` in normal and
> `--require-ready` modes, and preserve negative results. Submit only
> permitted redacted metadata/public documents; never expose restricted
> scans, recordings or consent identities by default. Handoff an accepted
> source ledger, uncertainty budget, raw hashes, human signoffs, admissibility
> findings and open blockers. Do not describe checksum success as validation.

## Gate 3 — implementation contracts: independently verified C++ boundaries

> You are the C++20 integration lead. Read
> `docs/engineering/10-prototype-contracts-and-test-expansion.md`,
> `docs/architecture/04-cpp20-module-contracts.md`,
> `docs/architecture/07-experimental-solver-evidence-and-choice.md`,
> `docs/architecture/02-physical-ports-and-state-ownership.md` and
> `roadmap/engineering-tasks.json`. Pick one unresolved interface:
> `[INTERFACE_AND_WORK_ORDER]`. List exact consumers, source ownership,
> physical units, coordinate transforms, fixed time level, energy conjugacy,
> initialization/reconfiguration, failure and concurrency constraints.
>
> Implement a minimal portable C++20 prototype and a separate independent
> oracle; do not freeze the public ABI from documentation alone. Require
> preparation to validate and publish complete immutable state atomically;
> reject invalid input transactionally. Model a four-persistent-string state
> and symmetric body feedback only when real equations and a port energy
> check exist; MIDI note events must not allocate an independent synthesizer
> oscillator for convenience. For audio-thread code prohibit allocation,
> blocking, exceptions, external I/O and unbounded iteration by *proof and
> executable instrumentation*, not by naming a function `noexcept`.
>
> Add failure injection, sample-block partition, state/retrigger/retune and
> cross-OS tests. Build Linux, Windows, macOS. Distinguish an executable
> scalar fixture from a validated 3D physical port; add adapter prototypes
> only with approved third-party licensing. Submit API rationale, exact
> compiler and test results, a diagram of ownership and all rejected
> alternatives. Leave the product interface proposed until independently
> validated integration results justify its final signature.

## Gate 4 — executable acceptance: close the remaining 70 unmapped cases

> You are the verification lead. Begin with
> `roadmap/requirements.json`, `roadmap/executable-test-coverage.json`,
> `docs/engineering/verification-matrix.md`,
> `docs/engineering/10-prototype-contracts-and-test-expansion.md` and
> `tools/audit_executable_coverage.py`. Do not convert the 79 planned
> identifiers into 79 empty or self-confirming CTests. For every case in
> `[SELECTED_REQUIREMENTS]`, state each claim, failure domain, independently
> computed expected result and exact physical/algorithmic tolerance.
>
> Start with failure-injection and analytic limiting cases, then add positive
> numerical experiments and measured comparisons where permitted. Use C++20
> CTests for numerical invariants and structured Python only for evidence/
> analysis. Register exact stable test IDs with labels and timeouts in CMake.
> Use sanitizer/race/allocator probes for state ownership and callback
> claims. On every OS inspect real compiler/test output and store hashes of
> raw outputs and plots. Do not count unexecuted/skipped host cells as passed.
>
> Update the partial coverage map with the verified subset and the remaining
> independent physical/host work; promote full requirement status only after
> all clauses and scientific/data gates pass. Run the handbook and register
> audits, mutation tests, three OS CTest matrix and generated report tests.
> Handoff the exact delta in `N/79` partially implemented names and a
> reviewer-readable map of which complete claims remain unproven.

## Gate 5 — actual product qualification, without invented evidence

> You are the JUCE/VST3 qualification and perceptual-validation lead. Read
> `docs/plugin/07-product-qualification-evidence-gates.md`,
> `docs/plugin/06-host-qualification-playbook.md`,
> `docs/ux/05-live-engineering-graphs-and-screenshot-contract.md`,
> `docs/validation/09-final-fidelity-acceptance.md` and
> `data/product-qualification-template.json`. Ask the owner to identify
> approved JUCE/Steinberg dependencies, actual VST3 build, DAW versions,
> OS/hardware coverage and participant/recording permissions. The current
> repository's optional JUCE diagnostic is **not** the complete product.
>
> Once a real plugin exists, run the official Steinberg validator and a
> separate real DAW host on Linux, Windows and macOS as actually supported;
> test exact VST3 lifecycle, events, state migrations, buses, irregular
> blocks, multiple instances and offline renders. Record exit codes,
> plugin/source hashes, SDK/host versions and raw console logs. Measure
> actual worst and p99 callback duration across a declared stress envelope,
> memory/allocations/locks, event drop and deadline misses. Capture PNG
> screenshots from the **live** host/editor plus visual review; do not
> substitute SVG mockups or generated charts. Pre-register the held-out
> perceptual and performer study, consent, sample-size rationale, blinds,
> randomization, analysis and acceptance criterion before enrollment.
>
> Fill private `data/product-qualification-template.json` copy with actual
> evidence records and hashes; run the checker both normally and using
> `--require-qualified`. An unavailable DAW, missing participant or unresolved
> legal permission is a blocked gate; return an explicit request for human
> authorization. Deliver per-OS/host/binary qualification, performance traces,
> permitted genuine screenshots, study evidence, failures and a claim-specific
> release recommendation. Never claim Pianoteq equivalence or 1:1 physical
> identity from passing software tests alone.

## Common evidence handoff checklist

- Exact commit and named work order; proposed claim and excluded scope.
- What changed, including file paths and architecture consequences.
- SI parameters, calibration/provenance references, uncertainty, data rights.
- Independent reference derivation, test input hashes, negative results.
- Commands actually run locally and in each **completed** OS workflow job.
- Raw CSV/binary traces, CTest XML, labelled real visualizations, source hash.
- Remaining numerical, physical, realtime, host, license and human gates.
- Decision requested of the owner, if the next step requires a real
  instrument, funds, study participants, third-party dependency or consent.

**Success is a falsifiable evidence packet.** It is entirely acceptable to
report "underdetermined: acquire bridge phase" or "not host-tested"; it is
not acceptable to make up a measurement, a compatibility pass, a CPU result
or a screenshot to fill a checkbox.
