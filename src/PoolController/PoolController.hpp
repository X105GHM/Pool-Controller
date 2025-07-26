#pragma once
#include <Arduino.h>
#include "DS18B20Sensor/DS18B20Sensor.hpp"
#include "CurrentSensor/CurrentSensor.hpp"
#include "Relay/Relay.hpp"
#include "EspNowComm/EspNowComm.hpp"
#include <esp_wifi.h>

static constexpr uint32_t SAMPLE_INTERVAL_US = 20000u / WF_SAMPLES;
static constexpr uint32_t CONNECTION_TIMEOUT_MS = 30000u;

static const DeviceAddress ADDRESS_REIN = {0x28, 0x61, 0x64, 0x0A, 0x48, 0x79, 0x0B, 0xCF}; // Rein sensor address
static const DeviceAddress ADDRESS_RAUS = {0x28, 0x61, 0x64, 0x0A, 0x48, 0x4B, 0xD0, 0x04}; // Raus sensor address

class PoolController
{
public:
    PoolController();
    void begin();
    void update();

private:
    DS18B20Sensor tempIn, tempOut;
    CurrentSensor current;
    Relay pumpRelay, freezeRelay, fanRelay;
    EspNowComm comm;
    uint32_t lastRecvTime = 0;
    float waveform[WF_SAMPLES];
    void onControlCommand(const ControlCommand &cmd);
    void failSafe();
    void readSensors(float &tIn, float &tOut, float &rms);
};
