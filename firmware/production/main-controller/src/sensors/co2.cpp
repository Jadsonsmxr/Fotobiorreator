#include <Wire.h>
#include <Adafruit_ADS1X15.h>
// #include <CO2Sensor.h>





//IMPORTANTE: O SENSOR NAO FOI CALIBRADO AINDA, FALTA ACHAR OS VALORES CERTOS DE V400 E V40000, ALEM DISSO QUERO COLOCAR UMA AMOSTRA A CADA 1 A 5 MINUTOS PARA EVITAR RUIDOS.
//TENTAR DIMINUIR E DEIXAR O CODIGO MAIS LIMPO POSSIVEL.





extern Adafruit_ADS1115 ads; 
// CO2Sensor CO2sensor(1, 0.99, 3); // usa canal 1 do ADS1115


// ============ INSIRA AQUI OS VALORES GERADOS NA CALIBRAÇÃO ============
// float A_cal =  1.3117055893;   // <-- COLE O VALOR QUE O SEU CÓDIGO GEROU
// float B_cal =-0.0604309589;   // <-- COLE O VALOR QUE O SEU CÓDIGO GEROU

float V400 = 0.65;     // tensão medida em 400 ppm (ar ambiente)
float V40000 = 0.40;   // tensão medida em 40000 ppm


// ===================== FILTRO EMA =====================
float ema = 0.0;
float alpha = 0.15;  // suavização moderada


// ---------- Parâmetros de funcionamento ----------
const unsigned long INTERVALO_MS = 1000UL; // leitura nominal a cada 1s
const unsigned long INTERVALO_MS_PRINT = 10000UL; // leitura nominal a cada 1s
const int NUM_MEDIANA = 5;    // número de leituras rápidas para mediana
const int TAMANHO_JANELA = 30;      // janela de média (30s se intervalo=1s)
const float ALFA_EMA = 0.15; // suavização final (menor = mais suave)
const float LIMIAR_STDDEV_V = 0.05; // 15 mV -> limite de estabilidade

// ---------- Buffers e estado ----------
float anel[TAMANHO_JANELA];
int indiceAnel = 0;
int contagemAnel = 0;
float mediaSuavizada = 0.0;
unsigned long ultimoTempoAmostra = 0;

// ---------- funções utilitárias simples ----------
float medianaN(float *arr, int n) {
  // copia e ordena (insertion sort simples — bom para n pequeno)
  float tmp[NUM_MEDIANA];
  for (int i = 0; i < n; i++) tmp[i] = arr[i];
  for (int i = 1; i < n; i++) {
    float chave = tmp[i];
    int j = i - 1;
    while (j >= 0 && tmp[j] > chave) { tmp[j + 1] = tmp[j]; j--; }
    tmp[j + 1] = chave;
  }
  return tmp[n / 2];
}

void empilharAnel(float v) {
  anel[indiceAnel] = v;
  indiceAnel = (indiceAnel + 1) % TAMANHO_JANELA;
  if (contagemAnel < TAMANHO_JANELA) contagemAnel++;
}

void calculaMediaDesvio(float &media, float &desvio) {

  if (contagemAnel == 0) { media = 0; desvio = 0; return; }
  media = 0;

  for (int i = 0; i < contagemAnel; i++) 
  media += anel[i];
  media /= contagemAnel;

  float s = 0;

  for (int i = 0; i < contagemAnel; i++) { float d = anel[i] - media; s += d * d; }
  desvio = sqrt(s / contagemAnel);
}

// usa sua fórmula já calibrada A_cal e B_cal (devem ser globais vindos da calibração)
float voltsParaPPM(float V) {
  // proteção contra divisão por zero (B_cal não pode ser 0)
//   if (B_cal == 0) return -1;
//   return exp((V - A_cal) / B_cal);
    float buffer = 0;
    buffer = (V400 - V40000)/(log10(400) - log10(40000)); // Delta V
    buffer = (V - V400)/buffer;
    buffer += log10(400);
    return pow(10, buffer);

}   






















// // ===================== FUNÇÃO V → PPM =====================
// float voltsToPPM(float V) {
//     // fórmula definida no SEU código de calibração:
//     // ppm = exp((V - A_cal) / B_cal);
//     return exp((V - A_cal) / B_cal);
// }






