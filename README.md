# Arduino Air Quality Monitor with ESP32 and Android App

## Overview

This project is an air quality monitoring system that measures particulate matter (PM2.5 and PM10) using an SDS011 sensor, displays the results on an LCD, transmits the data via an ESP32 microcontroller, and provides real-time monitoring and notifications through a modern Android application.

<div align="center">
 <img src="docs/circuit_schematic.PNG" width="1000"/>
</div>

___

<div align="center">
 <img src="docs/android_app_screen_shot.png" width="300"/>
</div>

---

# AirQualityMonitorAndroidApp

## Overview

Android app for monitoring particulate matter (PM2.5 and PM10) from an ESP32 web server. The app periodically fetches readings, displays the latest values and history, and sends notifications when thresholds are exceeded. Built with Kotlin, Jetpack Compose, Retrofit, and Koin.

## Features

- Foreground service that periodically polls the ESP32 JSON endpoint
- Notifications when PM thresholds are exceeded
- Editable thresholds and monitoring interval
- Persisted history of successful/failed fetches
- Modern Compose UI

## Requirements

- Android Studio (latest)
- Android device with API 26+ (Oreo) or emulator
- ESP32 running the companion firmware that serves `http://<ESP32_IP>/json` with payload:
  ```json
  { "pm2.5": <float>, "pm10": <float> }
  ```

## Quick Start

1. Open this module in Android Studio:
   - File → Open → select the `AirQualityMonitorAndroidApp` directory
2. Configure the ESP32 base URL:
   - Edit `app/src/main/java/com/mutkuensert/airqualitymonitor/Module.kt`
   - Update `baseUrl("http://192.168.0.184")` to your ESP32 IP
3. Build & Run on a device:
   - Connect a device on the same Wi‑Fi network as ESP32
   - Run the `app` configuration
   - Grant the notification permission when prompted

## How It Works

- `AirQualityMonitorService` runs as a foreground service and loops every N seconds (SharedPreferences-driven) to fetch data from ESP32.
- `AirQualityRepository` performs the network call via `AirQualityService` (Retrofit) and stores a JSON-serialized history of results/failures in SharedPreferences.
- `AirQualityStateManager` exposes a StateFlow consumed by `MainViewModel` to update UI.
- `AirQualityNotification` posts high-priority alerts when thresholds are exceeded.
- Dependencies are provided via Koin in `koinModule`.

## Configuration

- Threshold defaults:
  - PM2.5: `THRESHOLD_PM_25_DEFAULT = 10`
  - PM10: `THRESHOLD_PM_10_DEFAULT = 20`
- Monitoring interval default: `MONITORING_INTERVAL_SECONDS_DEFAULT = 30`
- Runtime changes are saved immediately via the UI text fields.

Edit locations:
- Base URL: `app/src/main/java/com/mutkuensert/airqualitymonitor/Module.kt`
- Defaults/keys: `app/src/main/java/com/mutkuensert/airqualitymonitor/data/AirQualityRepository.kt`

## Permissions

Declared in `AndroidManifest.xml`:
- `INTERNET`
- `POST_NOTIFICATIONS`
- `FOREGROUND_SERVICE`
- `FOREGROUND_SERVICE_DATA_SYNC`
- `WAKE_LOCK`

The app requests the notification permission at runtime on first launch.

## Build Info

- minSdk 26, target/compileSdk 35
- Kotlin 2.0.21, AGP 8.10.1
- Major libraries: Compose BOM 2025.05.01, Retrofit 2.11.0, OkHttp 4.12.0, Koin 4.0.2, kotlinx-serialization 1.8.0, Timber 5.0.1, Chucker 4.1.0

## JSON API

- Endpoint: `GET http://<ESP32_IP>/json`
- Response:
  ```json
  {
    "pm2.5": 12.3,
    "pm10": 25.7
  }
  ```
- See `AirQualityService.kt` and `AirQualityResponse.kt` for details.

## Screens

- Main screen shows current datetime, latest PM2.5/PM10 values, editable threshold/interval fields, and a scrollable history of previous results and errors.

## Notes

- Ensure the device and ESP32 are on the same network and the ESP32 endpoint is reachable.
- The app uses a foreground service; you can stop it from the ongoing notification action.

## Module Structure

- `presentation/`: Compose UI, `MainActivity`, `MainViewModel`, UI model
- `data/`: Repository, models, state manager, Retrofit service
- `application/`: Foreground service and notifications
- `ui/theme/`: Compose theme

# Arduino Firmware

## Overview

Reads particulate matter from an SDS011 sensor, displays values on a 16x2 I2C LCD, and streams compact readings over UART to the ESP32. Also toggles a GPIO pulse to let the ESP32 know new data is available.

## Hardware

- Arduino (Uno/Nano or similar)
- SDS011 particulate matter sensor (UART)
- 16x2 LCD with I2C backpack (address 0x27)
- One GPIO connected to the ESP32 as a data-ready signal

## Wiring

