#include <vector>
#include <algorithm>
#include <random>



// --- Improved Delay Effect: Longer, lowpass feedback, better wet/dry ---
struct ImprovedDelay {
    std::vector<float> buf;
    size_t idx = 0;
    float lastOut = 0.0f;
    float lp = 0.0f;
    ImprovedDelay() : buf(48000, 0.0f) {} // up to 1s at 48kHz
    void reset() { std::fill(buf.begin(), buf.end(), 0.0f); idx = 0; lp = 0.0f; }
    float process(float in, float amount, float sampleRate) {
        // Delay time: 40ms to 1s
        float maxDelay = 1.0f * sampleRate;
        float minDelay = 0.04f * sampleRate;
        size_t delaySamps = static_cast<size_t>(minDelay + (maxDelay - minDelay) * amount);
        size_t readIdx = (idx + buf.size() - delaySamps) % buf.size();
        float delayed = buf[readIdx];
        // Simple lowpass in feedback
        lp = 0.7f * lp + 0.3f * delayed;
        buf[idx] = in + lp * (0.45f + 0.45f * amount); // more feedback at higher amount
        idx = (idx + 1) % buf.size();
        // Wet/dry
        return in * (1.0f - amount) + delayed * amount;
    }
};

// --- Improved Chorus Effect: Multi-voice, LFO smoothing, interpolation ---
#include <array>
struct ImprovedChorus {
    static constexpr int voices = 3;
    std::vector<float> buf;
    size_t idx = 0;
    std::array<float, voices> phase{{0.0f, 2.1f, 4.2f}};
    ImprovedChorus() : buf(5120, 0.0f) {} // up to ~107ms, with margin
    void reset() { std::fill(buf.begin(), buf.end(), 0.0f); idx = 0; phase = {0.0f, 2.1f, 4.2f}; }
    float process(float in, float amount, float sampleRate) {
        float out = 0.0f;
        float lfoRate = 0.25f + 1.5f * amount;
        float lfoDepth = 80.0f + 320.0f * amount; // 1.7–8ms
        size_t bufsize = buf.size();
        for (int v = 0; v < voices; ++v) {
            phase[v] += lfoRate * (1.0f + 0.2f * v) * 2.0f * 3.14159f / sampleRate;
            if (phase[v] > 2.0f * 3.14159f) phase[v] -= 2.0f * 3.14159f;
            float mod = (std::sin(phase[v]) + 1.0f) * 0.5f;
            float delaySamps = lfoDepth * mod;
            // Clamp delaySamps to safe range (1 to bufsize-2)
            if (delaySamps < 1.0f) delaySamps = 1.0f;
            if (delaySamps > bufsize - 2) delaySamps = bufsize - 2;
            size_t readPos = (idx + bufsize - (size_t)delaySamps) % bufsize;
            size_t nextPos = (readPos + 1) % bufsize;
            float frac = delaySamps - (size_t)delaySamps;
            // Avoid wraparound glitches by not interpolating across buffer edge
            if (nextPos == 0 && readPos != 0) nextPos = readPos; // hold at edge
            float delayed = buf[readPos] * (1.0f - frac) + buf[nextPos] * frac;
            out += delayed;
        }
        out /= voices;
        buf[idx] = in;
        idx = (idx + 1) % bufsize;
        return in * (1.0f - amount) + out * amount;
    }
};

// DistrhoPlugin5yn7h_.cpp
#include "5yn7h_.hpp"
#include <cstdint>
#include <cstdio>
#include <cmath>
#include "engines/sine_engine.h"
#include "engines/triangle_engine.h"
#include "engines/square_engine.h"
#include "engines/saw_engine.h"

#include "engines/adsr_peaks.h"
#include "engines/ad_peaks.h"
#include "engines/lfo_peaks.h"
#include "engines/supersaw_engine.h"
#include "engines/faithful_virtual_analog_engine.h"
#include "engines/fm_engine.h"
#include "engines/formant_engine.h"
#include "engines/additive_engine.h"
#include "engines/chord_engine.h"


#include "engines/string_engine.h"
#include "engines/pwm_engine.h"

#include "moog_filter.hpp"

START_NAMESPACE_DISTRHO

