#include <iostream>
#include <wiringPi.h>
#include <mqtt/async_client.h>


//g++ -o relay_control relay_control.cpp -lwiringPi -lpaho-mqttpp3 -lpaho-mqtt3as




const int RELAY_PIN = 0; // GPIO pin for relay

int main() {
    // Initialize wiringPi
    wiringPiSetup();

    // Initialize MQTT client
    mqtt::async_client client("tcp://broker_address:1883", "client_id");

    // Set up GPIO pin
    pinMode(RELAY_PIN, OUTPUT);

    // Connect to MQTT broker
    client.connect().wait();

    // Subscribe to control topic
    client.subscribe("relay/control", 1).wait();

    // Handle incoming MQTT messages
    client.set_callback([](mqtt::const_message_ptr msg) {
        std::string payload = msg->to_string();
        if (payload == "on") {
            digitalWrite(RELAY_PIN, HIGH);
        } else if (payload == "off") {
            digitalWrite(RELAY_PIN, LOW);
        }
    });

    // Main loop
    while (true) {
        // Publish relay status periodically
        client.publish("relay/status", "on/off", 1, false).wait();
        delay(1000); // Delay for 1 second
    }

    return 0;
}
