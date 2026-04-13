# Project Context

## Identificação

- Projeto: `Fotobiorreator Web Dashboard`
- Diretório principal: `dashboard/`
- Stack principal: Flask, Jinja2, SQLAlchemy, Flask-SocketIO, MQTT, Black Dashboard
- Objetivo: monitoramento e operação de um fotobiorreator com dashboard web, ingestão de dados de sensores e atualização em tempo real.

## Arquitetura Resumida

### Backend

- `run.py`
  Ponto de entrada da aplicação.

- `apps/__init__.py`
  App factory, registro de blueprints e extensões.

- `apps/config.py`
  Configuração por ambiente.

- `apps/extensions.py`
  Instância compartilhada de `SocketIO`.

- `apps/authentication/`
  Login, cadastro, OAuth, modelos de usuário e sensores.

- `apps/home/`
  Rotas das páginas HTML do dashboard.

- `apps/mqtt/client.py`
  Cliente MQTT que recebe leituras dos sensores.

- `apps/websocket.py`
  Emissão periódica de eventos Socket.IO para frontend.

- `apps/services/`
  Regras de negócio para sensores e KPIs.

### Frontend

- `templates/layouts/base.html`
  Layout base.

- `templates/includes/sidebar.html`
  Sidebar principal.

- `templates/includes/scripts.html`
  Scripts compartilhados.

- `templates/home/index.html`
  Dashboard principal.

- `templates/home/img_rec.html`
  Página de reconhecimento de imagem integrada ao dashboard.

- `static/assets/css/custom.css`
  Customizações globais do projeto.

- `static/assets/js/dashboard.js`
  Charts e gauges do dashboard principal.

- `static/assets/js/realtime.js`
  Integração Socket.IO no frontend.

## Fluxo de Dados

1. sensores/dispositivos publicam leituras no broker MQTT;
2. `apps/mqtt/client.py` consome as mensagens;
3. leituras são validadas e persistidas no banco;
4. `apps/websocket.py` emite atualizações via Socket.IO;
5. o frontend atualiza gauges e KPIs em tempo real.

## Estado Atual da Página `img_rec.html`

### Objetivo

Permitir upload de imagem do fotobiorreator, análise por API externa e exibição do resultado dentro da temática do dashboard.

### Decisões de implementação já tomadas

- a página foi integrada ao layout do dashboard, sem reaproveitar o visual standalone de `img_rec copy.html`;
- a interface segue o padrão visual do restante do projeto;
- o preview da imagem foi movido para o card lateral;
- o preview usa `object-fit: contain` para não cortar a imagem;
- o botão principal ocupa toda a largura disponível do card;
- o estado visual do botão foi harmonizado com a paleta verde/ciano do projeto;
- a página responde à troca entre tema escuro e claro;
- o bloco de crop foi removido;
- o status redundante do segundo card foi removido;
- os textos foram revisados para português correto.

### Resumo rápido do que já foi feito

- correção da estrutura Jinja e Bootstrap da página;
- integração completa da tela ao tema do dashboard;
- criação de área de upload por clique e drag-and-drop;
- botão principal estilizado com identidade verde/ciano compatível com os gauges;
- correção dos estados de hover no tema escuro e no tema claro;
- adaptação visual para troca de tema via `body.white-content`;
- remoção da área de crop;
- criação de preview dedicado no card lateral com visualização da imagem inteira;
- refinamento dos textos em português;
- sidebar configurada para marcar `img_rec` como item ativo.

### Estrutura atual da página

#### Card principal

- título e descrição da funcionalidade;
- área de upload via clique ou drag-and-drop;
- botão `Analisar biomassa`;
- indicador de loading;
- feedback visual de erro/sucesso.

#### Card lateral

- preview da imagem enviada;
- metadados do arquivo;
- instruções curtas de uso.

#### Card de resultado

- classe prevista;
- confiança principal;
- features calculadas;
- probabilidades por classe;
- correção manual de rótulo.

## Backend da Página `img_rec.html`

A página `img_rec.html` agora utiliza rotas internas do próprio backend Flask.

Base URL:

- `/api/biomass-classifier`

Endpoints atuais:

- `POST /api/biomass-classifier/predict`
- `POST /api/biomass-classifier/labels`
- `GET /api/biomass-classifier/history`
- `POST /api/biomass-classifier/retrain`

### Expectativa de retorno de `/api/biomass-classifier/predict`

O frontend espera algo compatível com:

```json
{
  "id": 1,
  "status": "Alta",
  "confianca": 0.93,
  "features": {
    "mean_green_index": 0.1234,
    "mean_gray": 145.2,
    "std_color": 12.3,
    "mean_hue": 88.1
  },
  "probabilidades": {
    "Baixa": 0.03,
    "Media": 0.12,
    "Alta": 0.80,
    "Sedimentacao": 0.05
  }
}
```

### Expectativa de envio para `/api/biomass-classifier/labels`

FormData com:

- `id`
- `status_real`

### Implementação atual

O classificador foi embutido no projeto Flask em:

- `apps/biomass_classifier/__init__.py`
- `apps/biomass_classifier/routes.py`
- `apps/biomass_classifier/service.py`
- `apps/biomass_classifier/dataset_labeler.py`
- `apps/biomass_classifier/data/training_dataset.csv`

Pontos principais da nova estrutura:

- não existe mais dependência operacional de FastAPI separado para a tela `img_rec`;
- as rotas são protegidas por autenticação do dashboard;
- a lógica de inferência, persistência e retreino ficou centralizada em um módulo dedicado dentro de `apps/`;
- os caminhos de dataset, artefatos, uploads e banco local do classificador passaram a usar `Path(__file__).resolve().parent`.

