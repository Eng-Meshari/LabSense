#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>

struct SensorData {
  float temperature = NAN;
  float humidity = NAN;
  bool motionDetected = false;
  bool dhtValid = false;
  bool temperatureAlert = false;
};

void initSensors();
void updateSensors(SensorData &data);
void updateBuzzer(const SensorData &data);
void setBuzzer(bool state);
#endif
