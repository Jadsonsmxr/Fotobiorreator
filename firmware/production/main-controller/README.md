# Main Controller

Firmware de produção do controlador principal do fotobiorreator, baseado em PlatformIO + ESP32.

## Estrutura

- `src/main.cpp`: ponto de entrada, orquestra setup e loop principal.
- `src/sensors/`: drivers e loops dos sensores ativos.
- `src/mqtt/`: conexão Wi-Fi, MQTT e publicação das leituras.
- `src/espnow/`: envio alternativo via ESP-NOW.
- `src/ota/`: suporte a atualização OTA.
- `src/actuators/`: controle dos atuadores e ciclos agendados.
- `include/app_config.h`: constantes globais compartilhadas entre módulos.
- `include/secrets_example.h`: modelo público de configuração sensível.
- `include/secrets_local.h`: configuração local real, ignorada pelo git.
- `docs/notes/`: rascunhos e notas técnicas que não devem ficar misturados ao código de produção.

## Boas práticas adotadas

- Segredos fora do código versionado.
- Rascunhos fora de `src/`.
- Módulos organizados por responsabilidade.

## Observação

Arquivos dentro de `.pio/` e `.vscode/` são artefatos de build/IDE e não fazem parte da lógica do firmware.
