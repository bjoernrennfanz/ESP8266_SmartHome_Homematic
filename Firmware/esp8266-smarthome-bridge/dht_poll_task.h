/*
 * dht_poll_task.h
 *
 *  Created on: 04.03.2017
 *      Author: Björn Rennfanz <bjoern@fam-rennfanz.de>
 *      License: MIT, see LICENSE file for more details.
 */

#ifndef DHT_POLL_TASK_H_
#define DHT_POLL_TASK_H_

#include <stdint.h>
#include <queue.h>

// prototype definitions
void dhtMeasurementTask(void *pvParameters);

// transport data structures
struct dht_measurements
{
    int16_t temperature;
    int16_t humidity;
};

// Queue to transfer measurements
QueueHandle_t dht_transfer_queue;

#endif /* DHT_POLL_TASK_H_ */
