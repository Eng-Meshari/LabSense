#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ==========================================
// Pin Definitions
// ==========================================

// I2C OLED (0.96 inch SSD1306)
#define PIN_OLED_SDA        21
#define PIN_OLED_SCL        22
#define SCREEN_WIDTH        128
#define SCREEN_HEIGHT       64
#define OLED_RESET          -1
#define SCREEN_ADDRESS      0x3C

// DHT22 (Temp & Humidity)
#define PIN_DHT_DATA        4
#define DHTTYPE             DHT22

// MQ-136 Gas Sensor (Must use ADC1: GPIO 32-39)
#define PIN_MQ136_ANALOG    34
#define GAS_ALERT_THRESHOLD 1800  // ADC 12-bit (0-4095), adjust after calibration

// PIR Motion Sensor (HC-SR501)
#define PIN_PIR_MOTION      14

// Output Indicators
#define PIN_BUZZER          18

// ==========================================
// Timing & Operational Constraints
// ==========================================
#define SERIAL_BAUD_RATE    115200
#define DHT_READ_INTERVAL   2000   // DHT22 requires minimum 2000ms between reads
#define SENSOR_POLL_RATE    500    // Fast poll for gas and motion (ms)
#define DISPLAY_PAGE_TIME   3000   // Time before cycling screen views (ms)

#endif // CONFIG_H
