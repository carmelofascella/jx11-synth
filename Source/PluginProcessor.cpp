/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Utils.h"

//==============================================================================
JX11AudioProcessor::JX11AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    castParameter(apvts, ParameterID::oscMix, oscMixParam);
    castParameter(apvts, ParameterID::oscTune, oscTuneParam);
    castParameter(apvts, ParameterID::oscFine, oscFineParam);
    castParameter(apvts, ParameterID::glideMode, glideModeParam);
    castParameter(apvts, ParameterID::glideRate, glideRateParam);
    castParameter(apvts, ParameterID::glideBend, glideBendParam);
    castParameter(apvts, ParameterID::filterFreq, filterFreqParam);
    castParameter(apvts, ParameterID::filterReso, filterResoParam);
    castParameter(apvts, ParameterID::filterEnv, filterEnvParam);
    castParameter(apvts, ParameterID::filterLFO, filterLFOParam);
    castParameter(apvts, ParameterID::filterVelocity, filterVelocityParam);
    castParameter(apvts, ParameterID::filterAttack, filterAttackParam);
    castParameter(apvts, ParameterID::filterDecay, filterDecayParam);
    castParameter(apvts, ParameterID::filterSustain, filterSustainParam);
    castParameter(apvts, ParameterID::filterRelease, filterReleaseParam);
    castParameter(apvts, ParameterID::envAttack, envAttackParam);
    castParameter(apvts, ParameterID::envDecay, envDecayParam);
    castParameter(apvts, ParameterID::envSustain, envSustainParam);
    castParameter(apvts, ParameterID::envRelease, envReleaseParam);
    castParameter(apvts, ParameterID::lfoRate, lfoRateParam);
    castParameter(apvts, ParameterID::vibrato, vibratoParam);
    castParameter(apvts, ParameterID::noise, noiseParam);
    castParameter(apvts, ParameterID::octave, octaveParam);
    castParameter(apvts, ParameterID::tuning, tuningParam);
    castParameter(apvts, ParameterID::outputLevel, outputLevelParam);
    castParameter(apvts, ParameterID::polyMode, polyModeParam);
    
    apvts.state.addListener(this);  //add listener to the apvts (thisclass inherits from juce::ValueTree::Listener) 
}

JX11AudioProcessor::~JX11AudioProcessor()
{
    apvts.state.removeListener(this);
}

//==============================================================================
const juce::String JX11AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool JX11AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool JX11AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool JX11AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double JX11AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int JX11AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int JX11AudioProcessor::getCurrentProgram()
{
    return 0;
}

void JX11AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String JX11AudioProcessor::getProgramName (int index)
{
    return {};
}

void JX11AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void JX11AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    synth.allocateResources(sampleRate, samplesPerBlock);
    parametersChanged.store(true);
    reset();
}

void JX11AudioProcessor::releaseResources()
{
    synth.deallocateResources();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool JX11AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void JX11AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    //Clear any output channels that don't contain input data
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    //Check if parameter is changed
    bool expected = true;
    if (isNonRealtime() || parametersChanged.compare_exchange_strong(expected, false)) { //check if true and set back to false. it's an atomic operation.
    
        update();
    }
    
    splitBufferByEvents(buffer, midiMessages);

}

//==============================================================================
bool JX11AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* JX11AudioProcessor::createEditor()
{
    //Automatically generated Interface
    auto editor = new juce::GenericAudioProcessorEditor (*this);
    editor->setSize(500, 1050);
    return editor;
}

//==============================================================================
void JX11AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    copyXmlToBinary(*apvts.copyState().createXml(), destData);
}

void JX11AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if(xml.get() != nullptr && xml->hasTagName(apvts.state.getType())) {
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
        parametersChanged.store(true);
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new JX11AudioProcessor();
}

void JX11AudioProcessor::splitBufferByEvents(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    int bufferOffset = 0;
    
    // Loop through the MIDI messages, which are sorted by samplePosition.
    for (const auto metadata : midiMessages) {
        //Render the audio that happens before this event (if any).
        int samplesThisSegment = metadata.samplePosition - bufferOffset;
        if (samplesThisSegment > 0) {
            render(buffer, samplesThisSegment, bufferOffset);
            bufferOffset += samplesThisSegment;
        }
        
        // Handle the event. Ignore MIDI messages such as sysex.
        if (metadata.numBytes <= 3){
            uint8_t data1 = (metadata.numBytes >= 2) ? metadata.data[1] : 0;
            uint8_t data2 = (metadata.numBytes == 3) ? metadata.data[2] : 0;
            handleMIDI(metadata.data[0], data1, data2);
        }
    }
    
    // Render the audio after the last MIDI event. If there were no
    // MIDI events at all, this renders the entire buffer
    int samplesLastSegment = buffer.getNumSamples() - bufferOffset;
    if (samplesLastSegment > 0) {
        render(buffer, samplesLastSegment, bufferOffset);
    }
    
    midiMessages.clear();
        
    
}


void JX11AudioProcessor::handleMIDI(uint8_t data0, uint8_t data1, uint8_t data2)
{
    char s[16];
    snprintf(s, 16, "%02hhX %02hhX %02hhX", data0, data1, data2);
    DBG(s);
    
    synth.midiMessage(data0, data1, data2);
}

