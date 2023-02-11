#pragma once

/** @defgroup liblm liblm
 * @{
 *
 * Functions related to low memory (first 1 MB of physical memory), required for BIOS
 */

#include <stdbool.h>

#include <minix/syslib.h>
#include <minix/type.h>

#define PB2BASE(x) (((x) >> 4) & 0x0F000) // phys_bytes to segment base
#define PB2OFF(x) ((x) & (0x0FFFF))       // phys_bytes to segment offset

/** @name Memory Map Struct*/
/**@{
 *
 * Struct that keeps info regarding the mapping of physical memory to virtual memory
 */
typedef struct {
  phys_bytes phys; /**< @brief physical address */
  void *virt;      /**< @brief virtual address */
  size_t size;     /**< @brief size of mapped memory region */
} mmap_t;

/** @} end of Memory Map Struct */

/**
 * @brief Allocates a memory block in low memory area with the specified size
 * 
 * Allocates a memory block in the region up to the 1 MByte physical address with the input size,
 * and initializes the input mmap_t struct with the mapping information.
 * 
 * @param size size of the memory block to allocate
 * @param map pointer to mmap_t data structure, which represents the memory map
 * @return the virtual address of the memory block on success, NULL otherwise
 */
void *lm_alloc(size_t size, mmap_t *map);

/**
 * @brief Frees a memory block in the low memory area, previously allocated using lm_alloc()
 * 
 * Frees a memory block in the region up to the 1 MByte physical address, previously
 *  allocated using lm_alloc(). Takes as input the address of the mmap_t structure that
 *  was passed to lm_alloc(), and that must have not been modified since.
 * 
 * @param map pointer to mmap_t data structure of the block being freed
 * @return a boolean indicating whether memory deallocation succeeded
 */
bool lm_free(const mmap_t *map);

/** @} end of liblm */
