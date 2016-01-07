#include "shell_commands.h"

#include "hw.h"
#include "process.h"
#include "sched.h"

extern struct pcb_s* current_process;

void do_echo(char** args)
{
	uint32_t i = 0;
	char* word;
	while ((word = args[i++]) != '\0')
	{
		log_str(word);
	}
}


static void print_process(struct pcb_s* process)
{
	log_str("\npid: ");
	log_int(process->pid);
	log_str("\nppid: ");
	log_int(process->ppid);
	log_str("\npriority: ");
	log_int(process->priority);
	log_str("\nstatus: ");
	log_int(process->status);
	log_str("\n");

}

void do_ps(char** args)
{
	struct pcb_s* process = current_process;	
	while(process != current_process->previous)
	{
		print_process(process);
		process = process->next;
	}
	print_process(process);	
}

