/*
 * esp8266-smarthome-bridge.c
 *
 *  Created on: 04.03.2017
 *      Author: Björn Rennfanz <bjoern@fam-rennfanz.de>
 *      License: MIT, see LICENSE file for more details.
 */

#include <stdio.h>
#include <stdlib.h>

#include "espressif/esp_common.h"
#include "esp/uart.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "esp8266.h"

#include "telnet_server_task.h"
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

		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

void user_init(void)
{
    uart_set_baud(0, 115200);
    printf("ESP8266 HW-SDK version: %s\n", sdk_system_get_sdk_version());

    xTaskCreate(dhtProceedValuesTask, "dhtProceedValuesTask", 192, NULL, 2, NULL);
    xTaskCreate(dhtMeasurementTask, "dhtMeasurementTask", 128, NULL, 2, NULL);
    xTaskCreate(telnetd_task, "telnetd_task", 128, NULL, 2, NULL);
}
