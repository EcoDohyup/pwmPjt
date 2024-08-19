#include <WiFi.h>

const char* ssid = "ecopeace";
const char* password = "ecopeace123";

void setup() {
  Serial.begin(115200);
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  
  Serial.print("Connecting to ");
  Serial.print(ssid);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  // Print IP address
  Serial.println("");
  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Get the MAC Address
  String macAddress = WiFi.macAddress();

  // Print the MAC Address
  Serial.print("ESP32 MAC Address: ");
  Serial.println(macAddress);
}

void loop() {
  // Your main code here
}
