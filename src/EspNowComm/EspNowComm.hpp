#pragma once

#include <esp_now.h>
#include <WiFi.h>
#include <functional>
#include <cstdint>
#include <cstring>
#include <esp_wifi.h>

enum CommandType : uint8_t {
  Pump   = 0,
  Freeze = 1,
  Fan    = 2
};

struct ControlCommand {
  CommandType cmdType;
  bool        turnOn;
};

static constexpr size_t WF_SAMPLES = 50;
struct PoolData {
  float tempIn;
  float tempOut;
  float currentRMS;
  bool  pumpOn;
  bool  freezeOn;
  bool  fanOn;
  float waveform[WF_SAMPLES];
};

using RecvCallback = std::function<void(const ControlCommand&)>;

class EspNowComm {
public:
  void begin(const uint8_t peerMac[6], uint8_t wifiChannel);
  void sendStatus(const PoolData& data);
  void sendCommand(CommandType type, bool turnOn);
  void setReceiveCallback(RecvCallback cb);
private:
  static void onSent(const uint8_t* mac, esp_now_send_status_t status);
  static void onRecv(const uint8_t* mac, const uint8_t* data, int len);
  static RecvCallback recvCb;
  uint8_t peerAddr[6];
};
