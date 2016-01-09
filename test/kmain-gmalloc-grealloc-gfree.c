#include "kernel.h"
#include "sched.h"
#include "malloc.h"

static int test_process()
{
	int* momo = gmalloc(sizeof(int) * 5);
	momo[0] = 45;
	momo[4] = 33;
	
	momo = grealloc(momo, sizeof(int) * 10);
	
	int* dada = gmalloc(sizeof(int) * 20);
	
	gfree(momo);
	
	int* jojo = gmalloc(sizeof(int) * 5); // takes the place of momo
	jojo[4] = 123;
	
	int* jojo2 = grealloc(jojo, sizeof(int) * 50);
	
	dada++; jojo2++;
	return 0;
}

void kmain()
{
	init_kernel();
	
	create_process(&test_process);
	
	start_kernel();
	
	__asm("cps 0x10"); // CPU to USER mode
	
	while (1) ;
}
