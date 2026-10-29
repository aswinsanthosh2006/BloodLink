# MCU2 Firmware — Environmental Monitoring & Slot Occupancy

Firmware for the secondary telemetry node (`ESP8266-SENSOR-01`) in the **BloodLink** Smart Blood Bank system.

---

## 📋 Responsibilities

* **Environmental Telemetry**: Periodic measurement of internal rack temperature (°C) and relative humidity (%) using a calibrated DHT11 sensor.
* **Storage Slot Presence Detection**: Real-time optical sensing of slot occupancy for blood bank compartments using two digital infrared (IR) proximity sensors (`IR-01` and `IR-02`).
* **Heartbeat & Telemetry Ingestion**: Dispatches telemetry every 30 seconds over HTTPS to the Supabase Edge Function (`/functions/v1/device-ingest`), simultaneously refreshing the device heartbeat timestamp (`last_seen_at`) on the dashboard.
* **Data Sanitization**: Protects cloud records against faulty transmission by filtering out `NaN` or uncalibrated readings.

---

## 🔌 Hardware Connections

### Pin Mapping Matrix (NodeMCU v2/v3 ESP8266)

| Peripheral Module | Pin Label | ESP8266 Pin | GPIO Pin | Logic Level | Notes |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **DHT11 Sensor** | DATA | **D1** | GPIO5 | 3.3V Logic | 10kΩ pull-up resistor to 3.3V recommended |
| | VCC | **3V3 / 5V** | — | Power | Powered from 3.3V or 5V rail |
| | GND | **GND** | — | Ground | Common system ground |
| **IR Proximity Sensor 1** | OUT | **D2** | GPIO4 | Digital Input | **Active LOW** (GND when blood bag is present) |
| *(Slot S1-01 / IR-01)* | VCC | **3V3 / 5V** | — | Power | Regulated 3.3V or 5V |
| | GND | **GND** | — | Ground | Common system ground |
| **IR Proximity Sensor 2** | OUT | **D5** | GPIO14 | Digital Input | **Active LOW** (GND when blood bag is present) |
| *(Slot S1-02 / IR-02)* | VCC | **3V3 / 5V** | — | Power | Regulated 3.3V or 5V |
| | GND | **GND** | — | Ground | Common system ground |

---

## 📡 Sensor Ingestion Contract

### JSON Telemetry Payload Dispatched Every 30s:

```json
{
  "type": "sensor_reading",
  "temperature_c": 24.5,
  "humidity_pct": 65.0,
  "slots": [
    {
      "sensor_id": "IR-01",
      "occupied": true
    },
    {
      "sensor_id": "IR-02",
      "occupied": false
    }
  ]
}
```

### Request Headers Required:
* `Content-Type: application/json`
* `x-device-code: ESP8266-SENSOR-01`
* `x-device-secret: <SENSOR_DEVICE_SECRET>`

---

## 🛠️ Required Software & Libraries

Install via the **Arduino IDE Library Manager** (`Ctrl + Shift + I`):

1. **esp8266** by ESP8266 Community (via Board Manager >= 3.1.2)
2. **ArduinoJson** by Benoit Blanchon (v7.x or 6.21.x)
3. **DHT sensor library** by Adafruit (v1.4.6 or later)
4. **Adafruit Unified Sensor** by Adafruit (v1.1.14 or later)

---

## 🔐 Credentials Configuration

1. In `firmware/mcu2-sensors/`, create a copy of `secrets.example.h` and rename it to `secrets.h`:
   ```bash
   cp secrets.example.h secrets.h
   ```
2. Open `secrets.h` and input your network and device credentials:
   ```c
   const char* WIFI_SSID     = "Your_WiFi_Network";
   const char* WIFI_PASSWORD = "Your_WiFi_Password";
   const char* ENDPOINT      = "https://<your-project>.supabase.co/functions/v1/device-ingest";
   const char* DEVICE_CODE   = "ESP8266-SENSOR-01";
   const char* DEVICE_SECRET = "Your_Sensor_Device_Secret";
   ```
3. `secrets.h` is excluded from git commits via `.gitignore`.

---

## 🚀 Flashing & Upload Instructions

1. Open `mcu2_sensors.ino` in Arduino IDE.
2. Select Board: **NodeMCU 1.0 (ESP-12E Module)**.
3. Configure:
   * **CPU Frequency**: 80 MHz
   * **Flash Size**: 4MB (FS: 1MB / OTA:~1014KB)
   * **Upload Speed**: 115200 (or 921600)
   * **Port**: Select the corresponding COM port.
4. Click **Upload** (`Ctrl + U`).
5. Open Serial Monitor at **115200 baud** to verify sensor boot-up and loop telemetry.

---

## 🔍 IR Sensor Sensitivity Calibration

Each IR proximity sensor includes an on-board potentiometer:
1. Power up the sensor inside the 3D-printed enclosure.
2. Place a blood bag (or sample container) inside compartment `S1-01`.
3. Adjust the sensitivity screw until the sensor onboard indicator LED turns ON.
4. Remove the bag and confirm that the LED turns OFF immediately.
5. In the Serial Monitor, observe that the status updates from `EMPTY` to `OCCUPIED`.

---

## ❓ Troubleshooting

| Symptom | Cause | Solution |
| :--- | :--- | :--- |
| **"DHT11 returned NaN readings"** | Sensor signal line disconnected or wrong pin | Check signal pin connection to `D1`. Verify sensor receives 3.3V/5V and GND. |
| **Slot status inverted (Empty when occupied)** | Inverted comparator logic on IR board | Most LM393 IR modules output `LOW` on reflection (detection). Verify module output with a digital multimeter. |
| **HTTP Status -1 / Transmission error** | Wi-Fi dropped or SSL connection timeout | Inspect router 2.4GHz signal strength; verify power supply is supplying at least 500mA without voltage drops. |
| **Device shows "No recent heartbeat" on Web UI** | Edge Function not receiving updates | Verify that `x-device-code` matches `ESP8266-SENSOR-01` in the Supabase `devices` table. |
