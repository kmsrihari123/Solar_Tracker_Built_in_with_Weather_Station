Solar Tracker with Weather Station
A dual-axis solar tracking system integrated with a real-time weather station, built using Arduino Uno and ESP32.
The Arduino handles physical solar panel tracking using LDR sensors and servo motors with an LCD menu interface. The ESP32 collects atmospheric data (temperature, humidity, pressure) and pushes it to the Arduino IoT Cloud for remote monitoring.

![image alt](https://github.com/kmsrihari123/Solar_Tracker_Built_in_with_Weather_Station/blob/2d7778e2ff569ab61a9f9d778d82c8b320f711cb/Solar%20Tracker%20%2BWeather%20station.jpg)

SYSTEM ARCHITECTURE:
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


