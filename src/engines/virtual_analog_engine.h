// virtual_analog_engine.h
#pragma once
#include <cmath>


// PolyBLEP helper for band-limited saw/square
inline float polyblep(float t, float dt) {
    if (t < dt) {
        float x = t / dt;
        return x + x - x * x - 1.0f;
    } else if (t > 1.0f - dt) {
        float x = (t - 1.0f) / dt;
        return x * x + x + x + 1.0f;
    } else {
        return 0.0f;
    }
}


class VirtualAnalogEngine {
public:
    void setSampleRate(float sr) { sampleRate = sr; }
    void setFrequency(float freq) { frequency = freq; }
    void setHarmonics(float h) { harmonics = h; }
    void setTimbre(float t) { timbre = t; }
    void setMorph(float m) { morph = m; }
    void reset() { phase = 0.0f; gateOn = false; }
    void gate(bool on) { gateOn = on; }
    // Plaits-style process: morph = saw <-> square, timbre = wavefold, harmonics = brightness
    float process() {
        if (!gateOn) return 0.0f;
        float phaseInc = frequency / sampleRate;
        phase += phaseInc;
        if (phase >= 1.0f) phase -= 1.0f;
        // Band-limited saw
        float saw = 2.0f * (phase - 0.5f);
        saw -= polyblep(phase, phaseInc);
        // Band-limited square
        float pw = 0.5f + 0.48f * timbre; // 0.5-0.98
        float square = (phase < pw ? 1.0f : -1.0f);
        square += polyblep(phase, phaseInc);
        float t2 = phase + 1.0f - pw;
        if (t2 >= 1.0f) t2 -= 1.0f;
        square -= polyblep(t2, phaseInc);
        // Morph: crossfade saw <-> square
        float out = (1.0f - morph) * saw + morph * square;
        // Wavefolding (timbre): Plaits-style
        float folds = 1.0f + 4.0f * timbre; // up to 5 folds
        for (int i = 0; i < int(folds); ++i) {
            out = 2.0f * std::fabs(0.5f * out) - 1.0f;
        }
        // Harmonics: brightness (simple lowpass)
        float brightness = 0.5f + 0.49f * harmonics;
        lastOut = brightness * out + (1.0f - brightness) * lastOut;
        // Subtle analog drift
        float drift = ((float)rand() / RAND_MAX - 0.5f) * 0.002f;
        lastOut += drift;
        return std::tanh(lastOut * 1.2f) * 0.95f;
    }
private:
    float sampleRate = 48000.0f;
    float frequency = 440.0f;
    float harmonics = 0.0f;
    float timbre = 0.5f;
    float morph = 0.0f;
    float phase = 0.0f;
    float lastOut = 0.0f;
    bool gateOn = false;
    // PolyBLEP helper
    static float polyblep(float t, float dt) {
        if (t < dt) {
            float x = t / dt;
            return x + x - x * x - 1.0f;
        } else if (t > 1.0f - dt) {
            float x = (t - 1.0f) / dt;
            return x * x + x + x + 1.0f;
        } else {
            return 0.0f;
        }
    }
};
