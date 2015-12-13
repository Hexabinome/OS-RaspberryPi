#include "sched.h"
#include "vmem.h"
#include "config.h"

void kmain()
{
	sched_init();
	
	uint32_t log_addr1 = vmem_alloc_for_userland(NULL, 1024); // alloc 1KiB, one page only
	
	uint32_t* phy_addr;
	phy_addr = (uint32_t*)vmem_translate(log_addr1, NULL);
	phy_addr = (uint32_t*)vmem_translate(log_addr1+1, NULL);
	
	
	uint32_t log_addr2 = vmem_alloc_for_userland(NULL, 2048); // alloc 2KiB, one page only
	
	uint32_t log_addr3 = vmem_alloc_for_userland(NULL, 6144); // alloc 6KiB, two pages
	
	log_addr2++; log_addr3++; phy_addr++;
}
