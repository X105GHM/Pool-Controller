#include "CurrentSensor.hpp"
#include <math.h>

CurrentSensor::CurrentSensor(uint8_t p) : pin(p) {}

void CurrentSensor::begin()
{
    pinMode(pin, INPUT);
}

void CurrentSensor::sampleWaveform(float *buf, size_t len, uint32_t intervalUs)
{
    for (size_t i = 0; i < len; ++i)
    {
        int raw = analogRead(pin);

        float v = raw / float(ADC_RES) * VCC;

        buf[i] = v * (MAX_CURRENT / VCC);
        delayMicroseconds(intervalUs);
    }
}

float CurrentSensor::computeRMS(const float *buf, size_t len)
{
    double sum = 0;
    for (size_t i = 0; i < len; ++i)
    {
        sum += buf[i] * buf[i];
    }
    return sqrt(sum / len);
}
