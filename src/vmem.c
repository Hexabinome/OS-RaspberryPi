#include "vmem.h"
#include "sched.h"
#include "kheap.h"
#include "config.h"
#include "hw.h"
#include "asm_tools.h"
#include "syscall.h"



unsigned int MMUTABLEBASE;



void vmem_init()
{
	kheap_init();
	MMUTABLEBASE = init_kern_translation_table();
	configure_mmu_C();
	timer_init();
	ENABLE_IRQ();
	start_mmu_C();
	
}


void start_mmu_C()
{
	register unsigned int control;
	__asm("mcr p15, 0, %[zero], c1, c0, 0" : : [zero] "r"(0));//Disable cach
	__asm("mcr p15, 0, r0, c7, c7, 0");//Invalidate cache (data and instructions)
	__asm("mcr p15, 0, r0, c8, c7, 0");//Invalidate TLB entrie
	/* Enable ARMv6 MMU features (disable sub-page AP) */
	control = (1<<23) | (1 << 15) | (1 << 4) | 1;
	/* Invalidate the translation lookaside buffer (TLB) */
	__asm volatile("mcr p15, 0, %[data], c8, c7, 0" : : [data] "r" (0));
	/* Write control register */
	__asm volatile("mcr p15, 0, %[control], c1, c0, 0" : : [control] "r" (control));
}

void configure_mmu_C()
{
	register unsigned int pt_addr = MMUTABLEBASE;
	//total++;
/* Translation table 0 */
	__asm volatile("mcr p15, 0, %[addr], c2, c0, 0" : : [addr] "r" (pt_addr));
/* Translation table 1 */
	__asm volatile("mcr p15, 0, %[addr], c2, c0, 1" : : [addr] "r" (pt_addr));
	/* Use translation table 0 for everything */
	__asm volatile("mcr p15,0, %[n], c2, c0, 2" : : [n] "r" (0));
/* Set Domain 0 ACL to "Manager", not enforcing memory permissions* Every mapped section/page is in domain 0*/
	__asm volatile("mcr p15, 0, %[r], c3, c0, 0" : : [r] "r" (0x3));
}

unsigned int init_kern_translation_table(void)
{
	uint32_t** table1 =(uint32_t**) kAlloc_aligned(FIRST_LVL_TT_SIZE, FIRST_LVL_TT_ALIG);
	
	uint32_t i = 0;
	for(i=0; i<FIRST_LVL_TT_COUN; i++){
		table1[i] = (uint32_t*)kAlloc_aligned(SECON_LVL_TT_SIZE, SECON_LVL_TT_ALIG);
	}
	uint32_t device_flags = 0b010000010110;
	uint32_t log_addr = 0;
	uint16_t first_index = 0;
	uint16_t second_index = 0;
	uint32_t* table2;
	// init kern pages
	for(log_addr = 0; log_addr <0x1000000; log_addr++){
		first_index = (log_addr >> 20)<< 2;
		second_index = ((log_addr >> 12) & 0xFF)<<2;
		// on garde seulement les 12 derniers bits
		//page_index = log_addr & 4096;
		table2 = (uint32_t*)(((uint32_t)table1[first_index])&0xFFFFFC00);
		table2[second_index] = log_addr & 0b000001010010;
	}
	
	//init devices pages
	for(log_addr = 0x20000000; log_addr <0x2FFFFFFF; log_addr++){
		first_index = (log_addr >> 20)<< 2;
		second_index = ((log_addr >> 12) & 0xFF)<<2;
		// on garde seulement les 12 derniers bits
		//page_index = log_addr & 4096;
		table2 = (uint32_t*)(((uint32_t)table1[first_index])&0xFFFFFC00);
		table2[second_index] = log_addr & device_flags;
	}
	return (unsigned int)table1;
	
}

uint32_t vmem_translate(uint32_t va,struct pcb_s* process)
{
	uint32_t pa;
	/*
	 The
	 result
	 */
	/*
	 1st and 2nd table addresses
	 */
	uint32_t table_base;
	uint32_t second_level_table;
	/*
	 Indexes
	 */
	uint32_t first_level_index;
	uint32_t second_level_index;
	uint32_t page_index;
	/*
	 Descriptors
	 */
	uint32_t first_level_descriptor;
	uint32_t* first_level_descriptor_address;
	uint32_t second_level_descriptor;
	uint32_t* second_level_descriptor_address;
	if(process == 0)
		{
		__asm("mrc p15, 0, %[tb], c2, c0, 0" : [tb] "=r"(table_base));
		}
	else
		{
		table_base = (uint32_t) process->page_table;
		}
	table_base = table_base & 0xFFFFC000;
	/*
	 Indexes
	*/
	first_level_index = (va >> 20);
	second_level_index = ((va << 12) >> 24);
	page_index = (va & 0x00000FFF);
	/*
	 First
	 level
	 descriptor
	 */
	first_level_descriptor_address = (uint32_t*) (table_base | (first_level_index << 2));
	first_level_descriptor = *(first_level_descriptor_address);
	/*
	 Translation
	 fault
	*/
	if(! (first_level_descriptor & 0x3)) {
		return (uint32_t) FORBIDDEN_ADDRESS;
	}
	/*
	 Second
	 level
	 descriptor
	 */
	second_level_table = first_level_descriptor & 0xFFFFFC00;
	second_level_descriptor_address = (uint32_t*) (second_level_table | (second_level_index << 2));
	second_level_descriptor = *((uint32_t*) second_level_descriptor_address);
	/*
	 Translation
	 fault
	*/
	if(! (second_level_descriptor & 0x3)) {
		return (uint32_t) FORBIDDEN_ADDRESS;
	}
	/*
	 Physical
	 address
	 */
	pa = (second_level_descriptor & 0xFFFFF000) | page_index;
	return pa; 
}
