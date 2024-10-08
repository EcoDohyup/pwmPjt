#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <CytronMotorDriver.h>  // Include the Cytron Motor Driver library

// Motor Driver configuration
#define MOTOR_PWM_PIN 14  // PWM pin for controlling motor speed
#define MOTOR_DIR_PIN 27  // Motor direction pin
#define RELAY_PIN 16      // Define relay pin (if used)

// Create an instance of the motor driver with correct mode
CytronMD motor(PWM_DIR, MOTOR_PWM_PIN, MOTOR_DIR_PIN);

// Variable to store motor speed and state
int motorSpeed = 0;
bool relayState = false;
uint8_t homeChannel = 0;  // Initialize the homeChannel

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
  int16_t scanResults = WiFi.scanNetworks();
  Serial.println("");
  if (scanResults == 0) {
    Serial.println("No WiFi networks found");
  } else {
    Serial.print("Found "); Serial.print(scanResults); Serial.println(" networks");
    for (int i = 0; i < scanResults; ++i) {
      String SSID = WiFi.SSID(i);
      uint8_t channel = WiFi.channel(i);

      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(SSID);
      Serial.print(" (Channel: ");
      Serial.print(channel);
      Serial.println(")");

      if (SSID == "ecopeace") {  // Replace with your SSID
        homeChannel = channel;
        Serial.print("Home Channel found: ");
        Serial.println(homeChannel);
        break;
      }
    }
  }
  WiFi.scanDelete();
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
    digitalWrite(RELAY_PIN, LOW);  // Turn on relay (active-low logic)
    Serial.println("Relay ON");
  } else if (receivedData.indexOf("/relay/off") != -1) {
    relayState = false;
    digitalWrite(RELAY_PIN, HIGH);  // Turn off relay (active-low logic)
    motor.setSpeed(0);  // Stop motor using Cytron library
    Serial.println("Relay OFF");
  } else if (receivedData.startsWith("/motor/speed/")) {
    if (relayState) {
      // Extract speed value from the command
      Serial.println("Motor activated");
      int valueIndex = receivedData.indexOf("/motor/speed/") + String("/motor/speed/").length();
      motorSpeed = receivedData.substring(valueIndex).toInt();

      // Constrain speed value based on the desired range (0-255)
      //motorSpeed = constrain(motorSpeed, 0, 255);

      // Set motor speed using Cytron library
      motor.setSpeed(motorSpeed); // Directly set speed (positive for forward direction)
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

  // Turn off relay and motor initially
  digitalWrite(RELAY_PIN, HIGH);  // Relay off (active-low logic)
  motor.setSpeed(0);  // Stop motor initially

  WiFi.mode(WIFI_STA);
  scanForHomeChannel();  // Scan and set the correct Wi-Fi channel

  if (homeChannel != 0) {
    // Set the home channel
    esp_wifi_set_channel(homeChannel, WIFI_SECOND_CHAN_NONE);
  } else {
    Serial.println("Failed to find home channel");
  }

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
