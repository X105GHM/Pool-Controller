#include "EspNowComm.hpp"
#include <Arduino.h>
#include <esp_wifi.h>

RecvCallback EspNowComm::recvCb = nullptr;

void EspNowComm::begin(const uint8_t peerMac[6], uint8_t channel)
{
    Serial.println("ESP-NOW: Initializing...");
    
    WiFi.mode(WIFI_STA);

    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);

    esp_err_t initResult = esp_now_init();

    if (initResult != ESP_OK)
    {
        Serial.printf("ESP-NOW init failed: %d\n", initResult);
        return;
    }
    Serial.println("ESP-NOW init OK");

    memcpy(peerAddr, peerMac, 6);
    Serial.print("ESP-NOW: Peer MAC = ");

    for (int i = 0; i < 6; ++i)
    {
        Serial.printf("%02X", peerAddr[i]);
        if (i < 5)
            Serial.print(":");
    }
    Serial.println();

    esp_now_register_send_cb(onSent);
    esp_now_register_recv_cb(onRecv);

    esp_now_peer_info_t peer = {};
    memcpy(peer.peer_addr, peerAddr, 6);
    peer.channel = channel;
    peer.encrypt = false;

    esp_err_t addResult = esp_now_add_peer(&peer);
    if (addResult != ESP_OK)
    {
        Serial.printf("ESP-NOW add_peer failed: %d\n", addResult);
    }
    else
    {
        Serial.println("ESP-NOW peer added");
    }
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
    Serial.printf("sendCommand type=%u turnOn=%d → %s (len=%u)\n", unsigned(type), int(turnOn), res == ESP_OK ? "OK" : "FAIL", unsigned(sizeof(cmd)));
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

    if (len == (int)sizeof(ControlCommand) && recvCb)
    {
        ControlCommand cmd;
        memcpy(&cmd, data, sizeof(cmd));
        Serial.printf("  Parsed ControlCommand: type=%u turnOn=%d\n", unsigned(cmd.cmdType), int(cmd.turnOn));
        recvCb(cmd);
    }
    else
    {
        Serial.println("  Received unknown packet or no callback set");
    }
}
