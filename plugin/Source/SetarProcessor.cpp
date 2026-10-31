#include "SetarProcessor.h"

#include "SetarEditor.h"
#include "setar/instrument/setar_preset.hpp"

#include <algorithm>
#include <cmath>
#include <exception>

namespace setar::plugin {
namespace {

juce::StringArray tuning_choices() {
    juce::StringArray out;
    for (const auto& t : instrument::tuning_presets()) {
        out.add(juce::String(t.id));
    }
    if (out.isEmpty()) {
        out.add("(no tuning presets)");
    }
    return out;
}

juce::StringArray fret_map_choices() {
    juce::StringArray out;
    for (const auto& f : instrument::fret_map_definitions()) {
        out.add(juce::String(f.id));
    }
    if (out.isEmpty()) {
        out.add("(no fret maps)");
    }
    return out;
}

int choice_index(const std::atomic<float>* raw) noexcept {
    return static_cast<int>(std::lround(raw->load(std::memory_order_relaxed)));
}

} // namespace

SetarProcessor::SetarProcessor()
    : AudioProcessor(BusesProperties().withOutput("Observed pressure", juce::AudioChannelSet::stereo(), true)),
      parameters_(*this, nullptr, "SetarPrototypeState", create_layout()) {
    gain_db_ = parameters_.getRawParameterValue(param_output_gain_db);
    dynamics_ = parameters_.getRawParameterValue(param_dynamics_scale);
    tuning_index_ = parameters_.getRawParameterValue(param_tuning);
    fret_map_index_ = parameters_.getRawParameterValue(param_fret_map);
    // Tuning/fret-map changes are polled on the message thread (never from the audio
    // thread) and applied by a suspended re-prepare.
    startTimerHz(5);
}

SetarProcessor::~SetarProcessor() { stopTimer(); }

juce::AudioProcessorValueTreeState::ParameterLayout SetarProcessor::create_layout() {
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{param_output_gain_db, 1}, "Observation gain",
        juce::NormalisableRange<float>(-48.0f, 24.0f, 0.1f), 0.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{param_dynamics_scale, 1}, "Velocity scale",
        juce::NormalisableRange<float>(0.25f, 2.0f, 0.01f), 1.0f));
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{param_tuning, 1}, "Tuning (re-prepares)", tuning_choices(), 0,
        juce::AudioParameterChoiceAttributes().withAutomatable(false)));
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{param_fret_map, 1}, "Fret map (re-prepares)", fret_map_choices(), 0,
        juce::AudioParameterChoiceAttributes().withAutomatable(false)));
    return layout;
}

void SetarProcessor::rebuild_engine() {
    ready_.store(false, std::memory_order_release);
    applied_tuning_ = choice_index(tuning_index_);
    applied_fret_map_ = choice_index(fret_map_index_);
    if (sample_rate_ <= 0.0 || max_block_ == 0) {
        engine_status_ = "waiting for prepareToPlay";
        return;
    }
    try {
        const auto tunings = instrument::tuning_presets();
        const auto maps = instrument::fret_map_definitions();
        if (tunings.empty() || maps.empty()) {
            throw std::invalid_argument("no tuning presets or fret maps are available");
        }
        const auto& tuning = tunings[static_cast<std::size_t>(
            std::clamp(applied_tuning_, 0, static_cast<int>(tunings.size()) - 1))];
        const auto& map = maps[static_cast<std::size_t>(
            std::clamp(applied_fret_map_, 0, static_cast<int>(maps.size()) - 1))];
        instrument::PresetOptions options;
        options.tuning_id = tuning.id;
        options.fret_map_id = map.id;
        const instrument::InstrumentConfig config = instrument::synthetic_setar_config(options);
        instrument::ProcessSpec spec;
        spec.sample_rate_hz = sample_rate_;
        spec.max_block_frames = max_block_;
        spec.max_events_per_block = static_cast<std::uint32_t>(queue_capacity);
        spec.max_interaction_channels = 32;
        spec.max_newton_iterations = 50;
        spec.newton_tolerance_m = 1.0e-14;
        spec.output_gain_per_pa = 1.0; // full scale = 1 Pa observed (synthetic observation)
        spec.enable_energy_ledger = false;
        // Both prepares are transactional: on throw the previous engine/mapper stay intact
        // but are not marked ready, because the selection they represent is stale.
        engine_.prepare(config, spec);
        mapper_.prepare(engine_.config(), tuning.open_frequency_hz,
                        instrument::default_performance_settings(), 440.0);
        queue_size_ = 0;
        sample_clock_ = 0;
        engine_status_ = juce::String("ready: ") + tuning.id + " / " + map.id + " @ " +
                         juce::String(sample_rate_, 0) + " Hz (SYNTHETIC prototype)";
        ready_.store(true, std::memory_order_release);
    } catch (const std::exception& e) {
        engine_status_ = juce::String("engine not prepared: ") + e.what();
    }
}

