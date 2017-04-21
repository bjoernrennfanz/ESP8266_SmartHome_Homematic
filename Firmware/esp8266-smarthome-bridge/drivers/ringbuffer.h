/*
 * ringbuffer.h
 *
 *  Created on: 20.04.2017
 *      Author: Björn Rennfanz <bjoern@fam-rennfanz.de>
 *      License: MIT, see LICENSE file for more details.
 */

#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_

#include <stdint.h>
#include <stdbool.h>

// Ring buffer structures
typedef struct ring_buffer
{
	uint8_t *buffer;
	uint16_t size;
	
	uint16_t read;
	uint16_t write;
	uint16_t count;
	
} ring_buffer_t;

// Ring buffer prototypes
void ringBuffer_Init(ring_buffer_t *pBuffer, uint8_t* buffer, uint16_t bufferSize);
uint8_t ringBuffer_Peek(ring_buffer_t *pBuffer);
uint8_t ringBuffer_Read(ring_buffer_t *pBuffer);
void ringBuffer_Write(ring_buffer_t *pBuffer, uint8_t bufferValue);
bool ringBuffer_IsFull(ring_buffer_t *pBuffer);
bool ringBuffer_IsEmpty(ring_buffer_t *pBuffer);
uint16_t ringBuffer_GetLevel(ring_buffer_t *pBuffer);
uint16_t ringBuffer_GetFree(ring_buffer_t *pBuffer);

#endif /* RINGBUFFER_H_ */
