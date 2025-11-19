#include "CO2Sensor.h"

// ADS1115 externo (instância global)
extern Adafruit_ADS1115 ads;

const int co2_b = 600;
const int co2_d = 400;

#define CO2_LOW   600
#define CO2_HIGHT 1000  // (typo mantido por compatibilidade)

// -----------------------
//   Construtores
// -----------------------

CO2Sensor::CO2Sensor(int adsChannel) {
    _inertia = 0.99;
    _tries = 3;
    _adsChannel = adsChannel;
    init();
}

CO2Sensor::CO2Sensor(int adsChannel, float inertia, int tries) {
    _inertia = inertia;
    _tries = tries;
    _adsChannel = adsChannel;
    init();
}

// -----------------------
//       init()
// -----------------------

void CO2Sensor::init() {
    _co2_a = 1500;   // valor inicial (será calibrado)
    _co2ppm = co2_d;
}

// -----------------------
//      read()
// -----------------------

int CO2Sensor::read() {

    int sum = 0;

    // leitura descartada melhora ruído em ADC´s externos
    ads.readADC_SingleEnded(_adsChannel);

    for (int i = 0; i < _tries; i++) {
        sum += ads.readADC_SingleEnded(_adsChannel);
        delay(20);
    }

    // Média bruta (0–32767)
    //float avgRaw = sum / float(_tries);

    // Conversão p/ mV
    
    //float millivolts = avgRaw * 0.1875;

    // Filtro de inércia (EMA)
    _co2_v = (1 - _inertia) * (sum*6144.0)/(32768.0*_tries) + (_co2_v * _inertia);

    // Converte tensão → ppm via modelo exponencial
    double co2_exp = (_co2_a - _co2_v) / co2_b;
    _co2ppm = pow(co2_d, co2_exp);

    // Níveis de LED
    if (_co2ppm < CO2_LOW) {
        _greenLevel = 255;
    }
    else if (_co2ppm > CO2_HIGHT) {
        _greenLevel = 0;
    }
    else {
        _greenLevel = map(_co2ppm, CO2_LOW, CO2_HIGHT, 255, 0);
    }

    return _co2ppm;
}

// -----------------------
//     calibrate()
// -----------------------

void CO2Sensor::calibrate() {
    read();
    _co2_a = _co2_v + co2_b;  // ajusta coeficiente
    _co2ppm = co2_d;          // ppm padrão (400 ppm)
}

// -----------------------
//       getters
// -----------------------

int CO2Sensor::getVoltage() {
    return _co2_v;
}

int CO2Sensor::getGreenLevel() {
    return _greenLevel;
}

int CO2Sensor::getRedLevel() {
    return 255 - _greenLevel;
}
