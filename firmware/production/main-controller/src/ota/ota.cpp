#include <Arduino.h>
#include <ArduinoOTA.h>
#include <HTTPClient.h>
#include <Update.h>
#include <WiFi.h>
#include <espnow/espnow.h>
#include <mqtt/mqtt.h>
#include <ota/ota.h>

static char otaHostname[32] = "fotobiorreator-esp32";
static char otaPassword[32] = "ota123";
static char firmwareUrl[192] = "http://192.168.2.105:8080/firmware.bin";

static OtaBusyHook busyHook = nullptr;
static bool arduinoOtaReady = false;
static bool arduinoOtaInProgress = false;
static bool httpOtaInProgress = false;

static void copyConfigValue(char* destination, size_t destinationSize, const char* value) {
  if (value == nullptr || destinationSize == 0) {
    return;
  }

  strncpy(destination, value, destinationSize - 1);
  destination[destinationSize - 1] = '\0';
}

static bool otaCanStartUpdate() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("OTA indisponivel: WiFi desconectado.");
    return false;
  }

  if (busyHook != nullptr && busyHook()) {
    Serial.println("OTA bloqueada: atuador em operacao.");
    return false;
  }

  if (httpOtaInProgress) {
    Serial.println("OTA HTTP ja em andamento.");
    return false;
  }

  return true;
}

static void ensureArduinoOtaReady() {
  static unsigned long lastBeginAttempt = 0;
  const unsigned long beginRetryIntervalMs = 5000;

  if (WiFi.status() != WL_CONNECTED) {
    arduinoOtaReady = false;
    return;
  }

  if (arduinoOtaReady || millis() - lastBeginAttempt < beginRetryIntervalMs) {
    return;
  }
  lastBeginAttempt = millis();

  ArduinoOTA.setHostname(otaHostname);
  ArduinoOTA.setPassword(otaPassword);

  ArduinoOTA.onStart([]() {
    arduinoOtaInProgress = true;
    mqtt_disconnect();
    espnow_stop();
    Serial.println("ArduinoOTA iniciado.");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    static uint8_t lastPercent = 255;
    uint8_t percent = (total == 0) ? 0 : (progress * 100U) / total;

    if (percent != lastPercent) {
      Serial.printf("ArduinoOTA progresso: %u%%\n", percent);
      lastPercent = percent;
    }
  });

  ArduinoOTA.onEnd([]() {
    arduinoOtaInProgress = false;
    Serial.println("ArduinoOTA concluido com sucesso.");
  });

  ArduinoOTA.onError([](ota_error_t error) {
    arduinoOtaInProgress = false;
    Serial.printf("ArduinoOTA falhou. erro=%u\n", error);
  });

  ArduinoOTA.begin();
  arduinoOtaReady = true;

  Serial.print("ArduinoOTA pronto. Hostname: ");
  Serial.println(otaHostname);
}

void ota_setup() {
  ensureArduinoOtaReady();
}

void ota_loop() {
  ensureArduinoOtaReady();

  if (busyHook != nullptr && busyHook()) {
    return;
  }

  if (arduinoOtaReady && WiFi.status() == WL_CONNECTED && !httpOtaInProgress) {
    ArduinoOTA.handle();
  }
}

void ota_setBusyHook(OtaBusyHook hook) {
  busyHook = hook;
}

void ota_setHostname(const char* hostname) {
  copyConfigValue(otaHostname, sizeof(otaHostname), hostname);
  arduinoOtaReady = false;
}

void ota_setPassword(const char* password) {
  copyConfigValue(otaPassword, sizeof(otaPassword), password);
  arduinoOtaReady = false;
}

void ota_setFirmwareUrl(const char* url) {
  copyConfigValue(firmwareUrl, sizeof(firmwareUrl), url);
}

bool ota_isUpdateInProgress() {
  return arduinoOtaInProgress || httpOtaInProgress;
}

bool ota_triggerHttpUpdate() {
  if (!otaCanStartUpdate()) {
    return false;
  }

  WiFiClient client;
  HTTPClient http;

  Serial.print("OTA HTTP iniciando download: ");
  Serial.println(firmwareUrl);

  httpOtaInProgress = true;

  if (!http.begin(client, firmwareUrl)) {
    Serial.println("OTA HTTP falhou ao iniciar conexao.");
    httpOtaInProgress = false;
    return false;
  }

  int httpCode = http.GET();
  if (httpCode != HTTP_CODE_OK) {
    Serial.printf("OTA HTTP falhou. codigo=%d\n", httpCode);
    http.end();
    httpOtaInProgress = false;
    return false;
  }

  int contentLength = http.getSize();
  if (contentLength <= 0) {
    Serial.println("OTA HTTP falhou: tamanho do firmware invalido.");
    http.end();
    httpOtaInProgress = false;
    return false;
  }

  if (!Update.begin(contentLength)) {
    Serial.printf("OTA HTTP falhou ao iniciar Update. erro=%s\n", Update.errorString());
    http.end();
    httpOtaInProgress = false;
    return false;
  }

  WiFiClient* stream = http.getStreamPtr();
  size_t written = Update.writeStream(*stream);

  if (written != (size_t)contentLength) {
    Serial.printf("OTA HTTP incompleto. recebido=%u esperado=%d\n",
                  (unsigned int)written,
                  contentLength);
  }

  if (!Update.end()) {
    Serial.printf("OTA HTTP falhou ao finalizar. erro=%s\n", Update.errorString());
    http.end();
    httpOtaInProgress = false;
    return false;
  }

  if (!Update.isFinished()) {
    Serial.println("OTA HTTP falhou: imagem incompleta.");
    http.end();
    httpOtaInProgress = false;
    return false;
  }

  http.end();
  httpOtaInProgress = false;

  Serial.println("OTA HTTP concluido com sucesso. Reiniciando...");
  Serial.flush();
  ESP.restart();
  return true;
}
