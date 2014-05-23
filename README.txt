This is an ATtiny84 based audio level indicator.

Audio data comes in from an Electret microphone. This is amplified by an LM358 and fed to the ATtiny84. The microcontroller raeds in analog data, computes RMS amplitude, and lights up a set of LEDs acoording to intensity. 

To light the LEDs, pulsing will be used (light only one at a time) to reduce power consumption.

When there is no significant audio data coming in, LEDs will switch to a "Cylon" (Larsen Scanner) mode.
