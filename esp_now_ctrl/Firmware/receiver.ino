// 리시버 코드
#include <WiFi.h>
#include <esp_now.h>

void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
  String message = String((char *)incomingData);
  Serial.print("Received message: ");
  Serial.println(message);

  if (message == "ON") {
    // Code to turn the LED on
    Serial.println("LED ON");
  } else if (message == "OFF") {
    // Code to turn the LED off
    Serial.println("LED OFF");
  }
}

void setup() {
  Serial.begin(115200);

  // Initialize WiFi
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(1000);

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW initialization failed.");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // No need to handle clients, as this is ESP-NOW communication
}
