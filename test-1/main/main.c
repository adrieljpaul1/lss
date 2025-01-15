#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc/adc_oneshot.h"

// Definitions
#define HIGH 1
#define LOW 0
#define MOTOR_PIN GPIO_NUM_13
static const char *TAG = "Init System";

// Shared variables
float val[4];
SemaphoreHandle_t mutex = NULL;

// GPIO Configuration
void configure_gpio(int gpio_num, gpio_mode_t mode, gpio_pullup_t pullup, gpio_pulldown_t pulldown) {
    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask = (1ULL << gpio_num);
    io_conf.mode = mode;
    io_conf.pull_up_en = pullup;
    io_conf.pull_down_en = pulldown;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&io_conf);
}

// ADC Configuration
void configure_adc() {
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);
    adc1_config_channel_atten(ADC1_CHANNEL_3, ADC_ATTEN_DB_11);
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_11);
    adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_11);
}

// Convert Raw ADC Value to Percentage
float conv_percent(int raw_value) {
    raw_value = raw_value > 4095 ? 4095 : raw_value < 0 ? 0 : raw_value;
    return ((float)(4095 - raw_value) / 4095.0f) * 100.0f;
}

// Task: Read Sensor Values
void read_val_task(void *pvParams) {
    while (1) {
        float local_val[4];
        local_val[0] = conv_percent(adc1_get_raw(ADC1_CHANNEL_0));
        local_val[1] = conv_percent(adc1_get_raw(ADC1_CHANNEL_3));
        local_val[2] = conv_percent(adc1_get_raw(ADC1_CHANNEL_6));
        local_val[3] = conv_percent(adc1_get_raw(ADC1_CHANNEL_7));

        // Protect shared resource
        xSemaphoreTake(mutex, portMAX_DELAY);
        for (int i = 0; i < 4; i++) {
            val[i] = local_val[i];
        }
        xSemaphoreGive(mutex);

        ESP_LOGI(TAG, "Sensor Values: %.2f, %.2f, %.2f, %.2f", val[0], val[1], val[2], val[3]);
        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay 1 second
    }
}

// Task: Actuate Motor
void actuate_task(void *pvParams) {
    while (1) {
        float average = 0;

        // Protect shared resource
        xSemaphoreTake(mutex, portMAX_DELAY);
        for (int i = 0; i < 4; i++) {
            average += val[i];
        }
        xSemaphoreGive(mutex);

        average /= 4.0f;

        if (average < 40.0f) {
            gpio_set_level(MOTOR_PIN, HIGH);
            ESP_LOGI(TAG, "Motor Actuated: ON");
        } else {
            gpio_set_level(MOTOR_PIN, LOW);
            ESP_LOGI(TAG, "Motor Actuated: OFF");
        }

        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay 1 second
    }
}

// Task: Log Motor Status
void log_status_task(void *pvParams) {
    while (1) {
        int motor_state = gpio_get_level(MOTOR_PIN);
        ESP_LOGI(TAG, "Motor Status: %s", motor_state == LOW ? "ON" : "OFF");
        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay 1 second
    }
}

// Main Application
void app_main(void) {
    // Initialize Mutex
    mutex = xSemaphoreCreateMutex();
    if (mutex == NULL) {
        ESP_LOGE(TAG, "Failed to create mutex");
        return;
    }

    // Configure ADC and GPIO
    configure_adc();
    configure_gpio(MOTOR_PIN, GPIO_MODE_OUTPUT, GPIO_PULLUP_DISABLE, GPIO_PULLDOWN_DISABLE);

    // Create Tasks
    xTaskCreatePinnedToCore(read_val_task, "Read Values", 4096, NULL, 5, NULL, 1);
    xTaskCreatePinnedToCore(actuate_task, "Actuate Motor", 4096, NULL, 4, NULL, 1);
    xTaskCreatePinnedToCore(log_status_task, "Log Status", 2048, NULL, 3, NULL, 1);
}
