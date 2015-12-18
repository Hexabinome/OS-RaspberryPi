#include <stdint.h>
#include "sched.h"
#include "kheap.h"
#include "config.h"
#include "hw.h"
#include "asm_tools.h"
#include "syscall.h"
#include "vmem.h"
#include "schedulers.h"

#define STACK_SIZE 10000

unsigned int nbProcess;
extern const uint8_t nb_tables_kernel_device; // For kernel & device, we have 0xFFFFFF addresse to store, 16 = 0xFFFFFF / (RAME_SIZE[4096] * SECON_LVL_TT_COUN[256])
extern const uint16_t device_address_page_table_idx_start;
extern const uint16_t device_address_page_table_idx_end;
struct pcb_s* current_process;
static struct pcb_s kmain_process;

unsigned int MMUTABLEBASE; /* Page table address */

void (*current_scheduler)(void); // pointer on the current scheduler function

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

	current_scheduler = &electRoundRobin; // Default scheduler
	current_process->page_table = init_translation_table();
	
	timer_init();
	ENABLE_IRQ();
}

static void start_current_process()
{
	current_process->entry();
	sys_exit(0);
}

struct pcb_s* add_process(func_t* entry)
{
	// Invalidate TLB entries
	INVALIDATE_TLB();
	// Kernel MMU mod
	configure_mmu_kernel();
	
	// Alloc pcb
	struct pcb_s* process = (struct pcb_s*)kAlloc(sizeof(struct pcb_s));
	process->entry = entry;
	process->lr_svc = (uint32_t)&start_current_process;
	
	
	__asm("mrs %0, cpsr" : "=r"(process->cpsr_user)); // TODO : pourquoi nécessaire d'initialiser CPSR
	process->cpsr_user &= 0b1111111111111111111111101111111; // Put interruptions ON

	// Put the next process at the end of the list
	struct pcb_s* lastProcess = current_process;
	while (lastProcess->next != &kmain_process)
	{
		lastProcess = lastProcess->next;
	}
	lastProcess->next = process;
	process->previous = lastProcess;
	process->next = &kmain_process;

	// Initial status
	process->status = WAITING;
	++nbProcess;
	
	process->page_table = init_translation_table();
	
	//going to created process mmu mod to allocate stack
	// Invalidate TLB entries
	INVALIDATE_TLB();
	//process MMU mod
	configure_mmu_C((uint32_t)process->page_table);
	
	// Allocate stack
	process->sp_end = vmem_alloc_for_userland(process, STACK_SIZE);
	process->sp_user = (uint32_t*)(process->sp_end + STACK_SIZE);
	
	// Invalidate TLB entries
	INVALIDATE_TLB();
	// Current process MMU mod
	configure_mmu_C((uint32_t)current_process->page_table);
	
	
	return process;	
}

void create_process(func_t* entry)
{
	DISABLE_IRQ();
	add_process(entry);
	ENABLE_IRQ();
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
	
	//free stack
	vmem_free((uint8_t*)process->sp_end, process, STACK_SIZE);
	
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

void do_sys_yield(uint32_t* sp) // Points on saved r0 in stack
{
	// Save current context
	__asm("cps 0b11111"); // System mode
	__asm("mov %0, lr" : "=r"(current_process->lr_user)); // Save lr_user
	__asm("mov %0, sp" : "=r"(current_process->sp_user)); // Save sp_user
	__asm("cps 0b10011"); // SVC mode

	__asm("mrs %0, spsr" : "=r"(current_process->cpsr_user)); // Save user CPSR, which has been copied at swi in SPSR_svc

	int i;
	for (i = 0; i < NBREG; ++i)
	{
		current_process->registers[i] = sp[i];
	}
	current_process->lr_svc = sp[NBREG];

	// Elects new current process
	current_scheduler();

	// Set transalation table
	INVALIDATE_TLB();
	// Current process MMU mod
	configure_mmu_C((uint32_t)current_process->page_table);

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

void do_sys_exit(uint32_t* sp) // Points on saved r0 in stack
{
	current_process->status = TERMINATED;
	current_process->return_code = *(sp+1);

	sys_yield();
}

static void context_save_to_pcb(uint32_t* sp) // Points to the beginning of process context in stack
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

static void context_load_from_pcb(uint32_t* sp) // Points to the beginning of process context in stack
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
	context_save_to_pcb(sp);

	current_scheduler(); // calls current scheduler method
	
	// set translation table
	// Invalidate TLB entries
	INVALIDATE_TLB();
	// Current process MMU mod
	configure_mmu_C((uint32_t)current_process->page_table);

	context_load_from_pcb(sp);
}

void __attribute__((naked)) irq_handler()
{
	// Already in IRQ mode. So the old cpsr has been copied into spsr_irq. I bit of cpsr already to 1 (deactivated)

	// Save registers and return address (decremented by 4, see Encart 13 + p.18 the "2; il copie...")
	// Save return address lr because there is a function call in this function
	__asm("sub lr, #4");
	__asm("stmfd sp!, {r0-r12, lr}");
	//__asm("push {r0-r12, lr}"); // The same
	
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

void do_sys_setscheduler(uint32_t* sp)
{
	uint32_t scheduler = *(sp+1);
	uint8_t return_val = 0;
	
	switch (scheduler)
	{
		case ROUND_ROBIN_SCHED:
			current_scheduler = &electRoundRobin;
			break;
		case RANDOM_SCHED:
			current_scheduler = &electRandom;
			break;
		case FIXED_PRIORITY_SCHED:
			current_scheduler = &electFixPriority;
			break;
		case DYNAMIC_PRIORITY_SCHED:
			current_scheduler = &electDynamicPriority;
			break;
		default:
			return_val = -1;
	}
	
	*sp = return_val;
}
