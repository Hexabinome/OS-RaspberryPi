#ifndef SCHED_H
#define SCHED_H

#include "process.h"

enum PROCESS_STATUS
{
	READY,
	RUNNING,
	TERMINATED,
	BLOCKED
};

void sched_init();

struct pcb_s* add_process(func_t* entry);
void create_process(func_t* entry);
void create_process_with_fix_priority(func_t* entry, int priority);
void free_process(struct pcb_s* process);


#endif // SCHED_H
