/*
 * command.c
 *
 *  Created on: 21.04.2017
 *      Author: Björn Rennfanz <bjoern@fam-rennfanz.de>
 *      License: MIT, see LICENSE file for more details.
 */

#include "command.h"
#include "drivers/malloc_static_buffer.h"

#include<string.h>

#define COMMAND_SBMALLOC_HEAP_SIZE 2048
static char commandSbMallocBuffer[COMMAND_SBMALLOC_HEAP_SIZE];
static sbMallocHeap_t commandSbMallocHeap;

#define MSG_OK "OK\r\n"
#define MSG_ERROR "ERROR\r\n"
#define MSG_INVALID_CMD "UNKNOWN COMMAND\r\n"

void command_init()
{
	// Initialize private command heap
	sbMallocInit(&commandSbMallocHeap, &commandSbMallocBuffer[0], COMMAND_SBMALLOC_HEAP_SIZE);
	printf("command: Parser initialized\n");
}

char **command_parse_args(char *buf, uint8_t *argc)
{
	const char delim[] = " \t";
	char *save, *tok;
	char **argv = (char **)sbMalloc(&commandSbMallocHeap, sizeof(char *));

	*argc = 0;
	for (; *buf == ' ' || *buf == '\t'; ++buf); // absorb leading spaces
	for (tok = strtok_r(buf, delim, &save); tok; tok = strtok_r(NULL, delim, &save)) 
	{
		argv[*argc] = strdup(tok);
		argv = (char **)sbRealloc(&commandSbMallocHeap, argv, (*argc + 1) * sizeof(char *));
		(*argc)++;
	}

	return argv;
}

void command_parse_args_free(uint8_t argc, char *argv[]) 
{
	uint8_t i;
	for (i = 0; i <= argc; ++i) 
	{
		if (argv[i])
		{
			sbFree(&commandSbMallocHeap, argv[i]);
		}
	}

	sbFree(&commandSbMallocHeap, argv);
}

void command_parse_line(struct tcp_pcb *pcb, uint8_t *buf, uint16_t len)
{
	char *lbuf = (char *)sbMalloc(&commandSbMallocHeap, len + 1), **argv;
	uint8_t i, argc;
	
	// we need a '\0' end of the string
	memcpy(lbuf, buf, len);
	lbuf[len] = '\0';

	// command echo
	//espbuffsent(conn, lbuf, len);

	// remove any CR / LF
	for (i = 0; i < len; ++i)
		if (lbuf[i] == '\n' || lbuf[i] == '\r')
			lbuf[i] = '\0';

	// verify the command prefix
	if (strncmp(lbuf, "AT+", 3) != 0) 
	{
		printf("%s", lbuf);
		printf("No AT command");
		sbFree(&commandSbMallocHeap, lbuf);
		return;
	}
	// parse out buffer into arguments
	argv = command_parse_args(&lbuf[3], &argc);
	
	printf("Parsed %d arguments", argc);

	command_parse_args_free(argc, argv);
	sbFree(&commandSbMallocHeap, lbuf);
}
