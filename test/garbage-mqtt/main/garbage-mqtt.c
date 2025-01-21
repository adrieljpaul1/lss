#include <stdio.h>

#include "esp_adc/adc_oneshot.h"

#include "hal/adc_types.h"

#include "esp_log.h"

#include "freertos/FreeRTOS.h"

#include "freertos/task.h"

#include "driver/gpio.h"

#include "esp_wifi.h"

#include "esp_event.h"

#include "nvs_flash.h"

#include "esp_netif.h"

#include "esp_wifi_types.h"

#include "mqtt_client.h"

#include"esp_system.h"

#include "esp_random.h"

#define MQTT_RECONNECT_TIMEOUT_MS 10000

#define WIFI_RETRY_DELAY_MS 5000

#define ADC_READING_DELAY_MS 1000

#define WIFI_MAXIMUM_RETRY 5

// Global variables

int values[4]; // Array to store ADC readings

static int s_retry_num = 0; // WiFi retry counter

// Logging tags
static
const char * TAGS = "Soil Moisture";

static
const char * TAGw = "WiFi System";

static
const char * TAGm = "MQTT";

// MQTT client handle

static esp_mqtt_client_handle_t client = NULL;

static bool mqtt_connected = false;

// WiFi credentials - consider moving to menuconfig

#define WIFI_SSID "realme"

#define WIFI_PASS "ilakkiya"

#define MQTT_BROKER_URL "mqtt://5.196.78.28:1883"

// Log sensor values
void log_value() {
  for (int i = 0; i < 4; i++) {
    ESP_LOGI(TAGS, "values: %d",values[i]);
  }
}

// WiFi event handler
static void wifi_event_handler(void * arg, esp_event_base_t event_base,
  int32_t event_id, void * event_data) {
  if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
    ESP_LOGI(TAGw, "Connecting to AP...");
    esp_wifi_connect();
  } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
    if (s_retry_num < WIFI_MAXIMUM_RETRY) {
      esp_wifi_connect();
      s_retry_num++;
      ESP_LOGI(TAGw, "Retry to connect to AP (%d/%d)", s_retry_num, WIFI_MAXIMUM_RETRY);
    }
    ESP_LOGI(TAGw, "Failed to connect to AP");
  } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
    ip_event_got_ip_t * event = (ip_event_got_ip_t * ) event_data;
    ESP_LOGI(TAGw, "Got IP:"
      IPSTR, IP2STR( & event -> ip_info.ip));
    s_retry_num = 0;
  }
}

// MQTT event handler
static void mqtt_event_handler(void * handler_args, esp_event_base_t base, int32_t event_id, void * event_data) {
  ESP_LOGD(TAGm, "Event dispatched from event loop base=%s, event_id=%"
    PRIi32, base, event_id);
  esp_mqtt_event_handle_t event = event_data;

  switch (event -> event_id) {
  case MQTT_EVENT_CONNECTED:
    ESP_LOGI(TAGm, "MQTT Connected");
    mqtt_connected = true;
    esp_mqtt_client_subscribe(client, "/soil_moisture/control", 0);
    break;

  case MQTT_EVENT_DISCONNECTED:
    ESP_LOGW(TAGm, "MQTT Disconnected");
    mqtt_connected = false;
    break;

  case MQTT_EVENT_SUBSCRIBED:
    ESP_LOGI(TAGm, "MQTT Subscribed");
    break;

  case MQTT_EVENT_PUBLISHED:
    ESP_LOGI(TAGm, "MQTT Data Published");
    break;

  case MQTT_EVENT_ERROR:
    ESP_LOGE(TAGm, "MQTT Error occurred");
    if (event -> error_handle -> error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
      ESP_LOGE(TAGm, "Last error code reported from esp-tls: 0x%x", event -> error_handle -> esp_tls_last_esp_err);
      ESP_LOGE(TAGm, "Last tls stack error number: 0x%x", event -> error_handle -> esp_tls_stack_err);
      ESP_LOGE(TAGm, "Last captured errno : %d (%s)", event -> error_handle -> esp_transport_sock_errno,
        strerror(event -> error_handle -> esp_transport_sock_errno));
    }
    break;

  default:
    ESP_LOGW(TAGm, "Other event id: %d", event -> event_id);
    break;
  }
}

// Initialize MQTT client
static void mqtt_app_start(void) {
  esp_mqtt_client_config_t mqtt_cfg = {
    .broker.address.uri = "mqtt://5.196.78.28:1883", // Replace with your broker's IP
    .broker.verification.skip_cert_common_name_check = true,
    .credentials = {
      .username = "",
      .authentication = {
        .password = ""
      }
    },
    .session.last_will.topic = "/soil_moisture/status", // Optional: for device status tracking
    .session.last_will.msg = "offline",
    .session.last_will.retain = 1,
    .session.last_will.qos = 2
  };

  client = esp_mqtt_client_init( & mqtt_cfg);
  if (client == NULL) {
    ESP_LOGE(TAGm, "Failed to initialize MQTT client");
    return;
  }

  ESP_ERROR_CHECK(esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL));
  ESP_ERROR_CHECK(esp_mqtt_client_start(client));
}

// Initialize WiFi in station mode
static void wifi_init_sta(void) {
  // Initialize TCP/IP adapter
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  // Create default WiFi station interface
  esp_netif_t * sta_netif = esp_netif_create_default_wifi_sta();
  assert(sta_netif);

  // Initialize WiFi with default configuration
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init( & cfg));

  // Register event handlers
  ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
    ESP_EVENT_ANY_ID, &
    wifi_event_handler,
    NULL,
    NULL));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
    IP_EVENT_STA_GOT_IP, &
    wifi_event_handler,
    NULL,
    NULL));

  // Configure WiFi settings
  wifi_config_t wifi_config = {
    .sta = {
      .ssid = WIFI_SSID,
      .password = WIFI_PASS,
      .threshold.authmode = WIFI_AUTH_WPA2_PSK,
      .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
    },
  };

  // Set mode and configuration
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, & wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());

  ESP_LOGI(TAGw, "wifi_init_sta finished.");
}

void app_main(void) {
  // Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  // Initialize WiFi and MQTT
  wifi_init_sta();
  mqtt_app_start();
  log_value();
  // Main loop
  while (1) {
    if (mqtt_connected) {
    	for(int i = 0;i<4;i++){
  		values[i] = (int)(esp_random() % 100 +1);
  	}
      char message[100];
      snprintf(message, sizeof(message),
        "{\"moisture\":[%d,%d,%d,%d]}",
        values[0], values[1], values[2], values[3]);
      esp_mqtt_client_publish(client, "test/topic", message, 0, 1, 2);
      ESP_LOGI(TAGm,"{\"moisture\":[%d,%d,%d,%d]}",
        values[0], values[1], values[2], values[3]);
    }
    vTaskDelay(pdMS_TO_TICKS(ADC_READING_DELAY_MS));
  }
}