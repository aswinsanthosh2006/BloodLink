/**
 * =========================================================================
 * BloodLink — Smart Blood Bank Monitoring and Management System
 * Firmware: MCU2 (Environmental Sensor & Slot Occupancy Node)
 * Controller: ESP8266 NodeMCU (v2 / v3)
 * Device Code: ESP8266-SENSOR-01
 * 
 * Hardware Connections:
 *  - DHT11 Temperature & Humidity Sensor:
 *      VCC  -> 3.3V / 5V (depending on breakout module)
 *      DATA -> D1 (GPIO5)
 *      GND  -> GND
 *  - IR Proximity Sensor 1 (Slot S1-01 Occupancy):
 *      VCC  -> 3.3V / 5V
 *      OUT  -> D2 (GPIO4) [Active LOW on object detection]
 *      GND  -> GND
 *  - IR Proximity Sensor 2 (Slot S1-02 Occupancy):
 *      VCC  -> 3.3V / 5V
 *      OUT  -> D5 (GPIO14) [Active LOW on object detection]
 *      GND  -> GND
 * =========================================================================
 */

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ArduinoJson.h>
#include <DHT.h>

// ---------- CREDENTIALS & SECRETS ----------
// Credentials are decoupled into secrets.h. If secrets.h is not present,
// fallback placeholders are used to guide the developer.
#if __has_include("secrets.h")
  #include "secrets.h"
#else
  #warning "secrets.h not found. Using default placeholder configuration. Copy secrets.example.h to secrets.h."
  const char* WIFI_SSID     = "Enter wifi name";
  const char* WIFI_PASSWORD = "Enter password for wifi";
  const char* ENDPOINT      = "https://ycacfjmizaoegykxrtfl.supabase.co/functions/v1/device-ingest";
  const char* DEVICE_CODE   = "ESP8266-SENSOR-01";
  const char* DEVICE_SECRET = "Enter secret code";
#endif

// ---------- HARDWARE PIN DEFINITIONS ----------
#define DHTPIN  D1
#define DHTTYPE DHT11
#define IR1_PIN D2
#define IR2_PIN D5

DHT dht(DHTPIN, DHTTYPE);

// ---------- TIMING & UPLOAD SCHEDULE ----------
unsigned long lastUpload = 0;
const unsigned long UPLOAD_INTERVAL = 30000; // 30 seconds periodic interval

/**
 * Establish Wi-Fi connection with non-blocking status logging.
 */
void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.printf("\n[WIFI] Connecting to %s...", WIFI_SSID);
  unsigned long started = millis();

  while (WiFi.status() != WL_CONNECTED && (millis() - started < 20000)) {
    delay(300);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("\n[WIFI] Connected successfully. IP: %s\n", WiFi.localIP().toString().c_str());
  } else {
    Serial.println("\n[WIFI] Connection timed out. Will retry in loop.");
  }
}

/**
 * Dispatch sensor telemetry and slot occupancy over HTTPS to the cloud backend.
 */
bool sendSensorData(float temp, float humidity, bool slot1, bool slot2) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[HTTP] Wi-Fi offline. Skipping telemetry upload.");
    return false;
  }

  BearSSL::WiFiClientSecure client;

  // SECURITY NOTE ON TLS VERIFICATION:
  // For academic demonstration, certificate validation is set to insecure
  // due to ESP8266 RAM constraints with deep certificate chains.
  // For production environments, use client.setFingerprint(SHA256_FINGERPRINT)
  // or flash the Supabase root CA bundle.
  client.setInsecure();

  HTTPClient http;
  if (!http.begin(client, ENDPOINT)) {
    Serial.println("[HTTP] HTTPS initialization failed");
    return false;
  }

  http.setTimeout(10000);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("x-device-code", DEVICE_CODE);
  http.addHeader("x-device-secret", DEVICE_SECRET);

  // Construct JSON document
  JsonDocument doc;
  doc["type"]           = "sensor_reading";
  doc["temperature_c"]  = temp;
  doc["humidity_pct"]   = humidity;

  JsonArray slots = doc["slots"].to<JsonArray>();

  JsonObject s1 = slots.add<JsonObject>();
  s1["sensor_id"] = "IR-01";
  s1["occupied"]  = slot1;

  JsonObject s2 = slots.add<JsonObject>();
  s2["sensor_id"] = "IR-02";
  s2["occupied"]  = slot2;

  String payload;
  serializeJson(doc, payload);

  Serial.println("[HTTP] Dispatching telemetry to Supabase...");
  int httpStatus = http.POST(payload);
  Serial.printf("[HTTP] Status Code: %d\n", httpStatus);

  bool success = false;
  if (httpStatus > 0) {
    String responseBody = http.getString();
    Serial.printf("[HTTP] Response Body: %s\n", responseBody.c_str());

    JsonDocument responseDoc;
    DeserializationError jsonErr = deserializeJson(responseDoc, responseBody);
    if (!jsonErr && responseDoc["ok"] == true) {
      success = true;
      Serial.println("[HTTP] Telemetry accepted by backend.");
    }
  } else {
    Serial.printf("[HTTP] Transmission error: %s\n", http.errorToString(httpStatus).c_str());
  }

  http.end();
  return success;
}

// =========================================================================
// ARDUINO SETUP & MAIN LOOP
// =========================================================================

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n[BOOT] Initializing BloodLink MCU2 (Sensor Node)...");

  // IR sensors provide active LOW digital outputs on obstacle detection
  pinMode(IR1_PIN, INPUT);
  pinMode(IR2_PIN, INPUT);

  dht.begin();
  Serial.println("[INIT] Peripherals initialized. Connecting network...");

  connectWiFi();
}

void loop() {
  // Ensure network connectivity
  if (WiFi.status() != WL_CONNECTED) {
    static unsigned long lastReconnectAttempt = 0;
    if (millis() - lastReconnectAttempt > 15000) {
      lastReconnectAttempt = millis();
      Serial.println("[WIFI] Re-attempting connection...");
      connectWiFi();
    }
  }

  // Millis() rollover safe periodic upload timer
  if (millis() - lastUpload < UPLOAD_INTERVAL && lastUpload != 0) {
    delay(50);
    return;
  }

  lastUpload = millis();

  // Read environmental conditions from DHT11
  float temp = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Active-LOW logic: obstacle/bag in front of sensor pulls pin to GND
  bool slot1Occupied = (digitalRead(IR1_PIN) == LOW);
  bool slot2Occupied = (digitalRead(IR2_PIN) == LOW);

  Serial.println("\n--- BloodLink Environmental Telemetry ---");

  // Validate sensor integrity before transmitting
  if (isnan(temp) || isnan(humidity)) {
    Serial.println("[WARN] DHT11 returned NaN readings. Sensor may be disconnected. Retrying...");
    delay(1000);
    temp = dht.readTemperature();
    humidity = dht.readHumidity();

    if (isnan(temp) || isnan(humidity)) {
      Serial.println("[ERROR] Persistent DHT11 read failure; skipping transmission to prevent corrupt data.");
      return;
    }
  }

  Serial.printf("Storage Temperature: %.2f °C\n", temp);
  Serial.printf("Relative Humidity  : %.2f %%\n", humidity);
  Serial.printf("Rack Slot S1-01 (IR-01): %s\n", slot1Occupied ? "OCCUPIED" : "EMPTY");
  Serial.printf("Rack Slot S1-02 (IR-02): %s\n", slot2Occupied ? "OCCUPIED" : "EMPTY");

  // Transmit telemetry
  sendSensorData(temp, humidity, slot1Occupied, slot2Occupied);
}
