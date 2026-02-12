# Pool Controller Firmware

This repository contains the firmware running on an ESP32-based Pool Controller node. It reads temperature and current data, receives control commands over ESP-NOW, and drives local relays to switch the pool pump, freeze-protection heater, and cooling fan.

## Features

### Temperature Sensing
- Two DS18B20 sensors (water inlet & outlet) on a shared OneWire bus

### Current Measurement
- AC current sensor (ZMCT103C) sampling a full waveform and computing RMS

### Relay Outputs
- Solid-state relays to switch:
  - Pool pump  
  - Freeze-protection heater  
  - Cooling fan  

### Wireless Control & Monitoring
- ESP-NOW peer to a central gateway  
- Periodic status broadcast (temperatures, current, relay states, waveform)  
- Receives commands (Pump / Freeze / Fan ON ↔ OFF)  

### Failsafe
- If no gateway contact for 30 s, all relays are turned OFF  

### Modular C++ / FreeRTOS
- Clean separation: sensors, relays, wireless communication  
- PlatformIO project with `.cpp` / `.hpp` modules  

## Hardware

- **MCU:** ESP32-WROOM  
- **Temperature Sensors:** 2× DS18B20 on GPIO 18 (shared)  
- **Current Sensor:** ZMCT103C on ADC GPIO 32  
- **Relays:** 3 × SSRs on GPIO 25 (pump), 26 (freeze), 23 (fan)  
- **Power:** 3.3 V regulated supply  

## Gallery

![Pool Controller 1](https://raw.githubusercontent.com/X105GHM/Pool-Controller/assets/WhatsApp%20Image%202025-07-26%20at%2018.36.41%20(1).jpeg)
![Pool Controller 2](https://raw.githubusercontent.com/X105GHM/Pool-Controller/assets/WhatsApp%20Image%202025-07-26%20at%2018.36.41.jpeg)
![Pool Controller 3](https://raw.githubusercontent.com/X105GHM/Pool-Controller/assets/WhatsApp%20Image%202025-07-26%20at%2018.36.42%20(1).jpeg)
![Pool Controller 4](https://raw.githubusercontent.com/X105GHM/Pool-Controller/assets/WhatsApp%20Image%202025-07-26%20at%2018.36.42.jpeg)

