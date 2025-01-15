#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/ringbuf.h>
#include <esp_log.h>

#include <math.h>

#include "config.h"

static const char *TAG = "STF_P1:task_votador";

SYSTEM_TASK(TASK_VOTADOR) {
    TASK_BEGIN();
    ESP_LOGI(TAG, "Task votador running");

    // Desempaquetar argumentos de configuración
    task_votador_args_t* args = (task_votador_args_t*) TASK_ARGS;
    RingbufHandle_t* rbuf_read = args->rbuf_read;
    RingbufHandle_t* rbuf_write = args->rbuf_write;
    uint16_t mask = args->mask;

    void *ptr_receive = NULL;
    void *ptr_send = NULL;
    size_t length;

    float media = 0.0;
    uint16_t R;

    mensaje msg_received;
    mensaje msg_send;
    msg_send.uid = ID_VOTADOR;

    // Loop
    TASK_LOOP() {
        // Recibir datos del buffer del Sensor
        ptr_receive = xRingbufferReceive(*rbuf_read, &length, pdMS_TO_TICKS(1000));

        if (ptr_receive != NULL) {
            
            msg_received = *((mensaje*) ptr_receive);
            ESP_LOGI(TAG, "Mensaje Recibido");
            
            media = (msg_received.s1 + msg_received.s2 + msg_received.s3) / 3.0;

            R = (msg_received.lsb1 & msg_received.lsb2 & mask) |
                (msg_received.lsb2 & msg_received.lsb3 & mask) | 
                (msg_received.lsb1 & msg_received.lsb3 & mask);

            //R = R * 3.3f / 4095.0f;

            msg_send.media = media;
            msg_send.media_raw = R;

            // Log para depuración
            ESP_LOGI(TAG, "Media calculada: %.2f", media);

            // Preparar mensaje para Monitor
            if (xRingbufferSendAcquire(*rbuf_write, &ptr_send, sizeof(mensaje), pdMS_TO_TICKS(100)) != pdTRUE) {
                ESP_LOGW(TAG, "Buffer Monitor lleno. Descartando datos.");
            }
            else 
			{
				// Si xRingbufferSendAcquire tiene éxito, podemos escribir el número de bytes solicitados
				// en el puntero ptr. El espacio asignado estará bloqueado para su lectura hasta que 
				// se notifique que se ha completado la escritura
				memcpy(ptr_send, &msg_send, sizeof(mensaje));

				// Se notifica que la escritura ha completado. 
				xRingbufferSendComplete(*rbuf_write, ptr_send);
			}
            
            // Liberar elemento del buffer
            vRingbufferReturnItem(*rbuf_read, ptr_receive);
        } else {
            ESP_LOGW(TAG, "Esperando datos del Sensor...");
        }
    }

    ESP_LOGI(TAG, "Deteniendo la tarea Comprobador...");
    TASK_END();
}
