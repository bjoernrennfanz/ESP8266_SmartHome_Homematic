/*
 * telnetd.h
 *
 *  Created on: 17.04.2017
 *      Author: Björn Rennfanz <bjoern@fam-rennfanz.de>
 *      License: MIT, see LICENSE file for more details.
 */

#ifndef __TELNET_SERVER_H__
#define __TELNET_SERVER_H__

#include <stdbool.h>
#include <stdint.h>

#include <lwip/err.h>
#include <lwip/pbuf.h>
#include <lwip/tcp.h>

// Define maximum simultaneous connections
#define TELNETD_MAX_CONN		5

// Define maximum send buffer len
#define TELNETD_MAX_TXBUFFER 	1024

// Structure for client connection state
struct telnet_client_state
{
	struct tcp_pcb *pcb;
	uint8_t *txbuffer; // the buffer for the data to send
	uint16_t txbufferlen; // the length of data in txbuffer
	uint16_t txbuffersent; // the number of sent bytes in txbuffer.
	bool readytosend; //true, if txbuffer can send by espconn_sent
	uint8_t retries;
};

// Typedefs for callbacks of telnetd
typedef void (*tTcHandler)(struct tcp_pcb *pcb, uint8_t *data, uint16_t data_len);
typedef void (*tTcOpenHandler)(struct tcp_pcb *pcb);

/**
 * Write data to a connected telnet client.
 *
 * @param pcb tcp_pcb to send.
 * @param data data to send.
 * @param len data length.
 * @return ERR_OK if write succeeded.
 */
err_t telnetd_client_write(struct tcp_pcb *pcb, const uint8_t *data, uint16_t len);

/**
 * Register websocket callback functions. Use NULL if callback is not needed.
 *
 * @param tc_open_cb called when new connection is opened.
 * @param tc_cb called when data is received from client.
 */
void telnetd_register_callbacks(tTcOpenHandler tc_open_cb, tTcHandler tc_cb);

/**
 * Initialize telnet server instance
 *
 * @param port listening port of telnet server instance.
 */
void telnetd_init(uint16_t port);

#endif
