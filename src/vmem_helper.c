#include "vmem_helper.h"

/* **** Internal constants **** */
static const uint8_t FIRST_LVL_IDX_BEGIN = 20;
static const uint8_t SECOND_LVL_IDX_BEGIN = 12;
static const uint8_t SECOND_LVL_IDX_LEN = 0xFF;
static const uint16_t PAGE_IDX_LEN = 0xFFF;

static const uint32_t FIRST_LVL_ADDR_MASK = 0xFFFFC000; // last 14 bits to 0
static const uint32_t SECOND_LVL_ADDR_MASK = 0xFFFFFC00; // last 10 bits to 0
static const uint32_t PHY_ADDR_MASK = 0xFFFFF000; // last 12 bits to 0

/* **** Internal functions **** */

static uint32_t get_first_index(uint32_t log_addr)
{
    return log_addr >> FIRST_LVL_IDX_BEGIN;
}

static uint32_t get_second_index(uint32_t log_addr)
{
    return (log_addr >> SECOND_LVL_IDX_BEGIN) & SECOND_LVL_IDX_LEN;
}

static uint32_t get_page_index(uint32_t log_addr)
{
    return log_addr & PAGE_IDX_LEN;
}

/* **** Exposed functions **** */

uint32_t* get_first_lvl_descriptor_addr(uint32_t** table_base, uint32_t log_addr)
{
    table_base = (uint32_t**)((uint32_t)table_base & FIRST_LVL_ADDR_MASK);

    uint32_t first_lvl_idx = get_first_index(log_addr);

    return (uint32_t*) ((uint32_t)table_base | (first_lvl_idx << 2));
}

uint32_t* get_first_lvl_descriptor(uint32_t** table_base, uint32_t log_addr)
{
    return (uint32_t*) (*get_first_lvl_descriptor_addr(table_base, log_addr));
}
uint32_t* get_first_lvl_descriptor_from(uint32_t* first_lvl_descriptor_addr)
{
    return (uint32_t*) (*first_lvl_descriptor_addr);
}

uint32_t* get_second_lvl_descriptor_addr(uint32_t** table_base, uint32_t log_addr)
{
    uint32_t first_lvl_desc = ((uint32_t)get_first_lvl_descriptor(table_base, log_addr)) & SECOND_LVL_ADDR_MASK;

    uint32_t second_lvl_idx = get_second_index(log_addr);

    return (uint32_t*) (first_lvl_desc | (second_lvl_idx << 2));
}
uint32_t* get_second_lvl_descriptor_addr_from(uint32_t* first_lvl_descriptor, uint32_t log_addr)
{
    uint32_t first_lvl_desc = ((uint32_t)first_lvl_descriptor) & SECOND_LVL_ADDR_MASK;

    uint32_t second_lvl_idx = get_second_index(log_addr);

    return (uint32_t*) (first_lvl_desc | (second_lvl_idx << 2));
}

uint32_t* get_second_lvl_descriptor(uint32_t** table_base, uint32_t log_addr)
{
    return (uint32_t*) (*get_second_lvl_descriptor_addr(table_base, log_addr));
}
uint32_t* get_second_lvl_descriptor_from(uint32_t* second_lvl_descriptor_addr)
{
    return (uint32_t*) (*second_lvl_descriptor_addr);
}

uint32_t* get_phy_addr(uint32_t** table_base, uint32_t log_addr)
{
    uint32_t second_lvl_desc = ((uint32_t )get_second_lvl_descriptor(table_base, log_addr)) & PHY_ADDR_MASK;

    uint32_t page_idx = get_page_index(log_addr);

    return (uint32_t*) (second_lvl_desc | page_idx);
}
uint32_t* get_phy_addr_from(uint32_t* second_lvl_descriptor, uint32_t log_addr)
{
    uint32_t second_lvl_desc = ((uint32_t)second_lvl_descriptor) & PHY_ADDR_MASK;

    uint32_t page_idx = get_page_index(log_addr);

    return (uint32_t*) (second_lvl_desc | page_idx);
}
