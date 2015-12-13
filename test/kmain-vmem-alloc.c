#include "sched.h"
#include "vmem.h"
#include "config.h"

void kmain()
{
	sched_init();
	
	uint32_t log_addr1 = vmem_alloc_for_userland(NULL, 1024); // alloc 1KiB
	
	uint32_t* phy_addr;
	phy_addr = (uint32_t*)vmem_translate(log_addr1, NULL);
	phy_addr = (uint32_t*)vmem_translate(log_addr1+1, NULL);
	
	phy_addr++;
}
