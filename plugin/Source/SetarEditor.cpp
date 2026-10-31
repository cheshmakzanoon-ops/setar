#include "SetarEditor.h"

#include "SetarProcessor.h"

namespace setar::plugin {

SetarEditor::SetarEditor(SetarProcessor& owner) : AudioProcessorEditor(owner), processor_(owner) {
    disclaimer_.setText("SYNTHETIC physical-model prototype - not a measured or validated setar",
                        juce::dontSendNotification);
    disclaimer_.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(disclaimer_);
    addAndMakeVisible(status_);
    addAndMakeVisible(telemetry_);
    for (auto* s : {&gain_, &dynamics_}) {
        s->setSliderStyle(juce::Slider::LinearHorizontal);
        s->setTextBoxStyle(juce::Slider::TextBoxRight, false, 70, 20);
        addAndMakeVisible(*s);
    }
    auto& apvts = processor_.parameters();
    auto* tuning_param = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(param_tuning));
    auto* fret_param = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(param_fret_map));
    if (tuning_param != nullptr) {
        tuning_.addItemList(tuning_param->choices, 1);
    }
    if (fret_param != nullptr) {
        fret_map_.addItemList(fret_param->choices, 1);
    }
    addAndMakeVisible(tuning_);
    addAndMakeVisible(fret_map_);
    const std::pair<juce::Label*, const char*> labels[] = {{&gain_label_, "Observation gain"},
                                                           {&dynamics_label_, "Velocity scale"},
                                                           {&tuning_label_, "Tuning"},
                                                           {&fret_map_label_, "Fret map"}};
    for (const auto& [label, text] : labels) {
        label->setText(text, juce::dontSendNotification);
        addAndMakeVisible(*label);
    }
    gain_attachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, param_output_gain_db, gain_);
    dynamics_attachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, param_dynamics_scale, dynamics_);
    tuning_attachment_ =
        std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, param_tuning, tuning_);
    fret_map_attachment_ =
        std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, param_fret_map, fret_map_);
    setSize(560, 250);
    startTimerHz(10);
    timerCallback();
}

SetarEditor::~SetarEditor() { stopTimer(); }

void SetarEditor::paint(juce::Graphics& g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void SetarEditor::resized() {
    auto area = getLocalBounds().reduced(10);
    disclaimer_.setBounds(area.removeFromTop(24));
    const auto row = [&](juce::Label& label, juce::Component& control) {
        auto r = area.removeFromTop(30);
        label.setBounds(r.removeFromLeft(140));
        control.setBounds(r.reduced(0, 3));
    };
    row(gain_label_, gain_);
    row(dynamics_label_, dynamics_);
    row(tuning_label_, tuning_);
    row(fret_map_label_, fret_map_);
    status_.setBounds(area.removeFromTop(24));
    telemetry_.setBounds(area.removeFromTop(24));
}

void SetarEditor::timerCallback() {
    status_.setText(processor_.engine_status(), juce::dontSendNotification);
    const auto& t = processor_.telemetry();
    telemetry_.setText(juce::String("energy ") + juce::String(t.total_energy_j.load(), 6) + " J | channels " +
                           juce::String(t.max_active_channels.load()) + " | faults " +
                           juce::String(t.blocks_with_fault.load()) + " | rejected " +
                           juce::String(t.rejected_events.load()) + " | newton " +
                           juce::String(t.newton_failures.load()) + " | dropped " +
                           juce::String(t.dropped_gestures.load()),
                       juce::dontSendNotification);
}

} // namespace setar::plugin
