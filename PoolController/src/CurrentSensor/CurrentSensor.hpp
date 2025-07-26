#pragma once

#include <Arduino.h>

class CurrentSensor
{
public:
    CurrentSensor(uint8_t p);

    void begin();

    void sampleWaveform(float *buf, size_t len, uint32_t intervalUs);

    float computeRMS(const float *buf, size_t len);

private:
    uint8_t pin;
    static constexpr float MAX_CURRENT = 5.0f; // 3.3 V → 5 A
    static constexpr float VCC = 3.3f;         // Referenzspannung
    static constexpr int ADC_RES = 4095;       // 12-Bit ADC
};

