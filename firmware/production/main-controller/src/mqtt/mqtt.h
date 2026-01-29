#pragma once

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <sensors/temperature.h>
#include <sensors/ph.h>
#include <sensors/co2.h>




void conectaWiFi();
void conectaMQTT();
void mantemConexoes();
void mqtt_setup();
void mqtt_loop();