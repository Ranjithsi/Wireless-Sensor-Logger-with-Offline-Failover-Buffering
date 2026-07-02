/*
  ===========================================================
  RECEIVER ESP32
  ===========================================================
  - Listens for ESP-NOW packets from the transmitter
  - ESP-NOW automatically ACKs at the driver level (no extra code needed -
    the transmitter's esp_now_send callback reports success/failure)
  - On receiving valid data, connects to WiFi and forwards it to ThingSpeak

  BEFORE UPLOADING:
  1. Create a free account at https://thingspeak.com
  2. Create a new Channel, enable at least Field1
  3. Copy your channel's "Write API Key" into THINGSPEAK_API_KEY below
  4. Fill in your WiFi SSID/password below
*/

#include <esp_now.h>
#include <WiFi.h>
#include <HTTPClient.h>

// ---------- CONFIG ----------
const char *WIFI_SSID = "Indumathi4G";
const char *WIFI_PASSWORD = "indusiva@27";
const char *THINGSPEAK_API_KEY = "CKWA6OMWE5TBR4QJ";
const char *THINGSPEAK_URL = "http://api.thingspeak.com/update";

// ---------- STRUCT RECEIVED OVER ESP-NOW (must match transmitter exactly) ----------
typedef struct struct_message {
  uint32_t seq;
  uint32_t timestamp;
  int sensorValue;
} struct_message;

struct_message incomingReading;
volatile bool newDataAvailable = false;

// ---------- ESP-NOW RECEIVE CALLBACK ----------
void onDataRecv(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  if (len == sizeof(incomingReading)) {
    memcpy(&incomingReading, data, sizeof(incomingReading));
    newDataAvailable = true;
  }
}

// ---------- SETUP ----------
void setup() {
  Serial.begin(115200);
  delay(500);

  // Connect to WiFi so we can forward data to ThingSpeak
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to WiFi");
  uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
    delay(300);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected. IP: " + WiFi.localIP().toString());
  } else {
    Serial.println("\nWiFi NOT connected - will keep retrying in loop()");
  }

  // IMPORTANT: ESP-NOW and WiFi STA mode can coexist on the same channel
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed, restarting...");
    delay(2000);
    ESP.restart();
  }

  esp_now_register_recv_cb(onDataRecv);
  Serial.println("Receiver ready. Listening for ESP-NOW packets...");
}

// ---------- FORWARD ONE READING TO THINGSPEAK ----------
void sendToThingSpeak(struct_message &msg) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected, skipping ThingSpeak upload");
    return;
  }

  HTTPClient http;
  String url = String(THINGSPEAK_URL) +
               "?api_key=" + THINGSPEAK_API_KEY +
               "&field1=" + String(msg.sensorValue) +
               "&field2=" + String(msg.seq);

  http.begin(url);
  int httpCode = http.GET();

  if (httpCode > 0) {
    Serial.printf("ThingSpeak upload OK (seq=%lu, value=%d) - HTTP %d\n",
                  msg.seq, msg.sensorValue, httpCode);
  } else {
    Serial.printf("ThingSpeak upload FAILED for seq=%lu - error: %s\n",
                  msg.seq, http.errorToString(httpCode).c_str());
  }
  http.end();
}

// ---------- MAIN LOOP ----------
void loop() {
  // Reconnect WiFi if it drops
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.reconnect();
  }

  if (newDataAvailable) {
    newDataAvailable = false;
    Serial.printf("Received seq=%lu timestamp=%lu value=%d\n",
                  incomingReading.seq, incomingReading.timestamp, incomingReading.sensorValue);

    sendToThingSpeak(incomingReading);
  }

  // ThingSpeak free tier allows updates every 15 sec minimum,
  // so don't hammer it faster than that even if packets arrive quicker.
  delay(50);
}
