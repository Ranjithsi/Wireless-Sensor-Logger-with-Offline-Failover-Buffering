/*
  STEP 0: GET RECEIVER'S MAC ADDRESS
  -----------------------------------
  Upload this sketch to the RECEIVER ESP32 FIRST.
  Open Serial Monitor (115200 baud) and copy the MAC address printed.
  You'll paste that MAC address into the TRANSMITTER code later.

  After you've copied the MAC, you can upload the real receiver code
  (2_receiver.ino) to this board.
*/

#include <WiFi.h>
#include <esp_wifi.h>

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();   // forces the radio to actually come up
  delay(100);

  // Read the MAC straight from the driver instead of WiFi.macAddress(),
  // which can return all-zeros if called too early on some cores
  uint8_t mac[6];
  esp_wifi_get_mac(WIFI_IF_STA, mac);

  Serial.println();
  Serial.printf("Receiver MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\n",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  Serial.println("Copy this address into transmitter.ino -> receiverMAC[]");
}

void loop() {
  // Nothing to do here
}
