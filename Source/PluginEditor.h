/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "RotaryKnob.h"
#include "LookAndFeel.h"
#include "AdsrComponent.h"

//==============================================================================
/**
*/
class JX11AudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    JX11AudioProcessorEditor (JX11AudioProcessor&);
    ~JX11AudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    JX11AudioProcessor& audioProcessor;
    
    AdsrComponent adsrComponent;
    
    using APVTS = juce::AudioProcessorValueTreeState;
    using SliderAttachment = APVTS::SliderAttachment;
    using ButtonAttachment = APVTS::ButtonAttachment;
    
    RotaryKnob outputLevelKnob;
    RotaryKnob filterResoKnob;
    juce::TextButton polyModeButton;
    
    SliderAttachment outputLevelAttachment { audioProcessor.apvts, ParameterID::outputLevel.getParamID(), outputLevelKnob.slider };
    SliderAttachment filterResAttachment { audioProcessor.apvts, ParameterID::filterReso.getParamID(), filterResoKnob.slider };
    ButtonAttachment polyModeAttachment { audioProcessor.apvts, ParameterID::polyMode.getParamID(), polyModeButton };
    
    LookAndFeel globalLNF;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JX11AudioProcessorEditor)
};
