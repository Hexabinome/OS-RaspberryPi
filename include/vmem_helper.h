#ifndef VMEM_HELPER_H
#define VMEM_HELPER_H

#include <stdint.h>

/* **** Constants **** */
static const uint8_t FIRST_LVL_IDX_BEGIN = 20;
static const uint8_t SECOND_LVL_IDX_BEGIN = 12;
static const uint8_t SECOND_LVL_IDX_LEN = 0xFF;
static const uint16_t PAGE_IDX_LEN = 0xFFF;

static const uint32_t FIRST_LVL_ADDR_MASK = 0xFFFFC000; // last 14 bits to 0
static const uint32_t SECOND_LVL_ADDR_MASK = 0xFFFFFC00; // last 10 bits to 0
static const uint32_t PHY_ADDR_MASK = 0xFFFFF000; // last 12 bits to 0

/* **** Prototypes **** */

// Get first level descriptor address [table_base|first_index|00]
uint32_t* get_first_lvl_descriptor_addr(uint32_t** table_base, uint32_t log_addr);

// Get first level descriptor from beginning [table2_addr|flags]
uint32_t* get_first_lvl_descriptor(uint32_t** table_base, uint32_t log_addr);
// Get first level descriptor from last step [table2_addr|flags]
uint32_t* get_first_lvl_descriptor_from(uint32_t* first_lvl_descriptor_addr);

// Get second level descriptor address from beginning [table2_addr|second_index|00]
uint32_t* get_second_lvl_descriptor_addr(uint32_t** table_base, uint32_t log_addr);
// Get second level descriptor address from last step [table2_addr|second_index|00]
uint32_t* get_second_lvl_descriptor_addr_from(uint32_t* first_lvl_descriptor, uint32_t log_addr);

// Get second level descriptor from beginning [ram_addr|flags]
uint32_t* get_second_lvl_descriptor(uint32_t** table_base, uint32_t log_addr);
// Get second level descriptor from last step [ram_addr|flags]
uint32_t* get_second_lvl_descriptor_from(uint32_t* second_lvl_descriptor_addr);

// Get physical address from beginning [ram_addr|page_index]
uint32_t* get_phy_addr(uint32_t** table_base, uint32_t log_addr);
// Get physical address from last step [ram_addr|page_index]
uint32_t* get_phy_addr_from(uint32_t* second_lvl_descriptor, uint32_t log_addr);

#endif // VMEM_HELPER_H
