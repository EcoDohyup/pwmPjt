#include <WiFi.h>
#include <esp_now.h>
#include <WebServer.h>

const char* ssid = "ecopeace";
const char* password = "ecopeace123";

WebServer server(80);

// Define the MAC address of the second ESP32 (Receiver)
uint8_t receiverMACAddress[] = {0xA0, 0xDD, 0x6C, 0x04, 0x46, 0x18}; // Receiver MAC address

void onDataSent(const uint8_t *receiverMACAddress, esp_now_send_status_t status) {
  Serial.print("Send status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void sendCommandToReceiver(const char* command) {
  Serial.println("sending code activated");
  esp_err_t result = esp_now_send(receiverMACAddress, (const uint8_t*)command, strlen(command));
  if (result != ESP_OK) {
    Serial.print("Send failed, error code: ");
    Serial.println(result);
  } else {
    Serial.println("successful");
  }
}

void handleLEDOn() {
  Serial.println("Received from User : ON");
  sendCommandToReceiver("ON");
 
  // Add CORS headers
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/html", "<html><body><h1>Command Sent: LED ON</h1></body></html>");
}

void handleLEDOff() {
  Serial.println("Received from User : OFF");
  sendCommandToReceiver("OFF");
  
  // Add CORS headers
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/html", "<html><body><h1>Command Sent: LED OFF</h1></body></html>");
}

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to Wi-Fi");

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW initialization failed");
    return;
  } else {
    Serial.println("ESP-NOW initialized");
  }

  // Register callback for sending data
  esp_now_register_send_cb(onDataSent);

  // Add receiver peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, receiverMACAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
  } else {
    Serial.println("Peer added successfully");
  }

  // Handle requests
  server.on("/led/on", HTTP_GET, handleLEDOn);
  server.on("/led/off", HTTP_GET, handleLEDOff);

  server.begin();
}

void loop() {
  server.handleClient(); // Handle incoming client requests
}
