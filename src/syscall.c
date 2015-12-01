#include "syscall.h"
#include "util.h"
#include "asm_tools.h"
#include "hw.h"

enum SYSCALLS
{
	REBOOT,
	NOP,
	SETTIME,
	GETTIME,
	YIELDTO,
	YIELD,
	EXIT
};

void sys_reboot()
{
	__asm("MOV r0, %0": : "r"(REBOOT));
	__asm("SWI #0");
}

static void do_sys_reboot()
{
#ifdef RPI
	Set32(PM_WDOG, PM_PASSWORD | 1);
	Set32(PM_RSTC, PM_PASSWORD | PM_RSTC_WRCFG_FULL_RESET);
	while(1);
#else
	__asm("MOV pc, #0");
#endif
}

void sys_nop()
{
	__asm("MOV r0, %0": : "r"(NOP));
	__asm("SWI #0");
}


static void do_sys_nop() {}

void sys_settime(uint64_t date_ms)
{
	uint32_t leftBits = date_ms >> 32;
	uint32_t rightBits = (uint32_t)date_ms;
	__asm("MOV r1, %0": : "r"(leftBits));
	__asm("MOV r2, %0": : "r"(rightBits));
	__asm("MOV r0, %0": : "r"(SETTIME));
	__asm("SWI #0");
}

static void do_sys_settime(uint32_t* sp) // Is sp+1, pointing on r1
{
	uint32_t leftBits = *sp;
	uint32_t rightBits = *(sp+1);
	uint64_t date_ms = (((uint64_t)leftBits) << 32) | rightBits;
	set_date_ms(date_ms);
}

uint64_t sys_gettime()
{
	__asm("MOV r0, %0": : "r"(GETTIME));
	__asm("SWI #0"); // Call swi_handler, which puts response into r0 & r1
	
	uint32_t leftBits, rightBits;
	__asm("MOV %0, r1" : "=r"(rightBits));
	__asm("MOV %0, r0" : "=r"(leftBits));

	uint64_t date_ms = (((uint64_t)leftBits) << 32 | rightBits);
	return date_ms;
}

void do_sys_gettime(uint32_t* sp) // Pointing on r0
{
	uint64_t date_ms = get_date_ms();
	uint32_t leftBits = date_ms >> 32;
	uint32_t rightBits = (uint32_t)date_ms;

	*sp = leftBits;
	*(sp+1) = rightBits;
}

void sys_yieldto(struct pcb_s* dest)
{
	__asm("MOV r1, %0": : "r"(dest));
	__asm("MOV r0, %0": : "r"(YIELDTO));
	__asm("SWI #0");
}

void do_sys_yieldto(uint32_t* sp); // implemented in sched.c

void sys_yield()
{
	__asm("MOV r0, %0": : "r"(YIELD));
	__asm("SWI #0");
}

void do_sys_yield(uint32_t* sp); // implemented in sched.c

void sys_exit(int status)
{
	__asm("MOV r1, %0": : "r"(status));
	__asm("MOV r0, %0": : "r"(EXIT));
	__asm("SWI #0");
}

void do_sys_exit(uint32_t* status); // implemented in sched.c

void __attribute__((naked)) swi_handler()
{
	__asm("stmfd sp!, {r0-r12, lr}");

	uint32_t* sp; // Points on "r0" in stack
	__asm("MOV %0, sp" : "=r"(sp));

	int interruptNum;
	__asm("MOV %0, r0" : "=r"(interruptNum));
	switch (interruptNum)
	{
		case REBOOT:
			do_sys_reboot();
			break;
		case NOP:
			do_sys_nop();
			break;
		case SETTIME:
			do_sys_settime(sp+1);
			break;
		case GETTIME:
			do_sys_gettime(sp);
			break;
		case YIELDTO:
			do_sys_yieldto(sp);
			break;
		case YIELD:
			do_sys_yield(sp);
			break;
		case EXIT:
			do_sys_exit(sp);
			break;
		default:
			PANIC();
	}
	__asm("ldmfd sp!, {r0-r12, pc}^");
}

