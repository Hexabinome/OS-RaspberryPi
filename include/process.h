#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include "malloc.h"

#define MEMORY_SIZE 1000000 // 100 KB
#define HEAP_SIZE 900000 // 90 KB
#define STACK_SIZE 100000 // 10 KB
#define NBREG 13

typedef int (func_t) (void);

struct pcb_s
{
	uint32_t pid;
	uint32_t ppid;
	
	uint32_t registers[NBREG];
	uint32_t lr_svc;
	uint32_t lr_user;
	uint32_t* sp_user;
	uint32_t memory_start;
	struct heap_block* heap;
	uint32_t cpsr_user;
	
	func_t* entry;
	
	int status;
	int priority;
	int return_code;
	
	struct pcb_s* next;
	struct pcb_s* previous;
	
	struct pcb_s* next_waiting_sem;
	
	uint32_t** page_table;
	
	// A process knows his children and his brothers
	struct pcb_s* child;
	struct pcb_s* brother;
};

#endif // PROCESS_H
