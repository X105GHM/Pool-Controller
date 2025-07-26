#include "DS18B20Sensor.hpp"
#include <cstring>

DS18B20Sensor::DS18B20Sensor(uint8_t pin, const DeviceAddress &addr)
    : ow(pin), dt(&ow)
{
  memcpy(address, addr, sizeof(DeviceAddress));
}

void DS18B20Sensor::begin()
{
  dt.begin();
}

float DS18B20Sensor::readTemperature()
{
  dt.requestTemperaturesByAddress(address);
  return dt.getTempC(address);
}
