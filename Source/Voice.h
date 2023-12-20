/*
  ==============================================================================

    Voice.h
    Created: 19 Dec 2023 11:17:20pm
    Author:  Carmelo Fascella

  ==============================================================================
*/

#pragma once
#include "Oscillator.h"

struct Voice
{
    int note;
    Oscillator osc;
    
    void reset()
    {
        note = 0;
        osc.reset();
    }
    
    float render()
    {
        return osc.nextSample();
    }
};
