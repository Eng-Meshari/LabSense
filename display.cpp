#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"
#include "display.h"
#include "labnet.h"

static Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

static bool     oledOK      = false;
static uint32_t lastRefresh = 0;

// 6px per char at size 1, scaled by the text size.
static void drawCentered(const char *text, int16_t y, uint8_t size) {
  display.setTextSize(size);
  display.setCursor((SCREEN_WIDTH - (int16_t)(strlen(text) * 6 * size)) / 2, y);
  display.print(text);
}

static void drawRight(const char *text, int16_t y, uint8_t size) {
  display.setTextSize(size);
  display.setCursor(SCREEN_WIDTH - (int16_t)(strlen(text) * 6 * size), y);
  display.print(text);
}

// Priority 1: gas leak overrides everything on screen.
static void drawGasAlert(const SensorData &data) {
  drawCentered("WARNING!", 4, 2);
  drawCentered("GAS LEAK DETECTED", 26, 1);
  char line[24];
  snprintf(line, sizeof line, "Raw: %d / %d", data.gasRaw, GAS_ALERT_THRESHOLD);
  drawCentered(line, 44, 1);
}

// Priority 2: someone is in the lab.
static void drawGreeting() {
  drawCentered("Welcome to Lab!", 22, 1);
  drawCentered("Motion Active", 40, 1);
}

// Priority 3: idle dashboard.
static void drawDashboard(const SensorData &data) {
  char buf[16];

  getFormattedTime(buf, sizeof buf);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print(buf);
  drawRight(isWifiConnected() ? "WiFi" : "----", 0, 1);
  display.drawFastHLine(0, 10, SCREEN_WIDTH, SSD1306_WHITE);

  display.setTextSize(2);
  display.setCursor(0, 14);
  if (data.dhtValid) display.printf("%.1fC", data.temperature);
  else               display.print("--.-C");
  display.setCursor(74, 14);
  if (data.dhtValid) display.printf("%.0f%%", data.humidity);
  else               display.print("--%");

  display.setTextSize(1);
  display.setCursor(0, 38);
  display.printf("Gas: %d", data.gasRaw);

  getFormattedDate(buf, sizeof buf);
  display.setCursor(0, 52);
  display.print(buf);
}

void initDisplay() {
  Wire.begin(PIN_OLED_SDA, PIN_OLED_SCL);

  oledOK = display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  if (!oledOK) oledOK = display.begin(SSD1306_SWITCHCAPVCC, 0x3D);  // some panels ship on 0x3D
  if (!oledOK) {
    Serial.println("OLED not found -> check SDA=21 SCL=22, 3.3V, addr 0x3C/0x3D");
    return;
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.display();
}

void updateDisplay(const SensorData &data) {
  if (!oledOK) return;  // begin() failed: no frame buffer to draw into

  uint32_t now = millis();
  if (now - lastRefresh < DISPLAY_REFRESH_INTERVAL) return;
  lastRefresh = now;

  display.clearDisplay();
  if      (data.gasAlert)       drawGasAlert(data);
  else if (data.motionDetected) drawGreeting();
  else                          drawDashboard(data);
  display.display();
}
