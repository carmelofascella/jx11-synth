/*
  ==============================================================================

    AdsrComponent.h
    Created: 8 Jan 2024 2:30:22pm
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
class AdsrComponent  : public juce::Component
{
public:
    AdsrComponent(juce::AudioProcessorValueTreeState&,
                  juce::ParameterID,
                  juce::ParameterID,
                  juce::ParameterID,
                  juce::ParameterID,
                  juce::String);
    ~AdsrComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    
    RotaryKnob envAttackKnob;
    RotaryKnob envDecayKnob;
    RotaryKnob envSustainKnob;
    RotaryKnob envReleaseKnob;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    
    //SliderAttachment envAttackAttachment { audioProcessor.apvts, ParameterID::outputLevel.getParamID(), outputLevelKnob.slider };
    std::unique_ptr<SliderAttachment> envAttackAttachment;
    std::unique_ptr<SliderAttachment> envDecayAttachment;
    std::unique_ptr<SliderAttachment> envSustainAttachment;
    std::unique_ptr<SliderAttachment> envReleaseAttachment;
    
    LookAndFeel globalLNF;
    
    juce::String componentName;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AdsrComponent)
};
