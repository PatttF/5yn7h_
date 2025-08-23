// faithful_virtual_analog_engine.h
#pragma once
#include <cmath>
#include <algorithm>

// Faithful Plaits-style Virtual Analog Engine: dual oscillator, musical detune, morphable shape, pulse width, etc.
inline float clampf(float x, float a, float b) { return x < a ? a : (x > b ? b : x); }

class FaithfulVirtualAnalogEngine {
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
    void reset() { phase1 = 0.0f; phase2 = 0.25f; }
    // Stereo output
    void process(float& left, float& right) {
    // Gate ignored: always output sound
        static const float intervals[5] = {0.0f, 7.01f, 12.01f, 19.01f, 24.01f};
        float detune = harmonics * 4.0f;
        int idx = int(detune);
        float frac = detune - idx;
        float interval = intervals[idx] + (intervals[std::min(idx+1,4)] - intervals[idx]) * frac;
        float freq2 = frequency * std::pow(2.0f, interval / 12.0f);
        float shape1 = timbre * 1.5f;
        shape1 = clampf(shape1, 0.0f, 1.0f);
        float pw1 = 0.5f + (timbre - 0.66f) * 1.4f;
        pw1 = clampf(pw1, 0.5f, 0.99f);
        float shape2 = morph * 1.5f;
        shape2 = clampf(shape2, 0.0f, 1.0f);
        float pw2 = 0.5f + (morph - 0.66f) * 1.4f;
        pw2 = clampf(pw2, 0.5f, 0.99f);
        float inc1 = frequency / sampleRate;
        phase1 += inc1;
        if (phase1 >= 1.0f) phase1 -= 1.0f;
        float saw1 = 2.0f * (phase1 - 0.5f);
        float square1 = (phase1 < pw1 ? 1.0f : -1.0f);
        float out1 = (1.0f-shape1) * saw1 + shape1 * square1;
        float inc2 = freq2 / sampleRate;
        phase2 += inc2;
        if (phase2 >= 1.0f) phase2 -= 1.0f;
        float saw2 = 2.0f * (phase2 - 0.5f);
        float square2 = (phase2 < pw2 ? 1.0f : -1.0f);
        float out2 = (1.0f-shape2) * saw2 + shape2 * square2;
    // float out = 0.5f * (out1 + out2); // Removed unused variable
        left = out1 * level;
        right = out2 * level;
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
    float morph = 0.5f;
    float level = 1.0f;
    float lfoFreq = 0.0f;
    float lfoWave = 0.0f;
    float lfoVar = 0.0f;
    float attack = 0.01f, decay = 0.1f, sustain = 0.8f, release = 0.2f;
    bool gateOn = false;
    float phase1 = 0.0f;
    float phase2 = 0.25f;
};
