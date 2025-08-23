// square_engine.h
#pragma once
#include <cmath>

// Improved SquareEngine: PolyBLEP, variable pulse width, DC blocking
class SquareEngine {
public:
    void setSampleRate(float sr) { sampleRate = sr; }
    void setFrequency(float freq) { frequency = freq; }
    void setHarmonics(float h) { harmonics = h; }
    void setTimbre(float t) { timbre = t; }
    void setMorph(float m) { morph = m; }
    void reset() { phase = 0.0f; lastOut = 0.0f; dc = 0.0f; }
        void gate(bool g) { gateOn = g; }
        bool gateOn = false;
    // Stereo process for compatibility
    void process(float& left, float& right) {
        float v = process();
        left = right = v * level;
    }
    float process() {
        float phaseInc = frequency / sampleRate;
        phase += phaseInc;
        if (phase >= 1.0f) phase -= 1.0f;
        float pw = 0.5f + 0.49f * timbre; // pulse width
        float tphase = phase;
        float sq = (tphase < pw ? 1.0f : -1.0f);
        // PolyBLEP for both edges
        sq -= polyblep(tphase, phaseInc);
        sq += polyblep(fmod(tphase + 1.0f - pw, 1.0f), phaseInc);
        // Harmonics: add a little 3rd/5th for color
        sq += harmonics * 0.12f * std::sin(6.0f * M_PI * phase);
        sq += harmonics * 0.07f * std::sin(10.0f * M_PI * phase);
        // Morph: softens the edge (wavefolding)
        sq = std::tanh(sq * (1.0f + morph * 2.0f));
        // DC blocker
        float dcBlock = sq - lastOut + 0.995f * dc;
        lastOut = sq;
        dc = dcBlock;
        return dcBlock * 0.9f;
    }
    // Level (amplitude)
    void setLevel(float l) { level = l; }
    float getLevel() const { return level; }
    // LFO (no-op for now)
    void setLfoFreq(float) {}
    void setLfoWave(float) {}
    void setLfoVar(float) {}
    float getLfoFreq() const { return 0.0f; }
    float getLfoWave() const { return 0.0f; }
    float getLfoVar() const { return 0.0f; }
    // ADSR (no-op for now)
    void setADSR(float, float, float, float) {}
    // Parameter getters
    float getFrequency() const { return frequency; }
    float getHarmonics() const { return harmonics; }
    float getTimbre() const { return timbre; }
    float getMorph() const { return morph; }
    void getADSR(float& a, float& d, float& s, float& r) const { a = d = s = r = 0.0f; }
private:
    float polyblep(float t, float dt) {
        if (t < dt) {
            t /= dt;
            return t + t - t * t - 1.0f;
        } else if (t > 1.0f - dt) {
            t = (t - 1.0f) / dt;
            return t * t + t + t + 1.0f;
        } else {
            return 0.0f;
        }
    }
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
