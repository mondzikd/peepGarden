# peepGarden

## Overview

LAN soil moisture monitoring system, showing current and historic data on Node-RED dashboard.

## Prerequisities

### Hardware

- Raspberry Pi 4 Model B (8GB RAM) - Cost effective and compact computer for hosting LAN DNS, MQTT Broker and Application Server.

- ESP32 (DOIT ESP32 DEVKIT V1) - Microcontroler with WiFi and Bluetooth for sending sensor data. Available to program using Arduino IDE.

- Soil moisture sensor - for example "Capaitive Soil Moisture Sensor v1.2"

### Software

#### Raspberry

- [Pi-hole](https://pi-hole.net) - DNS sinkhole that can work as local DNS server

- [Mosquitto](https://mosquitto.org) - MQTT Broker

- [Node-RED](https://nodered.org) - Low-code programming for event-driven applications

#### Normal computer

(on normal computer for convenience only, if you want it can be done on the same Raspberry)

- Node-RED - Browser admin console.

- Arduino IDE with installed _DOIT ESP32 DEVKIT V1 board_

- Arduino libraries:
  - [ArduinoJson](https://arduinojson.org/?utm_source=meta&utm_medium=library.properties)
  - [WiFi](https://www.arduino.cc/en/Reference/WiFi)
  - [MQTT](https://github.com/256dpi/arduino-mqtt)

## Installation

### LAN setup

You can learn more about possible solutions here: [How to make a machine accessible from the LAN using its hostname](https://unix.stackexchange.com/questions/16890/how-to-make-a-machine-accessible-from-the-lan-using-its-hostname). We will use local DNS Server.

1. Static IP - To have peepGarden working and results available in LAN, let's start with configuring DHCP on Internet router. Assign static local IP address for raspberry:
   `DHCP Clients List` > `Assign raspberrypi MAC to IP (For example 192.168.1.101) | Permanent Lease type.`

2. Use LAN DNS - Assign primary DNS to static IP from Point 1 on router.

3. Install and setup LAN DNS - To have reuslts available behind user-friendly DNS address, let's configure LAN DNS on Raspberry Pi. Install [Pi-hole](https://pi-hole.net) on Raspberry. Configure "Local DNS Record" with human-friendly name (For example `pi.lan`) and point it to static IP address from point 1.

### MQTT Broker

[Install Mosquitto MQTT Broker on Raspberry Pi.](https://randomnerdtutorials.com/how-to-install-mosquitto-broker-on-raspberry-pi) with remote access enabled (authentication not needed).

Remote access can be enabled by editing `/etc/mosquitto/mosquitto.conf` with:

```
listener 1883
allow_anonymous true
```

Important part is making broker start on system boot `sudo systemctl enable mosquitto.service`, otherwise you need to start it each system boot manually.

### Node-RED

[Install Node-RED on Raspberry Pi.](https://randomnerdtutorials.com/install-node-red-raspberry-pi)

Important part is making server start on system boot `sudo systemctl enable nodered.service`, otherwise you need to start it each system boot manually.

You can access Admin console in LAN network behind http://pi.lan:1880

### Schema with sensors

### Working prototype
