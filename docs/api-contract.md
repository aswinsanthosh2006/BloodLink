# Device Ingest API Contract Specification

Technical documentation for the cloud ingestion gateway serving edge microcontroller nodes in the **BloodLink** system.

---

## 🌐 Endpoint Details

* **URL**: `https://<supabase-project-id>.supabase.co/functions/v1/device-ingest`
* **Protocol**: HTTPS (TLS 1.2 / 1.3)
* **Method**: `POST`
* **Authentication**: Custom HTTP Headers

---

## 🔑 Authentication Headers

Every HTTP request dispatched by an edge node must include the following headers:

| Header | Type | Description | Example |
| :--- | :--- | :--- | :--- |
| `Content-Type` | String | MIME type | `application/json` |
| `x-device-code` | String | Unique hardware node identifier | `ESP8266-RFID-01` or `ESP8266-SENSOR-01` |
| `x-device-secret` | String | Pre-shared device secret key | `sec_rfid_9f83a...` |

> **Note**: Upon receiving any authenticated request, the gateway updates `devices.last_seen_at = now()`, keeping the device connectivity heartbeat alive.

---

## 📡 Request & Response Schemas

### 1. RFID Blood Bag Scan (`type: "rfid_scan"`)

Dispatched by **MCU1** when an RFID card or key fob is brought near the RC522 antenna.

#### Request Schema:
```json
{
  "type": "rfid_scan",
  "uid": "93 2A 14 0B"
}
```

| Field | Type | Required | Description |
| :--- | :--- | :---: | :--- |
| `type` | String | Yes | Must be exactly `"rfid_scan"` |
| `uid` | String | Yes | Space-separated uppercase hex bytes of the RFID tag |

---

#### Success Response — Registered Bag (`known: true`):
HTTP Status: `200 OK`
```json
{
  "ok": true,
  "known": true,
  "bag": {
    "bag_code": "BB01",
    "blood_group": "O+",
    "status": "available",
    "entry_at": "2026-10-22T08:30:00.000Z",
    "expiry_date": "2026-11-26"
  },
  "temperature_c": 24.5,
  "temperature_recorded_at": "2026-10-29T11:45:00.000Z"
}
```

#### Success Response — Unregistered Tag (`known: false`):
HTTP Status: `200 OK`
```json
{
  "ok": true,
  "known": false
}
```

---

### 2. Environmental Telemetry & Slot Status (`type: "sensor_reading"`)

Dispatched by **MCU2** every 30 seconds.

#### Request Schema:
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

| Field | Type | Required | Description |
| :--- | :--- | :---: | :--- |
| `type` | String | Yes | Must be exactly `"sensor_reading"` |
| `temperature_c` | Float | Yes | Ambient storage rack temperature in °C |
| `humidity_pct` | Float | Yes | Relative humidity percentage (0.0 to 100.0) |
| `slots` | Array | Yes | Array of rack slot sensor states |
| `slots[].sensor_id` | String | Yes | Hardware identifier of the IR sensor (`IR-01`, `IR-02`) |
| `slots[].occupied` | Boolean | Yes | `true` if bag/object detected; `false` if clear |

---

#### Success Response:
HTTP Status: `200 OK`
```json
{
  "ok": true,
  "message": "Telemetry ingested successfully"
}
```

---

## ⚠️ Error Responses

### Missing Authentication:
HTTP Status: `401 Unauthorized`
```json
{
  "ok": false,
  "error": "Missing x-device-code header"
}
```

### Invalid or Malformed Request:
HTTP Status: `400 Bad Request`
```json
{
  "ok": false,
  "error": "Missing tag UID"
}
```

### Internal Server Exception:
HTTP Status: `500 Internal Server Error`
```json
{
  "ok": false,
  "error": "Database connection timeout"
}
```
