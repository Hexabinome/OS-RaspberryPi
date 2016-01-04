#ifndef MALLOC_H
#define MALLOC_H

#include <stdint.h>

struct heap_block
{
	uint32_t size;
	struct heap_block* next;
	struct heap_block* previous;
	uint8_t is_free;
};
#define HEAP_BLOCK_SIZE sizeof(struct heap_block)

void* gmalloc(unsigned int size);
void gfree(void* addr);

#endif // MALLOC_H
