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
	momo[15] = 9; // should fail

	return 0;
}

static void retirer_object(unsigned int* i)
{
	*i = chain[--counter];
	log_str("Take from chain ");
	uart_send_int(*i);
	log_str("\n");
}

void kmain()
{
	audio_init();
	sched_init();

	create_process(&producteur);
	create_process(&consommateur);

	irq_init();

	while (1);
}
