#pragma once
#include <stdint.h>

#define IDT_ENTRIES 256

// 64-bit IDT Gate Descriptor
struct idt_entry {
    uint16_t offset_low;   // Offset bits 0..15
    uint16_t selector;     // Code segment selector in GDT
    uint8_t  ist;          // Interrupt Stack Table offset (0 for no IST)
    uint8_t  type_attr;    // Type and attributes
    uint16_t offset_mid;   // Offset bits 16..31
    uint32_t offset_high;  // Offset bits 32..63
    uint32_t reserved;     // Reserved
} __attribute__((packed));

// IDT pointer structure for LIDT
struct idt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

// Initialize IDT
void idt_init(void);

// Set an IDT gate
void idt_set_gate(int n, uint64_t handler, uint16_t sel, uint8_t flags);

// PIC remapping and control
void pic_remap(void);
void pic_send_eoi(unsigned char irq);

// IRQ handler management
typedef void (*irq_handler_t)(void);
void irq_install_handler(int irq, irq_handler_t handler);

// Default IRQ handlers (weak symbols that can be overridden)
void irq0_handler(void);
void irq1_handler(void);
void irq2_handler(void);
void irq3_handler(void);
void irq4_handler(void);
void irq5_handler(void);
void irq6_handler(void);
void irq7_handler(void);
void irq8_handler(void);
void irq9_handler(void);
void irq10_handler(void);
void irq11_handler(void);
void irq12_handler(void);
void irq13_handler(void);
void irq14_handler(void);
void irq15_handler(void);