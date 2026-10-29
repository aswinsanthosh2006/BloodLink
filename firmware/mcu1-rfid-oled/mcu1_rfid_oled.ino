/**
 * =========================================================================
 * BloodLink — Smart Blood Bank Monitoring and Management System
 * Firmware: MCU1 (RFID Scanner & Local OLED Display Node)
 * Controller: ESP8266 NodeMCU (v2 / v3)
 * Device Code: ESP8266-RFID-01
 * 
 * Hardware Connections:
 *  - MFRC522 RFID Reader (SPI):
 *      SDA (SS) -> D4 (GPIO2)
 *      SCK      -> D5 (GPIO14)
 *      MOSI     -> D7 (GPIO13)
 *      MISO     -> D6 (GPIO12)
 *      RST      -> D0 (GPIO16)
 *      VCC      -> 3.3V
 *      GND      -> GND
 *  - SSD1306 0.96" OLED Display (I2C):
 *      SDA      -> D2 (GPIO4)
 *      SCL      -> D1 (GPIO5)
 *      VCC      -> 3.3V
 *      GND      -> GND
 *      I2C Addr -> 0x3C
 * =========================================================================
 */

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---------- CREDENTIALS & SECRETS ----------
// Credentials are decoupled into secrets.h. If secrets.h is not present,
// fallback placeholders are used to guide the developer.
#if __has_include("secrets.h")
  #include "secrets.h"
#else
  #warning "secrets.h not found. Using default placeholder configuration. Copy secrets.example.h to secrets.h."
  const char* WIFI_SSID     = "YOUR_WIFI_NAME";
  const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
  const char* ENDPOINT      = "https://ycacfjmizaoegykxrtfl.supabase.co/functions/v1/device-ingest";
  const char* DEVICE_CODE   = "ESP8266-RFID-01";
  const char* DEVICE_SECRET = "YOUR_RFID_DEVICE_SECRET";
#endif

// ---------- STORAGE SPECIFICATIONS ----------
// Clinical demonstration thresholds for refrigerated storage (2.0C - 6.0C).
// Note: In an un-refrigerated test environment, ambient readings will naturally
// exceed this demonstration window.
const float TEMP_MIN_C = 2.0;
const float TEMP_MAX_C = 6.0;

// Age after which a reading is treated as potentially stale (90 seconds)
const unsigned long TEMP_MAX_AGE_MS = 90000;

// ---------- HARDWARE PINOUT & CONSTANTS ----------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_I2C_ADDR 0x3C

#define SS_PIN D4
#define RST_PIN D0

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
MFRC522 rfid(SS_PIN, RST_PIN);

// ---------- SCAN STATE MANAGEMENT ----------
String lastUID = "";
unsigned long lastScanTime = 0;
const unsigned long SCAN_COOLDOWN = 2500; // Minimum time (ms) between identical scans

/**
 * Display formatted text lines on the SSD1306 OLED screen.
 */
void showPage(const String& title,
              const String& line1 = "",
              const String& line2 = "",
              const String& line3 = "",
              const String& line4 = "") {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  // Header Title bar
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("BloodLink - Node 01");
  display.drawLine(0, 11, 127, 11, SSD1306_WHITE);

  display.setCursor(0, 15);
  display.println(title);
  display.setCursor(0, 27);
  display.println(line1);
  display.setCursor(0, 36);
  display.println(line2);
  display.setCursor(0, 45);
  display.println(line3);
  display.setCursor(0, 54);
  display.println(line4);

  display.display();
}

/**
 * Extract YYYY-MM-DD from an ISO-8601 timestamp string.
 */
String shortDate(String value) {
  if (value.length() >= 10) {
    return value.substring(0, 10);
  }
  return "Not recorded";
}

/**
 * Establish Wi-Fi connection with timeout.
 */
bool connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  showPage("Connecting Wi-Fi...", "SSID: " + String(WIFI_SSID));
  unsigned long started = millis();

  while (WiFi.status() != WL_CONNECTED && (millis() - started < 20000)) {
    delay(300);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("\nWi-Fi connected. IP: ");
    Serial.println(WiFi.localIP());
    showPage("Wi-Fi Connected!", "IP: " + WiFi.localIP().toString());
    delay(1000);
    return true;
  }

  Serial.println("\nWi-Fi connection timed out");
  showPage("Wi-Fi Failed", "Check SSID/PW", "Will retry in loop");
  delay(1500);
  return false;
}

/**
 * Present scanned blood bag information across sequenced OLED screens.
 */
