#include "shell_commands.h"
#include "hw.h"
#include "process.h"
#include "sched.h"
#include "fb_cursor.h"
#include "syscall.h"


extern struct pcb_s* current_process;

void do_echo(int argc, char** argv)
{
	int i;
	
	for (i = 0; i < argc; ++i)
	{
		fb_print_text(argv[i]);
		fb_print_char(' ');
	}
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

void do_ps(int argc, char** argv)
{
	struct pcb_s* process = current_process;
	fb_print_text("pid:   ");
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

void do_fork(int argc, char** argv)
{
	int pid = sys_fork();
	if (pid == 0)
		{
			fb_print_text("child talking, my pid is ");
			fb_print_int(current_process->pid);
			fb_print_text(" and my ppid is ");
			fb_print_int(current_process->ppid);
			fb_print_char('\n');
			sys_exit(0);
		}
		else
		{
			int cmd_status;
			fb_print_text("parent talking, my pid is ");
			fb_print_int(current_process->pid);
			fb_print_char('\n');
			sys_waitpid(pid, &cmd_status);
		}

}

//TODO
void do_music(int argc, char** argv)
{

}


void do_clear(int argc, char** argv)
{
	fb_clear();
	
}
