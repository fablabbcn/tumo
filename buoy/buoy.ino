/*
 * TUMO - buoy.ino - Option 1: Serial logger
 *
 * Reads the on-board sensors once per second and prints one CSV line to the
 * Serial Monitor. The red LED blinks on every reading so you can see the buoy
 * is alive.
 *
 * Board:    Grove Beginner Kit for Arduino (Arduino Uno / ATmega328P)
 * Inputs:   light sensor            -> A6        (on kit)
 *           barometer SPL07-003     -> I2C 0x77  (on kit, "SPA06-003")
 *           accelerometer LIS3DHTR  -> I2C 0x19  (on kit)
 * Output:   red LED                 -> D4        (on kit)
 *           USB serial @ 9600 baud
 *
 * Libraries (Arduino IDE -> Tools -> Manage Libraries...):
 *   - "Seeed Arduino SPA06"     (provides SPL07-003.h)
 *   - "Seeed Arduino LIS3DHTR"
 *
 * NOTE: kits made before October 2025 have a BMP280 barometer instead. If the
 * sketch says "barometer not found", use the "Grove - Barometer Sensor BMP280"
 * library and swap the baro.* calls for bmp280.getTemperature()/getPressure().
 *
 * Capturing the data: open Tools -> Serial Monitor and copy-paste the lines
 * into a spreadsheet, or run a terminal logger (screen / tio / PuTTY) that
 * saves the serial output to a file.
 */

#include <Wire.h>
#include <math.h>
#include "SPL07-003.h"
#include "LIS3DHTR.h"

const int LED_PIN   = 4;    // on-board red LED
const int LIGHT_PIN = A6;   // on-board light sensor

const unsigned long SAMPLE_INTERVAL_MS = 60;

SPL07_003 baro;
LIS3DHTR<TwoWire> accel;

unsigned long lastSample = 0;

void pulseLed() {
  digitalWrite(LED_PIN, HIGH);
  delay(40);
  digitalWrite(LED_PIN, LOW);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  Wire.begin();

  if (!baro.begin(SPL07_ADDR_DEF, &Wire)) {
    Serial.println(F("ERROR: barometer (SPL07-003) not found - check the board"));
    while (true) { pulseLed(); delay(200); }   // fast blink = fatal error
  }
  baro.setPressureConfig(SPL07_4HZ, SPL07_16SAMPLES);
  baro.setTemperatureConfig(SPL07_4HZ, SPL07_1SAMPLE);
  baro.setMode(SPL07_CONT_PRES_TEMP);

  accel.begin(Wire, 0x19);
  accel.setOutputDataRate(LIS3DHTR_DATARATE_50HZ);

  Serial.println(F("t_ms,light,temp_C,pressure_Pa,accel_g"));
}

void loop() {
  unsigned long now = millis();
  if (now - lastSample < SAMPLE_INTERVAL_MS) return;
  lastSample = now;

  int   light    = analogRead(LIGHT_PIN);   // 0..1023, higher = brighter
  float tempC    = baro.readTemperature();  // degrees C
  float pressure = baro.readPressure();     // Pa

  float ax = accel.getAccelerationX();      // g
  float ay = accel.getAccelerationY();
  float az = accel.getAccelerationZ();
  float accelG = sqrt(ax * ax + ay * ay + az * az);  // ~1.0 g when still

  Serial.print(now);          Serial.print(',');
  Serial.print(light);        Serial.print(',');
  Serial.print(tempC, 2);     Serial.print(',');
  Serial.print(pressure, 1);  Serial.print(',');
  Serial.println(accelG, 3);

  pulseLed();
}
