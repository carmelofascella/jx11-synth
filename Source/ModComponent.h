/*
  ==============================================================================

    ModComponent.h
    Created: 11 Jan 2024 6:27:46pm
    Author:  Carmelo Fascella

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "RotaryKnob.h"
#include "LookAndFeel.h"

//==============================================================================
/*
*/
class ModComponent  : public juce::Component
{
public:
    ModComponent(juce::AudioProcessorValueTreeState&,
                 juce::ParameterID,
                 juce::ParameterID,
                 juce::ParameterID,
                 juce::ParameterID,
                 juce::String);
    ~ModComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    
    //juce::TextButton glideModeButton;
    juce::ComboBox glideModeBox;
    RotaryKnob glideRateKnob;
    RotaryKnob glideBendKnob;
    RotaryKnob vibratoKnob;

    //std::unique_ptr<ButtonAttachment> glideModeAttachment;
    std::unique_ptr<ComboBoxAttachment> glideModeAttachment;
    std::unique_ptr<SliderAttachment> glideRateAttachment;
    std::unique_ptr<SliderAttachment> glideBendAttachment;
    std::unique_ptr<SliderAttachment> vibratoAttachment;

    juce::String componentName;
    LookAndFeel globalLNF;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModComponent)
};
