//Switch pin=D4
//SDA = D21
//SCL = D22
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include "time.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Button Pin
const int BUTTON_PIN = 4;

// States
enum Mode { CLOCK_MODE, POMODORO_MODE };
Mode currentMode = CLOCK_MODE;

// Pomodoro Variables
int pomoMinutes = 25;
int pomoSeconds = 0;
bool pomoRunning = false;
unsigned long lastPomoTick = 0;

// Wi-Fi & NTP Settings

const char* ssid     = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";
const char* ntpServer = "pool.ntp.org";

// Timezone Offset: GMT+6 (Bangladesh) = 21600 seconds
const long  gmtOffset_sec = 21600;     
const int   daylightOffset_sec = 0;

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    for(;;); 
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(10, 25);
  display.println("Connecting WiFi...");
  display.display();
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void loop() {
  checkButton();

  switch (currentMode) {
    case CLOCK_MODE:
      drawClock();
      break;
    case POMODORO_MODE:
      updatePomodoro();
      drawPomodoro();
      break;
  }
  delay(10); 
}

// -------------------------------------------------------------
// CONTROLS (Single, Double, and Long Press Handling)
// -------------------------------------------------------------
void checkButton() {
  static bool lastButtonState = HIGH;
  static unsigned long buttonPressedTime = 0;
  static unsigned long lastReleaseTime = 0;
  static int clickCount = 0;
  static bool isWaitingForDouble = false;

  bool currentButtonState = digitalRead(BUTTON_PIN);
  unsigned long now = millis();

  if (lastButtonState == HIGH && currentButtonState == LOW) {
    buttonPressedTime = now;
  }
  
  if (lastButtonState == LOW && currentButtonState == HIGH) {
    unsigned long holdDuration = now - buttonPressedTime;
    
    if (holdDuration > 50) { 
      if (currentMode == POMODORO_MODE && holdDuration > 600) {
        pomoRunning = !pomoRunning;
        clickCount = 0;
        isWaitingForDouble = false;
      } else {
        clickCount++;
        lastReleaseTime = now;
        isWaitingForDouble = true;
      }
    }
  }

  if (isWaitingForDouble && (now - lastReleaseTime > 250)) {
    if (clickCount == 1) {
      currentMode = (Mode)((currentMode + 1) % 2);
    } 
    else if (clickCount >= 2) {
      if (currentMode == POMODORO_MODE && !pomoRunning) {
        pomoMinutes = 25;
        pomoSeconds = 0;
      }
    }
    clickCount = 0;
    isWaitingForDouble = false;
  }

  lastButtonState = currentButtonState;
}

// -------------------------------------------------------------
// DISPLAY INTERFACES
// -------------------------------------------------------------
void drawHeader(String title) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print(title);
  display.drawLine(0, 10, 128, 10, WHITE);
}

void drawClock() {
  drawHeader("RIFAT SHIKDAR");
  
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    display.setCursor(15, 30);
    display.print("Time Error");
    display.display();
    return;
  }
  
  int hour12 = timeinfo.tm_hour;
  String ampm = (hour12 >= 12) ? "PM" : "AM";
  
  if (hour12 == 0) hour12 = 12;
  else if (hour12 > 12) hour12 -= 12;
  
  // Display the Date
  display.setTextSize(1);
  display.setCursor(32, 18); 
  display.printf("%02d-%02d-%04d", timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900); 
  
  // Display clock numbers
  display.setTextSize(2);
  display.setCursor(5, 36);
  display.printf("%02d:%02d:%02d", hour12, timeinfo.tm_min, timeinfo.tm_sec); 
  
  // Display AM/PM marker
  display.setTextSize(1);
  display.setCursor(105, 42); 
  display.print(ampm);
  
  display.display();
}

void drawPomodoro() {
  drawHeader("POMODORO TIMER");
  
  display.setTextSize(2);
  display.setCursor(35, 25);
  display.printf("%02d:%02d", pomoMinutes, pomoSeconds);
  
  display.setTextSize(1);
  display.setCursor(8, 52);
  if (pomoRunning) {
    display.print("[ HOLD TO PAUSE ]");
  } else {
    display.print("[HOLD:START 2x:RESET]");
  }
  display.display();
}

// -------------------------------------------------------------
// POMODORO CORE LOGIC
// -------------------------------------------------------------
void updatePomodoro() {
  if (!pomoRunning) return;

  if (millis() - lastPomoTick >= 1000) {
    lastPomoTick = millis();
    
    if (pomoSeconds == 0) {
      if (pomoMinutes == 0) {
        pomoRunning = false;
        pomoMinutes = 25; 
      } else {
        pomoMinutes--;
        pomoSeconds = 59;
      }
    } else {
      pomoSeconds--;
    }
  }
}
