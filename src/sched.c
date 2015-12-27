#include <stdint.h>
#include "sched.h"
#include "kheap.h"
#include "config.h"
#include "hw.h"
#include "asm_tools.h"
#include "syscall.h"
#include "vmem.h"
#include "schedulers.h"

#define MEMORY_SIZE 1000000 // 1MB
#define HEAP_SIZE 900000 // 9KB
#define STACK_SIZE 100000 // 1KB

struct pcb_s* current_process;
static struct pcb_s kmain_process;

unsigned int nb_process;

static uint32_t pid_counter = 0;

unsigned int MMUTABLEBASE; /* Page table address */

void sched_init()
{
	// Init virtual memory
#if VMEM
	vmem_init();
#else
	kheap_init();
#endif

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
	
	// Invalidate TLB entries
	INVALIDATE_TLB();
	// Kernel MMU mod
	configure_mmu_C((uint32_t) current_process->page_table);
	nb_process = 1;
}

void irq_init()
{
	timer_init();
	ENABLE_IRQ();
}

static void start_current_process()
{
	current_process->entry();
	sys_exit(0);
}

static struct pcb_s* add_process(func_t* entry)
{
	// Invalidate TLB entries
	INVALIDATE_TLB();
	// Kernel MMU mod
	configure_mmu_kernel();

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
	
	process->page_table = init_translation_table();

	//going to created process mmu mod to allocate stack
	// Invalidate TLB entries
	INVALIDATE_TLB();
	//process MMU mod
	configure_mmu_C((uint32_t)process->page_table);
	// Allocate memory (stack + heap)
	process->memory_start = vmem_alloc_for_userland(process, MEMORY_SIZE);
	process->sp_user = (uint32_t*)(process->memory_start + MEMORY_SIZE);
	process->heap = (struct heap_block*) (process->memory_start);
	process->heap->is_free = TRUE;
	process->heap->next = process->heap;
	process->heap->size = HEAP_SIZE - HEAP_BLOCK_SIZE;
	// Invalidate TLB entries
	INVALIDATE_TLB();
	// Current process MMU mod
	configure_mmu_C((uint32_t)current_process->page_table);

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
	// Invalidate TLB entries
	INVALIDATE_TLB();
	// Process to delete MMU mod
	configure_mmu_C((uint32_t)process->page_table);

	// Free memory
	vmem_free((uint8_t*)process->memory_start, process, MEMORY_SIZE);

	// Invalidate TLB entries
	INVALIDATE_TLB();
	// Kernel MMU mod
	configure_mmu_kernel();

	//free page table
	kFree((uint8_t*)process->page_table, FIRST_LVL_TT_SIZE);
	//free pcb
	kFree((uint8_t*)process, sizeof(struct pcb_s));

	// Invalidate TLB entries
	INVALIDATE_TLB();
	// Current process MMU mod
	configure_mmu_C((uint32_t)current_process->page_table);

	--nb_process;
}

void do_sys_yieldto(uint32_t* sp) // Points on saved r0 in stack
{
	struct pcb_s* dest = (struct pcb_s*)(*(sp+1));

	__asm("mrs %0, spsr" : "=r"(current_process->cpsr_user)); // Save user CPSR, which has been copied at swi in SPSR_svc

	__asm("cps 0b11111"); // System mode
	__asm("mov %0, lr" : "=r"(current_process->lr_user)); // Save lr_user
	__asm("mov %0, sp" : "=r"(current_process->sp_user)); // Save sp_user
	__asm("cps 0b10011"); // SVC mode

	int i;
	for (i = 0; i < NBREG; ++i)
	{
		current_process->registers[i] = sp[i];
		// Update context in stack
		sp[i] = dest->registers[i];
	}
	current_process->lr_svc = sp[NBREG];
	sp[NBREG] = dest->lr_svc;

	current_process = dest;

	__asm("cps 0b11111"); // System mode
	__asm("mov lr, %0" : : "r"(current_process->lr_user));
	__asm("mov sp, %0" : : "r"(current_process->sp_user));
	__asm("cps 0b10011"); // SVC mode

	__asm("msr spsr, %0" : : "r"(current_process->cpsr_user));

}

static void context_load_from_pcb_svc(uint32_t* sp)
{
	// Save current context
	__asm("cps 0b11111"); // System mode
	__asm("mov %0, lr" : "=r"(current_process->lr_user)); // Save lr_user
	__asm("mov %0, sp" : "=r"(current_process->sp_user)); // Save sp_user
	__asm("cps 0b10011"); // SVC mode

	__asm("mrs %0, spsr" : "=r"(current_process->cpsr_user)); // Save user CPSR, which has been copied at swi in SPSR_svc

	uint8_t i;
	for (i = 0; i < NBREG; ++i)
	{
		current_process->registers[i] = sp[i];
	}
	current_process->lr_svc = sp[NBREG];
}

static void context_save_to_pcb_svc(uint32_t* sp)
{
	uint8_t i;
	// Update context which will be reloaded
	for (i = 0; i < NBREG; ++i)
	{
		sp[i] = current_process->registers[i];
	}
	sp[NBREG] = current_process->lr_svc;

	__asm("cps 0b11111"); // System mode
	__asm("mov lr, %0" : : "r"(current_process->lr_user));
	__asm("mov sp, %0" : : "r"(current_process->sp_user));
	__asm("cps 0b10011"); // SVC mode

	__asm("msr spsr, %0" : : "r"(current_process->cpsr_user));
}

