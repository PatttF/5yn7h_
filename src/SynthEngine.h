#include "engines/pwm_engine.h"
// SynthEngine.h - Minimal synth engine for native standalone
#pragma once
#include "engines/sine_engine.h"
#include "engines/triangle_engine.h"
#include "engines/square_engine.h"
#include "engines/saw_engine.h"
#include "engines/supersaw_engine.h"
#include "engines/faithful_virtual_analog_engine.h"
#include "engines/fm_engine.h"
#include "engines/formant_engine.h"
#include "engines/additive_engine.h"
#include "engines/chord_engine.h"
#include "engines/string_engine.h"
#include <iostream>



class SynthEngine {
public:
    // --- Public interface for standalone_gui.cpp ---
    void process(float& left, float& right);
    void gate(bool g);
    float getFrequency() const;
    float getHarmonics() const;
    float getTimbre() const;
    float getMorph() const;
    float getLevel() const;
    float getLfoFreq() const;
    float getLfoWave() const;
    float getLfoVar() const;
    void getADSR(float& a, float& d, float& s, float& r) const;
    enum EngineType {
        ENGINE_SINE = 0,
        ENGINE_TRIANGLE,
        ENGINE_SQUARE,
        ENGINE_SAW,
        ENGINE_SUPERSAW,
        ENGINE_VIRTUAL_ANALOG,
        ENGINE_FM,
        ENGINE_FORMANT,
        ENGINE_ADDITIVE,
        ENGINE_CHORD,
        ENGINE_STRING,
        ENGINE_PWM,
    ENGINE_COUNT
    };

    SynthEngine() : currentEngine(ENGINE_STRING) {
        std::cerr << "SynthEngine: constructor" << std::endl;
        setSampleRate(48000.0f);
        setFrequency(220.0f);
        setHarmonics(0.5f);
        setTimbre(0.5f);
        setMorph(0.5f);
    }


    int getEngine() const { return currentEngine; }

