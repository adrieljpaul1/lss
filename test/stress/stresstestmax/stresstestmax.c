#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_wifi.h"
#include "esp_spiffs.h"
#include "driver/gpio.h"
#include "driver/i2c.h"

// GPIO settings
#define GPIO_TEST_PIN GPIO_NUM_2

// I2C settings
#define I2C_MASTER_SCL_IO 22
#define I2C_MASTER_SDA_IO 21
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_FREQ_HZ 100000

// Function to initialize I2C
void i2c_master_init() {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ
    };
    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
}

// CPU stress test
void cpu_stress_task(void *pvParameters) {
    while (1) {
        volatile int n = 2;
        while (1) {
            volatile int is_prime = 1;
            for (int i = 2; i < n; i++) {
                if (n % i == 0) {
                    is_prime = 0;
                    break;
                }
            }
            n++;
        }
    }
}

// Memory stress test
void memory_stress_task(void *pvParameters) {
    while (1) {
        char *ptr = malloc(1024);  // Allocate 1 KB
        if (ptr) {
            memset(ptr, 0, 1024);
            free(ptr);
        }
    }
}

// GPIO stress test
void gpio_stress_task(void *pvParameters) {
    gpio_set_direction(GPIO_TEST_PIN, GPIO_MODE_OUTPUT);
    while (1) {
        gpio_set_level(GPIO_TEST_PIN, 1);
        gpio_set_level(GPIO_TEST_PIN, 0);
    }
}

// Wi-Fi stress test
void wifi_stress_task(void *pvParameters) {
    while (1) {
        esp_wifi_connect();
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        esp_wifi_disconnect();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

// SPIFFS stress test
void spiffs_stress_task(void *pvParameters) {
    while (1) {
        FILE *f = fopen("/spiffs/test.txt", "w");
        if (f) {
            fprintf(f, "Stress test data");
            fclose(f);
        }

        f = fopen("/spiffs/test.txt", "r");
        if (f) {
            char buffer[64];
            fread(buffer, 1, sizeof(buffer), f);
            fclose(f);
        }
    }
}

// I2C stress test
void i2c_stress_task(void *pvParameters) {
    uint8_t data[10] = {0};
    while (1) {
        i2c_master_write_to_device(I2C_MASTER_NUM, 0x3C, data, sizeof(data), 1000 / portTICK_PERIOD_MS);
    }
}

// Main function to create tasks
void app_main() {
    // Initialize peripherals
    esp_vfs_spiffs_register(NULL);  // Initialize SPIFFS
    i2c_master_init();             // Initialize I2C
    esp_wifi_init(NULL);           // Initialize Wi-Fi

    // Create tasks
    xTaskCreatePinnedToCore(cpu_stress_task, "CPU Stress Task", 2048, NULL, 5, NULL, 0);
    xTaskCreate(memory_stress_task, "Memory Stress Task", 2048, NULL, 5, NULL);
    xTaskCreate(gpio_stress_task, "GPIO Stress Task", 2048, NULL, 5, NULL);
    xTaskCreate(wifi_stress_task, "Wi-Fi Stress Task", 4096, NULL, 5, NULL);
    xTaskCreate(spiffs_stress_task, "SPIFFS Stress Task", 4096, NULL, 5, NULL);
    xTaskCreate(i2c_stress_task, "I2C Stress Task", 2048, NULL, 5, NULL);
}
