#include "SynthEngine.h"

void SynthEngine::process(float& left, float& right) {
    switch (currentEngine) {
        case ENGINE_SINE: sine.process(left, right); break;
        case ENGINE_TRIANGLE: triangle.process(left, right); break;
        case ENGINE_SQUARE: square.process(left, right); break;
        case ENGINE_SAW: saw.process(left, right); break;
        case ENGINE_SUPERSAW: superSaw.process(left, right); break;
        case ENGINE_VIRTUAL_ANALOG: va.process(left, right); break;
        case ENGINE_FM: fm.process(left, right); break;
        case ENGINE_FORMANT: formant.process(left, right); break;
        case ENGINE_ADDITIVE: additive.process(left, right); break;
        case ENGINE_CHORD: chord.process(left, right); break;
    case ENGINE_STRING: stringRes.process(left, right); break;
    case ENGINE_PWM: pwm.process(left, right); break;
        default: stringRes.process(left, right); break;
    }
}
