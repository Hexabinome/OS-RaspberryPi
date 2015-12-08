#include "vmem.h"
#include "kheap.h"
#include "config.h" // NULL

static unsigned int MMUTABLEBASE; /* Page table address */
static uint8_t* frame_table; /* frame occupation table */

void vmem_init()
{
	kheap_init();
	
	MMUTABLEBASE = init_kern_translation_table();
	frame_table = init_frame_occupation_table();
	
	configure_mmu_C();
	start_mmu_C();
	
	
}

unsigned int init_kern_translation_table(void)
{
	uint32_t** table_base =(uint32_t**) kAlloc_aligned(FIRST_LVL_TT_SIZE, FIRST_LVL_TT_ALIG);
	
	uint32_t i;
	for (i = 0; i < FIRST_LVL_TT_COUN; i++)
	{
		table_base[i] = (uint32_t*) kAlloc_aligned(SECON_LVL_TT_SIZE, SECON_LVL_TT_ALIG);
	}
	uint32_t* second_level_table;
	
	uint32_t device_flags = 0b010000010110;
	
	/* Indexes */
	uint32_t first_level_index;
	uint32_t second_level_index;
	/* Descriptors */
	uint32_t first_level_descriptor;
	uint32_t* first_level_descriptor_address;
	uint32_t* second_level_descriptor_address;
	
	uint32_t log_addr;
	
	// init kern pages
	for (log_addr = 0; log_addr < 0x1000000; log_addr++) // TODO __kernel_heap_end__ ???
	{
		first_level_index = log_addr >> 20;
		first_level_descriptor_address = (uint32_t*) ((uint32_t)table_base | (first_level_index << 2));
		*(first_level_descriptor_address) |= 0b0000000001;
		first_level_descriptor = *(first_level_descriptor_address);
		
		second_level_index = (log_addr >> 12) & 0xFF;
		second_level_table = (uint32_t*)(first_level_descriptor & 0xFFFFFC00); // keep from bit 12 to 31
		second_level_descriptor_address = (uint32_t*) ((uint32_t)second_level_table | (second_level_index << 2));
		*second_level_descriptor_address = (log_addr & 0xFFFFF000) | 0b000001010010;
	}
	
	// init devices pages
	for(log_addr = 0x20000000; log_addr < 0x20FFFFFF; log_addr++)
	{
		first_level_index = log_addr >> 20;
		first_level_descriptor_address = (uint32_t*) ((uint32_t)table_base | (first_level_index << 2));
		*(first_level_descriptor_address) |= 0b0000000001;
		first_level_descriptor = *(first_level_descriptor_address);
		
		
		second_level_index = (log_addr >> 12) & 0xFF;
		second_level_table = (uint32_t*)(first_level_descriptor & 0xFFFFFC00);
		second_level_descriptor_address = (uint32_t*) ((uint32_t)second_level_table | (second_level_index << 2));
		*second_level_descriptor_address = (log_addr & 0xFFFFF000) | device_flags;
	}
	return (unsigned int)table_base;
	
}

uint8_t* init_frame_occupation_table(void)
{
	uint8_t* ft = kAlloc(FRAME_TABLE_SIZE);
	
	unsigned int i;
	unsigned int frame_kernel_heap_end = 0x1000000 / 4;
	unsigned int frame_devices_start = 0x20000000 / 4;
	unsigned int frame_devices_end = 0x20FFFFFF / 4;
	
	for (i = 0; i <= frame_kernel_heap_end; ++i)
	{
		ft[i] = 1;
	}
	for (; i < frame_devices_start; ++i)
	{
		ft[i] = 0;
	}
	for (i = frame_devices_start; i <= frame_devices_end; ++i)
	{
		ft[i] = 1;
	}
	
	return ft;
}	

void configure_mmu_C()
{
	register unsigned int pt_addr = MMUTABLEBASE;
	
	/* Translation table 0 */
	__asm volatile("mcr p15, 0, %[addr], c2, c0, 0" : : [addr] "r" (pt_addr));
	
	/* Translation table 1 */
	__asm volatile("mcr p15, 0, %[addr], c2, c0, 1" : : [addr] "r" (pt_addr));
	
	/* Use translation table 0 for everything */
	__asm volatile("mcr p15,0, %[n], c2, c0, 2" : : [n] "r" (0));
	
	/* Set Domain 0 ACL to "Manager", not enforcing memory permissions* Every mapped section/page is in domain 0*/
	__asm volatile("mcr p15, 0, %[r], c3, c0, 0" : : [r] "r" (0x3));
}

void start_mmu_C()
{
	register unsigned int control;

	
	__asm("mcr p15, 0, %[zero], c1, c0, 0" : : [zero] "r"(0)); /* Disable cache */
	__asm("mcr p15, 0, r0, c7, c7, 0"); /* Invalidate cache (data and instructions) */
	__asm("mcr p15, 0, r0, c8, c7, 0"); /* Invalidate TLB entries */
	
	/* Enable ARMv6 MMU features (disable sub-page AP) */
	control = (1<<23) | (1 << 15) | (1 << 4) | 1;
	
	/* Invalidate the translation lookaside buffer (TLB) */
	__asm volatile("mcr p15, 0, %[data], c8, c7, 0" : : [data] "r" (0));
	
	/* Write control register */
	// doc at http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0333h/I1031142.html
	__asm volatile("mcr p15, 0, %[control], c1, c0, 0" : : [control] "r" (control));
}

uint32_t vmem_translate(uint32_t va, struct pcb_s* process)
{
	uint32_t pa; /* The result */
	
	/* 1st and 2nd table addresses */
	uint32_t table_base;
	uint32_t second_level_table;
	
	/* Indexes */
	uint32_t first_level_index;
	uint32_t second_level_index;
	uint32_t page_index;
	
	/* Descriptors */
	uint32_t first_level_descriptor;
	uint32_t* first_level_descriptor_address;
	uint32_t second_level_descriptor;
	uint32_t* second_level_descriptor_address;
	
	if (process == NULL)
	{
		__asm("mrc p15, 0, %[tb], c2, c0, 0" : [tb] "=r"(table_base));
	}
	else
	{
		table_base = (uint32_t) process->page_table;
	}
	table_base = table_base & 0xFFFFC000;
	
	/* Indexes */
	first_level_index = (va >> 20);
	second_level_index = ((va << 12) >> 24);
	page_index = (va & 0x00000FFF);
	
	/* First level descriptor */
	first_level_descriptor_address = (uint32_t*) (table_base | (first_level_index << 2));
	first_level_descriptor = *(first_level_descriptor_address);
	
	/* Translation fault */
	if(! (first_level_descriptor & 0x3)) {
		return (uint32_t) FORBIDDEN_ADDRESS;
	}
	
	/* Second level descriptor */
	second_level_table = first_level_descriptor & 0xFFFFFC00;
	second_level_descriptor_address = (uint32_t*) (second_level_table | (second_level_index << 2));
	second_level_descriptor = *((uint32_t*) second_level_descriptor_address);
	
	/* Translation fault */
	if (!(second_level_descriptor & 0x3)) {
		return (uint32_t) FORBIDDEN_ADDRESS;
	}
	
	/* Physical address */
	pa = (second_level_descriptor & 0xFFFFF000) | page_index;
	return pa;
}
