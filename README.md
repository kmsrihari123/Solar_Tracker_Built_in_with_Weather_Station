                                                       #SOLAR TRACKER WITH BUILT-IN WEATHER STATION
  
A dual-axis solar tracking system integrated with a real-time weather station, built using Arduino Uno and ESP32.
The Arduino handles physical solar panel tracking using LDR sensors and servo motors with an LCD menu interface. The ESP32 collects atmospheric data (temperature, humidity, pressure) and pushes it to the Arduino IoT Cloud for remote monitoring.

![image alt](https://github.com/kmsrihari123/Solar_Tracker_Built_in_with_Weather_Station/blob/2d7778e2ff569ab61a9f9d778d82c8b320f711cb/Solar%20Tracker%20%2BWeather%20station.jpg)

**SYSTEM ARCHITECTURE**

```
[4x LDR Sensors] ──► [Arduino Uno] ──► [2x Servo Motors]
                          │
                    [I2C LCD 16x2]
                    [Rotary Encoder]
                          │
                     Serial (RX/TX)
                          │
                      [ESP32]
                     /        \
              [DHT11]        [BMP280]
                 \              /
           [Arduino IoT Cloud Dashboard]
```

---

## Hardware Components

| Component | Quantity | Purpose |
|---|---|---|
| Arduino Uno | 1 | Solar tracking controller |
| ESP32 | 1 | Weather station + IoT Cloud |
| LDR (Light Dependent Resistor) | 4 | Sun position sensing |
| Servo Motor (SG90/MG996R) | 2 | Horizontal & Vertical axis |
| I2C LCD 16x2 (0x27) | 1 | Menu display |
| Rotary Encoder (KY-040) | 1 | Menu navigation |
| DHT11 | 1 | Temperature & Humidity |
| BMP280 | 1 | Atmospheric Pressure |
| 18650 Li-ion Battery | 2 | Power supply |
| Buck/Boost Converter | 1 | Voltage regulation |
| 10kΩ Resistors | 4 | LDR voltage dividers |

---
## Pin Configuration

```
ARDUINO UNO
-----------------------------------------
A0          → LDR Top-Left
A1          → LDR Top-Right
A2          → LDR Bottom-Left
A3          → LDR Bottom-Right
D9  (PWM)   → Horizontal Servo Signal
D10 (PWM)   → Vertical Servo Signal
D2          → Rotary Encoder CLK
D3          → Rotary Encoder DT
D4          → Rotary Encoder SW (Button)
D6          → DHT11 Data (local)
A4  (SDA)   → LCD I2C SDA
A5  (SCL)   → LCD I2C SCL
RX0         → ESP32 TX
TX1         → ESP32 RX
5V          → LCD VCC, Encoder VCC
GND         → LCD GND, Encoder GND

ESP32
-----------------------------------------
GPIO 4      → DHT11 Data
GPIO 21     → BMP280 SDA
GPIO 22     → BMP280 SCL
TX          → Arduino RX0
RX          → Arduino TX1
3.3V        → BMP280 VCC, DHT11 VCC
GND         → BMP280 GND, DHT11 GND

POWER
-----------------------------------------
2x 18650    → Buck/Boost Converter IN
Converter   → Arduino VIN + Servo Power
```
**LIBRARIES USED**

### Arduino Uno
- `Servo.h` *(built-in)*
- `Wire.h` *(built-in)*
- `LiquidCrystal_I2C` by Frank de Brabander
- `Encoder` by Paul Stoffregen
- `DHT sensor library` by Adafruit

### ESP32
- `ArduinoIoTCloud`
- `Arduino_ConnectionHandler`
- `Adafruit BMP280 Library`
- `DHT sensor library` by Adafruit

---

## FEATURES

- **Dual-axis LDR tracking** — 4 LDRs compute sun direction, servos auto-align panel
- **Sun Time Presets** — Manual preset positions: Morning, Near Noon, Afternoon, Evening, Night
- **Rotary Encoder Menu** — Navigate modes and settings via I2C LCD
- **Weather Display** — Real-time temperature, humidity, pressure on LCD
- **IoT Cloud Dashboard** — ESP32 streams sensor data to Arduino Cloud remotely
- **18650 Battery Powered** — Portable, regulated via buck/boost converter

---

## HOW IT WORKS

1. Four LDRs are positioned at the corners of the solar panel
2. Arduino reads analog values and computes brightness difference between top/bottom and left/right pairs
3. If the difference exceeds a tolerance threshold, the corresponding servo adjusts by 1°
4. ESP32 independently reads DHT11 and BMP280 every 2 seconds and sends data to Arduino IoT Cloud
5. The LCD menu (rotary encoder controlled) lets you switch between tracking mode, weather data, and manual sun time presets

---

## SETUP INSTRUCTIONS

1. Upload `arduino_solar_tracker.ino` to Arduino Uno using Arduino IDE
2. Create an Arduino IoT Cloud account and set up a Thing with variables: `tempertature`, `humidity`, `pressure`
3. Download the generated `thingProperties.h` and place it in the `esp32_weather_station/` folder
4. Upload `esp32_weather_station.ino` to ESP32
5. Connect ESP32 TX → Arduino RX and ESP32 RX → Arduino TX
6. Power the system via 18650 batteries through the buck/boost converter

---


Made by **Sri Hari**
