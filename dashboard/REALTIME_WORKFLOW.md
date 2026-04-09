# Realtime Workflow

Objetivo: consolidar o pipeline de dados em tempo real do dashboard com foco em simplicidade, confiabilidade e previsibilidade.

## Etapas

1. Revisar a arquitetura atual do realtime
   - MQTT recebe payload em `apps/mqtt/client.py`
   - leitura passa por `realtime_processing_service.py`
   - backend salva no banco, emite `sensor_update` imediato e envia snapshot no connect via `apps/websocket.py`
   - frontend inicializa dashboard em `static/assets/js/realtime.js` e atualiza KPIs/gauges conforme os eventos chegam
   - conclusao: o snapshot inicial e util e simples; o ponto que ainda merece decisao e o tamanho/complexidade da camada de processamento das leituras
   -> concluido

2. Decidir o nivel certo de tratamento das leituras
   - a camada `realtime_processing_service.py` foi simplificada para validacoes minimas e previsiveis
   - comportamento atual: aceitar apenas valor numerico, finito e dentro de faixa quando houver regra de sensor
   - removidos: smoothing, comparacao com leitura anterior e classificacao `suspect`
   -> concluido

3. Validar atualizacao de gauges e KPIs ponta a ponta
   - bootstrap visual agora nasce neutro e so assume valores quando os eventos reais chegam
   - o snapshot inicial continua no connect via websocket
   - `static/assets/js/realtime.js` deixou de depender de host fixo `127.0.0.1` e passou a usar a origem atual da pagina
   - a desconexao do socket na troca de pagina ficou mais robusta com `pagehide` + `beforeunload`
   - `apps/services/kpi_service.py` deixou de emitir valores mockados, para nao quebrar a honestidade do estado neutro
   -> concluido

4. Consolidar logs, erros e comportamento de fallback
   - `apps/mqtt/client.py` ganhou tratamento mais claro para falhas de conexao MQTT
   - o cliente agora registra desconexao inesperada separadamente de encerramento normal
   - erros de parse e excecoes inesperadas ficaram mais explicitos nos logs
   - a aplicacao continua podendo degradar com seguranca quando o broker nao estiver disponivel e `MQTT_REQUIRED=false`
   -> concluido

5. Fechar a etapa com resumo e proximos passos
   - decisoes consolidadas:
     - manter snapshot inicial via Socket.IO
     - manter bootstrap neutro para KPIs e gauges
     - manter validacao minima de leitura, sem inteligencia excessiva
     - manter fallback seguro quando o broker estiver indisponivel
   - riscos residuais:
     - KPIs ainda nao possuem calculo real implementado
     - nao ha telemetria/monitoramento mais detalhado para a saude do realtime
     - o pipeline ainda depende de teste manual com dados reais para validacao final de comportamento
   - proximos passos recomendados:
     - implementar calculo real dos KPIs
     - testar ponta a ponta com publicacoes MQTT reais e registrar cenarios de erro
     - decidir depois, com base em uso real, se vale adicionar mais inteligencia no tratamento das leituras
   -> concluido

## Status rapido

- foco atual: consolidacao do realtime concluida
- resultado: pipeline mais simples, bootstrap honesto, logs mais claros e fallback previsivel
