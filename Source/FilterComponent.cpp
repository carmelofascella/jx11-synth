/*
  ==============================================================================

    FilterComponent.cpp
    Created: 11 Jan 2024 12:18:53pm
    Author:  Carmelo Fascella

  ==============================================================================
*/

#include <JuceHeader.h>
#include "FilterComponent.h"

//==============================================================================
FilterComponent::FilterComponent(juce::AudioProcessorValueTreeState& apvts,
                                 juce::ParameterID filterFreqID,
                                 juce::ParameterID filterResoID,
                                 juce::ParameterID filterEnvID,
                                 juce::ParameterID filterLFOID,
                                 juce::ParameterID LFORateID,
                                 juce::ParameterID vibratoID,
                                 juce::ParameterID filterTypeID,
                                 juce::String componentName)
{
    
    filterFreqAttachment = std::make_unique<SliderAttachment>(apvts, filterFreqID.getParamID(), filterFreqKnob.slider);
    filterResoAttachment = std::make_unique<SliderAttachment>(apvts, filterResoID.getParamID(), filterResoKnob.slider);
    filterEnvAttachment = std::make_unique<SliderAttachment>(apvts, filterEnvID.getParamID(), filterEnvKnob.slider);
    filterLFOAttachment = std::make_unique<SliderAttachment>(apvts, filterLFOID.getParamID(), filterLFOKnob.slider);
    LFORateAttachment = std::make_unique<SliderAttachment>(apvts, LFORateID.getParamID(), LFORateKnob.slider);
    vibratoAttachment = std::make_unique<SliderAttachment>(apvts, vibratoID.getParamID(), vibratoKnob.slider);
    filterTypeAttachment = std::make_unique<ComboBoxAttachment>(apvts, filterTypeID.getParamID(), filterTypeBox);

    juce::StringArray choices { "SVF", "Ladder"};
    filterTypeBox.addItemList(choices, 1);
    
    this->componentName = {componentName};
    
    filterFreqKnob.label = "Cutoff";
    filterResoKnob.label = "Resonance";
    filterEnvKnob.label = "Filter Env";
    filterLFOKnob.label = "Filter LFO";
    LFORateKnob.label = "LFO Rate";
    vibratoKnob.label = "Vibrato/PWM";
    
    addAndMakeVisible(filterFreqKnob);
    addAndMakeVisible(filterResoKnob);
    addAndMakeVisible(filterEnvKnob);
    addAndMakeVisible(filterLFOKnob);
    addAndMakeVisible(LFORateKnob);
    addAndMakeVisible(vibratoKnob);
    addAndMakeVisible(filterTypeBox);

}

FilterComponent::~FilterComponent()
{
}

void FilterComponent::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().reduced (5);
    auto labelSpace = bounds.removeFromTop (25.0f);
    
    //g.fillAll (juce::Colours::black);
    g.setColour (juce::Colours::white);
    g.setFont (20.0f);
    // g.drawText (componentName, labelSpace.withX (5), juce::Justification::left);
    g.drawText (componentName, labelSpace.withX (5), juce::Justification::left);
    g.drawRoundedRectangle (bounds.toFloat(), 5.0f, 2.0f);
}

void FilterComponent::resized()
{
    juce::Rectangle r(20, 50, 100, 120);
    
    filterTypeBox.setSize(80, 30);
    filterTypeBox.setCentrePosition(r.getCentre());
    
    
    r = r.withX(r.getRight() + 20);
    filterFreqKnob.setBounds(r);
    
    r = r.withX(r.getRight() + 20);
    filterResoKnob.setBounds(r);
    
    r = r.withX(r.getRight() + 20);
    filterEnvKnob.setBounds(r);
    
    r = r.withX(r.getRight() + 20);
    filterLFOKnob.setBounds(r);
    
    r = r.withX(r.getRight() + 20);
    LFORateKnob.setBounds(r);
    
    r = r.withX(r.getRight() + 20);
    vibratoKnob.setBounds(r);

}
