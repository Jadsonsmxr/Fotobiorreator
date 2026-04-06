# Fotobiorreator Web Dashboard

Aplicação web para monitoramento de um fotobiorreator com autenticação, ingestão de leituras via MQTT, atualização em tempo real com Socket.IO e um módulo integrado de classificação de biomassa por imagem.

O projeto foi construído sobre Flask e Jinja2, usando o tema Black Dashboard como base visual e uma organização modular em `apps/` para separar autenticação, páginas, serviços, MQTT, WebSocket e classificador de biomassa.

## Visão geral

O sistema atende dois fluxos principais:

1. monitoramento operacional do fotobiorreator
- sensores publicam leituras em um broker MQTT;
- a aplicação consome, valida e persiste essas leituras;
- o dashboard distribui atualizações em tempo real por Socket.IO.

2. reconhecimento de biomassa por imagem
- o usuário envia uma imagem pela página `img_rec`;
- o backend extrai features da imagem e classifica o estado da biomassa;
- o resultado pode ser corrigido manualmente e reutilizado em retreino supervisionado.

## Principais capacidades

- login e cadastro de usuários;
- autenticação social com GitHub e Google;
- cadastro e persistência de sensores e leituras;
- dashboard em tempo real com KPIs, gráficos e gauges;
- reconhecimento de imagem integrado ao próprio dashboard;
- correção manual de rótulos da biomassa;
- retreino do classificador a partir de amostras rotuladas;
- seleção manual de ROI sobre a imagem para restringir a área útil do fotobiorreator;
- exportação de anotações de ROI para evolução futura de um detector automático de bordas.

## Stack tecnológica

### Backend

- Python 3.12 recomendado
- Flask 3
- Flask-SQLAlchemy
- Flask-Migrate
- Flask-Login
- Flask-SocketIO
- Flask-Dance
- Flask-Minify
- Paho MQTT
- Marshmallow

### Classificação de biomassa

- NumPy
- Pandas
- Pillow
- Joblib
- scikit-learn
- XGBoost

### Frontend

- Jinja2
- Bootstrap / Black Dashboard
- Chart.js
- JustGage
- Socket.IO client
- Vite
- Sass
- PostCSS

### Infraestrutura e dados

- SQLite por padrão
- suporte a banco relacional via variáveis de ambiente
- Docker / Docker Compose
- Nginx
- Redis preparado para uso com Celery
- Mosquitto para MQTT em ambiente local/laboratorial

## Estrutura do projeto

```text
dashboard/
├── apps/
│   ├── authentication/        # login, cadastro, OAuth, modelos de usuário e sensores
│   ├── biomass_classifier/    # módulo de classificação de biomassa por imagem
│   ├── charts/                # rotas relacionadas a gráficos
│   ├── dyn_dt/                # páginas com tabelas dinâmicas
│   ├── home/                  # rotas das páginas HTML principais
│   ├── mqtt/                  # cliente MQTT
│   ├── services/              # regras de negócio de sensores e KPIs
│   ├── config.py              # configuração da aplicação
│   ├── extensions.py          # extensões compartilhadas
│   └── __init__.py            # app factory e registro de blueprints
├── migrations/                # migrações do banco com Alembic
├── static/                    # CSS, JS, imagens, fontes e SCSS
├── templates/                 # layouts e páginas Jinja2
├── Dockerfile
├── docker-compose.yml
├── requirements.txt
├── package.json
├── PROJECT_CONTEXT.md         # contexto operacional e histórico técnico do projeto
└── run.py                     # ponto de entrada da aplicação
```

## Arquitetura resumida

### 1. Aplicação Flask

O bootstrap da aplicação acontece em `apps/__init__.py`, que:

- instancia o app Flask;
- registra blueprints e extensões;
- conecta SQLAlchemy, LoginManager, Migrate e Socket.IO;
- configura templates e arquivos estáticos.

### 2. Banco principal e autenticação

A base principal usa SQLAlchemy e, por padrão, grava em `apps/db.sqlite3`.

Modelos relevantes:

- `Users`
- `Sensor`
- `SensorReading`
- `OAuth`

O sistema suporta autenticação local e login social, desde que as credenciais de OAuth sejam fornecidas no ambiente.

### 3. Fluxo MQTT e tempo real

O cliente MQTT fica em `apps/mqtt/client.py` e consome leituras do broker configurado. Depois disso:

- a leitura é validada;
- o sensor é verificado;
- os dados são persistidos;
- o backend emite atualizações via Socket.IO;
- o frontend atualiza KPIs, gráficos e gauges.

Fluxo resumido:

```text
Sensor/Dispositivo
    ↓ MQTT
Broker
    ↓
apps/mqtt/client.py
    ↓
SensorService.add_reading()
    ↓
Banco de Dados
    ↓
apps/websocket.py
    ↓ Socket.IO
Frontend (realtime.js)
```

### 4. Módulo de classificação de biomassa

O módulo está em `apps/biomass_classifier/` e roda dentro do próprio Flask.

Arquivos principais:

- `apps/biomass_classifier/routes.py`
- `apps/biomass_classifier/service.py`
- `apps/biomass_classifier/dataset_labeler.py`
- `apps/biomass_classifier/data/training_dataset.csv`
- `apps/biomass_classifier/artifacts/biomass_classifier.joblib`

Esse módulo é responsável por:

- receber uploads da página `img_rec`;
- extrair features da imagem;
- executar a predição do estado da biomassa;
- persistir histórico local das inferências;
- aceitar correção manual de rótulos;
- retreinar o modelo com amostras supervisionadas;
- registrar ROI manual para restringir a análise à área útil do PBR.

## Página de reconhecimento de imagem

A página `templates/home/img_rec.html` faz parte do dashboard e não depende de uma aplicação externa separada.

