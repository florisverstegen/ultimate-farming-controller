// Ultimate Farming Controller - stap 1: schuifregister-test
//
// Leest een keten van 74HC165-schuifregisters uit en print de stand naar
// de seriele monitor zodra er iets verandert. Nog geen USB-gamecontroller.
//
// Knoppen schakelen naar GND en hebben een pull-up naar 3,3 V:
//   losgelaten = 1, ingedrukt = 0 (in de code omgedraaid naar "pressed").

#include <Arduino.h>

// --- Configuratie --------------------------------------------------------
constexpr uint8_t PIN_LOAD  = 2;  // 74HC165 pin 1 (SH/LD), alle chips
constexpr uint8_t PIN_CLOCK = 3;  // 74HC165 pin 2 (CLK), alle chips
constexpr uint8_t PIN_DATA  = 4;  // 74HC165 pin 9 (QH) van chip 1 (dichtst bij Pico)

constexpr uint8_t NUM_CHIPS  = 2;
constexpr uint8_t NUM_INPUTS = NUM_CHIPS * 8;

constexpr uint32_t CLOCK_DELAY_US = 2;   // ~250 kHz; verhogen bij lange kabels
constexpr uint32_t SCAN_INTERVAL_MS = 1; // 1000 scans per seconde
constexpr uint32_t DEBOUNCE_MS = 5;

// --- State ---------------------------------------------------------------
static bool stablePressed[NUM_INPUTS];
static bool lastRaw[NUM_INPUTS];
static uint32_t lastChangeMs[NUM_INPUTS];

// Leest alle bits van de keten in. Index 0 = eerste bit dat binnenkomt
// (input H van chip 1), zie printLegend() voor de volledige mapping.
static void readChain(bool raw[NUM_INPUTS]) {
  // Parallel load: neem een momentopname van alle ingangen
  digitalWrite(PIN_LOAD, LOW);
  delayMicroseconds(CLOCK_DELAY_US);
  digitalWrite(PIN_LOAD, HIGH);
  delayMicroseconds(CLOCK_DELAY_US);

  for (uint8_t i = 0; i < NUM_INPUTS; i++) {
    raw[i] = (digitalRead(PIN_DATA) == LOW);  // LOW = ingedrukt
    digitalWrite(PIN_CLOCK, HIGH);
    delayMicroseconds(CLOCK_DELAY_US);
    digitalWrite(PIN_CLOCK, LOW);
    delayMicroseconds(CLOCK_DELAY_US);
  }
}

static void printLegend() {
  Serial.println();
  Serial.println(F("=== Ultimate Farming Controller - schuifregister-test ==="));
  Serial.print(F("Chips: "));
  Serial.print(NUM_CHIPS);
  Serial.print(F(", ingangen: "));
  Serial.println(NUM_INPUTS);
  Serial.println(F("Bitvolgorde: bit 0..7 = chip 1 ingang H..A, bit 8..15 = chip 2 ingang H..A"));
  Serial.println(F("Weergave: links bit 0, '1' = ingedrukt"));
  Serial.println();
}

static void printState() {
  Serial.print(F("["));
  Serial.print(millis());
  Serial.print(F(" ms] "));
  for (uint8_t i = 0; i < NUM_INPUTS; i++) {
    if (i > 0 && i % 8 == 0) Serial.print(' ');
    Serial.print(stablePressed[i] ? '1' : '0');
  }
  Serial.print(F("  ingedrukt:"));
  bool any = false;
  for (uint8_t i = 0; i < NUM_INPUTS; i++) {
    if (stablePressed[i]) {
      Serial.print(' ');
      Serial.print(i);
      any = true;
    }
  }
  if (!any) Serial.print(F(" -"));
  Serial.println();
}

void setup() {
  pinMode(PIN_LOAD, OUTPUT);
  pinMode(PIN_CLOCK, OUTPUT);
  pinMode(PIN_DATA, INPUT);
  digitalWrite(PIN_LOAD, HIGH);
  digitalWrite(PIN_CLOCK, LOW);

  Serial.begin(115200);
  uint32_t start = millis();
  while (!Serial && millis() - start < 3000) {
    // wacht max 3 s op de seriele monitor
  }

  readChain(lastRaw);
  for (uint8_t i = 0; i < NUM_INPUTS; i++) {
    stablePressed[i] = lastRaw[i];
    lastChangeMs[i] = millis();
  }

  printLegend();
  printState();
}

void loop() {
  static uint32_t lastScan = 0;
  uint32_t now = millis();
  if (now - lastScan < SCAN_INTERVAL_MS) return;
  lastScan = now;

  bool raw[NUM_INPUTS];
  readChain(raw);

  bool changed = false;
  for (uint8_t i = 0; i < NUM_INPUTS; i++) {
    if (raw[i] != lastRaw[i]) {
      lastRaw[i] = raw[i];
      lastChangeMs[i] = now;
    }
    if (raw[i] != stablePressed[i] && now - lastChangeMs[i] >= DEBOUNCE_MS) {
      stablePressed[i] = raw[i];
      changed = true;
    }
  }

  if (changed) printState();
}
