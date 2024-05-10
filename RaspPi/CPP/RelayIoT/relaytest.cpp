#include <iostream>
#include <wiringPi.h>

// Define the GPIO pin to which the relay is connected
const int RELAY_PIN = 0; // This corresponds to GPIO pin 17 (wiringPi pin numbering)

int main() {
    // Setup wiringPi library
    if (wiringPiSetup() == -1) {
        std::cerr << "Failed to initialize WiringPi library." << std::endl;
        return 1;
    }

    // Set relay pin as OUTPUT
    pinMode(RELAY_PIN, OUTPUT);

    // Turn on the relay
    digitalWrite(RELAY_PIN, HIGH);
    std::cout << "Relay is ON" << std::endl;

    // Wait for 3 seconds
    delay(3000);

    // Turn off the relay
    digitalWrite(RELAY_PIN, LOW);
    std::cout << "Relay is OFF" << std::endl;

    return 0;
}