Fluxo atual:

1. o usuário envia uma imagem do fotobiorreator;
2. a própria área de upload vira preview da imagem;
3. opcionalmente, o usuário arrasta sobre a imagem para marcar uma ROI manual;
4. a imagem é enviada para o backend com ou sem ROI;
5. o sistema mostra:
- classe prevista;
- confiança principal;
- features calculadas;
- probabilidades por classe;
- modo de leitura usado (`imagem inteira` ou `seleção manual`).

Também é possível:

- salvar o rótulo correto manualmente;
- retreinar o modelo pela própria interface, quando houver amostras suficientes.

## Persistência do classificador

Além do banco principal do projeto, o módulo de biomassa mantém sua própria persistência local em:

- `apps/biomass_classifier/data/biomass_classifier.sqlite3`
- `apps/biomass_classifier/data/training_dataset.csv`
- `apps/biomass_classifier/data/manual_training_dataset.csv` quando houver retreino
- `apps/biomass_classifier/data/roi_annotations_dataset.csv` quando existirem anotações de ROI
- `apps/biomass_classifier/uploads/` para imagens enviadas
- `apps/biomass_classifier/artifacts/biomass_classifier.joblib` para o modelo treinado

### ROI manual

A ROI manual permite que o usuário delimite a área útil do fotobiorreator antes da inferência. Isso traz dois benefícios:

- melhora a qualidade da leitura atual quando há muito fundo, reflexo ou ruído na imagem;
- cria um dataset de anotações reutilizável para futura detecção automática de bordas.

Observação importante:

- `roi_annotations_dataset.csv` só é criado após uma nova predição ou atualização de rótulo executadas já com a versão atual do backend carregada.

## Endpoints internos do classificador

Base URL:

- `/api/biomass-classifier`

Rotas:

- `POST /api/biomass-classifier/predict`
- `POST /api/biomass-classifier/labels`
- `GET /api/biomass-classifier/history`
- `POST /api/biomass-classifier/retrain`

Resposta típica de `/predict`:

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
  },
  "roi_modo": "manual"
}
```

## Como executar localmente

### Pré-requisitos

- Python 3.12 recomendado
- Node.js 18+
- npm
- broker MQTT acessível, se o fluxo em tempo real for necessário
- Redis opcional, se houver uso de Celery

### 1. Criar e ativar ambiente virtual

```bash
python3.12 -m venv .venv
source .venv/bin/activate
```

### 2. Instalar dependências Python

```bash
pip install -r requirements.txt
```

### 3. Instalar dependências do frontend

```bash
npm install
```

### 4. Configurar variáveis de ambiente

Crie um arquivo `.env` com base em `env.sample`.

Exemplo mínimo:

```env
DEBUG=True
FLASK_APP=run.py
SECRET_KEY=YOUR_SUPER_KEY
ASSETS_ROOT=/static/assets
MQTT_BROKER=127.0.0.1
MQTT_PORT=1883
MQTT_TOPIC=cba_fotobiorreator/sensors/+/data
```

### 5. Aplicar migrações

```bash
flask db upgrade
```

Se ainda não existir histórico local de migração:

```bash
flask db init
flask db migrate
flask db upgrade
```

### 6. Rodar os assets em modo desenvolvimento

Em um terminal:

```bash
npm run dev
```

### 7. Subir a aplicação

Em outro terminal:

```bash
python run.py
```

Por padrão:

```text
http://127.0.0.1:5000
```

## Execução com Docker

O projeto inclui `Dockerfile` e `docker-compose.yml`.

Para subir com Docker Compose:

```bash
docker compose up --build
```

Observações:

- o projeto também inclui um serviço Nginx na composição;
- o build atual executa migrações, o que pode ser conveniente em protótipo, mas merece revisão para produção;
- para uso completo, a configuração de MQTT e banco também precisa ser alinhada ao ambiente de deploy.

## Configuração e ambiente

As configurações centrais estão em `apps/config.py`.

### Banco de dados

Por padrão:

- SQLite local em `apps/db.sqlite3`

Para banco externo, use variáveis como:

- `DB_ENGINE`
- `DB_NAME`
- `DB_HOST`
- `DB_PORT`
- `DB_USERNAME`
- `DB_PASS`

### OAuth

Integrações opcionais:

- GitHub
- Google

Variáveis esperadas:

- `GITHUB_ID`
- `GITHUB_SECRET`
- `GOOGLE_ID`
- `GOOGLE_SECRET`

## Pontos de atenção atuais

- a configuração de MQTT agora pode ser feita por ambiente via `.env` (`MQTT_ENABLED`, `MQTT_REQUIRED`, `MQTT_BROKER`, `MQTT_PORT`, `MQTT_TOPIC`);
- se o broker MQTT estiver indisponível no boot, a aplicação pode falhar antes de subir o servidor;
- o módulo de biomassa usa persistência local própria além do banco principal do projeto;
- o retreino depende de um volume mínimo de amostras rotuladas;
- o deploy completo exige atenção especial a MQTT, Socket.IO, persistência de arquivos e configuração por ambiente.

## Melhorias recomendadas

- tornar a inicialização do MQTT tolerante a falhas no ambiente de desenvolvimento;
- calcular KPIs reais a partir do histórico persistido;
- adicionar testes automatizados para serviços e rotas;
- revisar a estratégia de persistência entre banco principal e banco local do classificador;
- dockerizar o stack com configuração mais próxima de produção quando o projeto avançar para deploy completo.

## Licença

O repositório inclui `LICENSE.md`. Consulte o arquivo para os termos aplicáveis.

## Créditos

Base visual derivada de Black Dashboard / AppSeed, com customizações específicas para o contexto do projeto Fotobiorreator.
