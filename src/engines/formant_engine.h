// formant_engine.h
#pragma once
#include <cmath>


class FormantEngine {
public:
    void setSampleRate(float sr) { sampleRate = sr; }
    void setFrequency(float freq) { frequency = freq; }
    void setHarmonics(float h) { harmonics = h; }
    void setTimbre(float t) { timbre = t; }
    void setMorph(float m) { morph = m; }
    void setLevel(float l) { level = l; }
    void setLfoFreq(float f) { lfoFreq = f; }
    void setLfoWave(float w) { lfoWave = w; }
    void setLfoVar(float v) { lfoVar = v; }
    void setADSR(float a, float d, float s, float r) { attack = a; decay = d; sustain = s; release = r; }
    void gate(bool g) { gateOn = g; }
    void reset() { phase = 0.0f; }
    // Stereo output (identical L/R)
    void process(float& left, float& right) {
    // Gate ignored: always output sound
        float l = processMono();
        left = l * level;
        right = l * level;
    }
    // Mono process (for internal use)
    float processMono() {
        static const float formantTable[5][3] = {
            {800.0f, 1150.0f, 2900.0f},
            {400.0f, 1600.0f, 2700.0f},
            {350.0f, 1700.0f, 2700.0f},
            {450.0f, 800.0f, 2830.0f},
            {325.0f, 700.0f, 2530.0f}
        };
        static const float bwTable[3] = {80.0f, 90.0f, 120.0f};
        static const float ampTable[3] = {1.0f, 0.6f, 0.3f};
        float v = timbre * 4.0f;
        int v0 = int(v);
        int v1 = (v0 < 4) ? v0+1 : 4;
        float frac = v - v0;
        float f[3];
        for (int i=0; i<3; ++i) {
            f[i] = formantTable[v0][i] * (1.0f-frac) + formantTable[v1][i] * frac;
        }
        float phaseInc = frequency / sampleRate;
        phase += phaseInc;
        if (phase >= 1.0f) phase -= 1.0f;
        float out = 0.0f;
        for (int i=0; i<3; ++i) {
            float bw = bwTable[i] * (1.0f + 0.5f * morph);
            float amp = ampTable[i] * (1.0f - harmonics * 0.5f * i);
            float env = std::exp(-bw * std::abs(std::sin(M_PI * phase)) / frequency);
            out += amp * env * std::sin(2.0f * M_PI * f[i] * phase / frequency);
        }
        float noise = ((float)rand() / RAND_MAX - 0.5f) * 0.008f;
        out = std::tanh(out * 1.1f) + noise;
        return out * 0.95f;
    }
    // Parameter getters
    float getSampleRate() const { return sampleRate; }
    float getFrequency() const { return frequency; }
    float getHarmonics() const { return harmonics; }
    float getTimbre() const { return timbre; }
    float getMorph() const { return morph; }
    float getLevel() const { return level; }
    float getLfoFreq() const { return lfoFreq; }
    float getLfoWave() const { return lfoWave; }
    float getLfoVar() const { return lfoVar; }
    void getADSR(float& a, float& d, float& s, float& r) const { a = attack; d = decay; s = sustain; r = release; }
    bool isGateOn() const { return gateOn; }
private:
    float sampleRate = 48000.0f;
    float frequency = 440.0f;
    float harmonics = 0.0f;
    float timbre = 0.5f;
    float morph = 0.0f;
    float level = 1.0f;
    float lfoFreq = 0.0f;
    float lfoWave = 0.0f;
    float lfoVar = 0.0f;
    float attack = 0.01f, decay = 0.1f, sustain = 0.8f, release = 0.2f;
    bool gateOn = false;
    float phase = 0.0f;
};
