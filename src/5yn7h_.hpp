// 5yn7h_.hpp
#pragma once
#define DISTRHO_PLUGIN_WANT_MIDI_INPUT 1
#include "DistrhoPlugin.hpp"




[[maybe_unused]] static const char* kEngineNames[] = {
    "Sine",              // 0
    "Triangle",          // 1
    "Square",            // 2
    "Saw",               // 3
    "SuperSaw",          // 4
    "Virtual Analog",    // 5
    "FM/Phase Mod",      // 6
    "Formant",           // 7
    "Additive",          // 8
    "Chord",             // 9
    "String/Resonator",  // 10
    "PWM"                // 11
};
static constexpr int kNumEngines = sizeof(kEngineNames) / sizeof(kEngineNames[0]);


enum Parameters {
    kParamModel,      // Engine/model selector
    kParamHarmonics,  // Shared: harmonics
    kParamTimbre,     // Shared: timbre
    kParamMorph,      // Shared: morph
    kParamFreq,       // Shared: frequency
    kParamLevel,      // Shared: output level
    // Peaks ADSR/AD/LFO parameters
    kParamAttack,     // Peaks envelope attack
    kParamDecay,      // Peaks envelope decay
    kParamSustain,    // Peaks envelope sustain
    kParamRelease,    // Peaks envelope release
    kParamLfoFreq,    // Peaks LFO frequency
    kParamLfoWave,    // Peaks LFO waveform
    kParamLfoVar,     // Peaks LFO waveform variation
    kParamReverb,     // Basic reverb amount
    kParamDelay,      // Delay amount
    kParamChorus,     // Chorus amount
    kParamFilterCutoff, // Moog filter cutoff
    kParamFilterResonance, // Moog filter resonance
    kParamFilterWet, // Moog filter wet/dry
    kParamCount
};
