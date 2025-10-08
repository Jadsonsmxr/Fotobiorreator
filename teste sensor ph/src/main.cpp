#include<Arduino.h>

#include <Wire.h>
#include <Adafruit_ADS1X15.h>
Adafruit_ADS1115 ads;  // para o ADS1115 de 16 bits

// void setup() {
//   Serial.begin(115200);
//   Wire.begin();

//   if (!ads.begin()) {
//     Serial.println("Erro ao iniciar ADS1115!");
//     while (1);
//   }
//   Serial.println("ADS1115 conectado com sucesso!");
//   ads.setGain(GAIN_ONE);
// }

// void loop() {
//   // Lê tensão no canal 0 (AIN0 vs GND)
//   int16_t adc0 = ads.readADC_SingleEnded(0);
//   float voltage = ads.computeVolts(adc0); // 0.125 mV por bit (GAIN_ONE)

//   Serial.print("ADC: "); Serial.print(adc0);
//   Serial.print(" | Tensão: "); Serial.print(voltage, 3); Serial.println(" V");

//   delay(1000);
// }


//22
float buffer[10];
float calibration_value = 23.2; // ajuste do ponto neutro (pH 7)
float ph_act = 0.0;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  if (!ads.begin()) {
    Serial.println("Erro ao inicializar o ADS1115!");
    while (1);
  }

  // GAIN_TWOTHIRDS: faixa ±6.144V (compatível com saída AO do pH-4502C)
  ads.setGain(GAIN_TWOTHIRDS);
}

void loop() {
  float avg_voltage = 0.0;

  // 1️⃣ Ler 10 amostras reais
  for (int i = 0; i < 10; i++) {
    int16_t adc = ads.readADC_SingleEnded(0);      // ler canal AIN0
    buffer[i] = ads.computeVolts(adc);           // converter para volts
    delay(30);
  }

  // 2️⃣ Ordenar o buffer (bubble sort)
  for (int i = 0; i < 9; i++) {
    for (int j = i + 1; j < 10; j++) {
      if (buffer[i] > buffer[j]) {
        float temp = buffer[i];
        buffer[i] = buffer[j];
        buffer[j] = temp;
      }
    }
  }

  // 3️⃣ Calcular média descartando os 2 menores e 2 maiores
  for (int i = 2; i < 8; i++) {
    avg_voltage += buffer[i];
  }
  avg_voltage /= 6.0;

  // 4️⃣ Converter para pH usando fórmula calibrada
  //ph_act = -5.70 * avg_voltage + calibration_value;
  ph_act = -6.09 * avg_voltage + calibration_value;
  // 5️⃣ Mostrar resultados
  Serial.print("Tensão média: ");
  Serial.print(avg_voltage, 3);
  Serial.print(" V | Valor de pH: ");
  Serial.println(ph_act, 2);

  delay(1000); // espera 1 segundo antes da próxima leitura
}