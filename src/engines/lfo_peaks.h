// Faithful Mutable Instruments Peaks LFO clone
#pragma once
#include <cmath>

class PeaksLFO {
public:
    enum Waveform { SINE, TRIANGLE, SQUARE, STEPS, RANDOM };
    void setSampleRate(float sr) { sampleRate = sr; }
    void setFrequency(float f) { freq = f; phaseInc = freq / sampleRate; }
    void setWaveform(Waveform w) { waveform = w; }
    void setVariation(float v) { variation = v; }
    void reset() { phase = 0.0f; lastStep = 0.0f; }
    float process() {
        phase += phaseInc;
        if (phase >= 1.0f) phase -= 1.0f;
        switch (waveform) {
            case SINE:
                return std::sin(2.0f * M_PI * phase);
            case TRIANGLE:
                return 2.0f * fabs(2.0f * phase - 1.0f) - 1.0f;
            case SQUARE:
                return (phase < 0.5f) ? 1.0f : -1.0f;
            case STEPS: {
                int steps = 2 + int(variation * 14.0f); // 2-16 steps
                float step = std::floor(phase * steps) / (steps - 1.0f) * 2.0f - 1.0f;
                return step;
            }
            case RANDOM:
                if (phase < lastStep) curRand = 2.0f * ((float)rand() / RAND_MAX) - 1.0f;
                lastStep = phase;
                return curRand;
        }
        return 0.0f;
    }
private:
    float sampleRate = 48000.0f;
    float freq = 1.0f;
    float phase = 0.0f;
    float phaseInc = 0.0f;
    Waveform waveform = SINE;
    float variation = 0.0f;
    float lastStep = 0.0f;
    float curRand = 0.0f;
};
