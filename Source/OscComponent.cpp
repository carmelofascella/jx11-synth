/*
  ==============================================================================

    OscComponent.cpp
    Created: 11 Jan 2024 3:31:57pm
    Author:  Carmelo Fascella

  ==============================================================================
*/

#include <JuceHeader.h>
#include "OscComponent.h"

//==============================================================================
OscComponent::OscComponent(juce::AudioProcessorValueTreeState& apvts, juce::ParameterID polyModeID, juce::ParameterID oscMixID, juce::ParameterID oscTuneID, juce::ParameterID oscCentID, juce::String componentName)
{
    //using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    //using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    
    polyModeAttachment = std::make_unique<ButtonAttachment>(apvts, polyModeID.getParamID(), polyModeButton);
    oscMixAttachment = std::make_unique<SliderAttachment>(apvts, oscMixID.getParamID(), oscMixKnob.slider);
    oscTuneAttachment = std::make_unique<SliderAttachment>(apvts, oscTuneID.getParamID(), oscTuneKnob.slider);
    oscCentAttachment = std::make_unique<SliderAttachment>(apvts, oscCentID.getParamID(), oscCentKnob.slider);
    
    
    this->componentName = {componentName};
    
    oscMixKnob.label = "Osc Mix";
    oscTuneKnob.label = "Tuning";
    oscCentKnob.label = "Cent Env";
    
    polyModeButton.setButtonText("Poly");
    polyModeButton.setClickingTogglesState(true);
    
    addAndMakeVisible(polyModeButton);
    addAndMakeVisible(oscMixKnob);
    addAndMakeVisible(oscTuneKnob);
    addAndMakeVisible(oscCentKnob);

}

OscComponent::~OscComponent()
{
}

void OscComponent::paint (juce::Graphics& g)
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

void OscComponent::resized()
{
    juce::Rectangle r(20, 50, 100, 120);
    polyModeButton.setSize(80, 30);
    //polyModeButton.setBounds(r);
    polyModeButton.setCentrePosition(r.getCentre());
    
    r = r.withX(r.getRight() + 20);
    oscMixKnob.setBounds(r);
    
    r = r.withX(r.getRight() + 20);
    oscTuneKnob.setBounds(r);
    
    r = r.withX(r.getRight() + 20);
    oscCentKnob.setBounds(r);
    

}
