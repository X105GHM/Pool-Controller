#include <Arduino.h>
#include "PoolController/PoolController.hpp"

PoolController controller;

void setup()
{
  Serial.begin(115200);
  controller.begin();

  String mac = WiFi.macAddress();
  Serial.println("Meine STA-MAC: " + mac);

  String apMac = WiFi.softAPmacAddress();
  Serial.println("Meine AP-MAC:  " + apMac);
}

void loop()
{
  controller.update();
  delay(1000);
}
