#include <WiFi.h>
#include <PubSubClient.h>

// Wi-Fi credentials
const char* ssid = "Main Server";
const char* password = "admin@123";

// MQTT broker details
const char* mqtt_server = "5.196.78.28";
const char* topic = "freqtest"; // Topic to publish sensor data

WiFiClient espClient;
PubSubClient client(espClient);

// Soil moisture sensor pins
int sensorPins[] = {32, 33, 34, 35}; // Replace with your GPIO pins
const int sensorCount = sizeof(sensorPins) / sizeof(sensorPins[0]);

// Function to connect to Wi-Fi
void connectToWiFi() {
  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// Function to connect to MQTT broker
void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT broker...");
    if (client.connect("ESP32_Soil_Moisture")) {
      Serial.println("Connected to MQTT broker!");
    } else {
      Serial.print("Failed. Error code: ");
      Serial.println(client.state());
      delay(1000);
    }
  }
}

// Function to convert raw analog values to percentage
int convertToPercentage(int rawValue) {
  return constrain(map(rawValue, 0, 4095, 100, 0), 0, 100);
}

void setup() {
  Serial.begin(115200);

  // Initialize sensor pins
  for (int i = 0; i < sensorCount; i++) {
    pinMode(sensorPins[i], INPUT);
  }

  // Connect to Wi-Fi
  connectToWiFi();

  // Set up MQTT client
  client.setServer(mqtt_server, 1883);
}

void loop() {
  // Reconnect to MQTT broker if disconnected
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();

  // Read and publish soil moisture data
  String payload = "{";
  for (int i = 0; i < sensorCount; i++) {
    int moisture = convertToPercentage(analogRead(sensorPins[i]));
    payload += "\"sensor" + String(i + 1) + "\":" + String(moisture);
    if (i < sensorCount - 1) {
      payload += ",";
    }
  }
  payload += "}";

  // Publish the data
  Serial.println("Publishing payload: " + payload);
  client.publish(topic, payload.c_str());

  // Delay before the next reading
  delay(3000);
}
