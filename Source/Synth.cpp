/*
  ==============================================================================

    Synth.cpp
    Created: 19 Dec 2023 11:17:13pm
    Author:  Carmelo Fascella

  ==============================================================================
*/

#include "Synth.h"

static const float ANALOG = 0.002f;
static const int SUSTAIN = -1;

Synth::Synth()
{
    sampleRate = 44100.0f;
    sustainPedalPressed = false;
}

void Synth::allocateResources(double sampleRate_, int samplesPerBlock)
{
    sampleRate = static_cast<float>(sampleRate_);
    
    //allocate juce dsp building blocks
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1;
    
    for (int v = 0; v < MAX_VOICES; ++v) {
        //SVF Cytomic
        voices[v].filter.sampleRate = sampleRate;
        
        //Ladder
        voices[v].filterLadder.setMode(juce::dsp::LadderFilterMode::LPF12);
        voices[v].filterLadder.prepare(spec);
        voices[v].filterLadder.setDrive(1);
        }
    
}

void Synth::deallocateResources()
{
    
}

void Synth::reset()
{
    for (int v = 0; v < MAX_VOICES; ++v) {
        voices[v].reset();
    }
    noiseGen.reset();
    pitchBend = 1.0f;
    outputLevelSmoother.reset(sampleRate, 0.05);        //0.05 is the duration in seconds (50 ms) of the linear interpolation to go from old to new sample.
    
    lfo = 0.0f;
    lfoStep = 0;
    modWheel = 0.0;
    lastNote = 0;
    resonanctCtl = 1.0f;
    pressure = 0.0f;
    filterCtl = 0.0f;   //amount of filter cutoff modulation coming as midi CC from the keyboard.
    filterZip = 0.0f;
}

void Synth::render(float** outputBuffers, int sampleCount)
{
    float* outputBufferLeft = outputBuffers[0];
    float* outputBufferRight = outputBuffers[1];
    
    // 1) Update parameters
    for (int v = 0; v < MAX_VOICES; ++v) {
        Voice& voice = voices[v];
        if (voice.env.isActive()) {
            updatePeriod(voice);
            voice.glideRate = glideRate;
            voice.filterQ = filterQ * resonanctCtl;
            voice.pitchBend = pitchBend;
            voice.filterEnvDepth = filterEnvDepth;
            voice.filterType = filterType;
        }
    }
    
    // 2) Update sample by sample values
    for (int sample = 0; sample < sampleCount; ++sample) {
        updateLFO();
        
        float noise = noiseGen.nextValue() * noiseMix;  //get next val of the noise
        
        float outputLeft = 0.0f;
        float outputRight = 0.0f;
        for (int v = 0; v < MAX_VOICES; ++v) {
            Voice& voice = voices[v];
            if(voice.env.isActive()) {
                float output = voice.render(noise);     //oscillator rendering + sum of the noise.
                outputLeft += output * voice.panLeft;
                outputRight += output * voice.panRight;
            }
        }
        
        float outputLevel = outputLevelSmoother.getNextValue();
        outputLeft *= outputLevel;
        outputRight *= outputLevel;
        
        if(outputBufferRight != nullptr) {
            outputBufferLeft[sample] = outputLeft;
            outputBufferRight[sample] = outputRight;
        }
        else {
            outputBufferLeft[sample] = (outputLeft + outputRight) * 0.5f;
        }
    }
    
    // 3) Reset not active voices
    for (int v = 0; v < MAX_VOICES; ++v) {
        Voice& voice = voices[v];
        if(!voice.env.isActive()){
            voice.env.reset();
            voice.filter.reset();
            voice.filterQ = filterQ * resonanctCtl;
        }
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
            
        // Pitch Bend
        case 0xE0:
            pitchBend = std::exp(-0.000014102f * float(data1 + 128 * data2 - 8192));    //map the first two bytes into a range of +/-2 semitones.
            break;
        
        // Control change
        case 0xB0:
            controlChange(data1, data2);
            break;
            
        // Mod wheel
        case 0x01:
            modWheel = 0.000005f * float(data2 * data2);
            break;
        
        // Channel aftertouch
        case 0xD0:
            pressure = 0.0001f * float(data1 * data1);

    }
}

