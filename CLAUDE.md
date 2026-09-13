# Lab Environment Monitor (ESP32 IoT)

## Project Overview
Interactive lab environment monitor featuring temperature/humidity tracking (DHT22), H2S toxic gas monitoring (MQ-136), physical presence detection (HC-SR501), an OLED status dashboard (SSD1306), and an audible alert system (Active Buzzer).

## Hardware & Architecture Specs
- **Target:** ESP32 DevKit V1 (ESP-WROOM-32)
- **Framework:** Arduino C++
- **ADC Restriction:** MQ-136 MUST read from ADC1 (GPIO 34). Never map analog reads to ADC2 when Wi-Fi is active.
- **Timing:** Strictly non-blocking. Do NOT use `delay()`; use `millis()` or FreeRTOS timers.
- **DHT Constraint:** DHT22 minimum polling cycle is 2 seconds.

## Strict Pinout
- OLED SDA: GPIO 21
- OLED SCL: GPIO 22
- DHT22 Data: GPIO 4
- MQ-136 AOUT: GPIO 34 (ADC1_CH6)
- HC-SR501 OUT: GPIO 27
- Active Buzzer: GPIO 26

## Required Libraries
- `Adafruit_SSD1306` & `Adafruit_GFX` (for OLED)
- `DHT sensor library` by Adafruit
- `WiFi.h` & `PubSubClient` (or custom HTTP client for telemetry)
- `ArduinoJson` (v6 or v7)

## Coding Standards
- Implement modular logic in `sensors.cpp`, `network.cpp`, and keep `main.ino` minimal.
- Never hardcode credentials; read from `secrets.h`.