- SDS011 ↔ Arduino via `SoftwareSerial`:
  - `SENSOR_RECEIVER_PIN` D3 (Arduino RX) ← SDS011 TX
  - `SENSOR_TRANSMITTER_PIN` D4 (Arduino TX) → SDS011 RX
- LCD I2C at address `0x27` (SDA/SCL per your board)
- Data-ready signal: Arduino D2 → ESP32 GPIO 15
- USB Serial (115200) used for debugging

Constants (see `arduino.ino`):
- `DATA_READY_SIGNAL_PIN = 2`
- `SENSOR_RECEIVER_PIN = 3`
- `SENSOR_TRANSMITTER_PIN = 4`
- `ESP32_DATA_HEAD = 0xAA`

## Behavior

- Initializes LCD and the SDS011 in active reporting mode
- Cycles: work for up to 60s collecting data, then puts the sensor to sleep for ~60s
- On each measurement, updates the LCD lines:
  - Line 1: `PM2.5:<value>ug/m3`
  - Line 2: `PM10:<value>ug/m3`
- Sends a compact 5-byte frame to the ESP32 over `Serial`:
  - Byte 0: `0xAA` (data head)
  - Bytes 1-2: PM2.5 low, high
  - Bytes 3-4: PM10 low, high
- Pulses D2 HIGH for ~10ms so ESP32 interrupt can mark the data as ready

## Code Layout

- `arduino.ino`: setup/loop, LCD output, signaling, and frame transmit
- `Sds011SensorHandler.{h,cpp}`: SDS011 command frames and reading/parsing
- `PmResult.h`: data container including raw low/high bytes

## Build & Flash

- Open `arduino/arduino.ino` in Arduino IDE
- Select your board/port, install dependencies: `SoftwareSerial`, `LiquidCrystal_I2C`, `TimeLib`
- Upload and open Serial Monitor at 115200 for logs

## Notes

- Ensure the ESP32 shares GND with the Arduino
- If LCD address differs, change `LiquidCrystal_I2C lcd(0x27, 16, 2);`
- Adjust work/sleep durations in `loop()` if needed

# ESP32 Firmware

## Overview

Bridges the Arduino SDS011 readings to the network. Listens for a GPIO pulse from Arduino indicating new data, reads the compact frame from UART, and serves the last reading over HTTP (`/json`) and a simple HTML page (`/`). Can connect to Wi‑Fi with saved credentials or start an Access Point fallback.

## Hardware

- ESP32 DevKit (e.g., `esp32dev`)
- GPIO 15: interrupt input for Arduino data‑ready pulse
- UART0 (USB Serial) connected to Arduino `Serial` (ensure common GND)

## Wi‑Fi Behavior

1. Attempts to connect as a station (STA) using saved credentials from NVS (Preferences).
2. If connection fails (within ~5 seconds), starts an Access Point (`SSID: AccessPoint`, password `123456789`).
3. Uses static IP configuration (edit in `src/IpConfig.h`).

Credentials sources:
- Default compile‑time values in `Credentials.h`
- Runtime‑editable via `/savePassword?ssid=...&password=...` (stored in NVS)

## Serial Protocol From Arduino

The Arduino sends 5 bytes whenever a measurement is ready:
- Byte 0: `0xAA` (DATA_HEAD)
- Bytes 1–2: PM2.5 low, high (value = ((high << 8) | low) / 10.0)
- Bytes 3–4: PM10 low, high (value = ((high << 8) | low) / 10.0)

An interrupt on GPIO 15 (RISING) sets a flag indicating data is ready. The reader then fetches the last full frame present on the serial buffer.

## HTTP Endpoints

- `/` (HTML):
  - Shows Wi‑Fi RSSI and the last PM2.5/PM10 values when available
  - Includes a simple form to set SSID and password
- `/json` (application/json):
  - Returns `{ "pm2.5": <float>, "pm10": <float> }` if data is available, or an empty body otherwise
- `/savePassword` (GET):
  - Query params: `ssid`, `password`
  - Saves credentials to NVS for future boots

See `src/Presenter.cpp` for handlers and `src/Sds011Reader.{h,cpp}` for the parser.

## Configuration

- Static IP: `src/IpConfig.h`
- Default credentials: `Credentials.h`
- Interrupt/Signal pin: `SIGNAL_PIN` in `src/main.cpp` (default 15)
- UART: uses `Serial` at 115200

## Build & Flash (PlatformIO)

- Open the `esp32/` folder in VS Code with PlatformIO
- `platformio.ini` targets `esp32dev` with Arduino framework
- Set monitor speed to `115200`
- Flash and monitor via PlatformIO tasks

## Notes

- Ensure ESP32 GND is tied to Arduino GND
- If the ESP32 never receives data, verify the data‑ready wire (Arduino D2 → ESP32 GPIO 15) and serial wiring
- Adjust Wi‑Fi timing or AP credentials in `WirelessConnector.cpp` if needed

**For questions or contributions, please open an issue or pull request on the repository.**
