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
    }

    // Initialize Async Web Server routes
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send_P(200, "text/html", R"rawliteral(
        <!DOCTYPE HTML><html>
        <head><title>ESP32 Motor Control</title></head>
        <body>
          <h1>ESP32 Motor Control</h1>
          <p><a href="/relay/on"><button>Relay On</button></a></p>
          <p><a href="/relay/off"><button>Relay Off</button></a></p>
          <p>Motor Speed: <span id="speedVal">0</span></p>
          <input type="range" min="0" max="255" value="0" id="speedSlider" oninput="updateSpeed(this.value)">
          <p><button onclick="sendSpeed()">Set Speed</button></p>
          <script>
            function updateSpeed(val) {
              document.getElementById('speedVal').innerText = val;
            }
            function sendSpeed() {
              var speed = document.getElementById('speedSlider').value;
              fetch('/motor/speed/' + speed);
            }
          </script>
        </body>
        </html>
      )rawliteral");
    });

    server.on("/relay/on", HTTP_GET, [](AsyncWebServerRequest *request){
      sendData("/relay/on");  // Send relay on command to the slave
      request->send(200, "text/plain", "Relay On");
    });

    server.on("/relay/off", HTTP_GET, [](AsyncWebServerRequest *request){
      sendData("/relay/off");  // Send relay off command to the slave
      request->send(200, "text/plain", "Relay Off");
    });

    server.on("/motor/speed/:value", HTTP_GET, [](AsyncWebServerRequest *request){
      String speedValue = request->getParam("value")->value();
      sendData("/motor/speed/" + speedValue);  // Send speed command to the slave
      request->send(200, "text/plain", "Speed set to " + speedValue);
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
