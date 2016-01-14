#include <stdint.h>
#include "sched.h"
#include "kheap.h"
#include "config.h"
#include "syscall.h"
#include "vmem.h"
#include "schedulers.h"
#include "sched_svc.h"
#include "fb.h"
#include "hw.h"

struct pcb_s* current_process;
static struct pcb_s kmain_process;

unsigned int nb_process;

static uint32_t pid_counter = 0;


unsigned int MMUTABLEBASE; /* Page table address */

void sched_init()
{
	// Init process data structure
	current_process = &kmain_process;
	current_process->next = current_process;
	current_process->previous = current_process;
	current_process->status = RUNNING;
	current_process->priority = 1; // kmain priority
	current_process->page_table = init_translation_table();
	current_process->heap = NULL;
	current_process->next_waiting_sem = NULL;
	current_process->pid = pid_counter++;
	current_process->ppid = NULL;
	current_process->child = NULL;
	current_process->brother = NULL;
	
	switch_mmu_to(current_process);
	nb_process = 1;
}

static void start_current_process()
{
	current_process->entry();
	sys_exit(0);
}

struct pcb_s* add_process(func_t* entry)
{
	switch_mmu_to_kernel();

	// Alloc pcb
	struct pcb_s* process = (struct pcb_s*)kAlloc(sizeof(struct pcb_s));
	process->entry = entry;
	process->lr_svc = (uint32_t)&start_current_process;

	// Read the execution mode of the process
	__asm("mrs %0, cpsr" : "=r"(process->cpsr_user));
	process->cpsr_user &= 0b1111111111111111111111101111111; // Put interruptions ON

	// Initialize semaphore waiting list
	process->next_waiting_sem = NULL;

	// Initial status
	process->status = READY;
	
	process->pid = pid_counter++;
	process->ppid = current_process->pid;
	
	// If first child
	if (current_process->child == NULL)
	{
		current_process->child = process;
	}
	else // Else, it the brother of the last child
	{
		struct pcb_s* curr_child = current_process->child;
		while (curr_child->brother != NULL)
		{
			curr_child = curr_child->brother;
		}
		curr_child->brother = process;
	}
	process->child = NULL;
	process->brother = NULL;
	
	process->page_table = init_translation_table();

	//going to created process mmu mod to allocate stack
	switch_mmu_to(process);
	
	// Allocate memory (stack + heap)
	process->memory_start = vmem_alloc_for_userland(process, MEMORY_SIZE);
	process->sp_user = (uint32_t*)(process->memory_start + MEMORY_SIZE);
	process->heap = (struct heap_block*) (process->memory_start);
	process->heap->is_free = TRUE;
	process->heap->next = NULL;
	process->heap->previous = NULL;
	process->heap->size = HEAP_SIZE - HEAP_BLOCK_SIZE;
	
	switch_mmu_to(current_process);

	// Put the next process at the end of the list
	struct pcb_s* last_process = current_process;
	while (last_process->next != &kmain_process)
	{
		last_process = last_process->next;
	}
	last_process->next = process;
	process->previous = last_process;
	process->next = &kmain_process;

	++nb_process; // One more process

	return process;
}

void create_process(func_t* entry)
{
	add_process(entry);
}

void create_process_with_fix_priority(func_t* entry, int priority)
{
	struct pcb_s* process = add_process(entry);
	process->priority = priority;
}

void free_process(struct pcb_s* process)
{
	// If process had child, these become orphans
	if (process->child != NULL)
	{
		struct pcb_s* curr_child = process->child;
		while (curr_child != NULL)
		{
			if (curr_child->status == TERMINATED)
			{
				kFree((uint8_t*) process, sizeof(struct pcb_s));
			}
			else
			{
				curr_child->ppid = 0; // so this parent is kmain
			}
			curr_child = curr_child->brother;
		}
	}
	
	if (process->ppid == 0) // if orphan process, cannot become zombie
	{
		kFree((uint8_t*) process, sizeof(struct pcb_s));
	}
	
	switch_mmu_to(process);

	// Free memory
	vmem_free((uint8_t*)process->memory_start, process, MEMORY_SIZE);

	switch_mmu_to_kernel();

	//free page table
	kFree((uint8_t*)process->page_table, FIRST_LVL_TT_SIZE);
	
	switch_mmu_to(current_process);

	--nb_process;
}

void do_sys_exit(uint32_t* sp) // Points on saved r0 in stack
{
	current_process->status = TERMINATED;
	current_process->return_code = *(sp+1);
	
	elect();
	
	context_save_to_pcb_svc(sp);
}
