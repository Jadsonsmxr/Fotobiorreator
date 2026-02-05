#include "Arduino.h"

// defina aqui os pinos desejados
static const uint8_t COMPRESS_PIN = 32;
static const uint8_t VEN_PIN = 32;
static const uint8_t LIGHT_PIN = 33; // pino da luz (ainda não usado)

void actuators_setup() {
  pinMode(COMPRESS_PIN, OUTPUT);
  pinMode(VEN_PIN, OUTPUT);

  // estado inicial seguro
 // digitalWrite(COMPRESS_PIN, HIGH);
  digitalWrite(VEN_PIN, HIGH);
}

void actuators_loop() {
  //digitalWrite(COMPRESS_PIN, HIGH);
  digitalWrite(VEN_PIN, HIGH);
}
