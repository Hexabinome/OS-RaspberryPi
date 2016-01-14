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
	while ((!(current_block->is_free)) || current_block->size < size) // iterate on existing blocks, while not accurate
	{
		if (current_block == NULL)
		{
			return NULL; // no block found
		}
		current_block = current_block->next;
	}


	if (current_block->is_free && current_block->size >= size)
	{
		return current_block;
	}

	// No free block
	return NULL;
}

static void split_block(struct heap_block* block, unsigned int size)
{
	struct heap_block* new_second_block = (struct heap_block*) (((uint32_t) block) + size + HEAP_BLOCK_SIZE);
	new_second_block->is_free = TRUE;
	new_second_block->size = (block->size + HEAP_BLOCK_SIZE) - (size + HEAP_BLOCK_SIZE); // TODO correct ?
	new_second_block->next = block->next;
	new_second_block->previous = block;
	if (new_second_block->next != NULL)
	{
		new_second_block->next->previous = new_second_block;
	}
	block->size = size;
	block->next = new_second_block;
}

static void occupy_block_and_split_if_necessary(struct heap_block* block, unsigned int size)
{
	if (block->size > size)
	{
		split_block(block, size);
	}

	block->is_free = FALSE;
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
	
	occupy_block_and_split_if_necessary(new_block, size);

	// Meta data of block is stored at first position. block* + 1 => the first available address for the user
	return (new_block+1);
}

void* grealloc(void* ptr, unsigned int size)
{
	// Cases like real realloc implementation
	if (ptr == NULL)
	{
		return gmalloc(size);
	}
	if (size == NULL && ptr != NULL)
	{
		gfree(ptr);
		return NULL;
	}
	
	
	struct heap_block* block = ((struct heap_block*) ptr) - 1;
	if (block->size >= size) // has already requested size
	{
		return ptr;
	}
	
	// *** Normal behavior
	
	// If following block is free and together they are big enough
	// (it is not possible that two free blocks follow each other,
	// that's why we don't test if more than one block is free after the current one)
	if (block->next != NULL && block->next->is_free && block->next->size + block->size >= size)
	{
		// Fusion those two blocks
		block->size += block->next->size; // Size
		if (block->next->next != NULL) // Set previous of block further away, if exists
		{
			block->next->next->previous = block;
		}
		block->next = block->next->next; // Set next block of current
		
		split_block(block, size);
		
		return ptr;
	}
	else
	{
		struct heap_block* free_block = find_free_block(size);
		if (free_block == NULL)
		{
			return NULL; // failed to find block
		}
		
		// The found block is too big, split it
		occupy_block_and_split_if_necessary(free_block, size);
		
		// Copy content of old block to new one
		uint32_t i;
		uint32_t content;
		for (i = 0; i <= block->size; ++i)
		{
			content = ((uint32_t*) ptr)[i];
			((uint32_t*) (free_block+1))[i] = content;
		}
		
		// Free old block
		gfree(ptr);
		
		return free_block + 1;
	}
		
	PANIC();
	
}

void gfree(void* addr)
{
	ASSERT(addr >= 0); // address must be positive

	struct heap_block* block = ((struct heap_block*) addr) - 1;

	ASSERT(!(block->is_free)); // block must be taken

	block->is_free = TRUE;
	
	struct heap_block* previous_block = block->previous;
	while (previous_block != NULL && previous_block->is_free)
	{
		block = previous_block;
		previous_block = block->previous;
	}
	
    // Fusion each block with next on if possible, multiple tries
	struct heap_block* next_block = block->next;
	while (next_block->is_free && next_block->next != NULL)
	{
		block->size += next_block->size;//Metadata of second block are deleted so block->size += (next_block->size + HEAP_BLOCK_SIZE) ?
		block->next = next_block->next;
		block->next->previous = block;
			
		next_block = block->next;
	}
	
	// TODO solution for this current problem :
	// a = galloc();
    // b = galloc();
    // c = galloc();
    // free(a);free(b);free(c); -> no merge
    // IMPORTANT : current_process->heap must always point on heap_block!
    // 1. double chained list (and try to merge with previous block), but possibly no more heap_block on curr_pro->heap
    // 2. when freeing every block with the next one, beginning with the freed one (same pb as above)
    // 3. iterate over whole list at each free, beginning with the first one everytime (works but it cpu consuming)
}
