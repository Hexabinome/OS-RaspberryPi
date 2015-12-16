#include "syscall.h"
#include "vmem.h"
#include "config.h"

/*static int nbProcess = 5;
int* ptr = &nbProcess;*/

void kmain()
{
	sched_init();
	
	__asm("cps 0x10"); // CPU to USER mode

	uint32_t size = sizeof(int) * 10;
	int* momo = (int*)sys_mmap(size);

	momo[0] = 1;
	momo[4] = 4;
	momo[9] = 9;
	
	sys_munmap(momo, size);
}
