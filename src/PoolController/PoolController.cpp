#include "PoolController.hpp"

static constexpr uint8_t PIN_ONEWIRE = 18;
static constexpr uint8_t PIN_RELAY = 25;
static constexpr uint8_t PIN_CURRENT = 32;
static constexpr uint8_t PIN_FREEZE = 26;
static constexpr uint8_t PIN_FAN = 23;
static constexpr uint8_t GATEWAY_MAC[6] = {0xF8, 0xB3, 0xB7, 0x47, 0xE0, 0x28}; // F8:B3:B7:47:E0:28

PoolController::PoolController()
    : tempIn(PIN_ONEWIRE, ADDRESS_REIN),
      tempOut(PIN_ONEWIRE, ADDRESS_RAUS),
      current(PIN_CURRENT),
      pumpRelay(PIN_RELAY),
      freezeRelay(PIN_FREEZE),
      fanRelay(PIN_FAN),
      comm() {}

void PoolController::begin()
{
    tempIn.begin();
    tempOut.begin();
    current.begin();
    pumpRelay.begin();
    freezeRelay.begin();
    fanRelay.begin();
    const uint8_t START_CHANNEL = 1;
    comm.begin(GATEWAY_MAC, START_CHANNEL);
    comm.setReceiveCallback([this](const ControlCommand &c){ onControlCommand(c); });
   // comm.discoverPeer();
}


void PoolController::update()
{
   // if (comm.millisSinceHeard() > CONNECTION_TIMEOUT_MS) {
   //     failSafe();
   //     comm.discoverPeer();
   // }

    float tIn, tOut, rms;
    readSensors(tIn, tOut, rms);
    PoolData d;
    d.tempIn = tIn;
    d.tempOut = tOut;
    d.currentRMS = rms;
    d.pumpOn = pumpRelay.isOn();
    d.freezeOn = freezeRelay.isOn();
    d.fanOn = fanRelay.isOn();
    memcpy(d.waveform, waveform, sizeof(waveform));
    comm.sendStatus(d);
    Serial.printf("Temp In: %.2f °C, Temp Out: %.2f °C, RMS: %.2f A (ch=%u)\n", tIn, tOut, rms, comm.currentChannel());
}

void PoolController::onControlCommand(const ControlCommand &c)
{
    switch (c.cmdType)
    {
    case CommandType::Pump:   c.turnOn ? pumpRelay.on()   : pumpRelay.off();   break;
    case CommandType::Freeze: c.turnOn ? freezeRelay.on() : freezeRelay.off(); break;
    case CommandType::Fan:    c.turnOn ? fanRelay.on()    : fanRelay.off();    break;
    }
}

void PoolController::failSafe()
{
    static bool once = false;
    if (!once)
    {
        pumpRelay.off();
        freezeRelay.off();
        fanRelay.off();
        once = true;
    }
}

void PoolController::readSensors(float &tIn, float &tOut, float &rms)
{
    tIn = tempIn.readTemperature();
    tOut = tempOut.readTemperature();
    current.sampleWaveform(waveform, WF_SAMPLES, SAMPLE_INTERVAL_US);
    rms = current.computeRMS(waveform, WF_SAMPLES);
}
