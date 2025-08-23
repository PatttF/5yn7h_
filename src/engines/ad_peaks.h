// Faithful Mutable Instruments Peaks AD envelope clone
#pragma once
#include <cmath>

class PeaksAD {
public:
    void setSampleRate(float sr) { sampleRate = sr; }
    void setAttack(float a) { attack = a; calcRates(); }
    void setDecay(float d) { decay = d; calcRates(); }
    void gateOn() { state = ATTACK; }
    void gateOff() { state = IDLE; }
    void reset() { state = IDLE; env = 0.0f; }
    float process() {
        switch (state) {
            case ATTACK:
                env += attackRate;
                if (env >= 1.0f) { env = 1.0f; state = DECAY; }
                break;
            case DECAY:
                env -= decayRate;
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
    enum State { IDLE, ATTACK, DECAY };
    State state = IDLE;
    float sampleRate = 48000.0f;
    float attack = 0.01f, decay = 0.1f;
    float attackRate = 0.0f, decayRate = 0.0f;
    float env = 0.0f;
    void calcRates() {
        attackRate = (attack > 0.0001f) ? 1.0f / (attack * sampleRate) : 1.0f;
        decayRate = (decay > 0.0001f) ? 1.0f / (decay * sampleRate) : 1.0f;
    }
};
