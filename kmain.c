#include "sched.h"
#include "pwm.h"
#include "malloc.h"

int process1()
{
	int* momo = (int*)gmalloc(sizeof(int) * 10);
	momo[0] = 1;
	momo[4] = 4;
	momo[9] = 9;

	gfree(momo);

	momo = (int*)gmalloc(sizeof(int) * 10);
	momo[0] = 1;
	momo[4] = 4;
	momo[10] = 9; // should fail->
     //tested with the real C malloc, doesn't fail you can access to indexes out
    //of the table while it's in your allocated pages i think

	return 0;
}

void kmain()
{
	audio_init();
	sched_init();

	create_process(&process1);

	irq_init();

	while (1);
}
