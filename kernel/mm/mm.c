#include "mm.h"
#include "string.h"
#include "stdio.h"
#include <stdint.h>

// Memory map
#define MAX_MEMORY_ENTRIES 256
static memory_map_entry_t memory_map[MAX_MEMORY_ENTRIES];
static uint32_t memory_map_size = 0;
static uint32_t total_memory = 0;
static uint32_t used_memory = 0;

// Initialize memory manager
void mm_init(uint32_t mem_upper) {
    // Initialize memory map
    memory_map_size = 1;
    memory_map[0].start = KERNEL_HEAP_START;
    memory_map[0].size = KERNEL_HEAP_END - KERNEL_HEAP_START;
    memory_map[0].type = MEMORY_FREE;
    
    total_memory = mem_upper * 1024;  // Convert KB to bytes
    used_memory = 0;
    
    printf("Memory manager initialized. Total memory: %u KB\n", mem_upper);
}

// Find a free memory block of at least 'size' bytes
static memory_map_entry_t* find_free_block(size_t size) {
    for (uint32_t i = 0; i < memory_map_size; i++) {
        if (memory_map[i].type == MEMORY_FREE && memory_map[i].size >= size) {
            return &memory_map[i];
        }
    }
    return NULL;
}

// Allocate memory block
void* kmalloc(size_t size) {
    // Align size to 4 bytes
    size = (size + 3) & ~3;
    
    memory_map_entry_t* block = find_free_block(size);
    if (!block) {
        printf("kmalloc: Out of memory!\n");
        return NULL;
    }
    
    // If the block is larger than needed, split it
    if (block->size > size + sizeof(memory_map_entry_t)) {
        // Create a new free block after the allocated one
        memory_map_entry_t* new_block = (memory_map_entry_t*)((uint8_t*)block + size + sizeof(memory_map_entry_t));
        new_block->start = block->start + size + sizeof(memory_map_entry_t);
        new_block->size = block->size - size - sizeof(memory_map_entry_t);
        new_block->type = MEMORY_FREE;
        
        // Update the current block
        block->size = size;
        
        // Insert the new block into the memory map
        if (memory_map_size < MAX_MEMORY_ENTRIES - 1) {
            // Shift existing entries to make room
            for (uint32_t i = memory_map_size; i > 0; i--) {
                if (memory_map[i-1].start < new_block->start) {
                    memory_map[i] = *new_block;
                    break;
                }
                memory_map[i] = memory_map[i-1];
            }
            memory_map_size++;
        }
    }
    
    block->type = MEMORY_USED;
    used_memory += block->size;
    
    return (void*)(uintptr_t)(block->start);
}

// Free allocated memory
void kfree(void* ptr) {
    if (!ptr) return;
    
    // Find the memory block
    for (uint32_t i = 0; i < memory_map_size; i++) {
        if (memory_map[i].start == (uint32_t)(uintptr_t)ptr) {
            memory_map[i].type = MEMORY_FREE;
            used_memory -= memory_map[i].size;
            
            // Try to merge with next block if it's free
            if (i < memory_map_size - 1 && memory_map[i+1].type == MEMORY_FREE) {
                memory_map[i].size += memory_map[i+1].size + sizeof(memory_map_entry_t);
                // Remove the next block from the memory map
                for (uint32_t j = i+1; j < memory_map_size-1; j++) {
                    memory_map[j] = memory_map[j+1];
                }
                memory_map_size--;
            }
            
            // Try to merge with previous block if it's free
            if (i > 0 && memory_map[i-1].type == MEMORY_FREE) {
                memory_map[i-1].size += memory_map[i].size + sizeof(memory_map_entry_t);
                // Remove the current block from the memory map
                for (uint32_t j = i; j < memory_map_size-1; j++) {
                    memory_map[j] = memory_map[j+1];
                }
                memory_map_size--;
            }
            
            return;
        }
    }
    
    printf("kfree: Invalid pointer %p\n", ptr);
}

// Allocate and zero-initialize memory
void* kcalloc(size_t nmemb, size_t size) {
    size_t total_size = nmemb * size;
    void* ptr = kmalloc(total_size);
    if (ptr) {
        memset(ptr, 0, total_size);
    }
    return ptr;
}

// Reallocate memory block
void* krealloc(void* ptr, size_t size) {
    if (!ptr) {
        return kmalloc(size);
    }
    
    if (size == 0) {
        kfree(ptr);
        return NULL;
    }
    
    // Find the memory block
    for (uint32_t i = 0; i < memory_map_size; i++) {
        if (memory_map[i].start == (uint32_t)(uintptr_t)ptr) {
            if (memory_map[i].size >= size) {
                // If the block is already large enough, return it as is
                return ptr;
            }
            
            // Allocate a new block
            void* new_ptr = kmalloc(size);
            if (new_ptr) {
                // Copy the old data to the new block
                memcpy(new_ptr, ptr, memory_map[i].size);
                // Free the old block
                kfree(ptr);
            }
            return new_ptr;
        }
    }
    
    printf("krealloc: Invalid pointer %p\n", ptr);
    return NULL;
}

// Print memory statistics
void mm_print_stats(void) {
    printf("Memory Statistics:\n");
    printf("  Total memory: %u KB\n", total_memory / 1024);
    printf("  Used memory: %u KB\n", used_memory / 1024);
    printf("  Free memory: %u KB\n", (total_memory - used_memory) / 1024);
    
    printf("\nMemory Map (%d entries):\n", memory_map_size);
    for (uint32_t i = 0; i < memory_map_size; i++) {
        printf("  0x%08x - 0x%08x: %s (%u bytes)\n",
               memory_map[i].start,
               memory_map[i].start + memory_map[i].size - 1,
               memory_map[i].type == MEMORY_FREE ? "FREE" : "USED",
               memory_map[i].size);
    }
}
