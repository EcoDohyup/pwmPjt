#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

const char* targetSSID = "ecopeace"; // Master's SSID
uint8_t homeChannel = 0; // Master's channel

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

// Scan for the master's channel
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

      if (SSID == targetSSID) {
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

  // Extract the command from the HTTP request
  if (receivedData.indexOf("/led/on") != -1) {
    digitalWrite(LED_BUILTIN, HIGH);  // Turn LED on
    Serial.println("LED ON");
  } else if (receivedData.indexOf("/led/off") != -1) {
    digitalWrite(LED_BUILTIN, LOW);  // Turn LED off
    Serial.println("LED OFF");
  }
  Serial.println("");
}

void setup() {
  Serial.begin(115200);
  Serial.println("ESPNow/Basic/Slave Example");

  pinMode(LED_BUILTIN, OUTPUT);

  WiFi.mode(WIFI_STA);
  scanForHomeChannel();

  if (homeChannel == 0) {
    Serial.println("Failed to find home channel");
    return;
  }

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
