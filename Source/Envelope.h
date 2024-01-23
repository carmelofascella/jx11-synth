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
        level = multiplier * (level - target) + target; //this is the same as  level  = (1-multiplier) * target + multiplier, where multiplier is the 'a' of the one pole filter.
        if (level + target > 3.0f) {    //switch to decay multiplier (the target is 2.0 by definition, and the level should beb more than 1)
            multiplier = decayMultiplier;
            target = sustainLevel;
        }
        return level;
    }
    
    float level;
    
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
        multiplier = releaseMultiplier;     //just updates the release value from processor. this Release() is called in the noteOff()
    }
    
    inline bool isActive() const
    {
        return level > SILENCE;
    }
    
    inline bool isInAttack() const
    {
        return target >= 2.0f;
    }
    
    void attack()
    {
        level += SILENCE + SILENCE;     //to give an initial boost at the first level value (choice of implementation)
        target = 2.0f;      //implementation choice of the JX11: putting the target at 200% and not 100% (1) to make the attack more "attacky" (hard).
        multiplier = attackMultiplier;
    }
    
private:
    float target;
    float multiplier;   //amp value used to multiply the current sample of the wave, to reach a certain target in a certain time.
};
