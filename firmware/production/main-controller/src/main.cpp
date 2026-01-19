#include <Arduino.h>
#include <Adafruit_ADS1X15.h>
#include <sensors/ph.h>
#include <sensors/temperature.h>
#include <sensors/light.h>
#include <sensors/co2.h>
#include <sensors/co2calibration.h>
// put function declarations here:
int teste = 1;
Adafruit_ADS1115 ads;


void setup() {
  Serial.begin(115200);
  //codigo em funcionamento
  if (teste == 0) {
    sensorPH_Setup();
    sensorTemperature_Setup();
    sensorLight_Setup();
    
  }

  //codigo em testes
  else if (teste ==1) {
    sensorCO2_Setup();
    //calibrationCo2();
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  //codigo em funcionamento
  if (teste == 0) {
  sensorPH_Loop();
  sensorTemperature_Loop();
  sensorLight_Loop();
  }
 //codigo em testes
  else if (teste ==1) {
    sensorCO2_Loop();
  }
  
}