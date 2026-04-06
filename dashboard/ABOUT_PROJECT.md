# About The Project

## Apresentação

O Fotobiorreator Web Dashboard é uma plataforma web desenvolvida para apoiar o monitoramento operacional de um fotobiorreator por meio de visualização de dados, atualização em tempo real e análise assistida por imagens.

A proposta do projeto é reunir, em uma única interface, informações de sensores, indicadores operacionais e recursos de apoio à interpretação visual da biomassa, oferecendo uma base tecnológica para acompanhamento, estudo e evolução de sistemas de cultivo monitorados digitalmente.

## Objetivo

O projeto tem como objetivo disponibilizar um ambiente integrado para:

- acompanhar leituras de sensores do fotobiorreator em tempo real;
- organizar e apresentar indicadores operacionais em um dashboard web;
- apoiar a análise da biomassa a partir de imagens;
- registrar correções manuais para melhoria contínua do classificador;
- criar uma base tecnológica capaz de evoluir para cenários de operação, pesquisa e validação experimental.

## Problema Enfrentado

Sistemas de monitoramento aplicados a fotobiorreatores frequentemente exigem a consolidação de múltiplas fontes de informação, como leituras de sensores, interpretação visual do cultivo e acompanhamento histórico do processo. Quando esses dados ficam dispersos ou dependem de rotinas manuais, o acompanhamento se torna menos eficiente e mais sujeito a inconsistências.

Este projeto busca reduzir esse problema ao centralizar aquisição, visualização e análise em uma interface única, facilitando a leitura operacional do sistema e criando condições para evolução futura do monitoramento inteligente.

## Solução Proposta

A solução combina três frentes principais:

1. Monitoramento web em tempo real
Um dashboard baseado em Flask, Socket.IO e MQTT recebe dados dos sensores, persiste as leituras e atualiza a interface dinamicamente.

2. Organização visual e operacional
A interface apresenta KPIs, gráficos e gauges em um painel integrado ao restante do sistema, mantendo uma experiência unificada para operação e acompanhamento.

3. Classificação de biomassa por imagem
O sistema inclui uma página específica para envio de imagens do fotobiorreator, execução de inferência local, visualização do resultado, correção manual de rótulos e retreino supervisionado do modelo.

## Diferenciais do Projeto

- integração de monitoramento em tempo real com análise por imagem no mesmo ambiente;
- interface web única para operação e apoio à análise;
- uso de ROI manual para restringir a área útil da imagem do fotobiorreator;
- armazenamento de correções manuais para evolução supervisionada do classificador;
- arquitetura modular, preparada para expansão futura.

## Arquitetura em Alto Nível

O projeto foi construído com backend em Flask, frontend server-side com Jinja2 e componentes visuais do tema Black Dashboard.

Em alto nível, o funcionamento é o seguinte:

- sensores publicam leituras em um broker MQTT;
- a aplicação consome e persiste essas leituras;
- eventos são distribuídos ao frontend via Socket.IO;
- o dashboard apresenta dados em tempo real;
- imagens enviadas pelo usuário são processadas pelo módulo de classificação de biomassa;
- resultados podem ser corrigidos e reutilizados para retreino.

## Estado Atual

Atualmente, o projeto já contempla:

- autenticação de usuários;
- cadastro e persistência de sensores e leituras;
- dashboard com atualização em tempo real;
- integração MQTT;
- classificação de biomassa por imagem integrada ao próprio sistema;
- seleção manual de ROI na imagem;
- salvamento de rótulos manuais;
- retreino do modelo com base em amostras supervisionadas.

## Potencial de Evolução

A base atual permite avançar para novas etapas, como:

- cálculo de KPIs operacionais mais sofisticados;
- detecção automática da região útil do fotobiorreator;
- melhoria contínua do modelo de classificação com novos dados rotulados;
- configuração de deploy mais robusta para acesso remoto contínuo;
- expansão da plataforma para cenários de demonstração, pesquisa aplicada e uso colaborativo.

## Público de Interesse

O projeto pode ser relevante para:

- pesquisa aplicada em monitoramento de processos biotecnológicos;
- prototipagem de sistemas digitais para acompanhamento experimental;
- demonstração de integração entre IoT, dashboard web e classificação por imagem;
- validação de fluxos de supervisão assistida em ambientes de cultivo monitorado.

## Considerações Finais

O Fotobiorreator Web Dashboard representa uma base funcional para integração entre monitoramento em tempo real e análise visual assistida por aprendizado de máquina. Mais do que uma interface de visualização, o sistema foi estruturado para servir como plataforma de evolução incremental, permitindo refinamento técnico, ampliação de funcionalidades e fortalecimento da confiabilidade analítica ao longo do tempo.
