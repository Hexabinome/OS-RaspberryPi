<<<<<<< HEAD
=======
#include "syscall.h"
>>>>>>> ba0202b8fafd210bcc870edff16bdfabb61d6cd5
#include "sched.h"
#include "pwm.h"
#include "malloc.h"

<<<<<<< HEAD
int process1()
=======
static int process1()
>>>>>>> ba0202b8fafd210bcc870edff16bdfabb61d6cd5
{
	int* momo = (int*)gmalloc(sizeof(int) * 10);
	momo[0] = 1;
	momo[4] = 4;
	momo[9] = 9;
<<<<<<< HEAD

	gfree(momo);

	momo = (int*)gmalloc(sizeof(int) * 10);
	momo[0] = 1;
	momo[4] = 4;
	momo[10] = 9; // should fail->
     //tested with the real C malloc, doesn't fail you can access to indexes out
    //of the table while it's in your allocated pages i think

=======
	
	gfree(momo);
	
	momo = (int*)gmalloc(sizeof(int) * 10);
	momo[0] = 1;
	momo[4] = 4;
	momo[10] = 9; // should fail
	
>>>>>>> ba0202b8fafd210bcc870edff16bdfabb61d6cd5
	return 0;
}

void kmain()
{
	audio_init();
	sched_init();

	create_process(&process1);
<<<<<<< HEAD

	irq_init();

	while (1);
=======
	irq_init();
	
	__asm("cps 0x10"); // CPU to USER mode
	
	while (1)
	{
		sys_yield();
	}
>>>>>>> ba0202b8fafd210bcc870edff16bdfabb61d6cd5
}
