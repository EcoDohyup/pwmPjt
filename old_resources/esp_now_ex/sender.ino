#include <esp_now.h>
#include <WiFi.h>

// Peer MAC Address (example), 2번이 센더
uint8_t peerMACAddress[] = {0xA0, 0xDD, 0x6C, 0x85, 0x83, 0x90}; // target mac address

// Data to send
struct struct_message {
  int id;
  float temperature;
  float humidity;
} myData;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Add peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, peerMACAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  // Prepare data
  myData.id = 1;
  myData.temperature = 24.5;
  myData.humidity = 60.0;

  // Send data
  esp_now_send(peerMACAddress, (uint8_t *) &myData, sizeof(myData));

  // Print data
  Serial.print("Sent data: ");
  Serial.print(myData.temperature);
  Serial.print(", ");
  Serial.println(myData.humidity);

  delay(2000);
}
