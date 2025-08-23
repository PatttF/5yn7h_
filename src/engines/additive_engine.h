// additive_engine.h
#pragma once
#include <cmath>


class AdditiveEngine {
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
    void reset() { for (int i=0; i<16; ++i) { phases[i] = 0.0f; phaseOffsets[i] = ((float)rand()/RAND_MAX); } }
    // Stereo output (identical L/R)
    void process(float& left, float& right) {
    // Gate ignored: always output sound
        float l = processMono();
        left = l * level;
        right = l * level;
    }
    // Mono process (for internal use)
    float processMono() {
        float out = 0.0f;
        int numHarm = 2 + int(harmonics * 14.0f);
        for (int i=0; i<numHarm; ++i) {
            float amp = 1.0f / std::pow(i+1, 1.0f + 0.7f * timbre);
            float env = 1.0f - morph * std::abs(std::sin(M_PI * phases[i]));
            float detune = 1.0f + 0.001f * (i - numHarm/2) * (0.5f + 0.5f * timbre);
            float freqMul = float(i+1) * detune;
            float phaseInc = frequency * freqMul / sampleRate;
            phases[i] += phaseInc;
            if (phases[i] >= 1.0f) phases[i] -= 1.0f;
            float ph = phases[i] + phaseOffsets[i];
            if (ph >= 1.0f) ph -= 1.0f;
            out += amp * env * std::sin(2.0f * M_PI * ph);
        }
        float noise = ((float)rand() / RAND_MAX - 0.5f) * 0.006f;
        out = std::tanh(out * 1.1f) + noise;
        return out * 0.7f;
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
    float phases[16] = {0};
    float phaseOffsets[16] = {0};
};
