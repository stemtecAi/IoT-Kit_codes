#include <WiFi.h>
#include <PubSubClient.h>

// WiFi settings
const char* ssid = "TCE-IoT";
const char* password = "IoT-1010";

// MQTT broker settings
const char* mqtt_server = "10.42.0.222";
const int mqtt_port = 1883; // MQTT broker port
const char* mqtt_user = "stemtec";
const char* mqtt_password = "stem@123";

const char* relay_topic = "IoTKit/relay/control";
const char* button_topic = "IoTKit/button/state";

// Relay and Button Pin Definitions
const int relayPin = 2;     // GPIO pin connected to relay
const int buttonPin = 4;   // GPIO pin connected to push button

// Push button debounce delay (milliseconds)
const unsigned long debounceDelay = 50;

// WiFi and MQTT client
WiFiClient espClient;
PubSubClient mqttClient(espClient);

// Button state variables
int buttonState = LOW;
int lastButtonState = LOW;
unsigned long lastDebounceTime = 0;

void setup() {
  Serial.begin(115200);

  // Initialize GPIO pins
  pinMode(relayPin, OUTPUT);
  pinMode(buttonPin, INPUT);

  // Connect to Wi-Fi
  connectWiFi();

  // Set MQTT server and callback
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(mqttCallback);

  // Wait for serial communication to be ready
  while (!Serial) {
    delay(10);
  }
}

void loop() {
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }

  // Check button state for debounce
  int reading = digitalRead(buttonPin);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      // Publish button state to MQTT topic
      mqttClient.publish(button_topic, (buttonState == HIGH) ? "1" : "0");

      Serial.print("Button state changed to ");
      Serial.println(buttonState);

      // Toggle relay based on button state
      //nodigitalWrite(relayPin, buttonState);
    }
  }

  lastButtonState = reading;

  mqttClient.loop();
}

void connectWiFi() {
  Serial.println("Connecting to Wi-Fi");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to Wi-Fi");
}

void reconnectMQTT() {
  while (!mqttClient.connected()) {
    Serial.println("Attempting MQTT connection...");

    if (mqttClient.connect("ESP32Client", mqtt_user, mqtt_password)) {
      Serial.println("Connected to MQTT broker");

      // Subscribe to MQTT topics
      mqttClient.subscribe(relay_topic);
    } else {
      Serial.print("Failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on topic: ");
  Serial.println(topic);

  // Handle messages for controlling the relay
  if (strcmp(topic, relay_topic) == 0) {
    if (payload[0] == '1') {
      digitalWrite(relayPin, HIGH);
      Serial.println("Relay turned ON");
    } else if (payload[0] == '0') {
      digitalWrite(relayPin, LOW);
      Serial.println("Relay turned OFF");
    }
  }
}