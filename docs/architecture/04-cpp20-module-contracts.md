# Proposed C++20 module contracts: types, lifetime and exact boundaries

**Status: proposed, unimplemented beyond the existing synthetic reference.**
The declarations here are a reviewable target shape, not a header guaranteed to
compile unchanged. Freeze a public signature only after the associated analytical
fixtures pass. In particular, the port response shape and causal ordering cannot
be chosen independently from [the discrete coupling scheme](05-discrete-solver-selection.md).

## Design invariants that are already binding

1. The physical core is plain C++20; a headless build sees no JUCE type,
   GUI event, DAW buffer, file dialog, OS window, or plugin-format header.
2. Every physical boundary specifies **SI units, coordinate frame, orientation,
   time level and ownership**. `double` alone conveys none of these.
3. At a mechanical interface, force and velocity are conjugates. A force map
   and motion interpolation have an adjoint relation under the same discrete
   inner product. A microphone output is *not* a mechanical power port.
4. Preparation is fallible and non-realtime. Once accepted, a prepared asset is
   immutable; one audio-processing owner writes its live state. UI/loader and
   analytics may see snapshots and immutable asset metadata only.
5. A musical note event is **not** a new physical string. The target setar model
   retains four string histories and coupled structural state. Physical
   contact events refer to a string and location, and may overlap.
6. The audio callback has fixed-capacity inputs/outputs, bounded work, a
   nonthrowing result and a documented safe-containment response. A containment
   event is logged as numerical failure, not accepted physics.
7. Observation is distinguishable from physics. Microphone move, audio gain
   and headphone processing do not silently alter stored mechanical energy.

## Compile-time quantity vocabulary

Begin with a small typed boundary vocabulary. Avoid passing raw doubles across
module boundaries until an explicit conversion has validated the input. A
strong-type wrapper is most useful at public interfaces, even if inner vector
loops use efficiently packed scalar arrays. The following is illustrative C++20:

```cpp
namespace setar::physics {
struct Seconds       { double value; };
struct Hertz         { double value; };
struct Metres        { double value; };
struct MetresPerSec  { double value; };
struct Newtons       { double value; };
struct Pascal        { double value; };
struct CubicMetresPerSec { double value; };
struct Joules        { double value; };
struct NewtonMetres  { double value; };

struct Vec3 { double x, y, z; }; // components only; frame is attached separately
struct ReferenceFrame {
    // Orientation and origin are immutable, validated during preparation.
    Vec3 origin_m;
    double world_from_local_row_major[9]; // orthonormal, right-handed
};
struct MechanicalPortDefinition {
    std::uint32_t location_id;          // stable within model-asset version
    ReferenceFrame frame;
    Vec3 positive_force_axis_local;     // unit length; direction is explicit
    Vec3 positive_velocity_axis_local;  // matched conjugate projection
};
} // namespace setar::physics
```

Do not multiply two wrappers merely because their underlying type is `double`.
Compile-time dimensions, runtime finite/domain checks, and documented units
must complement each other. `force × velocity` gives watts; multiply by the
appropriate duration to obtain joules. `pressure × volume velocity` likewise
gives watts; `pressure × point displacement` does **not**. Moment/rotation is
another conjugate pair. Coordinates in source scans need a named transform,
including calibration and uncertainty, before entering the engine.

### Physical parameter and provenance shape

For each parameter distinguish an inferred bound, measured observation,
literature prior, synthetic fixture and unsupported unknown. For example:

```cpp
enum class EvidenceClass : std::uint8_t {
    Synthetic, LiteraturePrior, Measured, Inferred, Quarantined
};
struct ParameterRecord {
    double value_si;
    double standard_uncertainty_si;  // or explicit interval/distribution identifier
    std::uint32_t unit_id;
    std::uint32_t provenance_id;
    EvidenceClass evidence_class;
};
```

These are *metadata*; `ParameterRecord` is not a realtime coefficient. The
preparer resolves correlated input distributions and converts admissible
physical parameters to a compact immutable numeric realization. The runtime
must never accept `Quarantined` as a measured value or use a default modulus
to conceal an unresolved physical contradiction.

## Four layers and their APIs

### 1. Canonical physical assets (offline, immutable)

Proposed `src/assets` responsibilities:

- **Specimen model**: reference ID; construction/setup; physical and microphone
  frames; four string spans, density, tension, stiffness, loss, frets and bridge
  geometry; wooden soundboard/bowl/cavity meshes or reduced field descriptions;
  modal parameters and uncertainties; source/personal-data rights.
- **Physical model identity**: unique schema version and content hashes of
  canonical asset, calibration and preparation algorithm. Hashes identify
  bytes; they do not certify that those bytes describe reality.
