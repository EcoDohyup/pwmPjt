#include <WiFi.h>
#include <esp_now.h>


// Callback function to handle received data
void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  // Convert the received data to a string
  String command = String((char*)data);

  // Print received data for debugging
  Serial.print("Received command: ");
  Serial.println(command);

  // Control the LED based on the received command
  if (command == "ON") {
    digitalWrite(LED_BUILTIN, HIGH); // Turn LED ON
  } else if (command == "OFF") {
    digitalWrite(LED_BUILTIN, LOW);  // Turn LED OFF
  } else {
    Serial.println("Unknown command");
  }
}

void setup() {
  Serial.begin(115200);

  // Initialize LED pin
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); // Ensure LED is off initially

  // Set Wi-Fi to station mode and disconnect from any previous connections
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW initialization failed");
    return;
  }

  // Register callback for receiving data
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // Nothing to do here
}
