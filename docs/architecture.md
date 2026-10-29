# System Architecture & Data Flow

This document details the multi-tier engineering architecture, hardware-software integration, and separate data pipelines operating in **BloodLink**.

---

## 🏛️ Six-Layer Logical Architecture

```mermaid
graph TD
    subgraph Layer1["1. Sensing & Peripheral Layer"]
        RFID["RC522 RFID Reader<br/>(13.56 MHz SPI)"]
        OLED["SSD1306 OLED<br/>(128x64 I2C)"]
        DHT["DHT11 Climate Sensor<br/>(Temp & Humidity)"]
        IR["Dual IR Sensors<br/>(Slots S1-01, S1-02)"]
    end

    subgraph Layer2["2. Embedded Processing Layer"]
        MCU1["Node 1: ESP8266 NodeMCU<br/>(ESP8266-RFID-01)"]
        MCU2["Node 2: ESP8266 NodeMCU<br/>(ESP8266-SENSOR-01)"]
    end

    subgraph Layer3["3. Communication Layer"]
        WiFi["Wi-Fi 802.11 b/g/n (2.4 GHz)"]
        HTTPS["BearSSL HTTPS / REST (JSON)"]
    end

    subgraph Layer4["4. Backend Gateway"]
        EF["Supabase Edge Function<br/>(/functions/v1/device-ingest)"]
    end

    subgraph Layer5["5. Cloud Database Layer"]
        DB[("PostgreSQL 15+ (Supabase)<br/>tables: blood_bags, devices,<br/>storage_slots, readings, activity_events")]
    end

    subgraph Layer6["6. Application & Dashboard"]
        WEB["React Web Dashboard<br/>(Deployed on Vercel)"]
    end

    RFID -->|SPI| MCU1
    MCU1 -->|I2C| OLED
    DHT -->|Digital| MCU2
    IR -->|GPIO| MCU2

    MCU1 -->|WLAN| WiFi
    MCU2 -->|WLAN| WiFi
    WiFi -->|TLS/TCP| HTTPS
    HTTPS --> EF

    EF -->|Service Role DB Access| DB
    DB -->|PostgREST / Anon RLS| WEB
```

---

## 🔄 Separate Data Flow Pipelines

### 1. RFID Blood Bag Scan Pipeline (Interactive / Event-Driven)

```mermaid
sequenceDiagram
    autonumber
    actor Tech as Blood Bank Operator
    participant Tag as RFID Tag (on Blood Bag)
    participant MCU1 as ESP8266 Node 1
    participant OLED as Local OLED Screen
    participant EF as Supabase Edge Function
    participant DB as PostgreSQL Database

    Tech->>MCU1: Presents blood bag to RC522 antenna
    MCU1->>Tag: Reads 4-byte UID over SPI (e.g. 93 2A 14 0B)
    MCU1->>OLED: Displays "Verifying Tag..."
    MCU1->>EF: HTTPS POST /device-ingest { type: "rfid_scan", uid }
    Note over MCU1,EF: Authenticates with x-device-code & secret

    EF->>DB: UPDATE devices SET last_seen_at = now()
    EF->>DB: SELECT * FROM blood_bags WHERE rfid_uid = uid
    alt Tag is Registered
        DB-->>EF: Returns bag record (BB01, O+, Available, Expiry)
        EF->>DB: SELECT temperature_c FROM readings ORDER BY recorded_at DESC LIMIT 1
        DB-->>EF: Returns latest storage temperature (e.g. 31.8°C)
        EF->>DB: INSERT INTO activity_events (rfid_scan event)
        EF-->>MCU1: HTTP 200 { ok: true, known: true, bag: {...}, temperature_c }
        MCU1->>OLED: Page 1: Bag Code & Blood Group (2.5s)
        MCU1->>OLED: Page 2: Entry Date & Expiry Date (2.5s)
        MCU1->>OLED: Page 3: Latest Storage Temperature (2.5s)
        MCU1->>OLED: Returns to "Ready to Scan"
    else Tag is Unknown
        EF-->>MCU1: HTTP 200 { ok: true, known: false }
        MCU1->>OLED: Displays "UNKNOWN TAG - Register in Web App"
    end
```

---

### 2. Environmental Telemetry & Slot Occupancy Pipeline (Periodic)

```mermaid
sequenceDiagram
    autonumber
    participant MCU2 as ESP8266 Node 2
    participant DHT as DHT11 Sensor
    participant IR as Optical IR Sensors
    participant EF as Supabase Edge Function
    participant DB as PostgreSQL Database
    participant UI as Vercel Web Dashboard

    loop Every 30 Seconds
        MCU2->>DHT: Reads Temperature & Humidity
        MCU2->>IR: Reads Digital Output (LOW = Occupied)
        MCU2->>EF: HTTPS POST /device-ingest { type: "sensor_reading", temp, humidity, slots }
        Note over MCU2,EF: Authenticates with x-device-code & secret

        EF->>DB: UPDATE devices SET last_seen_at = now()
        EF->>DB: INSERT INTO readings (temperature_c, humidity_pct)
        EF->>DB: UPDATE storage_slots SET is_occupied = occupied
        EF-->>MCU2: HTTP 200 { ok: true, message: "Telemetry ingested" }
    end

    UI->>DB: Polls / Subscribes to table updates
    DB-->>UI: Refreshes Metric Cards, Slot Occupancy Grid & Heartbeats
```

---

## 🛡️ Security Boundaries & Trust Architecture

1. **Edge Node Authentication**: Nodes are isolated from direct database write privileges. Every ingestion request must pass through the `device-ingest` Edge Function with matching `x-device-code` and `x-device-secret`.
2. **Dashboard Permissions**: The web client uses read-only PostgREST queries backed by PostgreSQL **Row Level Security (RLS)**. No administrative credentials or write keys are exposed to the browser.
3. **Hardware Independence**: The two microcontrollers communicate purely through the cloud backend. Failure or power loss on one node does not interrupt the operations of the other.
