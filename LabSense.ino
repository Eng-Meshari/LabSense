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

static void i2cScan() {
  Serial.println("I2C scan:");
  bool any = false;
  for (uint8_t a = 1; a < 127; a++) {
    Wire.beginTransmission(a);
    if (Wire.endTransmission() == 0) { Serial.printf("  found 0x%02X\n", a); any = true; }
  }
  if (!any) Serial.println("  NOTHING on the bus -> SDA/SCL/VCC/GND wiring");
}

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
  i2cScan();

  oledOK = display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  if (!oledOK) oledOK = display.begin(SSD1306_SWITCHCAPVCC, 0x3D);  // some modules ship on 0x3D
  Serial.println(oledOK ? "OLED init OK" : "OLED NOT FOUND -> SDA=21 SCL=22 3.3V, addr 0x3C/0x3D");

  if (oledOK) {
    // every pixel on: if the screen does not go fully white here, the panel is not
    // receiving data (loose wire) or it is an SH1106 controller, not SSD1306
    display.clearDisplay();
    display.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
    display.display();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
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

  if (now - lastRead < DHT_READ_INTERVAL) return;  // DHT22 floor: 2s, doubles as the white-flash hold
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
