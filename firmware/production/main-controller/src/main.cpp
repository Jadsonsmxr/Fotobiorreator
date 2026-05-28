#include <Arduino.h>
#include <Adafruit_ADS1X15.h>
#include <sensors/ph.h>
#include <Wire.h>
#include <sensors/temperature.h>
#include <sensors/light.h>
#include <sensors/co2.h>
#include <sensors/co2calibration.h>
#include <espnow/espnow.h>
#include <mqtt/mqtt.h>
#include <actuators/actuators.h>
#include <ota/ota.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <stdlib.h>
#include <time.h>
#include "app_config.h"
// put function declarations here:
// int teste = 1;
Adafruit_ADS1115 ads;
//falta colocar para funcionar o sensor de co2


struct ScheduledTime {
  uint8_t hour;
  uint8_t minute;
};

static ScheduledTime scheduledTimes[] = {
  {12, 0},
  {14, 46},
  {16, 45}
};

static unsigned long scheduledCompressorDurationMs = 5UL * 60UL * 1000UL;
static int lastExecutedDay[sizeof(scheduledTimes) / sizeof(scheduledTimes[0])] = {-1, -1, -1};
static bool startupTimePrinted = false;
static const long MANAUS_GMT_OFFSET_SECONDS = -4 * 60 * 60;
static const int MANAUS_DAYLIGHT_OFFSET_SECONDS = 0;

void setupSchedulerTime() {
  configTime(MANAUS_GMT_OFFSET_SECONDS,
             MANAUS_DAYLIGHT_OFFSET_SECONDS,
             "pool.ntp.org",
             "time.nist.gov");
}

void printCurrentTimeWhenAvailable() {
  static unsigned long lastCheck = 0;
  const unsigned long checkIntervalMs = 5000;

  if (startupTimePrinted || millis() - lastCheck < checkIntervalMs) {
    return;
  }
  lastCheck = millis();

  struct tm timeInfo;
  if (getLocalTime(&timeInfo, 10)) {
    Serial.printf("Hora atual: %02d:%02d:%02d - %02d/%02d/%04d\n",
                  timeInfo.tm_hour,
                  timeInfo.tm_min,
                  timeInfo.tm_sec,
                  timeInfo.tm_mday,
                  timeInfo.tm_mon + 1,
                  timeInfo.tm_year + 1900);
    startupTimePrinted = true;
  }
}

void handleDailyActuatorSchedule() {
  static unsigned long lastCheck = 0;
  const unsigned long checkIntervalMs = 5000;
  unsigned long nowMs = millis();

  if (nowMs - lastCheck < checkIntervalMs) {
    return;
  }
  lastCheck = nowMs;

  struct tm timeInfo;
  if (!getLocalTime(&timeInfo, 10)) {
    return;
  }

  for (size_t i = 0; i < sizeof(scheduledTimes) / sizeof(scheduledTimes[0]); i++) {
    if (timeInfo.tm_hour == scheduledTimes[i].hour &&
        timeInfo.tm_min == scheduledTimes[i].minute &&
        lastExecutedDay[i] != timeInfo.tm_yday) {
      actuators_triggerCycle();
      lastExecutedDay[i] = timeInfo.tm_yday;
    }
  }
}

void setup() {

  Wire.begin();
  
  Serial.begin(115200);
  
  // Serial.println("Iniciando sistema...");
  
 

  WiFi.mode(WIFI_STA);
  
  WiFi.setSleep(false);

  // esp_wifi_set_channel(WIFI_ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE);

  
  // delay(100);
  
  

  mqtt_setup();
  espnow_setup();
  setupSchedulerTime();
  ota_setup();
  ota_setBusyHook(actuators_isBusy);

  

  // Serial.print("Canal WiFi atual: ");
  // Serial.println(wifiChannel);

  sensorPH_Setup();
  sensorTemperature_Setup();
  //sensorLight_Setup();
  
  
  
  sensorCO2_Setup();
  //calibrationCo2();
  actuators_setCompressorTime(scheduledCompressorDurationMs);
  actuators_setup();
  
}

void loop() {

  ota_loop();

  if (ota_isUpdateInProgress()) {
    return;
  }

  mqtt_loop();
  espnow_loop(); 

  sensorPH_Loop();

  sensorTemperature_Loop();

  //sensorLight_Loop();
 
  sensorCO2_Loop(); 

  printCurrentTimeWhenAvailable();
  handleDailyActuatorSchedule();
  actuators_loop(); 

  

}
