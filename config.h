#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

#define PIN_OLED_SDA        21
#define PIN_OLED_SCL        22
#define SCREEN_WIDTH        128
#define SCREEN_HEIGHT       64
#define OLED_RESET          -1
#define SCREEN_ADDRESS      0x3C

#define PIN_DHT_DATA        4
#define DHTTYPE             DHT22

// MQ-136 removed from project

#define PIN_PIR_MOTION      27

#define PIN_BUZZER          26

#define SERIAL_BAUD_RATE    115200

#define DHT_READ_INTERVAL   2000
#define SENSOR_POLL_RATE    500
#define DISPLAY_PAGE_TIME   3000
#define DISPLAY_REFRESH_INTERVAL 200

// Temperature alarm threshold
#define TEMP_ALERT_THRESHOLD 40.0

// Motion buzzer duration
#define MOTION_BEEP_DURATION 120

// Temperature alarm beep pattern
#define TEMP_BEEP_ON_TIME    300
#define TEMP_BEEP_OFF_TIME   1000


// ==========================================
// Network & NTP Time
// ==========================================

#define NTP_SERVER_1            "pool.ntp.org"
#define NTP_SERVER_2            "time.nist.gov"

#define GMT_OFFSET_SEC          (3 * 3600)
#define DAYLIGHT_OFFSET_SEC     0

#define WIFI_RECONNECT_INTERVAL 10000

#endif


