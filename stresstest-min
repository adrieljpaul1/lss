// Minimal test configuration
#include <WiFi.h>
#include <SPIFFS.h>
#include <Wire.h>

// GPIO settings
#define GPIO_TEST_PIN 2  // Built-in LED on most ESP32 dev boards

// WiFi credentials - CHANGE THESE
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

void setup() {
  Serial.begin(115200);
  delay(1000);  // Give serial connection time to start
  
  Serial.println("ESP32 Stress Test Starting...");
  
  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS initialization failed!");
    return;
  }
  Serial.println("SPIFFS initialized successfully");
  
  // Format SPIFFS if you want to start clean (optional)
  // SPIFFS.format();
  
  // Initialize I2C
  Wire.begin(21, 22);  // SDA = 21, SCL = 22
  Serial.println("I2C initialized");
  
  // Initialize WiFi in station mode
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  // Wait for WiFi connection
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 10) {
    delay(1000);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi connection failed");
  }
}

void loop() {
  // Simple test to verify everything is working
  digitalWrite(GPIO_TEST_PIN, HIGH);
  delay(1000);
  digitalWrite(GPIO_TEST_PIN, LOW);
  delay(1000);
  
  Serial.println("System running...");
}
