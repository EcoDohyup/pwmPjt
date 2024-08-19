#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "ecopeace";
const char* password = "ecopeace123";

WebServer server(80);

void handleLEDOn() {
  digitalWrite(LED_BUILTIN, HIGH); // Turn the LED on
  server.send(200, "text/html", "<html><body><h1>LED is ON</h1></body></html>");
}

void handleLEDOff() {
  digitalWrite(LED_BUILTIN, LOW); // Turn the LED off
  server.send(200, "text/html", "<html><body><h1>LED is OFF</h1></body></html>");
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); // Ensure LED is off initially

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to Wi-Fi");

  // Handle requests
  server.on("/led/on", HTTP_GET, handleLEDOn);
  server.on("/led/off", HTTP_GET, handleLEDOff);

  server.begin();
}

void loop() {
  server.handleClient(); // Handle incoming client requests
}
