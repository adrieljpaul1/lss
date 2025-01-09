// ESP32 Stress Test for Arduino IDE
#include <WiFi.h>
#include <SPIFFS.h>
#include <Wire.h>

// GPIO settings
#define GPIO_TEST_PIN 2

// I2C settings
#define I2C_MASTER_SCL_IO 22
#define I2C_MASTER_SDA_IO 21
#define I2C_MASTER_FREQ_HZ 100000

// Task handles
TaskHandle_t cpuTask;
TaskHandle_t memoryTask;
TaskHandle_t gpioTask;
TaskHandle_t wifiTask;
TaskHandle_t spiffsTask;
TaskHandle_t i2cTask;

// CPU stress test
void cpuStressTask(void *parameter) {
  while (1) {
    volatile int n = 2;
    while (1) {
      volatile int isPrime = 1;
      for (int i = 2; i < n; i++) {
        if (n % i == 0) {
          isPrime = 0;
          break;
        }
      }
      n++;
    }
  }
}

// Memory stress test
void memoryStressTask(void *parameter) {
  while (1) {
    char *ptr = (char*)malloc(1024);  // Allocate 1 KB
    if (ptr) {
      memset(ptr, 0, 1024);
      free(ptr);
    }
    delay(1);  // Small delay to prevent watchdog trigger
  }
}

// GPIO stress test
void gpioStressTask(void *parameter) {
  pinMode(GPIO_TEST_PIN, OUTPUT);
  while (1) {
    digitalWrite(GPIO_TEST_PIN, HIGH);
    digitalWrite(GPIO_TEST_PIN, LOW);
    delay(1);  // Small delay to prevent watchdog trigger
  }
}

// WiFi stress test
void wifiStressTask(void *parameter) {
  WiFi.mode(WIFI_STA);
  while (1) {
    WiFi.begin("YourSSID", "YourPassword");
    delay(5000);
    WiFi.disconnect();
    delay(1000);
  }
}

// SPIFFS stress test
void spiffsStressTask(void *parameter) {
  while (1) {
    File f = SPIFFS.open("/test.txt", "w");
    if (f) {
      f.println("Stress test data");
      f.close();
    }

    f = SPIFFS.open("/test.txt", "r");
    if (f) {
      String data = f.readString();
      f.close();
    }
    delay(1);  // Small delay to prevent watchdog trigger
  }
}

// I2C stress test
void i2cStressTask(void *parameter) {
  uint8_t data[10] = {0};
  while (1) {
    Wire.beginTransmission(0x3C);
    Wire.write(data, 10);
    Wire.endTransmission();
    delay(1);  // Small delay to prevent watchdog trigger
  }
}

void setup() {
  Serial.begin(115200);
  
  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS initialization failed!");
    return;
  }
  
  // Initialize I2C
  Wire.begin(I2C_MASTER_SDA_IO, I2C_MASTER_SCL_IO, I2C_MASTER_FREQ_HZ);
  
  // Create tasks
  xTaskCreatePinnedToCore(cpuStressTask, "CPU Stress", 2048, NULL, 5, &cpuTask, 0);
  xTaskCreate(memoryStressTask, "Memory Stress", 2048, NULL, 5, &memoryTask);
  xTaskCreate(gpioStressTask, "GPIO Stress", 2048, NULL, 5, &gpioTask);
  xTaskCreate(wifiStressTask, "WiFi Stress", 4096, NULL, 5, &wifiTask);
  xTaskCreate(spiffsStressTask, "SPIFFS Stress", 4096, NULL, 5, &spiffsTask);
  xTaskCreate(i2cStressTask, "I2C Stress", 2048, NULL, 5, &i2cTask);
  
  Serial.println("All stress tests started!");
}

void loop() {
  // Main loop can be used for monitoring
  Serial.println("System running...");
  delay(5000);
}
