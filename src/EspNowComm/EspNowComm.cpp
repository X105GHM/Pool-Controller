#include "EspNowComm.hpp"
#include <Arduino.h>
#include <esp_wifi.h>

RecvCallback EspNowComm::recvCb = nullptr;
volatile uint32_t EspNowComm::s_lastHeardMs = 0;

void EspNowComm::begin(const uint8_t peerMac[6], uint8_t channel)
{
    Serial.println("ESP-NOW: Initializing...");

    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);    
    esp_wifi_set_ps(WIFI_PS_NONE);
    setChannel(channel);

    esp_err_t initResult = esp_now_init();
    if (initResult != ESP_OK)
    {
        Serial.printf("ESP-NOW init failed: %d\n", initResult);
        return;
    }
    Serial.println("ESP-NOW init OK");

    memcpy(peerAddr, peerMac, 6);

    esp_now_register_send_cb(onSent);
    esp_now_register_recv_cb(onRecv);

    reAddPeer(channel);

    s_lastHeardMs = 0;
}

void EspNowComm::sendStatus(const PoolData &data)
{
    esp_err_t res = esp_now_send(peerAddr, (uint8_t *)&data, sizeof(data));
    Serial.printf("sendStatus → %s (len=%u)\n", res == ESP_OK ? "OK" : "FAIL", unsigned(sizeof(data)));
}

void EspNowComm::sendCommand(CommandType type, bool turnOn)
{
    ControlCommand cmd{type, turnOn};
    esp_err_t res = esp_now_send(peerAddr, (uint8_t *)&cmd, sizeof(cmd));
    Serial.printf("sendCommand type=%u turnOn=%d → %s (len=%u)\n",
                  unsigned(type), int(turnOn), res == ESP_OK ? "OK" : "FAIL", unsigned(sizeof(cmd)));
}

void EspNowComm::setReceiveCallback(RecvCallback cb)
{
    recvCb = cb;
    Serial.println("ESP-NOW: Receive callback set");
}

void EspNowComm::onSent(const uint8_t *mac, esp_now_send_status_t status)
{
    char macStr[18];
    sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    Serial.printf("onSent to %s → %s\n", macStr, status == ESP_NOW_SEND_SUCCESS ? "SUCCESS" : "FAIL");
}

void EspNowComm::onRecv(const uint8_t *mac, const uint8_t *data, int len)
{
    char macStr[18];
    sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    Serial.printf("onRecv from %s, len=%d\n", macStr, len);

    if (len >= (int)sizeof(DiscoverAck))
    {
        const DiscoverAck *a = reinterpret_cast<const DiscoverAck *>(data);
        if (a->magic == DISC_MAGIC && a->kind == DISC_ACK)
        {
            s_lastHeardMs = millis();
            Serial.println("  Got DISC_ACK");
            return; 
        }
    }

    if (len == (int)sizeof(ControlCommand) && recvCb)
    {
        ControlCommand cmd;
        memcpy(&cmd, data, sizeof(cmd));
        Serial.printf("  Parsed ControlCommand: type=%u turnOn=%d\n",
                      unsigned(cmd.cmdType), int(cmd.turnOn));
        s_lastHeardMs = millis();
        recvCb(cmd);
    }
    else
    {
        Serial.println("  Received unknown packet or no callback set");
    }
}

void EspNowComm::setChannel(uint8_t ch)
{
    esp_wifi_set_channel(ch, WIFI_SECOND_CHAN_NONE);
    curChannel = ch;
}

bool EspNowComm::reAddPeer(uint8_t ch)
{
    esp_now_del_peer(peerAddr);
    esp_now_peer_info_t peer = {};
    memcpy(peer.peer_addr, peerAddr, 6);
    peer.channel = ch;
    peer.encrypt = false;
    esp_err_t addResult = esp_now_add_peer(&peer);
    Serial.printf("ESP-NOW add_peer (chan %u): %d\n", ch, addResult);
    return addResult == ESP_OK;
}

bool EspNowComm::discoverPeer(uint8_t chFrom, uint8_t chTo,
                              uint32_t perChanMs, uint8_t rounds)
{
    Serial.println("DISCOVERY: scanning channels …");
    uint8_t my[6];
    esp_wifi_get_mac(WIFI_IF_STA, my);

    for (uint8_t r = 0; r < rounds; ++r)
    {
        for (uint8_t ch = chFrom; ch <= chTo; ++ch)
        {
            setChannel(ch);
            reAddPeer(ch);

            DiscoverPing p{};
            p.magic = DISC_MAGIC;
            p.kind = DISC_PING;
            memcpy(p.mac, my, 6);

            esp_now_send(peerAddr, reinterpret_cast<uint8_t *>(&p), sizeof(p));
            uint32_t t0 = millis();

            while (millis() - t0 < perChanMs)
            {
                if (s_lastHeardMs >= t0)
                {
                    Serial.printf("DISCOVERY: lock on channel %u\n", ch);
                    return true;
                }
                delay(2);
            }
        }
    }
    Serial.println("DISCOVERY: no reply");
    return false;
}

uint32_t EspNowComm::millisSinceHeard() const
{
    uint32_t last = s_lastHeardMs;
    if (last == 0)
        return 0xFFFFFFFFu;
    uint32_t now = millis();
    return now >= last ? (now - last) : 0u;
}
