# 5yn7h_ Synth Plugin

A modern, multi-engine synthesizer plugin built with DPF (DISTRHO Plugin Framework).

## Features

- **12 Synthesis Engines:**
	1. Sine
	2. Triangle
	3. Square
	4. Saw
	5. SuperSaw
	6. Virtual Analog
	7. FM
	8. Formant
	9. Additive
	10. Chord
	11. String Resonator
	12. PWM

- **Effects:**
	- Improved Reverb (Schroeder/Moorer, not Freeverb)
	- Delay (with feedback and lowpass)
	- Chorus (multi-voice, smooth LFO)

- **Moog Ladder Filter:**
	- Cutoff (40 Hz – 18 kHz)
	- Resonance
	- Wet/Dry blend (default: 0, fully dry)

- **Host-Driven UI:**
	- No custom UI; all parameters are exposed to the host/DAW
	- All sliders and controls are automatable

- **Robust Engine Selector:**
	- Integer parameter, robust to host quirks
	- Always allows engine 0 (Sine) to be selected
	- Out-of-range or fractional values are clamped and rounded

## Parameter Defaults

- Filter Wet: **0** (fully dry)
- Engine: **0** (Sine)
- All other parameters: sensible musical defaults

## Usage

1. Load the plugin in your DAW or plugin host.
2. Use the host’s parameter controls to select engines and adjust effects/filters.
3. All features are accessible via the host’s UI.

## License
MIT

---

For more details, see the source code and comments.


