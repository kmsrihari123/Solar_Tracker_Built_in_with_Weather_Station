#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Encoder.h>
#include <DHT.h>  // DHT library

// ================= HARDWARE =================
Servo horizontal;
Servo vertical;

LiquidCrystal_I2C lcd(0x27,16,2);
Encoder knob(2,3);
#define SW 4

// ===== DHT SENSOR =====
#define DHTPIN 6
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// ===== TELEMETRY FROM ESP32 =====
float temp = 0;
float hum = 0;
String serialBuffer = "";

// LDR pins
#define TL A0
#define TR A1
#define BL A2
#define BR A3

// Servo positions
int posH = 90;
int posV = 90;

// Limits
const int H_MIN = 5;
const int H_MAX = 175;
const int V_MIN = 5;
const int V_MAX = 175;

const int tolerance = 50;

// ================= MODES =================
enum Mode { SENSOR, TELEMETRY };
Mode currentMode = SENSOR;

// ================= MENU =================
enum MenuLevel { LEVEL_MAIN, LEVEL_TRACKING, LEVEL_WEATHER, LEVEL_SUNTIME };
MenuLevel level = LEVEL_MAIN;

int menuIndex = 0;
long lastPosition = -999;

// ================= SUN TIME =================
enum SunTime { MORNING, NEAR_NOON, AFTERNOON, EVENING, NIGHT };
SunTime sunTime = MORNING;

const char* sunTimeNames[] = {"Morning","Near Noon","Afternoon","Evening","Night"};

int sunPosH[] = {20, 90, 120, 180, 70};
int sunPosV[] = {40,70, 90,  60,  50};

void adjustPanelForSunTime() {
  posH = sunPosH[sunTime];
  posV = sunPosV[sunTime];
  horizontal.write(posH);
  vertical.write(posV);
}

// =====================================================
void setup()
{
  Serial.begin(9600);

  horizontal.attach(9);
  vertical.attach(10);

  horizontal.write(posH);
  vertical.write(posV);

  lcd.init();
  lcd.backlight();

  pinMode(SW, INPUT_PULLUP);

  dht.begin();  // Initialize DHT11

  lcd.setCursor(0,0);
  lcd.print("Solar Tracker");
  lcd.setCursor(0,1);
  lcd.print("Starting...");
  delay(1500);

  drawScreen();
}

////////////////////////////////////////////////////////
// ================= SERIAL RECEIVE ==================
////////////////////////////////////////////////////////
void readTelemetry() {
  if (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    parseData(line);
  }
}

void parseData(String data)
{
  int tIndex = data.indexOf("T:");
  int hIndex = data.indexOf(",H:");

  if(tIndex!=-1 && hIndex!=-1)
  {
    temp  = data.substring(tIndex+2, hIndex).toFloat();
    hum   = data.substring(hIndex+3).toFloat();
  }
}

////////////////////////////////////////////////////////
// ================= TRACKING ==================
////////////////////////////////////////////////////////
void runLDRTracking()
{
  int tl = analogRead(TL);
  int tr = analogRead(TR);
  int bl = analogRead(BL);
  int br = analogRead(BR);

  int topAvg    = (tl + tr) / 2;
  int bottomAvg = (bl + br) / 2;
  int leftAvg   = (tl + bl) / 2;
  int rightAvg  = (tr + br) / 2;

  int diffV = topAvg - bottomAvg;
  int diffH = leftAvg - rightAvg;

  if (abs(diffH) > tolerance)
  {
    //if (diffH > 0 && posH > H_MIN) posH--;
    //else if (diffH < 0 && posH < H_MAX) posH++;
    if (diffH > 0 && posH < H_MAX) posH++;   // swapped
    else if (diffH < 0 && posH > H_MIN) posH--; // swapped
    horizontal.write(posH);
  }

  if (abs(diffV) > tolerance)
  {
    if (diffV > 0 && posV < V_MAX) posV++;
    else if (diffV < 0 && posV > V_MIN) posV--;
    vertical.write(posV);
  }
}

