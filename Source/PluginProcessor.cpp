#include "PluginProcessor.h"
#include "PluginEditor.h"

PinkReverbAudioProcessor::PinkReverbAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
#endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
#endif
                       ),
#endif
    parameters(*this, nullptr, juce::Identifier("PinkReverbParams"),
    {
        std::make_unique<juce::AudioParameterFloat>("roomSize", "Room Size", 0.0f, 1.0f, 0.5f),
        std::make_unique<juce::AudioParameterFloat>("damping", "Damping", 0.0f, 1.0f, 0.5f),
        std::make_unique<juce::AudioParameterFloat>("wetLevel", "Wet Level", 0.0f, 1.0f, 0.33f),
        std::make_unique<juce::AudioParameterFloat>("dryLevel", "Dry Level", 0.0f, 1.0f, 0.4f),
        std::make_unique<juce::AudioParameterFloat>("width", "Width", 0.0f, 1.0f, 1.0f),
        std::make_unique<juce::AudioParameterFloat>("pinkMix", "Pink Mix", 0.0f, 1.0f, 0.5f)
    })
{
    roomSizeParam = parameters.getRawParameterValue("roomSize");
    dampingParam = parameters.getRawParameterValue("damping");
    wetLevelParam = parameters.getRawParameterValue("wetLevel");
    dryLevelParam = parameters.getRawParameterValue("dryLevel");
    widthParam = parameters.getRawParameterValue("width");
    pinkMixParam = parameters.getRawParameterValue("pinkMix");
}

PinkReverbAudioProcessor::~PinkReverbAudioProcessor()
{
}

const juce::String PinkReverbAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PinkReverbAudioProcessor::acceptsMidi() const
{
   return false;
}

bool PinkReverbAudioProcessor::producesMidi() const
{
    return false;
}

bool PinkReverbAudioProcessor::isMidiEffect() const
{
    return false;
}

double PinkReverbAudioProcessor::getTailLengthSeconds() const
{
    return 5.0;
}

int PinkReverbAudioProcessor::getNumPrograms()
{
    return 1;
}

int PinkReverbAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PinkReverbAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String PinkReverbAudioProcessor::getProgramName (int index)
{
    return {};
}

void PinkReverbAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

void PinkReverbAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32> (samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32> (getTotalNumOutputChannels());

    reverb.reset();
    reverb.prepare(spec);

    pinkNoiseL.reset();
    pinkNoiseR.reset();

    pinkNoiseL.prepare(sampleRate);
    pinkNoiseR.prepare(sampleRate);

    pinkNoiseBuffer.setSize(2, samplesPerBlock);
}

void PinkReverbAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PinkReverbAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
#else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void PinkReverbAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Update reverb parameters
    juce::Reverb::Parameters params;
    params.roomSize = *roomSizeParam;
    params.damping = *dampingParam;
    params.wetLevel = *wetLevelParam;
    params.dryLevel = *dryLevelParam;
    params.width = *widthParam;
    params.freezeMode = 0.0f;
    reverb.setParameters(params);

    // Process reverb
    reverb.processStereo(buffer.getWritePointer(0), buffer.getWritePointer(1), buffer.getNumSamples());

    // Generate pink noise and mix in
    float pinkMix = *pinkMixParam;
    if (pinkMix > 0.001f)
    {
        pinkNoiseBuffer.setSize(2, buffer.getNumSamples(), false, false, true);
        pinkNoiseL.getBlock(pinkNoiseBuffer.getWritePointer(0), buffer.getNumSamples());
        pinkNoiseR.getBlock(pinkNoiseBuffer.getWritePointer(1), buffer.getNumSamples());

        for (int ch = 0; ch < juce::jmin(2, buffer.getNumChannels()); ++ch)
        {
            auto* buf = buffer.getWritePointer(ch);
            auto* pink = pinkNoiseBuffer.getReadPointer(ch);
            for (int i = 0; i < buffer.getNumSamples(); ++i)
            {
                buf[i] += pink[i] * pinkMix * 0.15f; // subtle pink coloration
            }
        }
    }
}

//==============================================================================
// Pink noise generator (Voss-McCartney algorithm)
PinkReverbAudioProcessor::PinkNoiseGen::PinkNoiseGen()
{
    reset();
}

void PinkReverbAudioProcessor::PinkNoiseGen::prepare(double sampleRate)
{
    sr = sampleRate;
}

void PinkReverbAudioProcessor::PinkNoiseGen::reset()
{
    for (int i = 0; i < 7; ++i)
        rows[i] = 0;
    runningSum = 0;
    counter = 0;
    rand.seedRandom();
}

float PinkReverbAudioProcessor::PinkNoiseGen::getNextSample()
{
    int i;
    int last_counter = counter;
    counter++;
    if (counter > 0x7F)
        counter = 0;

    int diff = last_counter ^ counter;
    for (i = 0; i < 7; ++i)
    {
        if (diff & (1 << i))
        {
            rows[i] = rand.nextFloat() * 2.0f - 1.0f;
        }
    }

    float sum = 0.0f;
    for (i = 0; i < 7; ++i)
        sum += rows[i];

    return sum / 7.0f;
}

void PinkReverbAudioProcessor::PinkNoiseGen::getBlock(float* dest, int numSamples)
{
    for (int i = 0; i < numSamples; ++i)
        dest[i] = getNextSample();
}

//==============================================================================

bool PinkReverbAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* PinkReverbAudioProcessor::createEditor()
{
    return new PinkReverbAudioProcessorEditor (*this, parameters);
}

void PinkReverbAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto state = parameters.copyState())
    {
        std::unique_ptr<juce::XmlElement> xml (state.createXml());
        copyXmlToBinary (*xml, destData);
    }
}

void PinkReverbAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (parameters.state.getType()))
            parameters.replaceState (juce::ValueTree::fromXml (*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PinkReverbAudioProcessor();
}
```

```cpp