#pragma once
#include <cmath>


// Greatly improved PWM engine: bandlimited, analog drift, stereo spread, DC blocking, rich harmonics
class PWMEngine {
public:
    PWMEngine() : sampleRate(48000.0f), phaseL(0.0f), phaseR(0.0f), driftPhase(0.0f), dcL(0.0f), dcR(0.0f) {}
    void setSampleRate(float sr) { sampleRate = sr; }
    void setFrequency(float f) { freq = f; phaseInc = f / sampleRate; }
    void setHarmonics(float h) { harmonics = h; } // 0–1, controls saturation and noise
    void setTimbre(float t) { basePW = 0.05f + 0.9f * t; } // 0.05–0.95
    void setMorph(float m) { morph = m; } // 0–1, controls PWM LFO depth/rate
    void setLevel(float l) { level = l; }
    void reset() { phaseL = phaseR = driftPhase = 0.0f; dcL = dcR = 0.0f; }
    void process(float& left, float& right) {
        // Analog drift: slow random LFO modulates pulse width and phase
        driftPhase += 0.0003f + 0.001f * morph; // morph = drift speed
        if (driftPhase > 1.0f) driftPhase -= 1.0f;
        float drift = 0.002f * std::sin(2.0f * 3.14159f * driftPhase + 6.28f * morph) + 0.001f * (rand()/(float)RAND_MAX - 0.5f);
        // PWM LFO: morph controls depth and rate
        float lfoRate = 0.2f + 5.0f * morph;
        float lfoL = std::sin(2.0f * 3.14159f * lfoRate * phaseL);
        float lfoR = std::sin(2.0f * 3.14159f * (lfoRate * 1.03f) * phaseR + 0.3f); // stereo spread
        float pwL = basePW + 0.35f * morph * lfoL + drift;
        float pwR = basePW + 0.35f * morph * lfoR - drift;
        if (pwL < 0.05f) {
            pwL = 0.05f;
        }
        if (pwL > 0.95f) {
            pwL = 0.95f;
        }
        if (pwR < 0.05f) {
            pwR = 0.05f;
        }
        if (pwR > 0.95f) {
            pwR = 0.95f;
        }
        // Bandlimited PWM: two PolyBLEP saws
        float sawL1 = polyblepSaw(phaseL);
        float sawL2 = polyblepSaw(fmodf(phaseL + pwL, 1.0f));
        float outL = sawL1 - sawL2;
        float sawR1 = polyblepSaw(phaseR);
        float sawR2 = polyblepSaw(fmodf(phaseR + pwR, 1.0f));
        float outR = sawR1 - sawR2;
        // Soft saturation and noise for harmonics
        float noiseL = (rand()/(float)RAND_MAX - 0.5f) * 0.01f * harmonics;
        float noiseR = (rand()/(float)RAND_MAX - 0.5f) * 0.01f * harmonics;
        outL = std::tanh(outL + harmonics * outL * outL * outL + noiseL);
        outR = std::tanh(outR + harmonics * outR * outR * outR + noiseR);
        // DC blocking (simple 1-pole highpass)
        outL = dcBlock(outL, dcL);
        outR = dcBlock(outR, dcR);
        // Level
        left = outL * level;
        right = outR * level;
        // Advance phase
        phaseL += phaseInc + drift * 0.1f;
        phaseR += phaseInc - drift * 0.1f;
        if (phaseL >= 1.0f) phaseL -= 1.0f;
        if (phaseR >= 1.0f) phaseR -= 1.0f;
    }
private:
    float sampleRate = 48000.0f, freq = 440.0f, phaseInc = 0.01f;
    float phaseL = 0.0f, phaseR = 0.0f, driftPhase = 0.0f;
    float basePW = 0.5f, harmonics = 0.0f, morph = 0.0f, level = 1.0f;
    float dcL = 0.0f, dcR = 0.0f;
    // PolyBLEP bandlimited sawtooth
    float polyblepSaw(float t) {
        float dt = phaseInc;
        float s = 2.0f * t - 1.0f;
        s -= polyblep(t, dt);
        if (t + dt > 1.0f) s -= polyblep((t + dt - 1.0f), dt);
        return s;
    }
    // PolyBLEP step
    float polyblep(float t, float dt) {
        if (t < dt) {
            t /= dt;
            return t + t - t * t - 1.0f;
        } else if (t > 1.0f - dt) {
            t = (t - 1.0f) / dt;
            return t * t + t + t + 1.0f;
        } else return 0.0f;
    }
    // DC blocker (1-pole highpass)
    float dcBlock(float in, float& state) {
        float out = in - state + 0.995f * state;
        state = out;
        return out;
    }
};