- **Admissibility**: finite and positive masses and lengths, positive and
  physically meaningful tensions, SPD stored elastic tensors (subject to
  model constraints), nonnegative passive loss, compatible port dimensions,
  calibrated units and in-domain observation locations.
- **Serialization**: canonical SI or documented physical units, explicit
  schema and migration, deterministic field handling. For a preset, use a
  different schema from a raw specimen measurement or full live state.

`prepare(canonical_asset, numeric_policy, sample_rate, capacity_policy)` must
validate before it returns a `PreparedModel`. An error includes a stable
machine-readable code and exact failing field outside the callback. The error
must leave the previous live asset untouched. Preparation may allocate, build
matrices, factorize, optimize reduction and inspect external files on a worker
or during a supported synchronous host lifecycle method *outside processing*.

### 2. Reference numerical solver (offline, inspectable)

Proposed `include/setar/numerics` and `src/numerics`:

```cpp
struct ReferenceControls {
    double physical_time_s;
    std::span<const PhysicalGesture> gestures;
};
struct ReferenceDiagnostics {
    double mechanical_energy_j;
    double acoustic_energy_j;
    double external_work_j;
    double dissipated_energy_j;
    double discrete_work_defect_j;
    double nonlinear_residual;
    std::uint32_t solver_iterations;
};
// OfflineOnly: may allocate, adapt mesh, and run expensive diagnostics.
ReferenceStepResult advance_reference(
    const CanonicalModel&, ReferenceState&,
    const ReferenceControls&, const ReferenceSolverPolicy&);
```

The actual types depend on the adopted FDTD/modal/FEM connection. Reference
methods must preserve independent physical observables and raw state traces;
they are not required to be real-time. A high-order nonlinear reference solver
may reject a step that fails convergence. Do not swallow the rejection and
return a normalized waveform. Keep analytic oscillator/two-mass solutions and
manufactured PDE tests independent from code implementing the target update.

### 3. Prepared realtime engine (immutable plan + mutable instance)

Proposed `include/setar/engine` responsibilities:

```cpp
struct ProcessPlan {
    std::uint32_t sample_rate_hz;
    std::uint32_t max_host_block;
    std::uint32_t max_events_per_block;
    std::uint32_t max_active_contacts;
    std::uint32_t max_internal_iterations;
    std::uint32_t internal_rate_factor;
};

enum class GestureKind : std::uint8_t {
    NailEngage, NailMove, NailRelease,
    FingerEngage, FingerMove, FingerRelease,
    TensionMove, DampingMove, EmergencySilence
};
struct GestureEvent {
    std::uint32_t sample_offset;      // within [0, frames] by a defined policy
    std::uint32_t sub_sample_tick;    // optional fixed internal-rate phase
    std::uint32_t stable_sequence;    // deterministic equal-time ordering
    std::uint32_t source_id;
    std::uint8_t physical_string;     // explicit 0..3 after mapping
    GestureKind kind;
    double physical_value_si;        // type-specific interpretation
};
struct AudioFrameView {
    std::span<float> left;
    std::span<float> right;
};
enum class FaultCode : std::uint8_t {
    None, InvalidPreparedState, EventOverflow, InvalidEvent,
    NonFiniteState, ContactSolverFailure, CapacityExceeded
};
struct ProcessSummary {
    FaultCode fault;
    std::uint32_t processed_frames;
    std::uint32_t rejected_events;
    std::uint32_t capped_iterations;
};

// All storage is accepted and reserved before entry; behavior is nonthrowing.
ProcessSummary process( // proposed, not yet compiled
    const PreparedModel&, LiveInstrumentState&,
    std::span<const GestureEvent> normalized_events,
    AudioFrameView outputs) noexcept;
```

This sketch is not a commitment to a per-sample `std::span` copy. `std::span`
is non-owning; all buffers and state must remain valid for the call. `LiveInstrumentState`
must have fixed capacity and no reallocation during `process`. `PreparedModel`
must outlive every process call that reads it. In a future API a single
`InstrumentInstance` owner may encapsulate both, but the lifetime proof must
remain explicit.

Avoid a public `std::vector<GestureEvent>` or `std::function` dispatch whose
capacity/dynamic allocations are invisible to the callback. A caller may own a
preallocated buffer, sort its bounded event span or use a pre-sorted host
contract. The normalization layer rejects out-of-domain offsets, NaNs and
invalid string indices before the solver. Exact policy for an event offset
equal to block length belongs to the next block, with stable tie-breaking.

`EmergencySilence`, release and physically necessary damping updates must not
be discarded under the same overflow policy as noncritical continuous motion.
A priority queue can still be wrong if it silently reorders a contact release
before the engage that made it meaningful. Define a total order and prove
which events are coalescible; test it using adversarial long event streams.

### 4. JUCE/host boundary (optional adapter)

