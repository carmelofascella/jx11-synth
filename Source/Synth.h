/*
  ==============================================================================

    Synth.h
    Created: 19 Dec 2023 11:17:13pm
    Author:  Carmelo Fascella

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Voice.h"
#include "NoiseGenerator.h"
#include "Utils.h"

class Synth
{
public:
    Synth();
    void allocateResources(double sampleRate, int samplesPerBlock);
    void deallocateResources();
    void reset();
    void render(float** outputBuffers, int sampleCount);
    void midiMessage(uint8_t data0, uint8_t data1, uint8_t data2);
    
    float noiseMix;
    
    float envAttack;
    float envDecay;
    float envSustain;
    float envRelease;
    
private:
    float sampleRate;
    Voice voice;
    NoiseGenerator noiseGen;
    
    void noteOn(int note, int velocity);
    void noteOff(int note);
};
