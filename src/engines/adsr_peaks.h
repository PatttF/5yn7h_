// Faithful Mutable Instruments Peaks ADSR/AD envelope clone
#pragma once
#include <cmath>

class PeaksADSR {
public:
    enum Mode { ADSR, AD };
    void setSampleRate(float sr) { sampleRate = sr; }
    void setAttack(float a) { attack = a; calcRates(); }
    void setDecay(float d) { decay = d; calcRates(); }
    void setSustain(float s) { sustain = s; }
    void setRelease(float r) { release = r; calcRates(); }
    void setMode(Mode m) { mode = m; }
    void gateOn() { state = ATTACK; }
    void gateOff() { if (mode == ADSR) state = RELEASE; else state = IDLE; }
    void reset() { state = IDLE; env = 0.0f; }
    float process() {
        switch (state) {
            case ATTACK:
                env += attackRate;
                if (env >= 1.0f) { env = 1.0f; state = DECAY; }
                break;
            case DECAY:
                env -= decayRate;
                if (env <= sustain || mode == AD) { env = sustain; state = (mode == ADSR) ? SUSTAIN : RELEASE; }
                break;
            case SUSTAIN:
                // Hold
                break;
            case RELEASE:
                env -= releaseRate;
                if (env <= 0.0f) { env = 0.0f; state = IDLE; }
                break;
            case IDLE:
            default:
                env = 0.0f;
                break;
        }
        return env;
    }
private:
    enum State { IDLE, ATTACK, DECAY, SUSTAIN, RELEASE };
    State state = IDLE;
    Mode mode = ADSR;
    float sampleRate = 48000.0f;
    float attack = 0.01f, decay = 0.1f, sustain = 0.7f, release = 0.2f;
    float attackRate = 0.0f, decayRate = 0.0f, releaseRate = 0.0f;
    float env = 0.0f;
    void calcRates() {
        attackRate = (attack > 0.0001f) ? 1.0f / (attack * sampleRate) : 1.0f;
        decayRate = (decay > 0.0001f) ? (1.0f - sustain) / (decay * sampleRate) : 1.0f;
        releaseRate = (release > 0.0001f) ? sustain / (release * sampleRate) : 1.0f;
    }
};
