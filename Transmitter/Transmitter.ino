/*
  ===========================================================
  TRANSMITTER ESP32
  ===========================================================
  - Reads an analog sensor (LDR or potentiometer) on pin 34
  - Sends reading + timestamp + sequence number via ESP-NOW
  - Waits for ACK from receiver
  - If no ACK -> buffers reading to SPIFFS (survives power loss)
  - On next successful send -> flushes buffered readings first

  WIRING (LDR / potentiometer):
  - Potentiometer: middle pin -> GPIO34, outer pins -> 3.3V and GND
  - LDR: one leg -> 3.3V, other leg -> GPIO34 AND -> 10k resistor -> GND
         (voltage divider so GPIO34 reads a varying voltage)

  BEFORE UPLOADING:
  1. Run 0_get_mac_address.ino on the RECEIVER board first.
  2. Copy that MAC address into receiverMAC[] below.
*/

#include <esp_now.h>
#include <WiFi.h>
#include <SPIFFS.h>

// ---------- CONFIG ----------
#define SENSOR_PIN 34
#define SEND_INTERVAL_MS 5000      // how often to read + send
#define ACK_TIMEOUT_MS 1000        // how long to wait for ACK
#define BUFFER_FILE "/buffer.csv"  // SPIFFS file used when offline

// PASTE the receiver's MAC address here (from 0_get_mac_address.ino)
uint8_t receiverMAC[] = {0x24, 0xDC, 0xC3, 0xAE, 0xA4, 0xF8};

// ---------- STRUCT SENT OVER ESP-NOW ----------
typedef struct struct_message {
  uint32_t seq;        // sequence number
  uint32_t timestamp;  // millis() at time of reading
  int sensorValue;     // raw analog reading (0-4095)
} struct_message;

struct_message outgoingReading;

volatile bool ackReceived = false;
uint32_t sequenceCounter = 0;

// ---------- ESP-NOW SEND CALLBACK ----------
void onDataSent(const wifi_tx_info_t *tx_info, esp_now_send_status_t status) {
  ackReceived = (status == ESP_NOW_SEND_SUCCESS);
}

// ---------- SETUP ----------
void setup() {
  Serial.begin(115200);
  delay(500);

  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS mount failed");
  } else {
    Serial.println("SPIFFS mounted OK");
  }

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed, restarting...");
    delay(2000);
    ESP.restart();
  }

  esp_now_register_send_cb(onDataSent);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverMAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add ESP-NOW peer");
  }

  Serial.println("Transmitter ready.");
}

// ---------- SEND ONE READING, RETURN TRUE IF ACKED ----------
bool sendReading(struct_message &msg) {
  ackReceived = false;
  esp_err_t result = esp_now_send(receiverMAC, (uint8_t *)&msg, sizeof(msg));

  if (result != ESP_OK) {
    return false;
  }

  uint32_t start = millis();
  while (millis() - start < ACK_TIMEOUT_MS) {
    if (ackReceived) return true;
    delay(10);
  }
  return false; // timed out
}

// ---------- BUFFER A READING TO SPIFFS ----------
void bufferReading(struct_message &msg) {
  File f = SPIFFS.open(BUFFER_FILE, FILE_APPEND);
  if (!f) {
    Serial.println("Failed to open buffer file for append");
    return;
  }
  f.printf("%lu,%lu,%d\n", msg.seq, msg.timestamp, msg.sensorValue);
  f.close();
  Serial.printf("Buffered reading seq=%lu (offline)\n", msg.seq);
}

// ---------- FLUSH BUFFERED READINGS IF RECEIVER IS BACK ----------
void flushBuffer() {
  if (!SPIFFS.exists(BUFFER_FILE)) return;

  File f = SPIFFS.open(BUFFER_FILE, FILE_READ);
  if (!f) return;

  // Read all lines into memory (fine for small buffer files)
  String remaining = "";
  bool allSent = true;

  while (f.available()) {
    String line = f.readStringUntil('\n');
    if (line.length() == 0) continue;

    uint32_t seq, ts;
    int val;
    sscanf(line.c_str(), "%lu,%lu,%d", &seq, &ts, &val);

    struct_message bufferedMsg = { seq, ts, val };

    if (sendReading(bufferedMsg)) {
      Serial.printf("Flushed buffered seq=%lu\n", seq);
    } else {
      // Stop flushing on first failure, keep the rest queued
      remaining += line + "\n";
      allSent = false;
    }
  }
  f.close();

  // Rewrite buffer file with only the readings that failed to send
  SPIFFS.remove(BUFFER_FILE);
  if (!allSent && remaining.length() > 0) {
    File rewrite = SPIFFS.open(BUFFER_FILE, FILE_WRITE);
    rewrite.print(remaining);
    rewrite.close();
  }
}

// ---------- MAIN LOOP ----------
void loop() {
  // Try to flush any old buffered readings first
  flushBuffer();

  // Take a new live reading
  outgoingReading.seq = sequenceCounter++;
  outgoingReading.timestamp = millis();
  outgoingReading.sensorValue = analogRead(SENSOR_PIN);

  Serial.printf("Reading seq=%lu value=%d -> ",
                outgoingReading.seq, outgoingReading.sensorValue);

  if (sendReading(outgoingReading)) {
    Serial.println("sent + ACKed");
  } else {
    Serial.println("NO ACK -> buffering");
    bufferReading(outgoingReading);
  }

  delay(SEND_INTERVAL_MS);
}