void SetarProcessor::prepareToPlay(double sample_rate, int samples_per_block) {
    sample_rate_ = sample_rate;
    // Larger host blocks are split; the engine capacity never depends on host honesty.
    max_block_ = static_cast<std::uint32_t>(std::clamp(samples_per_block > 0 ? samples_per_block : 512, 32, 2048));
    previous_gain_ = juce::Decibels::decibelsToGain(gain_db_->load(std::memory_order_relaxed));
    rebuild_engine();
}

void SetarProcessor::releaseResources() {}

void SetarProcessor::reset() {
    // Host-serialized with processBlock: silence physical state and pending gestures.
    engine_.reset();
    queue_size_ = 0;
}

void SetarProcessor::timerCallback() {
    if (choice_index(tuning_index_) == applied_tuning_ && choice_index(fret_map_index_) == applied_fret_map_) {
        return;
    }
    // Documented policy: a tuning or fret-map change re-prepares the engine with
    // processing suspended. String motion is reset; this is NOT a physical retuning
    // transition (REQ-FRT-002 remains open).
    suspendProcessing(true);
    rebuild_engine();
    suspendProcessing(false);
}

bool SetarProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
    return layouts.getMainInputChannelSet().isDisabled() &&
           layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void SetarProcessor::enqueue(std::uint64_t sample, const instrument::GestureEvent& event) noexcept {
    if (queue_size_ == queue_capacity) {
        telemetry_.dropped_gestures.fetch_add(1, std::memory_order_relaxed);
        return;
    }
    // Stable sorted insertion: after all entries with sample <= this one.
    std::size_t at = queue_size_;
    while (at > 0 && queue_[at - 1].sample > sample) {
        queue_[at] = queue_[at - 1];
        --at;
    }
    queue_[at] = Pending{sample, event};
    ++queue_size_;
}

void SetarProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) {
    juce::ScopedNoDenormals no_denormals;
    buffer.clear();
    const int frames = buffer.getNumSamples();
    if (!ready_.load(std::memory_order_acquire) || buffer.getNumChannels() < 2 || frames <= 0) {
        midi.clear();
        return;
    }
    const float dynamics = dynamics_->load(std::memory_order_relaxed);
    const std::uint64_t block_start = sample_clock_;

    // MIDI -> physical gestures in absolute sample time.
    for (const auto metadata : midi) {
        const auto message = metadata.getMessage();
        const int position = std::clamp(metadata.samplePosition, 0, frames - 1);
        const std::uint64_t at = block_start + static_cast<std::uint64_t>(position);
        std::size_t count = 0;
        if (message.isNoteOn()) {
            const int scaled = static_cast<int>(std::lround(static_cast<float>(message.getVelocity()) * dynamics));
            count = mapper_.note_on(static_cast<std::uint8_t>(message.getNoteNumber()),
                                    static_cast<std::uint8_t>(std::clamp(scaled, 1, 127)), 0,
                                    std::span<instrument::GestureEvent>(mapped_.data(), mapped_.size()));
        } else if (message.isNoteOff()) {
            count = mapper_.note_off(static_cast<std::uint8_t>(message.getNoteNumber()), 0,
                                     std::span<instrument::GestureEvent>(mapped_.data(), mapped_.size()));
        } else if (message.isAllSoundOff()) {
            mapped_[0] = instrument::GestureEvent{};
            mapped_[0].kind = instrument::GestureKind::Silence;
            count = 1;
        }
        for (std::size_t i = 0; i < count; ++i) {
            // The mapper's offset (relative to 0 here) carries its own lead/lag timing.
            enqueue(at + mapped_[i].sample_offset, mapped_[i]);
        }
    }
    midi.clear();

    float* left = buffer.getWritePointer(0);
    float* right = buffer.getWritePointer(1);
    std::uint32_t worst_channels = telemetry_.max_active_channels.load(std::memory_order_relaxed);
    for (int start = 0; start < frames; start += static_cast<int>(max_block_)) {
        const int n = std::min(static_cast<int>(max_block_), frames - start);
        const std::uint64_t chunk_start = block_start + static_cast<std::uint64_t>(start);
        const std::uint64_t chunk_end = chunk_start + static_cast<std::uint64_t>(n);
        std::size_t taken = 0;
        while (taken < queue_size_ && queue_[taken].sample < chunk_end) {
            instrument::GestureEvent e = queue_[taken].event;
            e.sample_offset = queue_[taken].sample > chunk_start
                                  ? static_cast<std::uint32_t>(queue_[taken].sample - chunk_start)
                                  : 0u;
            e.sequence = static_cast<std::uint32_t>(taken);
            block_events_[taken] = e;
            ++taken;
        }
        std::copy(queue_.begin() + static_cast<std::ptrdiff_t>(taken),
                  queue_.begin() + static_cast<std::ptrdiff_t>(queue_size_), queue_.begin());
        queue_size_ -= taken;
        const auto summary = engine_.process(
            std::span<const instrument::GestureEvent>(block_events_.data(), taken),
            std::span<float>(left + start, static_cast<std::size_t>(n)),
            std::span<float>(right + start, static_cast<std::size_t>(n)));
        if (summary.first_fault != instrument::FaultCode::None) {
            telemetry_.blocks_with_fault.fetch_add(1, std::memory_order_relaxed);
        }
        telemetry_.rejected_events.fetch_add(summary.rejected_events, std::memory_order_relaxed);
        telemetry_.newton_failures.fetch_add(summary.newton_failures, std::memory_order_relaxed);
        telemetry_.capacity_overflows.fetch_add(summary.capacity_overflows, std::memory_order_relaxed);
        telemetry_.containment_resets.fetch_add(summary.containment_resets, std::memory_order_relaxed);
        worst_channels = std::max(worst_channels, summary.max_active_channels);
    }
    sample_clock_ += static_cast<std::uint64_t>(frames);
    telemetry_.max_active_channels.store(worst_channels, std::memory_order_relaxed);

    // Observation gain only: never feeds back into the mechanics. Ramped per block.
    const float gain = juce::Decibels::decibelsToGain(gain_db_->load(std::memory_order_relaxed));
    buffer.applyGainRamp(0, frames, previous_gain_, gain);
    previous_gain_ = gain;

    const auto snapshot = engine_.snapshot();
    double energy = snapshot.body_energy_j;
    for (const auto& s : snapshot.strings) {
        energy += s.energy_j;
    }
    telemetry_.total_energy_j.store(static_cast<float>(energy), std::memory_order_relaxed);
}

