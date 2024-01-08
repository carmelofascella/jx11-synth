/*
  ==============================================================================

    AdsrComponent.cpp
    Created: 8 Jan 2024 2:30:22pm
    Author:  Carmelo Fascella

  ==============================================================================
*/

#include <JuceHeader.h>
#include "AdsrComponent.h"

//==============================================================================
AdsrComponent::AdsrComponent(juce::AudioProcessorValueTreeState& apvts,
                             juce::ParameterID envAttackParamID,
                             juce::ParameterID envDecayParamID,
                             juce::ParameterID envSustainParamID,
                             juce::ParameterID envReleaseParamID)
{
    envAttackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, envAttackParamID.getParamID(),  envAttackKnob.slider);
    envDecayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, envDecayParamID.getParamID(),   envDecayKnob.slider);
    envSustainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, envSustainParamID.getParamID(), envSustainKnob.slider);
    envReleaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, envReleaseParamID.getParamID(), envReleaseKnob.slider);
    
    envAttackKnob.label = "Attack";
    envDecayKnob.label = "Decay";
    envSustainKnob.label = "Sustain";
    envReleaseKnob.label = "Release";
    
    addAndMakeVisible(envAttackKnob);
    addAndMakeVisible(envDecayKnob);
    addAndMakeVisible(envSustainKnob);
    addAndMakeVisible(envReleaseKnob);

}

AdsrComponent::~AdsrComponent()
{
}

void AdsrComponent::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().reduced (5);
    auto labelSpace = bounds.removeFromTop (25.0f);
    
    //g.fillAll (juce::Colours::black);
    g.setColour (juce::Colours::white);
    g.setFont (20.0f);
    // g.drawText (componentName, labelSpace.withX (5), juce::Justification::left);
    g.drawText ("Envelope ADSR", labelSpace.withX (5), juce::Justification::left);
    g.drawRoundedRectangle (bounds.toFloat(), 5.0f, 2.0f);
}

void AdsrComponent::resized()
{
    juce::Rectangle r(20, 50, 100, 120);
    envAttackKnob.setBounds(r);
    
    r = r.withX(r.getRight() + 20);
    envDecayKnob.setBounds(r);
    
    r = r.withX(r.getRight() + 20);
    envSustainKnob.setBounds(r);
    
    r = r.withX(r.getRight() + 20);
    envReleaseKnob.setBounds(r);

}