## Problemas já investigados no projeto

### Socket.IO / gauges / KPIs

Foi diagnosticado anteriormente que havia condição de corrida entre:

- inicialização dos componentes do dashboard;
- registro dos listeners do Socket.IO;
- atualização de gauges e KPIs.

Motivo principal:

- `realtime.js` começava a escutar eventos antes de `initDashboardPageCharts()` terminar de inicializar referências globais.

Isso gerava erros como:

- `refresh is not a function`
- `Cannot set properties of undefined`

Também foi identificado um segundo problema relacionado à navegação entre páginas:

- `realtime.js` estava sendo carregado globalmente pelo layout base;
- ao trocar de página, outras telas continuavam abrindo conexão Socket.IO e recebendo eventos;
- como essas páginas não possuem os elementos de KPIs e gauges da dashboard principal, surgiam erros como `Cannot set properties of undefined`.

Correção aplicada:

- o carregamento de `realtime.js` foi removido do include global de scripts;
- o módulo passou a ser incluído apenas em `templates/home/index.html`;
- a inicialização do Socket.IO agora acontece somente após `DOMContentLoaded`;
- `realtime.js` valida se a página contém os elementos esperados antes de iniciar;
- `kpis.js` e `sensors.js` receberam guards para evitar erro caso alguma referência ainda não exista.

### Switch de tema claro/escuro

Foi diagnosticado um efeito visual de "pulo" no switch de tema ao recarregar a página.

Motivo principal:

- o HTML da navbar renderiza o checkbox do switch inicialmente com `checked`;
- depois disso, apenas no `$(document).ready(...)`, o JavaScript consulta `localStorage` e corrige o estado real do switch e da classe `white-content` no `body`.

Consequência:

- a interface é pintada primeiro com um estado visual;
- em seguida o JavaScript troca para o estado salvo;
- o usuário percebe o switch "se movendo" no carregamento.

Arquivos diretamente envolvidos:

- `templates/includes/navigation.html`
- `static/assets/js/themeSettings.js`

Correção aplicada:

- o `checked` fixo do switch foi removido do HTML;
- o tema salvo passou a ser aplicado no início do carregamento da página, antes da interface principal aparecer;
- o estado do checkbox agora é sincronizado com a preferência salva sem depender apenas do `document.ready`.

Observação complementar:

- `themeSettings.js` chamava `updateGaugeColors()` como se a função existisse globalmente em todas as páginas;
- como `dashboard.js` não é carregado em todas as telas, isso passou a gerar `ReferenceError`;
- a chamada foi encapsulada com verificação segura para só executar quando `window.updateGaugeColors` existir.

## Ajustes visuais recentes

### Dashboard principal

- revisão de responsividade;
- ajustes de grid e espaçamento dos cards;
- preservação da estrutura do tema base.
- leve aumento na tipografia dos itens da sidebar para melhorar legibilidade sem descaracterizar o tema.

### Página `img_rec`

- revisão completa de layout;
- revisão das cores para tema escuro e claro;
- hover do botão corrigido para não herdar o rosa do tema base;
- preview reestruturado para leitura mais clara da imagem.
- melhoria de fluidez da experiência:
  - scroll automático para o card de resultado após análise bem-sucedida;
  - scroll para a área superior de feedback em erros, salvamento de rótulo e retreino;
  - redução da lentidão percebida com cache em memória do bundle do modelo carregado.

## Arquivos importantes para continuidade

- [run.py](/Users/jadsonmaciel/Documents/Fotobiorreator/web/dashboard/run.py)
- [apps/__init__.py](/Users/jadsonmaciel/Documents/Fotobiorreator/web/dashboard/apps/__init__.py)
- [apps/config.py](/Users/jadsonmaciel/Documents/Fotobiorreator/web/dashboard/apps/config.py)
- [apps/mqtt/client.py](/Users/jadsonmaciel/Documents/Fotobiorreator/web/dashboard/apps/mqtt/client.py)
- [apps/websocket.py](/Users/jadsonmaciel/Documents/Fotobiorreator/web/dashboard/apps/websocket.py)
- [templates/home/index.html](/Users/jadsonmaciel/Documents/Fotobiorreator/web/dashboard/templates/home/index.html)
- [templates/home/img_rec.html](/Users/jadsonmaciel/Documents/Fotobiorreator/web/dashboard/templates/home/img_rec.html)
- [templates/includes/sidebar.html](/Users/jadsonmaciel/Documents/Fotobiorreator/web/dashboard/templates/includes/sidebar.html)
- [static/assets/css/custom.css](/Users/jadsonmaciel/Documents/Fotobiorreator/web/dashboard/static/assets/css/custom.css)

## Próximos passos recomendados

### Curto prazo

- revisar o botão `Salvar rótulo manual` para harmonizar com o CTA principal;
- validar todos os textos retornados pela API para suportar rótulos com acentuação;
- testar a página `img_rec` em desktop e mobile com imagens reais.

### Itens úteis para retomada rápida

- a página usa o backend Flask interno em `/api/biomass-classifier`;
- o módulo do classificador fica em `apps/biomass_classifier/`;
- o preview foi deliberadamente mantido no card lateral para melhorar legibilidade;
- o botão principal foi desacoplado de `btn-primary` para evitar o hover rosa herdado do tema base.

### Médio prazo

