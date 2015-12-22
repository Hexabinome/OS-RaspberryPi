#include "schedulers.h"
#include "sched.h"
#include "hw.h"

extern struct pcb_s* current_process;
extern unsigned int nbProcess;

/* ** Random generator ** */
short x = 1;
short a = 31541;
short c = 1;

int getRandomNb()
{
	x = a*x + c;
	
	int result = (int) x;

	// Sachant que le modulo coute cher, on utilise 
	//		un masque pour réduire le nombre aléatoire
	return (result & 127);
}

/* ** Scheduler functions ** */
void electRoundRobin()
{
	// Delete current if terminated (so a terminated process does not wait at the end of list)
	if (current_process->status == TERMINATED)
	{
		// If it is the last process
		if (current_process == current_process->next && current_process == current_process->previous)
			terminate_kernel();

		current_process->previous->next = current_process->next;
		current_process->next->previous = current_process->previous;
		struct pcb_s* processToDelete = current_process;
		current_process = current_process->next;
		
		free_process(processToDelete);
	}
	else if (current_process->status  == BLOCKED)
	{
		current_process = current_process->next;
	}
	else
	{
		current_process->status = WAITING;
		current_process = current_process->next;
	}

	if (current_process->status == TERMINATED || current_process->status  == BLOCKED)
		electRoundRobin(); // Elect the next one, and delete the current one
	else
		current_process->status = RUNNING; // Else, this one is now running
}

// TODO refactor all schedulers + add blocked logic

void electDynamicPriority()
{
	// Delete current if terminated (so a terminated process does not wait at the end of list)
	if(current_process->status == TERMINATED)
	{
		// If it is the last process
		if (current_process == current_process->next && current_process == current_process->previous)
			terminate_kernel();
		
		struct pcb_s* processToDelete = current_process;
		
		struct pcb_s* processToChoose = current_process->next;
		
		current_process->previous->next = current_process->next;
		current_process->next->previous = current_process->previous;

		// on choisi le processus suivant 'randomIteration' fois
		for(int i=0; i<(nbProcess);i++)
		{
			current_process = current_process->next;
			if(processToChoose->priority < current_process->priority)
			{
				processToChoose = current_process;
				
			}
			else{
				current_process->priority += 1;
			}
		}

		current_process = processToChoose;
		// to avoid a negativ priority
		if (current_process->priority > 0)
		{
			current_process->priority -= 1;
		}
		
		free_process(processToDelete);
		nbProcess = nbProcess-1;
	}
	else
	{
		struct pcb_s* processToChoose = current_process;
		current_process->status = WAITING;

		for(int i=0; i<(nbProcess);i++)
		{
			current_process = current_process->next;

			if(processToChoose->priority < current_process->priority) {

				processToChoose = current_process;
			}
			else{
				current_process->priority += 1;
			}
		}

		current_process = processToChoose;
		// to avoid a negativ priority
		if (current_process->priority > 0)
		{
			current_process->priority -= 1;
		}
	}

	if (current_process->status == TERMINATED)
		electDynamicPriority(); // Elect the next one, and delete the current one
	else
		current_process->status = RUNNING; // Else, this one is now running
}

void electFixPriority()
{
		
	// Delete current if terminated (so a terminated process does not wait at the end of list)
	if(current_process->status == TERMINATED)
	{
		// If it is the last process
		if (current_process == current_process->next && current_process == current_process->previous)
			terminate_kernel();
		
		struct pcb_s* processToDelete = current_process;
		
		struct pcb_s* processToChoose = current_process->next;
		
		current_process->previous->next = current_process->next;
		current_process->next->previous = current_process->previous;

		// on choisi le processus suivant 'randomIteration' fois
		for(int i=0; i<(nbProcess);i++)
		{
			current_process = current_process->next;
			if(processToChoose->priority < current_process->priority)
			{
				processToChoose = current_process;
			}
		}

		current_process = processToChoose;
		
		free_process(processToDelete);
		nbProcess = nbProcess-1;
	}
	else
	{
		struct pcb_s* processToChoose = current_process;
		current_process->status = WAITING;

		for(int i=0; i<(nbProcess);i++)
		{
			current_process = current_process->next;

			if(processToChoose->priority < current_process->priority) {

				processToChoose = current_process;
			}
		}

		current_process = processToChoose;
	}

	if (current_process->status == TERMINATED)
		electFixPriority(); // Elect the next one, and delete the current one
	else
		current_process->status = RUNNING; // Else, this one is now running
}

void electRandom()
{
	//On genere le nombre aléatoire entre min et max int.
	int randomNb = getRandomNb();

	uint64_t division = divide(randomNb, (nbProcess));
	int moduloAide = division * (nbProcess);

	//On calcule un nombre d'iteration a partir du randomNB (sachant que c'est pas une numero entro 0 et 1 mais entre min int et max int)
	int randomIteration = randomNb - moduloAide;

	// Delete current if terminated (so a terminated process does not wait at the end of list)
	if (current_process->status == TERMINATED)
	{
		// If it is the last process
		if (current_process == current_process->next && current_process == current_process->previous)
			terminate_kernel();
		
		struct pcb_s* processToDelete = current_process;
		

		int i = 0;
		
		// on enleve le maillon de la chaine		
		current_process->previous->next = current_process->next;
		current_process->next->previous = current_process->previous;

		// on choisi le processus suivant 'randomIteration' fois
		for (i = 0; i < randomIteration; i++) 
		{
			current_process = current_process->next;
		}
		
		free_process(processToDelete);
		nbProcess = nbProcess-1;
	}
	else
	{
		current_process->status = WAITING;
		int j=0;
		for(j=0;j<randomIteration;j++)
		{
			current_process = current_process->next;
		}
	}

	if (current_process->status == TERMINATED)
		electRandom(); // Elect the next one, and delete the current one
	else
		current_process->status = RUNNING; // Else, this one is now running
}
