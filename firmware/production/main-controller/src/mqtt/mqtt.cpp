#include "mqtt/mqtt.h"


// DEFINIÇÕES das variáveis (apenas aqui!)
const char* SSID = "NPN-CBA";
const char* PASSWORD = "cba12345";
const char* BROKER_MQTT = "test.mosquitto.org";
const int BROKER_PORT = 1883;
const char* ID_MQTT = "esp32_mqtt_fotobiorreator_cba";

WiFiClient wifiClient;
PubSubClient MQTT(wifiClient);

float co2_mqtt = 400.0; //valor inicial do CO2

void conectaWiFi() {
  if (WiFi.status() == WL_CONNECTED) {
    return;
  }

  Serial.print("Conectando na rede: ");
  Serial.println(SSID);

  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Conectado com IP: ");
  Serial.println(WiFi.localIP());
}

void conectaMQTT() {
  while (!MQTT.connected()) {
    Serial.print("Conectando no broker MQTT: ");
    Serial.println(BROKER_MQTT);

    if (MQTT.connect(ID_MQTT)) {
      Serial.println("Conectado ao broker MQTT!");
    } else {
      Serial.println("Falha na conexão. Tentando novamente em 10 segundos.");
      delay(10000);
    }
  }
}

void mantemConexoes() {
    conectaWiFi();

  if (!MQTT.connected()) {
    conectaMQTT();
  }
  
}

void mqtt_setup() {
    conectaWiFi();
    MQTT.setServer(BROKER_MQTT, BROKER_PORT);
  
    conectaMQTT();
}

void mqtt_loop() {
  mantemConexoes();
  MQTT.loop();

  static unsigned long lastSend = 0;
  static uint8_t step = 0;
  //aqui é possivel configurar os intervalos de envio
  unsigned long now = millis();
  unsigned long interval = (step == 3) ? 4000 : 600;
  
  if (now - lastSend >= interval) {
    lastSend = now;
    
    switch(step) {
      case 0:
        MQTT.publish("temp_ftb_cba", String(temperature).c_str());
        break;
      case 1:
        MQTT.publish("ph_ftb_cba", String(ph_act).c_str());
        break;
      case 2:
        //MQTT.publish("co2_ftb_cba", String(co2_mqtt).c_str());
        break;
      case 3:
        step = 0;
        return;
    }
    step++;
  }




}