#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>

// Snapshot of every sensor, refreshed by updateSensors().
// Temperature/humidity hold the last valid DHT22 sample between reads.
struct SensorData {
  float temperature   = NAN;
  float humidity      = NAN;
  int   gasRaw        = 0;
  bool  gasAlert      = false;
  bool  motionDetected = false;
  bool  dhtValid      = false;
};

void initSensors();
void updateSensors(SensorData &data);
void setBuzzer(bool state);

#endif // SENSORS_H
