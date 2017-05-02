/*
 * esp8266-smarthome-bridge.c
 *
 *  Created on: 04.03.2017
 *      Author: Bjoern Rennfanz <bjoern@fam-rennfanz.de>
 *      License: MIT, see LICENSE file for more details.
 */

#include <stdio.h>
#include <stdlib.h>

#include "espressif/esp_common.h"
#include "esp/uart.h"

#include "FreeRTOS.h"
#include "esp8266.h"
#include "queue.h"
#include "task.h"

#include "homematic\homematic.h"
#include "drivers\cc1101.h"
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
				printf("DHT11: Humidity: %d%% Temp: %dC\n", rx_dht_measurement.humidity / 10, rx_dht_measurement.temperature / 10);
			}
		}

		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

void cc1101ProceedTask(void *pvParameters)
{
	// Create struct for module config values
	cc1101_driver_config_t module_config = {
		.crc_ok = 0,
		.rssi = 0,
		.lqi = 0,
		.cs_pin = 16
	};

	// Create struct for module callbacks
	cc1101_driver_t module_driver = {
		.set_idle = cc1101_set_idle,
		.detect_burst = cc1101_detect_burst,
		.init = cc1101_init,
		.snd_data = cc1101_snd_data,
		.rcv_data = cc1101_rcv_data
	};

	// Initialize homematic
	homematic_handle_t homematicInstance = homematic_construct();
	homematic_init(homematicInstance, &module_driver, &module_config);

	while (1)
	{
		// Handle homematic communication
		homematic_poll(homematicInstance);
	}
}

void user_init(void)
{
    uart_set_baud(0, 115200);
    printf("ESP8266 HW-SDK version: %s\n", sdk_system_get_sdk_version());

    xTaskCreate(dhtProceedValuesTask, "dhtProceedValuesTask", 192, NULL, 2, NULL);
    xTaskCreate(dhtMeasurementTask, "dhtMeasurementTask", 128, NULL, 2, NULL);
	xTaskCreate(cc1101ProceedTask, "cc1101ProceedTask", 256, NULL, 2, NULL);
}