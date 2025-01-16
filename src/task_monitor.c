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
#include "term.h"

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
	float lsb1 = 0.0;
	float lsb2 = 0.0;
	float lsb3 = 0.0;
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
			
			msg = *((mensaje *) ptr);

			if (msg.uid == ID_VOTADOR){
				lsb1 = msg.lsb1;
				lsb2 = msg.lsb2;
				lsb3 = msg.lsb3;
				
				
				
				// Muestra las temperaturas de los tres termistores
				ESP_LOGI(TAG, "NORMAL_MODE: T1 = %.5f; T2 = %.5f; T3 = %.5f", convert_lsb_t(lsb1),
																				convert_lsb_t(lsb2),
																				convert_lsb_t(lsb3));

				// Muestra la media convertida a grados centigrados
				ESP_LOGI(TAG, "NORMAL_MODE: Media = %.5f", convert_lsb_t(msg.media_raw) );
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
