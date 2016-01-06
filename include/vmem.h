#ifndef _VMEM_H
#define _VMEM_H

#include <stdint.h>
#include "sched.h"

#define PAGE_SIZE 4096
#define FRAME_SIZE 4096
#define FRAME_TABLE_SIZE 135168

#define FIRST_LVL_TT_COUN 4096 // Number of table 1 entries
#define FIRST_LVL_TT_SIZE 16384
#define FIRST_LVL_TT_ALIG 14 // Alignment

#define SECON_LVL_TT_COUN 256 // Number of table 2 entries
#define SECON_LVL_TT_SIZE 1024
#define SECON_LVL_TT_ALIG 10 // Alignment

#define FRAME_OCCUPIED 1
#define FRAME_FREE 0


void vmem_init();
unsigned int init_kern_translation_table(void);
uint32_t** init_translation_table(void);
uint8_t* init_frame_occupation_table(void);

void switch_mmu_to_kernel();
void switch_mmu_to(struct pcb_s* process);
uint32_t vmem_translate(uint32_t va, struct pcb_s* process);

uint8_t is_forbidden_address(uint32_t addr);

uint32_t vmem_alloc_for_userland(struct pcb_s* process, uint32_t size);
void vmem_free(uint8_t* vAddress, struct pcb_s* process, unsigned int size);

void insert_fb_address_in_mem(uint32_t fb_address, uint32_t max_address);

#endif // _VMEM_H
