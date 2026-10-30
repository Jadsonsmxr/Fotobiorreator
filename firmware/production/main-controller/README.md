# Controlador principal

Firmware para **NodeMCU-32S / ESP32**, com Arduino e PlatformIO.
Lê sensores, envia dados por MQTT e ESP-NOW e executa o agendamento local dos atuadores.

[Todos os projetos de firmware](../../README.md) · [Dashboard](../../../dashboard/README.md)

## Configurar

Abra esta pasta no PlatformIO. Copie `include/secrets_example.h` para
`include/secrets_local.h` e preencha:

| Campo | Valor |
| --- | --- |
| `WIFI_SSID` | Nome da rede Wi-Fi |
| `WIFI_PASSWORD` | Senha da rede |
| `MQTT_BROKER_HOST` | IP ou endereço do broker acessível pelo ESP32 |
| `MQTT_BROKER_PORT` | Porta do broker, normalmente 1883 |
| `MQTT_NAMESPACE` | Prefixo dos tópicos; por exemplo, `cba_fotobiorreator` |

O arquivo local é ignorado pelo Git. Sem ele, o código usa os valores de exemplo.
O endereço `127.0.0.1` não aponta para o computador a partir do ESP32:
use o endereço do computador ou servidor que executa o broker.

## Compilar e gravar por USB

No terminal do PlatformIO, dentro desta pasta:

```bash
pio run
pio run --target upload
pio device monitor --baud 115200
```

O primeiro comando compila, o segundo grava na placa conectada e o terceiro abre
o monitor serial. O PlatformIO instala as dependências de `platformio.ini`.

Confira no monitor a conexão Wi-Fi, a conexão MQTT e as leituras. Depois, abra
**Sistema** no dashboard para conferir a conexão do lado da aplicação.
O comando de gravação atua no hardware; confirme a placa conectada antes de executá-lo.

Referências: [compilação e gravação](https://docs.platformio.org/en/latest/core/userguide/cmd_run.html)
e [monitor serial](https://docs.platformio.org/en/latest/core/userguide/device/cmd_monitor.html).

## Sensores e conexões

Mapa extraído do código atual; confira a montagem e a calibração antes de usar os valores.

| Componente | Conexão no código | ID MQTT | Estado no programa principal |
| --- | --- | --- | --- |
| CO₂ interno | ADS1115, canal AIN1 | 1 | Leitura e publicação ativas; calibração a validar |
| CO₂ externo | Sem leitura no fluxo principal | 2 | ID reservado, sem publicação |
| Temperatura interna | OneWire, GPIO 14 | 3 | Leitura e publicação ativas |
| pH | ADS1115, canal AIN0 | 4 | Leitura e publicação ativas |
| Luminosidade | BH1750, I²C | 5 | Inicialização, leitura e publicação comentadas |

O barramento I²C usa `Wire.begin()` sem pinos explícitos; confira os padrões da placa
selecionada. pH e CO₂ compartilham a instância ADS1115 e alteram seu ganho durante o
`setup`; essa configuração ainda precisa de validação conjunta.
Esta tabela descreve o software, não substitui o esquema elétrico da montagem.

### Mensagens MQTT

```text
Tópico: cba_fotobiorreator/sensors/3/data
JSON:   {"sensor_id": 3, "value": 28.5}
```

O prefixo vem de `MQTT_NAMESPACE`. A rotina avança uma posição a cada 15 segundos:
CO₂, temperatura, pH e uma posição sem publicação de luminosidade. Assim, cada sensor
ativo é publicado aproximadamente uma vez por minuto, se houver conexão.

## Atuadores e horários

| Saída | GPIO | Comportamento |
| --- | --- | --- |
| Compressor | 25 | Ativado em nível baixo |
| Ventilador | 33 | Ativado em nível alto |
| Luz | 32 | Reservada; acionamento comentado |

A agenda em `src/main.cpp` dispara às **09:30, 13:30 e 16:30**, no fuso de Manaus.
Depende da sincronização de hora pela rede. Cada sequência espera 1 segundo, liga
o ventilador, espera 5 segundos, liga o compressor por 5 minutos e mantém o
ventilador por mais 15 segundos após desligar o compressor.

Esses acionamentos são locais ao ESP32. Iniciar um ciclo de cultivo no dashboard
não dispara essa sequência.

## Onde alterar

| Assunto | Arquivo ou pasta |
| --- | --- |
| Inicialização, agenda e duração do compressor | `src/main.cpp` |
| Leitura e calibração dos sensores | `src/sensors/` |
| Conexão, tópicos, IDs e intervalo de publicação | `src/mqtt/mqtt.cpp` |
| Destinatário ESP-NOW (`peerAddress`) | `src/espnow/espnow.cpp` |
| Sequência e pinos dos atuadores | `src/actuators/actuators.cpp` |
| Atualização remota OTA | `src/ota/` |
| Constantes compartilhadas | `include/app_config.h` |
| Experimentos de CO₂ | `docs/notes/` |

OTA e ESP-NOW têm configurações próprias no código. Para a primeira gravação,
use USB; confira essas configurações antes de usar os recursos remotos.

## Verificar o funcionamento

1. Compile e confira se não há erros.
2. Confira as conexões e grave na placa correta.
3. No monitor serial, confira Wi-Fi, MQTT e leituras.
4. No dashboard, confira se os IDs 1, 3 e 4 existem e se chegam novas leituras.
5. Valide sensores, horários e atuadores na montagem antes de operar o cultivo.

A documentação foi conferida com o código; a montagem física não foi testada nesta revisão.
