#include <WiFi.h>

// Replace these with your network credentials
const char* ssid = "kyobobooks_2G";
const char* password = "Qoeh0609tk!@";

// Create an instance of the WiFiServer class
WiFiServer server(80);

// Initialize Serial1 for communication with NodeMCU
// Use Serial1 (or Serial2 if available) on boards that support it
#define SERIAL1_BAUD_RATE 9600

void setup() {
  // Start the serial communication for debugging
  Serial.begin(115200);

  // Initialize Serial1 for communication with NodeMCU
  Serial1.begin(SERIAL1_BAUD_RATE);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Print the IP address
  Serial.println("Connected to Wi-Fi");

  // Start the server
  server.begin();
}

void loop() {
  // Listen for incoming clients
  WiFiClient client = server.available();

  if (client) {
    Serial.println("New client connected");

    // Wait for the client to send data
    while (client.connected() && !client.available()) {
      delay(1);
    }

    // Read the incoming data
    String request = "";
    while (client.available()) {
      char c = client.read();
      request += c;
    }

    // Print the received request to the Serial Monitor
    Serial.print("Received request: ");
    Serial.println(request);

    // Handle the request and send commands to NodeMCU via Serial1
    if (request.indexOf("/led/on") != -1) {
      Serial.println("LED ON command sent to NodeMCU");
      Serial1.write('1'); // Command to turn LED on
    } else if (request.indexOf("/led/off") != -1) {
      Serial.println("LED OFF command sent to NodeMCU");
      Serial1.write('0'); // Command to turn LED off
    }

    // Send a response to the client
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println("Connection: close");
    client.println();

    client.println("<!DOCTYPE HTML>");
    client.println("<html>");
    client.println("<body>");
    client.println("<h1>Arduino Server</h1>");
    client.println("<p>Request received</p>");
    client.println("</body>");
    client.println("</html>");

    // Close the connection
    client.stop();
    Serial.println("Client disconnected");
  }
}
