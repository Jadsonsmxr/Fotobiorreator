#include <Arduino.h>
#include <Display/Display_config.h>
#include <Display/Display_custom.h>
#include <Comunicação/Espnow.h>



void setup()
{
    Serial.begin(115200);
    Serial.println("Iniciando...");

    init_UI();
    transitions_screens();
    init_communication();
     // Valores iniciais de exemplo


    Serial.println("Setup concluído!");

}


void loop()
{
 update_UI();
 delay(1);
}
