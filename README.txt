AC power interrupter

This is the hardware and software design of a small box that interrupts the 120 VAC supply
to a device, with knobs controlling the on-time and the off-time.

The custom-built hardware consists of:
 - a Teensy LC microcontroller
 - two 20K potentiometers to set the on time and the off time
 - a three-position toggle switch to set the mode (on-off-auto)
 - a red/green LED (auto on, auto off)
 - a power relay
