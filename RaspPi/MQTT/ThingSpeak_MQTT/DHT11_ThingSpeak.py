import paho.mqtt.client as mqtt
import requests

# ThingSpeak parameters
CHANNEL_ID = '2549786'
WRITE_API_KEY = '5K9MU07XDIJGZS09'
THINGSPEAK_URL = f'https://api.thingspeak.com/update.json'

# MQTT parameters
MQTT_BROKER = '10.42.0.222'
MQTT_PORT = 1883
MQTT_TEMPERATURE_TOPIC = 'IoTKit/temperature'  # MQTT topic for temperature data
MQTT_HUMIDITY_TOPIC = 'IoTKit/humidity'        # MQTT topic for humidity data

MQTT_USERNAME = 'stemtec'
MQTT_PASSWORD = 'stem@123'

# Callback function for MQTT message reception
def on_message(client, userdata, message):
    payload = message.payload.decode('utf-8')
    print(f'Received MQTT message on topic {message.topic}: {payload}')

    # Determine field based on MQTT topic
    field = None
    if message.topic == MQTT_TEMPERATURE_TOPIC:
        field = 'field1'
    elif message.topic == MQTT_HUMIDITY_TOPIC:
        field = 'field2'

    if field is not None:
        # Send data to ThingSpeak
        data = {'api_key': WRITE_API_KEY, field: payload}
        response = requests.post(THINGSPEAK_URL, data=data)

        if response.status_code == 200:
            print('Data sent to ThingSpeak successfully')
        else:
            print(f'Failed to send data to ThingSpeak, HTTP status code: {response.status_code}')

# Set up MQTT client with authentication
client = mqtt.Client()
client.username_pw_set(MQTT_USERNAME, MQTT_PASSWORD)
client.on_message = on_message

# Connect to MQTT broker and subscribe to topics
client.connect(MQTT_BROKER, MQTT_PORT, keepalive=60)
client.subscribe([(MQTT_TEMPERATURE_TOPIC, 0), (MQTT_HUMIDITY_TOPIC, 0)])

# Start MQTT loop to continuously listen for messages
client.loop_forever()