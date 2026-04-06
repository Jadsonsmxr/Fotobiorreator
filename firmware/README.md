# Firmware

Este diretório reúne o código embarcado do sistema do fotobiorreator, incluindo o firmware principal do controlador e projetos auxiliares de teste de hardware.

A base atual está organizada para uso com PlatformIO e ESP32.

## Estrutura

```text
firmware/
├── production/
│   └── main-controller/   # firmware principal do controlador
├── hardware-tests/        # projetos isolados para validação de hardware
└── README.md              # documentação do módulo de firmware
```

## Módulo principal

O firmware de produção está em:

- [production/main-controller](/Users/jadsonmaciel/Documents/Fotobiorreator/firmware/production/main-controller)

### Papel do controlador principal

O `main-controller` é o firmware responsável por integrar os componentes embarcados do sistema, incluindo:

- leitura de sensores;
- publicação MQTT;
- comunicação ESP-NOW;
- rotinas de OTA;
- acionamento de atuadores;
- lógica de agendamento local.

### Ambiente embarcado

O projeto usa PlatformIO com:

- plataforma `espressif32`
- placa `nodemcu-32s`
- framework `arduino`

Arquivo principal de configuração:

- [platformio.ini](/Users/jadsonmaciel/Documents/Fotobiorreator/firmware/production/main-controller/platformio.ini)

### Bibliotecas principais do firmware de produção

Dependências declaradas atualmente:

- `Adafruit ADS1X15`
- `DallasTemperature`
- `OneWire`
- `BH1750`
- `MG811`
- `PubSubClient`
- `ArduinoJson`
- `MG811SpaceData`

### Módulos internos observados

Dentro de `src/`, o firmware principal está organizado em partes como:

- `sensors/`
- `mqtt/`
- `espnow/`
- `ota/`
- `actuators/`

Arquivo de entrada principal:

- [src/main.cpp](/Users/jadsonmaciel/Documents/Fotobiorreator/firmware/production/main-controller/src/main.cpp)

### Integração com a camada web

O firmware publica leituras via MQTT para o namespace:

- `cba_fotobiorreator`

Exemplos de tópicos observados no código:

- `cba_fotobiorreator/sensors/1/data`
- `cba_fotobiorreator/sensors/3/data`
- `cba_fotobiorreator/sensors/4/data`
- `cba_fotobiorreator/sensors/5/data`

Payload publicado:

```json
{
  "sensor_id": 1,
  "value": 425.7
}
```

Observação importante:

- no estado atual, SSID, senha do Wi-Fi e IP do broker MQTT ainda aparecem diretamente no código do firmware em `src/mqtt/mqtt.cpp`.
- antes de qualquer publicação aberta do repositório, esses dados devem ser revisados e removidos ou externalizados.

## Projetos de teste de hardware

O diretório `hardware-tests/` contém projetos separados para validação de componentes e experimentos embarcados.

Projetos identificados:

- `temperature-sensor-test`
- `ph-sensor-test`
- `light-sensor-test`
- `espnow-protocol-test`
- `espnow-communication-test`
- `display-test`

Esses testes são úteis para:

- validar sensores isoladamente;
- depurar comunicação entre dispositivos;
- testar display e interface local;
- reduzir risco antes de incorporar mudanças ao firmware principal.

### Observação sobre o display

O projeto `display-test` já possui um `README` próprio com notas práticas sobre bibliotecas e configuração visual.

Arquivo:

- [hardware-tests/display-test/README](/Users/jadsonmaciel/Documents/Fotobiorreator/firmware/hardware-tests/display-test/README)

## Como trabalhar com o firmware

Fluxo recomendado:

1. validar sensores e componentes em `hardware-tests/` quando necessário;
2. aplicar mudanças estáveis no `production/main-controller`;
3. testar comunicação MQTT e ESP-NOW;
4. validar integração com o dashboard.

## Próximos cuidados recomendados

- documentar pinagem e conexão dos sensores;
- revisar credenciais e IPs fixos no código;
- criar um mapa de sensores e IDs usados pelo dashboard;
- diferenciar claramente configuração local de configuração de demonstração;
- registrar melhor a função de cada projeto em `hardware-tests/`.
