#include "syscall.h"
#include "vmem.h"
#include "config.h"
#include "sched.h"

extern struct pcb_s* current_process;
void kmain()
{
	sched_init();
	uint32_t phy_addr;
	uint32_t size = sizeof(int) * 10;
	
	int* tab = (int*)sys_mmap(size);
	phy_addr = vmem_translate((uint32_t)tab, current_process); // Works even if vmem_translate(tab, NULL). WHY ??

	tab[0] = 1;
	tab[4] = 4;
	tab[9] = 9;
	
	sys_munmap(tab, size);
	phy_addr = vmem_translate((uint32_t)tab, current_process);
	
	
	__asm("cps 0x10"); // CPU to USER mode

	tab = (int*)sys_mmap(size);

	tab[0] = 1;
	tab[4] = 4;
	tab[9] = 9;
	
	sys_munmap(tab, size);
	
	phy_addr++;
}
