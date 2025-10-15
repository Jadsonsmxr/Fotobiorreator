#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>


uint8_t peerAddress[] = { 0x64, 0xE8, 0x33, 0x44, 0xB9, 0xA4 };


void onDataSent(const esp_now_send_info_t *info, esp_now_send_status_t status) {
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Enviado!" : "Falhou!");
}



void onDataRecv(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  Serial.print("Recebido: ");
  Serial.write(data, len);
  Serial.println();
}




void setup() {
  Serial.begin(115200);

  // Inicializa Wi-Fi no modo STA
  WiFi.mode(WIFI_MODE_STA);
  delay(100);
  Serial.print("Meu MAC: ");
  Serial.println(WiFi.macAddress());

  // Inicializa o ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Erro ao iniciar ESP-NOW");
    return;
  }



  // Adiciona o outro ESP como peer
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, peerAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  peerInfo.ifidx = WIFI_IF_STA;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Falha ao adicionar peer");
    return;
  }
  esp_now_register_send_cb(onDataSent);
  // Callback de recebimento
  esp_now_register_recv_cb(onDataRecv);

  Serial.println("ESP-NOW inicializado com sucesso!");
}



void loop() {
  const char msg[] = "Ola! Tela";
  esp_now_send(peerAddress, (uint8_t *)msg, sizeof(msg));
  delay(5000);
 
  }