// moog_filter.hpp - Moog Ladder Filter (TPT, high quality, public domain)
// Based on Will Pirkle, Vadim Zavalishin, and Diakopoulos implementations
#pragma once
#include <cmath>

class MoogFilter {
public:
    MoogFilter(float sampleRate) : sampleRate(sampleRate) { reset(); }
    void setCutoff(float fc) {
        cutoff = fc;
        float wd = 2.0f * M_PI * cutoff;
        float T = 1.0f / sampleRate;
        float wa = (2.0f / T) * tan(wd * T / 2.0f);
        g = wa * T / 2.0f;
        G = g / (1.0f + g);
    }
    void setResonance(float r) { resonance = r * 4.0f; } // 0..1 mapped to 0..4
    void reset() {
        for (int i = 0; i < 4; ++i) z[i] = 0.0f;
    }
    float process(float in) {
        // TPT 4-pole Moog ladder filter
        float x = in - resonance * z[3];
        for (int i = 0; i < 4; ++i) {
            x = G * (x - z[i]) + z[i];
            z[i] = x;
        }
        return x;
    }
private:
    float sampleRate, cutoff = 1000.0f, resonance = 0.0f, g = 0.0f, G = 0.0f;
    float z[4] = {0};
};
