
// ============================================================
// ESP32 + OLED + TTP223 + BUZZER
// Clock + Pomodoro Timer
//
// TTP223 OUT  -> GPIO 4
// OLED SDA    -> GPIO 21
// OLED SCL    -> GPIO 22
// BUZZER      -> GPIO 25
// ============================================================

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include "time.h"

// ============================================================
// OLED SETTINGS
// ============================================================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  OLED_RESET
);

// ============================================================
// PIN DEFINITIONS
// ============================================================

#define TOUCH_PIN 4
#define BUZZER_PIN 25

// ============================================================
// MODES
// ============================================================

enum Mode {
  CLOCK_MODE,
  POMODORO_MODE
};

Mode currentMode = CLOCK_MODE;

// ============================================================
// POMODORO VARIABLES
// ============================================================

int pomoMinutes = 25;
int pomoSeconds = 0;

bool pomoRunning = false;

unsigned long lastPomoTick = 0;

// ============================================================
// WIFI & NTP
// ============================================================

const char* ssid = "Your_wifi_name";
const char* password = "Your_wifi_password";
const char* ntpServer = "pool.ntp.org";

// Bangladesh GMT +6
const long gmtOffset_sec = 21600;
const int daylightOffset_sec = 0;

// ============================================================
// TOUCH SETTINGS
// ============================================================

const unsigned long DEBOUNCE_TIME = 50;
const unsigned long DOUBLE_CLICK_TIME = 350;
const unsigned long LONG_PRESS_TIME = 600;

// ============================================================
// SETUP
// ============================================================

void setup() {

  Serial.begin(115200);

  // TTP223
  pinMode(TOUCH_PIN, INPUT);

  // Buzzer
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // I2C
  Wire.begin(21, 22);

  // OLED
  if (!display.begin(
        SSD1306_SWITCHCAPVCC,
        0x3C
      )) {

    Serial.println("OLED not found!");

    while (1);
  }

  // Startup display
  display.clearDisplay();

  display.setTextColor(WHITE);
  display.setTextSize(1);

  display.setCursor(10, 15);
  display.println("RIFAT SHIKDAR");

  display.setCursor(10, 30);
  display.println("Pomodoro Clock");

  display.setCursor(10, 45);
  display.println("Connecting WiFi...");

  display.display();

  // WiFi
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);

    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected!");

  // NTP
  configTime(
    gmtOffset_sec,
    daylightOffset_sec,
    ntpServer
  );

  // Startup beep
  beep(100);

  delay(1000);
}

// ============================================================
// MAIN LOOP
// ============================================================

