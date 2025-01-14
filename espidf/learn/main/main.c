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
  }
*/

void app_main(void){
 //commented because im using the analog output AO in soilmoisture sensor
 // configuregpio(13, GPIO_MODE_INPUT, GPIO_PULLUP_ENABLE,GPIO_PULLDOWN_DISABLE);
  configure_adc();
  float rawval[4];
  float avg = 0;
  printf("init finished, going to read sensor!\n");
  while(1){
    /*for reference : 
      int rawval = adc1_get_raw(ADC1_CHANNEL_0);
    */
   *rawval = adc1_get_raw(ADC1_CHANNEL_0); // VP -- color = green
   *(rawval + 1) = adc1_get_raw(ADC1_CHANNEL_3); // VN - color = yellow
   *(rawval + 2) = adc1_get_raw(ADC1_CHANNEL_6); // D34 -- color = orange
   *(rawval + 3) = adc1_get_raw(ADC1_CHANNEL_7); // D35 -- color = red
   for(int i = 0;i<4;i++){
    avg += rawval[i];
   }
   //printf("%d",avg);
   avg = avg/4;
   //printf("");
   //printf("%d\n",rawval);

   for(int  i = 0;i<4;i++){
    printf("%d\n",*(rawval+i));
   }
   printf("Average = %f",avg);
   
   printf("\n===\n");
    vTaskDelay(1000/portTICK_PERIOD_MS);
  }
}