#include "sched.h"
#include "vmem.h"
#include "config.h"

void kmain()
{
	sched_init();
	
	 //__asm("cps 0x10"); // CPU to USER mode

	uint32_t a = vmem_translate(0, NULL);
	uint32_t b = vmem_translate(1, NULL);
	uint32_t c = vmem_translate(0x10111111, NULL);
	
	a++;
	b++;
	c++;
}
