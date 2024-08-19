#include <WiFi.h>
#include <ESPAsyncWebServer.h>

// Replace with your network credentials
const char* ssid = "ecopeace";
const char* password = "ecopeace123";

// Define GPIO pins
#define RELAY_PIN 16     // Relay control pin
#define MOTOR_ENA 14     // Motor speed control (PWM)
#define MOTOR_IN1 27     // Motor direction control 1
#define MOTOR_IN2 26     // Motor direction control 2

// Default motor speed
int motorSpeed = 0;

// Create an instance of the server
AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);

  // Initialize Relay and Motor pins
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  pinMode(MOTOR_ENA, OUTPUT);

  // Set initial state
  digitalWrite(RELAY_PIN, LOW);          // Relay off by default
  analogWrite(MOTOR_ENA, motorSpeed);   // Motor speed set to 0

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to Wi-Fi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Serve the web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String html = "<html><body>";
    html += "<h1>ESP32 Motor Control</h1>";
    html += "<div style='float:left; width:50%;'>";
    html += "<h2>Relay Control</h2>";
    html += "<button onclick=\"location.href='/relay/on'\">Relay On</button><br>";
    html += "<button onclick=\"location.href='/relay/off'\">Relay Off</button><br>";
    html += "</div>";
    html += "<div style='float:right; width:50%;'>";
    html += "<h2>Motor Speed Control</h2>";
    html += "<input type='number' id='speed' name='speed' min='0' max='255' value='0'>";
    html += "<button onclick=\"setSpeed()\">Set Speed</button><br>";
    html += "<p>Current Speed: <span id='currentSpeed'>" + String(motorSpeed) + "</span></p>";
    html += "<script>";
    html += "function setSpeed() {";
    html += "  var speed = document.getElementById('speed').value;";
    html += "  fetch('/setSpeed?value=' + speed).then(response => response.text()).then(data => {";
    html += "    document.getElementById('currentSpeed').innerText = speed;";
    html += "  });";
    html += "}";
    html += "</script>";
    html += "</div>";
    html += "</body></html>";
    request->send(200, "text/html", html);
  });

  // Handle Relay Control
  server.on("/relay/on", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(RELAY_PIN, HIGH); // Turn relay on
    request->send(200, "text/html", "Relay is ON. <a href='/'>Back</a>");
  });

  server.on("/relay/off", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(RELAY_PIN, LOW); // Turn relay off
    request->send(200, "text/html", "Relay is OFF. <a href='/'>Back</a>");
  });

  // Handle Motor Speed Control
  server.on("/setSpeed", HTTP_GET, [](AsyncWebServerRequest *request){
    String speedStr = request->getParam("value")->value();
    motorSpeed = speedStr.toInt();
    if (motorSpeed < 0) motorSpeed = 0;
    if (motorSpeed > 255) motorSpeed = 255;
    analogWrite(MOTOR_ENA, motorSpeed); // Set motor speed
    request->send(200, "text/plain", String(motorSpeed));
  });

  // Start the server
  server.begin();
}

void loop() {
  // No need to do anything here, everything is handled by the server
}
