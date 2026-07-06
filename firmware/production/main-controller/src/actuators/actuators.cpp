#include "Arduino.h"

// defina aqui os pinos desejados
static const uint8_t COMPRESS_PIN = 25;
static const uint8_t VEN_PIN = 33;
static const uint8_t LIGHT_PIN = 32; // pino da luz (ainda não usado)

enum State
{
  WAIT_START,
  FAN_ON,
  COMPRESSOR_ON,
  FAN_COOLDOWN,
  FINISHED
};

static State state = FINISHED;
static unsigned long stateTimestamp = 0;
static unsigned long compressorTimeMs = 5UL * 60UL * 1000UL;

const unsigned long START_DELAY = 1000;    // 1 s caso seja necessario, pode aumentar
const unsigned long FAN_PRE_TIME = 5000;   // fan antes
const unsigned long FAN_POST_TIME = 15000; // fan depois

void actuators_setup()
{
  digitalWrite(COMPRESS_PIN, HIGH);
  digitalWrite(VEN_PIN, LOW);
  // digitalWrite(LIGHT_PIN, LOW);

  pinMode(COMPRESS_PIN, OUTPUT);
  pinMode(VEN_PIN, OUTPUT);
  // pinMode(LIGHT_PIN, OUTPUT);
}

void actuators_setCompressorTime(unsigned long durationMs)
{
  compressorTimeMs = durationMs;
}

void actuators_triggerCycle()
{
  digitalWrite(COMPRESS_PIN, HIGH);
  digitalWrite(VEN_PIN, LOW);
  // digitalWrite(LIGHT_PIN, LOW);

  state = WAIT_START;
  stateTimestamp = 0;
}

bool actuators_isBusy()
{
  return state != FINISHED;
}

void actuators_loop()
{
  // digitalWrite(COMPRESS_PIN, HIGH);
  // digitalWrite(LIGHT_PIN, HIGH);
  unsigned long now = millis();

  switch (state)
  {

  case WAIT_START:
    if (stateTimestamp == 0)
      stateTimestamp = now;
    if (now - stateTimestamp >= START_DELAY)
    {
      digitalWrite(VEN_PIN, HIGH);
      stateTimestamp = now;
      state = FAN_ON;
    }
    break;

  case FAN_ON:
    if (now - stateTimestamp >= FAN_PRE_TIME)
    {
      digitalWrite(COMPRESS_PIN, LOW);
      stateTimestamp = now;
      state = COMPRESSOR_ON;
    }
    break;

  case COMPRESSOR_ON:
    if (now - stateTimestamp >= compressorTimeMs)
    {
      digitalWrite(COMPRESS_PIN, HIGH);
      stateTimestamp = now;
      state = FAN_COOLDOWN;
    }
    break;

  case FAN_COOLDOWN:
    if (now - stateTimestamp >= FAN_POST_TIME)
    {
      digitalWrite(VEN_PIN, LOW);
      state = FINISHED;
    }
    break;

  case FINISHED:
    // ciclo encerrado
    break;
  }
}
