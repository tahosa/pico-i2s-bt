#include <BluetoothAudio.h>
#include <I2S.h>
#include <math.h>

I2S i2s(INPUT);
A2DPSource a2dp;

volatile bool paused = false;
const uint sampleRate = 44100;

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

void setup() {
  // Set up Bluetooth
  digitalWrite(LED_BUILTIN, 1);
  Serial.begin(115200);

  delay(2000);

  // Set up I2S input
  i2s.setDATA(0);
  i2s.setBCLK(1); // Note: LRCLK = BCLK + 1
  i2s.setBitsPerSample(16);
  i2s.begin(sampleRate);

  a2dp.setName("PicoW Zune");
  a2dp.onAVRCP(avrcpCB);
  a2dp.onVolume(volumeCB);
  a2dp.onConnect(connectCB);
  a2dp.setFrequency(sampleRate);
  a2dp.begin();

  Serial.printf("Starting, press BOOTSEL to pair to first found speaker\n");
  digitalWrite(LED_BUILTIN, 0);
}

int16_t temp[2];

void loop() {
  i2s.read16(temp, temp + 1);

  if ((size_t)a2dp.availableForWrite() > sizeof(temp)) {
    a2dp.write((const uint8_t *)temp, sizeof(temp));
  }

  if (BOOTSEL) {
    digitalWrite(LED_BUILTIN, 1);

    while (BOOTSEL) {
      delay(1);
    }
    Serial.printf("Disconnecting...\n");
    a2dp.disconnect();
    a2dp.clearPairing();
    digitalWrite(LED_BUILTIN, 0);
    delay(100);
    digitalWrite(LED_BUILTIN, 1);

    Serial.printf("Connecting...");
    if (a2dp.connect()) {
      Serial.printf("Connected!\n");
      digitalWrite(LED_BUILTIN, 0);
    } else {
      Serial.printf("Failed!  :(\n");
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
