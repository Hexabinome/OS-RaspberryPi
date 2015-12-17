#include "syscall.h"
#include "vmem.h"
#include "config.h"

void kmain()
{
	sched_init();
	uint32_t phy_addr;
	uint32_t size = sizeof(int) * 10;
	
	int* tab = (int*)sys_mmap(size);
	phy_addr = vmem_translate((uint32_t)tab, NULL); // will not work once isolation is written

	tab[0] = 1;
	tab[4] = 4;
	tab[9] = 9;
	
	sys_munmap(tab, size);
	phy_addr = vmem_translate((uint32_t)tab, NULL); // will not work once isolation is written
	
	
	__asm("cps 0x10"); // CPU to USER mode

	tab = (int*)sys_mmap(size);

	tab[0] = 1;
	tab[4] = 4;
	tab[9] = 9;
	
	sys_munmap(tab, size);
	
	phy_addr++;
}
