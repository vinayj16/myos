#include <stdint.h>
#include "idt.h"
#include "io.h"

// Forward declarations for IRQ stubs
extern void irq0_stub(void);
extern void irq1_stub(void);
extern void irq2_stub(void);
extern void irq3_stub(void);
extern void irq4_stub(void);
extern void irq5_stub(void);
extern void irq6_stub(void);
extern void irq7_stub(void);
extern void irq8_stub(void);
extern void irq9_stub(void);
extern void irq10_stub(void);
extern void irq11_stub(void);
extern void irq12_stub(void);
extern void irq13_stub(void);
extern void irq14_stub(void);
extern void irq15_stub(void);

#define PIC1            0x20
#define PIC2            0xA0
#define PIC1_COMMAND    PIC1
#define PIC1_DATA       (PIC1+1)
#define PIC2_COMMAND    PIC2
#define PIC2_DATA       (PIC2+1)
#define ICW1_INIT       0x10
#define ICW1_ICW4       0x01
#define ICW4_8086       0x01

extern void idt_load(uint64_t);

struct idt_entry idt[IDT_ENTRIES];
struct idt_ptr idtp;

// Array of C-level IRQ handlers
static void (*irq_handlers[16])(void) = {0};

// Set an IDT gate
void idt_set_gate(int n, uint64_t handler, uint16_t sel, uint8_t flags) {
    idt[n].offset_low = handler & 0xFFFF;
    idt[n].selector = sel;
    idt[n].ist = 0;
    idt[n].type_attr = flags;
    idt[n].offset_mid = (handler >> 16) & 0xFFFF;
    idt[n].offset_high = (handler >> 32) & 0xFFFFFFFF;
    idt[n].reserved = 0;
}

void idt_init(void) {
    idtp.limit = (sizeof(struct idt_entry) * IDT_ENTRIES) - 1;
    idtp.base = (uint64_t)&idt;
    
    // Clear out the entire IDT
    for (int i = 0; i < IDT_ENTRIES; i++) {
        idt_set_gate(i, 0, 0, 0);
    }
    
    // Set up IRQ handlers for all 16 IRQs
    idt_set_gate(32, (uint64_t)&irq0_stub, 0x08, 0x8E);  // Timer
    idt_set_gate(33, (uint64_t)&irq1_stub, 0x08, 0x8E);  // Keyboard
    idt_set_gate(34, (uint64_t)&irq2_stub, 0x08, 0x8E);  // Cascade (used internally by the two PICs. never raised)
    idt_set_gate(35, (uint64_t)&irq3_stub, 0x08, 0x8E);  // COM2 (if enabled)
    idt_set_gate(36, (uint64_t)&irq4_stub, 0x08, 0x8E);  // COM1 (if enabled)
    idt_set_gate(37, (uint64_t)&irq5_stub, 0x08, 0x8E);  // LPT2 (if enabled)
    idt_set_gate(38, (uint64_t)&irq6_stub, 0x08, 0x8E);  // Floppy Disk
    idt_set_gate(39, (uint64_t)&irq7_stub, 0x08, 0x8E);  // LPT1 / Unreliable "spurious" interrupt (usually)
    idt_set_gate(40, (uint64_t)&irq8_stub, 0x08, 0x8E);  // CMOS real-time clock (if enabled)
    idt_set_gate(41, (uint64_t)&irq9_stub, 0x08, 0x8E);  // Legacy SCSI / NIC / Sound card / SCSI / NIC
    idt_set_gate(42, (uint64_t)&irq10_stub, 0x08, 0x8E); // SCSI / NIC / Other
    idt_set_gate(43, (uint64_t)&irq11_stub, 0x08, 0x8E); // SCSI / SATA / Other
    idt_set_gate(44, (uint64_t)&irq12_stub, 0x08, 0x8E); // PS/2 Mouse
    idt_set_gate(45, (uint64_t)&irq13_stub, 0x08, 0x8E); // FPU / Coprocessor / Inter-processor
    idt_set_gate(46, (uint64_t)&irq14_stub, 0x08, 0x8E); // Primary ATA Hard Disk
    idt_set_gate(47, (uint64_t)&irq15_stub, 0x08, 0x8E); // Secondary ATA Hard Disk
    
    // Load the IDT
    idt_load((uint64_t)&idtp);
    
    // Remap the PIC
    pic_remap();
}

void pic_remap(void) {
    uint8_t a1, a2;
    a1 = inb(PIC1_DATA);
    a2 = inb(PIC2_DATA);
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    outb(PIC1_DATA, 0x20);
    outb(PIC2_DATA, 0x28);
    outb(PIC1_DATA, 4);
    outb(PIC2_DATA, 2);
    outb(PIC1_DATA, ICW4_8086);
    outb(PIC2_DATA, ICW4_8086);
    outb(PIC1_DATA, a1);
    outb(PIC2_DATA, a2);
}

void pic_send_eoi(unsigned char irq) {
    if (irq >= 8) outb(PIC2_COMMAND, 0x20);
    outb(PIC1_COMMAND, 0x20);
}

void irq_install_handler(int irq, void (*handler)(void)) {
    if (irq < 16) irq_handlers[irq] = handler;
}

__attribute__((weak)) void irq0_handler(void) {
    if (irq_handlers[0]) irq_handlers[0]();
    pic_send_eoi(0);
}

__attribute__((weak)) void irq1_handler(void) {
    if (irq_handlers[1]) irq_handlers[1]();
    pic_send_eoi(1);
}

__attribute__((weak)) void irq2_handler(void) {
    if (irq_handlers[2]) irq_handlers[2]();
    pic_send_eoi(2);
}

__attribute__((weak)) void irq3_handler(void) {
    if (irq_handlers[3]) irq_handlers[3]();
    pic_send_eoi(3);
}

__attribute__((weak)) void irq4_handler(void) {
    if (irq_handlers[4]) irq_handlers[4]();
    pic_send_eoi(4);
}

__attribute__((weak)) void irq5_handler(void) {
    if (irq_handlers[5]) irq_handlers[5]();
    pic_send_eoi(5);
}

__attribute__((weak)) void irq6_handler(void) {
    if (irq_handlers[6]) irq_handlers[6]();
    pic_send_eoi(6);
}

__attribute__((weak)) void irq7_handler(void) {
    if (irq_handlers[7]) irq_handlers[7]();
    pic_send_eoi(7);
}

__attribute__((weak)) void irq8_handler(void) {
    if (irq_handlers[8]) irq_handlers[8]();
    pic_send_eoi(8);
}

__attribute__((weak)) void irq9_handler(void) {
    if (irq_handlers[9]) irq_handlers[9]();
    pic_send_eoi(9);
}

__attribute__((weak)) void irq10_handler(void) {
    if (irq_handlers[10]) irq_handlers[10]();
    pic_send_eoi(10);
}

__attribute__((weak)) void irq11_handler(void) {
    if (irq_handlers[11]) irq_handlers[11]();
    pic_send_eoi(11);
}

__attribute__((weak)) void irq12_handler(void) {
    if (irq_handlers[12]) irq_handlers[12]();
    pic_send_eoi(12);
}

__attribute__((weak)) void irq13_handler(void) {
    if (irq_handlers[13]) irq_handlers[13]();
    pic_send_eoi(13);
}

__attribute__((weak)) void irq14_handler(void) {
    if (irq_handlers[14]) irq_handlers[14]();
    pic_send_eoi(14);
}

__attribute__((weak)) void irq15_handler(void) {
    if (irq_handlers[15]) irq_handlers[15]();
    pic_send_eoi(15);
}

// I/O port functions are now in io.h