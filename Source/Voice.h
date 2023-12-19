/*
  ==============================================================================

    Voice.h
    Created: 19 Dec 2023 11:17:20pm
    Author:  Carmelo Fascella

  ==============================================================================
*/

#pragma once

struct Voice
{
    int note;
    int velocity;
    
    void reset()
    {
        note = 0;
        velocity = 0;
    }
};
