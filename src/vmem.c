#include "vmem.h"
#include "kheap.h"
#include "config.h" // NULL
#include "vmem_helper.h"
#include "util.h"


static unsigned int MMUTABLEBASE; /* Page table address */
static uint8_t* frame_table; /* frame occupation table */

static const uint32_t kernel_heap_end = 0x1000000; // TODO use __kernel_heap_end__

static const uint8_t first_table_flags = 1; // 0b0000000001
static const uint16_t kernel_flags = 0b000001010010;
static const uint16_t device_flags = 0b010000010110;

static uint32_t** get_table_base(struct pcb_s* process)
{
	uint32_t** table_base;
	if (process == NULL)
	{
		__asm("mrc p15, 0, %[tb], c2, c0, 0" : [tb] "=r"(table_base));
	}
	else
	{
		table_base = process->page_table;
	}
	
	return table_base;
}

static uint8_t is_forbidden_address(uint32_t addr)
{
	return !(addr & 0x3);
}

static void set_second_table_value(uint32_t** table_base, uint32_t log_addr, uint32_t phy_addr)
{
    /* Indexes */
    uint32_t second_level_index;
    /* Descriptors */
    uint32_t first_level_descriptor;
    uint32_t* second_level_descriptor_address;
    uint32_t* second_level_table;

    first_level_descriptor = (uint32_t) get_first_lvl_descriptor(table_base, log_addr);

    second_level_index = (log_addr >> 12) & 0xFF;
    second_level_table = (uint32_t*)(first_level_descriptor & 0xFFFFFC00); // keep from bit 12 to 31
    second_level_descriptor_address = (uint32_t*) ((uint32_t)second_level_table | (second_level_index << 2));
    *second_level_descriptor_address = (phy_addr & 0xFFFFF000) | kernel_flags;
// TODO okay ?
}

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
	
	/* Descriptors */
	uint32_t* first_level_descriptor;
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
        first_level_descriptor_address = get_first_lvl_descriptor_addr(table_base, log_addr);
        *first_level_descriptor_address |= first_table_flags;

        first_level_descriptor = get_first_lvl_descriptor_from(first_level_descriptor_address);

        second_level_descriptor_address = get_second_lvl_descriptor_addr_from(first_level_descriptor, log_addr);
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
        first_level_descriptor_address = get_first_lvl_descriptor_addr(table_base, log_addr);
        *first_level_descriptor_address |= first_table_flags;

        first_level_descriptor = get_first_lvl_descriptor_from(first_level_descriptor_address);

        second_level_descriptor_address = get_second_lvl_descriptor_addr_from(first_level_descriptor, log_addr);
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
		ft[i] = FRAME_OCCUPIED;
	}
	for (; i < frame_devices_start; ++i)
	{
		ft[i] = FRAME_FREE;
	}
	for (i = frame_devices_start; i <= frame_devices_end; ++i)
	{
		ft[i] = FRAME_OCCUPIED;
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
	uint32_t** table_base = get_table_base(process);
	
	// Test if first level descriptor is mapped
    uint32_t* first_lvl_desc = get_first_lvl_descriptor(table_base, va);
    if (is_forbidden_address((uint32_t) first_lvl_desc))
    {
		return (uint32_t) FORBIDDEN_ADDRESS_TABLE1;
	}
	
	// Test if second level descriptor is mapped to physical memory
	uint32_t* second_lvl_desc = get_second_lvl_descriptor_from(get_second_lvl_descriptor_addr_from(first_lvl_desc, va));
	if (is_forbidden_address((uint32_t) second_lvl_desc))
    {
		return (uint32_t) FORBIDDEN_ADDRESS;
	}
	
	
    return (uint32_t) get_phy_addr_from(second_lvl_desc, va);
}