void loop() {

  checkTouch();

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

// ============================================================
// BUZZER FUNCTIONS
// ============================================================

// Simple beep
void beep(int duration) {

  digitalWrite(BUZZER_PIN, HIGH);

  delay(duration);

  digitalWrite(BUZZER_PIN, LOW);
}

// ------------------------------------------------------------
// Single touch beep
// ------------------------------------------------------------

void singleTouchBeep() {

  beep(70);
}

// ------------------------------------------------------------
// Double touch / reset beep
// ------------------------------------------------------------

void doubleTouchBeep() {

  beep(70);

  delay(100);

  beep(70);
}

// ------------------------------------------------------------
// Start beep
// ------------------------------------------------------------

void startBeep() {

  beep(150);
}

// ------------------------------------------------------------
// Pause beep
// ------------------------------------------------------------

void pauseBeep() {

  beep(100);

  delay(100);

  beep(100);
}

// ------------------------------------------------------------
// Pomodoro finished alarm
// ------------------------------------------------------------

void pomodoroFinishedBeep() {

  for (int i = 0; i < 5; i++) {

    beep(200);

    delay(150);
  }
}

// ============================================================
// TOUCH CONTROL
// ============================================================

void checkTouch() {

  static bool lastTouchState = LOW;

  static unsigned long touchStartTime = 0;
  static unsigned long lastReleaseTime = 0;

  static int touchCount = 0;

  static bool waitingForDouble = false;
  static bool longPressHandled = false;

  bool currentTouchState =
    digitalRead(TOUCH_PIN);

  unsigned long now = millis();

  // ----------------------------------------------------------
  // TOUCH START
  // ----------------------------------------------------------

  if (
    lastTouchState == LOW &&
    currentTouchState == HIGH
  ) {

    touchStartTime = now;

    longPressHandled = false;

    Serial.println("Touch started");
  }

  // ----------------------------------------------------------
  // LONG TOUCH
  // ----------------------------------------------------------

  if (
    currentTouchState == HIGH &&
    !longPressHandled
  ) {

    if (
      now - touchStartTime >= LONG_PRESS_TIME
    ) {

      // Long touch only works in Pomodoro mode
      if (currentMode == POMODORO_MODE) {

        pomoRunning = !pomoRunning;

        lastPomoTick = millis();

        if (pomoRunning) {

          Serial.println("Pomodoro STARTED");

          startBeep();

        }
        else {

          Serial.println("Pomodoro PAUSED");

          pauseBeep();
        }
      }

      longPressHandled = true;

      touchCount = 0;

      waitingForDouble = false;
    }
  }

  // ----------------------------------------------------------
  // TOUCH RELEASE
  // ----------------------------------------------------------

  if (
    lastTouchState == HIGH &&
    currentTouchState == LOW
  ) {

    unsigned long touchDuration =
      now - touchStartTime;

    if (
      touchDuration >= DEBOUNCE_TIME &&
      !longPressHandled
    ) {

      touchCount++;

      lastReleaseTime = now;

      waitingForDouble = true;

      Serial.print("Touch count: ");
      Serial.println(touchCount);
    }
  }

  // ----------------------------------------------------------
  // SINGLE / DOUBLE TOUCH
  // ----------------------------------------------------------

  if (
    waitingForDouble &&
    now - lastReleaseTime > DOUBLE_CLICK_TIME
  ) {

    // ========================================================
    // SINGLE TOUCH
    // ========================================================

    if (touchCount == 1) {

      currentMode =
        (Mode)((currentMode + 1) % 2);

      Serial.println("Single touch -> Change mode");

      singleTouchBeep();
    }

    // ========================================================
    // DOUBLE TOUCH
    // ========================================================

    else if (touchCount >= 2) {

      if (
        currentMode == POMODORO_MODE &&
        !pomoRunning
      ) {

        pomoMinutes = 25;
        pomoSeconds = 0;

        Serial.println(
          "Double touch -> Pomodoro RESET"
        );

        doubleTouchBeep();
      }
    }

    touchCount = 0;

    waitingForDouble = false;
  }

  lastTouchState = currentTouchState;
}

// ============================================================
// DISPLAY HEADER
// ============================================================

void drawHeader(String title) {

  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(0, 0);

  display.print(title);

  display.drawLine(
    0,
    10,
    128,
    10,
    WHITE
  );
}

// ============================================================
// CLOCK DISPLAY
// ============================================================

void drawClock() {

  drawHeader("RIFAT SHIKDAR");

  struct tm timeinfo;

  if (!getLocalTime(&timeinfo)) {

    display.setCursor(15, 30);

    display.print("Time Error");

    display.display();

    return;
  }

  // ----------------------------------------------------------
  // 12 HOUR FORMAT
  // ----------------------------------------------------------

  int hour12 = timeinfo.tm_hour;

  String ampm;

  if (hour12 >= 12) {

    ampm = "PM";

  }
  else {

    ampm = "AM";
  }

  if (hour12 == 0) {

    hour12 = 12;

  }
  else if (hour12 > 12) {

    hour12 -= 12;
  }

  // ----------------------------------------------------------
  // DATE
  // ----------------------------------------------------------

  display.setTextSize(1);

  display.setCursor(32, 18);

  display.printf(
    "%02d-%02d-%04d",
    timeinfo.tm_mday,
    timeinfo.tm_mon + 1,
    timeinfo.tm_year + 1900
  );

  // ----------------------------------------------------------
  // TIME
  // ----------------------------------------------------------

  display.setTextSize(2);

  display.setCursor(5, 36);

  display.printf(
    "%02d:%02d:%02d",
    hour12,
    timeinfo.tm_min,
    timeinfo.tm_sec
  );

  // ----------------------------------------------------------
  // AM / PM
  // ----------------------------------------------------------

  display.setTextSize(1);

  display.setCursor(105, 42);

  display.print(ampm);

  display.display();
}

// ============================================================
// POMODORO DISPLAY
// ============================================================

void drawPomodoro() {

  drawHeader("POMODORO TIMER");

  // Timer
  display.setTextSize(2);

  display.setCursor(35, 25);

  display.printf(
    "%02d:%02d",
    pomoMinutes,
    pomoSeconds
  );

  // Status
  display.setTextSize(1);

  display.setCursor(5, 52);

  if (pomoRunning) {

    display.print("HOLD: PAUSE");

  }
  else {

    display.print("HOLD: START 2x: RESET");
  }

  display.display();
}

// ============================================================
// POMODORO TIMER
// ============================================================

void updatePomodoro() {

  if (!pomoRunning) {

    return;
  }

  if (
    millis() - lastPomoTick >= 1000
  ) {

    lastPomoTick += 1000;

    // --------------------------------------------------------
    // SECOND COUNTDOWN
    // --------------------------------------------------------

    if (pomoSeconds > 0) {

      pomoSeconds--;

    }

    // --------------------------------------------------------
    // MINUTE COUNTDOWN
    // --------------------------------------------------------

    else {

      if (pomoMinutes > 0) {

        pomoMinutes--;

        pomoSeconds = 59;

      }

      // ------------------------------------------------------
      // TIMER FINISHED
      // ------------------------------------------------------

      else {

        pomoRunning = false;

        Serial.println(
          "POMODORO FINISHED!"
        );

        // Alarm
        pomodoroFinishedBeep();

        // Reset
        pomoMinutes = 25;

        pomoSeconds = 0;
      }
    }
  }
}
