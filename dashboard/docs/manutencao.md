# Manutenção do dashboard

[Instalar e usar](../README.md) · [Visão geral](../../README.md)

## Onde alterar

| Quero alterar… | Arquivo ou pasta |
| --- | --- |
| Inicialização e servidor local | `run.py` |
| Criação da aplicação e registro de módulos | `apps/__init__.py` |
| Configuração e banco | `apps/config.py` e `.env` |
| Usuários, sensores, leituras e ciclos | `apps/authentication/models.py` |
| Login e cadastro | `apps/authentication/` |
| Páginas e ações de ciclos | `apps/home/routes.py` |
| Recebimento de leituras MQTT | `apps/mqtt/client.py` |
| Validação de leituras | `apps/services/realtime_processing_service.py` |
| Cálculo dos indicadores | `apps/services/kpi_service.py` |
| Resumos dos ciclos | `apps/services/cycle_service.py` |
| Eventos enviados ao navegador | `apps/websocket.py` |
| Análise, correções e retreino de biomassa | `apps/biomass_classifier/` |
| Conteúdo das telas | `templates/home/` |
| Menu e navegação | `templates/includes/` |
| Aparência geral | `static/assets/css/custom.css` |
| Gráficos e atualização no navegador | `static/assets/js/dashboard.js`, `realtime.js`, `sensors.js` e `kpis.js` |

## Como uma leitura chega à tela

1. `apps/mqtt/client.py` recebe JSON com `sensor_id` e `value`.
2. O sensor é procurado no banco; IDs desconhecidos são ignorados.
3. `RealtimeProcessingService` valida o valor; `SensorService` salva a leitura aceita.
4. Socket.IO envia `sensor_update` e `kpi_update` ao navegador.
5. A conexão inicial recebe as últimas leituras; outra tarefa repete esse envio a cada 15 segundos.

Exemplo de mensagem, usando o prefixo configurado no firmware:

```text
Tópico: cba_fotobiorreator/sensors/3/data
JSON:   {"sensor_id": 3, "value": 28.5}
```

Os indicadores atuais consideram temperatura interna e pH. CO₂ e luminosidade
estão excluídos desses cálculos por limitações registradas no próprio serviço.
Nomes dos sensores, faixas e janelas de tempo estão em `kpi_service.py`.

## Cadastrar sensores em uma instalação nova

O firmware principal publica CO₂ interno (1), temperatura interna (3) e pH (4).
Os IDs abaixo devem coincidir com os enviados pelo ESP32. O dashboard não cria sensores
automaticamente a partir das mensagens.

Depois de criar sua conta, pare o servidor e abra `python -m flask --app run shell`
na pasta `dashboard`. Substitua `seu_usuario` pelo nome cadastrado:

```python
from apps import db
from apps.authentication.models import Users, Sensor

usuario = Users.find_by_username("seu_usuario")
assert usuario is not None, "Crie a conta antes de cadastrar sensores."

sensores = [
    (1, "CO2 Interno", "co2", "ppm"),
    (3, "Temperatura Interno", "temperature", "°C"),
    (4, "pH", "ph", "pH"),
]
assert all(db.session.get(Sensor, item[0]) is None for item in sensores), "IDs já cadastrados; confira o banco."
for identificador, nome, tipo, unidade in sensores:
    db.session.add(Sensor(id=identificador, name=nome, type=tipo, unit=unidade, user_id=usuario.id))

db.session.commit()
exit()
```

Esse exemplo é destinado ao banco novo. Não substitua sensores existentes sem
conferir suas leituras e a correspondência com o hardware.

## Banco e arquivos

Caminhos relativos à pasta `dashboard`:

| Caminho | Conteúdo |
| --- | --- |
| `apps/db.sqlite3` | Banco padrão: usuários, sensores, leituras e ciclos |
| `apps/biomass_classifier/data/biomass_classifier.sqlite3` | Histórico de análises e rótulos manuais |
| `apps/biomass_classifier/data/training_dataset.csv` | Base inicial incluída no repositório |
| `apps/biomass_classifier/data/manual_training_dataset.csv` | Dados gerados no retreino |
| `apps/biomass_classifier/data/roi_annotations_dataset.csv` | Regiões selecionadas nas imagens |
| `apps/biomass_classifier/uploads/` | Imagens enviadas |
| `apps/biomass_classifier/artifacts/` | Modelo treinado |

