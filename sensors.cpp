#include <DHT.h>
#include "config.h"
#include "sensors.h"

static DHT dht(PIN_DHT_DATA, DHTTYPE);

static uint32_t lastDhtRead = 0;
static float lastTemp = NAN;
static float lastHum = NAN;
static bool lastDhtOK = false;

static uint32_t motionStart = 0;
static bool lastMotion = false;

static uint32_t tempBeepTimer = 0;
static bool buzzerState = false;
static bool motionBeepActive = false;
static uint32_t motionBeepStart = 0;

void initSensors() {
  pinMode(PIN_PIR_MOTION, INPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, HIGH);
  dht.begin();
}

void updateSensors(SensorData &data) {
  uint32_t now = millis();

  if (now - lastDhtRead >= DHT_READ_INTERVAL) {
    lastDhtRead = now;

    float t = dht.readTemperature();
    float h = dht.readHumidity();

    lastDhtOK = !isnan(t) && !isnan(h);

    if (lastDhtOK) {
      lastTemp = t;
      lastHum = h;
    }
  }

  data.temperature = lastTemp;
  data.humidity = lastHum;
  data.dhtValid = lastDhtOK;

  data.motionDetected = digitalRead(PIN_PIR_MOTION) == HIGH;
  data.temperatureAlert =
      data.dhtValid && data.temperature >= TEMP_ALERT_THRESHOLD;
}

void updateBuzzer(const SensorData &data) {
  uint32_t now = millis();

  // Single short beep when motion is detected
if (data.motionDetected && !lastMotion) {
  motionBeepActive = true;
  motionBeepStart = now;
  digitalWrite(PIN_BUZZER, HIGH);
}

if (motionBeepActive &&
    now - motionBeepStart >= MOTION_BEEP_DURATION) {

  motionBeepActive = false;
  digitalWrite(PIN_BUZZER, LOW);
}

lastMotion = data.motionDetected;

  // Intermittent beep for high temperature
  if (data.temperatureAlert) {
    if (now - tempBeepTimer >= TEMP_BEEP_OFF_TIME) {
      tempBeepTimer = now;
      buzzerState = true;
      digitalWrite(PIN_BUZZER, HIGH);
    }

    if (buzzerState && now - tempBeepTimer >= TEMP_BEEP_ON_TIME) {
      buzzerState = false;
      digitalWrite(PIN_BUZZER, LOW);
    }
  }
  else {
    buzzerState = false;
    digitalWrite(PIN_BUZZER, LOW);
  }
}
