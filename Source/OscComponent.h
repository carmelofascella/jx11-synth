/*
  ==============================================================================

    OscComponent.h
    Created: 11 Jan 2024 3:31:57pm
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
class OscComponent  : public juce::Component
{
public:
    OscComponent(juce::AudioProcessorValueTreeState&, juce::ParameterID, juce::ParameterID, juce::ParameterID, juce::ParameterID, juce::ParameterID, juce::ParameterID, juce::ParameterID, juce::String);
    ~OscComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    juce::TextButton polyModeButton;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    
    RotaryKnob oscMixKnob;
    RotaryKnob noiseKnob;
    RotaryKnob oscTuneKnob;
    RotaryKnob oscCentKnob;
    RotaryKnob octaveKnob;
    RotaryKnob detuneKnob;
    RotaryKnob tuningKnob;
    
    std::unique_ptr<ButtonAttachment> polyModeAttachment;
    std::unique_ptr<SliderAttachment> oscMixAttachment;
    std::unique_ptr<SliderAttachment> noiseAttachment;
    std::unique_ptr<SliderAttachment> oscTuneAttachment;
    std::unique_ptr<SliderAttachment> oscCentAttachment;
    std::unique_ptr<SliderAttachment> octaveAttachment;
    std::unique_ptr<SliderAttachment> tuningAttachment;
    
    juce::String componentName;
    
    LookAndFeel globalLNF;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscComponent)
};