class Plugin5yn7h_ : public Plugin {
    MoogFilter moogL, moogR;
    ImprovedDelay delayL, delayR;
    ImprovedChorus chorusL, chorusR;
    // Improved Schroeder/Moorer reverb buffers and state
    static constexpr int numCombs = 4, numAllpasses = 2;
    std::vector<float> combBufL[numCombs], combBufR[numCombs];
    size_t combIdxL[numCombs] = {0}, combIdxR[numCombs] = {0};
    float combFeedback[numCombs] = {0};
    std::vector<float> allpassBufL[numAllpasses], allpassBufR[numAllpasses];
    size_t allpassIdxL[numAllpasses] = {0}, allpassIdxR[numAllpasses] = {0};
private:
    float sampleRate;
    float paramValues[kParamCount];
    SineEngine sine;
    TriangleEngine triangle;
    SquareEngine square;
    SawEngine saw;
    SuperSawEngine supersaw;
    FaithfulVirtualAnalogEngine va;
    FMEngine fm;
    FormantEngine formant;
    AdditiveEngine additive;
    ChordEngine chord;
    StringEngine stringRes;
    PWMEngine pwm;
    PeaksADSR adsr;
    PeaksAD ad;
    PeaksLFO lfo;
    int currentNote;
    bool noteHeld;
    float midiFreq;
public:
    Plugin5yn7h_() : Plugin(kParamCount, 0, 0), moogL(48000.0f), moogR(48000.0f) {
    paramValues[kParamFilterWet] = 0.0f; // Default to fully dry
        sampleRate = 48000.0f;
        // Set up all engines
        sine.setSampleRate(sampleRate);
        triangle.setSampleRate(sampleRate);
        square.setSampleRate(sampleRate);
        saw.setSampleRate(sampleRate);
        adsr.setSampleRate(sampleRate);
        ad.setSampleRate(sampleRate);
        lfo.setSampleRate(sampleRate);
        // Set musical, audible defaults for all parameters
    for (int i = 0; i < kParamCount; ++i) paramValues[i] = 0.0f;
    paramValues[kParamReverb] = 0.0f;
    paramValues[kParamDelay] = 0.0f;
    paramValues[kParamChorus] = 0.0f;
    paramValues[kParamModel] = 0.0f;      // Sine engine
    paramValues[kParamHarmonics] = 0.5f;
    paramValues[kParamTimbre] = 0.5f;
    paramValues[kParamMorph] = 0.5f;
    paramValues[kParamFreq] = 261.63f; // C4, Plaits default
    paramValues[kParamLevel] = 0.8f;
    paramValues[kParamFilterCutoff] = 0.5f; // Default cutoff (normalized)
    paramValues[kParamFilterResonance] = 0.0f; // Default resonance
        // Peaks ADSR/AD defaults
        paramValues[kParamAttack] = 0.01f;
        paramValues[kParamDecay] = 0.1f;
        paramValues[kParamSustain] = 0.7f;
        paramValues[kParamRelease] = 0.2f;
        // Peaks LFO defaults
        paramValues[kParamLfoFreq] = 1.0f;
        paramValues[kParamLfoWave] = 0.0f;
        paramValues[kParamLfoVar] = 0.0f;

    // Init improved reverb buffers (comb: 4 delays, allpass: 2 delays)
    const int combLens[4] = {1116, 1188, 1277, 1356}; // prime lengths for diffusion
    const int allpassLens[2] = {225, 556};
    for (int i = 0; i < numCombs; ++i) {
        combBufL[i].assign(combLens[i], 0.0f);
        combBufR[i].assign(combLens[i], 0.0f);
        combIdxL[i] = combIdxR[i] = 0;
        combFeedback[i] = 0.0f;
    }
    for (int i = 0; i < numAllpasses; ++i) {
        allpassBufL[i].assign(allpassLens[i], 0.0f);
        allpassBufR[i].assign(allpassLens[i], 0.0f);
        allpassIdxL[i] = allpassIdxR[i] = 0;
    }
    // Set all shared parameters for all engines at startup
    float harmonics = paramValues[kParamHarmonics];
    float timbre = paramValues[kParamTimbre];
    float morph = paramValues[kParamMorph];
    float freq = paramValues[kParamFreq];
    float level = paramValues[kParamLevel];
    sine.setFrequency(freq); sine.setHarmonics(harmonics); sine.setTimbre(timbre); sine.setMorph(morph); sine.setLevel(level);
    triangle.setFrequency(freq); triangle.setHarmonics(harmonics); triangle.setTimbre(timbre); triangle.setMorph(morph); triangle.setLevel(level);
    square.setFrequency(freq); square.setHarmonics(harmonics); square.setTimbre(timbre); square.setMorph(morph); square.setLevel(level);
    saw.setFrequency(freq); saw.setHarmonics(harmonics); saw.setTimbre(timbre); saw.setMorph(morph); saw.setLevel(level);
    supersaw.setFrequency(freq); supersaw.setHarmonics(harmonics); supersaw.setTimbre(timbre); supersaw.setMorph(morph); supersaw.setLevel(level);
    va.setFrequency(freq); va.setHarmonics(harmonics); va.setTimbre(timbre); va.setMorph(morph); va.setLevel(level);
    fm.setFrequency(freq); fm.setHarmonics(harmonics); fm.setTimbre(timbre); fm.setMorph(morph); fm.setLevel(level);
    formant.setFrequency(freq); formant.setHarmonics(harmonics); formant.setTimbre(timbre); formant.setMorph(morph); formant.setLevel(level);
    additive.setFrequency(freq); additive.setHarmonics(harmonics); additive.setTimbre(timbre); additive.setMorph(morph); additive.setLevel(level);
    chord.setFrequency(freq); chord.setHarmonics(harmonics); chord.setTimbre(timbre); chord.setMorph(morph); chord.setLevel(level);
    stringRes.setFrequency(freq); stringRes.setHarmonics(harmonics); stringRes.setTimbre(timbre); stringRes.setMorph(morph); stringRes.setLevel(level);
    }