- adicionar modal de zoom para o preview;
- suportar fila/estado mais explícito durante análise;
- mover a URL da API para configuração por ambiente;
- criar testes para o fluxo de upload e renderização de resultado.

## Migração do protótipo de ML

O protótipo original em `dashboard/ml/` foi substituído por uma estrutura integrada ao Flask em `apps/biomass_classifier/`.

Objetivos da migração:

- eliminar a dependência de um segundo backend paralelo;
- padronizar nomes de arquivos e módulos;
- remover imports legados como `pbr_modelo`;
- alinhar o frontend `img_rec.html` com rotas internas do projeto;
- centralizar dataset, artefato do modelo, uploads e banco do classificador em uma estrutura previsível.

## Verificação de dependências do ambiente

Foi feita uma checagem no interpretador `python3` disponível no ambiente atual.

Resultado:

- o interpretador atual não possui instaladas nem mesmo dependências-base do projeto como `Flask`, `Flask-Login`, `Flask-SQLAlchemy`, `Flask-Migrate` e `Flask-SocketIO`;
- isso indica que o ambiente ativo provavelmente não está com a virtualenv do projeto preparada;
- além disso, o `requirements.txt` ainda não cobre todas as bibliotecas novas usadas pelo módulo `apps/biomass_classifier`.

Dependências novas do classificador que precisam constar no ambiente:

- `numpy`
- `pandas`
- `Pillow`
- `joblib`
- `scikit-learn`
- `xgboost`

### Situação da instalação

- o `requirements.txt` foi atualizado com as dependências do classificador;
- foi criada uma virtualenv local em `dashboard/.venv`;
- a tentativa de instalação na `.venv` esbarrou no fato de o ambiente estar em `Python 3.14.0`;
- nesse Python, bibliotecas científicas como `numpy` e `pandas` caíram em build de source, o que indica compatibilidade menos madura e instalação potencialmente instável para este projeto.

Recomendação prática:

- usar Python `3.11` ou `3.12` para o ambiente do dashboard;
- recriar a virtualenv com essa versão antes de instalar `requirements.txt`.

Atualização posterior:

- o ambiente foi recriado com `Python 3.12.10` em `dashboard/.venv`;
- a instalação passou a resolver corretamente as wheels dos pacotes científicos;
- porém a instalação completa interrompeu por falta de espaço em disco:
  `OSError: [Errno 28] No space left on device`.

Conclusão operacional:

- a versão de Python agora está adequada;
- o bloqueio atual para concluir o ambiente é espaço disponível em disco, não compatibilidade de dependência.

Atualização mais recente:

- após liberar espaço, a instalação em `dashboard/.venv` com `Python 3.12.10` foi concluída;
- os imports principais do projeto passaram, com exceção de `xgboost`;
- o `xgboost` instalado depende da runtime OpenMP no macOS e falha ao carregar `libxgboost.dylib` sem `libomp.dylib`.

Pendência atual do ambiente:

- instalar `libomp` no macOS para habilitar o carregamento do `xgboost`.

Status final da preparação do ambiente:

- `libomp` foi instalado via Homebrew;
- o import de `xgboost` passou após essa instalação;
- a virtualenv válida do projeto ficou em `dashboard/.venv`;
- a validação final dos imports principais do projeto retornou `all imports ok`.

Observação sobre VS Code:

- ativar a virtualenv no terminal com `source dashboard/.venv/bin/activate` não obriga o VS Code a trocar automaticamente o interpretador do workspace;
- para fixar isso no projeto, foi adicionado `python.defaultInterpreterPath` em `.vscode/settings.json` apontando para `dashboard/.venv/bin/python`.
- ao usar manualmente `Python: Select Interpreter` > `Enter interpreter path`, o arquivo correto é o executável:
  `dashboard/.venv/bin/python`
- não deve ser selecionada a pasta `.venv`, a pasta `bin`, nem o script `activate`.

## Observação sobre `Web` vs `web` no Git

Foi identificado um comportamento de casing entre sistema de arquivos e índice do Git:

- no disco, a pasta existente é `web`;
- porém o `git status` referencia caminhos como `../Web/...`;
- o repositório está com `core.ignorecase=true`, o que é comum no macOS;
- isso indica que o Git preservou um nome histórico com letra maiúscula no índice, enquanto o sistema de arquivos está tratando `Web` e `web` como equivalentes.

Conclusão:

- não são duas pastas físicas distintas no disco neste momento;
- é um efeito de histórico/casing do Git em filesystem case-insensitive.

Atualização posterior:

- o índice do Git foi normalizado para usar caminhos em minúsculas;
- o padrão consolidado passou a ser `web/dashboard`;
- após essa correção, `git ls-files` deixou de listar `Web/Dashboard` e passou a listar `web/dashboard`.

## Roteiro de teste da página `img_rec`

Sequência recomendada para validar o reconhecimento de imagem:

1. ativar a virtualenv do projeto:
   `source dashboard/.venv/bin/activate`
2. subir a aplicação Flask principal:
   `cd dashboard && python run.py`
3. acessar o dashboard autenticado no navegador;
4. abrir a página `img_rec.html`;
5. enviar uma imagem JPEG ou PNG do fotobiorreator;
6. executar a análise e verificar:
   - preview da imagem;
   - retorno de classe prevista;
   - confiança;
   - probabilidades por classe;
   - gravação de rótulo manual.
7. usar o botão `Retreinar modelo` na própria `img_rec.html` para disparar `POST /api/biomass-classifier/retrain`.

Observações:

