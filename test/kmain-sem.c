#include "sem.h"
#include "syscall.h"

struct sem_s sem;

int process1()
{
	sem_down(&sem);
	while (sem.counter != -1) ;
	sem_up(&sem);
	return 0;
}

int process2()
{
	sem_down(&sem);
	while (sem.counter != 1) ;
	sem_up(&sem);
	return 0;
}

int process3()
{
	sem_down(&sem); // blocked
	while (sem.counter != 0) ;
	sem_up(&sem);
	return 0;
}

int process4()
{
	while (sem.counter != 2) ;
	sem_up(&sem);
	return 0;
}

void kmain()
{
	sched_init();
	
	create_process(&process1);
	create_process(&process2);
	create_process(&process3);
	create_process(&process4);
	
	irq_init();
	
	__asm("cps 0x10"); // CPU to USER mode
	
	sem_init(&sem, 2);
	
	while (sem.counter != 3) ; // should be commented (until sem_wait_for is implemented)
	
	sem_wait_for(&sem, 3);
}