    // Provide engine names for the engine parameter for host combo box
    bool getParameterValueString(uint32_t index, float value, char* str) const {
        if (index == kParamModel) {
            int v = int(value + 0.5f);
            extern const char* kEngineNames[];
            if (v < 0) v = 0;
            if (v >= kNumEngines) v = kNumEngines - 1;
            std::strncpy(str, kEngineNames[v], 127);
            str[127] = '\0';
            return true;
        }
        return false;
    }

    const char* getLabel() const override { return "5yn7h_"; }
    const char* getMaker() const override { return "PatF"; }
    const char* getLicense() const override { return "MIT"; }
    uint32_t getVersion() const override { return 0x10000; }
    int64_t getUniqueId() const override { return 0x53794E37; } // 'SyN7'

    void initParameter(uint32_t index, Parameter& parameter) override {
        parameter.hints = kParameterIsAutomatable;
        switch (index) {
        case kParamFilterWet:
            parameter.name = "Filter Wet";
            parameter.symbol = "filter_wet";
            parameter.unit = "";
            parameter.ranges.def = 0.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 1.0f;
            break;
        case kParamFreq:
            parameter.name = "Frequency";
            parameter.symbol = "freq";
            parameter.unit = "Hz";
            parameter.ranges.def = 440.0f;
            parameter.ranges.min = 8.0f;
            parameter.ranges.max = 8000.0f;
            break;
        case kParamReverb:
            parameter.name = "Reverb";
            parameter.symbol = "reverb";
            parameter.unit = "";
            parameter.ranges.def = 0.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 1.0f;
            break;
        case kParamDelay:
            parameter.name = "Delay";
            parameter.symbol = "delay";
            parameter.unit = "";
            parameter.ranges.def = 0.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 1.0f;
            break;
        case kParamChorus:
            parameter.name = "Chorus";
            parameter.symbol = "chorus";
            parameter.unit = "";
            parameter.ranges.def = 0.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 1.0f;
            break;
        case kParamLfoFreq:
            parameter.name = "LFO Freq";
            parameter.symbol = "lfo_freq";
            parameter.unit = "Hz";
            parameter.ranges.def = 1.0f;
            parameter.ranges.min = 0.01f;
            parameter.ranges.max = 40.0f;
            break;
        case kParamModel:
            parameter.name = "Engine";
            parameter.symbol = "engine";
            parameter.unit = "";
            parameter.ranges.def = 0.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = float(kNumEngines - 1);
            parameter.hints |= kParameterIsInteger | kParameterIsAutomatable;
            break;
        case kParamHarmonics:
            parameter.name = "Harmonics";
            parameter.symbol = "harmonics";
            parameter.unit = "";
            parameter.ranges.def = 0.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 1.0f;
            break;
        case kParamTimbre:
            parameter.name = "Timbre";
            parameter.symbol = "timbre";
            parameter.unit = "";
            parameter.ranges.def = 0.5f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 1.0f;
            break;
        case kParamMorph:
            parameter.name = "Morph";
            parameter.symbol = "morph";
            parameter.unit = "";
            parameter.ranges.def = 0.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 1.0f;
            break;
    // Removed kParamFreq
        case kParamLevel:
            parameter.name = "Level";
            parameter.symbol = "level";
            parameter.unit = "";
            parameter.ranges.def = 0.8f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 1.0f;
            break;
        case kParamAttack:
            parameter.name = "Attack";
            parameter.symbol = "attack";
            parameter.unit = "s";
            parameter.ranges.def = 0.01f;
            parameter.ranges.min = 0.001f;
            parameter.ranges.max = 4.0f;
            break;
        case kParamDecay:
            parameter.name = "Decay";
            parameter.symbol = "decay";
            parameter.unit = "s";
            parameter.ranges.def = 0.1f;
            parameter.ranges.min = 0.001f;
            parameter.ranges.max = 4.0f;
            break;
        case kParamSustain:
            parameter.name = "Sustain";
            parameter.symbol = "sustain";
            parameter.unit = "";
            parameter.ranges.def = 0.7f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 1.0f;
            break;
        case kParamRelease:
            parameter.name = "Release";
            parameter.symbol = "release";
            parameter.unit = "s";
            parameter.ranges.def = 1.0f;
            parameter.ranges.min = 0.01f;
            parameter.ranges.max = 40.0f;
            break;
        case kParamLfoWave:
            parameter.name = "LFO Wave";
            parameter.symbol = "lfo_wave";
            parameter.unit = "";
            parameter.ranges.def = 0.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 4.0f;
            parameter.hints |= kParameterIsInteger;
            break;
        case kParamLfoVar:
            parameter.name = "LFO Var";
            parameter.symbol = "lfo_var";
            parameter.unit = "";
            parameter.ranges.def = 0.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 1.0f;
            break;
        case kParamFilterCutoff:
            parameter.name = "Filter Cutoff";
            parameter.symbol = "filter_cutoff";
            parameter.unit = "Hz";
            parameter.ranges.def = 0.5f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 1.0f;
            break;
        case kParamFilterResonance:
            parameter.name = "Filter Resonance";
            parameter.symbol = "filter_resonance";
            parameter.unit = "";
            parameter.ranges.def = 0.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 1.0f;
            break;
        default:
            break;
        }
    }