void Synth::noteOn(int note, int velocity)
{
    if (ignoreVelocity) { velocity = 80; }
    
    int v = 0;  // index of the voice to use (0 = mono voice)
    
    if (numVoices == 1) {
        if (voices[0].note > 0) {
            shiftQueuedNotes();                     //first shift all the notes
            restartMonoVoice(note, velocity);       //then allocates the current received
            return;
        }
    } else {
        v = findFreeVoice();
    }
    
    startVoice(v, note, velocity);
}

void Synth::startVoice(int v, int note, int velocity)
{
    float period = calcPeriod(v, note);
    
    Voice& voice = voices[v];
    voice.target = period;
    
    int noteDistance = 0;
    if( lastNote > 0 ) {
        if((glideMode == 2) || ((glideMode == 1) && isPlayingLegatoStyle())) {
            noteDistance = note - lastNote;     //calculate note distance between last note and the target if we are in glide mode.
        }
        
    }
    
    voice.period = period * std::pow(1.059463094359f, float(noteDistance) - glideBend);     //1.05.. is 2^(1/12), the rest is the n of semitones to go up or down from the starting period. The higher is glideBend, the longer it takes to reach the target.
    
    //limit period to not be so small.
    if (voice.period < 6.0f) { voice.period = 6.0f; }
    
    lastNote = note;
    voice.note = note;
    voice.updatePanning();
    
    float vel = 0.004f * float((velocity + 64) * (velocity + 64)) - 8.0f; //logarithmic mapping of velocity. Formula chosen by the author.
    //float sens = std::abs(velocitySensitivity) / 0.05;      //value between 0 and 1
    
    voice.osc1.amplitude = volumeTrim * vel;
    voice.osc2.amplitude = voice.osc1.amplitude * oscMix;   //the osc2 has the osc1 amplitude scaled by oscMix param.
    
    if(vibrato == 0.0f && pwmDepth > 0.0f) {
        voice.osc2.squareWave(voice.osc1, voice.period);
    }
    
    voice.cutoff = sampleRate / (period * PI);  //PI is an implementation choice of the author.
    voice.cutoff *= std::exp(velocitySensitivity * float(velocity - 64));   //velocity influences brightness of the sound. velocitySensitivity tells how much.
    
    // Amp envelope
    Envelope& env = voice.env;
    env.attackMultiplier = envAttack;
    env.decayMultiplier = envDecay;
    env.sustainLevel = envSustain;
    env.releaseMultiplier = envRelease;
    env.attack();
    
    // Filter envelope
    Envelope& filterEnv = voice.filterEnv;
    filterEnv.attackMultiplier = filterAttack;
    filterEnv.decayMultiplier = filterDecay;
    filterEnv.sustainLevel = filterSustain;
    filterEnv.releaseMultiplier = filterRelease;
    filterEnv.attack();
    
}

void Synth::noteOff(int note)
{
    
    //In mono mode, check if the keys that got release is the current one and find if there is another currently pressed note to play.
    if ((numVoices == 1) && (voices[0].note == note)) {
        int queuedNote = nextQueuedNote();
        if (queuedNote > 0) {
            restartMonoVoice(queuedNote, -1);
        }
    }
    
    
    for (int v = 0; v < MAX_VOICES; ++v) {
        if (voices[v].note == note) {
            
            if (sustainPedalPressed) {      //case we press the sustain pedal
                voices[v].note = SUSTAIN;
            } else {
                voices[v].release();
                voices[v].note = 0;
            }
        }
    }
    

}

float Synth::calcPeriod(int v, int note) const
{
    float period = tune * std::exp(-0.05776226505f * (float(note) + ANALOG * float(v)));  //optimization formula, derived from 440Hz * 2^((note - 69)/12), grouping elements.
    // ANALOG * v = a way to give a random detuning to the current pitch (1/period)
    // ANALOG = Multiplying period for a ANALOG vibes, gives it a detune typical of analog instruments.
    // v = index of the current voice.
    
    //Ensure period is at least 6 samples long for BLIT-based osc reliability. Highest pitch reachable is sampleRate / 6 samples
    while(period < 6.0f || (period * detune) < 6.0f ) { period += period ;}
    
    
    return period;
}

