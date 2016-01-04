#include "sched_irq.h"
#include <stdint.h>
#include "hw.h"
#include "asm_tools.h"
#include "schedulers.h"
#include "vmem.h"
#include "sched.h"

extern struct pcb_s* current_process;

static void handle_irq(uint32_t* sp);
static void context_save_to_pcb_irq(uint32_t* sp);
static void context_load_from_pcb_irq(uint32_t* sp);

void irq_init()
{
	timer_init();
	ENABLE_IRQ();
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

static void handle_irq(uint32_t* sp)
{
	context_save_to_pcb_irq(sp);

	elect(); // calls current scheduler method

	// Set translation table
	switch_mmu_to(current_process);

	context_load_from_pcb_irq(sp);
}

static void context_save_to_pcb_irq(uint32_t* sp) // Points to the beginning of process context in stack
{
	__asm("cps 0b11111"); // System mode
	__asm("mov %0, lr" : "=r"(current_process->lr_user)); // Save lr_user
	__asm("mov %0, sp" : "=r"(current_process->sp_user)); // Save sp_user
	__asm("cps 0b10010"); // IRQ mode

	__asm("mrs %0, spsr" : "=r"(current_process->cpsr_user)); // Save user CPSR, which has been copied at swi in SPSR_svc

	uint8_t i;
	for (i = 0; i < NBREG; ++i)
	{
		current_process->registers[i] = sp[i];
	}
	current_process->lr_svc = sp[NBREG];
}

static void context_load_from_pcb_irq(uint32_t* sp) // Points to the beginning of process context in stack
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
	__asm("cps 0b10010"); // IRQ mode

	__asm("msr spsr, %0" : : "r"(current_process->cpsr_user));
}
