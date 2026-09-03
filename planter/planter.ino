/*
 * TUMO - planter.ino - Opcao 1: Rega por limiar (threshold)
 *
 * Le a sonda de umidade do solo, mostra na tela a umidade do solo e a
 * temperatura + umidade do ar, e liga o rele (bomba / valvula) enquanto o solo
 * estiver mais seco que um limiar fixo. Os mesmos valores sao impressos pela
 * Serial em formato CSV.
 *
 * Placa:    Grove Beginner Kit for Arduino (Arduino Uno / ATmega328P)
 * Entradas: sonda resistiva de umidade do solo -> A1        (externa, 2 pinos)
 *           sensor de temp. + umidade do ar DHT20 -> I2C 0x38  (na placa)
 * Saidas:   display OLED SSD1315                -> I2C 0x3C  (na placa)
 *           modulo rele                          -> D2        (externo) -> bomba/valvula
 *           serial USB @ 115200 baud
 *
 * Bibliotecas (Arduino IDE -> Ferramentas -> Gerenciar Bibliotecas...):
 *   - "U8g2"                              (a tela; usamos o modo U8x8, leve)
 *   - "Grove Temperature And Humidity Sensor"  (DHT20)
 *
 * OBS tela: usamos a API U8x8 (so texto, sem buffer) porque o Arduino Uno tem
 * pouca memoria e o buffer completo do U8g2 (1 KB) + a biblioteca do DHT20
 * deixam a tela sem espaco. U8x8 gasta quase nada de RAM.
 *
 * OBS sensor de ar: os kits novos (a partir de out/2025) trazem o DHT20, que
 * fala por I2C (o rotulo diz "IIC"), no endereco 0x38. Os kits antigos tem um
 * DHT11 no pino D3 - nesse caso troque para  DHT dht(D3, DHT11);
 *
 * CALIBRACAO da sonda de solo - faca isso uma vez para cada sonda:
 *   1. Envie este sketch e abra Ferramentas -> Monitor Serial.
 *   2. Segure a sonda no ar seco e anote o valor "bruto" -> SOIL_RAW_DRY
 *   3. Mergulhe a sonda em um copo de agua, anote o valor -> SOIL_RAW_WET
 *   4. Coloque os dois numeros abaixo e envie o sketch novamente.
 * (Uma sonda resistiva le ALTO em solo seco e BAIXO em solo molhado.)
 */

#include <Wire.h>
#include <U8x8lib.h>
#include "Grove_Temperature_And_Humidity_Sensor.h"

const int SOIL_PIN  = A1;   // sonda resistiva de umidade do solo (externa)
const int RELAY_PIN = 2;    // D2: modulo rele (externo) -> bomba / valvula

// --- calibracao: valores brutos de analogRead(), troque pelos seus ---
const int SOIL_RAW_DRY = 600;   // sonda no ar
const int SOIL_RAW_WET = 150;   // sonda na agua

// rega enquanto a umidade do solo estiver abaixo disto (%), para quando subir
const int MOISTURE_THRESHOLD = 50;

// a maioria dos modulos rele Grove e ativa em nivel ALTO (HIGH = bomba ligada).
// se o seu for invertido, mude para LOW.
const int RELAY_ON = HIGH;

const unsigned long SAMPLE_INTERVAL_MS = 1000;   // tempo entre leituras (ms)

U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(U8X8_PIN_NONE);
DHT dht(DHT20);   // sensor de ar por I2C (endereco 0x38)

unsigned long lastSample = 0;

// ultimos valores lidos do ar (guardados caso uma leitura falhe)
float tempAr    = 0;   // graus Celsius
float umidadeAr = 0;   // % de umidade relativa

void relayOff() {
  digitalWrite(RELAY_PIN, RELAY_ON == HIGH ? LOW : HIGH);
}

void setup() {
  Serial.begin(115200);

  pinMode(RELAY_PIN, OUTPUT);
  relayOff();   // bomba desligada ao ligar

  Wire.begin();

  u8x8.begin();
  u8x8.setFlipMode(1);   // no kit a tela fica de cabeca para baixo; 0 desvira
  u8x8.setFont(u8x8_font_chroma48medium8_r);

  dht.begin();

  // cabecalho do CSV
  Serial.println(F("tempo_ms,bruto,umidade_solo_pct,temp_ar_C,umidade_ar_pct,regando"));
}

void loop() {
  unsigned long now = millis();
  if (now - lastSample < SAMPLE_INTERVAL_MS) return;
  lastSample = now;

  // --- umidade do solo ---
  int bruto = analogRead(SOIL_PIN);
  int umidadeSolo = constrain(map(bruto, SOIL_RAW_DRY, SOIL_RAW_WET, 0, 100), 0, 100);

  // --- temperatura + umidade do ar (DHT20) ---
  // readTempAndHumidity devolve 0 quando deu certo; [0] = umidade, [1] = temp.
  float ar[2] = {0};
  if (dht.readTempAndHumidity(ar) == 0) {
    umidadeAr = ar[0];
    tempAr    = ar[1];
  }

  // --- decisao de rega ---
  bool regando = umidadeSolo < MOISTURE_THRESHOLD;
  digitalWrite(RELAY_PIN, regando ? RELAY_ON : (RELAY_ON == HIGH ? LOW : HIGH));

  // --- Serial (CSV) ---
  Serial.print(now);            Serial.print(',');
  Serial.print(bruto);          Serial.print(',');
  Serial.print(umidadeSolo);    Serial.print(',');
  Serial.print(tempAr, 1);      Serial.print(',');
  Serial.print(umidadeAr, 1);   Serial.print(',');
  Serial.println(regando ? 1 : 0);

  // --- OLED (espacos no fim de cada linha apagam o texto anterior) ---
  u8x8.setCursor(0, 0);
  u8x8.print(F("Vaso            "));

  u8x8.setCursor(0, 2);
  u8x8.print(F("Solo: "));
  u8x8.print(umidadeSolo);
  u8x8.print(F(" %      "));

  u8x8.setCursor(0, 4);
  u8x8.print(F("Ar: "));
  u8x8.print(tempAr, 0);
  u8x8.print(F("C "));
  u8x8.print(umidadeAr, 0);
  u8x8.print(F("%     "));

  u8x8.setCursor(0, 6);
  u8x8.print(regando ? F("regando...      ") : F("solo OK         "));
}