int Synth::findFreeVoice() const
{
    /* Algorithm used to solve the Voice Stealing */
    
    int v = 0;
    float l = 100.0f;   //louder than any envelope
    
    for (int i=0; i < MAX_VOICES; ++i) {
        if (voices[i].env.level < l && !voices[i].env.isInAttack() ) {      //ignore voices in the attack stage
            l = voices[i].env.level;
            v = i;
        }
    }
    
    return v;
}


void Synth::controlChange (uint8_t data1, uint8_t data2)
{
    switch (data1) {
        //Sustain pedal
        case 0x40:
            sustainPedalPressed = (data2 >= 64);
            
            if (!sustainPedalPressed) {
                noteOff(SUSTAIN);   //voices marked as sustain will be released.
            }
            break;
        
        // Resonance
        case 0x47:
            resonanctCtl = 154.0f / float(154 - data2);
            break;
        
        // Filter +
        case 0x4A:
            filterCtl = 0.02f * float(data2);
            break;
            
        // Filter -
        case 0x4B:
            filterCtl = -0.03f * float(data2);
            break;

        //All notes off
        default:
            if (data1>= 0x78) {
                for (int v = 0; v < MAX_VOICES; ++v) {
                    voices[v].reset();
                }
                sustainPedalPressed = false;
            }
            break;
    }
}

void Synth::restartMonoVoice(int note, int velocity)
{
    float period = calcPeriod(0, note);
    
    Voice& voice = voices[0];
    voice.target = period;
    
    if (glideMode == 0) { voice.period = period; }
    
    voice.env.level += SILENCE + SILENCE;
    voice.note = note;
    voice.updatePanning();
    
    voice.cutoff = sampleRate / (period * PI);
    if(velocity > 0) {
        voice.cutoff *= std::exp(velocitySensitivity * float(velocity - 64));
    }
}

void Synth::shiftQueuedNotes()
{
    /* Shift all the voices to the right, starting from the one before the last one. It modifies only the note property. */
    
    for (int tmp = MAX_VOICES - 1; tmp > 0; tmp --) {
        voices[tmp].note = voices[tmp-1].note;
        voices[tmp].release();
    }
}

int Synth::nextQueuedNote()
{
    /* Find the first available note to play */
    int held = 0;
    for (int v = MAX_VOICES -1; v > 0; v--) {
        if (voices[v].note > 0) { held = v; }
    }
    
    if (held > 0) {
        int note = voices[held].note;
        voices[held].note = 0;
        return note;
    }
    
    return 0;
}

void Synth::updateLFO()
{
    /* Called on every samples at the audio rate, but it handles the update only at LFO rate. (every LFO_MAX steps) */
    if(--lfoStep <=0) {
        lfoStep = LFO_MAX;  //reset step
        
        lfo += lfoInc;  //increment lfo phase
        if (lfo > PI) { lfo -= TWO_PI; }        //phase kept between -+pi
        
        const float sine = std::sin(lfo);
        
        float vibratoMod = 1.0f + sine * (modWheel + vibrato);
        float pwm = 1.0f + sine * (modWheel + pwmDepth);
        
        float filterMod = filterKeyTracking + filterCtl + (filterLFODepth + pressure) * sine;   //multiple modulation sources affect the same target, and they are added togheter. (pressure is the aftertouch)
        
        filterZip += 0.005f * (filterMod - filterZip);      //smoothed version of filterMod (filter coeff hardcoded to 0.005)
        
        for (int v = 0; v < MAX_VOICES; ++v) {
            Voice& voice = voices[v];
            if (voice.env.isActive()) {
                voice.osc1.modulation = vibratoMod;
                voice.osc2.modulation = pwm;
                voice.filterMod = filterZip;
                voice.updateLFO();
                updatePeriod(voice);
            }
        }
        
    }
}


bool Synth::isPlayingLegatoStyle() const
{
    int held = 0;
    for (int i=0; i<MAX_VOICES; ++i) {
        if (voices[i].note > 0) { held += 1; }
    }
    return held > 0;
}


void Synth::updateStereoWidth(float stereoWidth)
{
    for (int i=0; i < MAX_VOICES; ++i){
        voices[i].stereoWidth = stereoWidth;
    }
}
