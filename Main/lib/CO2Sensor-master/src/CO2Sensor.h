#ifndef CO2SENSOR_H
#define CO2SENSOR_H

#include <Arduino.h>
#include <Adafruit_ADS1X15.h>

class CO2Sensor {
public:
    CO2Sensor(int adsChannel);
    CO2Sensor(int adsChannel, float inertia, int tries);

    int read();
    void calibrate();

    int getVoltage();     // mV filtrado
    int getGreenLevel();  // 0–255
    int getRedLevel();    // 0–255

private:
    void init();

    int _adsChannel;
    float _inertia;
    int _tries;

    float _co2_v;     // tensão filtrada (mV)
    int _co2ppm;      // ppm calculado
    int _greenLevel;  // cor do LED (0-255)

    float _co2_a;     // coeficiente do modelo matemático

    // constantes do modelo
    
};

#endif
