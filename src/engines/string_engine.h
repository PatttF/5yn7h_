// string_engine.h
// Plaits "String" engine: plucked/bowed/struck string physical modeling
#pragma once
#include <cmath>
#include <cstdlib>


class StringEngine {
public:
    void setLevel(float l) { level = l; }
    void setLfoFreq(float f) { lfoFreq = f; }
    void setLfoWave(float w) { lfoWave = w; }
    void setLfoVar(float v) { lfoVar = v; }
    void setADSR(float a, float d, float s, float r) { attack = a; decay = d; sustain = s; release = r; }
    void gate(bool g) { gateOn = g; }
    // Stereo process with level
    void process(float& left, float& right) {
    // Gate ignored: always output sound
        float l, r;
        processCore(l, r);
        left = l * level;
        right = r * level;
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
public:
    void setSampleRate(float sr) { sampleRate = sr; }
    void setFrequency(float freq) {
        frequency = std::max(20.0f, std::min(freq, sampleRate / 4.0f)); // Clamp to safe range
        updateDelay();
    }
    void setHarmonics(float h) { harmonics = h; updateDamping(); }
    void setTimbre(float t) { timbre = t; }
    void setMorph(float m) { morph = m; }
    void reset() {
        for (int i=0; i<maxDelay; ++i) { delayL[i] = 0.0f; delayR[i] = 0.0f; }
        idxL = idxR = 0;
        excitePhase = 0.0f;
        bowLP = 0.0f;
        apL = apR = 0.0f;
        delayLenL = 440;
        delayLenR = 440;
    }
    // Core stereo process
    void processCore(float& left, float& right) {
        // Excitation: morph between pluck, bow, strike, and position
        float exc = 0.0f;
        float excType = morph; // 0=pluck, 0.5=bow, 1=strike
        float pos = timbre; // excitation position
        if (excType < 0.33f) {
            // Pluck: short burst of noise
            if (excitePhase < 1.0f) {
                exc = ((float)rand() / RAND_MAX - 0.5f) * 2.0f * (1.0f - excitePhase);
                excitePhase += 1.0f / (sampleRate * 0.002f + pos * 0.02f * sampleRate);
            }
        } else if (excType < 0.66f) {
            // Bow: continuous noise with lowpass
            exc = ((float)rand() / RAND_MAX - 0.5f) * 0.4f;
            bowLP = bowLP * 0.96f + exc * 0.04f;
            exc = bowLP;
        } else {
            // Strike: short, sharp burst
            if (excitePhase < 0.5f) {
                exc = ((float)rand() / RAND_MAX - 0.5f) * 3.0f * (1.0f - 2.0f * excitePhase);
                excitePhase += 1.0f / (sampleRate * 0.001f + pos * 0.01f * sampleRate);
            }
        }
        // Stereo: two slightly detuned/offset delay lines for width
        float apCoef = 0.5f * (0.2f + 0.8f * harmonics);
        int safeLenL = std::max(1, delayLenL);
        int safeLenR = std::max(1, delayLenR);
        int prevIdxL = (idxL + safeLenL - 1) % safeLenL;
        int prevIdxR = (idxR + safeLenR - 1) % safeLenR;
        float apInL = delayL[prevIdxL];
        float apInR = delayR[prevIdxR];
        float apOutL = -apCoef * apL + apInL + apCoef * apL;
        float apOutR = -apCoef * apR + apInR + apCoef * apR;
        apL = apOutL;
        apR = apOutR;
        float fbL = 0.0f, fbR = 0.0f;
        if (safeLenL > 2) {
            fbL = 0.5f * (delayL[(idxL + 1) % safeLenL] + delayL[(idxL + 2) % safeLenL]);
        }
        if (safeLenR > 2) {
            fbR = 0.5f * (delayR[(idxR + 1) % safeLenR] + delayR[(idxR + 2) % safeLenR]);
        }
        fbL = fbL * damping + apOutL * (1.0f - damping);
        fbR = fbR * damping + apOutR * (1.0f - damping);
        delayL[idxL] = exc + fbL;
        delayR[idxR] = exc + fbR;
        float outL = delayL[idxL];
        float outR = delayR[idxR];
        idxL = (idxL + 1) % safeLenL;
        idxR = (idxR + 1) % safeLenR;
        float width = 0.7f + 0.3f * pos;
        left = outL * width;
        right = outR * width;
        float driftL = 1.0f + ((float)rand()/RAND_MAX-0.5f)*0.001f;
        float driftR = 1.0f + ((float)rand()/RAND_MAX-0.5f)*0.001f;
        left = std::tanh(left * 1.1f) * driftL;
        right = std::tanh(right * 1.1f) * driftR;
        float noiseL = ((float)rand() / RAND_MAX - 0.5f) * 0.002f;
        float noiseR = ((float)rand() / RAND_MAX - 0.5f) * 0.002f;
        left += noiseL;
        right += noiseR;
        if (excitePhase > 1.0f) excitePhase = 0.0f;
    }
private:
    float level = 1.0f;
    float lfoFreq = 0.0f;
    float lfoWave = 0.0f;
    float lfoVar = 0.0f;
    float attack = 0.01f, decay = 0.1f, sustain = 0.8f, release = 0.2f;
    bool gateOn = false;
    void updateDelay() {
        delayLenL = std::max(8, int(sampleRate / (frequency + 1.0f)));
        delayLenR = std::max(8, int(sampleRate / (frequency * 0.997f + 1.0f)));
        if (delayLenL > maxDelay) delayLenL = maxDelay;
        if (delayLenR > maxDelay) delayLenR = maxDelay;
    }
    void updateDamping() {
        damping = 0.96f + harmonics * 0.035f; // 0.96-0.995
    }
    float sampleRate = 48000.0f;
    float frequency = 440.0f;
    float harmonics = 0.0f;
    float timbre = 0.5f;
    float morph = 0.0f;
    static constexpr int maxDelay = 2048;
    float delayL[maxDelay] = {0};
    float delayR[maxDelay] = {0};
    int delayLenL = 440;
    int delayLenR = 440;
    int idxL = 0;
    int idxR = 0;
    float damping = 0.98f;
    float excitePhase = 0.0f;
    float bowLP = 0.0f;
    float apL = 0.0f, apR = 0.0f;
};