void do_sys_yield(uint32_t* sp) // Points on saved r0 in stack
{
	context_load_from_pcb_svc(sp);

	// Elects new current process
	elect();

	context_save_to_pcb_svc(sp);
}

void do_sys_exit(uint32_t* sp) // Points on saved r0 in stack
{
	current_process->status = TERMINATED;
	current_process->return_code = *(sp+1);

	elect();

	context_save_to_pcb_svc(sp);
}

static void context_save_to_pcb_irq(uint32_t* sp) // Points to the beginning of process context in stack
{
	__asm("cps 0b11111"); // System mode
	__asm("mov %0, lr" : "=r"(current_process->lr_user)); // Save lr_user
	__asm("mov %0, sp" : "=r"(current_process->sp_user)); // Save sp_user
	__asm("cps 0b10010"); // IRQ mode

	__asm("mrs %0, spsr" : "=r"(current_process->cpsr_user)); // Save user CPSR, which has been copied at swi in SPSR_svc

	int i;
	for (i = 0; i < NBREG; ++i)
	{
		current_process->registers[i] = sp[i];
	}
	current_process->lr_svc = sp[NBREG];
}

static void context_load_from_pcb_irq(uint32_t* sp) // Points to the beginning of process context in stack
{
	int i;
	// Update context which will be reloaded
	for (i = 0; i < NBREG; ++i)
	{
		sp[i] = current_process->registers[i];
	}
	sp[NBREG] = current_process->lr_svc;


	__asm("cps 0b11111"); // System mode
	__asm("mov lr, %0" : : "r"(current_process->lr_user));
	__asm("mov sp, %0" : : "r"(current_process->sp_user));
	__asm("cps 0b10010"); // IRQ mode

	__asm("msr spsr, %0" : : "r"(current_process->cpsr_user));

}

static void handle_irq(uint32_t* sp)
{
	context_save_to_pcb_irq(sp);

	elect(); // calls current scheduler method

	// Set translation table
	// Invalidate TLB entries
	INVALIDATE_TLB();
	// Current process MMU mod
	configure_mmu_C((uint32_t)current_process->page_table);


	context_load_from_pcb_irq(sp);
}

void __attribute__((naked)) irq_handler()
{
	// Already in IRQ mode. So the old cpsr has been copied into spsr_irq. I bit of cpsr already to 1 (deactivated)

	// Save registers and return address (decremented by 4, see Encart 13 + p.18 the "2; il copie...")
	// Save return address lr because there is a function call in this function
	__asm("sub lr, #4");
	__asm("stmfd sp!, {r0-r12, lr}");

	// Load right handler PC
	// TODO if (timerInterrupt) handle_irq(); ??????
	uint32_t* sp;
	__asm("mov %0, sp" : "=r"(sp));
	handle_irq(sp);

	// Re-arm timer
	set_next_tick_default();
    ENABLE_TIMER_IRQ();

	// Reactivate interruptions (I bit to 0)
	ENABLE_IRQ();
	
	// Reload registers, put return address (lr_irq) into pc to get back to interrupted code, and load spsr => change back CPU mode
	__asm("ldmfd sp!, {r0-r12, pc}^");
}

void do_sys_fork(uint32_t* sp)
{
	context_load_from_pcb_svc(sp); // update current process sp and lr
	register struct pcb_s* new_process = add_process(current_process->entry);

	uint32_t i;
	// Copy register content
	for (i = 0; i < NBREG; ++i)
	{
		new_process->registers[i] = current_process->registers[i];
	}
	new_process->lr_user = current_process->lr_user;
	new_process->priority = current_process->priority;
	new_process->cpsr_user = current_process->cpsr_user;
	new_process->lr_svc = current_process->lr_svc; // The new process must begin here (when coming from an IRQ interrupt or yield system call
	new_process->sp_user = (uint32_t*) (current_process->memory_start + STACK_SIZE - (STACK_SIZE - ((uint32_t) current_process->sp_user - current_process->memory_start)));
	// Copy stack content, we iterate 4 by 4 because we cast the sp value to a pointer
	register uint32_t memory_content;
	for (i = 0; i < (MEMORY_SIZE / 4); ++i)
	{
			memory_content = ((uint32_t*) current_process->memory_start)[i];
			
			// Invalidate TLB entries
			INVALIDATE_TLB();
			// Current process MMU mod
			configure_mmu_C((uint32_t) new_process->page_table);
	
			((uint32_t*) new_process->memory_start)[i] = memory_content;
			// Invalidate TLB entries
			INVALIDATE_TLB();
			// Current process MMU mod
			configure_mmu_C((uint32_t) current_process->page_table);
	}
	
	
	// TODO Copy heap content (careful with virtual and physical addresses for different page_tables)
	// check out :
	// stackoverflow.com/questions/8857830/fork-implementation
	// https://github.com/pykello/arunos/blob/master/kernel/proc/syscall_fork.c (might be useful)
	
	
	// Return 0 if the new process, or the pid when in parent process
	new_process->registers[0] = 0; // The child process' return is 0, which is stored in r0


	*sp = new_process->pid;
}
