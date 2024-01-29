/*
  ==============================================================================

    FilterComponent.h
    Created: 11 Jan 2024 12:18:53pm
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
class FilterComponent  : public juce::Component
{
public:
    FilterComponent(juce::AudioProcessorValueTreeState&, juce::ParameterID, juce::ParameterID, juce::ParameterID, juce::ParameterID, juce::ParameterID, juce::ParameterID, juce::ParameterID, juce::String);
    ~FilterComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    
    RotaryKnob filterFreqKnob;
    RotaryKnob filterResoKnob;
    RotaryKnob filterEnvKnob;
    RotaryKnob filterLFOKnob;
    RotaryKnob LFORateKnob;
    RotaryKnob vibratoKnob;
    juce::ComboBox filterTypeBox;
    
    
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    
    std::unique_ptr<SliderAttachment> filterFreqAttachment;
    std::unique_ptr<SliderAttachment> filterResoAttachment;
    std::unique_ptr<SliderAttachment> filterEnvAttachment;
    std::unique_ptr<SliderAttachment> filterLFOAttachment;
    std::unique_ptr<SliderAttachment> LFORateAttachment;
    std::unique_ptr<SliderAttachment> vibratoAttachment;
    std::unique_ptr<ComboBoxAttachment> filterTypeAttachment;
    
    juce::String componentName;
    
    LookAndFeel globalLNF;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilterComponent)
};
