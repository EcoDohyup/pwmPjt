#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <esp_now.h>
#include <esp_wifi.h>

// Wi-Fi network credentials
const char* ssid = "ecopeace";
const char* password = "ecopeace123";

// Async web server on port 80
AsyncWebServer server(80);

// Slave's MAC address
uint8_t slave_mac[] = {0xA0, 0xDD, 0x6C, 0x10, 0x4B, 0xCC};  // Replace with your slave's MAC address

void InitESPNow() {
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Success");
  } else {
    Serial.println("ESPNow Init Failed");
    ESP.restart();
  }
}

// Function to send data to the slave
void sendData(const String &data) {
  const char* dataStr = data.c_str();
  size_t dataSize = data.length() + 1;
  esp_err_t result = esp_now_send(slave_mac, (const uint8_t*)dataStr, dataSize);
  Serial.print("Sending data : ");
  Serial.println(dataStr);

  if (result == ESP_OK) {
    Serial.println("Send Success");
  } else {
    Serial.println("Send Failed");
  }
}

void setup() {
  Serial.begin(115200);

  // Attempt to connect to Wi-Fi
  WiFi.begin(ssid, password);
  WiFi.mode(WIFI_STA);
  int attempt = 0;
  while (WiFi.status() != WL_CONNECTED && attempt < 20) {  // Try for 10 seconds
    delay(500);
    Serial.print(".");
    attempt++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected to Wi-Fi");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    // Initialize ESP-NOW
    InitESPNow();

    // Add the peer (slave) once in setup
    esp_now_peer_info_t peerInfo;
    memset(&peerInfo, 0, sizeof(esp_now_peer_info_t));
    memcpy(peerInfo.peer_addr, slave_mac, 6);
    peerInfo.channel = WiFi.channel();
    peerInfo.ifidx = WIFI_IF_STA;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
      Serial.println("Failed to add peer");
      return;
    } else {
      Serial.println("Peer added successfully");
    }

    // Handle CORS for all endpoints
    auto handleCORS = [](AsyncWebServerRequest *request) {
      AsyncWebServerResponse *response = request->beginResponse(200);
      response->addHeader("Access-Control-Allow-Origin", "*");
      response->addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
      response->addHeader("Access-Control-Allow-Headers", "Content-Type");
      request->send(response);
    };

    // Handle preflight CORS requests (OPTIONS method)
    server.on("/relay/on", HTTP_OPTIONS, handleCORS);
    server.on("/relay/off", HTTP_OPTIONS, handleCORS);
    server.on("/motor/speed/:value", HTTP_OPTIONS, handleCORS);

    // Handle relay on
    server.on("/relay/on", HTTP_GET, [](AsyncWebServerRequest *request){
      sendData("/relay/on");  // Send relay on command to the slave
      AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Relay On");
      response->addHeader("Access-Control-Allow-Origin", "*");
      request->send(response);
    });

    // Handle relay off
    server.on("/relay/off", HTTP_GET, [](AsyncWebServerRequest *request){
      sendData("/relay/off");  // Send relay off command to the slave
      AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Relay Off");
      response->addHeader("Access-Control-Allow-Origin", "*");
      request->send(response);
    });

    // Handle motor speed control
    server.on("/motor/speed", HTTP_GET, [](AsyncWebServerRequest *request){
      if (request->hasParam("value")) {
        String speedValue = request->getParam("value")->value();
        sendData("/motor/speed/" + speedValue);  // Send speed command to the slave
        AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Speed set to " + speedValue);
        response->addHeader("Access-Control-Allow-Origin", "*");
        request->send(response);
      } else {
        AsyncWebServerResponse *response = request->beginResponse(400, "text/plain", "Invalid request");
        response->addHeader("Access-Control-Allow-Origin", "*");
        request->send(response);
      }
    });


    server.begin();
  } else {
    Serial.println("Failed to connect to Wi-Fi");
    ESP.restart();
  }
}

void loop() {
  // No specific actions needed in loop
}
