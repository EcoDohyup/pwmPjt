// 중계기 코드
#include <WiFi.h>
#include <esp_now.h>

const char* ssid = "ecopeace";
const char* password = "ecopeace123";

// Define the MAC address of the second ESP32 (Receiver)
uint8_t receiverMAC[] = {0xA0, 0xDD, 0x6C, 0x04, 0x46, 0x18}; // 리시버 맥주소

WebServer server(80);

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Last Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
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

  esp_now_register_send_cb(OnDataSent);

  // Add the receiver peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, receiverMAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer.");
    return;
  }

  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", 
      "<html><body>"
      "<h1>LED Control</h1>"
      "<button onclick=\"location.href='/led/on'\">LED ON</button>"
      "<button onclick=\"location.href='/led/off'\">LED OFF</button>"
      "</body></html>");
  });

  server.on("/led/on", HTTP_GET, []() {
    sendDataToReceiver("ON");
    server.send(200, "text/html", "<html><body><h1>LED Command Sent: ON</h1></body></html>");
  });

  server.on("/led/off", HTTP_GET, []() {
    sendDataToReceiver("OFF");
    server.send(200, "text/html", "<html><body><h1>LED Command Sent: OFF</h1></body></html>");
  });

  server.begin();
}

void sendDataToReceiver(const String &message) {
  esp_err_t result = esp_now_send(receiverMAC, (uint8_t *)message.c_str(), message.length());
  if (result == ESP_OK) {
    Serial.println("Send Success");
  } else {
    Serial.println("Send Error");
  }
}

void loop() {
  server.handleClient();
}
