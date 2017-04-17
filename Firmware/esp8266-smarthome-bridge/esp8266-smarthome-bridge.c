#include <stdio.h>
#include <stdlib.h>

#include "espressif/esp_common.h"
#include "esp/uart.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "esp8266.h"

#include "dht_poll_task.h"


void dhtProceedValuesTask(void *pvParameters)
{
	// Create struct for received values
	struct dht_measurements rx_dht_measurement;

	while(1)
	{
		// Check if transfer queue was allocated
		if( dht_transfer_queue != 0 )
		{
			// Receive a message on the created dht_rx_queue.
			if( xQueueReceive(dht_transfer_queue, (void *)&rx_dht_measurement, (TickType_t)0 ) )
			{
				printf("Humidity: %d%% Temp: %dC\n", rx_dht_measurement.humidity / 10, rx_dht_measurement.temperature / 10);
			}
		}
		// 1500 milli second delay...
		printf("Waiting...\n");
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

void user_init(void)
{
    uart_set_baud(0, 115200);
    xTaskCreate(dhtMeasurementTask, "dhtMeasurementTask", 192, NULL, 2, NULL);

    xTaskCreate(dhtProceedValuesTask, "dhtProceedValuesTask", 192, NULL, 2, NULL);
}
