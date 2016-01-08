#include "sched.h"
#include "sched_irq.h"
#include "kheap.h"
#include "malloc.h"

static void init()
{
	kheap_init();
	sched_init();
}

void kmain()
{
	init();
	
	__asm("cps 0x10"); // CPU to USER mode
	
	int* momo = gmalloc(sizeof(int) * 5);
	momo[0] = 45;
	momo[4] = 33;
	
	momo = grealloc(momo, sizeof(int) * 10);
	
	int* dada = gmalloc(sizeof(int) * 20);
	
	gfree(momo);
	
	int* jojo = gmalloc(sizeof(int) * 5); // takes the place of momo
	jojo[4] = 123;
	
	int* jojo2 = grealloc(jojo, sizeof(int) * 50);
	
	while (1) ;
	
	dada++; jojo2++;
}
