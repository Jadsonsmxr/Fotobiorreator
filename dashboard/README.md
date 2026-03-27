# Fotobiorreator Web Dashboard

Aplicação web para monitoramento de um fotobiorreator com autenticação de usuários, persistência de leituras de sensores, ingestão via MQTT e atualização em tempo real do dashboard com Flask-SocketIO.

O projeto combina backend em Flask, frontend baseado em Jinja2 + Black Dashboard, banco de dados com SQLAlchemy e uma camada de tempo real para exibição de KPIs e gauges no painel principal.

## Visão Geral

Este projeto foi estruturado para atender um cenário de supervisão operacional de sensores conectados a um fotobiorreator. O backend recebe leituras via MQTT, valida e persiste os dados, e em seguida disponibiliza essas informações ao frontend por WebSocket.

Principais capacidades:

- autenticação local com login e cadastro de usuários;
- suporte a OAuth com GitHub e Google;
- cadastro e persistência de sensores e leituras;
- atualização em tempo real de sensores e KPIs no dashboard;
- páginas HTML renderizadas no servidor com Jinja2;
- pipeline de assets com Vite, Sass e PostCSS;
- suporte a migrações com Flask-Migrate/Alembic;
- opção de execução local ou via Docker.

## Stack Tecnológica

### Backend

- Python 3.10+
- Flask 3
- Flask-SQLAlchemy
- Flask-Migrate
- Flask-Login
- Flask-SocketIO
- Flask-Dance
- Flask-Minify
- Paho MQTT

### Frontend

- Jinja2
- Bootstrap / Black Dashboard
- Chart.js
- JustGage
- Socket.IO client
- Vite
- Sass
- PostCSS / cssnano / autoprefixer

### Infraestrutura e Dados

- SQLite por padrão
- suporte a MySQL/PostgreSQL via variáveis de ambiente
- Docker
- Nginx
- Redis preparado para uso com Celery

## Arquitetura

O projeto segue uma organização modular em `apps/`, separando autenticação, rotas de páginas, serviços, integração MQTT, WebSocket e modelos de dados.

Fluxo resumido:

1. sensores externos publicam dados no broker MQTT;
2. o cliente MQTT da aplicação consome mensagens do tópico configurado;
3. as leituras são validadas e persistidas no banco;
4. uma tarefa em background emite os dados mais recentes via Socket.IO;
5. o frontend recebe os eventos e atualiza KPIs e gauges em tempo real.

## Estrutura do Projeto

```text
dashboard/
├── apps/
│   ├── authentication/   # login, cadastro, OAuth, modelos de usuário e sensores
│   ├── charts/           # rotas relacionadas a gráficos
│   ├── dyn_dt/           # tabelas dinâmicas
│   ├── home/             # rotas das páginas HTML principais
│   ├── mqtt/             # cliente MQTT
│   ├── services/         # regras de negócio para sensores e KPIs
│   ├── config.py         # configurações da aplicação
│   ├── extensions.py     # extensões compartilhadas, como SocketIO
│   └── __init__.py       # app factory e registro de blueprints
├── migrations/           # migrações do banco com Alembic
├── static/               # CSS, JS, imagens, fontes e SCSS
├── templates/            # layouts e páginas Jinja2
├── Dockerfile
├── docker-compose.yml
├── requirements.txt
├── package.json
└── run.py                # ponto de entrada da aplicação
```

## Componentes Principais

### 1. App Factory

O bootstrap da aplicação acontece em `apps/__init__.py`.

Responsabilidades:

- instanciar o Flask app;
- registrar extensões como SQLAlchemy, LoginManager e Migrate;
- registrar blueprints;
- habilitar login social;
- configurar diretórios de templates e arquivos estáticos.

### 2. Configuração

O arquivo `apps/config.py` centraliza:

- modo Debug e Production;
- `SECRET_KEY`;
- configuração de banco;
- parâmetros de OAuth;
- configuração de Redis para Celery;
- fallback para SQLite.

Por padrão, caso nenhuma configuração de banco externo seja fornecida, a aplicação usa um arquivo SQLite em `apps/db.sqlite3`.

### 3. Modelos de Dados

Os modelos principais ficam em `apps/authentication/models.py`.

Entidades relevantes:

- `Users`: usuários autenticáveis da plataforma;
- `Sensor`: sensores cadastrados por usuário;
- `SensorReading`: leituras históricas dos sensores;
- `OAuth`: vínculo de autenticação social.

### 4. Camada de Serviços

Os serviços em `apps/services/` encapsulam a lógica de domínio.

#### `sensor_service.py`

Fornece operações como:

- criar sensores;
- registrar leituras;
- consultar leitura mais recente;
- consultar histórico;
- validar existência de sensor.

#### `kpi_service.py`

Atualmente retorna KPIs mockados:

- `co2_total`
- `efficiency`
- `co2_monthly`
- `active_time`

O arquivo já está preparado para evoluir para cálculos reais baseados em dados persistidos.

### 5. Integração MQTT

O cliente MQTT está em `apps/mqtt/client.py`.

Configuração atual:

- broker: `192.168.2.105`
- porta: `1883`
- tópico: `cba_fotobiorreator/sensors/+/data`

Comportamento:

- realiza subscribe no tópico configurado;
- faz parse do payload JSON;
- extrai `sensor_id` e `value`;
- valida a existência do sensor;
- persiste a leitura no banco.

Exemplo esperado de payload:

```json
{
  "sensor_id": 1,
  "value": 425.7
}
```

