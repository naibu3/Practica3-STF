
#include <esp_err.h>
#include <stdio.h>
#include <math.h>
#include "term.h"

 adc_oneshot_unit_handle_t adc_hdlr;
 adc_oneshot_unit_init_cfg_t unit_cfg = {
 .unit_id = ADC_UNIT_1,
 .clk_src = ADC_RTC_CLK_SRC_DEFAULT,
 };

esp_err_t therm_init() {
    
    // Inicializa el ADC One-Shot
    adc_oneshot_unit_init_cfg_t init_cfg = {
        .unit_id = ADC_UNIT_1, // Usa ADC_UNIT_1 como predeterminado
    };
    
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&unit_cfg, &adc_hdlr));
    return ESP_OK; // Inicialización exitosa
}

esp_err_t therm_config(therm_t* thermistor, adc_channel_t channel, int gpio_pin) {

    // Almacena el ADC handle y el canal en la estructura del termistor
    thermistor->adc_channel = channel;
    thermistor->gpio_pin = gpio_pin;

    // Configura el canal ADC
    adc_oneshot_chan_cfg_t chan_cfg = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_11,  // Configuración típica para medir hasta ~3.3V
    };

    if(gpio_pin != -1){
        // Configurar el GPIO como salida
        gpio_config_t io_conf = {};
        io_conf.intr_type = GPIO_INTR_DISABLE;  // Deshabilitar interrupciones
        io_conf.mode = GPIO_MODE_OUTPUT;        // Configurar como salida
        io_conf.pin_bit_mask = (1ULL << gpio_pin); // Seleccionar el pin
        io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;  // No habilitar pull-down
        io_conf.pull_up_en = GPIO_PULLUP_DISABLE;      // No habilitar pull-up
        gpio_config(&io_conf);
    }


    esp_err_t ret = adc_oneshot_config_channel(adc_hdlr, channel, &chan_cfg);
    if (ret != ESP_OK) {
        return ret; // Devuelve error si la configuración del canal falla
    }

    return ESP_OK; // Configuración exitosa
}

void therm_up(therm_t thermistor){
    ESP_ERROR_CHECK(gpio_set_level(thermistor.gpio_pin, 1));
}

void therm_down(therm_t thermistor){
    ESP_ERROR_CHECK(gpio_set_level(thermistor.gpio_pin, 0));
}

 // termistor 1
 therm_t t1;
 //ESP_ERROR_CHECK(therm_config(&t1, ADC_CHANNEL_6)); ESSTO VA AL SENSOR.C


 //...
 // Lecturas
  float therm_read_v(therm_t t1){
    uint16_t raw = therm_read_lsb(t1);
    return ((raw) * 3.3f / 4095.0f);
 }

 float therm_read_t( therm_t t1){
    float v = therm_read_v(t1);
    float r_ntc = SERIES_RESISTANCE * (3.3 - v) / v;
	float t_kelvin = 1.0f / (1.0f / NOMINAL_TEMPERATURE + (1.0f / BETA_COEFFICIENT) * log(r_ntc / NOMINAL_RESISTANCE));
	// Resultado en grados centígrados
    return(t_kelvin - 273.15f); 
 }

 uint16_t therm_read_lsb(therm_t t1){
    int raw_value = 0;
    ESP_ERROR_CHECK(adc_oneshot_read(adc_hdlr, t1.adc_channel, &raw_value));
    return raw_value;
 }
