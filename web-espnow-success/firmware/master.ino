#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

// Wi-Fi network credentials
const char* ssid = "ecopeace";
const char* password = "ecopeace123";

WiFiServer server(80);  // Web server on port 80

uint8_t homeChannel;  // Wi-Fi network channel

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

void setup() {
  Serial.begin(115200);

  // Attempt to connect to Wi-Fi
  WiFi.begin(ssid, password);
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

    homeChannel = WiFi.channel();
    Serial.print("WiFi Channel: ");
    Serial.println(homeChannel);
    server.begin();

    WiFi.mode(WIFI_STA);  // Ensure ESP32 is in station mode
    Serial.println("Master Ready");
    Serial.print("STA MAC: ");
    Serial.println(WiFi.macAddress());

    InitESPNow();

    // Add the peer (slave) once in setup
    esp_now_peer_info_t peerInfo;
    memset(&peerInfo, 0, sizeof(esp_now_peer_info_t));
    memcpy(peerInfo.peer_addr, slave_mac, 6);
    peerInfo.channel = homeChannel;
    peerInfo.ifidx = WIFI_IF_STA;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
      Serial.println("Failed to add peer");
      return;
    }

  } else {
    Serial.println("Failed to connect to Wi-Fi");
    // Optional: Restart or take appropriate action if Wi-Fi connection fails
    ESP.restart();
  }
}

void sendData(const String &data) {
  const char* dataStr = data.c_str();
  size_t dataSize = data.length() + 1;
  esp_err_t result = esp_now_send(slave_mac, (const uint8_t*)dataStr, dataSize);

  if (result == ESP_OK) {
    Serial.println("Send Success");
  } else {
    Serial.println("Send Failed");
  }
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New Client Connected");
    unsigned long connectionTimeout = millis();

    while (client.connected()) {
      if (millis() - connectionTimeout > 5000) {  // 5-second timeout
        Serial.println("Client Timeout");
        break;
      }

      if (client.available()) {
        String inputMessage = client.readStringUntil('\n');
        inputMessage.trim();
        if (inputMessage.length() > 0) {
          Serial.println("Received from client: " + inputMessage);
          sendData(inputMessage);

          // Send a valid HTTP response with CORS headers
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/html");
          client.println("Access-Control-Allow-Origin: *"); // Allow CORS
          client.println("Connection: close");
          client.println();
          client.println("SP32 LED Control");
          client.println("LED " + inputMessage + "");


          break;  // Exit the loop after processing the message
        }
      }
    }

    client.stop();
    Serial.println("Client Disconnected");
  }
}