void SetarProcessor::getStateInformation(juce::MemoryBlock& destination) {
    auto state = parameters_.copyState();
    state.setProperty("schemaVersion", state_schema_version, nullptr);
    // Choice indices depend on preset-list order; the IDs are the durable identity.
    const auto tunings = instrument::tuning_presets();
    const auto maps = instrument::fret_map_definitions();
    const int t = choice_index(tuning_index_);
    const int f = choice_index(fret_map_index_);
    state.setProperty("tuningId",
                      t >= 0 && static_cast<std::size_t>(t) < tunings.size()
                          ? juce::String(tunings[static_cast<std::size_t>(t)].id)
                          : juce::String(),
                      nullptr);
    state.setProperty("fretMapId",
                      f >= 0 && static_cast<std::size_t>(f) < maps.size()
                          ? juce::String(maps[static_cast<std::size_t>(f)].id)
                          : juce::String(),
                      nullptr);
    if (const auto xml = state.createXml()) {
        copyXmlToBinary(*xml, destination);
    }
}

void SetarProcessor::setStateInformation(const void* data, int size_in_bytes) {
    const auto xml = getXmlFromBinary(data, size_in_bytes);
    // Malformed, foreign or newer-schema state is ignored: the current state is kept.
    if (xml == nullptr || !xml->hasTagName(parameters_.state.getType())) {
        return;
    }
    auto state = juce::ValueTree::fromXml(*xml);
    if (static_cast<int>(state.getProperty("schemaVersion", -1)) != state_schema_version) {
        return;
    }
    parameters_.replaceState(state);
    const auto resolve = [](const juce::String& id, auto list) {
        for (std::size_t i = 0; i < list.size(); ++i) {
            if (id == juce::String(list[i].id)) {
                return static_cast<int>(i);
            }
        }
        return -1;
    };
    const auto set_choice = [this](const char* param, int index) {
        if (index < 0) {
            return; // unknown ID: keep the restored index rather than guessing
        }
        if (auto* p = parameters_.getParameter(param)) {
            p->setValueNotifyingHost(p->convertTo0to1(static_cast<float>(index)));
        }
    };
    set_choice(param_tuning, resolve(state.getProperty("tuningId").toString(), instrument::tuning_presets()));
    set_choice(param_fret_map,
               resolve(state.getProperty("fretMapId").toString(), instrument::fret_map_definitions()));
    // The timer applies a changed selection via a suspended re-prepare.
}

juce::AudioProcessorEditor* SetarProcessor::createEditor() { return new SetarEditor(*this); }

} // namespace setar::plugin

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new setar::plugin::SetarProcessor(); }
