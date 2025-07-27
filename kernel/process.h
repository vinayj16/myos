#ifndef _PROCESS_H
#define _PROCESS_H

#include <stdint.h>

// Process states
#define PROCESS_RUNNING  0
#define PROCESS_READY    1
#define PROCESS_BLOCKED  2
#define PROCESS_ZOMBIE   3

// Maximum number of processes
#define MAX_PROCESSES 64

// Process control block (PCB)
typedef struct process_control_block {
    uint32_t pid;           // Process ID
    uint64_t rsp;           // Stack pointer (64-bit)
    uint64_t rbp;           // Base pointer (64-bit)
    uint64_t rip;           // Instruction pointer (64-bit)
    uint32_t state;         // Process state
    uint32_t priority;      // Process priority
    uint32_t time_slice;    // Time slice counter
    void* stack;            // Process stack
    struct process_control_block* next;  // Next process in the ready queue
} pcb_t;

// Function declarations
void process_init(void);
uint32_t process_create(void (*entry)(void), uint32_t priority);
void process_schedule(void);
void process_exit(int status);
void process_yield(void);
pcb_t* process_current(void);
void process_sleep(uint32_t ms);

#endif // _PROCESS_H
