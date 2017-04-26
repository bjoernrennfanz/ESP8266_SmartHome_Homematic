/*
 * malloc_static_buffer.h
 *
 *  Created on: 22.04.2017
 *      Author: Bj�rn Rennfanz <bjoern@fam-rennfanz.de>
 *      License: MIT, see LICENSE file for more details.
 *
 *      Remarks: For detailed information about malloc implementation
 *               see http://www.inf.udec.cl/~leo/Malloc_tutorial.pdf
 *
 *               Implementation of brk and sbrk in linux see also
 *               http://read.pudn.com/downloads95/sourcecode/embed/388113/OPEXdemo1/MyMalloc.c__.htm
 */

// #define DEBUG_MEMORY

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "malloc_static_buffer.h"

void malloc_static_buffer_init(malloc_static_buffer_heap_t *heap, char *bufferAddr, uint16_t bufferSize) 
{
	// Get start and end heap address
	heap->malloc_static_buffer_heap_start = bufferAddr;
	heap->malloc_static_buffer_heap_end = heap->malloc_static_buffer_heap_start + bufferSize;
	
	// Set brkval to null
	heap->malloc_static_buffer_brkval = NULL;

	// Reset malloc counter
	heap->malloc_static_buffer_count = 0;
}

void malloc_static_buffer_split_block(malloc_static_buffer_heap_t *heap, malloc_static_buffer_block_t b, uint16_t s)
{
	malloc_static_buffer_block_t n;
	
	// Setup new block
	n = (malloc_static_buffer_block_t)(b->data + s);
	n->size = b->size - s - MALLOC_STATIC_BUFFER_BLOCKSIZE;
	n->next = b->next;
	n->prev = b;
	n->free = true;
	n->ptr = n->data;
	
	// Change old block
	b->size = s;
	b->next = n;
	
	// Check if new next
	if (n->next)
	{
		// Set previous block new
		n->next->prev = n;
	}
}

malloc_static_buffer_block_t malloc_static_buffer_find_block(malloc_static_buffer_heap_t *heap, malloc_static_buffer_block_t *last, uint16_t s)
{
	malloc_static_buffer_block_t b = heap->malloc_static_buffer_base;
	
	// Try to find a free block matching block
	while(b && !(b->free && (b->size >= s)))
	{
		// Iterate thought linked list
		*last = b;
		b = b->next;
	}
	
	return b;
}

malloc_static_buffer_block_t malloc_static_buffer_extend_heap(malloc_static_buffer_heap_t *heap, malloc_static_buffer_block_t last, uint16_t s)
{
	malloc_static_buffer_block_t b;
	
	// Check if brkval is set
	if (heap->malloc_static_buffer_brkval == NULL)
	{
		// Set brkval to heap start
		heap->malloc_static_buffer_brkval = heap->malloc_static_buffer_heap_start;
	}
	
	// Initialize block
	b = (malloc_static_buffer_block_t)heap->malloc_static_buffer_brkval;
	
	// Check if enough space is left
	if ((heap->malloc_static_buffer_brkval + (MALLOC_STATIC_BUFFER_BLOCKSIZE + s)) > heap->malloc_static_buffer_heap_end)
	{
		// No more heap space, go to die
		return NULL;
	}
	
	// Increment brkval
	heap->malloc_static_buffer_brkval += (MALLOC_STATIC_BUFFER_BLOCKSIZE + s);
	
	// Initialize block structure
	b->size = s;
	b->next = NULL;
	b->prev = last;
	b->ptr = b->data;
	b->free = false;
	
	// Check if last block not null
	if (last)
	{
		last->next = b;
	}
	
	return b;
}

malloc_static_buffer_block_t malloc_static_buffer_fusion_block(malloc_static_buffer_heap_t *heap, malloc_static_buffer_block_t b)
{
	// Check if next block is free
	if (b->next && b->next->free)
	{
		// Copy blocks together and set new next block
		b->size += MALLOC_STATIC_BUFFER_BLOCKSIZE + b->next->size;
		b->next = b->next->next;

		// Check if next block is present
		if(b->next)
		{
			// Set previous block new
			b->next->prev = b;
		}
	}
	
	return b;
}

malloc_static_buffer_block_t malloc_static_buffer_get_block(malloc_static_buffer_heap_t *heap, void *p)
{
	char *tmp;
	tmp = p;

	return (p = tmp -= MALLOC_STATIC_BUFFER_BLOCKSIZE);
}

bool malloc_static_buffer_valid_addr(malloc_static_buffer_heap_t *heap, void *p)
{
	// Check if base address is set
	if(heap->malloc_static_buffer_base)
	{
		// Check if pointer is in correct range
		if((p > (void *)(heap->malloc_static_buffer_base)) && (p < (void *)(heap->malloc_static_buffer_brkval)))
		{
			// Verify that pointer matches block pointer
			return (p == (malloc_static_buffer_get_block(heap, p))->ptr);
		}
	}
	
	return NULL;
}