- o modelo é carregado pelo módulo `apps/biomass_classifier/service.py`;
- se o artefato do modelo ainda não existir, o serviço tenta treiná-lo automaticamente a partir de `apps/biomass_classifier/data/training_dataset.csv`;
- o primeiro teste pode demorar um pouco mais por conta desse carregamento inicial.
- o retreino só deve funcionar quando houver quantidade mínima de amostras com `manual_status` salvo.

## Como reutilizar este contexto em outra conta

Ao iniciar uma nova conversa ou trocar de conta:

1. informe o caminho do projeto;
2. peça para o agente ler este arquivo:
   `dashboard/PROJECT_CONTEXT.md`

## Observação importante

O agente novo não garante memória automática do contexto anterior.

Este arquivo existe justamente para reduzir essa perda e acelerar retomadas futuras.

Atualizacao estrutural mais recente:

- o nivel intermediario `web/` foi removido da estrutura fisica do projeto;
- o dashboard agora fica diretamente em `dashboard/` na raiz do repositorio;
- o diretorio `.vscode/` util do projeto tambem foi movido para a raiz do repositorio.

## Revisao de Git

Em 27/03/2026 foi revisado o estado do Git apos a reorganizacao estrutural.

Faz sentido commitar:

- o flattening de `Web/Dashboard` para `dashboard/`;
- `.vscode/settings.json` na raiz;
- os ajustes de frontend e dashboard feitos no periodo;
- o modulo `apps/biomass_classifier`;
- `README.md` e `PROJECT_CONTEXT.md`.

Ruido identificado no stage:

- `.DS_Store` na raiz;
- remocoes historicas de `.DS_Store` em caminhos antigos.

Esses arquivos de sistema nao devem entrar no commit final.

## Limpeza apos commit

Em 27/03/2026 o arquivo local de apoio `dashboard/templates/home/img_rec copy.html` foi removido para evitar ruido no repositorio apos o commit estrutural.

## Iteracao de UI - img_rec

Em 30/03/2026 foi iniciada uma reestruturacao incremental da pagina `img_rec.html`.

Passo 1 aplicado:

- upload e pre-visualizacao foram unificados em um unico card principal;
- a antiga divisao em dois cards concorrentes foi removida;
- a interface superior agora segue um fluxo mais continuo entre envio, conferencia e acao;
- o card lateral passou a ser uma coluna interna de apoio visual dentro do mesmo bloco principal.

A primeira proposta do passo 1 foi refeita na mesma data. A segunda direcao adotada substituiu a coluna lateral por uma estrutura em etapas visuais dentro de um card mestre, com dois paineis internos: envio e pre-visualizacao.

A interface numerada foi descartada. A direcao adotada passou a ser um workspace unico de selecao de imagem, com upload e pre-visualizacao dentro do mesmo bloco principal, evitando excesso de didatismo e melhorando a coesao visual.

A pre-visualizacao deixou de existir como bloco separado e passou a fazer parte da propria area de selecao de imagem. A zona de upload virou um workspace unico, com acao de selecao de um lado e conferencial visual da imagem no mesmo contexto.

Em 30/03/2026 a area de upload do `img_rec` foi corrigida para funcionar como um unico container com dois estados: vazio e preenchido. A mesma caixa de selecao agora vira a area de preview apos o envio da imagem, removendo a duplicacao visual e simplificando a hierarquia da interface.

A interface da caixa de imagem foi refinada para ter estado preenchido sem hover chamativo, com acao explicita de remover a imagem. O bloco de resultado tambem ganhou uma caixa interna propria para equilibrar o peso visual com a area de selecao.

Em 30/03/2026 foi feita uma nova revisao critica da UX da pagina `img_rec`. A avaliacao atual e que a tela evoluiu bem na integracao entre selecao e preview, mas ainda pode melhorar em polimento visual, consistencia semantica do bloco de resultado, linguagem da interface e refinamento do estado preenchido da caixa de imagem.

Foi alinhada a hierarquia visual entre a caixa de selecao de imagem e a caixa de resultado. As duas secoes agora seguem o mesmo padrao de titulo, subtitulo e conteudo interno, evitando a sensacao de que uma esta estruturada por dentro da caixa e a outra por fora.

- 2026-03-31: Implementada base de ROI manual na tela `img_rec` com seleção por arraste sobre o preview, envio das coordenadas para o backend, persistência no SQLite e exportação contínua em `apps/biomass_classifier/data/roi_annotations_dataset.csv` para futura evolução de um detector automático de bordas do fotobiorreator.

- 2026-03-31: O arquivo `apps/biomass_classifier/data/roi_annotations_dataset.csv` só é criado após uma nova predição ou salvamento de rótulo executados já com a versão atual do backend carregada. Se ele ainda não existir em disco, é sinal de que o fluxo novo ainda não foi disparado após o deploy/restart local.

- 2026-03-31: Ao subir `run.py`, a aplicação pode abortar antes do start do servidor Flask se o broker MQTT configurado em `apps/mqtt/client.py` (`192.168.2.105:1883`) não estiver acessível. O `start_mqtt(app)` é chamado no boot e hoje não trata falha de conexão como opcional.

- 2026-03-31: Foi discutido o momento de deploy. A orientação atual é não migrar a aplicação para Streamlit, porque o projeto já é um dashboard Flask com autenticação, Socket.IO, MQTT e UI própria. A recomendação é primeiro estabilizar configuração, responsividade, fluxo de reconhecimento e serviços, e depois pensar em deploy do stack atual (Flask + broker + banco), preferencialmente com configuração por ambiente.

