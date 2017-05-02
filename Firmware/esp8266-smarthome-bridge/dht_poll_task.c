/*
 * dht_poll_task.c
 *
 *  Created on: 04.03.2017
 *      Author: Bjoern Rennfanz <bjoern@fam-rennfanz.de>
 *      License: MIT, see LICENSE file for more details.
 */

#include <stdio.h>
#include <stdlib.h>

#include <FreeRTOS.h>
#include <task.h>

#include <esp8266.h>
#include <dht/dht.h>
#include "dht_poll_task.h"

// Measure on GPIO4 with an DHT11
const dht_sensor_type_t sensor_type = DHT_TYPE_DHT11;
uint8_t const dht_gpio = 4;

void dhtMeasurementTask(void *pvParameters)
{
	struct dht_measurements cur_dht_measurement;

    // DHT sensors that come mounted on a PCB generally have
    // pull-up resistors on the data pin.  It is recommended
    // to provide an external pull-up resistor otherwise...
    gpio_set_pullup(dht_gpio, false, false);

    // Initialize transfer queue
    dht_transfer_queue = xQueueCreate( 1, sizeof(struct dht_measurements));
    if (dht_transfer_queue == 0)
    {
    	// Terminate task, queue is needed
    	vTaskDelete( NULL );
    }
    else
    {
		while(1)
		{
			// Read data packet from sensor
			if (dht_read_data(sensor_type, dht_gpio, &cur_dht_measurement.humidity, &cur_dht_measurement.temperature))
			{
				// Transfer to consume thread
				xQueueSend(dht_transfer_queue, (void *)&cur_dht_measurement, (TickType_t)0);
			}

			// Three second delay...
			vTaskDelay(3000 / portTICK_PERIOD_MS);
		}
    }
}
