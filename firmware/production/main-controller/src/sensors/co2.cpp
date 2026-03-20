#include <Wire.h>
#include <Adafruit_ADS1X15.h>

extern Adafruit_ADS1115 ads; 

int i = 0;
float buffer_co2[10];

 // Variável global para armazenar o valor de CO2 lido
float V400ppm = 0.705;
float V10000ppm = 0.400;

float co2 = 412.0; // valor inicial do CO2

float Slope = (V10000ppm - V400ppm) / (log10(10000) - log10(400));

float calcularPPM(float v_sensor){

  //if (v_sensor >= V400ppm) return 400.0;

  float expoente = (v_sensor - V400ppm) / Slope;
  float ppm = pow(10, expoente + log10(400));
  return ppm;

}

void sensorCO2_Setup() {
  // Serial.begin(115200);
  Serial.println("Medindo CO2");
  
  if (!ads.begin()) {
    Serial.println("Erro ao inicializar o ADS1115!");
    while (1);
  }

  // GAIN_TWO: faixa ±2.048V (compatível com saída AO do sensor de CO2)
  ads.setGain(GAIN_TWO);
}

void sensorCO2_Loop() {
  static unsigned long lastRead = 0;
  const unsigned long interval = 100;
  unsigned long now = millis();
  
  if (now - lastRead >= interval && i < 10) {
    lastRead = now;


    int16_t adc = ads.readADC_SingleEnded(1);
    float voltage = ads.computeVolts(adc);
    buffer_co2[i] = voltage;
    i++;
  }


  if (i >= 10) {
     i = 0;
     
     for (int i = 0; i < 9; i++) {
      for (int j = i + 1; j < 10; j++) {
        if (buffer_co2[i] > buffer_co2[j]) {
          float temp = buffer_co2[i];
          buffer_co2[i] = buffer_co2[j];
          buffer_co2[j] = temp;
        }
      }
    }

    float avg_voltage = 0.0;
      for (int i = 2; i < 8; i++) {
        avg_voltage += buffer_co2[i];
       }
    avg_voltage /= 6.0;


    
    

    co2 = calcularPPM(avg_voltage);
   

    //Serial.print("Saída Módulo: "); Serial.print(avg_voltage, 3);
    
    //Serial.print("V | CO2: "); Serial.print(co2); Serial.println(" ppm");

    }
    
}

