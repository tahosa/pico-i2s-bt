#include <I2S.h>

I2S i2s(INPUT);

void setup() {
  Serial.begin(115200);

  i2s.setDATA(0);
  i2s.setBCLK(1); // Note: LRCLK = BCLK + 1
  i2s.setBitsPerSample(16);
  i2s.setFrequency(44100);
  i2s.setSysClk(44100);
  i2s.begin();

  int16_t l, r, lastL, lastR;
  uint32_t count = 0;

  while (1) {
    lastL = l;
    lastR = r;
    i2s.read16(&l, &r);
    if(l != lastL || r != lastR) {
      Serial.printf("change at cycle\t%d\t%d\t%d\n", count, l, r);
      count = 0;
    }
    count++;
  }
}

void loop() {}
