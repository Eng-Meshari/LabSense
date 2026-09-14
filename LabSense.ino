// LabSense -- orchestrator only. All logic lives in sensors/network/display.

#include "config.h"

#include "sensors.h"
#include "labnet.h"
#include "display.h"


static SensorData data;


void setup() {

  Serial.begin(SERIAL_BAUD_RATE);

  Serial.println("\n--- LabSense boot ---");


  initSensors();

  Serial.println("[ok] sensors DHT22/PIR/buzzer");


  initDisplay();

  Serial.println("[ok] display SSD1306 128x64");


  initNetwork();

  Serial.println("[..] network Wi-Fi associating, NTP in background");


  Serial.println("--- running ---");
}



void loop() {

  updateNetwork();

  updateSensors(data);

  updateBuzzer(data);

  updateDisplay(data);


  yield();
}