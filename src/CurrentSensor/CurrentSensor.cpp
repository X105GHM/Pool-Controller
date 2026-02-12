#include "CurrentSensor.hpp"
#include <math.h>
#include <driver/adc.h>

CurrentSensor::CurrentSensor(uint8_t p) : pin(p) {}

void CurrentSensor::begin()
{
    pinMode(pin, INPUT);
    analogReadResolution(12);
    analogSetPinAttenuation(pin, ADC_11db); 
}

void CurrentSensor::setCalibration(float a_per_v)
{
    aPerVolt = a_per_v;
}

void CurrentSensor::sampleWaveform(float *buf, size_t len, uint32_t intervalUs)
{
    uint64_t sum = 0;
    for (size_t i = 0; i < len; ++i)
    {
        sum += analogRead(pin);
        delayMicroseconds(intervalUs);
    }
    const float meanRaw = float(sum) / float(len);
    const int meanRawInt = int(meanRaw + 0.5f);
    const float mvPerCount = 3300.0f / 4095.0f;
    const float meanMv = meanRaw * mvPerCount;

    for (size_t i = 0; i < len; ++i)
    {
        int raw = analogRead(pin);
        float mv = raw * mvPerCount;
        float acmV = mv - meanMv;
        float v = acmV / 1000.0f; 
        buf[i] = v * aPerVolt;   
        delayMicroseconds(intervalUs);
    }
}

float CurrentSensor::computeRMS(const float *buf, size_t len)
{
    double sum = 0.0;
    for (size_t i = 0; i < len; ++i)
        sum += double(buf[i]) * double(buf[i]);
    return sqrt(sum / double(len));
}
