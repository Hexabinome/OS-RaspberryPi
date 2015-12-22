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
// Corresponds to a P operation. Takes one from semaphore
void sem_down(struct sem_s* sem);
// Corresponds to a V operation. Gives back one to the semaphore
void sem_up(struct sem_s* sem);
// Corresponds to a Z operation. Blocks the process until the semaphores reaches the specified value
void sem_wait_for(struct sem_s* sem, int val);

#endif // SEM_H
