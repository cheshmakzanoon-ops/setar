#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

namespace setar::plugin {

class SetarProcessor;

// Minimal editor: observation gain, velocity scale, tuning and fret-map selection,
// the engine status and lock-free telemetry. The SYNTHETIC label is always visible.
class SetarEditor final : public juce::AudioProcessorEditor, private juce::Timer {
  public:
    explicit SetarEditor(SetarProcessor& owner);
    ~SetarEditor() override;
    void paint(juce::Graphics& g) override;
    void resized() override;

  private:
    void timerCallback() override;

    SetarProcessor& processor_;
    juce::Label disclaimer_;
    juce::Label status_;
    juce::Label telemetry_;
    juce::Slider gain_;
    juce::Slider dynamics_;
    juce::ComboBox tuning_;
    juce::ComboBox fret_map_;
    juce::Label gain_label_, dynamics_label_, tuning_label_, fret_map_label_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gain_attachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dynamics_attachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> tuning_attachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> fret_map_attachment_;
};

} // namespace setar::plugin
