# Teste do display

Projeto isolado de tela com LVGL, configurado para `esp32-s3-devkitm-1`.

Abra esta pasta no PlatformIO. As dependências e opções da placa estão em `platformio.ini`.

## Notas de configuração

- A versão registrada como funcional da GFX Library for Arduino é **1.3.7**, fixada na configuração.
- A nota original orienta fornecer `lv_conf.h` na biblioteca instalada em `.pio/libdeps/`, mas não especifica a subpasta. Confira o caminho de inclusão e a compatibilidade do arquivo antes de compilar; há um rascunho de configuração em `src/lv_conf.txt`, que não é um cabeçalho `.h`.
- A nota original considera TFT_eSPI dispensável, mas ela continua declarada em `platformio.ini`. A remoção depende de conferir o uso no código.
- Alterações dentro de `.pio/` podem se perder ao reinstalar dependências.

Este roteiro preserva as observações do experimento. A compilação e a tela física
ainda precisam de validação antes de tratar estas instruções como uma instalação reproduzível.

[Voltar ao firmware](../../README.md)
