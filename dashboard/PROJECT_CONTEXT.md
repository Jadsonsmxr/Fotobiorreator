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
