/*
 * command.h
 *
 *  Created on: 21.04.2017
 *      Author: Björn Rennfanz <bjoern@fam-rennfanz.de>
 *      License: MIT, see LICENSE file for more details.
 */

#ifndef COMMAND_H_
#define COMMAND_H_

#include <stdint.h>
#include <lwip/tcp.h>

// Command structure definition
typedef struct command
{
	char *commandName;
	void(*commandFunction)(struct tcp_pcb *pcb, uint8_t argc, char *argv[]);
} command_t;

// Command prototypes
void command_init();
void command_parse_line(struct tcp_pcb *pcb, uint8_t *buf, uint16_t len);

#endif /* COMMAND_H_ */
