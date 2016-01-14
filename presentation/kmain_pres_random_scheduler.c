#include "syscall.h"
#include "sched.h"
#include "schedulers.h"
#include "asm_tools.h"
#include "kernel.h"

/*
 * Ordonanceur a priorite fixe
 * 
 * On cree 3 process a priorites differentes 
 */

extern struct pcb_s* current_process;

int process1()
{
	int a =0;
	
	for(int i=0; i!= 1000000000; i++) {
		a = a+1;
	}
	sys_exit(0);
	return 0;
}

int process2()
{
	int a =0;
	
	for(int i=0; i!= 1000000000; i++) {
		a = a+1;
	}
	sys_exit(0);
	return 0;

}

int process3()
{
	int a =0;
	
	for(int i=0; i!= 1000000000; i++) {
		a = a+1;
	}
	sys_exit(0);
	return 0;
}

void kmain()
{
	init_kernel();
	int ret;
	
	ret = sys_setscheduler(RANDOM_SCHED);
	ret++;
	create_process(&process1);
	create_process(&process2);
	create_process(&process3);

	
	start_kernel();
	
	while(1);
	
}

