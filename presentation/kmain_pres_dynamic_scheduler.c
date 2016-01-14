#include "syscall.h"
#include "sched.h"
#include "schedulers.h"
#include "asm_tools.h"
#include "kernel.h"
#include "fb_cursor.h"
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


void kmain()
{
	init_kernel();
	int ret;
	ret = sys_setscheduler(DYNAMIC_PRIORITY_SCHED);
	ret++;
	create_process_with_fix_priority(&process1, 10);
	create_process_with_fix_priority(&process2, 5);

	start_kernel();

	while(1);

}