- 2026-03-31: README atualizado para refletir a arquitetura atual do projeto, incluindo módulo `apps/biomass_classifier`, fluxo de ROI manual, persistência local do classificador, execução local e pontos de atenção de MQTT/deploy.

- 2026-04-01: Foi definida uma direcao de documentacao dupla para o projeto: uma apresentacao mais institucional para submissao/apresentacao a FAPEAM e uma versao publica voltada a GitHub, com README tecnico, contexto do projeto, roteiro de execucao e futura limpeza de segredos, arquivos locais e configuracoes sensiveis antes da publicacao aberta.

- 2026-04-01: A documentacao institucional do projeto deve permanecer neutra, sem citar orgaos ou contextos especificos, para poder servir tanto em apresentacoes formais quanto em futura publicacao aberta do repositorio.

- 2026-04-01: Criado `ABOUT_PROJECT.md` com posicionamento institucional e neutro do projeto, cobrindo apresentacao, objetivo, problema, solucao proposta, diferenciais, arquitetura em alto nivel, estado atual e potencial de evolucao.

- 2026-04-01: Criado `ROADMAP.md` com planejamento de curto, medio e longo prazo, cobrindo consolidacao do monitoramento, refinamento da `img_rec`, evolucao do classificador, uso de ROI, maturidade de dados, deploy e robustez de engenharia.

- 2026-04-01: Foi recomendada uma estrategia de documentacao em camadas para o repositorio monorepo, com um README raiz explicando a visao geral e documentos especificos para cada modulo principal (`dashboard/` e `firmware/`), evitando misturar instrucoes de backend web com codigo de microcontrolador.

- 2026-04-01: Criados um README na raiz do repositorio e um `firmware/README.md`, separando a documentacao entre visao geral do sistema, modulo web (`dashboard/`) e modulo embarcado (`firmware/`). A documentacao do firmware agora registra o controlador principal, os testes de hardware, o uso de PlatformIO/ESP32 e o acoplamento atual com MQTT.

- 2026-04-01: Na preparacao para publicacao futura do repositorio, foram reforcadas regras de `.gitignore` na raiz e em `dashboard/` para cobrir `.env`, bases SQLite locais, datasets gerados do classificador, `.DS_Store`, logs e outros artefatos de ambiente.

- 2026-04-01: Foi explicado o motivo de mover configuracoes como MQTT para `.env`: separar ambiente local do codigo versionado, evitar IPs e credenciais fixas no repositorio e facilitar deploy futuro. Tambem foi esclarecido o conceito de arquivos template/exemplo de configuracao, usados para versionar apenas o formato esperado sem expor valores reais.

- 2026-04-01: A configuracao MQTT do dashboard foi externalizada para `.env` (`MQTT_BROKER`, `MQTT_PORT`, `MQTT_TOPIC`) e `run.py` passou a carregar explicitamente o arquivo `.env` no boot. Isso desacopla IP local do codigo e prepara melhor o projeto para troca de ambiente, demonstracao e futuro deploy.

- 2026-04-01: O arquivo local `.env` do dashboard recebeu explicitamente as chaves `MQTT_BROKER`, `MQTT_PORT` e `MQTT_TOPIC`, alinhando o ambiente real de desenvolvimento com o `env.sample`.

- 2026-04-01: Foi esclarecido que `apps/mqtt/client.py` ainda mostra valores de MQTT porque eles ficaram como fallbacks de desenvolvimento no `os.getenv(...)`. O ambiente real agora vem do `.env`; os valores no codigo servem apenas como padrao quando as variaveis nao estiverem definidas.

- 2026-04-02: Foi esclarecido que o projeto usa `socketio.run(...)` em `run.py` para execucao local, o que na pratica caracteriza o servidor de desenvolvimento do Flask/Flask-SocketIO. Para deploy, o repositório já aponta para Gunicorn em `Dockerfile` e `render.yaml`, embora a compatibilidade final com Socket.IO precise ser considerada no ambiente de producao.

- 2026-04-06: Ajustado o tamanho da tipografia da sidebar em `static/assets/css/custom.css` para reduzir excesso visual e melhorar a proporcao dos itens de navegacao.

- 2026-04-06: Foi feita uma orientacao estrategica de proximo foco do projeto. A recomendacao atual e priorizar consolidacao antes de expansao: estabilizar configuracao/servicos, revisar fluxo fim a fim da `img_rec`, melhorar confiabilidade do classificador e so depois ampliar funcionalidades ou partir para deploy completo.

- 2026-04-06: Foi definido um plano priorizado de curto prazo em tres frentes: (1) confiabilidade do sistema e ambiente, (2) fechamento funcional da `img_rec` ponta a ponta, e (3) melhoria da qualidade do classificador e do dataset. Tambem foi sugerido usar no Trello um topico macro focado em consolidacao da base antes de expansao.

- 2026-04-06: O boot do dashboard foi tornado mais resiliente ao MQTT. `apps/mqtt/client.py` agora respeita `MQTT_ENABLED` e `MQTT_REQUIRED`, permitindo subir a aplicacao sem broker quando desejado e falhar explicitamente apenas quando configurado como obrigatorio.

- 2026-04-06: Fechamento funcional inicial da `img_rec` revisado. Ajustes aplicados no template para deixar o fluxo ponta a ponta mais confiável: título da página padronizado em português, rótulo visual de `Média` corrigido, parsing de respostas HTTP endurecido com fallback para respostas inválidas, botão de salvar rótulo desabilitado após sucesso e, principalmente, invalidação automática do resultado anterior quando a ROI é alterada ou removida, evitando exibição de análise stale para uma área de leitura diferente.

