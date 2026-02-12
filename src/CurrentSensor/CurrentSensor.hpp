#pragma once
#include <Arduino.h>

class CurrentSensor
{
public:
    explicit CurrentSensor(uint8_t p);

    void begin();     
    void setCalibration(float a_per_v); 
    void sampleWaveform(float *buf, size_t len, uint32_t intervalUs);
    float computeRMS(const float *buf, size_t len);

private:
    uint8_t pin;
    float aPerVolt = 1.0f; 
    static constexpr int ADC_RES = 4095;
};
