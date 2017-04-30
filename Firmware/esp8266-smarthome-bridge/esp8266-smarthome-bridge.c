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

#include "drivers\cc1101.h"

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

void cc1101ProceedTask(void *pvParameters)
{
	uint8_t tmpCCBurst = 0, chkCCBurst = 0;
	uint8_t ccBuffer[60];

	// Create struct for received values
	cc1101_module_t module = {
		.crc_ok = 0,
		.rssi = 0,
		.lqi = 0,
		.cs_pin = 16
	};

	// Initialize CC1101
	cc1101_init(&module);

	while (1)
	{
		tmpCCBurst = cc1101_detect_burst(&module);
		if ((tmpCCBurst) && (!chkCCBurst)) 
		{			
			// burst detected for the first time
			chkCCBurst = 1;	// set the flag
		} 
		else
		{
			if ((tmpCCBurst) && (chkCCBurst))
			{		// burst detected for the second time
				chkCCBurst = 0;	// reset the flag
				printf("Burst detected!\n");
			}
			else
			{
				if ((!tmpCCBurst) && (chkCCBurst))
				{		// secondary test was negative, reset the flag
					chkCCBurst = 0;	// reset the flag	
				}
			}
		}

		// if (!chkCCBurst) vTaskDelay(128 / portTICK_PERIOD_MS);
		if (chkCCBurst) vTaskDelay(32 / portTICK_PERIOD_MS);

		uint8_t num = cc1101_rcv_data(&module, &ccBuffer[0]);
		if (num)
		{
			printf("%d Bytes read, CRC %s, RSSI %d, Link quality %d\n", num, module.crc_ok ? "Ok" : "NOk", module.rssi, module.lqi);
		}
	}
}

void user_init(void)
{
    uart_set_baud(0, 115200);
    printf("ESP8266 HW-SDK version: %s\n", sdk_system_get_sdk_version());

    struct sdk_station_config config = {
        .ssid = "SEWB113",
        .password = "palmm100",
    };

    /* required to call wifi_set_opmode before station_set_config */
    sdk_wifi_set_opmode(STATION_MODE);
    sdk_wifi_station_set_config(&config);
    sdk_wifi_station_connect();

    xTaskCreate(dhtProceedValuesTask, "dhtProceedValuesTask", 192, NULL, 2, NULL);
    xTaskCreate(dhtMeasurementTask, "dhtMeasurementTask", 128, NULL, 2, NULL);
	xTaskCreate(cc1101ProceedTask, "cc1101ProceedTask", 256, NULL, 2, NULL);
    // xTaskCreate(telnetd_task, "telnetd_task", 512, NULL, 2, NULL);
}