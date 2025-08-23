// fm_engine.h
#pragma once
#include <cmath>


class FMEngine {
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
    void reset() { phaseC = 0.0f; phaseM = 0.0f; env = 1.0f; }
    // Stereo output (identical L/R)
    void process(float& left, float& right) {
    // Gate ignored: always output sound
        float l = processMono();
        left = l * level;
        right = l * level;
    }
    // Mono process (for internal use)
    float processMono() {
        float ratio = 1.0f + harmonics * 5.0f;
        float feedback = timbre * 2.0f;
        float modIndex = 0.5f + morph * 7.5f;
        float phaseIncC = frequency / sampleRate;
        float phaseIncM = frequency * ratio / sampleRate;
        phaseC += phaseIncC;
        phaseM += phaseIncM;
        if (phaseC >= 1.0f) phaseC -= 1.0f;
        if (phaseM >= 1.0f) phaseM -= 1.0f;
        float mod = std::sin(2.0f * M_PI * (phaseM + feedback * lastOut));
        float shaper = mod - 0.2f * std::pow(mod,3);
        env *= 0.9995f;
        if (env < 0.05f) env = 1.0f;
        float idx = modIndex * env;
        float out = std::sin(2.0f * M_PI * phaseC + idx * shaper);
        float noise = ((float)rand() / RAND_MAX - 0.5f) * 0.008f;
        out = std::tanh(out * (1.0f + 0.2f * harmonics)) + noise;
        lastOut = out;
        return out * 0.98f;
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
    float phaseC = 0.0f, phaseM = 0.0f;
    float lastOut = 0.0f;
    float env = 1.0f;
};