void malloc_static_buffer_free(malloc_static_buffer_heap_t *heap, void *p)
{
	malloc_static_buffer_block_t b;

	if(malloc_static_buffer_valid_addr(heap, p))
	{
		// Get block from pointer
		b = malloc_static_buffer_get_block(heap, p);
		b->free = true;
		
		// Decrement counter value
		heap->malloc_static_buffer_count--;
		
		// Check if prev block is free
		if(b->prev && b->prev->free)
		{
			// Try to fusion block	
			b = malloc_static_buffer_fusion_block(heap, b->prev);
		}
		
		// Then fusion with next
		if (b->next)
		{
			malloc_static_buffer_fusion_block(heap, b);
		}
		else
		{
			// Free the end of heap
			if(b->prev)
			{
				b->prev->next = NULL;
			}
			else
			{
				// No more blocks
				heap->malloc_static_buffer_base = NULL;
			}
			
			// Free heap memory
			heap->malloc_static_buffer_brkval = (char *)b;
		}
	}
}

void *malloc_static_buffer_malloc(malloc_static_buffer_heap_t *heap, uint16_t size)
{
	malloc_static_buffer_block_t b, last;
	uint16_t s;
	
	// Align size by 4
	s = malloc_static_buffer_align4(size);
	
	// Check if heap was initialized
	if (heap->malloc_static_buffer_base)
	{
		// Find first block
		last = heap->malloc_static_buffer_base;
		b = malloc_static_buffer_find_block(heap, &last, s);
		
		// Check if matching block was found
		if (b)
		{
			// Check if we can split
			if ((b->size - size) >= (MALLOC_STATIC_BUFFER_BLOCKSIZE + 4))
			{
				// Split block
				malloc_static_buffer_split_block(heap, b, s);
			}

			// Mark as non free
			b->free = false;
		}
		else
		{
			// No fitting block, extend the heap
			b = malloc_static_buffer_extend_heap(heap, last, s);
						
			// Check if block could be allocated
			if (!b)
			{				
				return NULL;
			}
		}
	}
	else
	{
		// First time
		b = malloc_static_buffer_extend_heap(heap, NULL, s);
		
		// Check if block could be allocated
		if (!b)
		{				
			return NULL;
		}
		
		// Remember base address
		heap->malloc_static_buffer_base = b;
	}
	
	// Increment counter value
	heap->malloc_static_buffer_count++;
		
	return b->data;
}

uint16_t malloc_static_buffer_get_heap_size(malloc_static_buffer_heap_t *heap)
{
	// Check if malloc was initialized
	if (heap->malloc_static_buffer_brkval)
	{
		// Calculate current amount of used memory
		return (heap->malloc_static_buffer_brkval - heap->malloc_static_buffer_heap_start);
	}
	
	return 0;
}

uint16_t malloc_static_buffer_get_malloc_count(malloc_static_buffer_heap_t *heap)
{
	// Return amount of current allocations
	return heap->malloc_static_buffer_count;
}

void *malloc_static_buffer_realloc(malloc_static_buffer_heap_t *heap, void *p, uint16_t size)
{
	malloc_static_buffer_block_t curBlock, newBlock;
	void *ptrNewBlock;
	uint16_t s;
	
	// Check if pointer is not null
	if (!p)
	{
		// Return new block
		return (malloc_static_buffer_malloc(heap, size));
	}
	
	// Check if pointer is valid
	if(malloc_static_buffer_valid_addr(heap, p))
	{
		// Calculate new block size
		s = malloc_static_buffer_align4(size);
		
		// Get block from pointer
		curBlock = malloc_static_buffer_get_block(heap, p);
		
		// Check if block size matches
		if (curBlock->size >= s)
		{
			// Check if new size matches into current size
			if ((curBlock->size - s) >= ( MALLOC_STATIC_BUFFER_BLOCKSIZE + 4))
			{
				// Split block
				malloc_static_buffer_split_block(heap, curBlock, s);
			}
		}
		else
		{
			// Try fusion with next if possible
			if (curBlock->next && curBlock->next->free && ((curBlock->size + MALLOC_STATIC_BUFFER_BLOCKSIZE + curBlock->next->size) >= s))
			{
				// Fusion block
				malloc_static_buffer_fusion_block(heap, curBlock);
				
				// Check if new size matches into current size
				if ((curBlock->size - s) >= ( MALLOC_STATIC_BUFFER_BLOCKSIZE + 4))
				{
					// Split block
					malloc_static_buffer_split_block(heap, curBlock, s);
				}
			}
			else
			{
				// Good old realloc with a new block
				ptrNewBlock = malloc_static_buffer_malloc(heap, s);
				if (!ptrNewBlock)
				{
					// We�re doomed!
					return NULL;
				}

				// Get block from pointer
				newBlock = malloc_static_buffer_get_block(heap, ptrNewBlock);
				
				// Copy data
				
				memcpy(newBlock->ptr, curBlock->ptr, MIN(curBlock->size, newBlock->size));
				
				// Free the old one
				malloc_static_buffer_free(heap, p);
				
				return ptrNewBlock;
			}
		}
		
		return p;
	}
	
	// We�re doomed!
	return NULL;
}
