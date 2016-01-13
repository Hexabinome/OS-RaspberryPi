#include "fork.h"
#include "sched.h"
#include "syscall.h"
#include "config.h"
#include "process.h"
#include "vmem.h"
#include "sched_svc.h"
#include "kheap.h"
#include "asm_tools.h"
#include "util.h"

extern struct pcb_s* current_process;
extern void set_second_table_value(uint32_t** table_base, uint32_t log_addr, uint32_t phy_addr);
extern uint8_t* frame_table;
static void frame_cpy(uint32_t* phy_addr1, uint32_t* phyaddr2);
extern struct pcb_s kmain_process;
//TODO AMELIORER, beaucoup trop long
void do_sys_fork(uint32_t* sp)
{
	context_load_from_pcb_svc(sp); // update current process sp and lr
	register struct pcb_s* new_process = add_process(current_process->entry);

	uint32_t i;
	// Copy register content
	for (i = 0; i < NBREG; ++i)
	{
		new_process->registers[i] = current_process->registers[i];
	}
	new_process->lr_user = current_process->lr_user;
	new_process->priority = current_process->priority;
	new_process->cpsr_user = current_process->cpsr_user;
	new_process->lr_svc = current_process->lr_svc; // The new process must begin here (when coming from an IRQ interrupt or yield system call
	new_process->sp_user = (uint32_t*) (new_process->memory_start + ((uint32_t) current_process->sp_user - current_process->memory_start));
	// Copy stack content, we iterate 4 by 4 because we cast the sp value to a pointer
	/*register uint32_t memory_content;
	for (i = 0; i < (MEMORY_SIZE / 4); ++i)
	{
			memory_content = ((uint32_t*) current_process->memory_start)[i];

			switch_mmu_to(new_process);

			((uint32_t*) new_process->memory_start)[i] = memory_content;

			switch_mmu_to(current_process);
	}*/
	__asm("mov sp, %0" : : "r"(kmain_process.sp_user));
	
	//NEW Algo
	//We disable MMU
	disable_mmu();
	uint32_t** page_table1;
	uint32_t* page_table2;
	uint32_t* frame_address;
	uint32_t j, frame;	
	
	//We look at current_process page table,
	//i = 4 because table2 size is 1024 and kernel memory size is 4096 (frame)
	// so the 4 table1 entries are already taken
	for(i = 4; i < FIRST_LVL_TT_COUN; i++)
	{
		
		//For each table 1 entry, if not empty we look at table 2
		if(current_process->page_table[i] != NULL)
		{
			page_table1 = (uint32_t**)current_process->page_table[i];
			for(j=0;j<SECON_LVL_TT_COUN; j++)
			{
				//For each table 2 entry, if not empty, we find a free frame
				if(page_table1[j] != NULL)
				{
					page_table2 = (uint32_t*)page_table1[j];
					frame = 0;
					while((frame < FRAME_TABLE_SIZE) && (frame_table[frame] == FRAME_OCCUPIED))
					{
						frame++;
					}
					if(frame == FRAME_TABLE_SIZE)
					{
						PANIC();
					}
					frame_table[frame] = FRAME_OCCUPIED;
					frame_address = (uint32_t*)(frame * FRAME_SIZE);
					//we copy the content of the frame referenced in table 2 in the new frame
					frame_cpy((uint32_t*)page_table2[0], frame_address);
					//we put the adress of the new frame in the new process table 2
					*((uint32_t*)(new_process->page_table[i][j])) = (uint32_t) frame_address;
					//set_second_table_value(new_process->page_table, (uint32_t)page_table2, (uint32_t)frame_address);
				}
			}
		}
	}
		
	
	
	
	//reactivate MMU
	start_mmu_C();
	switch_mmu_to(current_process);
	__asm("mov sp, %0" : : "r"(current_process->sp_user));
	// TODO Copy heap content (careful with virtual and physical addresses for different page_tables)
	// check out :
	// stackoverflow.com/questions/8857830/fork-implementation
	// https://github.com/pykello/arunos/blob/master/kernel/proc/syscall_fork.c (might be useful)


	// Return 0 if the new process, or the pid when in parent process
	new_process->registers[0] = 0; // The child process' return is 0, which is stored in r0


	*sp = new_process->pid;
}
//TODO Ameliorer avec l'etat du processus, le pere se met en etat WAIT et le
//scheduler gere la remise en marche
void do_sys_waitpid(uint32_t* sp)
{
	uint32_t pid;
	uint32_t* status;
	__asm("mov %0, r1" : "=r"(pid));
	__asm("mov %0, r2" : "=r"(status));

	struct pcb_s* process_waiting_for = current_process->child;

	if (process_waiting_for->pid == pid)
	{
		// Wait that this process if terminated
		ENABLE_IRQ();
		while (process_waiting_for->status != TERMINATED)
		{
			//sys_yield();
		}
		DISABLE_IRQ();

		// Delete from brother list
		current_process->child = process_waiting_for->brother;
		*status = process_waiting_for->return_code;

		// Free struct pcb
		kFree((uint8_t*) process_waiting_for, sizeof(struct pcb_s));
	}
	else if (pid == -1)
	{
		// Check if one child process is terminated
		ENABLE_IRQ();
		while (process_waiting_for->status != TERMINATED)
		{
			process_waiting_for = process_waiting_for->brother;
			if (process_waiting_for == NULL)
			{
				process_waiting_for = current_process->child;
				//sys_yield();
			}
		}
		DISABLE_IRQ();

		if (process_waiting_for == current_process->child)
		{
			current_process->child = process_waiting_for->brother;
		}
		else
		{
			struct pcb_s* previous_waiting_process = current_process->child;

			while (previous_waiting_process->brother != process_waiting_for)
			{
				previous_waiting_process = previous_waiting_process->brother;
			}
			previous_waiting_process->brother = process_waiting_for->brother;
		}

		*status = process_waiting_for->return_code;
		// Free struct pcb
		kFree((uint8_t*) process_waiting_for, sizeof(struct pcb_s));
	}

	else
	{
		struct pcb_s* process_waiting_for_previous = process_waiting_for;
		process_waiting_for = process_waiting_for->brother;
		while (process_waiting_for != NULL && process_waiting_for->pid != pid)
		{
			process_waiting_for_previous = process_waiting_for;
			process_waiting_for = process_waiting_for->brother;
		}
		if (process_waiting_for == NULL)
		{
			return; // TODO errno error code
		}

		// Wait that this process if terminated
		ENABLE_IRQ();
		while (process_waiting_for->status != TERMINATED)
		{
			//sys_yield();
		}
		DISABLE_IRQ();

		// Delete from brother list
		process_waiting_for_previous->brother = process_waiting_for->brother;
		*status = process_waiting_for->return_code;

		// Free struct pcb
		kFree((uint8_t*) process_waiting_for, sizeof(struct pcb_s));
	}
}

void frame_cpy(uint32_t* phy_addr1, uint32_t* phy_addr2)
{
	uint32_t i;
	// Frame = 4096Bytes = 4096/4 uint32_t
	for(i=0; i < FRAME_SIZE/4; i++)
	{
		phy_addr2[i] = phy_addr1[i];
	}
		
}
