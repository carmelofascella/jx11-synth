/*
  ==============================================================================

    Voice.h
    Created: 19 Dec 2023 11:17:20pm
    Author:  Carmelo Fascella

  ==============================================================================
*/

#pragma once
#include "Oscillator.h"
#include "Envelope.h"
#include "Filter.h"

struct Voice
{
    int note;
    Oscillator osc1;
    Oscillator osc2;
    float saw;
    Envelope env;
    
    float period;
    
    float panLeft, panRight;    //stereophonic spacing of the midi notes.
    
    float target;   //target note to reach in glide mode.
    
    float glideRate;
    
    Filter filter;
    FilterLadder filterLadder;
    
    float cutoff;
    
    float filterMod;
    
    float filterQ;
    
    float pitchBend;
    
    Envelope filterEnv;     //filter ADSR
    float filterEnvDepth;   //depth of the ENV filter.
    
    float stereoWidth;      //parameter used to multiply the panning value. The higher, the more horizontally spatialized.
    
    float filterType;
    
    void reset()
    {
        note = 0;
        osc1.reset();
        osc2.reset();
        saw = 0.0f;
        env.reset();
        panLeft = 0.707f;
        panRight = 0.707f;
        filter.reset();
        filterLadder.reset();
        filterEnv.reset();
    }
    
    float render(float input)
    {
        /* The input is the noise value */
        
        // 1) Render the two oscillators: getting next sample
        float sample1 = osc1.nextSample();
        float sample2 = osc2.nextSample();
        
        // 2) Combine the two oscillators into a single wave. The * 0.997f is to scale the harmonics of the BLIT in frequency, like the sawtooth oscillator (leaky integrator).
        saw = saw * 0.997f + sample1 - sample2;

        float output = saw + input;
        
        //3) Filter the sound
        filterRender(output);
        
        // 4) Apply the envelope
        float envelope = env.nextValue();
        return output * envelope;
        //return envelope;
    }
    
    void release()
    {
        env.release();
        filterEnv.release();
    }
    
    void updatePanning()
    {
        /* Apply the -3d panning law */
        
        float panning = std::clamp((note - 60.0f) / 24.0f, -1.0f, 1.0f);    //value to space the midi note between -1 and 1. pan value for note 60 (middle C) is 0, so in the centre.
        panLeft = std::sin(PI_OVER_4 * (1.0f - panning * stereoWidth));
        panRight = std::sin(PI_OVER_4 * (1.0f + panning * stereoWidth));
    }
    
    void updateLFO()
    {
        period += glideRate * (target - period);
        
        float fenv = filterEnv.nextValue();
        
        float modulatedCutoff = cutoff * std::exp(filterMod + filterEnvDepth * fenv) / pitchBend;  //makes cutoff higher or lower, relative to the midi-note dependent one, and according to the filter envelope.
        
        /* Update here the cutoff because it's every 32 samples (better for heavy math) */
        modulatedCutoff = std::clamp(modulatedCutoff, 30.0f, 20000.0f); //limit cutoff between 30 and 20k hz
        filter.updateCoefficients(modulatedCutoff, filterQ);
        filterLadder.updateCoefficients(modulatedCutoff, filterQ);
    }
    
    void filterRender(float& value)
    {
        if(filterType==0){
            value = filter.render(value);
        }
        else if(filterType==1){
            value = filterLadder.render(value);
        }
        
    }
};
