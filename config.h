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

// MQ-136 Gas Sensor
// IMPORTANT:
// AO from MQ-136 should go to GPIO34 through a voltage divider
#define PIN_MQ136_ANALOG    34

// Temporary starting value.
// Must be calibrated using real sensor readings.
#define GAS_ALERT_THRESHOLD 1800

// PIR Motion Sensor (HC-SR501)
#define PIN_PIR_MOTION      27

// Active Buzzer Module
// S -> GPIO26
#define PIN_BUZZER          26

// ==========================================
// Timing & Operational Constraints
// ==========================================

#define SERIAL_BAUD_RATE    115200

// DHT22 should not be read too frequently
#define DHT_READ_INTERVAL   2000

// Gas + PIR polling interval
#define SENSOR_POLL_RATE    500

// Time before changing display pages
#define DISPLAY_PAGE_TIME   3000

#endif // CONFIG_H
