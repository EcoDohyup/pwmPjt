#include <WiFi.h>

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Initialize WiFi (station mode is default)
  WiFi.mode(WIFI_STA);
  WiFi.begin();

  // Get the MAC Address
  String macAddress = WiFi.macAddress();

  // Print the MAC Address
  Serial.print("ESP32 MAC Address: ");
  Serial.println(macAddress);
}

void loop() {
  // Nothing to do here
}
