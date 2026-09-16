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


// Active screen, tracked so Serial logs fire only on transitions
enum class Screen : uint8_t { None, Warning, Greeting, Dashboard };

static Screen lastScreen = Screen::None;


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


// Waving hand icon (16x16): open palm with motion arcs top-left
static const unsigned char PROGMEM waveIcon[] = {
  0x38, 0x60,
  0x43, 0x6C,
  0x9B, 0x6C,
  0xA3, 0x6C,
  0x83, 0x6C,
  0x1B, 0x6C,
  0x1B, 0x6C,
  0x1B, 0x6D,
  0x1F, 0xFD,
  0x1F, 0xFD,
  0x1F, 0xFF,
  0x1F, 0xFE,
  0x0F, 0xFC,
  0x07, 0xF8,
  0x03, 0xF0,
  0x03, 0xF0
};



static void drawCentered(const char *text, int16_t y, uint8_t size) {

  display.setTextSize(size);

  int16_t x = (SCREEN_WIDTH - strlen(text) * 6 * size) / 2;

  display.setCursor(x, y);

  display.print(text);
}



// Motion screen: wave icon + headline centred as one group
static void drawGreeting() {

  const char *headline = "Welcome";
  const uint8_t size = 2;
  const int16_t y = 5;

  const int16_t iconSize = 16;
  const int16_t gap = 4;


  // Classic font advances 6px per char, but the spacer column after
  // the last char is blank, so drop it to get the visible width.
  int16_t textW = strlen(headline) * 6 * size - size;

  int16_t textH = 7 * size;

  int16_t x = (SCREEN_WIDTH - (iconSize + gap + textW)) / 2;


  // Centre the icon vertically on the glyph height
  display.drawBitmap(x, y + (textH - iconSize) / 2, waveIcon,
                     iconSize, iconSize, SSD1306_WHITE);

  display.setTextSize(size);

  display.setCursor(x + iconSize + gap, y);

  display.print(headline);


  drawCentered("to", 25, 2);

  drawCentered("Rimalx Lab", 45, 2);
}



// Edge-triggered: prints once when the active screen changes
static void logScreenChange(Screen screen) {

  if (screen == lastScreen) return;

  lastScreen = screen;


  if (screen == Screen::Greeting)

    Serial.println("[UI] Showing Greeting Screen (Wave Icon + Welcome)");

  else if (screen == Screen::Dashboard)

    Serial.println("[UI] Showing Dashboard Screen");
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


    logScreenChange(Screen::Warning);


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


    logScreenChange(Screen::Greeting);


    drawGreeting();

  }



  // Main dashboard
  else {


    logScreenChange(Screen::Dashboard);


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