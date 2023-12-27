/*
  ==============================================================================

    Synth.cpp
    Created: 19 Dec 2023 11:17:13pm
    Author:  Carmelo Fascella

  ==============================================================================
*/

#include "Synth.h"

Synth::Synth()
{
    sampleRate = 44100.0f;
}

void Synth::allocateResources(double sampleRate_, int samplesPerBlock)
{
    sampleRate = static_cast<float>(sampleRate_);
}

void Synth::deallocateResources()
{
    
}

void Synth::reset()
{
    voice.reset();
    noiseGen.reset();
}

void Synth::render(float** outputBuffers, int sampleCount)
{
    float* outputBufferLeft = outputBuffers[0];
    float* outputBufferRight = outputBuffers[1];
    
    voice.osc1.period = voice.period;
    voice.osc2.period = voice.osc1.period * detune;
    
    for (int sample = 0; sample < sampleCount; ++sample) {
        float noise = noiseGen.nextValue() * noiseMix;
        
        float output = 0.0f;
        if(voice.env.isActive()) {
            output = voice.render(noise);
        }
        
        outputBufferLeft[sample] = output;
        if(outputBufferRight != nullptr) {
            outputBufferRight[sample] = output;
        }
    }
    
    if(!voice.env.isActive()){
        voice.env.reset();
    }
    
    protectYourEars(outputBufferLeft, sampleCount);
    protectYourEars(outputBufferRight, sampleCount);
}

void Synth::midiMessage(uint8_t data0, uint8_t data1, uint8_t data2)
{
    switch (data0 & 0xF0) {
        //Note Off
        case 0x80:
            noteOff(data1 & 0x7F);
            break;
            
        //Note On
        case 0x90: {
            uint8_t note = data1 & 0x7F;
            uint8_t velo = data2 & 0x7F;
            if (velo > 0) {
                noteOn(note, velo);
            } else {
                noteOff(note);
            }
            break;
        }
        
    }
}

void Synth::noteOn(int note, int velocity)
{
    voice.note = note;
    
    //float freq = 440.0f * std::exp2((float(note-69) + tune) / 12.0f);
    float period = calcPeriod(note);
    voice.period = period;
    
    voice.osc1.amplitude = (velocity / 127.0f) * 0.5f;
    voice.osc2.amplitude = voice.osc1.amplitude * oscMix;
    
    Envelope& env = voice.env;
    env.attackMultiplier = envAttack;
    env.decayMultiplier = envDecay;
    env.sustainLevel = envSustain;
    env.releaseMultiplier = envRelease;
    env.attack();
    

    
}

void Synth::noteOff(int note)
{
    if(voice.note == note) {
        voice.release();
    }
}

float Synth::calcPeriod(int note) const
{
    float period = tune * std::exp(-0.05776226505f * float(note));  //optimization formula
    
    //Ensure period is at least 6 samples long for BLIT-based osc reliability. Highest pitch reachable is sampleRate / 6 samples
    while(period < 6.0f || (period * detune) < 6.0f ) { period += period ;}
    
    
    return period;
}
