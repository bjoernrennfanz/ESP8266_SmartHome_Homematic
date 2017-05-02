/*
 * ringbuffer.c
 *
 *  Created on: 20.04.2017
 *      Author: Bjoern Rennfanz <bjoern@fam-rennfanz.de>
 *      License: MIT, see LICENSE file for more details.
 */
 
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "ringbuffer.h"

void ringBuffer_Init(ring_buffer_t *pBuffer, uint8_t* buffer, uint16_t bufferSize)
{
	// Save pointers
	pBuffer->buffer = buffer;
	pBuffer->size = bufferSize;
	
	// Reset counters
	pBuffer->count = 0;
	pBuffer->read = 0;
	pBuffer->write = 0;
}

uint8_t ringBuffer_Peek(ring_buffer_t *pBuffer)
{
	// Return head value
	return pBuffer->buffer[pBuffer->read];
}

uint8_t ringBuffer_Read(ring_buffer_t *pBuffer)
{
	// Get value from buffer
	uint8_t bufferValue = pBuffer->buffer[pBuffer->read];
	pBuffer->read = ((pBuffer->read + 1) & (pBuffer->size - 1));
	pBuffer->count--;
	
	// Return value
	return bufferValue;
}

void ringBuffer_Write(ring_buffer_t *pBuffer, uint8_t bufferValue)
{
	// Compute next buffer position and save value
	uint16_t next = ((pBuffer->write + 1) & (pBuffer->size - 1));
	pBuffer->buffer[pBuffer->write] = bufferValue;
	pBuffer->write = next;
	pBuffer->count++;
}

bool ringBuffer_IsFull(ring_buffer_t *pBuffer)
{
	// Compute next buffer position and check if same as read
	uint16_t next = ((pBuffer->write + 1) & (pBuffer->size - 1));
	if (pBuffer->read == next)
	{
		return true;
	}
	
	return false;
}

bool ringBuffer_IsEmpty(ring_buffer_t *pBuffer)
{
	// Check if read and write are the same
	if (pBuffer->read == pBuffer->write)
	{
		return true;
	}
	
	return false;
}

uint16_t ringBuffer_GetLevel(ring_buffer_t *pBuffer)
{
	// Return bytes count in buffer
	return pBuffer->count;
}

uint16_t ringBuffer_GetFree(ring_buffer_t *pBuffer)
{
	// Return bytes count in buffer
	return pBuffer->size - pBuffer->count;
}
