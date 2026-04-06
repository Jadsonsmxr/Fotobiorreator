# Roadmap

## Visão Geral

Este roadmap organiza a evolução do Fotobiorreator Web Dashboard em etapas progressivas, priorizando estabilidade, qualidade de análise, maturidade arquitetural e capacidade de uso em cenários demonstrativos e operacionais.

A proposta não é apenas adicionar funcionalidades, mas consolidar o projeto como uma plataforma evolutiva de monitoramento e análise assistida por imagem.

## Curto Prazo

### 1. Consolidação do monitoramento atual

- externalizar configuração de MQTT para variáveis de ambiente;
- tornar a inicialização do cliente MQTT tolerante a falhas em ambiente de desenvolvimento;
- revisar pontos de configuração fixa ainda dependentes do ambiente local;
- melhorar mensagens de erro e feedback operacional no dashboard.

### 2. Refinamento da página de reconhecimento de imagem

- continuar o polimento de UX da página `img_rec`;
- melhorar a apresentação visual do resultado da análise;
- revisar microinterações, estados vazios, feedbacks e responsividade fina;
- consolidar a padronização visual da página com o restante do dashboard.

### 3. Melhoria da documentação técnica

- manter `README.md`, `ABOUT_PROJECT.md` e `PROJECT_CONTEXT.md` atualizados;
- documentar melhor dependências externas, como Mosquitto e ambiente Python;
- organizar instruções de execução local com mais previsibilidade.

## Médio Prazo

### 4. Evolução do classificador de biomassa

- ampliar a base de amostras rotuladas manualmente;
- revisar qualidade e balanceamento do dataset;
- aprimorar o processo de retreino supervisionado;
- avaliar novas features ou ajustes no pipeline atual;
- acompanhar consistência entre predição automática e rotulagem manual.

### 5. Evolução do uso de ROI

- consolidar o uso da ROI manual como parte do fluxo padrão de análise;
- ampliar o dataset de anotações em `roi_annotations_dataset.csv`;
- utilizar essas anotações como base para experimentos de detecção automática da área útil do fotobiorreator;
- reduzir influência de fundo, reflexo e ruído externo na classificação.

### 6. Maturidade de dados e indicadores

- substituir KPIs hoje mockados por indicadores calculados a partir das leituras reais;
- revisar persistência de dados do classificador e integração com o restante do sistema;
- melhorar rastreabilidade entre leitura operacional, imagem analisada e decisão registrada.

## Longo Prazo

### 7. Detecção automática do fotobiorreator na imagem

- treinar um mecanismo de detecção automática da região útil do reator;
- usar ROI manual como dado supervisionado para essa evolução;
- reduzir dependência de intervenção manual na seleção da área de leitura.

### 8. Deploy e acesso remoto estruturado

- preparar o projeto para deploy em ambiente persistente;
- revisar stack completo para execução com Flask, MQTT, Socket.IO e classificador;
- separar configuração local e configuração de demonstração/produção;
- estruturar estratégia de persistência para banco, uploads e artefatos do modelo.

### 9. Robustez de engenharia

- adicionar testes automatizados para serviços, rotas e fluxos críticos;
- revisar observabilidade e logging;
- melhorar organização de módulos frontend específicos;
- tornar o projeto mais fácil de replicar, manter e evoluir.

## Linha de Evolução Esperada

A evolução planejada segue esta lógica:

1. estabilizar o que já existe;
2. melhorar a qualidade da análise por imagem;
3. amadurecer a base de dados e o retreino;
4. automatizar partes hoje assistidas manualmente;
5. preparar o sistema para acesso remoto e maior confiabilidade operacional.

## Resultado Esperado

Ao seguir esse roadmap, o projeto tende a evoluir de um protótipo funcional e integrado para uma plataforma mais robusta de monitoramento, análise visual e apoio à tomada de decisão em sistemas baseados em fotobiorreatores.
