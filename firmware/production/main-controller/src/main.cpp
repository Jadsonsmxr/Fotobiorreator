#include <Arduino.h>
#include <Adafruit_ADS1X15.h>
#include <sensors/ph.h>
#include <Wire.h>
#include <sensors/temperature.h>
#include <sensors/light.h>
#include <sensors/co2.h>
#include <sensors/co2calibration.h>
#include <espnow/espnow.h>
// put function declarations here:
// int teste = 1;
Adafruit_ADS1115 ads;


void setup() {
  
  Serial.begin(115200);
  
  Serial.println("Iniciando sistema...");
  Wire.begin();
  
  sensorPH_Setup();
  sensorTemperature_Setup();
  //sensorLight_Setup();
  espnow_setup();
  //sensorCO2_Setup();
  //calibrationCo2();
    
  
}

void loop() {
 
 sensorPH_Loop();
 sensorTemperature_Loop();
  //sensorLight_Loop();
 espnow_loop();
  //sensorCO2_Loop(); 
delay(10);

}