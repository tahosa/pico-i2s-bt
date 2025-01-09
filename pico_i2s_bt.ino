#include <BluetoothAudio.h>
#include <I2S.h>

#include "src/cbuf.hpp"

I2S i2s(INPUT);
A2DPSource a2dp;

size_t size = 2048;
int16_t buffer[2048];
cbuf_t *cbuf = cbuf_init(buffer, size);
volatile bool paused = false;

void avrcpCB(void *param, avrcp_operation_id_t op, int pressed) {
  (void) param;
  if (pressed && op == AVRCP_OPERATION_ID_PLAY) {
    paused = !paused;
    if (paused) {
      Serial.printf("Pausing\n");
    } else {
      Serial.printf("Resuming\n");
    }
  }
}

void volumeCB(void *param, int pct) {
  (void) param;
  Serial.printf("Speaker volume changed to %d%%\n", pct);
}

void connectCB(void *param, bool connected) {
  (void) param;
  if (connected) {
    Serial.printf("A2DP connection started to %s\n", bd_addr_to_str(a2dp.getSinkAddress()));
  } else {
    Serial.printf("A2DP connection stopped\n");
  }
}

void fillPCM() {


}

void setup() {
  // Setup for I2S intpu
  Serial.begin(115200);

  i2s.setDATA(0);
  i2s.setBCLK(1); // Note: LRCLK = BCLK + 1
  i2s.setBitsPerSample(16);
  i2s.setFrequency(44100);
  i2s.begin();

  int16_t sample[2];
  for (;;) {
    i2s.read16(sample, sample + 1);
  }
}

void loop() {}

void setup1() {
  // Setup for BT broadcast
  delay(2000);
  a2dp.onAVRCP(avrcpCB);
  a2dp.onVolume(volumeCB);
  a2dp.onConnect(connectCB);
  a2dp.begin();
  Serial.printf("Starting, press BOOTSEL to pair to first found speaker\n");

  int16_t out[128];
  size_t samples;

  for (;;) {
    // We don't want to over-fill the BT buffer, so we need to check how many samples can be written.
    samples = min(a2dp.availableForWrite(), 128);

    // If the shared buffer has fewer samples than we want to read, it will return how many it has
    samples = cbuf_read(cbuf, out, samples);
    a2dp.write((const uint8_t *)out, samples); // This might need to be samples * sizeof(int16_t)
  }
}

void loop1() {
   if (BOOTSEL) {
    while (BOOTSEL) {
      delay(1);
    }
    a2dp.disconnect();
    a2dp.clearPairing();
    Serial.printf("Connecting...");
    if (a2dp.connect()) {
      Serial.printf("Connected!\n");
    } else {
      Serial.printf("Failed!  :(\n");
    }
  }
}
