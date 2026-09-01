/*
 * TUMO - planter.ino - Opcao 1: Rega por limiar (threshold)
 *
 * Le a sonda de umidade do solo, mostra o nivel de umidade no OLED, e liga o
 * rele (bomba / valvula) enquanto o solo estiver mais seco que um limiar
 * fixo. Os mesmos valores sao impressos pela Serial em formato CSV.
 *
 * Placa:    Grove Beginner Kit for Arduino (Arduino Uno / ATmega328P)
 * Entradas: sonda resistiva de umidade do solo -> A1  (externa, sensor de 2 pinos)
 * Saidas:   display OLED SSD1315              -> I2C 0x3C  (na placa)
 *           modulo rele                        -> D2        (externo) -> bomba/valvula
 *           serial USB @ 115200 baud
 *
 * Bibliotecas (Arduino IDE -> Ferramentas -> Gerenciar Bibliotecas...):
 *   - "U8g2"   (OLED)
 *
 * O DHT11 da placa (temperatura + umidade do ar) nao e usado aqui - ele entra
 * nas opcoes com sensibilidade ao clima.
 *
 * CALIBRACAO - faca isso uma vez para cada sonda:
 *   1. Envie este sketch e abra Ferramentas -> Monitor Serial.
 *   2. Segure a sonda no ar seco e anote o valor "bruto" -> SOIL_RAW_DRY
 *   3. Mergulhe a sonda em um copo de agua, anote o valor -> SOIL_RAW_WET
 *   4. Coloque os dois numeros abaixo e envie o sketch novamente.
 * (Uma sonda resistiva le ALTO em solo seco e BAIXO em solo molhado.)
 */

#include <Wire.h>
#include <U8g2lib.h>

const int SOIL_PIN  = A1;   // sonda resistiva de umidade (externa)
const int RELAY_PIN = 2;    // D2: modulo rele (externo) -> bomba / valvula

// --- calibracao: valores brutos de analogRead(), troque pelos seus ---
const int SOIL_RAW_DRY = 600;   // sonda no ar
const int SOIL_RAW_WET = 150;   // sonda na agua

// rega enquanto a umidade estiver abaixo disto (%), para quando subir de novo
const int MOISTURE_THRESHOLD = 50;

// a maioria dos modulos rele Grove e ativa em nivel ALTO (HIGH = bomba ligada).
// se o seu for invertido, mude para LOW.
const int RELAY_ON = HIGH;

const unsigned long SAMPLE_INTERVAL_MS = 1000;   // tempo entre leituras (ms)

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

unsigned long lastSample = 0;

void setup() {
  Serial.begin(115200);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, RELAY_ON == HIGH ? LOW : HIGH);   // bomba desligada

  u8g2.begin();

  // cabecalho do CSV: tempo, valor bruto, umidade (%), regando (0/1)
  Serial.println(F("tempo_ms,bruto,umidade_pct,regando"));
}

void loop() {
  unsigned long now = millis();
  if (now - lastSample < SAMPLE_INTERVAL_MS) return;
  lastSample = now;

  int bruto = analogRead(SOIL_PIN);
  int umidade = constrain(map(bruto, SOIL_RAW_DRY, SOIL_RAW_WET, 0, 100), 0, 100);

  bool regando = umidade < MOISTURE_THRESHOLD;
  digitalWrite(RELAY_PIN, regando ? RELAY_ON : (RELAY_ON == HIGH ? LOW : HIGH));

  // --- Serial (CSV) ---
  Serial.print(now);       Serial.print(',');
  Serial.print(bruto);     Serial.print(',');
  Serial.print(umidade);   Serial.print(',');
  Serial.println(regando ? 1 : 0);

  // --- OLED ---
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB10_tr);
  u8g2.setCursor(0, 14);
  u8g2.print(F("Vaso"));

  u8g2.setFont(u8g2_font_ncenB14_tr);
  u8g2.setCursor(0, 42);
  u8g2.print(umidade);
  u8g2.print(F(" %"));

  u8g2.setFont(u8g2_font_6x12_tr);
  u8g2.setCursor(0, 61);
  u8g2.print(regando ? F("regando...") : F("solo OK"));

  u8g2.sendBuffer();
}
