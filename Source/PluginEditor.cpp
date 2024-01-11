/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
JX11AudioProcessorEditor::JX11AudioProcessorEditor (JX11AudioProcessor& p)
    : AudioProcessorEditor (&p),
audioProcessor (p),
envAdsrComponent(audioProcessor.apvts, ParameterID::envAttack, ParameterID::envDecay, ParameterID::envSustain, ParameterID::envRelease, "Env ADSR"),
filterAdsrComponent(audioProcessor.apvts, ParameterID::filterAttack, ParameterID::filterDecay, ParameterID::filterSustain, ParameterID::filterRelease, "Filter ADSR"),
filterComponent(audioProcessor.apvts, ParameterID::filterFreq, ParameterID::filterReso, ParameterID::filterEnv, ParameterID::filterLFO, ParameterID::lfoRate, "Filter"),
oscComponent(audioProcessor.apvts, ParameterID::polyMode, ParameterID::oscMix, ParameterID::oscFine, ParameterID::oscTune, "Oscillator")

{
    outputLevelKnob.label = "Level";
    //filterResoKnob.label = "Reso";
    
    //polyModeButton.setButtonText("Poly");
    //polyModeButton.setClickingTogglesState(true);
    
    addAndMakeVisible(outputLevelKnob);
    //addAndMakeVisible(filterResoKnob);
    //addAndMakeVisible(polyModeButton);
    
    addAndMakeVisible(envAdsrComponent);
    addAndMakeVisible(filterAdsrComponent);
    addAndMakeVisible(filterComponent);
    addAndMakeVisible(oscComponent);
    
    juce::LookAndFeel::setDefaultLookAndFeel(&globalLNF);
    
    setSize (1080, 600);
}

JX11AudioProcessorEditor::~JX11AudioProcessorEditor()
{
}

//==============================================================================
void JX11AudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

}

void JX11AudioProcessorEditor::resized()
{
    juce::Rectangle r(20, 20, 100, 120);
    outputLevelKnob.setBounds(r);
    
    //r = r.withX(r.getRight() + 20);
    //filterResoKnob.setBounds(r);
    
    //polyModeButton.setSize(80, 30);
    //polyModeButton.setCentrePosition(r.withX(r.getRight()).getCentre());
    
    oscComponent.setBounds(outputLevelKnob.getRight() + 20, r.getY() - 20, 500, 200);
    envAdsrComponent.setBounds(outputLevelKnob.getX(), r.getHeight() + 80, 500, 200);
    filterAdsrComponent.setBounds(envAdsrComponent.getRight() + 20, r.getHeight() + 80, 500, 200);
    filterComponent.setBounds(envAdsrComponent.getX(), envAdsrComponent.getBottom() + 20, 650, 200);
}
