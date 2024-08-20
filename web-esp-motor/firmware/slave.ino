#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

// Pin definitions for motor and relay
#define RELAY_PIN 16
#define ENA_PIN 14
#define IN1_PIN 27
#define IN2_PIN 26

// Variable to store motor speed
int motorSpeed = 0;
bool relayState = false;

// Initialize ESP-NOW
void InitESPNow() {
  WiFi.disconnect();
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Success");
  } else {
    Serial.println("ESPNow Init Failed");
    ESP.restart();
  }
}

// Scan for the master's channel (if needed)
void scanForHomeChannel() {
  // This function is retained in case you need to scan for the master’s Wi-Fi channel.
}

// Callback function when data is received from the master
void OnDataRecv(const esp_now_recv_info *recv_info, const uint8_t *data, int data_len) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           recv_info->src_addr[0], recv_info->src_addr[1], recv_info->src_addr[2],
           recv_info->src_addr[3], recv_info->src_addr[4], recv_info->src_addr[5]);
  Serial.print("Last Packet Recv from: ");
  Serial.println(macStr);

  Serial.print("Data length: ");
  Serial.println(data_len);

  // Convert data to string and check the command
  String receivedData = "";
  for (int i = 0; i < data_len; i++) {
    receivedData += (char)data[i];
  }
  Serial.print("Last Packet Recv Data: ");
  Serial.println(receivedData);

  // Process commands
  if (receivedData.indexOf("/relay/on") != -1) {
    relayState = true;
    digitalWrite(RELAY_PIN, HIGH);  // Turn on relay
    Serial.println("Relay ON");
  } else if (receivedData.indexOf("/relay/off") != -1) {
    relayState = false;
    digitalWrite(RELAY_PIN, LOW);  // Turn off relay
    analogWrite(ENA_PIN, 0);  // Stop motor
    Serial.println("Relay OFF");
  } else if (receivedData.startsWith("/motor/speed/")) {
    if (relayState) {
      // Extract speed value from the command
      int speedIndex = receivedData.indexOf("/motor/speed/") + String("/motor/speed/").length();
      motorSpeed = receivedData.substring(speedIndex).toInt();
      analogWrite(ENA_PIN, motorSpeed);  // Set motor speed
      Serial.println("Motor Speed set to " + String(motorSpeed));
    } else {
      Serial.println("Relay is OFF, can't set motor speed");
    }
  }

  Serial.println("");
}

void setup() {
  Serial.begin(115200);
  Serial.println("ESPNow/Motor/Slave Example");

  // Set GPIO pins as outputs
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(ENA_PIN, OUTPUT);
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);

  // Turn off relay and motor initially
  digitalWrite(RELAY_PIN, LOW);
  analogWrite(ENA_PIN, 0);
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, LOW);

  WiFi.mode(WIFI_STA);
  // If needed, call scanForHomeChannel() here to set the correct channel.

  // Set the home channel
  esp_wifi_set_channel(homeChannel, WIFI_SECOND_CHAN_NONE);

  // Print the MAC address of the slave
  Serial.print("STA MAC: ");
  Serial.println(WiFi.macAddress());

  // Initialize ESP-NOW
  InitESPNow();

  // Register the receive callback function
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // No specific actions needed in loop
}
