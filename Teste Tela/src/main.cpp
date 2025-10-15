#include <Arduino.h>

 // Arquivos gerados pelo EEZ Studio
#include <Display_config.h>



// ----------------------
// Setup
// ----------------------
void setup()
{
    Serial.begin(115200);
    Serial.println("Iniciando...");


    init_UI();
   

    Serial.println("Setup concluído!");
}

// ----------------------
// Loop principal
// ----------------------
void loop()
{
 update_UI();
 delay(3);
}
