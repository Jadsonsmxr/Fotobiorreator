#include <Arduino.h>
#include <Display_config.h>
#include <Display_custom.h>




void setup()
{
    Serial.begin(115200);
    Serial.println("Iniciando...");


    init_UI();
    
    ui_custom_update_values(35, 7.4, 415); // Valores iniciais de exemplo

    Serial.println("Setup concluído!");
}


void loop()
{
 update_UI();
 delay(3);
}
