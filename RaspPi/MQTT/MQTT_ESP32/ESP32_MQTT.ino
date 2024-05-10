#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// WiFi settings
const char* ssid = "TCE-IoT";
const char* password = "IoT-1010";

// MQTT broker settings
const char* mqtt_server = "10.42.0.222";
const int mqtt_port = 1883; // MQTT broker port
const char* mqtt_user = "stemtec";
const char* mqtt_password = "stem@123";

// MQTT topics
const char* temperature_topic = "IoTKit/temperature";
const char* humidity_topic = "IoTKit/humidity";

// DHT sensor settings
#define DHTPIN 4       // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11  // DHT sensor type (DHT11 or DHT22)
DHT dht(DHTPIN, DHTTYPE);

// WiFi and MQTT client
WiFiClient espClient;
PubSubClient mqttClient(espClient);

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  connectWiFi();

  // Set MQTT server and callback
  mqttClient.setServer(mqtt_server, mqtt_port);
  
  // Initialize DHT sensor
  dht.begin();
}

void loop() {
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }

  // Read DHT sensor data
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Publish temperature and humidity to MQTT
  publishSensorData(temperature, temperature_topic);
  publishSensorData(humidity, humidity_topic);

  delay(5000); // Delay between sensor readings
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
    } else {
      Serial.print("Failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void publishSensorData(float data, const char* topic) {
  char msg[10];
  dtostrf(data, 4, 2, msg); // Convert float to string with 2 decimal places

  if (mqttClient.publish(topic, msg)) {
    Serial.print("Published to ");
    Serial.print(topic);
    Serial.print(": ");
    Serial.println(msg);
  } else {
    Serial.println("Failed to publish data");
  }
}