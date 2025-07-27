#ifndef _MM_H
#define _MM_H

#include <stdint.h>
#include <stddef.h>

// Memory management constants
#define PAGE_SIZE 4096
#define KERNEL_HEAP_START 0x1000000  // 16MB
#define KERNEL_HEAP_END   0x2000000  // 32MB

// Memory map entry types
#define MEMORY_FREE 0
#define MEMORY_USED 1

// Memory map entry structure
typedef struct {
    uint32_t start;
    uint32_t size;
    uint8_t type;
} memory_map_entry_t;

// Function declarations
void mm_init(uint32_t mem_upper);
void* kmalloc(size_t size);
void kfree(void* ptr);
void* kcalloc(size_t nmemb, size_t size);
void* krealloc(void* ptr, size_t size);
void mm_print_stats(void);

#endif // _MM_H
