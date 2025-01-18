#include <WiFi.h>
#include <PubSubClient.h>

// Wi-Fi credentials
const char* ssid = "Main Server";
const char* password = "admin@123";

// MQTT broker details
const char* mqtt_server = "5.196.78.28";  // Replace with your MQTT broker IP

WiFiClient espClient;
PubSubClient client(espClient);

// Sensor values received from the slaves
int sensor[4];

// Number of slaves (adjust this based on the number of ESP32s you have)
const int numSlaves = 10;

void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  
  // Convert byte payload to String
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  // Debug print: Check the raw incoming message
    Serial.print("        ");
    Serial.print(topic);
    Serial.println("  ");
    Serial.println(message);
    Serial.println("     ");

  // Extract the slave number from the topic and determine which slave the message is from
  int slaveNumber = 0;
  sscanf(topic, "sector %d", &slaveNumber);  // Extract slave number from topic

  if (slaveNumber > 0 && slaveNumber <= numSlaves) {
    // Parse the moisture values (assuming CSV format)
    int parsedValues = sscanf(message.c_str(), "%d,%d,%d,%d", &sensor[0], &sensor[1], &sensor[2], &sensor[3]);

    // Check if the values were parsed correctly
    if (parsedValues == 4) {
      Serial.print("        ");
      Serial.print(topic);
      Serial.println("  ");
      Serial.println(message);
      Serial.println("     ");

      for (int i = 0; i < 4; i++) {
        Serial.print("Sensor ");
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.println(sensor[i]);
      }
    } 
  } else {
    Serial.println("Invalid slave number or topic.");
  }
}

// Function to reconnect to MQTT broker
unsigned long lastAttemptTime = 0;
unsigned long retryInterval = 5000; // 5 seconds

void reconnect() {
  if (millis() - lastAttemptTime >= retryInterval) {
    lastAttemptTime = millis();
    while (!client.connected()) {
      Serial.print("Attempting MQTT connection...");

      // Attempt to connect with a unique client ID
      if (client.connect("GIGA_Master")) {
        Serial.println("Connected to MQTT broker");

        // Subscribe to all slave topics
        for (int i = 1; i <= numSlaves; i++) {
          String topic = "sector " + String(i);  // Corrected with the semicolon
          if (client.subscribe(topic.c_str())) {  // Removed extra semicolon
            Serial.print("Subscribed to: ");
            Serial.println(topic);
          } else {
            Serial.print("Failed to subscribe to: ");
            Serial.println(topic);
          }
        }

      } else {
        Serial.print("Failed, rc=");
        Serial.println(client.state());  // Print the error code
        delay(1000);  // Retry after 1 second
      }
    }
  }
}

void setup() {
  // Start serial communication
  Serial.begin(115200);
  Serial.println("Giga MQTT Subscriber");

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Wi-Fi connected");

  // Print IP address
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Setup MQTT client
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // Ensure we connect to MQTT broker and subscribe to slave topics
  reconnect();
}

void loop() {
  // Ensure connection to MQTT broker
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  // Optionally print some feedback to confirm loop is running
  delay(1000);  // Add delay to prevent flooding the Serial Monitor with output
}
