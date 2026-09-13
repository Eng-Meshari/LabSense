#include <WiFi.h>
#include <time.h>
#include "config.h"
#include "secrets.h"
#include "network.h"

static uint32_t lastWifiCheck = 0;

// getLocalTime() with an explicit 0 timeout: one shot, no 5s default wait.
// It reports false until SNTP has moved the clock past 2016.
static bool localNow(struct tm &timeinfo) {
  return getLocalTime(&timeinfo, 0);
}

static void formatNow(char *buffer, size_t maxLen, const char *fmt, const char *fallback) {
  if (!buffer || maxLen == 0) return;
  struct tm timeinfo;
  if (!localNow(timeinfo) || strftime(buffer, maxLen, fmt, &timeinfo) == 0) {
    snprintf(buffer, maxLen, "%s", fallback);
  }
}

void initNetwork() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  // Association finishes in the background; the SNTP client fires once DHCP is up.
  configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER_1, NTP_SERVER_2);
}

void updateNetwork() {
  uint32_t now = millis();
  if (now - lastWifiCheck < WIFI_RECONNECT_INTERVAL) return;
  lastWifiCheck = now;

  if (WiFi.status() != WL_CONNECTED) WiFi.reconnect();
}

bool isWifiConnected() {
  return WiFi.status() == WL_CONNECTED;
}

bool isTimeSynced() {
  struct tm timeinfo;
  return localNow(timeinfo);
}

void getFormattedTime(char *buffer, size_t maxLen) {
  formatNow(buffer, maxLen, "%H:%M:%S", "--:--:--");
}

void getFormattedDate(char *buffer, size_t maxLen) {
  formatNow(buffer, maxLen, "%Y-%m-%d", "----/--/--");
}
