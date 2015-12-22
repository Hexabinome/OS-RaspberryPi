#include "sem.h"
#include "config.h"

// TODO : operations must be atomic

extern struct pcb_s* current_process;

void sem_init(struct sem_s* sem, unsigned int val)
{
	sem->counter = val;
	sem->first_waiting = NULL;
}

void sem_down(struct sem_s* sem)
{
	--(sem->counter);
	if (sem->counter < 0)
	{
		current_process->status = BLOCKED;
		// Add process at the end of waiting processes
		if (sem->first_waiting == NULL) // first one
		{
			sem->first_waiting = current_process;
		}
		else // at least already one waiting for sem
		{
			struct pcb_s* curr = sem->first_waiting;
			while (curr->next_waiting_sem != NULL)
			{
				curr = curr->next_waiting_sem;
			}
			curr->next_waiting_sem = current_process;
		}
	}
}

void sem_up(struct sem_s* sem)
{
	++(sem->counter);
	if (sem->counter <= 0)
	{
		struct pcb_s* unblocked_process = sem->first_waiting;
		unblocked_process->status = WAITING;
		sem->first_waiting = unblocked_process->next_waiting_sem;
	}
}
