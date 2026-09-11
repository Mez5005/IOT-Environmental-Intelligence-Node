#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// --- LCD ---
LiquidCrystal_I2C lcd(0x27, 20, 4);

// --- DHT11 ---
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// --- Analog Pins ---
const int PH_PIN    = A0;
const int MQ135_PIN = A1;

// --- Smoothing ---
const int NUM_READINGS = 10;
int gasReadings[NUM_READINGS];
int phReadings[NUM_READINGS];
int readIndex = 0;
long gasTotal = 0;
long phTotal  = 0;

// --- Timing ---
unsigned long lastScreenUpdate = 0;
const long screenInterval = 1000;

unsigned long lastCloudUpdate = 0;
const long cloudInterval = 15000;

// --- Sensor Values ---
float temperature = 0.0;
float humidity    = 0.0;
int   averagedGas = 0;
float averagedPH  = 0.0;

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);

  dht.begin();

  for (int i = 0; i < NUM_READINGS; i++) {
    gasReadings[i] = 0;
    phReadings[i]  = 0;
  }

  lcd.init();
  lcd.backlight();
  lcd.clear();
}

void loop() {
  // --- Rolling Average ---
  gasTotal -= gasReadings[readIndex];
  phTotal  -= phReadings[readIndex];

  gasReadings[readIndex] = analogRead(MQ135_PIN);
  phReadings[readIndex]  = analogRead(PH_PIN);

  gasTotal += gasReadings[readIndex];
  phTotal  += phReadings[readIndex];

  readIndex++;
  if (readIndex >= NUM_READINGS) readIndex = 0;

  int rawGasAvg = gasTotal / NUM_READINGS;
  int rawPHAvg  = phTotal  / NUM_READINGS;

  // --- Gas PPM ---
  averagedGas = rawGasAvg * 0.33;
  if (averagedGas < 0) averagedGas = 0;

  // --- pH (CALIBRATED the probe) ---
  float voltagePH = rawPHAvg * (5.0 / 1023.0);
  averagedPH = 7.0 + ((1.44 - voltagePH) * 3.5);
  if (averagedPH < 0.0)  averagedPH = 0.0;
  if (averagedPH > 14.0) averagedPH = 14.0;

  unsigned long currentMillis = millis();

  // --- 1s: Read DHT11 + Update LCD & Serial ---
  if (currentMillis - lastScreenUpdate >= screenInterval) {
    lastScreenUpdate = currentMillis;

    float newTemp = dht.readTemperature();
    float newHum  = dht.readHumidity();

    if (!isnan(newTemp) && !isnan(newHum)) {
      temperature = newTemp;
      humidity    = newHum;
    }

    Serial.print("Local Stream (1s) -> Temp: ");
    Serial.print(temperature, 1);
    Serial.print("C | Hum: ");
    Serial.print(humidity, 0);
    Serial.print("% | Gas: ");
    Serial.print(averagedGas);
    Serial.print(" PPM | pH: ");
    Serial.println(averagedPH, 2);

    lcd.setCursor(0, 0);
    lcd.print("--- MONITOR LIVE ---");
    lcd.setCursor(0, 1);
    lcd.print("  T: " + String(temperature, 1) + "C | H: " + String(humidity, 0) + "%  ");
    lcd.setCursor(0, 2);
    lcd.print("Air Quality: " + String(averagedGas) + " PPM   ");
    lcd.setCursor(0, 3);
    lcd.print("Water pH: " + String(averagedPH, 2) + "    ");
  }

  // --- 15s: Send to ESP32 ---
  if (currentMillis - lastCloudUpdate >= cloudInterval) {
    lastCloudUpdate = currentMillis;

    String payload = "[" + String(temperature, 1) + ","
                         + String(humidity, 1)    + ","
                         + String(averagedGas)    + ","
                         + String(averagedPH, 2)  + "]";

    Serial1.println(payload);
    Serial.println(">> Cloud Sync Packet Dispatched: " + payload);
  }

  delay(20);
}