#include "sched.h"
#include "vmem.h"
#include "config.h"

void kmain()
{
	sched_init();
	
	 //__asm("cps 0x10"); // CPU to USER mode

	uint32_t a = vmem_translate(0, NULL);
	uint32_t b = vmem_translate(1, NULL);
	uint32_t page_addr1 = vmem_alloc_for_userland(NULL,5000);
	uint32_t* c1 = (uint32_t*)vmem_translate(page_addr1, NULL);
	uint32_t* c2 = (uint32_t*)vmem_translate(page_addr1+1, NULL);
	uint32_t* c3 = (uint32_t*)vmem_translate(page_addr1+2, NULL);
	uint32_t* c4 = (uint32_t*)vmem_translate(page_addr1+4097, NULL);
	uint32_t page_addr2 = vmem_alloc_for_userland(NULL,5000);
	uint32_t* c5 = (uint32_t*)vmem_translate(page_addr2, NULL);
	
	vmem_free((uint8_t*)page_addr1, NULL, 5000);
	c1 = (uint32_t*)vmem_translate(page_addr1, NULL);
	c2 = (uint32_t*)vmem_translate(page_addr1+1, NULL);
	c3 = (uint32_t*)vmem_translate(page_addr1+2, NULL);
	c4 = (uint32_t*)vmem_translate(page_addr1+4097, NULL);
	
	vmem_free((uint8_t*)page_addr2, NULL, 5000);
	c1 = (uint32_t*)vmem_translate(page_addr1, NULL);
	c2 = (uint32_t*)vmem_translate(page_addr1+1, NULL);
	c3 = (uint32_t*)vmem_translate(page_addr1+2, NULL);
	c4 = (uint32_t*)vmem_translate(page_addr1+4097, NULL);
	c5 = (uint32_t*)vmem_translate(page_addr2, NULL);
	
	a++;
	b++;
	c1++;
	c2++;
	c3++;
	c4++;
	c5++;
}
