#ifndef __VMEM__
#define __VMEM__
#include <stdint.h>
#include "sched.h"

#define PAGE_SIZE 4096
#define FIRST_LVL_TT_COUN 4096
#define FIRST_LVL_TT_SIZE 16384
//alignement
#define FIRST_LVL_TT_ALIG 14
#define SECON_LVL_TT_COUN 256
#define SECON_LVL_TT_SIZE 1024
#define SECON_LVL_TT_ALIG 10

unsigned int init_kern_translation_table(void);
uint32_t vmem_translate(uint32_t va,struct pcb_s* process);
void vmem_init();
void configure_mmu_C();
void start_mmu_C();
#endif //__VMEM__
