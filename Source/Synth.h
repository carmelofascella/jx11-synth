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
    
    float oscMix;
    float detune;
    
    float tune;
    
    static constexpr int MAX_VOICES = 8;
    int numVoices;
    
    float volumeTrim;
    
    juce::LinearSmoothedValue<float> outputLevelSmoother;
    
    float velocitySensitivity;
    bool ignoreVelocity;
    
    const int LFO_MAX = 32; //how often the LFO is updated;
    float lfoInc; //phase increment for the lfo
    
    float vibrato;
    
    float pwmDepth;
    
private:
    float sampleRate;
    std::array<Voice, MAX_VOICES> voices;
    NoiseGenerator noiseGen;
    
    void noteOn(int note, int velocity);
    Voice& voice = voices[0];
    void noteOff(int note);
    

    float calcPeriod(int v, int note) const;
    
    float pitchBend;
    
    void startVoice(int v, int note, int velocity);
    
    int findFreeVoice() const;
    
    bool sustainPedalPressed;
    
    void controlChange (uint8_t data1, uint8_t data2);
    
    void restartMonoVoice(int note, int velocity);
    
    void shiftQueuedNotes();
    int nextQueuedNote();
    
    void updateLFO();
    
    int lfoStep;
    float lfo;
};
