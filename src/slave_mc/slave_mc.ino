#include <WiFi.h>
#include <PubSubClient.h>

#define SIR_UID "realme"
#define SIR_PWD "ilakkiya"

// Wi-Fi credentials
const char* ssid = "Main_Server";
const char* password = "admin@123";

// MQTT broker details
const char* mqtt_server = "5.196.78.28";

WiFiClient espClient;
PubSubClient client(espClient);

// Pin assignments
int valvePin = 13;
int sensorPins[] = {32, 33, 34, 35}; // Moisture sensor pins
const int sensorCount = sizeof(sensorPins) / sizeof(sensorPins[0]);

// MQTT topics
const char* moistureTopic = "sector_2";
const char* sprinklerControlTopic = "sector_2_sprinkler";

// Convert raw sensor values to percentage
int convertToPercentage(int rawValue) {
  return constrain(map(rawValue, 0, 4095, 100, 0), 0, 100);
}

// MQTT callback function
void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  if (String(topic) == sprinklerControlTopic) {
    if (message == "sprinkler1_on") {
      digitalWrite(valvePin, HIGH);
    } else if (message == "sprinkler1_off") {
      digitalWrite(valvePin, LOW);
    }
  }
}

// Reconnect to MQTT broker
void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32_Sector2")) {
      client.subscribe(sprinklerControlTopic);
    } else {
      delay(1000);
    }
  }
}

// Connect to Wi-Fi
void connectToWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(valvePin, OUTPUT);
  digitalWrite(valvePin, LOW);

  for (int i = 0; i < sensorCount; i++) {
    pinMode(sensorPins[i], INPUT);
  }

  connectToWiFi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  reconnect();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  String payload = "";
  for (int i = 0; i < sensorCount; i++) {
    int moisture = convertToPercentage(analogRead(sensorPins[i]));
    payload += "Sensor " + String(i + 1) + ": " + String(moisture) + "%\n";
  }

  client.publish(moistureTopic, payload.c_str());
  delay(1000);
}
