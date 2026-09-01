/*
 * TUMO - planter.ino - Option 1: Threshold watering
 *
 * Reads the soil-moisture probe, shows the moisture level on the OLED, and
 * switches the relay (pump / valve) ON while the soil is drier than a fixed
 * threshold. The same values are printed over Serial as CSV.
 *
 * Board:    Grove Beginner Kit for Arduino (Arduino Uno / ATmega328P)
 * Inputs:   resistive soil-moisture probe -> A1   (external, 2-prong sensor)
 * Outputs:  OLED display SSD1315          -> I2C 0x3C  (on kit)
 *           relay module                  -> D2        (external) -> pump/valve
 *           USB serial @ 9600 baud
 *
 * Libraries (Arduino IDE -> Tools -> Manage Libraries...):
 *   - "U8g2"   (OLED)
 *
 * The on-kit DHT11 (air temp + humidity) is not used here - it comes in with
 * the climate-aware options.
 *
 * CALIBRATION - do this once per probe:
 *   1. Upload this sketch and open Tools -> Serial Monitor.
 *   2. Hold the probe in dry air and note the "raw" value  -> SOIL_RAW_DRY
 *   3. Dip the probe in a glass of water, note the "raw"    -> SOIL_RAW_WET
 *   4. Put those two numbers below and upload again.
 * (A resistive probe reads HIGH in dry soil and LOW in wet soil.)
 */

#include <Wire.h>
#include <U8g2lib.h>

const int SOIL_PIN  = A1;   // resistive soil probe (external)
const int RELAY_PIN = 2;    // D2: relay module (external) -> pump / valve

// --- calibration: raw analogRead() values, replace with your own ---
const int SOIL_RAW_DRY = 1023;   // probe in air
const int SOIL_RAW_WET = 250;    // probe in water

// water while moisture is below this %, stop once it climbs back above it
const int MOISTURE_THRESHOLD = 35;

// most Grove relay modules are active-HIGH (HIGH = pump on). If yours is
// inverted, set this to LOW.
const int RELAY_ON = HIGH;

const unsigned long SAMPLE_INTERVAL_MS = 1000;

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

unsigned long lastSample = 0;

void setup() {
  Serial.begin(115200);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, RELAY_ON == HIGH ? LOW : HIGH);   // pump off

  u8g2.begin();

  Serial.println(F("t_ms,raw,moisture_pct,watering"));
}

void loop() {
  unsigned long now = millis();
  if (now - lastSample < SAMPLE_INTERVAL_MS) return;
  lastSample = now;

  int raw = analogRead(SOIL_PIN);
  int moisture = constrain(map(raw, SOIL_RAW_DRY, SOIL_RAW_WET, 0, 100), 0, 100);

  bool watering = moisture < MOISTURE_THRESHOLD;
  digitalWrite(RELAY_PIN, watering ? RELAY_ON : (RELAY_ON == HIGH ? LOW : HIGH));

  // --- Serial (CSV) ---
  Serial.print(now);       Serial.print(',');
  Serial.print(raw);       Serial.print(',');
  Serial.print(moisture);  Serial.print(',');
  Serial.println(watering ? 1 : 0);

  // --- OLED ---
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB10_tr);
  u8g2.setCursor(0, 14);
  u8g2.print(F("Planter"));

  u8g2.setFont(u8g2_font_ncenB14_tr);
  u8g2.setCursor(0, 42);
  u8g2.print(moisture);
  u8g2.print(F(" %"));

  u8g2.setFont(u8g2_font_6x12_tr);
  u8g2.setCursor(0, 61);
  u8g2.print(watering ? F("watering...") : F("soil OK"));

  u8g2.sendBuffer();
}
