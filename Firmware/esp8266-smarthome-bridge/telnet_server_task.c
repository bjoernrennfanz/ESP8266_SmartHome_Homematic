/*
 * telnet_server_task.h
 *
 *  Created on: 17.04.2017
 *      Author: Björn Rennfanz <bjoern@fam-rennfanz.de>
 *      License: MIT, see LICENSE file for more details.
 */

#include <stdio.h>
#include <stdlib.h>

#include <FreeRTOS.h>
#include <task.h>

#include "telnet_server_task.h"
#include "telnetd.h"

/**
 * This function is called when telnetd receives data from client.
 *
 * Note: this function is executed on TCP thread and should return as soon
 * as possible.
 */
void telnetd_cb(struct tcp_pcb *pcb, uint8_t *data, uint16_t data_len)
{
    printf("[telnetd_receive_cb]:\n%.*s\n", (int) data_len, (char*) data);

    // Loop back data
    telnetd_client_write(pcb, data, data_len);
}

void telnetd_task(void *pvParameters)
{
    /*
     * Register handlers and start the server
     */
    telnetd_register_callbacks(NULL, (tTcHandler) telnetd_cb);
    telnetd_init(23);

    for (;;);
}
