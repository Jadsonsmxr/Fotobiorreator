
#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <sensors/temperature.h>
#include <sensors/ph.h>
#include <sensors/light.h>
#include <esp_wifi.h>
#include <main.h>


static bool espnowInitialized = false;
static bool espnowReadyLogged = false;
static unsigned long channelStableSince = 0;

static const unsigned long ESPNOW_CHANNEL_STABLE_TIME_MS = 2000;

uint8_t peerAddress[] = { 0x64, 0xE8, 0x33, 0x44, 0xB9, 0xA4 };

extern float temperature;
extern float ph_act;  // usar extern quando tiver o sensor de pH
extern float lux;
extern float co2;  // usar extern quando tiver o sensor de CO2







typedef struct {
  char topic[16];
  float value;
} struct_message;

struct_message myData;

void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  if (status == ESP_NOW_SEND_SUCCESS) {
    //Serial.println("Enviado!");
  } else {
    Serial.println("Falhou!");
  }
}

void sendTopic(const char *topic, float value) {
  strncpy(myData.topic, topic, sizeof(myData.topic) - 1);
  myData.topic[sizeof(myData.topic) - 1] = '\0';
  myData.value = value;

  esp_err_t result = esp_now_send(peerAddress, (uint8_t *)&myData, sizeof(myData));

  if (result == ESP_OK) {
    //Serial.printf(" Enviando -> %s: %.1f\n", topic, value);
  } else {
    Serial.println("Erro ao enviar");
  }
}

void espnow_setup() {
  Serial.print("Meu MAC: ");
  Serial.println(WiFi.macAddress());
}

void espnow_stop() {
  if (!espnowInitialized) {
    return;
  }

  esp_now_deinit();
  espnowInitialized = false;
  espnowReadyLogged = false;
  Serial.println("ESP-NOW desativado para OTA.");
}

static bool espnowCanRun() {
  unsigned long now = millis();
  int currentChannel = WiFi.channel();

  if (currentChannel != WIFI_ESPNOW_CHANNEL) {
    channelStableSince = 0;
    return false;
  }

  if (channelStableSince == 0) {
    channelStableSince = now;
  }

  if (WiFi.status() == WL_CONNECTED) {
    return true;
  }

  return now - channelStableSince >= ESPNOW_CHANNEL_STABLE_TIME_MS;
}

static void ensureEspnowReady() {
  if (!espnowCanRun()) {
    if (espnowInitialized) {
      esp_now_deinit();
      espnowInitialized = false;
    }
    espnowReadyLogged = false;
    return;
  }

  if (espnowInitialized) {
    return;
  }

  if (esp_now_init() != ESP_OK) {
    Serial.println("Erro ao iniciar ESP-NOW");
    return;
  }

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, peerAddress, 6);
  peerInfo.channel = WIFI_ESPNOW_CHANNEL;
  peerInfo.encrypt = false;
  peerInfo.ifidx = WIFI_IF_STA;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Falha ao adicionar peer");
    esp_now_deinit();
    return;
  }

  esp_now_register_send_cb(onDataSent);
  espnowInitialized = true;

  if (!espnowReadyLogged) {
    Serial.println("ESP-NOW inicializado com sucesso!");
    espnowReadyLogged = true;
  }
}

//Versão não-bloqueante com millis()
void espnow_loop() {
  ensureEspnowReady();

  if (!espnowInitialized) {
    return;
  }

  static unsigned long lastSend = 0;
  static uint8_t step = 0;
  //aqui é possivel configurar os intervalos de envio
  unsigned long now = millis();
  unsigned long interval = (step == 3) ? 2000 : 600;
  
  if (now - lastSend >= interval) {
    lastSend = now;
    
    switch(step) {
      case 0:
        sendTopic("temp", temperature);
        break;
      case 1:
        sendTopic("ph", ph_act);
        break;
      case 2:
        sendTopic("co2", co2);
        break;
      case 3:
        step = 0;
        return;
    }
    step++;
  }
}
