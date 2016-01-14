#include "syscall.h"
#include "sched.h"
#include "schedulers.h"
#include "asm_tools.h"
#include "kernel.h"

/*
 * Ordonanceur a priorite fixe
 * 
 * On cree 3 process a priorites differentes 
 * On observe qu'une fois le processus a la priorite 
 * la plus forte est termine, c'est le 2° a la
 * priorite la plus forte qui prend la main et ainsi de suite
 * --> pas de changement de processu tant que celui a la
 * priorite la plus forte n'est pas termine
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
	
	ret = sys_setscheduler(FIXED_PRIORITY_SCHED);
	ret++;
	create_process_with_fix_priority(&process1, 1);
	create_process_with_fix_priority(&process2, 2);
	create_process_with_fix_priority(&process3, 3);
	
	start_kernel();
	
	while(1);
	
}

