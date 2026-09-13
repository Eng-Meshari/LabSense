// LabSense -- orchestrator only. All logic lives in sensors/network/display.
#include "config.h"
#include "sensors.h"
#include "network.h"
#include "display.h"

static SensorData data;

// Pulses the buzzer while the gas alert holds; silent otherwise. Never blocks.
static void updateBuzzer(bool alert) {
  static uint32_t lastToggle = 0;
  static bool buzzerOn = false;

  if (!alert) {
    setBuzzer(false);
    buzzerOn = false;
    return;
  }

  uint32_t now = millis();
  if (now - lastToggle < BUZZER_PULSE_INTERVAL) return;
  lastToggle = now;
  buzzerOn = !buzzerOn;
  setBuzzer(buzzerOn);
}

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  Serial.println("\n--- LabSense boot ---");

  initSensors();
  Serial.println("[ok] sensors  DHT22/MQ-136/PIR/buzzer");

  initDisplay();
  Serial.println("[ok] display  SSD1306 128x64");

  initNetwork();
  Serial.println("[..] network  Wi-Fi associating, NTP in background");

  Serial.println("--- running ---");
}

void loop() {
  updateNetwork();
  updateSensors(data);
  updateBuzzer(data.gasAlert);
  updateDisplay(data);

  yield();  // hand the CPU back to the RTOS; no delay() anywhere in this firmware
}