void sensorCO2_Setup(){
    //pinMode(34, INPUT); // Definir o pino analógico para o sensor de CO2
    Wire.begin();

  if (!ads.begin()) {
    Serial.println("Erro ao inicializar o ADS1115!");
    while (1);
  }

//   // GAIN_TWOTHIRDS: faixa ±6.144V (compatível com saída AO do sensor de CO2)
   //ads.setGain(GAIN_ONE);
//   //ads.setGain(GAIN_TWOTHIRDS);
     ads.setGain(GAIN_TWO);
//  // para o ADS1115 de 16 bits

//   delay (500);
//   Serial.println("Calibrando sensor de CO2...");
//   CO2sensor.calibrate();
//   Serial.println("Calibragem concluída.");

}

void sensorCO2_Loop(){
    
//  int ppm = CO2sensor.read();

//   Serial.print("CO2: ");
//   Serial.print(ppm);
//   Serial.print(" ppm | ");
//   Serial.print(CO2sensor.getVoltage());
//   Serial.println(" mV");

// int adc = analogRead(34); // exemplo
//   float volts = adc * 3.3 / 4095.0;
//     int16_t adc = ads.readADC_SingleEnded(1);      // ler canal AIN1
//     float volts = ads.computeVolts(adc);           // converter para volts
//   Serial.print("ADC: ");
//   Serial.print(adc);
//   Serial.print(" | Volts: ");
//   Serial.println(volts, 3);


unsigned long agoraMs = millis();
  if (agoraMs - ultimoTempoAmostra < INTERVALO_MS) return;
  ultimoTempoAmostra = agoraMs;

  // 1) tirar MEDIANA de NUM_MEDIANA leituras rápidas (elimina spikes)
  float janela[NUM_MEDIANA];
  for (int i = 0; i < NUM_MEDIANA; i++) {
    int16_t bruto = ads.readADC_SingleEnded(1); // canal 1 como no seu código
    janela[i] = ads.computeVolts(bruto);
    delay(6); // pequeno espaçamento para I2C (6ms é suficiente)
  }
  float med = medianaN(janela, NUM_MEDIANA);

  // 2) empilha no buffer circular (janela maior)
  empilharAnel(med);

  // 3) calcula média e desvio padrão da janela
  float mediaV, desvioV;
  calculaMediaDesvio(mediaV, desvioV);

  // 4) EMA sobre média (suaviza a apresentação final)
  if (contagemAnel == 1 && mediaSuavizada == 0.0) 
  mediaSuavizada = mediaV; // inicializa na primeira amostra
  mediaSuavizada = ALFA_EMA * mediaV + (1.0 - ALFA_EMA) * mediaSuavizada;

  // 5) verifica estabilidade e converte p/ ppm
  bool estavel = (contagemAnel == TAMANHO_JANELA) && (desvioV < LIMIAR_STDDEV_V);
  //float Vcorrigida = mediaSuavizada;
  float Vcorrigida = mediaV; // se quiser aplicar A/B no sinal: Vcorrigida = A_cal * mediaSuavizada + B_cal;
  float ppm = voltsParaPPM(Vcorrigida);




  // 6) saída simples e em PT-BR
  if (estavel) {
    Serial.print("CO2 (ppm): ");
    Serial.print(ppm, 1);
    Serial.print(" | V: ");
    Serial.print(Vcorrigida, 4);
    Serial.print(" | stdV: ");
    Serial.println(desvioV, 4);
  } else {
    Serial.print("Ajustando... stdV=");
    Serial.print(desvioV, 4);
    Serial.print(" | Vmedio=");
    Serial.println(mediaV, 4);
  }
}


// // ===== LEITURA ADS1115 =====
//     int16_t adc = ads.readADC_SingleEnded(1);
//     float volts = ads.computeVolts(adc);

//     // ===== FILTRO EXPONENCIAL =====
//     ema = (alpha * volts) + ((1 - alpha) * ema);

//     // ===== CONVERSÃO PARA PPM =====
//     float ppm = voltsToPPM(ema);

//     // ===== EXIBIR RESULTADOS =====
//     Serial.print("ADC: "); Serial.print(adc);
//     Serial.print(" | V: "); Serial.print(volts, 5);
//     Serial.print(" | EMA: "); Serial.print(ema, 5);
//     Serial.print(" | PPM: "); Serial.println(ppm, 1);



// //   delay(1000);



   
//}


