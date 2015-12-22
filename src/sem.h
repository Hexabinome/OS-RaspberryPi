#ifndef SEM_H
#define SEM_H

#include "sched.h"
#include <stdint.h>

struct sem_s
{
	int32_t counter;
	struct pcb_s* first_waiting; // pointer on first waiting context
};

void sem_init(struct sem_s* sem, unsigned int val);
void sem_down(struct sem_s* sem);
void sem_up(struct sem_s* sem);

#endif // SEM_H
