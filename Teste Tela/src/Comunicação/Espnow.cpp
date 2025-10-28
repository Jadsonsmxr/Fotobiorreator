#include <esp_now.h>
#include <WiFi.h>
#include <Display/Display_custom.h>

uint8_t peerAddress[] = { 0x24, 0x6F, 0x28, 0x77, 0xF1, 0x14 };

typedef struct struct_message {
  char topic[16];
  float value;
} struct_message;

struct_message incomingData;


// void onDataSent(const esp_now_send_info_t *info, esp_now_send_status_t status) {
//   Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Enviado!" : "Falhou!");
// }

volatile float newTemp = NAN;
volatile float newPh   = NAN;
volatile float newCo2  = NAN;
volatile bool newDataAvailable = false;


void onDataRecv(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  memcpy(&incomingData, data, sizeof(incomingData));

  //Serial.printf("Recebido -> Tópico: %s | Valor: %.2f\n", incomingData.topic, incomingData.value);

 if (strcmp(incomingData.topic, "temp") == 0) {
    newTemp = incomingData.value;
  } 
  else if (strcmp(incomingData.topic, "ph") == 0) {
    newPh = incomingData.value;
  } 
  else if (strcmp(incomingData.topic, "co2") == 0) {
    newCo2 = incomingData.value;
  } 

  newDataAvailable = true;
}


void init_communication() {
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
//   esp_now_register_send_cb(onDataSent);
  // Callback de recebimento
  esp_now_register_recv_cb(onDataRecv);

  Serial.println("ESP-NOW inicializado com sucesso!");
}

void update_data(){
    if (newDataAvailable) {
    newDataAvailable = false;
    //Serial.printf(" Atualizando UI: %.2f %.2f %.2f\n", newTemp, newPh, newCo2);
    ui_custom_update_values(newTemp, newPh, newCo2);
  }
}