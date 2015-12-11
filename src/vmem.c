#include "vmem.h"
#include "kheap.h"
#include "config.h" // NULL


static unsigned int MMUTABLEBASE; /* Page table address */
static uint8_t* frame_table; /* frame occupation table */

static const uint32_t kernel_heap_end = 0x1000000; // TODO use __kernel_heap_end__

static const uint8_t first_table_flags = 1; // 0b0000000001
static const uint16_t kernel_flags = 0b000001010010;
static const uint16_t device_flags = 0b010000010110;
	

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
	// Alloc first table
	uint32_t** table_base =(uint32_t**) kAlloc_aligned(FIRST_LVL_TT_SIZE, FIRST_LVL_TT_ALIG);
	
	uint32_t* second_level_table;
	
	/* Indexes */
	uint32_t first_level_index;
	uint32_t second_level_index;
	/* Descriptors */
	uint32_t first_level_descriptor;
	uint32_t* first_level_descriptor_address;
	uint32_t* second_level_descriptor_address;
	
	uint32_t log_addr;
	
	// ** Init kernel pages
	uint32_t first_page = 0;
	// Number of second level table to store all devices adress
	uint32_t nbPage = (uint32_t)(kernel_heap_end / FRAME_SIZE / SECON_LVL_TT_SIZE);
	uint32_t i;
	
	// Alloc table pages for kernel
	for(i = first_page; i < first_page+nbPage; ++i) 
	{
		first_level_descriptor_address = (uint32_t*) ((uint32_t)table_base | (i << 2));
		(*first_level_descriptor_address) = (uint32_t) kAlloc_aligned(SECON_LVL_TT_SIZE, SECON_LVL_TT_ALIG);
	}
	// Fill second level tables
	for (log_addr = 0; log_addr < kernel_heap_end; log_addr++)
	{
		first_level_index = log_addr >> 20;
		first_level_descriptor_address = (uint32_t*) ((uint32_t)table_base | (first_level_index << 2));
		*(first_level_descriptor_address) |= first_table_flags;
		first_level_descriptor = *(first_level_descriptor_address);
		
		second_level_index = (log_addr >> 12) & 0xFF;
		second_level_table = (uint32_t*)(first_level_descriptor & 0xFFFFFC00); // keep from bit 12 to 31
		second_level_descriptor_address = (uint32_t*) ((uint32_t)second_level_table | (second_level_index << 2));
		*second_level_descriptor_address = (log_addr & 0xFFFFF000) | kernel_flags;
	}
	
	// ** Init devices pages
	first_page = 0x20000000 >> 20;
	// Number of second level table to store all devices adress
	nbPage = (uint32_t)(0xFFFFFF/ FRAME_SIZE / SECON_LVL_TT_SIZE);
	
	// Alloc table pages for devices
	for(i = first_page; i < first_page+nbPage; ++i) 
	{
		first_level_descriptor_address = (uint32_t*) ((uint32_t)table_base | (i << 2));
		(*first_level_descriptor_address) = (uint32_t) kAlloc_aligned(SECON_LVL_TT_SIZE, SECON_LVL_TT_ALIG);
	}
	// Fill second level tables
	for(log_addr = 0x20000000; log_addr < 0x20FFFFFF; log_addr++)
	{
		first_level_index = log_addr >> 20;
		first_level_descriptor_address = (uint32_t*) ((uint32_t)table_base | (first_level_index << 2));
		*(first_level_descriptor_address) |= first_table_flags;
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
	unsigned int frame_kernel_heap_end = kernel_heap_end / FRAME_SIZE;
	unsigned int frame_devices_start = 0x20000000 / FRAME_SIZE;
	unsigned int frame_devices_end = 0x20FFFFFF / FRAME_SIZE;
	
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
	uint32_t condition = (first_level_descriptor & 0x3);
	if(! condition ) {
		return (uint32_t) FORBIDDEN_ADDRESS_TABLE1;
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


uint32_t vmem_alloc_for_userland(struct pcb_s* process, uint32_t size)
{
	uint32_t nbPage = (uint32_t)(size/PAGE_SIZE)+1;
	uint32_t** table_base;
	if (process == NULL)
	{
		__asm("mrc p15, 0, %[tb], c2, c0, 0" : [tb] "=r"(table_base));
	}
	else
	{
		table_base = process->page_table;
	}
	uint32_t i, j;
	uint32_t first_page;
	uint32_t phys_addr;
	uint32_t* first_level_descriptor_address;
	uint32_t consecutive = 0;
	uint8_t page_free;
	//increment by 2^11
	for(i = 0x1000000; i < 0x1FFFFFFF; i=((i>>12)+1)<<12)
	{
		consecutive = 1;
		phys_addr = vmem_translate(i, NULL);
		if(!((phys_addr == (uint32_t)FORBIDDEN_ADDRESS) || (phys_addr == (uint32_t)FORBIDDEN_ADDRESS_TABLE1)))
		{
			continue;
		}
		for(j = 1; j < nbPage; j++)
		{
			phys_addr = vmem_translate(i+(j<<12), NULL);
			page_free = (phys_addr == (uint32_t)FORBIDDEN_ADDRESS) || (phys_addr == (uint32_t)FORBIDDEN_ADDRESS_TABLE1);
			// if one page is not free
			if(!page_free)
			{
				consecutive = 0;
				break;
			}
			
		}
		if(consecutive)
		{
			first_page = i;
			break;
		}
	}
	

	phys_addr = vmem_translate(first_page, NULL);
	if(phys_addr == (uint32_t)FORBIDDEN_ADDRESS_TABLE1)
	{
		// alloc new 2nd table page lvl
		first_level_descriptor_address = (uint32_t*) ((uint32_t)table_base | ( (first_page >> 20) << 2));
		*(first_level_descriptor_address) = (uint32_t) kAlloc_aligned(SECON_LVL_TT_SIZE, SECON_LVL_TT_ALIG);
		*(first_level_descriptor_address) |= first_table_flags;
	}

	
	uint32_t free_frame = NULL;
	for(i = 0; i < nbPage; i++)
	{
		for(j = 0; j < FRAME_TABLE_SIZE; j++)
		{
			if(frame_table[j] == 0)
			{
				frame_table[j] = 1;
				// find the physical adress of the frame
				free_frame = j*4096;
				break;
			}
		}
		//COUILLE ICI, on alloue 1 page en trop
		set_second_table_value(table_base, first_page + (i<<12), free_frame);
	}
	return (uint32_t)first_page;
}


void set_second_table_value(uint32_t** table_base, uint32_t log_addr, uint32_t phy_addr)
{
	/* Indexes */
	uint32_t first_level_index;
	uint32_t second_level_index;
	/* Descriptors */
	uint32_t first_level_descriptor;
	uint32_t* first_level_descriptor_address;
	uint32_t* second_level_descriptor_address;
	uint32_t* second_level_table;

	first_level_index = log_addr >> 20;
	first_level_descriptor_address = (uint32_t*) ((uint32_t)table_base | (first_level_index << 2));
	first_level_descriptor = *(first_level_descriptor_address);
	
	second_level_index = (log_addr >> 12) & 0xFF;
	second_level_table = (uint32_t*)(first_level_descriptor & 0xFFFFFC00); // keep from bit 12 to 31
	second_level_descriptor_address = (uint32_t*) ((uint32_t)second_level_table | (second_level_index << 2));
	*second_level_descriptor_address = (phy_addr & 0xFFFFF000) | kernel_flags;
	
}
