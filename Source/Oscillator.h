/*
  ==============================================================================

    Oscillator.h
    Created: 20 Dec 2023 7:40:52pm
    Author:  Carmelo Fascella

  ==============================================================================
*/

#pragma once

const float PI_OVER_4 = 0.7853981633974483f;
const float PI = 3.1415926535897932f;
const float TWO_PI = 6.2831853071795864f;

class Oscillator
{
public:
    float period = 0.0f;
    float amplitude = 1.0f;
    float modulation = 1.0f;        //current amount of modulation to apply to the period.
    
    
    void reset()
    {
        inc = 0.0f;
        phase = 0.0f;
        dc = 0.0f;
    }

    //This is the BLIT (bandlimited impulse train) implementation of the sawtooth.
    float nextSample()
    {
        float output = 0.0f;

        phase += inc;
        if (phase <= PI_OVER_4) {
            float halfPeriod = (period / 2.0f) * modulation;
            phaseMax = std::floor(0.5f + halfPeriod) - 0.5f;

            dc = 0.5f * amplitude / phaseMax;
            phaseMax *= PI;

            inc = phaseMax / halfPeriod;
            phase = -phase;

            //Thi is the optimized digital resonator implementation.
            sin0 = amplitude * std::sin(phase);
            sin1 = amplitude * std::sin(phase - inc);
            dsin = 2.0f * std::cos(inc);

            if (phase*phase > 1e-9) {
                output = sin0 / phase;
            } else {
                output = amplitude;
            }
        } else {
            if (phase > phaseMax) {
                phase = phaseMax + phaseMax - phase;
                inc = -inc;
            }

            float sinp = dsin * sin0 - sin1;
            sin1 = sin0;
            sin0 = sinp;

            output = sinp / phase;
        }

        return output - dc;     //remove DC offset
    }
    
    void squareWave(Oscillator& other, float newPeriod)
    {
        /* called when in PWN mode */
        
        reset();
        
        if(other.inc > 0.0f) {
            phase = other.phaseMax + other.phaseMax - other.phase;
            inc = -other.inc;
        } else if (other.inc < 0.0f) {
            phase = other.phase;
            inc = other.inc;
        } else {
            phase = -PI;
            inc = PI;
        }
        
        phase += PI * newPeriod / 2.0f;
        phaseMax = phase;
    }
    
private:
    float phase;
    float phaseMax;
    
    float sin0;
    float sin1;
    float dsin;
    float inc;
    
    float dc;
};
