#ifndef SECRETS_H
#define SECRETS_H

// =========================================================================
// BloodLink MCU1 Configuration & Credentials Template
// INSTRUCTIONS:
// 1. Copy this file to "secrets.h" in the same directory.
// 2. Fill in your Wi-Fi credentials and Supabase credentials.
// 3. Keep secrets.h untracked by Git (already specified in .gitignore).
// =========================================================================

// Wi-Fi Network Configuration
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// Supabase Device Ingest Edge Function Endpoint
// Example: "https://<your-project-id>.supabase.co/functions/v1/device-ingest"
const char* ENDPOINT = "https://ycacfjmizaoegykxrtfl.supabase.co/functions/v1/device-ingest";

// Device Authentication
const char* DEVICE_CODE = "ESP8266-RFID-01";
const char* DEVICE_SECRET = "YOUR_DEVICE_SECRET_HERE";

#endif // SECRETS_H