Os arquivos gerados podem ainda não existir antes do primeiro uso.
Para preservar o trabalho, faça cópia dos bancos, imagens, datasets gerados e modelo
com a aplicação parada. O banco principal também pode ser externo, conforme a configuração.

O histórico Alembic está ignorado no Git e não acompanha uma cópia nova do projeto.
`db.create_all()` serve somente para criar tabelas ausentes: não altera tabelas existentes.
Antes de mudar modelos em um banco com dados, faça backup e recupere ou estabeleça
um histórico de migrações compatível com esse banco. Não execute `db init`, `migrate`
ou `stamp` como tentativa automática de corrigir um banco existente.

Referência: [Flask-Migrate](https://flask-migrate.readthedocs.io/en/latest/).

## Configurações opcionais

- **Banco externo:** `DB_ENGINE`, `DB_NAME`, `DB_HOST`, `DB_PORT`, `DB_USERNAME` e `DB_PASS`; confira `apps/config.py`. O driver do banco precisa estar instalado.
- **Login GitHub:** `GITHUB_ID` e `GITHUB_SECRET`.
- **Login Google:** `GOOGLE_ID` e `GOOGLE_SECRET`.
- **MQTT:** `MQTT_ENABLED`, `MQTT_REQUIRED`, `MQTT_BROKER`, `MQTT_PORT` e `MQTT_TOPIC`.

O `.env` contém a configuração local e não deve ser versionado.

## API de biomassa

Todas as rotas exigem sessão autenticada. Prefixo: `/api/biomass-classifier`.

| Método e caminho | Entrada | Função |
| --- | --- | --- |
| `POST /predict` | Formulário: arquivo `imagem`; ROI opcional | Analisar e salvar resultado |
| `POST /labels` | Formulário: `id`, `status_real` | Salvar correção |
| `GET /history` | Parâmetro opcional `limit` (1 a 100) | Consultar análises |
| `POST /retrain` | Sem campos obrigatórios | Retreinar com rótulos manuais |

A ROI usa `roi_x`, `roi_y`, `roi_width` e `roi_height`, em proporções de 0 a 1.
Envie os quatro campos para aplicar uma região. As classes aceitas e o formato
completo das respostas estão em `apps/biomass_classifier/service.py`.

## Conferir uma alteração

Use um banco de teste ou cópia dos dados quando a alteração envolver gravação.

1. Execute `python run.py`, entre na conta e abra as quatro telas do menu.
2. Para mudanças MQTT, publique uma leitura de sensor cadastrado em um broker de teste e confira banco e tela.
3. Para biomassa, analise uma imagem, confira o resultado e salve uma correção.
4. Para ciclos, confira início, resumo, finalização e histórico.
5. Confira o terminal e o console do navegador para erros.

## Limitações e pendências

- Versionar e validar o histórico de migrações para tornar a instalação reproduzível.
- Unificar a inicialização: MQTT, Socket.IO e tarefa periódica estão no bloco de execução direta de `run.py`; Gunicorn não executa esse bloco.
- Remover a inicialização duplicada de Flask-Migrate e revisar o `debug=True` fixo do servidor.
- Revisar o build frontend: Vite aponta para `static/assets/scss/custom.scss`, ausente; Gulp usa caminhos antigos e dependências não declaradas.
- Revisar Docker: executa migrações durante a construção e inicia por Gunicorn.
- Validar sensores e calibração com o hardware; os indicadores seguem regras locais do código.
- Capturar as telas de Biomassa, Ciclos e Sistema para complementar o guia. A captura do painel principal já está em `docs/imagens/painel-principal.png`. As imagens de `media/` são herdadas do template e não devem representar o produto atual.

Este guia descreve a estrutura atual. Histórico de tentativas e propostas substituídas
foi retirado para evitar instruções conflitantes.
