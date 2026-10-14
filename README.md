# Rifu-DeskAssistant

# 👀 Rifu-DeskAssistant

### Animated Robot Eyes • Digital Clock • Pomodoro Timer • Touch Control • Buzzer

**Rifu-DeskAssistant** is an ESP32-based smart desk companion featuring animated robot eyes, a real-time digital clock, and a Pomodoro productivity timer. Built using an SSD1306 OLED display, a TTP223 capacitive touch sensor, and a buzzer, it combines functionality with an interactive robotic personality.

This project explores embedded systems programming, I²C communication, Wi-Fi time synchronization, touch input handling, OLED graphics, and real-time animation.

---

## ✨ Features

- 👀 **Animated Robot Eyes** — Random blinking, pupil movement, and expressive eye animations.
- 🕒 **Digital Clock** — Displays the current time and date using Wi-Fi and NTP synchronization.
- 🇧🇩 **Bangladesh Standard Time** — Configured for UTC+6.
- 🍅 **Pomodoro Timer** — A 25-minute countdown for focused work sessions.
- 👆 **Touch Control** — Interactive controls using a TTP223 capacitive touch sensor.
- 🔊 **Buzzer Feedback** — Different beep patterns for user interactions and timer completion.
- 😊 **Expressive Animations** — Normal, happy, sleepy, and alert eye expressions.
- ⚡ **Responsive Operation** — Uses `millis()`-based timing for animations and buzzer patterns.

## 🛠️ Hardware Requirements

| Component | Quantity |
|---|---:|
| ESP32 DevKit V1 | 1 |
| 0.96-inch 128×64 SSD1306 OLED Display (I²C) | 1 |
| TTP223 Capacitive Touch Sensor | 1 |
| Active Buzzer | 1 |
| Jumper Wires | As required |
| Breadboard (optional) | 1 |

## 🔌 Circuit Connections

### OLED Display → ESP32

| OLED Pin | ESP32 Pin |
|---|---|
| VCC | 3.3V |
| GND | GND |
| SDA | GPIO 21 |
| SCL | GPIO 22 |

### TTP223 Touch Sensor → ESP32

| TTP223 Pin | ESP32 Pin |
|---|---|
| VCC | 3.3V |
| GND | GND |
| OUT | GPIO 4 |

### Buzzer → ESP32

| Buzzer Pin | ESP32 Pin |
|---|---|
| Positive (+) | GPIO 25 |
| Negative (−) | GND |

**Note:** Check the buzzer's voltage and current requirements before connecting it directly to an ESP32 GPIO. Use a suitable transistor driver if necessary. These connections assume a standard ESP32 DevKit V1 and a 3.3V-compatible OLED.

## 🎮 Touch Controls

| Action | Function |
|---|---|
| Single touch | Switch between Clock and Pomodoro modes |
| Double touch | Reset Pomodoro to 25:00 when paused |
| Long touch (over 600 ms) | Start or pause Pomodoro |
| Timer reaches zero | Trigger an alert and display an alert expression |

The TTP223 sensor provides a digital signal when touched. The controls assume the module outputs HIGH on touch and LOW on release.

## 👁️ Animated Eye Expressions

| Expression | Trigger |
|---|---|
| Normal eyes | Default state |
| Blinking eyes | Periodic random intervals |
| Moving pupils | Random gaze changes |
| Happy eyes | Touch interaction or starting Pomodoro |
| Sleepy eyes | Pausing Pomodoro |
| Alert eyes | Startup, reset, or timer completion |

The eye graphics are rendered on the OLED using the Adafruit GFX library.

## 🔊 Buzzer Feedback

| Event | Feedback |
|---|---|
| Startup | One short beep |
| Single touch | One short beep |
| Double touch/reset | Two short beeps |
| Start Pomodoro | One medium beep |
| Pause Pomodoro | Two medium beeps |
| Timer completion | Five alert beeps |

## 💻 Software Requirements

- [Arduino IDE](https://www.arduino.cc/en/software)
- ESP32 board support package
- Adafruit GFX Library
- Adafruit SSD1306 Library
- Wi-Fi connection with internet access for NTP synchronization

The project also uses `Wire.h`, `WiFi.h`, and `time.h`, which are available through the ESP32 Arduino core or standard Arduino toolchain as applicable.

### Install the Libraries

1. Open Arduino IDE.
2. Go to **Sketch → Include Library → Manage Libraries**.
3. Search for and install `Adafruit GFX Library`.
4. Search for and install `Adafruit SSD1306`.
5. Make sure ESP32 board support is installed through Boards Manager.

## 🚀 Getting Started

### 1. Clone the Repository

Open a terminal and run:

```bash
git clone https://github.com/RifatShikdar/Rifu-DeskAssistant.git
cd Rifu-DeskAssistant
```

Alternatively, download the repository as a ZIP file from GitHub.

### 2. Open the Project

Open the project's `.ino` sketch in Arduino IDE.

### 3. Configure Wi-Fi

Update the Wi-Fi credentials in your local sketch:

```cpp
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";
```

**Security reminder:** Never commit your real Wi-Fi password to a public repository. Replace any real credentials with placeholders before uploading your code to GitHub.

### 4. Select the Board and Port

In Arduino IDE:

- Select **ESP32 Dev Module** or the appropriate board for your hardware.
- Select the correct COM port.
- Verify that the OLED, touch sensor, and buzzer are connected correctly.

### 5. Upload the Code

1. Click **Verify** to compile the sketch.
2. Click **Upload** to flash the code to your ESP32.
3. Open Serial Monitor if your code provides debugging output.
4. After startup, the device connects to Wi-Fi, synchronizes the time through NTP, and starts the desk companion interface.

## 🧠 How It Works

The ESP32 coordinates the main functions of the device:

1. **Touch input:** Reads the TTP223 output and detects touch actions.
2. **Display rendering:** Draws the robot eyes, clock, date, and Pomodoro timer on the SSD1306 OLED.
3. **Time synchronization:** Retrieves network time through NTP and applies the UTC+6 time offset.
4. **Timer management:** Updates the Pomodoro countdown once per second.
5. **Audio feedback:** Generates buzzer patterns for user interactions and timer completion.
6. **Eye animation:** Updates blinking, pupil movement, and facial expressions using elapsed-time checks.



## 🤝 Contributing

Suggestions, bug reports, and contributions are welcome! Feel free to fork the repository, improve the animations, add features, or optimize the embedded code.



## 👨‍💻 Author

**Rifat Shikdar**

Electrical and Electronic Engineering (EEE) Undergraduate  
Embedded Systems | Robotics | Mechatronics

- **GitHub Profile:** [@RifatShikdar](https://github.com/RifatShikdar)
- **Project Repository:** [Rifu-DeskAssistant](https://github.com/RifatShikdar/Rifu-DeskAssistant)

---

⭐ If you find this project interesting, consider giving the repository a star!

