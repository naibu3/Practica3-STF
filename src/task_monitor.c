/**********************************************************************
* FILENAME : task_monitor.c       
*
* DESCRIPTION : 
*
* PUBLIC LICENSE :
* Este código es de uso público y libre de modificar bajo los términos de la
* Licencia Pública General GNU (GPL v3) o posterior. Se proporciona "tal cual",
* sin garantías de ningún tipo.
*
* AUTHOR :   Dr. Fernando Leon (fernando.leon@uco.es) University of Cordoba
******************************************************************************/

// libc
#include <time.h>
#include <stdio.h>
#include <sys/time.h>

// freerqtos
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

// esp
#include <esp_system.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_timer.h>

// propias
#include "config.h"

static const char *TAG = "STF_P1:task_monitor";


// Tarea MONITOR
SYSTEM_TASK(TASK_MONITOR)
{
	TASK_BEGIN();
	ESP_LOGI(TAG,"Task Monitor running");

	// Recibe los argumentos de configuración de la tarea y los desempaqueta
	task_monitor_args_t* ptr_args = (task_monitor_args_t*) TASK_ARGS;
	RingbufHandle_t* rbuf = ptr_args->rbuf; 

	// variables para reutilizar en el bucle
	size_t length;
	void *ptr;
	mensaje msg;
	float v1 = 0.0;
	float v2 = 0.0;
	float v3 = 0.0;
	//float deviation = 0.0;
	//float min_val = 0.0;
	//float max_val = 0.0;

	// Loop
	TASK_LOOP()
	{
		// Se bloquea en espera de que haya algo que leer en RingBuffer.
		// Tiene un timeout de 1 segundo para no bloquear indefinidamente la tarea, 
		// pero si expira vuelve aquí sin consecuencias
		ptr = xRingbufferReceive(*rbuf, &length, pdMS_TO_TICKS(1000));

		//Si el timeout expira, este puntero es NULL
		if (ptr != NULL) 
		{
			// Este código se puede usar para notificar cuántos bytes ha recibido del
			// sensor a través de la estructura RingBuffer. 
			//ESP_LOGI(TAG,"Recibidos: %d bytes", length);
			msg = *((mensaje *) ptr);
			
			if (msg.uid == ID_SENSOR){
				v1 = msg.s1;
				v2 = msg.s2;
				v3 = msg.s3;
				ESP_LOGI(TAG, "NORMAL_MODE: T1 = %.5f; T2 = %.5f; T3 = %.5f", v1, v2, v3);
			}

			vRingbufferReturnItem(*rbuf, ptr);
		} 
		else 
		{
			ESP_LOGW(TAG, "Esperando datos ...");
		}
	}
	ESP_LOGI(TAG,"Deteniendo la tarea ...");
	TASK_END();
}
