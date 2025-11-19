#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <CO2Sensor.h>


extern Adafruit_ADS1115 ads; 
CO2Sensor CO2sensor(1, 0.99, 50); // usa canal 1 do ADS1115

void sensorCO2_Setup(){
//  pinMode(34, INPUT); // Definir o pino analógico para o sensor de CO2
 Wire.begin();

  if (!ads.begin()) {
    Serial.println("Erro ao inicializar o ADS1115!");
    while (1);
  }

  // GAIN_TWOTHIRDS: faixa ±6.144V (compatível com saída AO do sensor de CO2)
  //ads.setGain(GAIN_ONE);
  ads.setGain(GAIN_TWOTHIRDS);
 // para o ADS1115 de 16 bits

  delay (500);
  Serial.println("Calibrando sensor de CO2...");
  CO2sensor.calibrate();
  Serial.println("Calibragem concluída.");

}

void sensorCO2_Loop(){
    
 int ppm = CO2sensor.read();

  Serial.print("CO2: ");
  Serial.print(ppm);
  Serial.print(" ppm | ");
  Serial.print(CO2sensor.getVoltage());
  Serial.println(" mV");

  delay(1000);



   
}


