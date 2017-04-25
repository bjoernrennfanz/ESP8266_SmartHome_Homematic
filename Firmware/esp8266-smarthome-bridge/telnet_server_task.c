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
#include "command.h"

/**
 * This function is called when telnetd receives data from client.
 *
 * Note: this function is executed on TCP thread and should return as soon
 * as possible.
 */
void telnetd_cb(struct tcp_pcb *pcb, uint8_t *data, uint16_t data_len)
{
    printf("[telnetd_receive_cb]:\nread %u - %s\n", (int) data_len, (char*) data);
	command_parse_line(pcb, data, data_len);

    // Loop back data
    telnetd_client_write(pcb, data, data_len);
}

/**
 * This function is called when telnetd client is connected.
 */
void telnetd_open_cb(struct tcp_pcb *pcb, uint8_t client_index)
{
	printf("telnetd: Client on slot %u connected.\n", client_index);
}

void telnetd_task(void *pvParameters)
{
	/*
	 * Initialize command parser
	 */
	command_init();

    /*
     * Register handlers and start the server
     */
    telnetd_register_callbacks((tTcOpenHandler) telnetd_open_cb, (tTcHandler) telnetd_cb);
    telnetd_init(23);

    for (;;);
}