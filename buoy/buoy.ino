/*
 * TUMO - buoy.ino - Opcao 1: Registrador serial (data logger)
 *
 * Le os sensores da placa a cada ciclo (intervalo definido por
 * SAMPLE_INTERVAL_MS) e imprime uma linha CSV no Monitor Serial. O LED
 * vermelho pisca a cada leitura para mostrar que a boia esta funcionando.
 *
 * Placa:    Grove Beginner Kit for Arduino (Arduino Uno / ATmega328P)
 * Entradas: sensor de luz              -> A6        (na placa)
 *           barometro SPL07-003        -> I2C 0x77  (na placa, "SPA06-003")
 *           acelerometro LIS3DHTR      -> I2C 0x19  (na placa)
 * Saida:    LED vermelho               -> D4        (na placa)
 *           serial USB @ 115200 baud
 *
 * Bibliotecas (Arduino IDE -> Ferramentas -> Gerenciar Bibliotecas...):
 *   - "Seeed Arduino SPA06"     (fornece o arquivo SPL07-003.h)
 *   - "Seeed Arduino LIS3DHTR"
 *
 * OBS: kits fabricados antes de outubro de 2025 tem um barometro BMP280. Se o
 * sketch indicar "barometro nao encontrado", use a biblioteca "Grove -
 * Barometer Sensor BMP280" e troque as chamadas baro.* por
 * bmp280.getTemperature()/getPressure().
 *
 * Como capturar os dados: abra Ferramentas -> Monitor Serial e copie/cole as
 * linhas em uma planilha, ou use um programa de terminal (screen / tio / PuTTY)
 * que salve a saida serial em um arquivo.
 */

#include <Wire.h>
#include <math.h>
#include "SPL07-003.h"
#include "LIS3DHTR.h"

const int LED_PIN   = 4;    // LED vermelho da placa
const int LIGHT_PIN = A6;   // sensor de luz da placa

const unsigned long SAMPLE_INTERVAL_MS = 60;   // tempo entre leituras (ms)

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
    Serial.println(F("ERRO: barometro (SPL07-003) nao encontrado - verifique a placa"));
    while (true) { pulseLed(); delay(200); }   // piscada rapida = erro fatal
  }
  baro.setPressureConfig(SPL07_4HZ, SPL07_16SAMPLES);
  baro.setTemperatureConfig(SPL07_4HZ, SPL07_1SAMPLE);
  baro.setMode(SPL07_CONT_PRES_TEMP);

  accel.begin(Wire, 0x19);
  accel.setOutputDataRate(LIS3DHTR_DATARATE_50HZ);

  // cabecalho do CSV: tempo, luz, temperatura, pressao, aceleracao
  Serial.println(F("tempo_ms,luz,temp_C,pressao_Pa,acel_g"));
}

void loop() {
  unsigned long now = millis();
  if (now - lastSample < SAMPLE_INTERVAL_MS) return;
  lastSample = now;

  int   luz      = analogRead(LIGHT_PIN);   // 0..1023, maior = mais claro
  float tempC    = baro.readTemperature();  // graus Celsius
  float pressao  = baro.readPressure();     // Pa (pascal)

  float ax = accel.getAccelerationX();      // g
  float ay = accel.getAccelerationY();
  float az = accel.getAccelerationZ();
  float acelG = sqrt(ax * ax + ay * ay + az * az);  // ~1.0 g quando parado

  Serial.print(now);          Serial.print(',');
  Serial.print(luz);          Serial.print(',');
  Serial.print(tempC, 2);     Serial.print(',');
  Serial.print(pressao, 1);   Serial.print(',');
  Serial.println(acelG, 3);

  pulseLed();
}
