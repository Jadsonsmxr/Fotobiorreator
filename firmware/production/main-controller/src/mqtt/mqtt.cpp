#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <sensors/temperature.h>
#include <sensors/ph.h>
#include <sensors/co2.h>
#include <esp_wifi.h>
#include <sensors/light.h>
#include <main.h>



// DEFINIÇÕES das variáveis (apenas aqui!)
const char* SSID = "Jadson";
const char* PASSWORD = "12345678*";
const char* BROKER_MQTT = "test.mosquitto.org";
const int BROKER_PORT = 1883;
const char* ID_MQTT = "esp32_mqtt_fotobiorreator_cba";

const char* NAMESPACE = "cba_fotobiorreator";

// IDs dos sensores (devem existir no banco de dados do servidor)
const int SENSOR_ID_CO2_INTERNO = 1;     // CO2 Interno
const int SENSOR_ID_CO2_EXTERNO = 2;     // CO2 Externo
const int SENSOR_ID_TEMP_INTERNO = 3;   // Temperatura Interno
const int SENSOR_ID_PH = 4;              // pH
const int SENSOR_ID_LUMINOSIDADE = 5;    // Luminosidade


WiFiClient wifiClient;
PubSubClient MQTT(wifiClient);

int wifiChannel = 0;

float co2_mqtt = 400.0; //valor inicial do CO2

void conectaWiFi() {
  static unsigned long lastTry = 0;
  static bool canalCapturado = false;
  
  if (WiFi.status() != WL_CONNECTED) {
    canalCapturado = false;
  }


  if (WiFi.status() == WL_CONNECTED) {
    if (!canalCapturado) {
      wifiChannel = WiFi.channel();
      Serial.print("Conectado com IP: ");
      Serial.println(WiFi.localIP());
      Serial.print("Canal WiFi capturado: ");
      Serial.println(wifiChannel);
      canalCapturado = true;
    }
    
    return;
  }
  if (millis() - lastTry < 5000) {
    return; // tenta conectar a cada 10 segundos
  }
  lastTry = millis();

  Serial.print("Conectando na rede: ");
  Serial.println(SSID);
  //esp_wifi_set_channel(WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE);
  WiFi.begin(SSID, PASSWORD, WIFI_ESPNOW_CHANNEL);

  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(100);
  //   Serial.print(".");
  // }

  
}

void conectaMQTT() {
static unsigned long lastTry = 0; 

  if (MQTT.connected()) {
    return;
  }
  if (millis() - lastTry < 5000) {
    return; // tenta conectar a cada 10 segundos
  }
  lastTry = millis();

  //while (!MQTT.connected()) {
  Serial.print("Conectando no broker MQTT: ");
  Serial.println(BROKER_MQTT);

  if (MQTT.connect(ID_MQTT)) {
    Serial.println("Conectado ao broker MQTT!");
  } else {
    Serial.println("Falha na conexão. Tentando novamente em 10 segundos.");
     // delay(10000);
    //}
  }
}

void mantemConexoes() {
    conectaWiFi();

  if (!MQTT.connected()) {
    conectaMQTT();
  }
  
}

// Função para publicar no formato JSON com ID numérico
void publicarSensor(int sensor_id, float value) {
  // Cria documento JSON
  JsonDocument doc;
  doc["sensor_id"] = sensor_id;  // ID numérico do banco
  doc["value"] = value;
  
  // Serializa para string
  char jsonBuffer[256];
  serializeJson(doc, jsonBuffer);
  
  // Monta o tópico: namespace/sensors/{sensor_id}/data
  char topic[100];
  snprintf(topic, sizeof(topic), "%s/sensors/%d/data", NAMESPACE, sensor_id);
  
  // Publica
  if (MQTT.publish(topic, jsonBuffer)) {
    Serial.print("[");
    Serial.print(topic);
    Serial.print("] ");
    Serial.println(jsonBuffer);
  } else {
    Serial.println("Erro ao publicar MQTT");
  }
}

void mqtt_setup() {
    conectaWiFi();

    


    MQTT.setServer(BROKER_MQTT, BROKER_PORT);
    MQTT.setBufferSize(512);
    conectaMQTT();

    Serial.println("\n========== CONFIGURAÇÃO MQTT ==========");
  Serial.print("Namespace: ");
  Serial.println(NAMESPACE);
  Serial.println("\nTópicos configurados:");
  Serial.println("- " + String(NAMESPACE) + "/sensors/3/data  → Temperatura");
  Serial.println("- " + String(NAMESPACE) + "/sensors/4/data  → pH");
  Serial.println("- " + String(NAMESPACE) + "/sensors/1/data  → CO2 Interno");
  Serial.println("=======================================\n");
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
        
        //publicarSensor(SENSOR_ID_CO2_INTERNO, co2_mqtt);
        break;
      case 1:
        publicarSensor(SENSOR_ID_TEMP_INTERNO, temperature);
        
        break;
      case 2:
        publicarSensor(SENSOR_ID_PH, ph_act);
        break;
      case 3:
        step = 0;
        return;
    }
    step++;
  }




}