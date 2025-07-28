#include "process.h"
#include "mm/mm.h"
#include "string.h"
#include "stdio.h"

// Process table
static pcb_t process_table[MAX_PROCESSES];
static pcb_t *current_process = NULL;
static pcb_t *ready_queue = NULL;
static uint32_t next_pid = 1;

// Default stack size for processes (8KB for 64-bit)
#define PROCESS_STACK_SIZE 8192

// Initialize the process system
void process_init(void) {
    // Clear the process table
    memset(process_table, 0, sizeof(process_table));
    
    // Initialize the idle process (PID 0)
    pcb_t *idle = &process_table[0];
    idle->pid = 0;
    idle->state = PROCESS_RUNNING;
    idle->priority = 0;
    idle->time_slice = 10;
    
    current_process = idle;
    ready_queue = NULL;
    
    printf("Process system initialized\n");
}

// Create a new process
uint32_t process_create(void (*entry)(void), uint32_t priority) {
    // Find a free process slot
    pcb_t *proc = NULL;
    for (int i = 1; i < MAX_PROCESSES; i++) {
        if (process_table[i].state == 0) {  // Unused
            proc = &process_table[i];
            break;
        }
    }
    
    if (!proc) {
        printf("process_create: No free process slots\n");
        return 0;  // No free slots
    }
    
    // Allocate stack
    void *stack = kmalloc(PROCESS_STACK_SIZE);
    if (!stack) {
        printf("process_create: Failed to allocate stack\n");
        return 0;
    }
    
    // Initialize process control block
    proc->pid = next_pid++;
    proc->state = PROCESS_READY;
    proc->priority = priority;
    proc->time_slice = 10;  // Default time slice
    proc->stack = stack;
    
    // Set up the initial stack frame for x86_64
    uint64_t *stack_top = (uint64_t *)((uint8_t *)stack + PROCESS_STACK_SIZE);
    
    // Make space for the interrupt frame (x86_64)
    stack_top -= 5;  // Space for SS, RSP, RFLAGS, CS, RIP
    
    // Set up the initial stack frame for iretq
    stack_top[4] = 0x10;             // SS (kernel data segment)
    stack_top[3] = (uint64_t)stack_top + 5 * sizeof(uint64_t); // RSP after iretq
    stack_top[2] = 0x202;            // RFLAGS (interrupts enabled)
    stack_top[1] = 0x08;             // CS (kernel code segment)
    stack_top[0] = (uint64_t)entry;   // RIP (entry point)
    
    // Set the stack pointer to the top of the stack
    proc->rsp = (uint64_t)stack_top;
    proc->rbp = proc->rsp;
    
    // Add to ready queue
    proc->next = ready_queue;
    ready_queue = proc;
    
    printf("Created process %u\n", proc->pid);
    return proc->pid;
}

// Context switch (implemented in assembly)
__attribute__((naked)) void context_switch(uint64_t *old_rsp, uint64_t *old_rbp, uint64_t new_rsp, uint64_t new_rbp) {
    (void)old_rsp; (void)old_rbp; (void)new_rsp; (void)new_rbp; // Suppress unused parameter warnings
    
    __asm__ __volatile__ (
        // Save current context
        "pushfq                  \n"  // Save RFLAGS
        "pushq %%rax             \n"  // Save RAX
        "pushq %%rbx             \n"  // Save RBX
        "pushq %%rcx             \n"  // Save RCX
        "pushq %%rdx             \n"  // Save RDX
        "pushq %%rsi             \n"  // Save RSI
        "pushq %%rdi             \n"  // Save RDI
        "pushq %%r8              \n"  // Save R8
        "pushq %%r9              \n"  // Save R9
        "pushq %%r10             \n"  // Save R10
        "pushq %%r11             \n"  // Save R11
        "pushq %%r12             \n"  // Save R12
        "pushq %%r13             \n"  // Save R13
        "pushq %%r14             \n"  // Save R14
        "pushq %%r15             \n"  // Save R15
        
        // Save current RSP and RBP
        "movq %%rsp, (%%rdi)     \n"  // Save RSP to *old_rsp
        "movq %%rbp, (%%rsi)     \n"  // Save RBP to *old_rbp
        
        // Load new RSP and RBP
        "movq %%rdx, %%rsp       \n"  // Load new_rsp into RSP
        "movq %%rcx, %%rbp       \n"  // Load new_rbp into RBP
        
        // Restore new context
        "popq %%r15              \n"  // Restore R15
        "popq %%r14              \n"  // Restore R14
        "popq %%r13              \n"  // Restore R13
        "popq %%r12              \n"  // Restore R12
        "popq %%r11              \n"  // Restore R11
        "popq %%r10              \n"  // Restore R10
        "popq %%r9               \n"  // Restore R9
        "popq %%r8               \n"  // Restore R8
        "popq %%rdi              \n"  // Restore RDI
        "popq %%rsi              \n"  // Restore RSI
        "popq %%rdx              \n"  // Restore RDX
        "popq %%rcx              \n"  // Restore RCX
        "popq %%rbx              \n"  // Restore RBX
        "popq %%rax              \n"  // Restore RAX
        "popfq                   \n"  // Restore RFLAGS
        
        // Return to the new context
        "retq                    \n"
        : // No outputs (we write through pointers)
        : "D" (old_rsp), "S" (old_rbp), "d" (new_rsp), "c" (new_rbp)
        : "memory"
    );
}

// Schedule the next process to run
void process_schedule(void) {
    if (!ready_queue) {
        // No processes to run, just return
        return;
    }
    
    // Save current process state
    pcb_t *prev = current_process;
    
    // Get the next process from the ready queue
    pcb_t *next = ready_queue;
    ready_queue = ready_queue->next;
    
    // If the previous process is still runnable, add it to the end of the queue
    if (prev->state == PROCESS_RUNNING) {
        prev->state = PROCESS_READY;
        prev->next = NULL;
        
        // Find the end of the queue
        pcb_t *p = ready_queue;
        if (p) {
            while (p->next) p = p->next;
            p->next = prev;
        } else {
            ready_queue = prev;
        }
    }
    
    // Switch to the next process
    next->state = PROCESS_RUNNING;
    current_process = next;
    
    // Perform the context switch
    context_switch(&prev->rsp, &prev->rbp, next->rsp, next->rbp);
    
    // When we return here, we're running in the context of the new process
}

// Terminate the current process
void process_exit(int status) {
    printf("Process %u exited with status %d\n", current_process->pid, status);
    
    // Free the process's stack
    if (current_process->stack) {
        kfree(current_process->stack);
    }
    
    // Mark the process as a zombie
    current_process->state = PROCESS_ZOMBIE;
    
    // Schedule the next process
    process_schedule();
    
    // We should never get here
    for (;;) __asm__ volatile("hlt");
}

// Yield the CPU to another process
void process_yield(void) {
    __asm__ volatile("int $0x20");  // Trigger a software interrupt
}

// Get the current process
pcb_t* process_current(void) {
    return current_process;
}

// Simple sleep function (not very accurate)
void process_sleep(uint32_t ms) {
    // This is a very simple implementation that just yields
    // In a real OS, this would use a timer and block the process
    volatile uint32_t count = ms * 1000;
    while (count-- > 0) {
        __asm__ volatile("pause");
    }
    process_yield();
}