- 2026-04-06: A UX da `img_rec` foi refinada para separar feedback global e feedback local. A análise principal continua usando o `feedback-box` do card superior, mas salvar rótulo manual e retreinar modelo passaram a exibir retorno no próprio bloco de resultado, evitando scroll automático para o topo e preservando o contexto da ação para o usuário.

- 2026-04-06: A `img_rec` ganhou reenquadramento automático do workspace após seleção de imagem. Quando o preview carrega e altera a altura do bloco, a página agora faz scroll suave para recentrar a área relevante, priorizando a ação principal em telas compactas e evitando que o botão de analisar fique fora de vista após uploads com proporções diferentes.

- 2026-04-06: As notificacoes da `img_rec` foram refinadas para ficar mais semanticas e contextuais. O card superior agora usa titulos curtos para sucesso e falha da analise, enquanto o bloco de resultado ganhou feedbacks locais com cabecalho e mensagem propriamente separados para salvar rotulo e retreinar modelo, melhorando clareza e leitura visual.

- 2026-04-06: A estrategia de notificacao da `img_rec` foi alterada para um modelo hibrido. Sucessos de analise, salvamento de rotulo e retreino passaram a usar toasts fixos no viewport, evitando competir com o scroll automatico para o resultado. Erros continuam inline nos blocos relevantes para preservar contexto e permanencia visual.

- 2026-04-06: A estrategia de notificacao da `img_rec` foi refinada novamente. O toast de sucesso da analise foi removido porque o proprio bloco de resultado ja confirma a acao visualmente. Sucessos de salvar rotulo manual e retreinar modelo voltaram para o feedback local logo abaixo dos botoes da area de correcao, reduzindo deslocamento visual e mantendo a confirmacao perto da origem da interacao.

- 2026-04-06: O scroll de exibicao do resultado na `img_rec` foi refinado para recentrar melhor o bloco apos a analise. Em vez de ancorar no topo do card, o template agora espera a expansao do conteudo e reenquadra a area de resultado no centro do viewport, com alvo diferente para telas compactas e maiores, reduzindo o efeito de bloco cortado.

- 2026-04-06: A confirmacao de sucesso para salvar rotulo manual e retreinar modelo foi movida novamente para toasts no canto inferior direito da viewport. A ideia foi evitar depender do enquadramento do bloco de resultado para mensagens curtas de confirmacao, mantendo o aparecimento do resultado como confirmacao suficiente para a analise principal e deixando erros ainda inline no contexto da acao.

- 2026-04-06: O visual do toast de sucesso no canto inferior direito da `img_rec` foi refinado para uma paleta mais viva, reduzindo a sensacao de bloco escuro demais. O gradiente passou a dialogar melhor com o verde/ciano da interface, com contraste mais claro no icone e na mensagem, inclusive no tema claro.

- 2026-04-06: O toast de sucesso da `img_rec` foi simplificado visualmente para fundo solido, sem gradiente. A intencao foi deixar a confirmacao mais limpa e menos chamativa, mantendo a paleta verde viva mas com aspecto mais elegante e coerente com o restante da interface.

- 2026-04-07: Foi corrigido o atraso visual na troca de cor dos gauges ao navegar entre paginas no tema branco. A cor do tema passou a ser aplicada ja na configuracao inicial dos gauges em `dashboard.js`, e o refresh tardio de 1 segundo em `themeSettings.js` foi substituido por um `requestAnimationFrame`, reduzindo o flash de gauges escuros antes da paleta clara.

- 2026-04-07: Foi diagnosticado que a defasagem percebida nos gauges nao vem do componente JustGage, e sim da estrategia atual de emissao realtime. Em `apps/websocket.py`, a funcao `emitir_periodicamente` consulta o banco e reemite leituras apenas a cada 5 segundos (`socketio.sleep(5)`), o que introduz atraso visivel mesmo quando o MQTT grava rapidamente no banco. O loop ainda recalcula KPIs dentro da iteracao de cada sensor, aumentando custo e redundancia de emissao.

- 2026-04-07: Foi alinhada a direcao arquitetural para o realtime dos sensores: substituir o polling pesado de 5 segundos por emissao mais imediata apos o MQTT, mas sem enviar leituras cruas diretamente ao frontend. A recomendacao e introduzir uma pequena camada de processamento/validacao entre ingestao e emissao, com regras para valores invalidos, outliers, smoothing opcional e sinalizacao de qualidade do dado antes de atualizar gauges e KPIs.

- 2026-04-07: Revisao da etapa de realtime: o problema percebido ao recarregar ou trocar de pagina nao era principalmente a latencia MQTT->WebSocket, e sim a inicializacao dos gauges com valores mockados fixos em `dashboard.js` (ex.: 445, 32, 712), que ficam visiveis ate a chegada do primeiro dado real. Ficou alinhado que a cor dos gauges deve continuar nascendo correta, mas que a proxima simplificacao desejavel e remover/mock values do bootstrap do dashboard e substitui-los por estado neutro ou snapshot inicial real. A camada de processamento realtime e util para robustez, mas nao resolve esse sintoma especifico de primeira pintura.

- 2026-04-07: Simplificacao do bootstrap do dashboard alinhada com a filosofia de codigo simples e funcional. Os gauges deixaram de nascer com alguns valores mockados arbitrarios mais chamativos e o backend passou a emitir um snapshot inicial real no evento de conexao Socket.IO, reduzindo a janela em que o usuario ve dados ficticios ao recarregar ou navegar de volta para a dashboard.

