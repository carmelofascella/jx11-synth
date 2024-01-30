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
OscComponent::OscComponent(juce::AudioProcessorValueTreeState& apvts, juce::ParameterID outputLevelID, juce::ParameterID polyModeID, juce::ParameterID oscMixID, juce::ParameterID noiseID, juce::ParameterID oscCentID, juce::ParameterID oscTuneID, juce::ParameterID octaveID, juce::ParameterID tuningID, juce::String componentName)
{

    outputLevelAttachment = std::make_unique<SliderAttachment>(apvts, outputLevelID.getParamID(), outputLevelKnob.slider);
    polyModeAttachment = std::make_unique<ButtonAttachment>(apvts, polyModeID.getParamID(), polyModeButton);
    oscMixAttachment = std::make_unique<SliderAttachment>(apvts, oscMixID.getParamID(), oscMixKnob.slider);
    noiseAttachment = std::make_unique<SliderAttachment>(apvts, noiseID.getParamID(), noiseKnob.slider);
    oscTuneAttachment = std::make_unique<SliderAttachment>(apvts, oscTuneID.getParamID(), oscTuneKnob.slider);
    oscCentAttachment = std::make_unique<SliderAttachment>(apvts, oscCentID.getParamID(), oscCentKnob.slider);
    octaveAttachment = std::make_unique<SliderAttachment>(apvts, octaveID.getParamID(), octaveKnob.slider);
    tuningAttachment = std::make_unique<SliderAttachment>(apvts, tuningID.getParamID(), tuningKnob.slider);
    
    
    this->componentName = {componentName};
    
    outputLevelKnob.label = "Level";
    oscMixKnob.label = "Osc Mix";
    oscTuneKnob.label = "Osc Detune Semi";
    oscCentKnob.label = "Osc Detune Cent";
    noiseKnob.label = "Noise";
    octaveKnob.label = "Octave";
    tuningKnob.label = "Detune";
    
    
    
    
    polyModeButton.setButtonText("Poly");
    polyModeButton.setClickingTogglesState(true);
    
    addAndMakeVisible(outputLevelKnob);
    addAndMakeVisible(polyModeButton);
    addAndMakeVisible(oscMixKnob);
    addAndMakeVisible(noiseKnob);
    addAndMakeVisible(oscTuneKnob);
    addAndMakeVisible(oscCentKnob);
    addAndMakeVisible(octaveKnob);
    addAndMakeVisible(tuningKnob);

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
    polyModeButton.setCentrePosition(r.getCentre());
    
    r = r.withX(r.getRight() + 20);
    outputLevelKnob.setBounds(r);
    
    r = r.withX(r.getRight() + 20);
    oscMixKnob.setBounds(r);
    
    r = r.withX(r.getRight() + 20);
    noiseKnob.setBounds(r);
    
    r = r.withX(r.getRight() + 20);
    oscTuneKnob.setBounds(r);
    
    r = r.withX(r.getRight() + 20);
    oscCentKnob.setBounds(r);
    
    r = r.withX(r.getRight() + 20);
    octaveKnob.setBounds(r);
    
    r = r.withX(r.getRight() + 20);
    tuningKnob.setBounds(r);

}
