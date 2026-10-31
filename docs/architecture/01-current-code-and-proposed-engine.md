# Current code and proposed engine boundaries

## The implemented dependency graph

The headless build defines the C++20 library setar_reference and the in-tree alias setar::reference. The offline executable setar_render and the numerical test executable link that library. The finite-difference experiments live in setar_numerical_prototype. The playable synthetic prototype adds five static libraries (setar_instrument_numerics, setar_instrument_engine, setar_instrument_body, setar_instrument_preset, setar_instrument_performance) behind the aggregate alias setar::instrument, the executables setar_play and setar_benchmark, and one test runner per module. Python is used by offline report and audit tools, not by the numerical update. JUCE is optional and disabled by default; the prototype's adapter in plugin/ is a separate opt-in.

These statements describe inspected source and CMake configuration. They do not assert that every platform job has run, that a plugin host accepted the optional target, or that the reference reproduces a measured instrument. See [testing and CI](../engineering/testing-and-ci.md) for the evidence required before making those claims.

The public header is deliberately small:

| Actual API | Responsibility | Important exclusion |
|---|---|---|
| StringParameters and validate | Synthetic string configuration and supported-domain checks | No specimen characterization |
| ModeState | Modal state and physical diagnostic quantities | Not a transferable UI snapshot |
| PassiveResonator | One prepared underdamped scalar oscillator | Not a body multiport model |
| ModalString | Finite uncoupled string modes and point displacement observation | No bridge-force input or nail contact |

Do not introduce a parallel “current engine API” in architecture prose. These are the actual names. Any larger interface below remains a proposal until code and tests establish it.

## Actual string lifecycle

ModalString::prepare validates parameters, allocates candidate vectors, precomputes transitions and observation weights, and installs the candidate after successful construction. Invalid input leaves the previous prepared model usable. This transactional behavior is part of the current contract.

configure_pluck computes and stores a finite modal projection of a triangular displacement without changing live motion. retrigger replaces live displacements with those stored values and sets velocities to zero. pluck combines configuration and retrigger for control/offline convenience. reset silences live motion while retaining the configured excitation.

This retrigger is useful for a deterministic fixture. It is not a model of a fingernail interacting with an already vibrating string. Future excitation must not inherit the physical interpretation of a continuous contact merely because it calls retrigger at a MIDI offset.

advance moves the state one sample. displacement observes without advancing. tick observes the current state and then advances. render repeats tick over caller-owned output memory. Preserving this order is necessary for consistent timestamps and block-partition tests.

modes returns a const span into internal vector storage. Const access prevents direct mutation through that view; it does not make concurrent access safe. Preparation can replace storage, and rendering changes elements. A UI cannot retain this span as a thread-safe telemetry channel.

## Actual scalar resonator boundary

PassiveResonator exposes prepare, set_state, and apply_impulse as control/offline operations. Its impulse changes the velocity of one scalar oscillator through the declared mass. The prepared free-response update is allocation-free and noexcept at the public boundary.

The existence of this impulse operation does not imply that ModalString has an external-force port. It does not establish continuous forcing, a synchronized bridge solve, or arbitrary damped-state-space integration. Those extensions need independent work-balance and forced-response tests.

The exact sampled transition is exact only for the selected autonomous finite linear equations, up to floating-point evaluation. It must not be advertised as an exact integrator for the proposed nonlinear coupled instrument.

## Actual applications

The offline renderer parses physical configuration, renders complete arrays, checks finite state and fixed-gain headroom, and writes WAV, sample CSV, modal CSV, and metadata. Its signal is transverse displacement at one point. The fixed gain maps meters to digital amplitude; it is not a microphone calibration.

The renderer rejects known output overwrites and invalid configurations. Its file writes are not a whole-directory atomic transaction; an I/O failure can leave partial artifacts. The report layer validates content and records hashes. See [artifact contracts](../engineering/artifacts-and-reproducibility.md).