Proposed `apps/plugin` is the only layer that includes JUCE. It owns
`AudioProcessor` preparation, process callbacks, bus negotiation, event
translation, stable parameter identifiers, state serialization, editor, VST3
packaging and optional AU qualification. It never implements the authoritative
string PDE or directly manipulates hidden physical state.

The adapter must preserve sample-accurate offsets where the host supplies
them, and map MIDI/MPE/tuning messages to an explicit physical gesture grammar.
Conventional MIDI note-on alone often does not identify a unique setar string;
ambiguity must follow a documented user mode or error policy. No conversion
may silently reset sympathetic vibration or create an independent sample voice.
Test host lifecycle and state roundtrip separately from the headless engine.

## Concrete state ownership map

| State | Writer | Readers | Transfer policy |
| --- | --- | --- | --- |
| Recorded/source files | Research/acquisition process | Authorized analysis | Immutable, hash/provenance, permission controlled |
| Canonical model | Preparer/importer | Reference and preparation workers | Versioned and immutable after admission |
| Prepared arrays, filters, model projections | Worker/preparer | Audio single-owner, offline compare | Publish as complete immutable candidate |
| Four-string displacement/momentum/strain | One processing owner | Solver and bounded snapshot publisher | Never mutable across worker/UI thread |
| Nail/fret friction/compression history | One processing owner | Contact/coupled step | Persistent across event and host block boundaries |
| Bridge/body/cavity dynamic state | One processing owner | Coupled solver, observation | Shared physical state across strings |
| MIDI note/event tracker | Host/event adapter | Processing owner through normalized events | Separate from physical energy/reset |
| Meter/chart data | Audio writes bounded snapshot | UI/analysis consumes copied snapshot | Bounded stale/lost-data policy |
| Preset/settings bytes | Host state interface | Worker parser/preparer | Validate in temporary state, then publish |

Asset publication needs a proof of pointer lifetime. Atomically replacing a
pointer **without reclamation discipline** is inadequate: the previous owner
could be destroyed on the audio callback, or another reader could still hold a
stale reference. Select a double-buffer, epoch/hazard-based, preallocated
handoff, or equivalent scheme only after its ownership tests pass. A failed
replacement preserves the last accepted model or enters a documented safe
state. No final destructor, file close, or giant deallocation occurs on the
processing path.

## State transitions and error semantics

Define separate operations, each with a precondition and a measurable result:

- `silence_physical_state`: remove stored vibration/contact history and record
  that the action is nonphysical unless a modeled damping transition explains
  it. Use for explicit emergency containment, not an ordinary note-off.
- `clear_host_event_state`: reset note bookkeeping and pending event queues
  while retaining physically meaningful state by policy.
- `release_all_contacts`: move contacts through a defined release transition
  instead of deleting their history silently.
- `replace_prepared_model`: validate the new sample rate/geometry/coefficients,
  decide whether state mapping is supported, then atomically accept or reject.
- `reset_observation`: clear microphone/room/downstream filter memory without
  rewriting physical string displacement.

For a mathematically invalid reference step, return a diagnostic and reject
the candidate. For a realtime fault, a bounded containment path can silence
or reset safely; it must raise a telemetry code. Never call an exception
formatter, logger, allocator, promise/future or GUI callback from `process`.

An internal C++ ABI is not the serialized plugin state format. Public plugin
parameter identifiers must remain stable as C++ layout changes, and schema
migrations must preserve the meaning of old presets or fail transparently.

## Ownership/port acceptance tests before accepting an API

1. `numerics.force_interpolation_adjoint`: random displacement/velocity vector,
   distributed force and weighted work equality with independent explicit
   matrix multiplication, including endpoints and a rotated coordinate frame.
2. `coupling.two_mass_oracle`: two masses + spring with zero/positive damping;
   compare to analytic eigenmodes; force and velocity time-level audit.
3. `coupling.port_power_balance`: coupled total energy and residual over free,
   driven and damped runs; intentionally delayed or reversed port must fail.
4. `performance.persistent_state`: repeated notes/gestures on one physical
   string preserve the original displacement and other string states.
5. `realtime.asset_handoff`: repeated failed/valid preparation and reclamation
   with concurrent snapshot consumption; no dangling references.
6. `plugin.event_partition`: normalized events produce the same physical state
   across block partitions and after host adapter round-trip.
7. `plugin.state_roundtrip`: schema version, unknown/old values, asset hashes,
   byte bounds, rollback and separate physical/live-state semantics.

Until these tests exist and pass, these declarations remain a design proposal.
Read the [implementation work orders](../../roadmap/engineering-tasks.json)
for the exact proposed source/test locations and [requirements](../../roadmap/requirements.json)
for the broader planned acceptance set.
