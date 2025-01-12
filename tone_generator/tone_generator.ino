/*
  This example generates a square wave based tone at a specified frequency
  and sample rate. Then outputs the data using the I2S interface to a
  MAX08357 I2S Amp Breakout board.

  created 17 November 2016
  by Sandeep Mistry
  modified for RP2040 by Earle F. Philhower, III <earlephilhower@yahoo.com>


    bool setBCLK(pin_size_t pin);
    - This assigns two adjacent pins - the pin after this one (one greater)
      is the WS (word select) signal, which toggles before the sample for
      each channel is sent

    bool setDATA(pin_size_t pin);
    - Sets the DOUT pin, can be any valid GPIO pin
*/

#include <I2S.h>
#include <math.h>

// Create the I2S port using a PIO state machine
I2S i2s(OUTPUT);

int sampleRate = 44100;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 1);

  Serial.begin(115200);
  Serial.println("I2S simple tone");

  i2s.setBCLK(2); // Note: LRCLK = BCLK + 1
  i2s.setDATA(16);
  i2s.setBitsPerSample(16);

  // start I2S at the sample rate with 16-bits per sample
  if (!i2s.begin(sampleRate)) {
    Serial.println("Failed to initialize I2S!");
    while (1); // do nothing
  }
}

int16_t left = 0;
int16_t right = 0;
uint16_t freq = 200;
double interval = 1.0 / (double) sampleRate;
uint32_t count = 0;
double sample = 0.0;

void loop() {
  sample = sin(2 * M_PI * freq * interval * count);
  //i2s.write16((uint16_t)(3000 * sample), 0);
  i2s.write16(1024, 0);
  count++;
}
