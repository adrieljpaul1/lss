#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc/adc_oneshot.h"

static const char *TAG = "please help";


//gpio configuration da boi
void configuregpio(int gpio_num, gpio_mode_t mode, gpio_pullup_t pullup, gpio_pulldown_t pulldown) {
    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask = (1ULL << gpio_num);  // Select the GPIO pin
    io_conf.mode = mode;                        // Set mode (input/output)
    io_conf.pull_up_en = pullup;                // Enable pull-up resistor
    io_conf.pull_down_en = pulldown;            // Enable pull-down resistor
    io_conf.intr_type = GPIO_INTR_DISABLE;      // Disable interrupts (for now)

    gpio_config(&io_conf);  // Apply the configuration
}
//adc configuration for ranged input-output from sensors
void configure_adc() {
    adc1_config_width(ADC_WIDTH_BIT_12); // Configure width (12-bit resolution)
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);// Configure attenuation
    adc1_config_channel_atten(ADC1_CHANNEL_3, ADC_ATTEN_DB_11);// Configure attenuation
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_11);// Configure attenuation
    adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_11);// Configure attenuation
}
/*
for reference:
   int read_soil_moisture() {
    return adc1_get_raw(ADC1_CHANNEL_0); // Read raw ADC value
  // configuregpio(13, GPIO_MODE_INPUT, GPIO_PULLUP_ENABLE,GPIO_PULLDOWN_DISABLE);
  }
*/
//Definitions

int val[4];
//int perval[4]; you dont need this
int avg = 0;
QueueHandle_t taskQueue = NULL;

//function to get raw sensor data and record it in an array
float convPercent(int argument){
  /*
    if (raw_value > 4095) raw_value = 4095;
    if (raw_value < 0) raw_value = 0;
    */
   //keeping the raw values in bound
  argument = argument > 4095 ? 4095 : argument;
  argument = argument < 0 ? 0 : argument;  
  float percent = ((float)(4095 - argument) / 4095.0f) * 100.0f;
  return percent;
}
void readVal(){
  while(1){
    /*for reference : 
      int %d = adc1_get_raw(ADC1_CHANNEL_0);
    */
   *val = adc1_get_raw(ADC1_CHANNEL_0); // VP -- color = green
   *(val + 1) = adc1_get_raw(ADC1_CHANNEL_3); // VN - color = yellow
   *(val + 2) = adc1_get_raw(ADC1_CHANNEL_6); // D34 -- color = orange
   *(val + 3) = adc1_get_raw(ADC1_CHANNEL_7); // D35 -- color = red
   
  }
  printf("\n===\n");
  
  vTaskDelay(1000/portTICK_PERIOD_MS);
  }
}


void app_main(void){
  configure_adc(); 
}