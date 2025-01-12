#include <I2S.h>

I2S i2s(OUTPUT);

void setup() {
  Serial.begin(115200);

  i2s.setDATA(0);
  i2s.setBCLK(1); // Note: LRCLK = BCLK + 1
  i2s.setBitsPerSample(16);
  i2s.setFrequency(44100);
  i2s.setSysClk(44100);
  i2s.begin();

  while (1) {
    i2s.write16(1024, 0);
  }
}

void loop() {}