void JX11AudioProcessor::render(juce::AudioBuffer<float>& buffer, int sampleCount, int bufferOffset)
{
    float* outputBuffers[2] = {nullptr, nullptr};
    outputBuffers[0] = buffer.getWritePointer(0) + bufferOffset;
    if (getTotalNumOutputChannels() > 1) {
        outputBuffers[1] = buffer.getWritePointer(1) + bufferOffset;
    }
    
    synth.render(outputBuffers, sampleCount);
}

void JX11AudioProcessor::reset()
{
    synth.reset();
    
}

void JX11AudioProcessor::update()
{
    float noiseMix = noiseParam->get() / 100.0f;
    noiseMix *= noiseMix;
    synth.noiseMix = noiseMix * 0.06f;
}


juce::AudioProcessorValueTreeState::ParameterLayout JX11AudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    layout.add(std::make_unique<juce::AudioParameterChoice> (
        ParameterID::polyMode,
        "Polyphony",
        juce::StringArray {"Mono", "Poly"},
        1
                                                             ));
    
    layout.add(std::make_unique<juce::AudioParameterFloat> (
        ParameterID::oscTune,
        "Osc Tune",
        juce::NormalisableRange<float>(-24.0f, 24.0f, 1.0f),
        -12.0f,
        juce::AudioParameterFloatAttributes().withLabel("semi")
                                                             ));
    
    layout.add(std::make_unique<juce::AudioParameterFloat> (
        ParameterID::oscFine,
        "Osc Fine",
        juce::NormalisableRange<float>(-50.0f, 50.0f, 0.1f, 0.3f, true),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel("cent")
                                                             ));
    
    auto oscMixStringFromValue = [](float value, int)
    {
        char s[16] = { 0 };
        snprintf(s, 16, "%4.0f:%2.0f", 100.0 - 0.5f * value, 0.5f * value);
        return juce::String(s);
    };

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterID::oscMix,
        "Osc Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f),
        0.0f,
        juce::AudioParameterFloatAttributes()
                .withLabel("%")
                .withStringFromValueFunction(oscMixStringFromValue)));
    
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        ParameterID::glideMode,
        "Glide Mode",
        juce::StringArray { "Off", "Legato", "Always" },
        0));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterID::glideRate,
        "Glide Rate",
        juce::NormalisableRange<float>(0.0f, 100.f, 1.0f),
        35.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterID::glideBend,
        "Glide Bend",
        juce::NormalisableRange<float>(-36.0f, 36.0f, 0.01f, 0.4f, true),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel("semi")));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterID::filterFreq,
        "Filter Freq",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        100.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterID::filterReso,
        "Filter Reso",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
        15.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterID::filterEnv,
        "Filter Env",
        juce::NormalisableRange<float>(-100.0f, 100.0f, 0.1f),
        50.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterID::filterLFO,
        "Filter LFO",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    auto filterVelocityStringFromValue = [](float value, int)
    {
        if (value < -90.0f)
            return juce::String("OFF");
        else
            return juce::String(value);
    };

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterID::filterVelocity,
        "Velocity",
        juce::NormalisableRange<float>(-100.0f, 100.0f, 1.0f),
        0.0f,
        juce::AudioParameterFloatAttributes()
                .withLabel("%")
                .withStringFromValueFunction(filterVelocityStringFromValue)));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterID::filterAttack,
        "Filter Attack",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterID::filterDecay,
        "Filter Decay",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
        30.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterID::filterSustain,
        "Filter Sustain",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterID::filterRelease,
        "Filter Release",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
        25.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterID::envAttack,
        "Env Attack",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterID::envDecay,
        "Env Decay",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
        50.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterID::envSustain,
        "Env Sustain",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
        100.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterID::envRelease,
        "Env Release",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
        30.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    auto lfoRateStringFromValue = [](float value, int)
    {
        float lfoHz = std::exp(7.0f * value - 4.0f);
        return juce::String(lfoHz, 3);
    };

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterID::lfoRate,
        "LFO Rate",
        juce::NormalisableRange<float>(),
        0.81f,
        juce::AudioParameterFloatAttributes()
                .withLabel("Hz")
                .withStringFromValueFunction(lfoRateStringFromValue)));

    auto vibratoStringFromValue = [](float value, int)
    {
        if (value < 0.0f)
            return "PWM " + juce::String(-value, 1);
        else
            return juce::String(value, 1);
    };

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterID::vibrato,
        "Vibrato",
        juce::NormalisableRange<float>(-100.0f, 100.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes()
                .withLabel("%")
                .withStringFromValueFunction(vibratoStringFromValue)));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterID::noise,
        "Noise",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterID::octave,
        "Octave",
        juce::NormalisableRange<float>(-2.0f, 2.0f, 1.0f),
        0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterID::tuning,
        "Tuning",
        juce::NormalisableRange<float>(-100.0f, 100.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel("cent")));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterID::outputLevel,
        "Output Level",
        juce::NormalisableRange<float>(-24.0f, 6.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));
    
    
    return layout;
}
