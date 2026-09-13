#include <DHT.h>          // must precede config.h: DHTTYPE expands to a DHT.h macro
#include "config.h"
#include "sensors.h"

static DHT dht(PIN_DHT_DATA, DHTTYPE);

static uint32_t lastDhtRead = 0;
static float    lastTemp    = NAN;
static float    lastHum     = NAN;
static bool     lastDhtOK   = false;

void initSensors() {
  pinMode(PIN_PIR_MOTION, INPUT);
  pinMode(PIN_MQ136_ANALOG, INPUT);   // GPIO34 is input-only; explicit for clarity
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);
  dht.begin();
}

void updateSensors(SensorData &data) {
  uint32_t now = millis();

  // DHT22 cannot be polled faster than DHT_READ_INTERVAL; skip, never block.
  if (now - lastDhtRead >= DHT_READ_INTERVAL) {
    lastDhtRead = now;
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    lastDhtOK = !isnan(t) && !isnan(h);
    if (lastDhtOK) {
      lastTemp = t;
      lastHum  = h;
    }
  }
  data.temperature = lastTemp;
  data.humidity    = lastHum;
  data.dhtValid    = lastDhtOK;

  // MQ-136 on ADC1 (Wi-Fi safe). Raw 0-4095; threshold needs real-sensor calibration.
  data.gasRaw   = analogRead(PIN_MQ136_ANALOG);
  data.gasAlert = data.gasRaw > GAS_ALERT_THRESHOLD;

  data.motionDetected = digitalRead(PIN_PIR_MOTION) == HIGH;
}

void setBuzzer(bool state) {
  digitalWrite(PIN_BUZZER, state ? HIGH : LOW);
}
