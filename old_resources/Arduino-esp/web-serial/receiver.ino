void setup() {
  

  // Start serial communication for debugging
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8N1, 16, 17); // Baud rate should match Arduino's Serial1
  // Initialize the built-in LED pin as an output
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.println("NodeMCU ready to receive commands...");
}

void loop() {
  // Check if data is available from Arduino
  if (Serial1.available()) {
    char command = Serial1.read();
    
    // Print the received command to the Serial Monitor
    Serial.print("Received command: ");
    Serial.println(command);

    // Control the LED based on the command
    if (command == '1') {
      digitalWrite(LED_BUILTIN, HIGH); // Turn LED on
      Serial.println("LED ON");
    } else if (command == '0') {
      digitalWrite(LED_BUILTIN, LOW); // Turn LED off
      Serial.println("LED OFF");
    }
  }
}
