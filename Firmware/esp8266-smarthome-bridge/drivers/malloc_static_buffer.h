/*
 * malloc_static_buffer.h
 *
 *  Created on: 22.04.2017
 *      Author: Björn Rennfanz <bjoern@fam-rennfanz.de>
 *      License: MIT, see LICENSE file for more details.
 */

#ifndef MALLOC_STATIC_BUFFER_H_
#define MALLOC_STATIC_BUFFER_H_

#include <stdint.h>
#include <stdbool.h>

// Block structure definition
struct malloc_static_buffer_block_s 
{
	// Size of block
	uint16_t size;

	// Double linked list
	struct malloc_static_buffer_block_s *next;
	struct malloc_static_buffer_block_s *prev;
	
	// Reserved byte
	uint8_t reserved;
	
	// Block free flag
	bool free;
	
	// A pointer to the allocated block
	void *ptr;
	char data [1];
};

// Virtual heap structure definition
struct malloc_static_buffer_heap_s
{
	// Definitions for virtual heap implementation
	char *malloc_static_buffer_heap_start;
	char *malloc_static_buffer_heap_end;

	void *malloc_static_buffer_base;
	char *malloc_static_buffer_brkval;

	// Definitions for statistics
	uint16_t malloc_static_buffer_count;
};

// Type definitions
typedef struct malloc_static_buffer_block_s *malloc_static_buffer_block_t;
typedef struct malloc_static_buffer_heap_s malloc_static_buffer_heap_t;

// Define the block size since the sizeof will be wrong
#define MALLOC_STATIC_BUFFER_BLOCKSIZE 20

// Define the MIN and MAX macors
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

// Align value by 4
#define malloc_static_buffer_align4(x) (((((x)-1)>>2)<<2)+4)

// Prototype helpers
void malloc_static_buffer_split_block(malloc_static_buffer_heap_t *heap, malloc_static_buffer_block_t b, uint16_t s);
malloc_static_buffer_block_t malloc_static_buffer_find_block(malloc_static_buffer_heap_t *heap, malloc_static_buffer_block_t *last, uint16_t s);
malloc_static_buffer_block_t malloc_static_buffer_extend_heap(malloc_static_buffer_heap_t *heap, malloc_static_buffer_block_t last , uint16_t s);
malloc_static_buffer_block_t malloc_static_buffer_fusion_block(malloc_static_buffer_heap_t *heap, malloc_static_buffer_block_t b);
malloc_static_buffer_block_t  malloc_static_buffer_get_block(malloc_static_buffer_heap_t *heap, void *p);
bool malloc_static_buffer_valid_addr(malloc_static_buffer_heap_t *heap, void *p);

// Prototypes
void malloc_static_buffer_init(malloc_static_buffer_heap_t *heap, char *bufferAddr, uint16_t bufferSize);
void malloc_static_buffer_free(malloc_static_buffer_heap_t *heap, void *p);
void *malloc_static_buffer_malloc(malloc_static_buffer_heap_t *heap, uint16_t size);
void *malloc_static_buffer_realloc(malloc_static_buffer_heap_t *heap, void *p, uint16_t size);

// Statistics
uint16_t malloc_static_buffer_get_heap_size(malloc_static_buffer_heap_t *heap);
uint16_t malloc_static_buffer_get_malloc_count(malloc_static_buffer_heap_t *heap);

// Short prototypes
#define sbMallocInit malloc_static_buffer_init
#define sbMalloc malloc_static_buffer_malloc
#define sbFree malloc_static_buffer_free
#define sbRealloc malloc_static_buffer_realloc
#define sbMallocHeap_t malloc_static_buffer_heap_t

#endif /* MALLOC_STATIC_BUFFER_H_ */