    void setSampleRate(float sr) {
        sine.setSampleRate(sr);
        triangle.setSampleRate(sr);
        square.setSampleRate(sr);
        saw.setSampleRate(sr);
        superSaw.setSampleRate(sr);
        va.setSampleRate(sr);
        fm.setSampleRate(sr);
        formant.setSampleRate(sr);
        additive.setSampleRate(sr);
        chord.setSampleRate(sr);
        stringRes.setSampleRate(sr);
        pwm.setSampleRate(sr);
    }
    void setFrequency(float f) {
        switch (currentEngine) {
            case ENGINE_SINE: sine.setFrequency(f); break;
            case ENGINE_TRIANGLE: triangle.setFrequency(f); break;
            case ENGINE_SQUARE: square.setFrequency(f); break;
            case ENGINE_SAW: saw.setFrequency(f); break;
            case ENGINE_SUPERSAW: superSaw.setFrequency(f); break;
            case ENGINE_VIRTUAL_ANALOG: va.setFrequency(f); break;
            case ENGINE_FM: fm.setFrequency(f); break;
            case ENGINE_FORMANT: formant.setFrequency(f); break;
            case ENGINE_ADDITIVE: additive.setFrequency(f); break;
            case ENGINE_CHORD: chord.setFrequency(f); break;
            case ENGINE_STRING: stringRes.setFrequency(f); break;
            case ENGINE_PWM: pwm.setFrequency(f); break;
            default: stringRes.setFrequency(f); break;
        }
    }
    void setHarmonics(float h) {
        switch (currentEngine) {
            case ENGINE_SINE: sine.setHarmonics(h); break;
            case ENGINE_TRIANGLE: triangle.setHarmonics(h); break;
            case ENGINE_SQUARE: square.setHarmonics(h); break;
            case ENGINE_SAW: saw.setHarmonics(h); break;
            case ENGINE_SUPERSAW: superSaw.setHarmonics(h); break;
            case ENGINE_VIRTUAL_ANALOG: va.setHarmonics(h); break;
            case ENGINE_FM: fm.setHarmonics(h); break;
            case ENGINE_FORMANT: formant.setHarmonics(h); break;
            case ENGINE_ADDITIVE: additive.setHarmonics(h); break;
            case ENGINE_CHORD: chord.setHarmonics(h); break;
            case ENGINE_STRING: stringRes.setHarmonics(h); break;
            case ENGINE_PWM: pwm.setHarmonics(h); break;
            default: stringRes.setHarmonics(h); break;
        }
    }
    void setTimbre(float t) {
        switch (currentEngine) {
            case ENGINE_SINE: sine.setTimbre(t); break;
            case ENGINE_TRIANGLE: triangle.setTimbre(t); break;
            case ENGINE_SQUARE: square.setTimbre(t); break;
            case ENGINE_SAW: saw.setTimbre(t); break;
            case ENGINE_SUPERSAW: superSaw.setTimbre(t); break;
            case ENGINE_VIRTUAL_ANALOG: va.setTimbre(t); break;
            case ENGINE_FM: fm.setTimbre(t); break;
            case ENGINE_FORMANT: formant.setTimbre(t); break;
            case ENGINE_ADDITIVE: additive.setTimbre(t); break;
            case ENGINE_CHORD: chord.setTimbre(t); break;
            case ENGINE_STRING: stringRes.setTimbre(t); break;
            case ENGINE_PWM: pwm.setTimbre(t); break;
            default: stringRes.setTimbre(t); break;
        }
    }
    void setMorph(float m) {
        switch (currentEngine) {
            case ENGINE_SINE: sine.setMorph(m); break;
            case ENGINE_TRIANGLE: triangle.setMorph(m); break;
            case ENGINE_SQUARE: square.setMorph(m); break;
            case ENGINE_SAW: saw.setMorph(m); break;
            case ENGINE_SUPERSAW: superSaw.setMorph(m); break;
            case ENGINE_VIRTUAL_ANALOG: va.setMorph(m); break;
            case ENGINE_FM: fm.setMorph(m); break;
            case ENGINE_FORMANT: formant.setMorph(m); break;
            case ENGINE_ADDITIVE: additive.setMorph(m); break;
            case ENGINE_CHORD: chord.setMorph(m); break;
            case ENGINE_STRING: stringRes.setMorph(m); break;
            case ENGINE_PWM: pwm.setMorph(m); break;
            default: stringRes.setMorph(m); break;
        }
    }
    void setLevel(float l) {
        switch (currentEngine) {
            case ENGINE_SINE: sine.setLevel(l); break;
            case ENGINE_TRIANGLE: triangle.setLevel(l); break;
            case ENGINE_SQUARE: square.setLevel(l); break;
            case ENGINE_SAW: saw.setLevel(l); break;
            case ENGINE_SUPERSAW: superSaw.setLevel(l); break;
            case ENGINE_VIRTUAL_ANALOG: va.setLevel(l); break;
            case ENGINE_FM: fm.setLevel(l); break;
            case ENGINE_FORMANT: formant.setLevel(l); break;
            case ENGINE_ADDITIVE: additive.setLevel(l); break;
            case ENGINE_CHORD: chord.setLevel(l); break;
            case ENGINE_STRING: stringRes.setLevel(l); break;
            case ENGINE_PWM: pwm.setLevel(l); break;
            default: stringRes.setLevel(l); break;
        }
    }
    void setLfoFreq(float f) {
        switch (currentEngine) {
            case ENGINE_SINE: sine.setLfoFreq(f); break;
            case ENGINE_TRIANGLE: triangle.setLfoFreq(f); break;
            case ENGINE_SQUARE: square.setLfoFreq(f); break;
            case ENGINE_SAW: saw.setLfoFreq(f); break;
            case ENGINE_SUPERSAW: superSaw.setLfoFreq(f); break;
            case ENGINE_VIRTUAL_ANALOG: va.setLfoFreq(f); break;
            case ENGINE_FM: fm.setLfoFreq(f); break;
            case ENGINE_FORMANT: formant.setLfoFreq(f); break;
            case ENGINE_ADDITIVE: additive.setLfoFreq(f); break;
            case ENGINE_CHORD: chord.setLfoFreq(f); break;
            case ENGINE_STRING: stringRes.setLfoFreq(f); break;
            default: stringRes.setLfoFreq(f); break;
        }
    }
    void setLfoWave(float w) {
        switch (currentEngine) {
            case ENGINE_SINE: sine.setLfoWave(w); break;
            case ENGINE_TRIANGLE: triangle.setLfoWave(w); break;
            case ENGINE_SQUARE: square.setLfoWave(w); break;
            case ENGINE_SAW: saw.setLfoWave(w); break;
            case ENGINE_SUPERSAW: superSaw.setLfoWave(w); break;
            case ENGINE_VIRTUAL_ANALOG: va.setLfoWave(w); break;
            case ENGINE_FM: fm.setLfoWave(w); break;
            case ENGINE_FORMANT: formant.setLfoWave(w); break;
            case ENGINE_ADDITIVE: additive.setLfoWave(w); break;
            case ENGINE_CHORD: chord.setLfoWave(w); break;
            case ENGINE_STRING: stringRes.setLfoWave(w); break;
            default: stringRes.setLfoWave(w); break;
        }
    }
    void setLfoVar(float v) {
        switch (currentEngine) {
            case ENGINE_SINE: sine.setLfoVar(v); break;
            case ENGINE_TRIANGLE: triangle.setLfoVar(v); break;
            case ENGINE_SQUARE: square.setLfoVar(v); break;
            case ENGINE_SAW: saw.setLfoVar(v); break;
            case ENGINE_SUPERSAW: superSaw.setLfoVar(v); break;
            case ENGINE_VIRTUAL_ANALOG: va.setLfoVar(v); break;
            case ENGINE_FM: fm.setLfoVar(v); break;
            case ENGINE_FORMANT: formant.setLfoVar(v); break;
            case ENGINE_ADDITIVE: additive.setLfoVar(v); break;
            case ENGINE_CHORD: chord.setLfoVar(v); break;
            case ENGINE_STRING: stringRes.setLfoVar(v); break;
            default: stringRes.setLfoVar(v); break;
        }
    }
    void setADSR(float a, float d, float s, float r) {
        switch (currentEngine) {
            case ENGINE_SINE: sine.setADSR(a, d, s, r); break;
            case ENGINE_TRIANGLE: triangle.setADSR(a, d, s, r); break;
            case ENGINE_SQUARE: square.setADSR(a, d, s, r); break;
            case ENGINE_SAW: saw.setADSR(a, d, s, r); break;
            case ENGINE_SUPERSAW: superSaw.setADSR(a, d, s, r); break;
            case ENGINE_VIRTUAL_ANALOG: va.setADSR(a, d, s, r); break;
            case ENGINE_FM: fm.setADSR(a, d, s, r); break;
            case ENGINE_FORMANT: formant.setADSR(a, d, s, r); break;
            case ENGINE_ADDITIVE: additive.setADSR(a, d, s, r); break;
            case ENGINE_CHORD: chord.setADSR(a, d, s, r); break;
            case ENGINE_STRING: stringRes.setADSR(a, d, s, r); break;
            default: stringRes.setADSR(a, d, s, r); break;
        }
    }
    void setEngine(int e) {
        // Save current parameter values
        float freq = getFrequency();
        float harm = getHarmonics();
        float timbre = getTimbre();
        float morph = getMorph();
        float level = getLevel();
        float lfoFreq = getLfoFreq();
        float lfoWave = getLfoWave();
        float lfoVar = getLfoVar();
        float a, d, s, r;
        getADSR(a, d, s, r);
        // Gate off previous engine and reset
        switch (currentEngine) {
            case ENGINE_SINE: sine.gate(false); sine.reset(); break;
            case ENGINE_TRIANGLE: triangle.gate(false); triangle.reset(); break;
            case ENGINE_SQUARE: square.gate(false); square.reset(); break;
            case ENGINE_SAW: saw.gate(false); saw.reset(); break;
            case ENGINE_SUPERSAW: superSaw.gate(false); superSaw.reset(); break;
            case ENGINE_VIRTUAL_ANALOG: va.gate(false); va.reset(); break;
            case ENGINE_FM: fm.gate(false); fm.reset(); break;
            case ENGINE_FORMANT: formant.gate(false); formant.reset(); break;
            case ENGINE_ADDITIVE: additive.gate(false); additive.reset(); break;
            case ENGINE_CHORD: chord.gate(false); chord.reset(); break;
            case ENGINE_STRING: stringRes.gate(false); stringRes.reset(); break;
            default: stringRes.gate(false); stringRes.reset(); break;
        }
        currentEngine = (e < 0 ? 0 : (e >= ENGINE_COUNT ? ENGINE_COUNT-1 : e));
        // Reset new engine and restore parameter values
        switch (currentEngine) {
            case ENGINE_SINE:
                sine.reset();
                sine.setFrequency(freq); sine.setHarmonics(harm); sine.setTimbre(timbre); sine.setMorph(morph); sine.setLevel(level);
                sine.setLfoFreq(lfoFreq); sine.setLfoWave(lfoWave); sine.setLfoVar(lfoVar); sine.setADSR(a, d, s, r);
                break;
            case ENGINE_TRIANGLE:
                triangle.reset();
                triangle.setFrequency(freq); triangle.setHarmonics(harm); triangle.setTimbre(timbre); triangle.setMorph(morph); triangle.setLevel(level);
                triangle.setLfoFreq(lfoFreq); triangle.setLfoWave(lfoWave); triangle.setLfoVar(lfoVar); triangle.setADSR(a, d, s, r);
                break;
            case ENGINE_SQUARE:
                square.reset();
                square.setFrequency(freq); square.setHarmonics(harm); square.setTimbre(timbre); square.setMorph(morph); square.setLevel(level);
                square.setLfoFreq(lfoFreq); square.setLfoWave(lfoWave); square.setLfoVar(lfoVar); square.setADSR(a, d, s, r);
                break;
            case ENGINE_SAW:
                saw.reset();
                saw.setFrequency(freq); saw.setHarmonics(harm); saw.setTimbre(timbre); saw.setMorph(morph); saw.setLevel(level);
                saw.setLfoFreq(lfoFreq); saw.setLfoWave(lfoWave); saw.setLfoVar(lfoVar); saw.setADSR(a, d, s, r);
                break;
            case ENGINE_SUPERSAW:
                superSaw.reset();
                superSaw.setFrequency(freq); superSaw.setHarmonics(harm); superSaw.setTimbre(timbre); superSaw.setMorph(morph); superSaw.setLevel(level);
                superSaw.setLfoFreq(lfoFreq); superSaw.setLfoWave(lfoWave); superSaw.setLfoVar(lfoVar); superSaw.setADSR(a, d, s, r);
                break;
            case ENGINE_VIRTUAL_ANALOG:
                va.reset();
                va.setFrequency(freq); va.setHarmonics(harm); va.setTimbre(timbre); va.setMorph(morph); va.setLevel(level);
                va.setLfoFreq(lfoFreq); va.setLfoWave(lfoWave); va.setLfoVar(lfoVar); va.setADSR(a, d, s, r);
                break;
            case ENGINE_FM:
                fm.reset();
                fm.setFrequency(freq); fm.setHarmonics(harm); fm.setTimbre(timbre); fm.setMorph(morph); fm.setLevel(level);
                fm.setLfoFreq(lfoFreq); fm.setLfoWave(lfoWave); fm.setLfoVar(lfoVar); fm.setADSR(a, d, s, r);
                break;
            case ENGINE_FORMANT:
                formant.reset();
                formant.setFrequency(freq); formant.setHarmonics(harm); formant.setTimbre(timbre); formant.setMorph(morph); formant.setLevel(level);
                formant.setLfoFreq(lfoFreq); formant.setLfoWave(lfoWave); formant.setLfoVar(lfoVar); formant.setADSR(a, d, s, r);
                break;
            case ENGINE_ADDITIVE:
                additive.reset();
                additive.setFrequency(freq); additive.setHarmonics(harm); additive.setTimbre(timbre); additive.setMorph(morph); additive.setLevel(level);
                additive.setLfoFreq(lfoFreq); additive.setLfoWave(lfoWave); additive.setLfoVar(lfoVar); additive.setADSR(a, d, s, r);
                break;
            case ENGINE_CHORD:
                chord.reset();
                chord.setFrequency(freq); chord.setHarmonics(harm); chord.setTimbre(timbre); chord.setMorph(morph); chord.setLevel(level);
                chord.setLfoFreq(lfoFreq); chord.setLfoWave(lfoWave); chord.setLfoVar(lfoVar); chord.setADSR(a, d, s, r);
                break;
            case ENGINE_STRING:
                stringRes.reset();
                stringRes.setFrequency(freq); stringRes.setHarmonics(harm); stringRes.setTimbre(timbre); stringRes.setMorph(morph); stringRes.setLevel(level);
                stringRes.setLfoFreq(lfoFreq); stringRes.setLfoWave(lfoWave); stringRes.setLfoVar(lfoVar); stringRes.setADSR(a, d, s, r);
                break;
            default:
                stringRes.reset();
                stringRes.setFrequency(freq); stringRes.setHarmonics(harm); stringRes.setTimbre(timbre); stringRes.setMorph(morph); stringRes.setLevel(level);
                stringRes.setLfoFreq(lfoFreq); stringRes.setLfoWave(lfoWave); stringRes.setLfoVar(lfoVar); stringRes.setADSR(a, d, s, r);
                break;
        }
    }

