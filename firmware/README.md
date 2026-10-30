# Firmware do fotobiorreator

Código que roda no ESP32: lê sensores, publica mensagens MQTT e aciona os atuadores locais.

[Visão geral](../README.md) · [Dashboard](../dashboard/README.md)

## Começar

1. Instale o VS Code e a extensão PlatformIO IDE.
2. Abra a pasta `firmware/production/main-controller` no PlatformIO.
3. Siga o [guia do controlador](production/main-controller/README.md) para configurar a rede, compilar e gravar.

O projeto principal usa **ESP32 NodeMCU-32S**, framework Arduino e monitor serial a **115200 baud**.
Cada projeto tem seu próprio `platformio.ini`, com a placa e as bibliotecas utilizadas.

## Pastas

| Pasta | Uso |
| --- | --- |
| [production/main-controller](production/main-controller/README.md) | Controlador principal do fotobiorreator |
| `hardware-tests/temperature-sensor-test` | Testar temperatura com DallasTemperature/OneWire |
| `hardware-tests/ph-sensor-test` | Testar leitura de pH com ADS1115 |
| `hardware-tests/light-sensor-test` | Testar luminosidade com BH1750 |
| `hardware-tests/espnow-protocol-test` | Experimento com protocolo ESP-NOW |
| `hardware-tests/espnow-communication-test` | Experimento de comunicação ESP-NOW |
| [hardware-tests/display-test](hardware-tests/display-test/README.md) | Testar a tela e interface LVGL |

Abra cada teste como um projeto independente. Confira sua placa, pinos e configuração
antes de gravar: os projetos de display e comunicação também usam variantes ESP32-S3.

## Integração com o dashboard

O ESP32 e o computador precisam alcançar o mesmo broker MQTT.
O `MQTT_NAMESPACE` do firmware deve corresponder ao prefixo do `MQTT_TOPIC` do dashboard.
Os IDs publicados precisam estar cadastrados no banco da aplicação web.

O [guia do controlador](production/main-controller/README.md#sensores-e-conexões) lista os IDs e pinos atuais.

## Onde ficam os arquivos

- `src/`: código do projeto; `main.cpp` é a entrada.
- `include/`: cabeçalhos compartilhados.
- `lib/`: bibliotecas locais; preserve a documentação de terceiros.
- `test/`: espaço para testes automatizados do PlatformIO, quando existirem.
- `.pio/`: dependências e arquivos gerados pelo PlatformIO.

Consulte as [instruções oficiais do PlatformIO](https://docs.platformio.org/en/latest/integration/ide/vscode/index.html)
para instalar e usar a extensão.
