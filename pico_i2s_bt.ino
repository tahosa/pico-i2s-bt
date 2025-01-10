#include <BluetoothAudio.h>
#include <I2S.h>

#include "src/cbuf.hpp"

I2S i2s(INPUT);
A2DPSource a2dp;

int16_t buffer[1024];
cbuf_t *cbuf = cbuf_init(buffer, 1024);
volatile bool paused = false;

void avrcpCB(void *param, avrcp_operation_id_t op, int pressed) {
  (void) param;
  if (pressed && op == AVRCP_OPERATION_ID_PLAY) {
    paused = !paused;
    if (paused) {
      Serial.printf("Pausing\r\n");
    } else {
      Serial.printf("Resuming\r\n");
    }
  }
}

void volumeCB(void *param, int pct) {
  (void) param;
  Serial.printf("Speaker volume changed to %d%%\r\n", pct);
}

void connectCB(void *param, bool connected) {
  (void) param;
  if (connected) {
    Serial.printf("A2DP connection started to %s\r\n", bd_addr_to_str(a2dp.getSinkAddress()));
  } else {
    Serial.printf("A2DP connection stopped\r\n");
  }
}

void setup1() {
  // Set up I2S input
  i2s.setDATA(0);
  i2s.setBCLK(1); // Note: LRCLK = BCLK + 1
  i2s.setBitsPerSample(16);
  i2s.setFrequency(11025); // Set freq to 11.025kHz for testing
  i2s.begin();
}

int16_t temp[2] = {0, 0};
void loop1() {
  // Read into the circular buffer
  i2s.read16(temp, temp + 1);
  cbuf_write(cbuf, temp, sizeof(temp));
}

void setup() {
  // Set up Bluetooth
  digitalWrite(LED_BUILTIN, 1);
  Serial.begin(9600);

  delay(2000);
  a2dp.onAVRCP(avrcpCB);
  a2dp.onVolume(volumeCB);
  a2dp.onConnect(connectCB);
  a2dp.begin();

  Serial.printf("Starting, press BOOTSEL to pair to first found speaker\r\n");
  digitalWrite(LED_BUILTIN, 0);
}

int16_t out[128];
size_t samples;

void loop() {
  // Don't read more than the BT buffer can hold; wait for it to have at least 128 samples free
  while ((size_t)a2dp.availableForWrite() > sizeof(out)) {
    samples = cbuf_read(cbuf, out, sizeof(out));
    a2dp.write((const uint8_t *)out, samples);
  }

  if (BOOTSEL) {
    digitalWrite(LED_BUILTIN, 1);

    while (BOOTSEL) {
      delay(1);
    }
    Serial.printf("Disconnecting...\r\n");
    a2dp.disconnect();
    a2dp.clearPairing();
    digitalWrite(LED_BUILTIN, 0);
    delay(100);
    digitalWrite(LED_BUILTIN, 1);

    Serial.printf("Connecting...");
    if (a2dp.connect()) {
      Serial.printf("Connected!\r\n");
      digitalWrite(LED_BUILTIN, 0);
    } else {
      Serial.printf("Failed!  :(\r\n");
      for (int i = 0; i < 5; i++) {
        digitalWrite(LED_BUILTIN, 0);
        delay(200);
        digitalWrite(LED_BUILTIN, 1);
        delay(200);
      }
    }
    digitalWrite(LED_BUILTIN, 0);
  }
}
