#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include "time.h"

// OLED configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Wi-Fi Credentials
const char* ssid     = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

// NTP Server and Time Zone Configuration
const char* ntpServer = "pool.ntp.org";

// Adjust these two values for your local time zone:
// Formula: (GMT Offset in hours) * 3600
// Examples: EST (New York) = -5 * 3600 = -18000 | GMT (London) = 0 | CET (Paris) = 1 * 3600 = 3600
const long  gmtOffset_sec = 21600; 
const int   daylightOffset_sec = 0; // 3600 if Daylight Saving Time is active, otherwise 0

void setup() {
  Serial.begin(115200);

  // Initialize OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("Connecting to Wi-Fi...");
  display.display();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");

  // Initialize and fetch time from NTP server
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  
  display.clearDisplay();
  display.println("Syncing Time...");
  display.display();
}

void loop() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }

  // Buffers to hold formatted time and date strings
  char timeString[9];  // hh:mm:ss
  char dateString[20]; // Day, Month DD YYYY

  // Format specifiers: %H=24hr (%I=12hr), %M=Minute, %S=Second, %A=Day, %B=Month, %d=Day num, %Y=Year
  strftime(timeString, sizeof(timeString), "%I:%M:%S %p", &timeinfo);
  strftime(dateString, sizeof(dateString), "%A, %b %d", &timeinfo);

  // Update the OLED Display
  display.clearDisplay();
  
  // Draw top banner
  display.setTextSize(1);
  display.setCursor(15, 0);
  display.println("ESP32 NTP CLOCK");
  display.drawLine(0, 10, 128, 10, SSD1306_WHITE);

  // Print Time (Large text)
  display.setTextSize(2);
  display.setCursor(16, 22);
  display.println(timeString);

  // Print Date (Small text)
  display.setTextSize(1);
  display.setCursor(20, 50);
  display.println(dateString);

  display.display();
  
  delay(1000); // Update every second
}
