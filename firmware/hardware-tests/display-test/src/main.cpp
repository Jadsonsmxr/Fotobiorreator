#include <Arduino.h>
#include <Display/Display_config.h>
#include <Display/Display_custom.h>
#include <Comunicação/Espnow.h>



void setup()
{
    Serial.begin(115200);
    
    Serial.println("Iniciando...");

    
    
    init_communication();
    delay(300);
     // Valores iniciais de exemplo
    init_UI();
    //transitions_screens();
    Serial.println("Setup concluído!");

}


void loop()
{

  
 update_data();
 update_UI();
 delay(1);
}
