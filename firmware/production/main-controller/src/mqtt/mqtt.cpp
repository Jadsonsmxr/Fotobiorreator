#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <sensors/temperature.h>
#include <sensors/ph.h>
#include <sensors/co2.h>
#include <esp_wifi.h>
#include <sensors/light.h>
#include <main.h>

// DEFINICOES das variaveis (apenas aqui!)
const char* SSID = "NPN-CBA";
const char* PASSWORD = "cba12345";
// const char* SSID = "Jadson";
// const char* PASSWORD = "12345678*";
// const char* SSID = "Daniel celular";
// const char* PASSWORD = "code2702";
const char* BROKER_MQTT = "192.168.2.105";
const int BROKER_PORT = 1883;
const char* ID_MQTT = "esp32_mqtt_fotobiorreator_cba";

const char* NAMESPACE = "cba_fotobiorreator";

// IDs dos sensores (devem existir no banco de dados do servidor)
const int SENSOR_ID_CO2_INTERNO = 1;
const int SENSOR_ID_CO2_EXTERNO = 2;
const int SENSOR_ID_TEMP_INTERNO = 3;
const int SENSOR_ID_PH = 4;
const int SENSOR_ID_LUMINOSIDADE = 5;

static const unsigned long WIFI_RETRY_INTERVAL_MS = 5000;
static const unsigned long MQTT_RETRY_INTERVAL_MS = 5000;
static const unsigned long MQTT_PUBLISH_INTERVAL_MS = 500;
static const unsigned long MQTT_ERROR_LOG_INTERVAL_MS = 5000;

WiFiClient wifiClient;
PubSubClient MQTT(wifiClient);

int wifiChannel = 0;

bool mqttProntoParaPublicar() {
  return WiFi.status() == WL_CONNECTED && MQTT.connected();
}

void logErroPublicacao() {
  static unsigned long lastErrorPrint = 0;
  unsigned long now = millis();

  if (now - lastErrorPrint >= MQTT_ERROR_LOG_INTERVAL_MS) {
    Serial.print("Publicacao MQTT indisponivel. WiFi=");
    Serial.print(WiFi.status());
    Serial.print(" MQTT=");
    Serial.println(MQTT.state());
    lastErrorPrint = now;
  }
}

void conectaWiFi() {
  static unsigned long lastTry = 0;
  static bool canalCapturado = false;
  static bool firstTry = true;

  if (WiFi.status() != WL_CONNECTED) {
    canalCapturado = false;
    esp_wifi_set_channel(WIFI_ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE);
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

  if (!firstTry && millis() - lastTry < WIFI_RETRY_INTERVAL_MS) {
    return;
  }
  firstTry = false;
  lastTry = millis();

  Serial.print("Canal WiFi em fallback para ESP-NOW: ");
  Serial.println(WIFI_ESPNOW_CHANNEL);
  Serial.print("Conectando na rede: ");
  Serial.println(SSID);
  esp_wifi_set_channel(WIFI_ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE);
  WiFi.begin(SSID, PASSWORD, WIFI_ESPNOW_CHANNEL);
}

void conectaMQTT() {
  static unsigned long lastTry = 0;
  static bool firstTry = true;

  if (MQTT.connected()) {
    return;
  }

  if (WiFi.status() != WL_CONNECTED) {
    return;
  }

  if (!firstTry && millis() - lastTry < MQTT_RETRY_INTERVAL_MS) {
    return;
  }
  firstTry = false;
  lastTry = millis();

  Serial.print("Conectando no broker MQTT: ");
  Serial.println(BROKER_MQTT);

  if (MQTT.connect(ID_MQTT)) {
    Serial.println("Conectado ao broker MQTT!");
  } else {
    Serial.print("Falha na conexao MQTT. state=");
    Serial.println(MQTT.state());
  }
}

void mantemConexoes() {
  conectaWiFi();

  if (WiFi.status() != WL_CONNECTED) {
    if (MQTT.connected()) {
      MQTT.disconnect();
    }
    return;
  }

  if (!MQTT.connected()) {
    conectaMQTT();
  }
}

void mqtt_disconnect() {
  if (MQTT.connected()) {
    MQTT.disconnect();
  }
}

bool publicarSensor(int sensor_id, float value) {
  if (!mqttProntoParaPublicar()) {
    logErroPublicacao();
    return false;
  }

  JsonDocument doc;
  doc["sensor_id"] = sensor_id;
  doc["value"] = value;

  char jsonBuffer[256];
  serializeJson(doc, jsonBuffer);

  char topic[100];
  snprintf(topic, sizeof(topic), "%s/sensors/%d/data", NAMESPACE, sensor_id);

  if (MQTT.publish(topic, jsonBuffer)) {
    // Serial.print("[");
    // Serial.print(topic);
    // Serial.print("] ");
    // Serial.println(jsonBuffer);
    return true;
  }

  logErroPublicacao();
  return false;
}

void mqtt_setup() {
  WiFi.setAutoReconnect(true);
  WiFi.persistent(false);
  conectaWiFi();

  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
  MQTT.setBufferSize(512);
  MQTT.setKeepAlive(30);
  MQTT.setSocketTimeout(3);
  conectaMQTT();

  Serial.println("\n========== CONFIGURACAO MQTT ==========");
  Serial.print("Namespace: ");
  Serial.println(NAMESPACE);
  Serial.println("\nTopicos configurados:");
  Serial.println("- " + String(NAMESPACE) + "/sensors/3/data  -> Temperatura");
  Serial.println("- " + String(NAMESPACE) + "/sensors/4/data  -> pH");
  Serial.println("- " + String(NAMESPACE) + "/sensors/1/data  -> CO2 Interno");
  Serial.println("=======================================\n");
}

void mqtt_loop() {
  mantemConexoes();

  if (MQTT.connected()) {
    MQTT.loop();
  }

  if (!mqttProntoParaPublicar()) {
    return;
  }

  static unsigned long lastPublish = 0;
  static uint8_t publishStep = 0;
  unsigned long now = millis();

  if (now - lastPublish < MQTT_PUBLISH_INTERVAL_MS) {
    return;
  }
  lastPublish = now;

  switch (publishStep) {
    case 0:
      publicarSensor(SENSOR_ID_CO2_INTERNO, co2);
      break;
    case 1:
      publicarSensor(SENSOR_ID_TEMP_INTERNO, temperature);
      break;
    case 2:
      publicarSensor(SENSOR_ID_PH, ph_act);
      break;
    case 3:
      publicarSensor(SENSOR_ID_LUMINOSIDADE, luminosidade);
      break;
  }

  publishStep = (publishStep + 1) % 4;
}
