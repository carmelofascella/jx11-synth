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
filterComponent(audioProcessor.apvts, ParameterID::filterFreq, ParameterID::filterReso, ParameterID::filterEnv, ParameterID::filterLFO, ParameterID::lfoRate, ParameterID::filterType, "Filter"),
oscComponent(audioProcessor.apvts, ParameterID::outputLevel, ParameterID::polyMode, ParameterID::oscMix, ParameterID::noise, ParameterID::oscFine, ParameterID::oscTune,  ParameterID::octave, ParameterID::tuning, "Oscillator"),
modComponent(audioProcessor.apvts, ParameterID::glideMode, ParameterID::glideRate, ParameterID::glideBend, ParameterID::filterVelocity, ParameterID::vibrato, ParameterID::stereoWidth, "Mod")

{

    
    addAndMakeVisible(envAdsrComponent);
    addAndMakeVisible(filterAdsrComponent);
    addAndMakeVisible(filterComponent);
    addAndMakeVisible(oscComponent);
    addAndMakeVisible(modComponent);
    
    juce::LookAndFeel::setDefaultLookAndFeel(&globalLNF);
    
    setSize (1024, 900);
}

JX11AudioProcessorEditor::~JX11AudioProcessorEditor()
{
}

//==============================================================================
void JX11AudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    
    auto backgroundImage = juce::ImageCache::getFromMemory(BinaryData::back_wavy_png, BinaryData::back_wavy_pngSize);
    g.drawImageAt (backgroundImage, 0, 0);

}

void JX11AudioProcessorEditor::resized()
{
    juce::Rectangle r(20, 20, 100, 120);

    const int heightComp = 200;
    
    //r = r.withX(r.getRight() + 20);
    //filterResoKnob.setBounds(r);
    
    //polyModeButton.setSize(80, 30);
    //polyModeButton.setCentrePosition(r.withX(r.getRight()).getCentre());
    
    oscComponent.setBounds( 10, r.getY() - 20, 1000, heightComp);
    envAdsrComponent.setBounds(oscComponent.getX(), r.getHeight() + 100, 500, heightComp);
    filterAdsrComponent.setBounds(envAdsrComponent.getRight() + 10, r.getHeight() + 100, 500, heightComp);
    filterComponent.setBounds(envAdsrComponent.getWidth()/3 - 20, envAdsrComponent.getBottom() + 20, 750, heightComp);
    modComponent.setBounds(filterComponent.getX(), filterComponent.getBottom()+20, 750, heightComp);
    
}
