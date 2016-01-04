#include "syscall.h"
#include "sched.h"
#include "schedulers.h"
#include "asm_tools.h"
#include "sched_irq.h"

extern struct pcb_s* current_process;

int process1()
{
	return 0;
}

int process2()
{
	return 0;
}

int process3()
{
	return 0;
}

void kmain()
{
	sched_init();
	int ret;
	
	ret = sys_setscheduler(ROUND_ROBIN_SCHED); // returns 0
	create_process(&process1);
	create_process(&process2);
	create_process(&process3);
	
	irq_init();
	
	while (current_process != current_process->next || current_process != current_process->previous) ;
	
	DISABLE_IRQ();
	
	ret = sys_setscheduler(RANDOM_SCHED);
	create_process(&process1);
	create_process(&process2);
	create_process(&process3);
	
	ENABLE_IRQ();
	
	while (current_process != current_process->next || current_process != current_process->previous) ;
	
	DISABLE_IRQ();
	
	ret = sys_setscheduler(FIXED_PRIORITY_SCHED);
	create_process_with_fix_priority(&process1, 5);
	create_process_with_fix_priority(&process2, 2);
	create_process_with_fix_priority(&process3, 4);
	
	ENABLE_IRQ();
	
	while (current_process != current_process->next || current_process != current_process->previous) ;
	
	DISABLE_IRQ();
	
	ret = sys_setscheduler(DYNAMIC_PRIORITY_SCHED);
	create_process(&process1);
	create_process(&process2);
	create_process(&process3);
	
	ENABLE_IRQ();
	
	while (current_process != current_process->next || current_process != current_process->previous) ;
	
	
	ret = sys_setscheduler(-50); // fail (returns -1)
	ret++;
}
