
#include <BluetoothAudio.h>
#include <I2S.h>

I2S i2s(OUTPUT);
A2DPSink a2dp;

volatile A2DPSink::PlaybackStatus status = A2DPSink::STOPPED;

void volumeCB(void param, int pct) {
  (void) param;
  Serial.printf("Speaker volume changed to %d%%\n", pct);
}

void connectCB(voidparam, bool connected) {
  (void) param;
  if (connected) {
    Serial.printf("A2DP connection started to %s\n", bd_addr_to_str(a2dp.getSourceAddress()));
  } else {
    Serial.printf("A2DP connection stopped\n");
  }
}

void playbackCB(void param, A2DPSink::PlaybackStatus state) {
  (void) param;
  status = state;
}

void setup() {
  Serial.begin(115200);
  delay(3000);

  i2s.setDATA(0);
  i2s.setBCLK(1); // Note: LRCLK = BCLK + 1
  i2s.setBitsPerSample(16);
  i2s.setFrequency(41000);
  i2s.begin();

  Serial.printf("Starting, connect to the PicoW and start playing music\n");
  Serial.printf("Use BOOTSEL to pause/resume playback\n");
  a2dp.setName("PicoW Boom 00:00:00:00:00:00");
  a2dp.setConsumer(new BluetoothAudioConsumerI2S(i2s));
  a2dp.onVolume(volumeCB);
  a2dp.onConnect(connectCB);
  a2dp.onPlaybackStatus(playbackCB);
  a2dp.begin();
}

charnowPlaying = nullptr;

void loop() {
  if (BOOTSEL) {
    if (status == A2DPSink::PAUSED) {
      a2dp.play();
      Serial.printf("Resuming\n");
    } else if (status == A2DPSink::PLAYING) {
      a2dp.pause();
      Serial.printf("Pausing\n");
    }
    while (BOOTSEL);
  }
  if (!nowPlaying || strcmp(nowPlaying, a2dp.trackTitle())) {
    free(nowPlaying);
    nowPlaying = strdup(a2dp.trackTitle());
    Serial.printf("NOW PLAYING: %s\n", nowPlaying);
  }
}

