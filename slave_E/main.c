#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "mqtt_client.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_log.h"

// Wi-Fi credentials
#define WIFI_SSID "Main Server"
#define WIFI_PASS "admin@123"

// MQTT broker details
#define MQTT_URI "mqtt://5.196.78.28"
#define MOISTURE_TOPIC "sector 2"
#define CONTROL_TOPIC "sector 2 sprinkler"

// GPIO pins
#define VALVE1_PIN GPIO_NUM_13
#define VALVE2_PIN GPIO_NUM_12
#define VALVE3_PIN GPIO_NUM_14
#define VALVE4_PIN GPIO_NUM_27
#define SENSOR1_PIN ADC1_CHANNEL_6 // GPIO34
#define SENSOR2_PIN ADC1_CHANNEL_7 // GPIO35
#define SENSOR3_PIN ADC1_CHANNEL_4 // GPIO32
#define SENSOR4_PIN ADC1_CHANNEL_5 // GPIO33

static const char *TAG = "ESP32_MQTT";

static esp_mqtt_client_handle_t client;

static void connect_wifi() {
    ESP_LOGI(TAG, "Connecting to Wi-Fi...");
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    esp_event_loop_create_default();

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();

    esp_wifi_connect();
    ESP_LOGI(TAG, "Wi-Fi connected");
}

static int convert_to_percentage(int raw_value) {
    return (int)((100.0 * (4095 - raw_value)) / 4095);
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = event_data;

    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "Received: topic=%.*s, data=%.*s",
                     event->topic_len, event->topic,
                     event->data_len, event->data);

            if (strncmp(event->topic, CONTROL_TOPIC, event->topic_len) == 0) {
                if (strncmp(event->data, "sprinkler1_on", event->data_len) == 0) {
                    gpio_set_level(VALVE1_PIN, 1);
                } else if (strncmp(event->data, "sprinkler1_off", event->data_len) == 0) {
                    gpio_set_level(VALVE1_PIN, 0);
                }
                // Add additional control logic for other valves here
            }
            break;
        default:
            break;
    }
}

void app_main(void) {
    nvs_flash_init();
    connect_wifi();

    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << VALVE1_PIN) | (1ULL << VALVE2_PIN) |
                        (1ULL << VALVE3_PIN) | (1ULL << VALVE4_PIN),
        .mode = GPIO_MODE_OUTPUT,
    };
    gpio_config(&io_conf);

    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(SENSOR1_PIN, ADC_ATTEN_DB_11);
    adc1_config_channel_atten(SENSOR2_PIN, ADC_ATTEN_DB_11);
    adc1_config_channel_atten(SENSOR3_PIN, ADC_ATTEN_DB_11);
    adc1_config_channel_atten(SENSOR4_PIN, ADC_ATTEN_DB_11);

    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = MQTT_URI,
    };
    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);

    while (true) {
        int sensor_values[4];
        sensor_values[0] = convert_to_percentage(adc1_get_raw(SENSOR1_PIN));
        sensor_values[1] = convert_to_percentage(adc1_get_raw(SENSOR2_PIN));
        sensor_values[2] = convert_to_percentage(adc1_get_raw(SENSOR3_PIN));
        sensor_values[3] = convert_to_percentage(adc1_get_raw(SENSOR4_PIN));

        ESP_LOGI(TAG, "Moisture Sensor 1: %d%%", sensor_values[0]);
        ESP_LOGI(TAG, "Moisture Sensor 2: %d%%", sensor_values[1]);
        ESP_LOGI(TAG, "Moisture Sensor 3: %d%%", sensor_values[2]);
        ESP_LOGI(TAG, "Moisture Sensor 4: %d%%", sensor_values[3]);

        char payload[100];
        snprintf(payload, sizeof(payload), "sensor 1: %d\nsensor 2: %d\nsensor 3: %d\nsensor 4: %d",
                 sensor_values[0], sensor_values[1], sensor_values[2], sensor_values[3]);
        esp_mqtt_client_publish(client, MOISTURE_TOPIC, payload, 0, 1, 0);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