- revisao de simplicidade do realtime: o principal problema visual dos gauges ao trocar de pagina nao era latencia do banco, e sim bootstrap com valores mockados; a correcao passou a usar snapshot inicial via websocket e estado visual neutro "Aguardando leitura" ate a primeira leitura real, evitando numeros enganosos no carregamento.

- 2026-04-07: A tentativa de introduzir um estado visual de gauges pendentes com overlay de "Aguardando leitura" foi revertida por gerar ruído e comportamento visual bugado. A direcao voltou para uma abordagem mais simples: manter o snapshot inicial via Socket.IO e evitar complexidade extra no frontend sempre que possivel.

- 2026-04-07: O bootstrap visual do dashboard foi simplificado para evitar numeros falsos na primeira pintura. KPIs agora nascem com `--` e classe de carregamento; os gauges exibem apenas um placeholder neutro e o componente JustGage so e criado quando chega a primeira leitura real via Socket.IO.

- 2026-04-07: Foi refinado o alinhamento visual dos placeholders `--` nos gauges do dashboard, ajustando largura, altura util e centralizacao dentro da `chart-area` para evitar que o estado neutro parecesse deslocado no card.

- 2026-04-07: Foi criado o arquivo `REALTIME_WORKFLOW.md` para acompanhar manualmente a consolidacao do pipeline de dados em tempo real. A combinacao alinhada e manter esse arquivo sempre atualizado com seta de progresso nas etapas, junto das mudancas feitas nessa frente.

- 2026-04-07: A etapa 1 da consolidacao do realtime foi fechada e registrada em `REALTIME_WORKFLOW.md`. O diagnostico atual e: snapshot inicial via Socket.IO e util e deve permanecer; o principal ponto em aberto agora e decidir se `apps/services/realtime_processing_service.py` fica como esta, e simplificado, ou sai para manter o pipeline mais enxuto.

- 2026-04-07: A etapa 2 da consolidacao do realtime foi simplificada. `apps/services/realtime_processing_service.py` deixou de comparar leituras anteriores e de aplicar smoothing/classificacao `suspect`; agora faz apenas validacao minima e previsivel (numero, finitude e faixa basica por sensor). `apps/mqtt/client.py` tambem foi enxugado para emitir KPIs pelo helper de websocket e manter logs mais simples.

- 2026-04-07: A etapa 3 da consolidacao do realtime foi fechada. O frontend passou a conectar o Socket.IO usando a origem atual da pagina em `static/assets/js/realtime.js`, com desconexao mais robusta em navegacao via `pagehide` e `beforeunload`. Tambem foi removida a emissao de KPIs mockados em `apps/services/kpi_service.py`, para que o estado neutro do dashboard so seja substituido quando houver dado real ou calculo real implementado.

- 2026-04-07: A etapa 4 da consolidacao do realtime foi fechada. `apps/mqtt/client.py` ganhou tratamento mais claro para falhas de conexao e desconexao MQTT, mensagens de log mais objetivas para payload invalido e leitura descartada, e uso de `logger.exception` para erros inesperados. O fallback com `MQTT_REQUIRED=false` foi mantido como estrategia de degradacao segura.

- 2026-04-07: A etapa 5 da consolidacao do realtime foi fechada. O resultado final desse ciclo foi: snapshot inicial mantido, bootstrap neutro para KPIs e gauges, validacao minima de leitura no pipeline e fallback previsivel para indisponibilidade do broker. Ficaram registrados como proximos passos reais a implementacao de KPIs calculados de verdade e a validacao ponta a ponta com dados MQTT reais.

- 2026-04-09: Revisao da aba `user.html`: a tela atual foi considerada majoritariamente heranca do template e pouco alinhada ao dominio do projeto. A direcao recomendada passou a ser reaproveitar essa area como `Configuracoes`/`Administracao`, priorizando dados operacionais do sistema (broker MQTT, identificacao do operador, preferencias do dashboard e parametros de funcionamento) em vez de manter um `User Profile` generico sem suporte real no backend.

- 2026-04-09: Foi criado o arquivo `SETTINGS_PAGE_PROPOSAL.md` com a proposta concreta de transformar a aba `User Profile` em `Configuracoes`, incluindo estrutura recomendada da pagina, blocos operacionais, layout, faseamento e itens do template que devem ser removidos.

- 2026-04-09: Foi implementada a fase 1 da proposta de `Configuracoes`. A antiga tela `User Profile` foi substituida por uma pagina simples e real, baseada em `current_user`, resumo da conta e atalhos operacionais do sistema. A navegacao da sidebar e do dropdown superior tambem foi renomeada de `User Profile`/`Profile` para `Configuracoes`.

- 2026-04-09: Foi implementada a fase 2 da pagina `Configuracoes`. O backend passou a expor contexto real de conectividade e sistema em `apps/home/routes.py`, com status operacional do MQTT vindo de `apps/mqtt/client.py`, e a tela `templates/home/user.html` passou a mostrar broker, topico, estado de conexao, modo da aplicacao, contagem de sensores/leituras e ultima leitura registrada.

