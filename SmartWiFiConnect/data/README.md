# SPIFFS Data Folder

This folder contains files that will be uploaded to ESP32's SPIFFS filesystem.

## Setup

1. Copy `networks.txt.example` to `networks.txt`
2. Edit `networks.txt` with your WiFi credentials (format: `SSID:PASSWORD`)
3. Upload to ESP32 using **Tools > ESP32 Sketch Data Upload** in Arduino IDE

## Requirements

Install the ESP32 Sketch Data Upload tool:
https://github.com/me-no-dev/arduino-esp32fs-plugin

## File Format

```
# Comment lines start with #
NetworkName1:password123
NetworkName2:anotherpass
```

Networks are tried in order of signal strength (strongest first).
