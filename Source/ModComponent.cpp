/*
  ==============================================================================

    ModComponent.cpp
    Created: 11 Jan 2024 6:27:46pm
    Author:  Carmelo Fascella

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ModComponent.h"

//==============================================================================
ModComponent::ModComponent(juce::AudioProcessorValueTreeState& apvts,
                           juce::ParameterID glideModeID,
                           juce::ParameterID glideRateID,
                           juce::ParameterID glideBendID,
                           juce::ParameterID filterVelocityID,
                           juce::ParameterID vibratoID,
                           juce::ParameterID stereoWidthID,
                           juce::String componentName)
{
    
    juce::StringArray choices { "Off", "Legato", "Always" };
    glideModeBox.addItemList(choices, 1);
    
    glideModeAttachment = std::make_unique<ComboBoxAttachment>(apvts, glideModeID.getParamID(), glideModeBox);
    glideRateAttachment = std::make_unique<SliderAttachment>(apvts, glideRateID.getParamID(), glideRateKnob.slider);
    glideBendAttachment = std::make_unique<SliderAttachment>(apvts, glideBendID.getParamID(), glideBendKnob.slider);
    filterVelocityAttachment = std::make_unique<SliderAttachment>(apvts, filterVelocityID.getParamID(), filterVelocityKnob.slider);
    vibratoAttachment = std::make_unique<SliderAttachment>(apvts, vibratoID.getParamID(), vibratoKnob.slider);
    stereoWidthAttachment = std::make_unique<SliderAttachment>(apvts, stereoWidthID.getParamID(), stereoWidthKnob.slider);
    
    glideRateKnob.label = "Glide Rate";
    glideBendKnob.label = "Glide Bend";
    filterVelocityKnob.label = "Velocity Sensitivity";
    vibratoKnob.label = "PWM/Vibrato";
    stereoWidthKnob.label = "Stereo Width";
    
    //glideModeButton.setButtonText("Poly");
    //glideModeButton.setClickingTogglesState(true);
    
    this->componentName = {componentName};
    
    addAndMakeVisible(glideModeBox);
    addAndMakeVisible(glideRateKnob);
    addAndMakeVisible(glideBendKnob);
    addAndMakeVisible(filterVelocityKnob);
    addAndMakeVisible(vibratoKnob);
    addAndMakeVisible(stereoWidthKnob);

}

ModComponent::~ModComponent()
{
}

void ModComponent::paint (juce::Graphics& g)
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

void ModComponent::resized()
{
    juce::Rectangle r(20, 50, 100, 120);
    glideModeBox.setSize(80, 30);
    glideModeBox.setCentrePosition(r.getCentre());
    
    r = r.withX(r.getRight() + 20);
    glideRateKnob.setBounds(r);
    
    r = r.withX(r.getRight() + 20);
    glideBendKnob.setBounds(r);
    
    r = r.withX(r.getRight() + 20);
    filterVelocityKnob.setBounds(r);
    
    r = r.withX(r.getRight() + 20);
    vibratoKnob.setBounds(r);
    
    r = r.withX(r.getRight() + 20);
    stereoWidthKnob.setBounds(r);
    
}
