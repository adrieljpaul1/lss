// RTOS Configuration Example
#include <WiFi.h>

// Core definitions
#define ARDUINO_RUNNING_CORE 1
#define STRESS_TEST_CORE    0

// Task handles
TaskHandle_t Task1;
TaskHandle_t Task2;

// Example of two tasks running on different cores
void Task1code(void *pvParameters) {
  for(;;) {
    // Task 1 running on core 0
    Serial.print("Task1 running on core ");
    Serial.println(xPortGetCoreID());
    delay(1000);
  }
}

void Task2code(void *pvParameters) {
  for(;;) {
    // Task 2 running on core 1
    Serial.print("Task2 running on core ");
    Serial.println(xPortGetCoreID());
    delay(1000);
  }
}

void setup() {
  Serial.begin(115200);
  
  // Create tasks with specific core assignments
  xTaskCreatePinnedToCore(
    Task1code,    /* Task function */
    "Task1",      /* Task name (for debugging) */
    10000,        /* Stack size (bytes) */
    NULL,         /* Parameter to pass */
    1,            /* Task priority (1-24) */
    &Task1,       /* Task handle */
    0             /* Core where the task should run */
  );

  xTaskCreatePinnedToCore(
    Task2code,    /* Task function */
    "Task2",      /* Task name */
    10000,        /* Stack size */
    NULL,         /* Parameter */
    2,            /* Priority (higher than Task1) */
    &Task2,       /* Handle */
    1             /* Core */
  );
}

void loop() {
  // The loop() function runs on core 1 by default
  delay(1000);
}
