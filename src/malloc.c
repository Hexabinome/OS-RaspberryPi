#include "malloc.h"
#include "sched.h"
#include "config.h"
#include "util.h"

extern struct pcb_s* current_process;

static struct heap_block* get_new_block(struct heap_block* first, uint32_t size)
{
	// TODO what?
}

// Finds the first block which has at least the requested size
static struct heap_block* find_free_block(struct heap_block* first_block, uint32_t size)
{
	struct heap_block* current_block = first_block;
	while ((!(current_block->is_free)) || current_block->size < size || current_block->next != first_block) // iterate on existing blocks, while not accurate
	{
		current_block = current_block->next;
	}
	
	if (current_block->next == first_block) // no free block
	{
		return NULL;
	}
	
	// Found a free block
	return current_block;
}

void* gmalloc(unsigned int size)
{
	struct heap_block* new_block;
	
	if (current_process->heap == NULL) // first call
	{
		new_block = get_new_block(NULL, size);
		if (new_block == NULL) // error
		{
			return NULL;
		}
	}
	else // at least one memory block has already been allocated
	{
		new_block = find_free_block(current_process->heap, size);
		if (new_block == NULL) // no free block found
		{
			// alloc new block
			new_block = get_new_block(current_process->heap, size);
			if (new_block == NULL) // error
			{
				return NULL;
			}
		}
		else // free block found
		{
			// TODO split block in two if block is bigger than needed size
			new_block->is_free = FALSE;	
		}
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
	
	if (!block->is_free) // if not free, error
	{
		PANIC();
	}
	
	block->is_free = TRUE;
}
