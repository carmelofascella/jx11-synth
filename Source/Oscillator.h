/*
  ==============================================================================

    Oscillator.h
    Created: 20 Dec 2023 7:40:52pm
    Author:  Carmelo Fascella

  ==============================================================================
*/

#pragma once

const float TWO_PI = 6.2831853071795864f;

class Oscillator
{
public:
    float amplitude;
    float inc;
    float phase;
    float sin0;
    float sin1;
    float dsin;
    
    void reset()
    {
        phase = 0.0f;
        
        // Digital resonator: optimized way to compute sin oscillator
        sin0 = amplitude * std::sin(phase * TWO_PI);
        sin1 = amplitude * std::sin((phase - inc) * TWO_PI);
        dsin = 2.0f * std::cos(inc * TWO_PI);
    }
    
    float nextSample()
    {
        phase += inc;
        if ( phase >= 1.0f ) {
            phase -= 1.0f;
        }
        
        float sinx = dsin * sin0 - sin1;
        sin1 = sin0;
        sin0 = sinx;
        
        return sinx;
        
        //return amplitude * std::sin(TWO_PI * phase);
    }
};
