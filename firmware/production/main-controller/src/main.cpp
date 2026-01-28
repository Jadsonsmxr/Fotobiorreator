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
// put function declarations here:
// int teste = 1;
Adafruit_ADS1115 ads;

//falta colocar para funcionar o sensor de co2

void setup() {
  
  Serial.begin(115200);
  
  Serial.println("Iniciando sistema...");
  Wire.begin();

  WiFi.mode(WIFI_STA);
  delay(100);
  
  mqtt_setup();
  //espnow_setup();

  //sensorPH_Setup();
  //sensorTemperature_Setup();
  //sensorLight_Setup();
  
  
  
  //sensorCO2_Setup();
  //calibrationCo2();
    
  
}

void loop() {
  mqtt_loop();
  //espnow_loop();
  
  //sensorPH_Loop();
  //sensorTemperature_Loop();
  //sensorLight_Loop();
 
  //sensorCO2_Loop(); 


}