- 2026-04-09: A página `Configurações` (`user.html`) apresentou relato de erro 500 após a Fase 2. A revisão não reproduziu a falha com `test_client`, mas a rota `apps/home/routes.py` foi endurecida: agora registra a exceção real no log e trata defensivamente falhas ao montar `mqtt_status`, contagens e última leitura, evitando que dados operacionais incompletos derrubem a página inteira.
- 2026-04-10: Revisão da página de Configurações mostrou que `user_sensor_count` está correto para o estado atual do banco: existe apenas 1 usuário (`id=1`) e a tabela `sensors` tem `user_id` preenchido como `1..9`, então só o sensor `CO2 Interno` pertence ao usuário logado. Os demais sensores apontam para usuários inexistentes e indicam inconsistência de modelagem/seeding, não erro da UI.
- 2026-04-10: A métrica `Sensores do operador` foi removida da página `Configurações` e do contexto em `apps/home/routes.py`. Para esta versão do projeto, a associação `sensor -> usuário` só estava gerando ruído de modelagem e não agrega valor operacional ao sistema.
- 2026-04-10: Os atalhos rápidos foram removidos da página `Configurações`. Como a navegação principal já está clara na sidebar, os links extras estavam redundantes e foram substituídos por um bloco simples de evolução futura da página.
- 2026-04-10: A navegação foi limpa para ficar mais alinhada ao produto: a aba de notificações foi removida da navbar e da sidebar, o item `Image Recognition` passou para `Reconhecimento de imagem` e o ícone de `Configurações` foi trocado para uma engrenagem.
- 2026-04-10: O item da sidebar da página `img_rec` foi encurtado de `Reconhecimento de imagem` para `Análise de biomassa`, buscando melhor equilíbrio entre clareza e proporção visual na navegação.
- 2026-04-10: A antiga página `Configurações` foi reposicionada como `Sistema`, com ajuste de navegação e microcopy. A decisão foi tomada para alinhar a promessa da tela ao que ela realmente entrega hoje: visão resumida da conta, conectividade e estado operacional, sem sugerir edição de parâmetros ainda inexistentes.
- 2026-04-10: A aba `Sistema` foi finalizada com uma estrutura mais enxuta e orientada a uso real: ícone da sidebar trocado para `icon-components`, remoção de blocos explicativos e manutenção apenas de três áreas úteis ao usuário final (`Conta`, `Conectividade` e `Resumo do sistema`). Campos sem valor operacional imediato, como `bio`, indicadores de configuração inexistente e textos de bastidor, foram descartados para deixar a tela mais coerente com um sistema IoT.
- 2026-04-10: A lupa e o modal de busca foram removidos da navbar. Para o escopo atual do protótipo, não existe conteúdo pesquisável suficiente para justificar esse elemento, então a navegação superior foi simplificada.
- 2026-04-10: A tipografia global foi harmonizada em `static/assets/css/custom.css` sem trocar a família base do tema. A decisão foi manter `Poppins` e padronizar pesos, tamanhos e espaçamento para títulos, navbar, sidebar, cards, textos auxiliares e controles, buscando coerência visual no sistema inteiro com baixo risco de regressão.
- 2026-04-10: A tentativa de harmonização tipográfica global foi revertida após piorar a leitura do sistema. A direção adotada passou a ser local: restaurar o `custom.css` ao comportamento anterior e alinhar apenas a página `Sistema` com a linguagem visual da `Análise de biomassa`, preservando a diferença natural de pesos e tamanhos do restante do dashboard.
- 2026-04-10: Após revisão visual, a sidebar foi mantida no tamanho aprovado manualmente (`0.80rem`, peso `500`) e a correção tipográfica passou a focar apenas a página `Sistema`. Nessa tela, o título principal deixou de usar `h2.title` e passou a usar a mesma lógica de `card-title` da `Análise de biomassa`, reduzindo o peso excessivo do cabeçalho.
- 2026-04-10: O kicker da página `Sistema` foi ajustado de `Visão do sistema` para `Status do sistema`, por comunicar melhor o papel atual da tela no protótipo.
- 2026-04-10: As telas de autenticação (`accounts/login.html` e `accounts/register.html`) foram alinhadas à identidade do dashboard. O layout fullscreen agora respeita o tema claro/escuro do sistema, a navbar fullscreen usa o mesmo comportamento do switch principal, e os cartões de autenticação adotam a paleta verde/ciano do produto em vez do padrão genérico do template.
- 2026-04-10: Referências visíveis a `Creative Tim` e `AppSeed` foram removidas dos layouts e rodapés principais. Os títulos base do navegador deixaram de incluir `AppSeed`, e os footers passaram a exibir apenas a identidade do projeto.
- 2026-04-10: O contraste do kicker verde (`Acesso ao dashboard` / `Novo acesso`) nas telas de autenticação foi reforçado no tema claro. A cor do texto passou para um verde mais escuro e a cápsula ganhou borda/fundo levemente mais fortes para melhorar legibilidade sem perder a identidade visual.
- 2026-04-10: Os chips de status da página `Sistema` tiveram o contraste reforçado no tema claro. No `white-content`, as variantes `muted`, `success`, `warning` e `error` receberam texto mais escuro e borda sutil para melhorar legibilidade.
- 2026-04-10: O kicker `Status do sistema` na página `Sistema` teve o contraste reforçado no tema claro, com verde mais escuro no texto e cápsula com borda/fundo levemente mais fortes.
- 2026-04-13: Os KPIs mockados foram recolocados temporariamente em `apps/services/kpi_service.py` (`445.0`, `7.4`, `32.0`, `712`) apenas para captura visual/fotografia da dashboard. A intenção continua sendo voltar ao estado sem mock após esse uso pontual.
- 2026-04-13: O mock temporário dos KPIs em `apps/services/kpi_service.py` foi cancelado e o serviço voltou a retornar `{}`.
