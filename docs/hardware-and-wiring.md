# Hardware Interconnection & Wiring Guide

Detailed pin mappings, electrical characteristics, and power distribution for the **BloodLink** dual-ESP8266 architecture.

---

## 📌 NodeMCU ESP8266 Pinout Mapping Reference

The ESP8266 NodeMCU development board maps physical board labels (e.g., `D1`, `D2`) to internal ESP8266 GPIO numbers. The table below provides the authoritative cross-reference:

| NodeMCU Pin | ESP8266 GPIO | Primary Function | Boot Pull State | Notes for Peripherals |
| :---: | :---: | :---: | :---: | :--- |
| **D0** | GPIO16 | User GPIO / Wake | High on boot | Used as **RFID RST** |
| **D1** | GPIO5 | I2C SCL | Floating | Used as **OLED SCL** (MCU1) / **DHT11 Data** (MCU2) |
| **D2** | GPIO4 | I2C SDA | Floating | Used as **OLED SDA** (MCU1) / **IR1 Out** (MCU2) |
| **D3** | GPIO0 | Flash Mode | Pull-up (Bootloader) | Avoid using during active boot |
| **D4** | GPIO2 | TXD1 / User GPIO | Pull-up (Bootloader) | Used as **RFID SS / CS** (MCU1) |
| **D5** | GPIO14 | HSPI SCK | Floating | Used as **RFID SCK** (MCU1) / **IR2 Out** (MCU2) |
| **D6** | GPIO12 | HSPI MISO | Floating | Used as **RFID MISO** (MCU1) |
| **D7** | GPIO13 | HSPI MOSI | Floating | Used as **RFID MOSI** (MCU1) |
| **D8** | GPIO15 | HSPI SS | Pull-down | Unused |

---

## 🔌 MCU1 Wiring: RFID Scanner & OLED Display

**Device Code**: `ESP8266-RFID-01`

```
  +------------------+                    +-----------------+
  |  ESP8266 NodeMCU |                    |  MFRC522 RFID   |
  |                  |                    |                 |
  |             3.3V |------------------->| VCC (3.3V ONLY) |
  |              GND |------------------->| GND             |
  |        D0 (GP16) |------------------->| RST             |
  |        D4 (GP02) |------------------->| SDA (SS)        |
  |        D5 (GP14) |------------------->| SCK             |
  |        D6 (GP12) |<-------------------| MISO            |
  |        D7 (GP13) |------------------->| MOSI            |
  |                  |                    +-----------------+
  |                  |                    +-----------------+
  |                  |                    |  SSD1306 OLED   |
  |                  |                    |                 |
  |             3.3V |------------------->| VCC             |
  |              GND |------------------->| GND             |
  |        D1 (GP05) |------------------->| SCL             |
  |        D2 (GP04) |<------------------>| SDA             |
  +------------------+                    +-----------------+
```

### MCU1 Verification Checklist:
* [x] **MFRC522 Supply Voltage**: Supplied strictly with **3.3V** from NodeMCU. Applying 5V will destroy the MFRC522 IC.
* [x] **I2C Pull-ups**: Adafruit SSD1306 breakout includes internal 4.7kΩ pull-up resistors to 3.3V.
* [x] **I2C Address**: Verified as `0x3C`.

---

## 🔌 MCU2 Wiring: Environmental Telemetry & Slot Occupancy

**Device Code**: `ESP8266-SENSOR-01`

```
  +------------------+                    +-----------------+
  |  ESP8266 NodeMCU |                    |  DHT11 Climate  |
  |                  |                    |                 |
  |         3.3V/VIN |------------------->| VCC             |
  |              GND |------------------->| GND             |
  |        D1 (GP05) |<-------------------| DATA            |
  |                  |                    +-----------------+
  |                  |                    +-----------------+
  |                  |                    |  IR Sensor 1    |
  |                  |                    |  (Slot S1-01)   |
  |         3.3V/VIN |------------------->| VCC             |
  |              GND |------------------->| GND             |
  |        D2 (GP04) |<-------------------| OUT (Active LOW)|
  |                  |                    +-----------------+
  |                  |                    +-----------------+
  |                  |                    |  IR Sensor 2    |
  |                  |                    |  (Slot S1-02)   |
  |         3.3V/VIN |------------------->| VCC             |
  |              GND |------------------->| GND             |
  |        D5 (GP14) |<-------------------| OUT (Active LOW)|
  +------------------+                    +-----------------+
```

### MCU2 Verification Checklist:
* [x] **DHT11 Data Line**: Connected to `D1` (GPIO5). If using a 3-pin breakout module, the 10kΩ pull-up is included onboard.
* [x] **IR Sensor Polarity**: Verified comparator logic outputs **LOW (0V)** when reflection occurs (bag present) and **HIGH (3.3V/5V)** when slot is clear.
* [x] **Slot Expandability**: Slots `S2-01` and `S2-02` are unpopulated in the prototype, documented as ready for expansion.

---

## ⚡ Electrical Power & Decoupling

* **NodeMCU Onboard Regulator**: AMS1117-3.3 provides up to 800mA peak current.
* **Peak Wi-Fi Bursts**: ESP8266 draws up to 250mA during Wi-Fi transmission bursts. To prevent brownout resets during simultaneous RFID reads and Wi-Fi uploads, place a **100µF electrolytic capacitor** across the 3.3V and GND rails near the MFRC522.
* **Common Ground**: All ground lines between controllers, sensors, and power supplies must be connected together.
