# Settings Page Proposal

Objetivo: substituir a atual aba `User Profile` por uma pagina de `Configuracoes` alinhada ao dominio do projeto.

## Direcao

A pagina atual deve deixar de ser tratada como perfil pessoal generico e passar a funcionar como uma area administrativa do sistema.

Titulo recomendado:
- `Configuracoes`

Rotulo na sidebar e no dropdown:
- `Configuracoes`

## Estrutura proposta

### Bloco 1: Operador

Finalidade:
- mostrar dados reais do usuario autenticado
- substituir o antigo card de perfil fake

Conteudo:
- nome de usuario
- email
- origem da conta
  - local
  - GitHub
  - Google
- bio do usuario, se existir

Fonte de dados:
- `current_user`
- `apps/authentication/models.py`

### Bloco 2: Conectividade

Finalidade:
- mostrar se o sistema esta ligado ao broker e com quais parametros

Conteudo:
- MQTT habilitado
- MQTT obrigatorio
- broker configurado
- porta configurada
- topico configurado
- status de conexao
  - conectado
  - desconectado
  - desabilitado por ambiente

Fonte inicial:
- variaveis de ambiente do dashboard
- estado atual do cliente MQTT

Observacao:
- na fase inicial, o status pode ser somente informativo
- em fase futura, pode ganhar teste ativo de conectividade

### Bloco 3: Dashboard

Finalidade:
- reunir preferencias visuais e operacionais do frontend

Conteudo:
- tema atual
- preferencia salva de tema
- observacao sobre atualizacao em tempo real
- espaco reservado para futuras preferencias de exibicao

Fonte inicial:
- `localStorage` no frontend
- configuracoes simples do dashboard

### Bloco 4: Sistema

Finalidade:
- expor informacoes institucionais e tecnicas do projeto

Conteudo:
- modo atual da aplicacao
  - debug
  - production
- quantidade de sensores cadastrados
- ultima atualizacao recebida, se disponivel
- versao do projeto, se houver

Fonte inicial:
- backend Flask
- banco de dados

### Bloco 5: Acoes administrativas

Finalidade:
- concentrar operacoes importantes que fazem sentido no contexto do sistema

Conteudo inicial sugerido:
- abrir pagina de analise de biomassa
- abrir historico de classificacoes, quando existir
- acao futura de retreino centralizado
- acao futura de teste MQTT

Importante:
- evitar colocar botoes falsos ou placeholders clicaveis sem funcionalidade
- se a acao ainda nao existir, mostrar como card informativo e nao como CTA principal

## Layout recomendado

### Desktop

Coluna esquerda:
- Operador
- Conectividade

Coluna direita:
- Dashboard
- Sistema
- Acoes administrativas

### Mobile

Empilhar blocos nesta ordem:
1. Operador
2. Conectividade
3. Dashboard
4. Sistema
5. Acoes administrativas

## O que deve ser removido da pagina atual

- avatar ficticio
- nome ficticio
- cargo ficticio
- redes sociais falsas
- campos genericos de endereco
- formulario de perfil sem persistencia real
- textos herdados do template sem relacao com o projeto

## Faseamento recomendado

### Fase 1: Reaproveitamento limpo

Objetivo:
- renomear a pagina e substituir o conteudo fake por dados reais ja disponiveis

Entregas:
- mudar `User Profile` para `Configuracoes`
- trocar a tela atual por blocos informativos reais
- manter a implementacao simples e sem formularios que nao salvam nada

### Fase 2: Estado operacional

Objetivo:
- adicionar dados de sistema e conectividade de forma confiavel

Entregas:
- exibir parametros MQTT atuais
- exibir status do cliente MQTT
- exibir informacoes do sistema e sensores
- Status: implementada

### Fase 3: Acoes administrativas reais

Objetivo:
- transformar a pagina em centro operacional do sistema

Entregas:
- retreino do modelo a partir de area administrativa
- historico de classificacoes
- teste de conectividade MQTT
- configuracoes operacionais persistidas no backend

## Minha recomendacao final

A pagina faz sentido se deixar de ser `perfil` e passar a ser `Configuracoes do Sistema`.

A melhor implementacao agora e:
- simples
- informativa
- sem campos falsos
- sem prometer edicao onde ainda nao existe backend para salvar
