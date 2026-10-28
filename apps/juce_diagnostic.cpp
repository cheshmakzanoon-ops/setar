// Optional JUCE 8+ integration scaffold. Not part of the default headless CI.
// This is one fixed synthetic reference, not a musical setar or a host-validated plugin.
#include <JuceHeader.h>
#include "setar/reference.hpp"

#include <algorithm>
#include <atomic>

namespace {
static_assert(std::atomic<bool>::is_always_lock_free,
              "The diagnostic trigger requires a lock-free boolean atomic.");

class DiagnosticProcessor final : public juce::AudioProcessor {
  public:
    DiagnosticProcessor()
        : AudioProcessor(BusesProperties().withOutput("Reference observation",
                                                      juce::AudioChannelSet::stereo(), true)) {}

    void prepareToPlay(double sample_rate, int) override {
        ready_ = false;
        trigger_requested_.store(false, std::memory_order_relaxed);
        try {
            model_.prepare(setar::reference::StringParameters{}, sample_rate, 0.73);
            model_.configure_pluck(0.0005, 0.23);
            ready_ = true;
        } catch (const std::exception&) {
            // Unsupported host sample rates and allocation failures yield silence.
            // No fallback that silently changes physical parameters is introduced.
            model_.reset();
        }
    }

    void releaseResources() override {
        model_.reset();
        ready_ = false;
    }

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override {
        const auto output = layouts.getMainOutputChannelSet();
        return layouts.getMainInputChannelSet().isDisabled() &&
               (output == juce::AudioChannelSet::mono() || output == juce::AudioChannelSet::stereo());
    }

    void processBlock(juce::AudioBuffer<float>& output, juce::MidiBuffer& midi) override {
        juce::ScopedNoDenormals no_denormals;
        output.clear();
        if (!ready_) {
            midi.clear();
            return;
        }
        if (trigger_requested_.exchange(false, std::memory_order_relaxed)) {
            model_.retrigger();
        }
        int offset = 0;
        for (const auto event : midi) {
            // Inspect short MIDI bytes directly. Creating an owning MidiMessage
            // from arbitrary SysEx input could allocate in the audio callback.
            const bool note_on = event.numBytes == 3 && (event.data[0] & 0xf0U) == 0x90U &&
                                 event.data[2] > 0;
            if (!note_on) {
                continue;
            }
            const int boundary = std::clamp(event.samplePosition, offset, output.getNumSamples());
            render_range(output, offset, boundary);
            model_.retrigger();
            offset = boundary;
        }
        render_range(output, offset, output.getNumSamples());
        midi.clear();
    }

    void request_pluck() noexcept { trigger_requested_.store(true, std::memory_order_relaxed); }
    const juce::String getName() const override { return "Setar Reference Diagnostic"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 20.0; }
    bool hasEditor() const override { return true; }
    juce::AudioProcessorEditor* createEditor() override;
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return "Synthetic fixed fixture"; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock& destination) override { destination.reset(); }
    void setStateInformation(const void*, int) override {}

  private:
    void render_range(juce::AudioBuffer<float>& output, int begin, int end) noexcept {
        for (int sample = begin; sample < end; ++sample) {
            const float value = static_cast<float>(800.0 * model_.tick());
            for (int channel = 0; channel < output.getNumChannels(); ++channel) {
                output.setSample(channel, sample, value);
            }
        }
    }
    setar::reference::ModalString model_;
    std::atomic<bool> trigger_requested_{false};
    bool ready_ = false;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DiagnosticProcessor)
};

class DiagnosticEditor final : public juce::AudioProcessorEditor {
  public:
    explicit DiagnosticEditor(DiagnosticProcessor& processor)
        : AudioProcessorEditor(processor), processor_(processor) {
        title_.setText("SETAR / REFERENCE LAB", juce::dontSendNotification);
        title_.setFont(juce::FontOptions(28.0f));
        subtitle_.setText("One synthetic stiff-string fixture. Not a validated setar.",
                          juce::dontSendNotification);
        details_.setText("16 exact linear modes | fixed gain | no samples\n"
                         "0.66 m / 35 N / 0.0007 kg per m\n"
                         "Any MIDI note-on retriggers the same pitch.\n"
                         "Pitch, velocity, note-off and pedals are intentionally not modeled.",
                         juce::dontSendNotification);
        warning_.setText("No body radiation, nail contact, frets, coupled strings or calibration.\n"
                         "Use the headless renderer and its report for numerical evidence.",
                         juce::dontSendNotification);
        pluck_.setButtonText("Pluck synthetic reference");
        pluck_.setTooltip("Retrigger the fixed synthetic displacement at the next audio block");
        pluck_.onClick = [this] { processor_.request_pluck(); };
        for (auto* label : {&title_, &subtitle_, &details_, &warning_}) {
            label->setColour(juce::Label::textColourId, juce::Colour(0xff14212b));
            addAndMakeVisible(label);
        }
        warning_.setColour(juce::Label::textColourId, juce::Colour(0xff8f3d38));
        addAndMakeVisible(pluck_);
        setSize(680, 380);
    }

    void paint(juce::Graphics& graphics) override {
        graphics.fillAll(juce::Colour(0xfff5f4ed));
        graphics.setColour(juce::Colour(0xffdce1dc));
        graphics.drawLine(28.0f, 110.0f, static_cast<float>(getWidth() - 28), 110.0f);
    }

    void resized() override {
        auto area = getLocalBounds().reduced(28);
        title_.setBounds(area.removeFromTop(43));
        subtitle_.setBounds(area.removeFromTop(34));
        area.removeFromTop(20);
        details_.setBounds(area.removeFromTop(90));
        area.removeFromTop(12);
        pluck_.setBounds(area.removeFromTop(42).removeFromLeft(270));
        area.removeFromTop(12);
        warning_.setBounds(area);
    }

  private:
    DiagnosticProcessor& processor_;
    juce::Label title_, subtitle_, details_, warning_;
    juce::TextButton pluck_;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DiagnosticEditor)
};

juce::AudioProcessorEditor* DiagnosticProcessor::createEditor() {
    return new DiagnosticEditor(*this);
}
} // namespace

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new DiagnosticProcessor(); }