### 6. WebSocket / Tempo Real

A integração de tempo real está distribuída em:

- `apps/extensions.py`
- `apps/websocket.py`
- `static/assets/js/realtime.js`

No backend:

- `SocketIO` é configurado com `cors_allowed_origins="*"`;
- uma tarefa em background emite periodicamente:
  - `sensor_update`
  - `kpi_update`

No frontend:

- o cliente Socket.IO recebe eventos;
- `sensors.js` atualiza gauges;
- `kpis.js` atualiza os indicadores numéricos;
- `dashboard.js` inicializa os gráficos e componentes visuais.

### 7. Frontend e Templates

O frontend usa renderização server-side com Jinja2.

Arquivos importantes:

- `templates/layouts/base.html`: layout base da aplicação;
- `templates/includes/scripts.html`: scripts globais;
- `templates/home/index.html`: dashboard principal;
- `static/assets/js/dashboard.js`: gráficos e gauges;
- `static/assets/css/custom.css`: customizações visuais do projeto.

## Como Executar Localmente

### Pré-requisitos

- Python 3.10 ou superior
- Node.js 18+ recomendado
- npm
- broker MQTT acessível, se o fluxo em tempo real for necessário

### 1. Criar ambiente virtual

```bash
python -m venv venv
source venv/bin/activate
```

No Windows:

```bash
venv\Scripts\activate
```

### 2. Instalar dependências Python

```bash
pip install -r requirements.txt
```

### 3. Configurar variáveis de ambiente

Crie um arquivo `.env` com base em `env.sample`.

Exemplo:

```env
DEBUG=True
FLASK_APP=run.py
SECRET_KEY=YOUR_SUPER_KEY
ASSETS_ROOT=/static/assets
```

### 4. Aplicar migrações

```bash
flask db upgrade
```

Caso ainda não exista histórico local de migração:

```bash
flask db init
flask db migrate
flask db upgrade
```

### 5. Instalar dependências front-end

```bash
npm install
```

### 6. Rodar o watcher de assets

Em um terminal:

```bash
npm run dev
```

### 7. Subir a aplicação Flask

Em outro terminal:

```bash
python run.py
```

Por padrão, a aplicação é iniciada em:

```text
http://127.0.0.1:5000
```

## Execução com Docker

O projeto inclui `Dockerfile` e `docker-compose.yml`.

### Subir com Docker Compose

```bash
docker compose up --build
```

Observações:

- o container principal é `appseed_app`;
- o Nginx expõe a porta `5085`;
- o `Dockerfile` executa migrações durante o build.

## Scripts NPM

Definidos em `package.json`:

- `npm run dev`: build em modo desenvolvimento com watch;
- `npm run build`: build de produção e minificação de CSS;
- `npm run minify-css`: minifica os CSS gerados.

## Banco de Dados

### Padrão

- SQLite local em `apps/db.sqlite3`

### Externo

Pode ser configurado via variáveis de ambiente:

- `DB_ENGINE`
- `DB_NAME`
- `DB_HOST`
- `DB_PORT`
- `DB_USERNAME`
- `DB_PASS`

## Autenticação

O sistema suporta:

- login/senha local;
- autenticação com GitHub;
- autenticação com Google.

As integrações sociais dependem do preenchimento de:

- `GITHUB_ID`
- `GITHUB_SECRET`
- `GOOGLE_ID`
- `GOOGLE_SECRET`

## Rotas e Módulos

### Home

`apps/home/routes.py`

- `/index`
- `/<template>`

Serve as páginas HTML da pasta `templates/home/`.

### Authentication

`apps/authentication/routes.py`

Responsável por:

- login;
- logout;
- registro;
- tratamento de páginas de erro;
- fluxo de OAuth.

### Dynamic Tables

`apps/dyn_dt/routes.py`

Módulo voltado a páginas com tabelas dinâmicas.

### Charts

`apps/charts/routes.py`

Rotas auxiliares para visualização de gráficos.

## Fluxo de Dados em Tempo Real

```text
Dispositivo/Sensor
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
    ↓
dashboard.js / sensors.js / kpis.js
```

## Convenções de Desenvolvimento

- backend modularizado por responsabilidade;
- uso de serviços para encapsular regras de domínio;
- templates Jinja2 para renderização server-side;
- JavaScript modular no frontend para tempo real;
- migrações versionadas com Alembic.

## Pontos de Atenção

O projeto já está funcional, mas há alguns pontos importantes para evolução:

- `KPIService` ainda entrega valores mockados;
- o broker MQTT está fixo no código e idealmente deveria vir de variável de ambiente;
- o carregamento global de scripts em tempo real merece cuidado para páginas que não possuem os mesmos componentes do dashboard;
- seria útil adicionar testes automatizados para serviços e rotas;
- o processo de build Docker atualmente executa migrações no build, o que pode não ser ideal em todos os ambientes.

## Melhorias Recomendadas

- externalizar configuração MQTT para `.env`;
- calcular KPIs reais a partir do histórico persistido;
- adicionar testes unitários e de integração;
- isolar scripts do dashboard apenas nas páginas que realmente usam charts/gauges;
- adicionar observabilidade básica com logs estruturados;
- documentar payloads MQTT e eventos Socket.IO em uma seção de contrato técnico.

## Licença

O repositório inclui `LICENSE.md`. Verifique o arquivo para os termos de uso aplicáveis.

## Créditos

Base visual derivada de Black Dashboard / AppSeed, com customizações específicas para o contexto do projeto Fotobiorreator.