////////////////////////////////////////////////////////
// ================= LOOP ==================
////////////////////////////////////////////////////////
void loop()
{
  readTelemetry();     // ALWAYS read ESP32
  readEncoder();

  if(currentMode == SENSOR)
    runLDRTracking();

  // Read DHT only if weather menu active
  if(level == LEVEL_WEATHER)
  {
    temp = dht.readTemperature();
    hum  = dht.readHumidity();
  }

  // Update sun panel if in sun time menu
  if(level == LEVEL_SUNTIME)
    adjustPanelForSunTime();

  delay(40);
}

////////////////////////////////////////////////////////
// ================= ENCODER ==================
////////////////////////////////////////////////////////
void readEncoder()
{
  long newPos = knob.read()/4;

  if (newPos != lastPosition)
  {
    if (newPos > lastPosition) menuIndex++;
    else menuIndex--;

    if (menuIndex < 0) menuIndex = 0;

    if(level == LEVEL_MAIN && menuIndex > 1) menuIndex = 1;
    if(level == LEVEL_TRACKING && menuIndex > 2) menuIndex = 2;
    if(level == LEVEL_WEATHER && menuIndex > 2) menuIndex = 2;
    if(level == LEVEL_SUNTIME && menuIndex > 5) menuIndex = 5; // 5 options: Morning→Night→Back

    lastPosition = newPos;
    drawScreen();
  }

  if(digitalRead(SW)==LOW)
  {
    delay(180);
    handleClick();
    drawScreen();
  }
}

////////////////////////////////////////////////////////
// ================= CLICK ==================
////////////////////////////////////////////////////////
void handleClick()
{
  if(level == LEVEL_MAIN)
  {
    if(menuIndex == 0){ level = LEVEL_TRACKING; menuIndex = 0; }
    else if(menuIndex == 1){ level = LEVEL_WEATHER; menuIndex = 0; }
  }

  else if(level == LEVEL_TRACKING)
  {
    if(menuIndex == 0){ level = LEVEL_SUNTIME; menuIndex = 0; currentMode = SENSOR; }
    else if(menuIndex == 1) currentMode = SENSOR;
    else if(menuIndex == 2){ level = LEVEL_MAIN; menuIndex = 0; }
  }

  else if(level == LEVEL_WEATHER)
  {
    if(menuIndex==2){ level = LEVEL_MAIN; menuIndex = 0; }
  }

  else if(level == LEVEL_SUNTIME)
  {
    if(menuIndex==5){ level = LEVEL_MAIN; menuIndex = 0; } // Back to main
    else sunTime = static_cast<SunTime>(menuIndex); // Select sun time
  }
}

////////////////////////////////////////////////////////
// ================= LCD ==================
////////////////////////////////////////////////////////
void drawScreen()
{
  lcd.clear();

  if(level == LEVEL_MAIN)
  {
    lcd.setCursor(0,0);
    lcd.print(menuIndex==0?">Tracking Mode":" Tracking Mode");
    lcd.setCursor(0,1);
    lcd.print(menuIndex==1?">Weather Data":" Weather Data");
  }

  else if(level == LEVEL_TRACKING)
  {
    if(menuIndex==0){
      lcd.setCursor(0,0); lcd.print(">Live Telemetry");
      lcd.setCursor(0,1); lcd.print(" Sun Time");
    }
    else if(menuIndex==1){
      lcd.setCursor(0,0); lcd.print(">Sensor Data");
      lcd.setCursor(0,1); lcd.print(" Back");
    }
    else{
      lcd.setCursor(0,0); lcd.print(">Back");
      lcd.setCursor(0,1); lcd.print(" ");
    }
  }

  else if(level == LEVEL_WEATHER)
  {
    lcd.setCursor(0,0);
    if(menuIndex==0){
      lcd.print(">Temp:");
      lcd.print(temp,1); lcd.print((char)223); lcd.print("C");
    }
    else if(menuIndex==1){
      lcd.print(">Humidity:");
      lcd.print(hum,1); lcd.print("%");
    }
    else lcd.print(">Back");
    lcd.setCursor(0,1); lcd.print("                   ");
  }

  else if(level == LEVEL_SUNTIME)
  {
    if(menuIndex<5){
      lcd.setCursor(0,0);
      lcd.print(">");
      lcd.print(sunTimeNames[menuIndex]);
      lcd.setCursor(0,1);
      lcd.print("Adjusting Panel...");
    } else {
      lcd.setCursor(0,0);
      lcd.print(">Back");
      lcd.setCursor(0,1); lcd.print(" ");
    }
  }
}