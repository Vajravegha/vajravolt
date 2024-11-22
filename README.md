# Vajravolt AI IoT 4G LTE Industrial Gateway Board (VVM701)

![Vajravolt Isometric View](https://github.com/Vajravegha/vajravolt/blob/main/Images/1.IsometricView.jpg)

## Overview

The **Vajravolt AI IoT Gateway Board (VVM701)** is a compact, feature-rich development platform designed for embedded Artificial Intelligence (AI) and the Internet of Things (IoT). Developed by **Vajravegha Mobility Pvt Ltd**, this all-in-one board simplifies development and deployment of edge devices, industrial automation, and data acquisition solutions. 

This board integrates multiple interfaces, connectivity options, and peripheral support for seamless development while ensuring high performance and ease of use.

---

## Key Features

- **Microcontroller**: ESP32-S3 WROOM-1-N16R8
  - Wi-Fi and Bluetooth 5
  - AI acceleration support
  - 240 MHz processor with 16MB Flash and 8MB PSRAM
- **Cellular Connectivity**: Quectel EC200U-CN 4G LTE Cat 1 module with GNSS/GPS
- **I/O Interfaces**:
  - RS485/RS232 (jumper selectable)
  - 5 Analog Inputs (4–20mA / 0–5V)
  - 4 Isolated Digital Inputs (up to 24V)
  - 2 Relay Outputs (30V DC/2A)
- **Memory and Expansion**:
  - Micro SD card slot (16GB max)
  - Nano SIM slot
- **Connectivity**:
  - Ethernet (via optional W5500 module)
  - Protocols: MQTT, HTTP, FTP, Modbus, SPI, I2C, etc.
  - Over-the-air updates (OTA)
- **Power Supply**: 9–30V DC
- **Size**: PCB dimensions of 130mm x 105mm
- **Peripherals**:
  - Support for cameras (AI vision), microphones (I2S audio), LCDs, and LoRa modules.

---

## Applications

- Embedded/Edge AIoT
- Industrial Automation
- Data Acquisition and Logging
- Control Systems
- Remote Monitoring

---

## Hardware Specifications

### Pin Mapping and Peripheral Interfaces

![Top View with Labels](https://github.com/Vajravegha/vajravolt/blob/main/Images/3.TopViewLabel.jpg)

### Basic Schematic

![Basic Schematic](https://github.com/Vajravegha/vajravolt/blob/main/Images/4.BasicSchematic.jpg)

### Supported Peripherals

![Peripherals](https://github.com/Vajravegha/vajravolt/blob/main/Images/6.TopWithPeripherals.jpg)

- **Ethernet**: W5500 Module
- **Display**: 2.4" TFT LCD with Touch
- **Camera**: OV Series (OV2640, OV7670, etc.)
- **Microphone**: INMP441 Digital I2S
- **LoRa**: RFM95W module
- **CAN Transceiver**: TJA1050/TJA1051
- **Accelerometer**: MPU6050

---

## Setup Instructions

1. **Power Supply**:
   - Connect a regulated DC supply (9–30V) to the power input terminal.
   - Ensure proper polarity to avoid damage.
2. **4G and GPS**:
   - Attach the 4G antenna and insert a Nano SIM card with an active data plan.
   - Connect an active GPS antenna if required.
3. **Micro SD Card**:
   - Insert a Micro SD card (optional) for data logging or storage.
4. **Programming**:
   - Connect the board to a computer via USB Type-B for programming.
   - Use Arduino IDE or similar platforms with ESP32-S3 configuration.
   - Default Baud Rate: **115200**.
5. **Web Interface**:
   - Connect the device to the hotspot with SSID: `iotgateway` and password: `iot@1234`.
   - Access the default web interface at `iotgateway.local`.

---

## Getting Started with the Default Code

The board ships with a default program (`BasicTest`). To upload your own program:
1. Set board type to **ESP32-S3 Dev Module** in Arduino IDE.
2. Ensure "USB CDC on Boot" is enabled in the Tools menu.
3. Install required libraries and upload the code.

For troubleshooting, use the serial monitor or tools like PuTTY.

---

## Additional Notes and Precautions

- Only one communication mode (RS485/RS232/CAN) can be active at a time.
- Ensure correct AT commands for the 4G module to avoid memory corruption.
- Use high-quality regulated power supplies for reliable operation.
- This board is **not suitable for direct voice calls**.

---

## Sample Codes and Documentation

Access sample codes, libraries, and detailed documentation on our [GitHub Repository](http://www.github.com/vajravegha/vajravolt).

---

## Technical Support

For queries and assistance, contact us at:

- Email: [info@vv-mobility.com](mailto:info@vv-mobility.com)

Visit our [website](http://www.vv-mobility.com) for more information.

---

### Credits

Developed by Vajravegha Mobility Pvt Ltd as part of the **Make in India** initiative.
# vajravolt