uint32_t vmem_alloc_for_userland(struct pcb_s* process, uint32_t size)
{
	uint32_t** table_base = get_table_base(process);
	uint32_t nb_page = (uint32_t)(size/FRAME_SIZE)+1;
	
	uint32_t log_addr, i, j;
	uint32_t first_page, last_page;
	uint32_t* first_page_table;
	uint32_t* last_page_table;
	uint32_t begin_phys_addr, phys_addr;
	uint32_t* first_level_descriptor_address;
	uint8_t is_consecutive;
	
	// Find a free space in logical addresses
	for(log_addr = 0x1000000; log_addr < 0x1FFFFFFF; log_addr += (1 << 12)) // iterate over each logical address, increment by page size (2^12 = 4096)
	// TODO iterate after devices range (cuz logical address)
	{
		is_consecutive = TRUE;
		begin_phys_addr = vmem_translate(log_addr, process);
		// If corresponding physical address is already taken, skip
		if (begin_phys_addr != (uint32_t) FORBIDDEN_ADDRESS && begin_phys_addr != (uint32_t) FORBIDDEN_ADDRESS_TABLE1)
		{
			continue;
		}
		// If is empty, check if the next (nb_page - 1) pages are also free
		for (i = 1; i < nb_page; ++i)
		{
			phys_addr = vmem_translate(log_addr + (i << 12), process);
			// If one page is not free : not consecutive
			if (phys_addr != (uint32_t) FORBIDDEN_ADDRESS && phys_addr != (uint32_t) FORBIDDEN_ADDRESS_TABLE1)
			{
				is_consecutive = FALSE;
				break;
			}
			
		}
		// Only if the right number of pages is free, we exit the search for space
		if (is_consecutive)
		{
			first_page = log_addr;
			first_page_table = get_first_lvl_descriptor_addr(table_base, first_page);
			last_page = log_addr + ((nb_page-1) << 12);
			last_page_table = get_first_lvl_descriptor_addr(table_base, last_page);
			break;
		}
	}
	
	// TODO if log_addr has exceeded 0x1F FF FF FF : no available memory
	 

	// Iterate over each table 1 address, to find if the table needs to be allocated
	for (first_level_descriptor_address = first_page_table;
		first_level_descriptor_address <= last_page_table;
		first_level_descriptor_address += 1)
	{
		if (is_forbidden_address(*first_level_descriptor_address))
		{
			// alloc new 2nd table page lvl
			*(first_level_descriptor_address) = (uint32_t) kAlloc_aligned(SECON_LVL_TT_SIZE, SECON_LVL_TT_ALIG);
			*(first_level_descriptor_address) |= first_table_flags;
		}
	}
	
	uint32_t free_frame = NULL;
	for(i = 0; i < nb_page; i++)
	{
		for(j = 0; j < FRAME_TABLE_SIZE; j++)
		{
			if(frame_table[j] == FRAME_FREE)
			{
				frame_table[j] = FRAME_OCCUPIED;
				// find the physical adress of the frame
				free_frame = j*4096;
				break;
			}
		}
		//COUILLE ICI, on alloue 1 page en trop. Ou pas, parce addr phy 0x1000000 occupé / kernel
		set_second_table_value(table_base, first_page + (i<<12), free_frame);
	}
	return (uint32_t)first_page;
}

static uint8_t is_table2_free(uint32_t* first_lvl_desc)
{
	uint32_t* second_lvl_desc;
	uint32_t i, log_addr;
	
	for (i = 0; i <  SECON_LVL_TT_COUN; ++i)
	{
		log_addr = i << 12;
		second_lvl_desc = get_second_lvl_descriptor_from(get_second_lvl_descriptor_addr_from(first_lvl_desc, log_addr));
		
		if (! is_forbidden_address((uint32_t) second_lvl_desc))
		{
			return FALSE;
		}
	}
	
	return TRUE;
}

void vmem_free(uint8_t* vAddress, struct pcb_s* process, unsigned int size)
{
	uint32_t** table_base = (uint32_t**)( (uint32_t)get_table_base(process) & 0xFFFFC000);
	
	uint32_t* first_lvl_desc_addr = get_first_lvl_descriptor_addr(table_base, (uint32_t) vAddress);
	uint32_t* first_lvl_desc = get_first_lvl_descriptor_from(first_lvl_desc_addr);
	
	uint32_t nb_page = (size/FRAME_SIZE)+1;
	uint32_t max_log_addr = (uint32_t) vAddress + (nb_page << 12);
	uint32_t log_addr;
	// Set second level descriptors to forbidden address & free frame occupation table
	for (log_addr = (uint32_t) vAddress; log_addr < max_log_addr; log_addr += (1 << 12))
	{	
		// Get second level descriptor address
		uint32_t* second_lvl_desc_addr = get_second_lvl_descriptor_addr_from(first_lvl_desc, log_addr);
		
		// Free frame occupation table
		uint32_t* phy_addr = get_phy_addr_from(get_second_lvl_descriptor_from(second_lvl_desc_addr), log_addr);
		uint32_t frame_occupation_idx = (uint32_t) phy_addr / (uint32_t)FRAME_SIZE;
		if (frame_table[frame_occupation_idx] == FRAME_FREE)
		{
			PANIC();
		}
		frame_table[frame_occupation_idx] = FRAME_FREE;
		
		// Set entry to forbidden address
		*second_lvl_desc_addr = NULL;
	}
	

	uint32_t* curr_first_lvl_desc;
	uint32_t* curr_first_lvl_desc_addr;
	uint32_t* last_first_lvl_desc_addr = get_first_lvl_descriptor_addr(table_base, (uint32_t) vAddress + ((nb_page-1) << 12));
	// Iterate over each table 2 implicated this operation (each table which might be freed)
	for (curr_first_lvl_desc_addr = first_lvl_desc_addr;
			curr_first_lvl_desc_addr <= last_first_lvl_desc_addr;
			curr_first_lvl_desc_addr += (1 << 2))
	{
		curr_first_lvl_desc = get_first_lvl_descriptor_from(curr_first_lvl_desc_addr);
		if (is_table2_free(curr_first_lvl_desc))
		{
			kFree((uint8_t*) curr_first_lvl_desc, SECON_LVL_TT_SIZE); // Free table 2
			*curr_first_lvl_desc_addr = NULL; // First level descriptor = 0 (will be an forbidden address table1)
		}
	}
}
