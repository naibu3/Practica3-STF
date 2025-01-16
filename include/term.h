#ifndef __THERM_H__
#define __THERM_H__
#include <esp_adc/adc_oneshot.h>
#include <hal/adc_types.h>
// libc
#include <time.h>
#include <stdio.h>
#include <sys/time.h>

// freerqtos
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <driver/gpio.h>

// esp
#include <esp_system.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_timer.h>


#define SERIES_RESISTANCE 10000 // 10K ohms
#define NOMINAL_RESISTANCE 10000 // 10K ohms
#define NOMINAL_TEMPERATURE 298.15 // 25°C en Kelvin
#define BETA_COEFFICIENT 3950 // Constante B
#define GPIO_OUTPUT_PIN_2 2 //Puerto GPIO de salida


typedef struct therm_conf_t{
 adc_channel_t adc_channel;
 int gpio_pin;
}therm_t;


// funciones inicializacion y configuracion
esp_err_t therm_init();
esp_err_t therm_config(therm_t* thermistor,adc_channel_t channel, int gpio_pin);
//funcionalidades thermistor
float therm_read_t(therm_t thermistor);
float therm_read_v (therm_t thermistor);
uint16_t therm_read_lsb(therm_t thermistor);
void therm_up(therm_t thermistor);
void therm_down(therm_t thermistor);

// Converion lsb a temperatura
float convert_lsb_t(uint16_t lsb_value);

#endif