#pragma once

// JUCE host adapter for setar::instrument::Instrument (ENG-P14-T01 prototype).
// JUCE owns host lifecycle, MIDI, parameters and state only; all physics runs in the
// headless C++20 engine. SYNTHETIC prototype: not a measured or validated setar and
// not a host-qualified plugin (see docs/plugin/08-prototype-juce-adapter.md).

#include "setar/instrument/instrument.hpp"
#include "setar/instrument/performance.hpp"

#include <juce_audio_processors/juce_audio_processors.h>

#include <array>
#include <atomic>
#include <cstdint>
#include <vector>

namespace setar::plugin {

// Stable parameter identifiers (version 1). Never rename or reuse an ID.
inline constexpr const char* param_output_gain_db = "outputGainDb";
inline constexpr const char* param_dynamics_scale = "dynamicsScale";
inline constexpr const char* param_tuning = "tuning";
inline constexpr const char* param_fret_map = "fretMap";
inline constexpr int state_schema_version = 1;

// Written by the audio thread after each block, read by the editor. Lock-free.
struct Telemetry {
    std::atomic<float> total_energy_j{0.0f};
    std::atomic<std::uint32_t> blocks_with_fault{0};
    std::atomic<std::uint32_t> rejected_events{0};
    std::atomic<std::uint32_t> newton_failures{0};
    std::atomic<std::uint32_t> capacity_overflows{0};
    std::atomic<std::uint32_t> containment_resets{0};
    std::atomic<std::uint32_t> dropped_gestures{0};
    std::atomic<std::uint32_t> max_active_channels{0};
};

class SetarProcessor final : public juce::AudioProcessor, private juce::Timer {
  public:
    SetarProcessor();
    ~SetarProcessor() override;

    void prepareToPlay(double sample_rate, int samples_per_block) override;
    void releaseResources() override;
    void reset() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) override;
    using juce::AudioProcessor::processBlock;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "Setar Prototype Synthetic"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 10.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return "Synthetic default"; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock& destination) override;
    void setStateInformation(const void* data, int size_in_bytes) override;

    juce::AudioProcessorValueTreeState& parameters() noexcept { return parameters_; }
    const Telemetry& telemetry() const noexcept { return telemetry_; }
    // Message thread only.
    juce::String engine_status() const { return engine_status_; }
    bool engine_ready() const noexcept { return ready_.load(std::memory_order_acquire); }

  private:
    struct Pending {
        std::uint64_t sample = 0;
        instrument::GestureEvent event{};
    };

    static juce::AudioProcessorValueTreeState::ParameterLayout create_layout();
    // Message/lifecycle thread: builds a config for the selected tuning and fret map
    // and prepares engine + mapper. Never runs concurrently with processBlock.
    void rebuild_engine();
    void timerCallback() override;
    void enqueue(std::uint64_t sample, const instrument::GestureEvent& event) noexcept;

    juce::AudioProcessorValueTreeState parameters_;
    std::atomic<float>* gain_db_ = nullptr;
    std::atomic<float>* dynamics_ = nullptr;
    std::atomic<float>* tuning_index_ = nullptr;
    std::atomic<float>* fret_map_index_ = nullptr;

    instrument::Instrument engine_;
    instrument::MidiGestureMapper mapper_;
    std::atomic<bool> ready_{false};
    juce::String engine_status_{"not prepared"};
    double sample_rate_ = 0.0;
    std::uint32_t max_block_ = 0;
    int applied_tuning_ = -1;
    int applied_fret_map_ = -1;

    // Fixed-capacity gesture queue in absolute sample time (sorted, stable).
    static constexpr std::size_t queue_capacity = 256;
    std::array<Pending, queue_capacity> queue_{};
    std::size_t queue_size_ = 0;
    std::array<instrument::GestureEvent, queue_capacity> block_events_{};
    std::array<instrument::GestureEvent, 16> mapped_{};
    std::uint64_t sample_clock_ = 0;
    float previous_gain_ = 1.0f;
    Telemetry telemetry_;
};

} // namespace setar::plugin