    // Process audio (stereo)


    // Gate (note on/off)


    // --- Engine instances ---
    SineEngine sine;
    TriangleEngine triangle;
    SquareEngine square;
    SawEngine saw;
    SuperSawEngine superSaw;
    FaithfulVirtualAnalogEngine va;
    FMEngine fm;
    FormantEngine formant;
    AdditiveEngine additive;
    ChordEngine chord;
    StringEngine stringRes;
    PWMEngine pwm;

    // --- Engine state ---
    int currentEngine;

    // Helper getters for current engine (private)
    float getCurrentEngineFrequency() const {
        switch (currentEngine) {
            case ENGINE_SINE: return sine.getFrequency();
            case ENGINE_TRIANGLE: return triangle.getFrequency();
            case ENGINE_SQUARE: return square.getFrequency();
            case ENGINE_SAW: return saw.getFrequency();
            case ENGINE_SUPERSAW: return superSaw.getFrequency();
            case ENGINE_VIRTUAL_ANALOG: return va.getFrequency();
            case ENGINE_FM: return fm.getFrequency();
            case ENGINE_FORMANT: return formant.getFrequency();
            case ENGINE_ADDITIVE: return additive.getFrequency();
            case ENGINE_CHORD: return chord.getFrequency();
            case ENGINE_STRING: return stringRes.getFrequency();
            case ENGINE_MODAL: return modal.getFrequency();
            case ENGINE_WAVESHAPER: return waveshaper.getFrequency();
            default: return waveshaper.getFrequency();
        }
    }
    float getCurrentEngineHarmonics() const {
        switch (currentEngine) {
            case ENGINE_SINE: return sine.getHarmonics();
            case ENGINE_TRIANGLE: return triangle.getHarmonics();
            case ENGINE_SQUARE: return square.getHarmonics();
            case ENGINE_SAW: return saw.getHarmonics();
            case ENGINE_SUPERSAW: return superSaw.getHarmonics();
            case ENGINE_VIRTUAL_ANALOG: return va.getHarmonics();
            case ENGINE_FM: return fm.getHarmonics();
            case ENGINE_FORMANT: return formant.getHarmonics();
            case ENGINE_ADDITIVE: return additive.getHarmonics();
            case ENGINE_CHORD: return chord.getHarmonics();
            case ENGINE_STRING: return stringRes.getHarmonics();
            case ENGINE_MODAL: return modal.getHarmonics();
            case ENGINE_WAVESHAPER: return waveshaper.getHarmonics();
            default: return waveshaper.getHarmonics();
        }
    }
    float getCurrentEngineTimbre() const {
        switch (currentEngine) {
            case ENGINE_SINE: return sine.getTimbre();
            case ENGINE_TRIANGLE: return triangle.getTimbre();
            case ENGINE_SQUARE: return square.getTimbre();
            case ENGINE_SAW: return saw.getTimbre();
            case ENGINE_SUPERSAW: return superSaw.getTimbre();
            case ENGINE_VIRTUAL_ANALOG: return va.getTimbre();
            case ENGINE_FM: return fm.getTimbre();
            case ENGINE_FORMANT: return formant.getTimbre();
            case ENGINE_ADDITIVE: return additive.getTimbre();
            case ENGINE_CHORD: return chord.getTimbre();
            case ENGINE_STRING: return stringRes.getTimbre();
            case ENGINE_MODAL: return modal.getTimbre();
            case ENGINE_WAVESHAPER: return waveshaper.getTimbre();
            default: return waveshaper.getTimbre();
        }
    }
    float getCurrentEngineMorph() const {
        switch (currentEngine) {
            case ENGINE_SINE: return sine.getMorph();
            case ENGINE_TRIANGLE: return triangle.getMorph();
            case ENGINE_SQUARE: return square.getMorph();
            case ENGINE_SAW: return saw.getMorph();
            case ENGINE_SUPERSAW: return superSaw.getMorph();
            case ENGINE_VIRTUAL_ANALOG: return va.getMorph();
            case ENGINE_FM: return fm.getMorph();
            case ENGINE_FORMANT: return formant.getMorph();
            case ENGINE_ADDITIVE: return additive.getMorph();
            case ENGINE_CHORD: return chord.getMorph();
            case ENGINE_STRING: return stringRes.getMorph();
            case ENGINE_MODAL: return modal.getMorph();
            case ENGINE_WAVESHAPER: return waveshaper.getMorph();
            default: return waveshaper.getMorph();
        }
    }
    float getCurrentEngineLevel() const {
        switch (currentEngine) {
            case ENGINE_SINE: return sine.getLevel();
            case ENGINE_TRIANGLE: return triangle.getLevel();
            case ENGINE_SQUARE: return square.getLevel();
            case ENGINE_SAW: return saw.getLevel();
            case ENGINE_SUPERSAW: return superSaw.getLevel();
            case ENGINE_VIRTUAL_ANALOG: return va.getLevel();
            case ENGINE_FM: return fm.getLevel();
            case ENGINE_FORMANT: return formant.getLevel();
            case ENGINE_ADDITIVE: return additive.getLevel();
            case ENGINE_CHORD: return chord.getLevel();
            case ENGINE_STRING: return stringRes.getLevel();
            case ENGINE_MODAL: return modal.getLevel();
            case ENGINE_WAVESHAPER: return waveshaper.getLevel();
            default: return waveshaper.getLevel();
        }
    }
    float getCurrentEngineLfoFreq() const {
        switch (currentEngine) {
            case ENGINE_SINE: return sine.getLfoFreq();
            case ENGINE_TRIANGLE: return triangle.getLfoFreq();
            case ENGINE_SQUARE: return square.getLfoFreq();
            case ENGINE_SAW: return saw.getLfoFreq();
            case ENGINE_SUPERSAW: return superSaw.getLfoFreq();
            case ENGINE_VIRTUAL_ANALOG: return va.getLfoFreq();
            case ENGINE_FM: return fm.getLfoFreq();
            case ENGINE_FORMANT: return formant.getLfoFreq();
            case ENGINE_ADDITIVE: return additive.getLfoFreq();
            case ENGINE_CHORD: return chord.getLfoFreq();
            case ENGINE_STRING: return stringRes.getLfoFreq();
            case ENGINE_MODAL: return modal.getLfoFreq();
            case ENGINE_WAVESHAPER: return waveshaper.getLfoFreq();
            default: return waveshaper.getLfoFreq();
        }
    }
    float getCurrentEngineLfoWave() const {
        switch (currentEngine) {
            case ENGINE_SINE: return sine.getLfoWave();
            case ENGINE_TRIANGLE: return triangle.getLfoWave();
            case ENGINE_SQUARE: return square.getLfoWave();
            case ENGINE_SAW: return saw.getLfoWave();
            case ENGINE_SUPERSAW: return superSaw.getLfoWave();
            case ENGINE_VIRTUAL_ANALOG: return va.getLfoWave();
            case ENGINE_FM: return fm.getLfoWave();
            case ENGINE_FORMANT: return formant.getLfoWave();
            case ENGINE_ADDITIVE: return additive.getLfoWave();
            case ENGINE_CHORD: return chord.getLfoWave();
            case ENGINE_STRING: return stringRes.getLfoWave();
            case ENGINE_MODAL: return modal.getLfoWave();
            case ENGINE_WAVESHAPER: return waveshaper.getLfoWave();
            default: return waveshaper.getLfoWave();
        }
    }
    float getCurrentEngineLfoVar() const {
        switch (currentEngine) {
            case ENGINE_SINE: return sine.getLfoVar();
            case ENGINE_TRIANGLE: return triangle.getLfoVar();
            case ENGINE_SQUARE: return square.getLfoVar();
            case ENGINE_SAW: return saw.getLfoVar();
            case ENGINE_SUPERSAW: return superSaw.getLfoVar();
            case ENGINE_VIRTUAL_ANALOG: return va.getLfoVar();
            case ENGINE_FM: return fm.getLfoVar();
            case ENGINE_FORMANT: return formant.getLfoVar();
            case ENGINE_ADDITIVE: return additive.getLfoVar();
            case ENGINE_CHORD: return chord.getLfoVar();
            case ENGINE_STRING: return stringRes.getLfoVar();
            case ENGINE_MODAL: return modal.getLfoVar();
            case ENGINE_WAVESHAPER: return waveshaper.getLfoVar();
            default: return waveshaper.getLfoVar();
        }
    }
    void getCurrentEngineADSR(float& a, float& d, float& s, float& r) const {
        switch (currentEngine) {
            case ENGINE_SINE: sine.getADSR(a, d, s, r); break;
            case ENGINE_TRIANGLE: triangle.getADSR(a, d, s, r); break;
            case ENGINE_SQUARE: square.getADSR(a, d, s, r); break;
            case ENGINE_SAW: saw.getADSR(a, d, s, r); break;
            case ENGINE_SUPERSAW: superSaw.getADSR(a, d, s, r); break;
            case ENGINE_VIRTUAL_ANALOG: va.getADSR(a, d, s, r); break;
            case ENGINE_FM: fm.getADSR(a, d, s, r); break;
            case ENGINE_FORMANT: formant.getADSR(a, d, s, r); break;
            case ENGINE_ADDITIVE: additive.getADSR(a, d, s, r); break;
            case ENGINE_CHORD: chord.getADSR(a, d, s, r); break;
            case ENGINE_STRING: stringRes.getADSR(a, d, s, r); break;
            case ENGINE_MODAL: modal.getADSR(a, d, s, r); break;
            case ENGINE_WAVESHAPER: waveshaper.getADSR(a, d, s, r); break;
            default: waveshaper.getADSR(a, d, s, r); break;
        }
    }
};
