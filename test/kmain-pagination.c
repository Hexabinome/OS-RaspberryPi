#include "sched.h"
#include "vmem.h"
#include "config.h"

void kmain()
{
	sched_init();
	
	// switch cpu to user mode
	 __asm("cps 0x10");

	vmem_translate(0, NULL);
	vmem_translate(1, NULL);
	vmem_translate(0x20000000, NULL);
	vmem_translate(0x19999999, NULL);
	vmem_translate(0x21000000, NULL);
}
