#pragma once

// Playable four-string setar prototype engine: persistent modal strings (two
// transverse polarizations each), a modal body shared through bridge springs,
// and one-sided nail / finger / fret contacts solved implicitly with an
// energy-consistent discrete-gradient scheme (docs/numerics/15-modal-contact-solver.md).
//
// STATUS: implemented prototype with synthetic parameters. It is NOT a measured
// or validated setar. Claims are limited to the executable tests that exercise it.
//
// Threading: prepare() is a control-thread operation that may allocate and throw.
// process(), reset(), snapshot() and ledger() are noexcept, allocation-free and
// must be called by a single owner (the audio thread) — not concurrently with
// prepare().

#include "setar/instrument/config.hpp"
#include "setar/instrument/gesture.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>

namespace setar::instrument {

enum class FaultCode : std::uint8_t {
    None = 0,
    NotPrepared,            // process() before a successful prepare(); outputs zeroed
    InvalidBlock,           // frames > max_block_frames or left/right size mismatch
    InvalidEvent,           // bad string/slot/kind/field or out-of-order offset; event ignored
    EventOverflow,          // more events than max_events_per_block; excess ignored
    ChannelCapacityExceeded,// more simultaneous channels than max_interaction_channels
    NewtonNotConverged,     // iteration cap reached; last iterate used and counted
    NonFiniteState,         // containment reset performed; output for that sample is 0
};

struct ProcessSummary {
    FaultCode first_fault = FaultCode::None;
    std::uint32_t processed_frames = 0;
    std::uint32_t rejected_events = 0;
    std::uint32_t newton_failures = 0;
    std::uint32_t capacity_overflows = 0;
    std::uint32_t containment_resets = 0;
    std::uint32_t max_newton_iterations = 0;
    std::uint32_t max_active_channels = 0;
};

enum class NailPhase : std::uint8_t { Idle, Approaching, InContact, Released };

struct StringSnapshot {
    NailPhase nail_phase = NailPhase::Idle;
    double nail_force_n = 0.0;                       // >= 0 along the stroke direction
    std::array<bool, 2> finger_active{};
    std::array<double, 2> finger_position_m{};
    std::array<double, 2> finger_force_n{};          // >= 0, pad on string
    std::array<double, polarization_count> bridge_force_n{}; // force on the body, signed
    double energy_j = 0.0;                           // both polarizations, modal H^{n+1/2}
};

struct InstrumentSnapshot {
    std::uint64_t sample_clock = 0;                  // samples processed since prepare/reset
    std::array<StringSnapshot, string_count> strings{};
    double body_energy_j = 0.0;
    std::uint32_t active_channels = 0;
};

// Discrete energy balance since the last prepare()/reset() (requires
// ProcessSpec::enable_energy_ledger; otherwise all fields stay zero).
//   residual = (stored - initial_stored) - external_work + dissipation - switching
// stored = string + body modal energy + interaction potential (all >= 0).
struct EnergyLedger {
    std::array<double, string_count> string_energy_j{};
    double body_energy_j = 0.0;
    double interaction_potential_j = 0.0;
    double initial_stored_j = 0.0;
    double external_work_j = 0.0;   // cumulative work of prescribed nail/finger motion (signed)
    double dissipation_j = 0.0;     // cumulative modal + contact damping + release losses (>= 0)
    double switching_j = 0.0;       // cumulative signed energy of channel (de)activation not provably dissipative
    double residual_j = 0.0;
};

struct PreparedInfo {
    double sample_rate_hz = 0.0;
    std::array<std::size_t, string_count> string_modes{}; // per polarization
    std::size_t body_modes = 0;
    std::size_t total_degrees_of_freedom = 0;
};

class Instrument {
  public:
    Instrument();
    ~Instrument();
    Instrument(Instrument&&) noexcept;
    Instrument& operator=(Instrument&&) noexcept;
    Instrument(const Instrument&) = delete;
    Instrument& operator=(const Instrument&) = delete;

    // Validates config/spec, builds modal banks and preallocates every buffer.
    // Throws std::invalid_argument; on throw the previous prepared state is intact.
    void prepare(const InstrumentConfig& config, const ProcessSpec& spec);
    [[nodiscard]] bool prepared() const noexcept;
    [[nodiscard]] const PreparedInfo& info() const noexcept;
    [[nodiscard]] const InstrumentConfig& config() const noexcept;
    [[nodiscard]] const ProcessSpec& spec() const noexcept;

    // Zero mechanical state, gestures, sample clock and ledger. Keeps preparation.
    void reset() noexcept;

    // Renders left.size() frames (== right.size(), <= max_block_frames) of observed
    // pressure * output_gain_per_pa. Never normalizes or clips.
    ProcessSummary process(std::span<const GestureEvent> events, std::span<float> left,
                           std::span<float> right) noexcept;

    [[nodiscard]] InstrumentSnapshot snapshot() const noexcept;
    [[nodiscard]] EnergyLedger ledger() const noexcept;

    // Offline/test probes (control thread, not realtime-qualified). Throw on bad index.
    [[nodiscard]] double string_displacement_m(std::size_t string_index, Polarization p,
                                               double x_m) const;
    [[nodiscard]] double string_velocity_m_per_s(std::size_t string_index, Polarization p,
                                                 double x_m) const;
    // Overwrite one polarization's modal displacements (both time levels equal:
    // zero velocity). Used for analytic free-vibration fixtures. Resets the ledger.
    void set_string_modal_displacement(std::size_t string_index, Polarization p,
                                       std::span<const double> modal_displacement_m);

  private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace setar::instrument