    float getParameterValue(uint32_t index) const override {
    if (index < kParamCount) return paramValues[index];
    return 0.0f;
    }

    void setParameterValue(uint32_t index, float value) override {
    if (index < kParamCount) paramValues[index] = value;
    }

    void activate() override {
        // Reset all engines
        sine.reset();
        triangle.reset();
        square.reset();
        saw.reset();
        adsr.reset();
        ad.reset();
        lfo.reset();
        delayL.reset(); delayR.reset();
        chorusL.reset(); chorusR.reset();
        for (int i = 0; i < numCombs; ++i) {
            std::fill(combBufL[i].begin(), combBufL[i].end(), 0.0f);
            std::fill(combBufR[i].begin(), combBufR[i].end(), 0.0f);
            combIdxL[i] = combIdxR[i] = 0;
            combFeedback[i] = 0.0f;
        }
        for (int i = 0; i < numAllpasses; ++i) {
            std::fill(allpassBufL[i].begin(), allpassBufL[i].end(), 0.0f);
            std::fill(allpassBufR[i].begin(), allpassBufR[i].end(), 0.0f);
            allpassIdxL[i] = allpassIdxR[i] = 0;
        }
    }

    void run(const float** inputs, float** outputs, uint32_t frames, const MidiEvent* midiEvents, uint32_t midiEventCount) override {
        (void)inputs;
        // MIDI event handling
        for (uint32_t e = 0; e < midiEventCount; ++e) {
            const MidiEvent& ev = midiEvents[e];
            if ((ev.data[0] & 0xF0) == 0x90 && ev.data[2] > 0) { // Note On
                currentNote = ev.data[1];
                midiFreq = 440.0f * std::pow(2.0f, (currentNote - 69) / 12.0f);
                noteHeld = true;
                adsr.gateOn();
                ad.gateOn();
            } else if (((ev.data[0] & 0xF0) == 0x80) || ((ev.data[0] & 0xF0) == 0x90 && ev.data[2] == 0)) { // Note Off
                if (currentNote == ev.data[1]) {
                    noteHeld = false;
                    adsr.gateOff();
                    ad.gateOff();
                }
            }
        }

    // Get engine/model selection
    int model = static_cast<int>(paramValues[kParamModel] + 0.5f);
    if (model < 0) model = 0;
    if (model >= kNumEngines) model = kNumEngines - 1;

        // Set shared parameters
        float harmonics = paramValues[kParamHarmonics];
        float timbre = paramValues[kParamTimbre];
        float morph = paramValues[kParamMorph];
    // float freq = paramValues[kParamFreq]; // Removed unused variable
        float level = paramValues[kParamLevel];

        // Peaks params
        float attack = paramValues[kParamAttack];
        float decay = paramValues[kParamDecay];
        float sustain = paramValues[kParamSustain];
        float release = paramValues[kParamRelease];
        float lfoFreq = paramValues[kParamLfoFreq];
        float lfoWave = paramValues[kParamLfoWave];
        float lfoVar = paramValues[kParamLfoVar];

        // Envelope and LFO setup
    adsr.setAttack(attack);
    adsr.setDecay(decay);
    adsr.setSustain(sustain);
    adsr.setRelease(release);
    // ad.setAttack(attack); // PeaksAD does not have setAttack
    // ad.setDecay(decay);  // PeaksAD does not have setDecay
    lfo.setFrequency(lfoFreq);
    lfo.setWaveform(static_cast<PeaksLFO::Waveform>(static_cast<int>(lfoWave)));
    lfo.setVariation(lfoVar);



    float reverbAmount = paramValues[kParamReverb];
    float delayAmt = paramValues[kParamDelay];
    float chorusAmt = paramValues[kParamChorus];
    static bool wasSilent = true;
    for (uint32_t i = 0; i < frames; ++i) {
    float dryL = 0.0f, dryR = 0.0f;
    // --- Moog filter before effects ---
    float filterCutoff = paramValues[kParamFilterCutoff];
    float filterResonance = paramValues[kParamFilterResonance];
    float cutoffHz = 40.0f + filterCutoff * (18000.0f - 40.0f);
    moogL.setCutoff(cutoffHz);
    moogR.setCutoff(cutoffHz);
    moogL.setResonance(filterResonance);
    moogR.setResonance(filterResonance);
        float env = adsr.process();
        float lfoVal = lfo.process();
    // Use midiFreq for all engines so each note plays the correct pitch
    float noteFreq = midiFreq;
        bool silent = (env <= 0.0001f);
        if (silent && !wasSilent) { delayL.reset(); delayR.reset(); chorusL.reset(); chorusR.reset(); }
        wasSilent = silent;
    // Maximally robust engine selection: clamp, round, always allow engine 0
    float rawModel = paramValues[kParamModel];
    // Clamp to valid range before rounding
    if (rawModel < 0.0f) rawModel = 0.0f;
    if (rawModel > float(kNumEngines - 1)) rawModel = float(kNumEngines - 1);
    int modelIdx = int(std::round(rawModel));
    // If value is very close to zero, always select engine 0
    if (paramValues[kParamModel] >= -0.5f && paramValues[kParamModel] < 0.5f) modelIdx = 0;
    // Fallback: if out of range, select engine 0
    if (modelIdx < 0 || modelIdx >= kNumEngines) modelIdx = 0;
    switch (modelIdx) {
            case 0: sine.setFrequency(noteFreq); sine.setHarmonics(harmonics); sine.setTimbre(timbre); sine.setMorph(morph); dryL = dryR = sine.process(); break;
            case 1: triangle.setFrequency(noteFreq); triangle.setHarmonics(harmonics); triangle.setTimbre(timbre); triangle.setMorph(morph); dryL = dryR = triangle.process(); break;
            case 2: square.setFrequency(noteFreq); square.setHarmonics(harmonics); square.setTimbre(timbre); square.setMorph(morph); dryL = dryR = square.process(); break;
            case 3: saw.setFrequency(noteFreq); saw.setHarmonics(harmonics); saw.setTimbre(timbre); saw.setMorph(morph); dryL = dryR = saw.process(); break;
            case 4: supersaw.setSampleRate(sampleRate); supersaw.setFrequency(noteFreq); supersaw.setHarmonics(harmonics); supersaw.setTimbre(timbre); supersaw.setMorph(morph); supersaw.process(dryL, dryR); break;
            case 5: va.setSampleRate(sampleRate); va.setFrequency(noteFreq); va.setHarmonics(harmonics); va.setTimbre(timbre); va.setMorph(morph); va.process(dryL, dryR); break;
            case 6: fm.setFrequency(noteFreq); fm.setHarmonics(harmonics); fm.setTimbre(timbre); fm.setMorph(morph); fm.process(dryL, dryR); break;
            case 7: formant.setFrequency(noteFreq); formant.setHarmonics(harmonics); formant.setTimbre(timbre); formant.setMorph(morph); formant.process(dryL, dryR); break;
            case 8: additive.setFrequency(noteFreq); additive.setHarmonics(harmonics); additive.setTimbre(timbre); additive.setMorph(morph); additive.process(dryL, dryR); break;
            case 9: chord.setFrequency(noteFreq); chord.setHarmonics(harmonics); chord.setTimbre(timbre); chord.setMorph(morph); chord.process(dryL, dryR); break;
            case 10: stringRes.setSampleRate(sampleRate); stringRes.setFrequency(noteFreq); stringRes.setHarmonics(harmonics); stringRes.setTimbre(timbre); stringRes.setMorph(morph); stringRes.process(dryL, dryR); break;
            case 11: pwm.setSampleRate(sampleRate); pwm.setFrequency(noteFreq); pwm.setHarmonics(harmonics); pwm.setTimbre(timbre); pwm.setMorph(morph); pwm.setLevel(level); pwm.process(dryL, dryR); break;
            default: dryL = dryR = 0.0f; break;
        }
        float lfoMod = 1.0f + 0.2f * lfoVal;
        dryL = dryL * env * level * lfoMod;
        dryR = dryR * env * level * lfoMod;
    // --- Process through Moog filter and blend wet/dry ---
    float filterWet = paramValues[kParamFilterWet];
    float filteredL = moogL.process(dryL);
    float filteredR = moogR.process(dryR);
    dryL = dryL * (1.0f - filterWet) + filteredL * filterWet;
    dryR = dryR * (1.0f - filterWet) + filteredR * filterWet;
    // --- Apply delay and chorus effects ---
    dryL = delayL.process(dryL, delayAmt, sampleRate);
    dryR = delayR.process(dryR, delayAmt, sampleRate);
    dryL = chorusL.process(dryL, chorusAmt, sampleRate);
    dryR = chorusR.process(dryR, chorusAmt, sampleRate);
        // --- Improved Schroeder/Moorer reverb ---
        // 4 parallel combs per channel
        float combOutL = 0.0f, combOutR = 0.0f;
        float combFeedbackAmt = 0.75f + 0.22f * reverbAmount; // 0.75-0.97
        for (int c = 0; c < numCombs; ++c) {
            float inL = dryL + combFeedback[c] * combBufL[c][combIdxL[c]];
            float inR = dryR + combFeedback[c] * combBufR[c][combIdxR[c]];
            combOutL += combBufL[c][combIdxL[c]];
            combOutR += combBufR[c][combIdxR[c]];
            combFeedback[c] = combFeedbackAmt;
            combBufL[c][combIdxL[c]] = inL;
            combBufR[c][combIdxR[c]] = inR;
            combIdxL[c] = (combIdxL[c] + 1) % combBufL[c].size();
            combIdxR[c] = (combIdxR[c] + 1) % combBufR[c].size();
        }
        combOutL /= numCombs;
        combOutR /= numCombs;
        // 2 series allpasses per channel
        float apL = combOutL, apR = combOutR;
        for (int a = 0; a < numAllpasses; ++a) {
            float bufOutL = allpassBufL[a][allpassIdxL[a]];
            float bufOutR = allpassBufR[a][allpassIdxR[a]];
            float inL = apL + 0.5f * bufOutL;
            float inR = apR + 0.5f * bufOutR;
            allpassBufL[a][allpassIdxL[a]] = inL;
            allpassBufR[a][allpassIdxR[a]] = inR;
            apL = -0.5f * inL + bufOutL;
            apR = -0.5f * inR + bufOutR;
            allpassIdxL[a] = (allpassIdxL[a] + 1) % allpassBufL[a].size();
            allpassIdxR[a] = (allpassIdxR[a] + 1) % allpassBufR[a].size();
        }
        // Mix dry and wet
        float outL = dryL * (1.0f - reverbAmount) + apL * reverbAmount;
        float outR = dryR * (1.0f - reverbAmount) + apR * reverbAmount;
        outputs[0][i] = outL;
        if (outputs[1]) outputs[1][i] = outR;
    }
    }
};

Plugin* createPlugin() { return new Plugin5yn7h_(); }

END_NAMESPACE_DISTRHO
