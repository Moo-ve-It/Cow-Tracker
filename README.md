# Cow Tracker

![Moo-ve-It Logo](assets/cow_farm.jpg)

## Overview

ESP32 firmware for real-time cow tracking and monitoring as part of the Moo-ve-It smart farm system. This firmware collects location data from air tag modules and transmits it to Sentry.io for processing by the backend, enabling autonomous herd management through robotic control.

## Features

- **Air Tag Integration** - Reads location data from air tag modules for real-time cow positioning
- **Sentry.io Telemetry** - Sends structured JSON events to Sentry.io at regular intervals
- **Sensor Data Collection** - Monitors battery level and optional environmental data
- **Low Power Design** - Optimized for battery-powered operation on livestock
- **Network Resilience** - Automatic reconnection handling for WiFi disruptions
- **Configurable Reporting** - Adjustable transmission intervals

## Getting Started

### Prerequisites

- Arduino IDE (1.8.x+) or PlatformIO
- ESP32 board support package
- Required libraries:
  - WiFi (built-in)
  - HTTPClient (built-in)
  - ArduinoJson (6.x+)
  - Sentry SDK for Arduino (if available) or custom HTTP implementation

### Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/Moo-ve-It/Cow-Tracker.git
   cd Cow-Tracker
   ```

2. Configure credentials:
   ```bash
   cp secrets.h.example secrets.h
   ```
   Edit `secrets.h` with your settings:
   - WiFi SSID and password
   - Sentry.io DSN
   - Reporting interval (milliseconds)

3. Flash to ESP32:
   - Open the project in Arduino IDE or PlatformIO
   - Select your ESP32 board
   - Upload the firmware

## Configuration

Edit `secrets.h` to configure:

```cpp
#define WIFI_SSID "your-network"
#define WIFI_PASSWORD "your-password"
#define SENTRY_DSN "https://your-sentry-dsn@sentry.io/project-id"
#define REPORT_INTERVAL 30000  // milliseconds
#define DEVICE_ID "cow-001"
```

## Data Format

The firmware sends JSON payloads to Sentry.io:

```json
{
  "device_id": "cow-001",
  "timestamp": 1700000000,
  "location": {
    "tag_id": "tag-abc123",
    "x": 45.2,
    "y": 78.5
  },
  "battery_level": 87,
  "temperature": 22.5
}
```

## Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/improvement`)
3. Commit your changes (`git commit -am 'Add new feature'`)
4. Push to the branch (`git push origin feature/improvement`)
5. Open a Pull Request

## License

MIT License

## Acknowledgments

Developed by the Moo-ve-It team for the hackathon. Special thanks to our sponsors:
- Daytona
- Sentry
- Code Rabbit
- BrowserStack
- Galileo

---

Part of the [Moo-ve-It](https://github.com/Moo-ve-It) smart farm ecosystem.
