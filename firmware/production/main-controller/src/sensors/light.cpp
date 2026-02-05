#include <Wire.h>
#include <BH1750.h>

BH1750 lightMeter;

void sensorLight_Setup(){
 // Serial.begin(115200);

  // Initialize the I2C bus (BH1750 library doesn't do this automatically)
  //Wire.begin();
  // On esp8266 you can select SCL and SDA pins using Wire.begin(D4, D3);
  // For Wemos / Lolin D1 Mini Pro and the Ambient Light shield use Wire.begin(D2, D1);

  lightMeter.begin();

  Serial.println(F("BH1750 Test begin"));
}

void sensorLight_Loop() {
  static unsigned long lastRead = 0;
  const unsigned long interval = 2000;

  unsigned long now = millis();

  if (now - lastRead >= interval) {
    lastRead = now;

    float lux = lightMeter.readLightLevel();
    Serial.print("Luz: ");
    Serial.print(lux);
    Serial.println(" lx");
  }
}