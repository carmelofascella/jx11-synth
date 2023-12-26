/*
  ==============================================================================

    Envelope.h
    Created: 26 Dec 2023 3:04:16pm
    Author:  Carmelo Fascella

  ==============================================================================
*/

#pragma once

const float SILENCE = 0.0001f;


class Envelope
{
public:
    float nextValue()
    {
        level = multiplier * (level - target) + target;
        return level;
    }
    
    float level;
    float multiplier;
    float target;
    
    float attackMultiplier;
    float decayMultiplier;
    float sustainLevel;
    float releaseMultiplier;
    
    void reset()
    {
        level = 0.0f;
        target = 0.0f;
        multiplier = 0.0f;
    }
    
    void release()
    {
        target = 0.0f;
        multiplier = releaseMultiplier;
    }
    
    inline bool isActive() const
    {
        return level > SILENCE;
    }
};
