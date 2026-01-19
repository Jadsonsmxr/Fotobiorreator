#include <Wire.h>
#include <Adafruit_ADS1X15.h>

extern Adafruit_ADS1115 ads;

// ===== CONFIGURAÇÕES DO SENSOR =====
const float ppm1 = 400.0;       // ponto 1: ar ambiente
const float ppm2 = 40000.0;     // ponto 2: ar exalado (valor típico)

unsigned long coletaMs = 30000; // 30 segundos coletando

// variáveis de calibração
float V1 = 0, V2 = 0;
float A_calc = 0, B_calc = 0;

// === FUNÇÃO PARA COLETAR MÉDIA EM UM INTERVALO ===
float coletarMedia(unsigned long tempoMs) {
  unsigned long inicio = millis();
  float soma = 0;
  int contador = 0;

  while (millis() - inicio < tempoMs) {
    int16_t adc = ads.readADC_SingleEnded(1);
    float volts = ads.computeVolts(adc);

    soma += volts;
    contador++;

    delay(50); // 20 amostras por segundo
  }

  return soma / contador;
}

void calibrationCo2() {
    Wire.begin();

  if (!ads.begin()) {
    Serial.println("Erro ao inicializar o ADS1115!");
    while (1);
  }

  // GAIN_ONE = ±4.096V, ideal para sinal do MG811
  

  Serial.println("\n========== CALIBRAÇÃO MG811 ==========");
//   Serial.println("Aguarde 5 minutos para pre-aquecimento...");
//   delay(5 * 60 * 1000);

  // ===== PONTO 1: AR AMBIENTE =====
  Serial.println("\n[1/2] Coloque o sensor em ar ambiente.");
  delay(2000);
  Serial.println("Coletando dados por 30 segundos...");
  V1 = coletarMedia(coletaMs);
  Serial.print("V1 = ");
  Serial.print(V1, 6);
  Serial.println(" V");

  // ===== PONTO 2: AR EXALADO =====
  Serial.println("\nAgora prepare o saco com AR EXALADO.");
  Serial.println("Coloque o sensor dentro e aguarde estabilizar (30–40 s).");

  for (int i = 30; i > 0; i--) {
    Serial.print("Aguardando estabilizar: ");
    Serial.print(i);
    Serial.println(" s");
    delay(1000);
  }

  Serial.println("Coletando dados (ar exalado) por 30 segundos...");
  V2 = coletarMedia(coletaMs);

  Serial.print("V2 = ");
  Serial.print(V2, 6);
  Serial.println(" V");

  // ===== CÁLCULO DOS COEFICIENTES A E B ==
  B_calc = (V2 - V1) / (log(ppm2) - log(ppm1));
  A_calc = V1 - B_calc * log(ppm1);

  Serial.println("\n========== RESULTADO FINAL ==========");
  Serial.print("A = ");
  Serial.println(A_calc, 10);
  Serial.print("B = ");
  Serial.print(B_calc, 10);

  Serial.println("\n=== Fórmula para converter V → ppm ===");
  Serial.print("ppm = exp((V - ");
  Serial.print(A_calc, 6);
  Serial.print(") / ");
  Serial.print(B_calc, 6);
  Serial.println(");");

  Serial.println("\nCalibração concluída.");
}


