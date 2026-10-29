# Setup, Flashing, and Testing Guide

Comprehensive instructions for setting up the development environment, flashing the firmware, deploying the cloud backend, and validating system functionality.

---

## 🛠️ 1. Development Environment Setup

### A. Arduino IDE Setup
1. Download and install **Arduino IDE 2.x** from [arduino.cc](https://www.arduino.cc/en/software).
2. Open **File** -> **Preferences** (`Ctrl + ,`).
3. Add the following URL to **Additional Boards Manager URLs**:
   ```
   http://arduino.esp8266.com/stable/package_esp8266com_index.json
   ```
4. Navigate to **Tools** -> **Board** -> **Boards Manager**, search for `esp8266`, and install version **3.1.2** or higher.
5. Install required libraries via **Sketch** -> **Include Library** -> **Manage Libraries...**:
   * `ArduinoJson` (v7.x or v6.21.x)
   * `MFRC522` by GithubCommunity
   * `Adafruit SSD1306`
   * `Adafruit GFX Library`
   * `DHT sensor library` by Adafruit
   * `Adafruit Unified Sensor`

---

## ☁️ 2. Supabase Backend Setup

### A. Database Initialization
1. Create a free account at [supabase.com](https://supabase.com) and create a new project.
2. In the project dashboard, go to the **SQL Editor**.
3. Copy the contents of [`supabase/migrations/20241001000000_init_schema.sql`](../supabase/migrations/20241001000000_init_schema.sql) and click **Run**.
4. Confirm tables (`devices`, `storage_slots`, `blood_bags`, `readings`, `activity_events`) are populated with initial seeds.

### B. Edge Function Deployment
1. Install Supabase CLI:
   ```bash
   npm install -g supabase
   ```
2. Link project and deploy function:
   ```bash
   npx supabase login
   npx supabase link --project-ref <your-project-id>
   npx supabase functions deploy device-ingest --no-verify-jwt
   ```

---

## ⚡ 3. Firmware Flashing

### MCU1: RFID Scanner & OLED
1. Connect NodeMCU 1 via micro-USB.
2. Copy `firmware/mcu1-rfid-oled/secrets.example.h` to `secrets.h`.
3. Fill in your Wi-Fi credentials and Supabase URL.
4. Select board **NodeMCU 1.0 (ESP-12E Module)** and select the COM port.
5. Click **Upload**.
6. Verify OLED displays `"BloodLink"` and transitions to `"Ready to Scan"`.

### MCU2: Climate & Occupancy Node
1. Connect NodeMCU 2 via micro-USB.
2. Copy `firmware/mcu2-sensors/secrets.example.h` to `secrets.h`.
3. Fill in your Wi-Fi credentials and Supabase URL.
4. Click **Upload**.
5. Open Serial Monitor at **115200 baud** and confirm periodic 30-second telemetry uploads.

---

## 🧪 4. Hardware Validation & Verification Test Cases

| Test Case | Procedure | Expected Result | Verification Status |
| :--- | :--- | :--- | :---: |
| **TC-01: OLED Initialization** | Power on MCU1 | Screen displays startup splash, Wi-Fi IP, and "Ready to Scan" | ✅ Verified |
| **TC-02: Registered Tag Scan** | Tap tag `93 2A 14 0B` against RC522 | OLED displays "BAG IDENTIFIED: BB01", blood group, expiry date, and current temp | ✅ Verified |
| **TC-03: Unknown Tag Scan** | Tap an un-registered 13.56MHz tag | OLED displays "UNKNOWN TAG" and prompts registration in web app | ✅ Verified |
| **TC-04: Scan Cooldown** | Hold card continuously on RC522 | Tag is scanned once; subsequent scan only accepted after 2.5s cooldown | ✅ Verified |
| **TC-05: Telemetry Dispatch** | Run MCU2 for > 30 seconds | Serial prints temperature, humidity, and HTTP 200 backend response | ✅ Verified |
| **TC-06: Slot Occupancy Sensing** | Place object in Slot 1 | IR-01 onboard LED turns ON; payload reports `occupied: true`; web dashboard updates slot | ✅ Verified |
| **TC-07: Inactive Device Detection** | Power down MCU1 or MCU2 | Web dashboard connectivity panel transitions device from active to "No recent heartbeat" | ✅ Verified |
