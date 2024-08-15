void setup() {
    Serial.begin(115200); // Initialize serial communication for debugging
    Serial1.begin(115200, SERIAL_8N1, 16, 17); // Use Serial1 with RX on D16 and TX on D17
    pinMode(LED_BUILTIN, OUTPUT); // Set the built-in LED pin as output
}

void loop() {
    if (Serial1.available()) {
        String command = Serial1.readStringUntil('\n'); // Read command from Arduino
        command.trim(); // Remove any extra whitespace
        Serial.print("Received command: ");
        Serial.println(command);

        if (command == "on") {
            digitalWrite(LED_BUILTIN, HIGH); // Turn LED ON
            Serial.println("Result: LED is ON"); // Display result
        } else if (command == "off") {
            digitalWrite(LED_BUILTIN, LOW);  // Turn LED OFF
            Serial.println("Result: LED is OFF"); // Display result
        } else {
            Serial.println("Result: Unknown command"); // Display result for unknown commands
        }
    }
}