// supersaw_engine.h
#pragma once
#include <cmath>
#include <cstdlib>

class SuperSawEngine {
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
    void reset() {
        for (int i = 0; i < 9; ++i) phase[i] = float(rand()) / RAND_MAX;
    }
    // Stereo output
    void process(float& left, float& right) {
    // Gate ignored: always output sound
        static const float detune[9] = {0.0f, -0.018f, 0.018f, -0.045f, 0.045f, -0.09f, 0.09f, -0.14f, 0.14f};
        static const float pan[9] = {0.0f, -0.7f, 0.7f, -0.4f, 0.4f, -1.0f, 1.0f, -0.2f, 0.2f};
        float spread = 0.2f + 0.8f * timbre;
        float detuneAmt = 0.04f + 0.12f * harmonics;
        float sumL = 0.0f, sumR = 0.0f;
        for (int i = 0; i < 9; ++i) {
            float freq = frequency * (1.0f + detune[i] * detuneAmt);
            float inc = freq / sampleRate;
            phase[i] += inc;
            if (phase[i] >= 1.0f) phase[i] -= 1.0f;
            float saw = 2.0f * (phase[i] - 0.5f);
            float p = 0.5f + 0.5f * pan[i] * spread;
            sumL += saw * (1.0f - p);
            sumR += saw * p;
        }
        left = (sumL / 6.0f) * level;
        right = (sumR / 6.0f) * level;
        left += ((float)rand() / RAND_MAX - 0.5f) * 0.002f;
        right += ((float)rand() / RAND_MAX - 0.5f) * 0.002f;
        left = std::tanh(left * 0.8f);
        right = std::tanh(right * 0.8f);
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
    float harmonics = 0.5f;
    float timbre = 0.5f;
    float morph = 0.0f;
    float level = 1.0f;
    float lfoFreq = 0.0f;
    float lfoWave = 0.0f;
    float lfoVar = 0.0f;
    float attack = 0.01f, decay = 0.1f, sustain = 0.8f, release = 0.2f;
    bool gateOn = false;
    float phase[9] = {0};
};