The optional JUCE application owns a single ModalString. It prepares a fixed synthetic configuration, duplicates one observation into the selected output channels, and retriggers on its button or recognized note-on. Pitch and velocity do not change physical configuration; note-off and expressive controllers are not modeled. Serialization is empty.

Its event scan is bounded and its UI trigger is an atomic boolean whose repeated requests can coalesce. These are diagnostic policies, not a production gesture queue. Stereo output is dual mono, not spatial radiation.

## Implemented prototype engine

The responsibilities proposed below now have a first concrete, synthetic realization in `setar::instrument`. It does not retire the reference above; the reference remains an independent oracle.

| Responsibility | Actual implementation | Present boundary |
|---|---|---|
| Instrument state | `Instrument` in [instrument.hpp](../../include/setar/instrument/instrument.hpp): four persistent strings, two polarizations each, one shared body bank, nail and two finger slots per string | Synthetic parameters; no cavity pressure state beyond the modal reduction |
| Coupled step | Simultaneous discrete-gradient Newton over bridge, nail, finger, fret and stick channels ([numerics.hpp](../../include/setar/instrument/numerics.hpp)) | Frictionless point contacts; bounded iterations with reported non-convergence |
| Observation | Body modal acceleration times synthetic radiation weights, two channels, fixed gain | Uncalibrated; one-way |
| Prepared asset | `InstrumentConfig` validated in `prepare()`, transactional on failure | No hashing, publication or off-thread handoff yet |
| Event adapter | [performance.hpp](../../include/setar/instrument/performance.hpp): score parser, scheduler, MIDI-like mapper | Placeholder grammar, not practitioner-reviewed |
| Application boundary | `setar_play` offline player; optional JUCE adapter in `plugin/` | Adapter not built in CI and not validated in a host |

See [the prototype chapter](../engineering/11-playable-prototype.md) for the capability register and [the solver chapter](../numerics/15-modal-contact-solver.md) for the scheme.

## Proposed engine decomposition

The full engine is proposed to separate physical state, coupled interactions, observation, and host adaptation:

| Proposed responsibility | Owns | Must not own |
|---|---|---|
| Instrument state | Persistent strings, contacts, body/cavity states | Host buffers or editor widgets |
| Coupled step | Work-consistent exchange and bounded solver scratch | File loading or parameter fitting |
| Observation | Radiation and microphone projection | Hidden changes to mechanical energy |
| Prepared asset | Immutable coefficients, ports, provenance, validity | Live mutable oscillator state |
| Event adapter | Host/controller interpretation and timestamps | An undocumented alternative physics model |
| Application boundary | Device/host lifecycle, UI, serialization | Constitutive laws duplicated from the core |

These are responsibility names, not prescribed C++ class declarations. Choose concrete interfaces only after the numerical and ownership contracts are reviewed. Avoid creating empty abstraction layers merely to make the diagram appear implemented.

## Migration from the reference

Keep the synthetic reference available as an independent executable baseline while adding new physics. A future forced resonator can be verified against analytical response before it joins a coupled string fixture. A coupled fixture can then test body loading before introducing measured geometry.

Do not mutate the existing reference until its old experiment becomes irreproducible without a deliberate versioned change. New methods may live alongside it with distinct model identifiers. Shared low-level helpers are appropriate only when they do not destroy test independence.

The first integration seam is a physical force/velocity contract, not a larger preset list. Another seam is a smooth finite-energy excitation replacing the sharp triangle’s known continuum-energy limitation. Neither is supplied by adding a GUI slider.

## Review rules

For each change, name the actual layer modified and the new evidence it produces. A header addition needs lifetime, units, preparation, failure, and test semantics. A host feature needs event and lifecycle evidence. A measured asset needs provenance and validity.

Do not freeze an installed ABI prematurely. The current CMake project supplies an in-tree library alias, not a versioned installed-package promise. Source compatibility and future binary distribution are separate decisions.

Architecture acceptance means responsibilities and interfaces are testable and accurately described. It does not close the acoustic-fidelity gate. That remains dependent on specimen measurements and held-out validation.
