#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>


uint8_t peerAddress[] = { 0x64, 0xE8, 0x33, 0x44, 0xB9, 0xA4 };


float temperatura = 32;
float ph = 7.1;
float co2 = 415;

typedef struct {
  char topic[16];
  float value;
} struct_message;

struct_message myData;


void onDataSent(const esp_now_send_info_t *info, esp_now_send_status_t status) {
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Enviado!" : "Falhou!");
}



// void onDataRecv(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
//   Serial.print("Recebido: ");
//   Serial.write(data, len);
//   Serial.println();
// }

void sendTopic(const char *topic, float value) {
  strcpy(myData.topic, topic);  // copia o nome do tópico
  myData.value = value;         // define o valor

  esp_err_t result = esp_now_send(peerAddress, (uint8_t *)&myData, sizeof(myData));

  if (result == ESP_OK) {
    Serial.printf(" Enviando -> %s: %.1f\n", topic, value);
  } else {
    Serial.println(" Erro ao enviar");
  }
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
  //esp_now_register_recv_cb(onDataRecv);

  Serial.println("ESP-NOW inicializado com sucesso!");
}



void loop() {


  sendTopic("temp", temperatura);
  delay(1000);
  sendTopic("ph", ph);
  delay(1000);
  sendTopic("co2", co2);
  delay(1000);

  delay(5000);
 
  }