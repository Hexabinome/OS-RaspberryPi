#include "sched.h"
#include "vmem.h"
#include "config.h"

void kmain()
{
	sched_init();

	uint32_t res; 

	res = vmem_translate(0, NULL);
	res = vmem_translate(1, NULL);
	res = vmem_translate(0x20000000, NULL);
	res = vmem_translate(0x20123456, NULL);
	res = vmem_translate(0x20FFFFFF, NULL);
	
	res = vmem_translate(0x19999999, NULL);
	res = vmem_translate(0x21000000, NULL);
	res = vmem_translate(0x10000000, NULL);
	res = vmem_translate(0x19000000, NULL);
	
	res++;
}
