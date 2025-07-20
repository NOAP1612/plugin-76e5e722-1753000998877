CMakeLists.txt

cmake_minimum_required(VERSION 3.15)

project(PinkReverb VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# JUCE setup
find_package(JUCE CONFIG REQUIRED)

juce_add_plugin(PinkReverb
    COMPANY_NAME "PinkAudio"
    PLUGIN_MANUFACTURER_CODE Pnka
    PLUGIN_CODE Prvb
    FORMATS VST3 AU
    PRODUCT_NAME "Pink Reverb"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
)

target_sources(PinkReverb
    PRIVATE
        PluginProcessor.cpp
        PluginEditor.cpp
        PluginProcessor.h
        PluginEditor.h
)

target_link_libraries(PinkReverb
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
)
```