// sine_engine.h
#pragma once
#include <cmath>

// Improved SineEngine: better morph/timbre, DC blocking
class SineEngine {
public:
    void setSampleRate(float sr) { sampleRate = sr; }
    void setFrequency(float freq) { frequency = freq; }
    void setHarmonics(float h) { harmonics = h; }
    void setTimbre(float t) { timbre = t; }
    void setMorph(float m) { morph = m; }
    void reset() { phase = 0.0f; lastOut = 0.0f; dc = 0.0f; }
    void gate(bool g) { gateOn = g; }
    // Stereo process for compatibility
    void process(float& left, float& right) {
    float v = process();
    left = right = v * level;
    }
    // Mono process
    float process() {
        // Always output sound, ignore gate
        float phaseInc = frequency / sampleRate;
        phase += phaseInc;
        if (phase >= 1.0f) phase -= 1.0f;
        // Morph: blend sine and soft triangle
        float tri = 2.0f * fabs(2.0f * phase - 1.0f) - 1.0f;
        float out = (1.0f - morph) * std::sin(2.0f * M_PI * phase) + morph * tri;
        // Harmonics: add a little 2nd/3rd harmonic for color
        out += harmonics * 0.2f * std::sin(4.0f * M_PI * phase);
        out += harmonics * 0.1f * std::sin(6.0f * M_PI * phase);
        // Timbre: soft saturation
        out = std::tanh(out * (1.0f + timbre * 2.0f));
        // Simple DC blocker
        float dcBlock = out - lastOut + 0.995f * dc;
        lastOut = out;
        dc = dcBlock;
        return dcBlock * 0.9f;
    }
    // Level (amplitude)
    void setLevel(float l) { level = l; }
    float getLevel() const { return level; }
    void setLfoFreq(float) {}
    void setLfoWave(float) {}
    void setLfoVar(float) {}
    void setADSR(float, float, float, float) {}
    float getFrequency() const { return frequency; }
    float getHarmonics() const { return harmonics; }
    float getTimbre() const { return timbre; }
    float getMorph() const { return morph; }
    float getLfoFreq() const { return 0.0f; }
    float getLfoWave() const { return 0.0f; }
    float getLfoVar() const { return 0.0f; }
    void getADSR(float& a, float& d, float& s, float& r) const { a = d = s = r = 0.0f; }
private:
    bool gateOn = false;
private:
    float sampleRate = 48000.0f;
    float frequency = 440.0f;
    float harmonics = 0.0f;
    float timbre = 0.5f;
    float morph = 0.0f;
    float phase = 0.0f;
    float lastOut = 0.0f;
    float dc = 0.0f;
    float level = 1.0f;
};
