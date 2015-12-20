#include "malloc.h"
#include "sched.h"
#include "config.h"
#include "util.h"
#include "syscall.h"

extern struct pcb_s* current_process;

// Finds the first block which has at least the requested size
static struct heap_block* find_free_block(uint32_t size)
{
	struct heap_block* current_block = current_process->heap;
	while ((!(current_block->is_free)) || current_block->size < size || current_block->next != current_process->heap) // iterate on existing blocks, while not accurate
	{
		current_block = current_block->next;
	}
	
	if (current_block->is_free && current_block->size >= size)
	{
		
		return current_block;
	}
	
	// No free block
	return NULL;
}

void* gmalloc(unsigned int size)
{
	struct heap_block* new_block;
	
	// Find first free block, which is big enough
	new_block = find_free_block(size);
	if (new_block == NULL) // no free block found
	{
		return NULL;
	}
	
	if (new_block->size == size) // If the block has the exact size
	{
		new_block->is_free = FALSE;
	}
	else // The found block is too big, split it
	{
		struct heap_block* new_second_block = (new_block + size + HEAP_BLOCK_SIZE);
		new_second_block->is_free = TRUE;
		new_second_block->size = (new_block->size + HEAP_BLOCK_SIZE) - (size + HEAP_BLOCK_SIZE); // TODO correct ?
		new_second_block->next = new_block->next;
		
		new_block->is_free = FALSE;
		new_block->size = size;
		new_block->next = new_second_block;
	}
	
	
	// Meta data of block is stored at first position. block* + 1 => the first available address for the user
	return (new_block+1);
}

void gfree(void* addr)
{
	if (addr < 0) // if negative address
	{
		PANIC();
	}
	
	struct heap_block* block = ((struct heap_block*) addr) - 1;
	
	if (block->is_free) // if free, error
	{
		PANIC();
	}
	
	block->is_free = TRUE;
	
	// Fusion with next block if free
	struct heap_block* next_block = block->next;
	if (next_block->is_free && next_block != block)
	{
		block->size += next_block->size;
		block->next = next_block->next;
	}
}
