// #include <Arduino.h>
// #include <esp_now.h>
// #include <WiFi.h>
// #include <sensors/temperature.h>
// #include <sensors/ph.h>
// #include <sensors/light.h>

// uint8_t peerAddress[] = { 0x64, 0xE8, 0x33, 0x44, 0xB9, 0xA4 };

// extern float temperature;
// // float temperatura = 32;

// extern float ph_act;
// extern float lux;
// float co2 = 415;

// typedef struct {
//   char topic[16];
//   float value;
// } struct_message;

// struct_message myData;


// void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
//   Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Enviado!" : "Falhou!");
// }



// // void onDataRecv(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
// //   Serial.print("Recebido: ");
// //   Serial.write(data, len);
// //   Serial.println();
// // }

// void sendTopic(const char *topic, float value) {
//   strcpy(myData.topic, topic);  // copia o nome do tópico
//   myData.value = value;         // define o valor

//   esp_err_t result = esp_now_send(peerAddress, (uint8_t *)&myData, sizeof(myData));

//   if (result == ESP_OK) {
//     Serial.printf(" Enviando -> %s: %.1f\n", topic, value);
//   } else {
//     Serial.println(" Erro ao enviar");
//   }
// }


// void espnow_setup() {
  

//   // Inicializa Wi-Fi no modo STA
//   WiFi.mode(WIFI_MODE_STA);
//   delay(100);
//   Serial.print("Meu MAC: ");
  

//   Serial.println(WiFi.macAddress());

//   // Inicializa o ESP-NOW
//   if (esp_now_init() != ESP_OK) {
//     Serial.println("Erro ao iniciar ESP-NOW");
//     return;
//   }


//   // Adiciona o outro ESP como peer
  
//   esp_now_peer_info_t peerInfo = {};
//   memcpy(peerInfo.peer_addr, peerAddress, 6);
//   peerInfo.channel = 0;
//   peerInfo.encrypt = false;
//   peerInfo.ifidx = WIFI_IF_STA;

//   if (esp_now_add_peer(&peerInfo) != ESP_OK) {
//     Serial.println("Falha ao adicionar peer");
//     return;
//   }
//   esp_now_register_send_cb(onDataSent);
//   // Callback de recebimento
//   //esp_now_register_recv_cb(onDataRecv);

//   Serial.println("ESP-NOW inicializado com sucesso!");
// }



// void espnow_loop() {


//   sendTopic("temp", temperature);
//   delay(600);
//   sendTopic("ph", ph_act);
//   delay(600);
//   sendTopic("co2", co2);
//   delay(600);

//   delay(6000);
 
//   }

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <sensors/temperature.h>
#include <sensors/ph.h>
#include <sensors/light.h>

uint8_t peerAddress[] = { 0x64, 0xE8, 0x33, 0x44, 0xB9, 0xA4 };

extern float temperature;
extern float ph_act;  // usar extern quando tiver o sensor de pH
extern float lux;
float co2 = 415;  // usar extern quando tiver o sensor de CO2

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
  
  
  WiFi.mode(WIFI_MODE_STA);
  delay(100);
  Serial.print("Meu MAC: ");
  Serial.println(WiFi.macAddress());

  if (esp_now_init() != ESP_OK) {
    Serial.println("Erro ao iniciar ESP-NOW");
    return;
  }

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
  Serial.println("ESP-NOW inicializado com sucesso!");
}

//Versão não-bloqueante com millis()
void espnow_loop() {
  static unsigned long lastSend = 0;
  static uint8_t step = 0;
  //aqui é possivel configurar os intervalos de envio
  unsigned long now = millis();
  unsigned long interval = (step == 3) ? 4000 : 600;
  
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
