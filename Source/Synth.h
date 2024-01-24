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
    void updateStereoWidth(float stereoWidth);
    
    float noiseMix;
    
    float envAttack;
    float envDecay;     //value in samples of the decay Time. Calculated in update() in the processor.
    float envSustain;
    float envRelease;
    
    float oscMix;   //how much the second oscillator is mixed in the final sound.
    float detune;   //Osc2 detune: parameter that combines both Osc Tune (detune in semitons) and Osc Fine (detune in cents).
    float tune;     //Global tuning: parameter that combines Octave (+-2 octs) and Tuning (+-100 cents) of the global sound.
    
    static constexpr int MAX_VOICES = 8;
    int numVoices;
    
    float volumeTrim;   //used for gain compensation in different occasions, like when 2 osc are playing togheter with noise.
    
    juce::LinearSmoothedValue<float> outputLevelSmoother;
    
    float velocitySensitivity;
    bool ignoreVelocity;
    
    const int LFO_MAX = 32; //how often the LFO is updated, how many samples per second;
    float lfoInc; //phase increment for the lfo
    
    float vibrato;      //amount of vibrato
    
    float pwmDepth;
    
    int glideMode;
    float glideRate;
    float glideBend;
    
    float filterKeyTracking;
    
    float filterQ;
    
    float filterLFODepth;
    
    float filterAttack, filterDecay, filterSustain, filterRelease;
    float filterEnvDepth;
    
    float stereoWidth;
    
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
    
    float modWheel;
    
    int lastNote;
    
    inline void updatePeriod(Voice& voice)
    {
        voice.osc1.period = voice.period * pitchBend;   //pitch bend val is from the modulation wheel.
        voice.osc2.period = voice.osc1.period * detune; //the period (and the pitch) of the second osc is a bit detuned from the first one. This is because, since we use subtractive synthesis, we don't want to cancels out the two synths.
    }
    
    bool isPlayingLegatoStyle() const;
    
    float resonanctCtl;
    
    float pressure;
    
    float filterCtl;
    
    float filterZip;
};
