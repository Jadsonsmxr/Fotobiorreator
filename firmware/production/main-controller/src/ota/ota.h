#pragma once

typedef bool (*OtaBusyHook)();

void ota_setup();
void ota_loop();
void ota_setBusyHook(OtaBusyHook hook);
void ota_setHostname(const char* hostname);
void ota_setPassword(const char* password);
void ota_setFirmwareUrl(const char* url);
bool ota_isUpdateInProgress();
bool ota_triggerHttpUpdate();
