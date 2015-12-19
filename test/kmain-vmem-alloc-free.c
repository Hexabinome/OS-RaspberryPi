#include "sched.h"
#include "vmem.h"
#include "config.h"

void kmain()
{
	sched_init();
	
	uint32_t log_addr1 = vmem_alloc_for_userland(NULL, 1024); // alloc 1KiB, one page only
	
	uint32_t* phy_addr;
	phy_addr = (uint32_t*)vmem_translate(log_addr1, NULL); // Physical address of allocated memory
	phy_addr = (uint32_t*)vmem_translate(log_addr1+1, NULL); // Still in allocated memory
	phy_addr = (uint32_t*)vmem_translate(log_addr1+FRAME_SIZE, NULL); // Second page (in table2), which has a forbidden address
	phy_addr = (uint32_t*)vmem_translate(log_addr1 + (1 << 20), NULL); // Second table1 entry, which has a forbidden address table1
	
	uint32_t log_addr2 = vmem_alloc_for_userland(NULL, 2*FRAME_SIZE); // alloc 8KiB, two pages (exacly, so it actually allocates three pages)
	
	phy_addr = (uint32_t*)vmem_translate(log_addr2, NULL); // allocated
	phy_addr = (uint32_t*)vmem_translate(log_addr2 + FRAME_SIZE, NULL); // allocated
	phy_addr = (uint32_t*)vmem_translate(log_addr2 + (2*FRAME_SIZE), NULL); // not allocated
	phy_addr = (uint32_t*)vmem_translate(log_addr2 + (3*FRAME_SIZE), NULL); // not allocated
	phy_addr = (uint32_t*)vmem_translate(log_addr1 + (1 << 20), NULL); // Second table1 entry, which has a forbidden address table1
	
	uint32_t log_addr3 = vmem_alloc_for_userland(NULL, 263*FRAME_SIZE); // alloc xKiB, 253 (fill first table2) + 10 (on second table2)
	
	phy_addr = (uint32_t*)vmem_translate(log_addr3, NULL);
	phy_addr = (uint32_t*)vmem_translate(log_addr3 + (1 << 12), NULL);
	phy_addr = (uint32_t*)vmem_translate(log_addr1 + (1 << 20), NULL); // Second table1 entry, which should be allocated
	
	// FREE
	// Free log_addr2
	vmem_free((uint8_t*) log_addr2, NULL, 2*FRAME_SIZE);
	phy_addr = (uint32_t*)vmem_translate(log_addr1, NULL); // alloc
	phy_addr = (uint32_t*)vmem_translate(log_addr1 + FRAME_SIZE, NULL); // freed
	phy_addr = (uint32_t*)vmem_translate(log_addr1 + 2*FRAME_SIZE, NULL); // freed
	phy_addr = (uint32_t*)vmem_translate(log_addr1 + 3*FRAME_SIZE, NULL); // alloc
	phy_addr = (uint32_t*)vmem_translate(log_addr1 + 4*FRAME_SIZE, NULL); // alloc
	
	// Free log_addr1
	vmem_free((uint8_t*) log_addr1, NULL, 1024);
	phy_addr = (uint32_t*)vmem_translate(log_addr1, NULL); // freed
	phy_addr = (uint32_t*)vmem_translate(log_addr1 + FRAME_SIZE, NULL); // freed before
	phy_addr = (uint32_t*)vmem_translate(log_addr1 + 2*FRAME_SIZE, NULL); // freed before
	phy_addr = (uint32_t*)vmem_translate(log_addr1 + 3*FRAME_SIZE, NULL); // freed before
	phy_addr = (uint32_t*)vmem_translate(log_addr1 + 4*FRAME_SIZE, NULL); // alloc
	
	// Free log_addr2, which should free 2 second lvl tables
	vmem_free((uint8_t*) log_addr3, NULL, 263*FRAME_SIZE);
	phy_addr = (uint32_t*)vmem_translate(log_addr1, NULL); // freed before, but table not available
	phy_addr = (uint32_t*)vmem_translate(log_addr1 + FRAME_SIZE, NULL); // freed before, but table not available
	phy_addr = (uint32_t*)vmem_translate(log_addr1 + 2*FRAME_SIZE, NULL); // freed before, but table not available
	phy_addr = (uint32_t*)vmem_translate(log_addr1 + 3*FRAME_SIZE, NULL); // freed before, but table not available
	phy_addr = (uint32_t*)vmem_translate(log_addr1 + 4*FRAME_SIZE, NULL); // freed, table not available
	phy_addr = (uint32_t*)vmem_translate(log_addr1 + (1 << 20), NULL); // Second table1 entry, which should be a forbidden address table1
	
	log_addr2++; log_addr3++; phy_addr++;
}
