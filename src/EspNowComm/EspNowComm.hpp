#pragma once

#include <esp_now.h>
#include <WiFi.h>
#include <functional>
#include <cstdint>
#include <cstring>
#include <esp_wifi.h>

enum CommandType : uint8_t
{
  Pump = 0,
  Freeze = 1,
  Fan = 2
};

struct ControlCommand
{
  CommandType cmdType;
  bool turnOn;
};

static constexpr size_t WF_SAMPLES = 50;
struct PoolData
{
  float tempIn;
  float tempOut;
  float currentRMS;
  bool pumpOn;
  bool freezeOn;
  bool fanOn;
  float waveform[WF_SAMPLES];
};

using RecvCallback = std::function<void(const ControlCommand &)>;

static constexpr uint32_t DISC_MAGIC = 0x504F4F4Cu;

enum : uint8_t
{
  DISC_PING = 1,
  DISC_ACK = 2
};

struct __attribute__((packed)) DiscoverPing
{
  uint32_t magic; 
  uint8_t kind;   
  uint8_t mac[6]; 
};

struct __attribute__((packed)) DiscoverAck
{
  uint32_t magic;
  uint8_t kind; 
  uint8_t mac[6]; 
};

class EspNowComm
{
public:
  void begin(const uint8_t peerMac[6], uint8_t wifiChannel);
  void sendStatus(const PoolData &data);
  void sendCommand(CommandType type, bool turnOn);
  void setReceiveCallback(RecvCallback cb);

  bool discoverPeer(uint8_t chFrom = 1, uint8_t chTo = 13,
                    uint32_t perChanMs = 60, uint8_t rounds = 2);
  uint32_t millisSinceHeard() const;
  uint8_t currentChannel() const { return curChannel; }

private:
  static void onSent(const uint8_t *mac, esp_now_send_status_t status);
  static void onRecv(const uint8_t *mac, const uint8_t *data, int len);
  static RecvCallback recvCb;

  static volatile uint32_t s_lastHeardMs;

  uint8_t peerAddr[6]{};
  uint8_t curChannel = 1;

  void setChannel(uint8_t ch);
  bool reAddPeer(uint8_t ch);
};
