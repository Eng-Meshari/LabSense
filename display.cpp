#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "config.h"
#include "display.h"
#include "labnet.h"

static Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

static bool oledOK = false;

static uint32_t motionStartTime = 0;
static bool lastMotionState = false;


// Temperature icon
static const unsigned char PROGMEM tempIcon[] = {
  0x18,
  0x18,
  0x18,
  0x18,
  0x18,
  0x3C,
  0x3C,
  0x18
};


// Humidity icon
static const unsigned char PROGMEM humIcon[] = {
  0x10,
  0x38,
  0x7C,
  0x7C,
  0x38,
  0x10,
  0x00,
  0x00
};



static void drawCentered(const char *text, int16_t y, uint8_t size) {

  display.setTextSize(size);

  int16_t x = (SCREEN_WIDTH - strlen(text) * 6 * size) / 2;

  display.setCursor(x, y);

  display.print(text);
}



void initDisplay() {

  Wire.begin(PIN_OLED_SDA, PIN_OLED_SCL);

  oledOK = display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);

  if (!oledOK) return;


  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.display();
}



void updateDisplay(const SensorData &data) {

  if (!oledOK) return;


  uint32_t now = millis();


  // Detect new motion event
  if (data.motionDetected && !lastMotionState) {

    motionStartTime = now;

  }

  lastMotionState = data.motionDetected;


  display.clearDisplay();



  // Temperature warning screen
  if (data.temperatureAlert) {


    drawCentered("WARNING!", 0, 2);


    drawCentered("High Temp", 28, 1);


    char temp[20];

    snprintf(temp, sizeof(temp),
             "%.1f C",
             data.temperature);


    drawCentered(temp, 45, 2);

  }



  // Motion screen for 3 seconds
  else if (now - motionStartTime < 3000 &&
           motionStartTime != 0) {


    drawCentered("Welcome", 5, 2);

    drawCentered("to", 25, 2);

    drawCentered("Rimalx Lab", 45, 2);

  }



  // Main dashboard
  else {


    char timeBuffer[16];

    char dateBuffer[16];


    getFormattedTime(timeBuffer, sizeof(timeBuffer));

    getFormattedDate(dateBuffer, sizeof(dateBuffer));



    drawCentered("RimalSense", 0, 1);



    display.setTextSize(1);



    // Temperature
    display.drawBitmap(0,18,tempIcon,8,8,SSD1306_WHITE);

    display.setCursor(11,18);

    if(data.dhtValid)

      display.printf("Temp: %.1f C",
                     data.temperature);

    else

      display.print("Temp: --");



    // Humidity
    display.drawBitmap(0,32,humIcon,8,8,SSD1306_WHITE);

    display.setCursor(11,32);

    if(data.dhtValid)

      display.printf("Hum : %.0f%%",
                     data.humidity);

    else

      display.print("Hum : --");



    display.setCursor(0,45);

    display.print(dateBuffer);



    display.setCursor(0,57);

    display.print(timeBuffer);

  }


  display.display();

}