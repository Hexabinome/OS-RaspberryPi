#include "schedulers.h"
#include "sched.h"
#include "hw.h"
#include "asm_tools.h"
#include "vmem.h"
#include "fb_cursor.h"

extern struct pcb_s* current_process;
extern unsigned int nb_process;

static void electRoundRobin();
static void electRandom();
static void electFixPriority();
static void electDynamicPriority();

static void (*current_scheduler)() = &electRoundRobin; // pointer on the current scheduler function

/* ** Random generator ** */
short x = 1;
short a = 31541;
short c = 1;

static uint32_t get_random()
{
	x = a*x + c;
	
	int result = (uint32_t) x;

	// Sachant que le modulo coute cher, on utilise 
	//		un masque pour réduire le nombre aléatoire
	return (result & 127);
}

static void terminate_if_last_process()
{
	if (current_process == current_process->next && current_process == current_process->previous)
	{
		terminate_kernel();
	}
}

/* ** Scheduler functions ** */
static void electRoundRobin()
{
	current_process = current_process->next;
}

static void electRandom()
{
	// Generate random number
	uint32_t random_nb = get_random();

	uint64_t division = divide(random_nb, nb_process);
	int modulo = division * nb_process;

	// On calcule un nombre d'iteration a partir du randomNB (sachant que c'est pas une numero entro 0 et 1 mais entre min int et max int)
	int random_it = (random_nb - modulo) + 1;
	
	// on choisi le processus suivant 'randomIteration' fois
	uint32_t i;
	for (i = 0; i < random_it; ++i) 
	{
		current_process = current_process->next;
	}
}

static void electFixPriority()
{
	struct pcb_s* process_to_choose = current_process->next;

	uint32_t i;
	for (i = 0; i < nb_process; ++i)
	{
		current_process = current_process->next;

		if (process_to_choose->priority < current_process->priority)
		{
			process_to_choose = current_process;
		}
	}

	current_process = process_to_choose;
}

static void electDynamicPriority()
{
	struct pcb_s* process_to_choose = current_process->next;

	uint32_t i;
	for (i = 0; i < nb_process; ++i)
	{
		current_process = current_process->next;

		if (process_to_choose->priority < current_process->priority)
		{
			process_to_choose = current_process;
		}
		else
		{
			 (current_process->priority) += ((current_process->priority)<20 ?1:0);
		}
	}

	current_process = process_to_choose;

	(current_process->priority) -= ((current_process->priority)>0 ?1:0); 

}

static void choose_next_process()
{
	current_scheduler();
	
	switch_mmu_to(current_process);
}

/* *** External visible functions *** */
void elect()
{
	// Delete current if terminated (so a terminated process does not wait at the end of list)
	if (current_process->status == TERMINATED)
	{
		terminate_if_last_process();

		current_process->previous->next = current_process->next;
		current_process->next->previous = current_process->previous;
		
		struct pcb_s* process_to_delete = current_process;
		
		choose_next_process();
		
		free_process(process_to_delete);
	}
	else if (current_process->status  == BLOCKED)
	{
		choose_next_process();
	}
	else
	{
		current_process->status = READY;
		choose_next_process();
	}
	
	#if DEBUG
	#if FB
		fb_print_char('\n');
		fb_print_text("The process with PID ");
		fb_print_int(current_process->pid);
		fb_print_text(" chosen with priority ");
		fb_print_int(current_process->priority);
		fb_print_char('\n');
		
	#else
		
		log_str("\n The process with PID ");
		log_int(current_process->pid);
		log_str(" chosen with priority ");
		log_int(current_process->priority);
		log_str("\n");
		
	#endif	
	#endif
	
	if (current_process->status == TERMINATED || current_process->status  == BLOCKED)
		elect(); // Elect the next one, and delete the current one
	else
		current_process->status = RUNNING; // Else, this one is now running
}

void do_sys_setscheduler(uint32_t* sp)
{
	uint32_t scheduler = *(sp+1);
	int8_t return_val = 0;
	
	switch (scheduler)
	{
		case ROUND_ROBIN_SCHED:
			current_scheduler = &electRoundRobin;
			break;
		case RANDOM_SCHED:
			current_scheduler = &electRandom;
			break;
		case FIXED_PRIORITY_SCHED:
			current_scheduler = &electFixPriority;
			break;
		case DYNAMIC_PRIORITY_SCHED:
			current_scheduler = &electDynamicPriority;
			break;
		default:
			return_val = -1;
	}
	
	*sp = return_val;
}
