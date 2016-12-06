/***************************************************************************************************

    AC power interrupter

   This is the software for a small box that interrupts the 120 VAC supply to a device,
   with knobs controlling the on-time and the off-time.

   The custom-built hardware consists of:
     - a Teensy LC microcontroller
     - two 20K potentiometers to set the on time and the off time
     - a three-position toggle switch to set the mode (on-off-auto)
     - a red/green LED (auto on, auto off)
     - a power relay

   The design of the hardware and software for the sensors and the corresponding
   receiver is open-source.  See https://github.com/LenShustek/power_interrupter.

   ------------------------------------------------------------------------------------------------------
   Copyright (c) 2016, Len Shustek

   The MIT License (MIT)
   Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
   associated documentation files (the "Software"), to deal in the Software without restriction,
   including without limitation the rights to use, copy, modify, merge, publish, distribute,
   sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all copies or
   substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
   NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
   DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
   ------------------------------------------------------------------------------------------------------

  **** Change log ****

   1 December 2016, L. Shustek; first version for D. West

*****************************************************************************************************/
#include <Arduino.h>

// operational configuration

#define MIN_ON_TIME 1   // these times are in seconds
#define MAX_ON_TIME 30
#define MIN_OFF_TIME 1
#define MAX_OFF_TIME 180

// hardware configuration

#define LED_RED 23
#define LED_GREEN 22
#define RELAY 18
#define KNOB_ON A0
#define KNOB_OFF A1
#define TOGGLE_ON 19
#define TOGGLE_AUTO 21
#define DEBOUNCE_DELAY 50 // msec

void setup (void) {
   pinMode(LED_RED, OUTPUT);
   pinMode(LED_GREEN, OUTPUT);
   pinMode(RELAY, OUTPUT);
   pinMode(KNOB_ON, INPUT); // analog input is 0..1023 for 0..3.3V
   pinMode(KNOB_OFF, INPUT);
   pinMode(TOGGLE_ON, INPUT_PULLUP);
   pinMode(TOGGLE_AUTO, INPUT_PULLUP);
   for (int i = 0; i < 5; ++i) { // announce our presence by winking the light
      digitalWrite(LED_GREEN, HIGH);
      delay(250);
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_RED, HIGH);
      delay(250);
      digitalWrite(LED_RED, LOW); } }

bool delay_while_auto(unsigned long delaymsec) {
   do {
      delay(100); // delay in 100 msec increments
      if (delaymsec < 100)
         return false; // end timeout while still in auto mode
      delaymsec -= 100; }
   while (digitalRead(TOGGLE_AUTO) == LOW);
   return true; // switch changed: exit auto mode
}

void loop (void) {
   uint16_t knob_rotation; // 0..1023
   unsigned long knob_time; // seconds

   if (digitalRead(TOGGLE_ON) == LOW) { // switch up: manual power on
      delay(DEBOUNCE_DELAY);
      digitalWrite(RELAY, HIGH);
      while (digitalRead(TOGGLE_ON) == LOW) ; // continue as long as switch is in the up position
      digitalWrite(RELAY, LOW);
      delay(DEBOUNCE_DELAY); }

   while (digitalRead(TOGGLE_AUTO) == LOW) { // while switch down: auto mode
      delay(DEBOUNCE_DELAY);

      digitalWrite(RELAY, HIGH); // auto: power on
      digitalWrite(LED_GREEN, HIGH);
      knob_rotation = analogRead(KNOB_ON);
      knob_time = ((unsigned long)(MAX_ON_TIME - MIN_ON_TIME + 1) * knob_rotation + 512UL)/ 1024UL + MIN_ON_TIME;
      if (delay_while_auto(1000 * knob_time))
         break; // left auto mode
      digitalWrite(RELAY, LOW);
      digitalWrite(LED_GREEN, LOW);

      digitalWrite(LED_RED, HIGH); // auto: power off
      knob_rotation = analogRead(KNOB_OFF);
      knob_time = ((unsigned long)(MAX_OFF_TIME - MIN_OFF_TIME + 1) * knob_rotation + 512UL) / 1024UL + MIN_OFF_TIME;
      if (delay_while_auto(1000 * knob_time))
         break; // left auto mode
      digitalWrite(LED_RED, LOW); }

   digitalWrite(RELAY, LOW);
   digitalWrite(LED_GREEN, LOW);
   digitalWrite(LED_RED, LOW);
   delay(DEBOUNCE_DELAY); }

//*
