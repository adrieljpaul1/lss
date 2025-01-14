#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"

// Create a handle for our queue that will pass data between tasks
static QueueHandle_t sensor_queue;
static const char *TAG = "SensorApp";

// Structure to hold our sensor data
typedef struct {
    int sensor_value;
    uint32_t timestamp;
} sensor_data_t;

// This task simulates reading from a sensor
void sensor_task(void *pvParameters) {
    // Initialize variables for our simulated sensor
    int sensor_value = 0;
    sensor_data_t data;
    
    while(1) {
        // Simulate reading a sensor by incrementing a value
        sensor_value = (sensor_value + 1) % 100;
        
        // Package our sensor reading into our data structure
        data.sensor_value = sensor_value;
        data.timestamp = xTaskGetTickCount() * portTICK_PERIOD_MS;  // Get current time in ms
        
        // Send the data to our processing task through the queue
        if (xQueueSend(sensor_queue, &data, pdMS_TO_TICKS(100)) != pdPASS) {
            ESP_LOGE(TAG, "Failed to send data to queue!");
        }
        
        // Wait for 1 second before next reading
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// This task processes and displays sensor data
void process_task(void *pvParameters) {
    sensor_data_t received_data;
    
    while(1) {
        // Wait for data to arrive from the sensor task
        if (xQueueReceive(sensor_queue, &received_data, portMAX_DELAY) == pdPASS) {
            // Log the received data
            ESP_LOGI(TAG, "Sensor Value: %d, Timestamp: %lu ms", 
                     received_data.sensor_value, received_data.timestamp);
            
            // Process the data (in this case, we'll just check thresholds)
            if (received_data.sensor_value > 80) {
                ESP_LOGW(TAG, "High sensor value detected!");
            }
        }
    }
}

void app_main(void) {
    // Create our queue before starting tasks
    sensor_queue = xQueueCreate(10, sizeof(sensor_data_t));
    
    if (sensor_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create queue!");
            return 1;
    }

    // Create our tasks
    xTaskCreate(sensor_task, "sensor_task", 2048, NULL, 5, NULL);
    xTaskCreate(process_task, "process_task", 2048, NULL, 4, NULL);
    
    ESP_LOGI(TAG, "Application started!");
}