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
#include "homematic\thsensor.h"
#include "drivers\cc1101.h"

#include "dht_poll_task.h"

static uint8_t dht11Temperature;
static uint8_t dht11Humidity; 
static uint8_t tadoTemperature;
static uint8_t tadoHumidity;

void MeasureDht11Values()
{
	// Check if transfer queue was allocated
	if( dht_transfer_queue != 0 )
	{
		struct dht_measurements dht11_measurement;

		// Receive a message on the created dht_rx_queue.
		if( xQueueReceive(dht_transfer_queue, (void *)&dht11_measurement, (TickType_t)0 ) )
		{
			dht11Humidity = (uint8_t)(dht11_measurement.humidity / 10);
			dht11Temperature = (uint8_t)(dht11_measurement.temperature / 10);

			printf("DHT11: Humidity: %d%% Temp: %dC\n", dht11Humidity, dht11Temperature);
		}
	}
}

void MeasureTadoValues()
{

}

void homematicProceedTask(void *pvParameters)
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

	// Initialize virtual TADO sensor
	thsensor_handle_t tadoThSensor = thsensor_construct();
	thsensor_init(tadoThSensor, MeasureTadoValues, &tadoTemperature, &tadoHumidity);
	thsensor_homematic_register(tadoThSensor, 0, 0, homematicInstance);

	thsensor_handle_t dht11ThSensor = thsensor_construct();
	thsensor_init(dht11ThSensor, MeasureDht11Values, &dht11Temperature, &dht11Humidity);
	thsensor_homematic_register(dht11ThSensor, 0, 0, homematicInstance);

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

    xTaskCreate(dhtMeasurementTask, "dhtMeasurementTask", 128, NULL, 2, NULL);
	xTaskCreate(homematicProceedTask, "homematicProceedTask", 256, NULL, 2, NULL);
}