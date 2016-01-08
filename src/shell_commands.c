#include "shell_commands.h"
#include "hw.h"
#include "process.h"
#include "sched.h"
#include "fb_cursor.h"


extern struct pcb_s* current_process;

void do_echo(char** args)
{
	/*uint32_t i = 0;
	char* word;
	while ((word = args[i++]) != '\0')
	{
		fb_print_text(word);
	}*/
	fb_print_text(args[0]);
}


static void print_process(struct pcb_s* process)
{
	fb_print_int(process->pid);
	
	fb_print_text("      ");
	fb_print_int(process->ppid);
	
	fb_print_text("       ");
	fb_print_int(process->priority);
	
	fb_print_text("           ");
	fb_print_int(process->status);
	fb_print_text("\n");
}

void do_ps(char** args)
{
	struct pcb_s* process = current_process;
	fb_print_text("\npid:   ");
	fb_print_text("ppid:   ");
	fb_print_text("priority:   ");
	fb_print_text("status:   ");
	fb_print_text("\n");

	while(process != current_process->previous)
	{
		print_process(process);
		process = process->next;
	}
	print_process(process);	
}

