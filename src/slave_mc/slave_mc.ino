#include <WiFi.h>
#include <PubSubClient.h>
#define SIR_UID "realme"
#define SIR_PWD "ilakkiya"
// Wi-Fi credentials
const char* ssid = "Main Server";
const char* password = "admin@123";

// MQTT broker details
const char* mqtt_server = "5.196.78.28";

WiFiClient espClient;
PubSubClient client(espClient);

// Pin assignments for one (360 sprklr) solenoid valve
int valvePin = 13;

// Moisture sensor pins
int sensor1Pin = 34;
int sensor2Pin = 35;
int sensor3Pin = 32;
int sensor4Pin = 33;

// MQTT topic names
const char* moistureTopic = "sector 2";
const char* sprinklerControlTopic = "sector 2 sprinkler";

// Function to convert raw moisture sensor values to percentage
int convertToPercentage(int rawValue) {
  // Assuming a raw value of 0 is 100% (very wet) and 4095 is 0% (very dry)
  int percentage = map(rawValue, 0, 4095, 100, 0);  // Invert the scale
  return constrain(percentage, 0, 100);             // Ensure percentage is within 0-100
}

// Function to handle incoming MQTT messages
void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Received message from topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  Serial.println(message);

  if (String(topic) == sprinklerControlTopic) {
    if (message == "sprinkler1_on") {
      digitalWrite(valvePin, HIGH);
    } else if (message == "sprinkler1_off") {
      digitalWrite(valvePin, LOW);
    }
    // Add controls for other sprinklers as needed
  }
}

// Function to reconnect to MQTT broker
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32_Sector1")) {
      Serial.println("Connected to MQTT broker");
      client.subscribe(sprinklerControlTopic);
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" trying again in 1 second");
      delay(1000);
    }
  }
}

// Function to connect to Wi-Fi
void connectToWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);

  // Set up moisture sensor pins as INPUT
  pinMode(sensor1Pin, INPUT);
  pinMode(sensor2Pin, INPUT);
  pinMode(sensor3Pin, INPUT);
  pinMode(sensor4Pin, INPUT);

  // Set up solenoid valve control pins as OUTPUT
  pinMode(valvePin, OUTPUT);

  // Initially turn off all valves
  digitalWrite(valvePin, LOW);

  // Connect to Wi-Fi first
  connectToWiFi();

  // Setup MQTT client
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // Ensure we connect to MQTT broker
  reconnect();
}

void loop() {
  // Ensure connection to MQTT broker
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Read moisture levels from sensors and convert to percentage
  int moistureValues[4];
  moistureValues[0] = convertToPercentage(analogRead(sensor1Pin));
  moistureValues[1] = convertToPercentage(analogRead(sensor2Pin));
  moistureValues[2] = convertToPercentage(analogRead(sensor3Pin));
  moistureValues[3] = convertToPercentage(analogRead(sensor4Pin));

  // Debugging: print moisture levels to the serial monitor
  Serial.print("Moisture Sensor 1: ");
  Serial.println(moistureValues[0]);
  Serial.print("Moisture Sensor 2: ");
  Serial.println(moistureValues[1]);
  Serial.print("Moisture Sensor 3: ");
  Serial.println(moistureValues[2]);
  Serial.print("Moisture Sensor 4: ");
  Serial.println(moistureValues[3]);

  // Send moisture levels to the MQTT broker as a percentage
  String payload = String() + "sensor 1 :" + String(moistureValues[0]) + "\n"
                                                                         "sensor 2 :"
                   + String(moistureValues[1]) + "\n"
                                                 "sensor 3 :"
                   + String(moistureValues[2]) + "\n"
                                                 "sensor 4 :"
                   + String(moistureValues[3]);
  client.publish(moistureTopic, payload.c_str());

  delay(1000);  // Check every second for burning the esp32
}
