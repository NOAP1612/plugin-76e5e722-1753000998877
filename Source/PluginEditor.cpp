#include "PluginProcessor.h"
#include "PluginEditor.h"

PinkReverbAudioProcessorEditor::PinkReverbAudioProcessorEditor (PinkReverbAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), audioProcessor (p), valueTreeState(vts)
{
    setSize (420, 300);

    // Pink background
    pink = juce::Colour::fromRGB(255, 105, 180);

    // Room Size
    addAndMakeVisible(roomSizeSlider);
    roomSizeSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    roomSizeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    roomSizeSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::white.withAlpha(0.9f));
    roomSizeSlider.setColour(juce::Slider::thumbColourId, pink.darker(0.2f));
    roomSizeLabel.setText("Room Size", juce::dontSendNotification);
    roomSizeLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(roomSizeLabel);
    roomSizeAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, "roomSize", roomSizeSlider));

    // Damping
    addAndMakeVisible(dampingSlider);
    dampingSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    dampingSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    dampingSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::white.withAlpha(0.9f));
    dampingSlider.setColour(juce::Slider::thumbColourId, pink.darker(0.2f));
    dampingLabel.setText("Damping", juce::dontSendNotification);
    dampingLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(dampingLabel);
    dampingAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, "damping", dampingSlider));

    // Wet Level
    addAndMakeVisible(wetSlider);
    wetSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    wetSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    wetSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::white.withAlpha(0.9f));
    wetSlider.setColour(juce::Slider::thumbColourId, pink.darker(0.2f));
    wetLabel.setText("Wet", juce::dontSendNotification);
    wetLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(wetLabel);
    wetAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, "wetLevel", wetSlider));

    // Dry Level
    addAndMakeVisible(drySlider);
    drySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    drySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    drySlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::white.withAlpha(0.9f));
    drySlider.setColour(juce::Slider::thumbColourId, pink.darker(0.2f));
    dryLabel.setText("Dry", juce::dontSendNotification);
    dryLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(dryLabel);
    dryAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, "dryLevel", drySlider));

    // Width
    addAndMakeVisible(widthSlider);
    widthSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    widthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    widthSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::white.withAlpha(0.9f));
    widthSlider.setColour(juce::Slider::thumbColourId, pink.darker(0.2f));
    widthLabel.setText("Width", juce::dontSendNotification);
    widthLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(widthLabel);
    widthAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, "width", widthSlider));

    // Pink Mix
    addAndMakeVisible(pinkMixSlider);
    pinkMixSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    pinkMixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    pinkMixSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::white.withAlpha(0.9f));
    pinkMixSlider.setColour(juce::Slider::thumbColourId, pink.darker(0.2f));
    pinkMixLabel.setText("Pink Mix", juce::dontSendNotification);
    pinkMixLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(pinkMixLabel);
    pinkMixAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, "pinkMix", pinkMixSlider));
}

PinkReverbAudioProcessorEditor::~PinkReverbAudioProcessorEditor()
{
}

void PinkReverbAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (pink);

    // Title
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font("Arial Black", 28.0f, juce::Font::bold));
    g.drawFittedText("Pink Reverb", 0, 10, getWidth(), 40, juce::Justification::centred, 1);

    // Subtle logo
    g.setColour(pink.darker(0.2f));
    g.setFont(juce::Font("Arial", 18.0f, juce::Font::italic));
    g.drawFittedText("ריוורב בצבע ורוד", 0, getHeight() - 30, getWidth(), 20, juce::Justification::centred, 1);
}

void PinkReverbAudioProcessorEditor::resized()
{
    const int sliderW = 80;
    const int sliderH = 100;
    const int margin = 20;
    const int labelH = 20;
    const int y0 = 60;

    roomSizeSlider.setBounds(margin, y0, sliderW, sliderH);
    roomSizeLabel.setBounds(margin, y0 + sliderH, sliderW, labelH);

    dampingSlider.setBounds(margin + sliderW + margin, y0, sliderW, sliderH);
    dampingLabel.setBounds(margin + sliderW + margin, y0 + sliderH, sliderW, labelH);

    wetSlider.setBounds(margin + 2 * (sliderW + margin), y0, sliderW, sliderH);
    wetLabel.setBounds(margin + 2 * (sliderW + margin), y0 + sliderH, sliderW, labelH);

    drySlider.setBounds(margin + 3 * (sliderW + margin), y0, sliderW, sliderH);
    dryLabel.setBounds(margin + 3 * (sliderW + margin), y0 + sliderH, sliderW, labelH);

    widthSlider.setBounds(margin + sliderW / 2, y0 + sliderH + labelH + 20, sliderW, sliderH);
    widthLabel.setBounds(margin + sliderW / 2, y0 + 2 * sliderH + labelH + 20, sliderW, labelH);

    pinkMixSlider.setBounds(getWidth() - margin - sliderW - sliderW / 2, y0 + sliderH + labelH + 20, sliderW, sliderH);
    pinkMixLabel.setBounds(getWidth() - margin - sliderW - sliderW / 2, y0 + 2 * sliderH + labelH + 20, sliderW, labelH);
}
```

```cmake
#