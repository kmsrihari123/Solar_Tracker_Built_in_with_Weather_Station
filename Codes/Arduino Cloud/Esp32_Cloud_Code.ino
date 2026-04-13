#include "arduino_secrets.h"
#include "thingProperties.h"
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <DHT.h>

// --- DHT11 setup ---
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// --- BMP280 setup ---
Adafruit_BMP280 bmp; // I2C

void setup() {
  Serial.begin(9600); // For Arduino Uno communication
  delay(1500);

  // Initialize IoT Cloud
  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  // Initialize sensors
  dht.begin();
  Wire.begin(21, 22); // SDA=21, SCL=22

  if (!bmp.begin(0x76)) {   // Try 0x77 if needed
    Serial.println("Could not find BMP280 sensor!");
    while (1);
  }
}

void loop() {
  ArduinoCloud.update();

  // --- DHT11 readings ---
  float temp = dht.readTemperature();
  float hum  = dht.readHumidity();

  if (isnan(temp) || isnan(hum)) {
    temp = 0;
    hum  = 0;
  }

  // --- BMP280 readings ---
  float pres = bmp.readPressure() / 100.0F; // hPa
  if (isnan(pres)) pres = 0;

  // --- Assign to Cloud variables ---
  tempertature = (int)temp;  // matches your Cloud variable type
  humidity     = (int)hum;
  pressure     = pres;

  // --- Send to Arduino Uno ---
  // Format: T:xx.x,H:yy.y,P:zzz.z\n
  Serial.print("T:");
  Serial.print(temp, 1);
  Serial.print(",H:");
  Serial.print(hum, 1);
  Serial.print(",P:");
  Serial.println(pres, 1);

  // Debug print to Cloud serial monitor
  Serial.print("Temp: "); Serial.print(temp); Serial.print(" °C, ");
  Serial.print("Humidity: "); Serial.print(hum); Serial.print(" %, ");
  Serial.print("Pressure: "); Serial.print(pres); Serial.println(" hPa");

  delay(2000); // 2s delay for sensor reading and transmission
}

// --- Cloud variable callbacks ---
void onTempertatureChange() {}
void onPressureChange() {}
void onHumidityChange() {}