void showBagDetails(JsonDocument& result) {
  JsonObject bag = result["bag"];

  String bagCode = bag["bag_code"] | "Unknown";
  String group   = bag["blood_group"] | "Unknown";
  String status  = bag["status"] | "Unknown";
  String entry   = shortDate(bag["entry_at"] | "");
  String expiry  = shortDate(bag["expiry_date"] | "");

  // Page 1: Identity & Blood Group
  showPage("BAG IDENTIFIED",
           "Code: " + bagCode,
           "Group: " + group,
           "Status: " + status,
           "UID: " + lastUID);
  delay(2500);

  // Page 2: Dates (Traceability & Expiry)
  showPage("VALIDITY DATES",
           "Entry:  " + entry,
           "Expiry: " + expiry,
           "Storage: RACK-1",
           "");
  delay(2500);

  // Page 3: Environmental Rack Temperature
  JsonVariant tempValue = result["temperature_c"];
  JsonVariant timeValue = result["temperature_recorded_at"];

  if (tempValue.isNull()) {
    showPage("TEMPERATURE",
             "No reading available",
             "Waiting for sensor",
             "",
             "");
    delay(2000);
    return;
  }

  float temp = tempValue.as<float>();
  String tempText = String(temp, 1) + " C";
  bool inRange = (temp >= TEMP_MIN_C && temp <= TEMP_MAX_C);

  showPage("STORAGE TEMPERATURE",
           "Temp: " + tempText,
           "Ideal: 2.0 - 6.0 C",
           String("Status: ") + (inRange ? "OPTIMAL" : "OUT OF RANGE"),
           "Ambient Test Note");
  delay(2500);
}

/**
 * Dispatch an RFID scan event over HTTPS to the Supabase device-ingest endpoint.
 */
void sendRFIDScan(const String& uid) {
  if (WiFi.status() != WL_CONNECTED) {
    showPage("Offline", "Cannot verify tag", "UID: " + uid);
    return;
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
    Serial.println("[HTTP] Unable to initialize client connection");
    showPage("HTTPS Error", "Endpoint init fail");
    return;
  }

  http.setTimeout(12000);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("x-device-code", DEVICE_CODE);
  http.addHeader("x-device-secret", DEVICE_SECRET);

  // Construct request JSON payload
  JsonDocument request;
  request["type"] = "rfid_scan";
  request["uid"]  = uid;

  String payload;
  serializeJson(request, payload);

  showPage("Verifying Tag...", "UID: " + uid, "Querying cloud...");

  int httpCode = http.POST(payload);
  Serial.printf("[HTTP] POST status: %d\n", httpCode);

  if (httpCode <= 0) {
    Serial.printf("[HTTP] Error: %s\n", http.errorToString(httpCode).c_str());
    showPage("Upload Failed", "Code: " + String(httpCode), "Check Wi-Fi / API");
    http.end();
    return;
  }

  String response = http.getString();
  Serial.println("[HTTP] Response: " + response);
  http.end();

  // Parse JSON response
  JsonDocument result;
  DeserializationError err = deserializeJson(result, response);

  if (err) {
    Serial.printf("[JSON] Deserialization failed: %s\n", err.c_str());
    showPage("JSON Parse Error", "Malformed response");
    return;
  }

  if (result["ok"] == true) {
    bool known = result["known"] | false;

    if (!known) {
      showPage("UNKNOWN TAG", "UID: " + uid, "Unregistered bag", "Register in web app");
      return;
    }

    showBagDetails(result);
    showPage("Ready to Scan", "Present RFID tag");
    return;
  }

  String errorMsg = result["error"] | "Unknown server error";
  showPage("Request Denied", errorMsg);
}

// =========================================================================
// ARDUINO SETUP & MAIN LOOP
// =========================================================================

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n[BOOT] Initializing BloodLink MCU1...");

  // Initialize I2C for SSD1306 (SDA=D2, SCL=D1)
  Wire.begin(D2, D1);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDR)) {
    Serial.println("[ERROR] SSD1306 OLED initialization failed at 0x3C");
    while (true) delay(1000);
  }

  display.display();
  delay(500);

  // Initialize SPI bus and MFRC522 reader
  SPI.begin();
  rfid.PCD_Init();

  showPage("Booting System...", "Init: OLED OK", "Init: RFID OK");
  delay(1000);

  connectWiFi();
  showPage("Ready to Scan", "Present RFID tag");
  Serial.println("[READY] BloodLink RFID node ready for operation");
}

void loop() {
  // Guard Wi-Fi connectivity
  if (WiFi.status() != WL_CONNECTED) {
    static unsigned long lastReconnectAttempt = 0;
    if (millis() - lastReconnectAttempt > 15000) {
      lastReconnectAttempt = millis();
      Serial.println("[WIFI] Reconnecting...");
      WiFi.reconnect();
    }
  }

  // Check if a new RFID card/tag is present in the RF field
  if (!rfid.PICC_IsNewCardPresent()) {
    delay(20);
    return;
  }

  // Read card serial number
  if (!rfid.PICC_ReadCardSerial()) {
    delay(20);
    return;
  }

  // Format UID as uppercase space-separated hex bytes (e.g. "93 2A 14 0B")
  char uidBuffer[40] = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    char byteStr[4];
    snprintf(byteStr, sizeof(byteStr), "%02X", rfid.uid.uidByte[i]);
    if (i > 0) strcat(uidBuffer, " ");
    strcat(uidBuffer, byteStr);
  }

  String uid = String(uidBuffer);
  unsigned long now = millis();

  // Handle scan cooldown to prevent rapid repetitive triggers
  if (uid != lastUID || (now - lastScanTime >= SCAN_COOLDOWN)) {
    lastUID = uid;
    lastScanTime = now;

    Serial.printf("[RFID] Scanned UID: %s\n", uid.c_str());
    sendRFIDScan(uid);
  }

  // Instruct PICC to enter halt state
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  delay(100);
}
