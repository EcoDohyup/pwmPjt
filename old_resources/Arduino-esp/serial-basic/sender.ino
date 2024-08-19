void setup() {
    Serial.begin(115200); // Initialize serial communication with the computer
    Serial1.begin(115200); // Initialize serial communication with the ESP32
    // Serial1 is the hardware serial port that corresponds to pins 0 (RX) and 1 (TX)
}

void loop() {
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n'); // Read the command from the serial monitor
        command.trim(); // Remove any extra whitespace
        
        // Send the command to the ESP32
        if (command == "on" || command == "off") {
            Serial1.println(command); // Send the command over Serial1 to ESP32
            Serial.println("Command sent: " + command); // Echo command to the serial monitor
        } else {
            Serial.println("Invalid command. Use 'on' or 'off'.");
        }
    }
}
