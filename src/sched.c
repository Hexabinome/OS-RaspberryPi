#include <stdint.h>
#include "sched.h"
#include "kheap.h"
#include "config.h"
#include "hw.h"
#include "asm_tools.h"
#include "syscall.h"
#include "vmem.h"

#define STACK_SIZE 10000

static struct pcb_s* current_process;
static struct pcb_s kmain_process; 

void sched_init()
{
	// Init process data structure
	current_process = &kmain_process;
	current_process->next = current_process;
	current_process->previous = current_process;
	current_process->status = RUNNING;
	
	// Init memory
#if VMEM
	vmem_init();
#else
	kheap_init();
#endif
	
}

static void start_current_process()
{
	current_process->entry();
	sys_exit(0);
}

void create_process(func_t* entry)
{
	struct pcb_s* process = (struct pcb_s*)kAlloc(sizeof(struct pcb_s));
	process->entry = entry;
	process->lr_svc = (uint32_t)&start_current_process;

	// Allocate stack
	process->sp_user = (uint32_t*)kAlloc(STACK_SIZE) + STACK_SIZE;

	__asm("mrs %0, cpsr" : "=r"(process->cpsr_user)); // TODO : pourquoi nécessaire d'initialiser CPSR

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
}

static void free_process(struct pcb_s* process)
{
	kFree((uint8_t*)process, sizeof(struct pcb_s) + STACK_SIZE);
}

static void elect()
{
	// Delete current if terminated (so a terminated process does not wait at the end of list)
	if (current_process->status == TERMINATED)
	{
		// If it is the last process
		if (current_process == current_process->next && current_process == current_process->previous)
			terminate_kernel();

		current_process->previous->next = current_process->next;
		current_process->next->previous = current_process->previous;
		struct pcb_s* processToDelete = current_process;
		current_process = current_process->next;
		
		free_process(processToDelete);
	}
	else
	{
		current_process->status = WAITING;
		current_process = current_process->next;
	}

	if (current_process->status == TERMINATED)
		elect(); // Elect the next one, and delete the current one
	else
		current_process->status = RUNNING; // Else, this one is now running
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
	elect();

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
	elect();
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

