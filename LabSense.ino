// Wiring smoke test: prints every sensor to OLED + Serial. No Wi-Fi, no alerts.
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>          // must precede config.h: DHTTYPE expands to a DHT.h macro
#include "config.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
DHT dht(PIN_DHT_DATA, DHTTYPE);

static uint32_t lastRead = 0;
static uint32_t beepUntil = 0;
static bool oledOK = false;

static void beep(uint16_t ms) {
  digitalWrite(PIN_BUZZER, HIGH);
  beepUntil = millis() + ms;
}

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  pinMode(PIN_PIR_MOTION, INPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);

  Wire.begin(PIN_OLED_SDA, PIN_OLED_SCL);
  oledOK = display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  if (!oledOK) {
    Serial.println("OLED NOT FOUND -> check SDA=21 / SCL=22 / 3.3V / addr 0x3C");
  } else {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("LabSense");
    display.println("wiring test...");
    display.display();
  }

  dht.begin();
  beep(150);  // hear one short beep at boot = buzzer wired OK
  Serial.println("\n--- LabSense wiring test ---");
}

void loop() {
  uint32_t now = millis();

  if (beepUntil && now >= beepUntil) {
    digitalWrite(PIN_BUZZER, LOW);
    beepUntil = 0;
  }

  if (now - lastRead < DHT_READ_INTERVAL) return;  // DHT22 floor: 2s
  lastRead = now;

  float t = dht.readTemperature();
  float h = dht.readHumidity();
  int gas = analogRead(PIN_MQ136_ANALOG);   // raw 0-4095; MQ-136 needs ~24h burn-in before it means anything
  bool motion = digitalRead(PIN_PIR_MOTION);
  bool dhtOK = !isnan(t) && !isnan(h);

  char l1[24], l2[24], l3[24];
  if (dhtOK) snprintf(l1, sizeof l1, "T:%.1fC H:%.0f%%", t, h);
  else       snprintf(l1, sizeof l1, "DHT: no data");
  snprintf(l2, sizeof l2, "Gas(raw): %d", gas);
  snprintf(l3, sizeof l3, "Motion: %s", motion ? "YES" : "no");

  Serial.printf("%s | %s | %s\n", l1, l2, l3);

  if (oledOK) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("LabSense TEST");
    display.println(l1);
    display.println(l2);
    display.println(l3);
    display.display();
  }

  if (motion) beep(60);  // wave a hand = short chirp
}
