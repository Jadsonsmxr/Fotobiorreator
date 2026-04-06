# Fotobiorreator

Repositório principal do sistema de monitoramento do fotobiorreator, organizado em dois módulos complementares:

- `dashboard/`: aplicação web para autenticação, visualização de dados, monitoramento em tempo real e classificação de biomassa por imagem;
- `firmware/`: código embarcado do microcontrolador, responsável por leitura de sensores, controle local e publicação de dados.

A ideia do repositório é reunir, em um único lugar, tanto a camada de software de supervisão quanto a camada embarcada que interage diretamente com o hardware do sistema.

## Estrutura do repositório

```text
Fotobiorreator/
├── dashboard/   # aplicação web Flask + dashboard + classificação por imagem
├── firmware/    # firmware do microcontrolador e testes de hardware
└── README.md    # visão geral do sistema
```

## Visão geral da arquitetura

Em alto nível, o sistema funciona assim:

1. o firmware coleta leituras dos sensores conectados ao microcontrolador;
2. essas leituras são publicadas via MQTT;
3. o dashboard consome os dados, persiste as leituras e atualiza a interface em tempo real;
4. além do monitoramento, o dashboard também oferece análise de biomassa por imagem, com correção manual e retreino do classificador.

Fluxo resumido:

```text
Sensores / Atuadores
    ↓
Firmware (ESP32)
    ↓ MQTT
Broker
    ↓
Dashboard Flask
    ↓
Banco + Socket.IO + Interface Web
```

## Módulos

### Dashboard

O módulo `dashboard/` contém a aplicação web principal.

Capacidades centrais:

- autenticação de usuários;
- dashboard em tempo real com MQTT e Socket.IO;
- persistência de sensores e leituras;
- reconhecimento de biomassa por imagem;
- seleção manual de ROI na imagem;
- correção manual e retreino do classificador.

Documentação específica:

- [dashboard/README.md](/Users/jadsonmaciel/Documents/Fotobiorreator/dashboard/README.md)
- [dashboard/ABOUT_PROJECT.md](/Users/jadsonmaciel/Documents/Fotobiorreator/dashboard/ABOUT_PROJECT.md)
- [dashboard/ROADMAP.md](/Users/jadsonmaciel/Documents/Fotobiorreator/dashboard/ROADMAP.md)

### Firmware

O módulo `firmware/` contém o código embarcado do microcontrolador e os experimentos de validação de hardware.

Capacidades centrais:

- leitura de sensores;
- publicação MQTT;
- integração com Wi-Fi;
- comunicação ESP-NOW;
- rotinas locais de controle e agendamento;
- testes separados para sensores e display.

Documentação específica:

- [firmware/README.md](/Users/jadsonmaciel/Documents/Fotobiorreator/firmware/README.md)

## Organização recomendada da documentação

A documentação foi separada por responsabilidade para evitar misturar instruções de backend web com instruções de firmware embarcado.

- `README.md` na raiz: visão geral do sistema;
- `dashboard/README.md`: documentação técnica da aplicação web;
- `firmware/README.md`: documentação técnica do firmware;
- documentos complementares dentro de `dashboard/` para contexto institucional e evolução planejada.

## Observações importantes

- o `dashboard/` e o `firmware/` evoluem juntos, mas têm dependências, ferramentas e ambientes de execução diferentes;
- o dashboard pode continuar sendo refinado sem exigir alteração imediata no firmware;
- o firmware pode ser testado isoladamente em `hardware-tests/` antes de impactar o controlador principal;
- quando o repositório for publicado abertamente, vale revisar segredos, IPs fixos, credenciais Wi-Fi e arquivos locais gerados.
