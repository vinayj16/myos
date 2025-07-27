; Working multiboot kernel for MyOS
; This uses a simple, tested multiboot header format

section .multiboot_header
header_start:
    ; multiboot2 header
    dd 0xe85250d6                ; magic number (multiboot 2)
    dd 0                         ; architecture 0 (protected mode i386)
    dd header_end - header_start ; header length
    ; checksum
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))

    ; insert optional multiboot tags here

    ; required end tag
    dw 0    ; type
    dw 0    ; flags
    dd 8    ; size
header_end:

section .text
bits 32
global _start

_start:
    ; The bootloader has loaded us into 32-bit protected mode on a x86 machine.
    ; Interrupts are disabled. Paging is disabled. Processor state is as
    ; defined in the multiboot standard. The kernel has full control of the CPU.
    ; The kernel can only make use of hardware features and any code it provides
    ; as part of itself. There's no printf function, unless the kernel provides
    ; its own <stdio.h> header and a printf implementation. There are no
    ; security restrictions, no safeguards, no debugging mechanisms, only what
    ; the kernel provides itself. It has absolute and complete power over the
    ; machine.

    ; To set up a stack, we set the esp register to point to the top of our
    ; stack (as it grows downwards on x86 systems). This is necessarily done
    ; in assembly as languages such as C cannot function without a stack.
    mov esp, stack_top

    ; This is a good place to initialize crucial processor state before the
    ; high-level kernel is entered. It's best to minimize the early
    ; environment where crucial features are offline. Note that the
    ; processor is not fully initialized yet: Features such as floating
    ; point instructions and instruction set extensions are not initialized
    ; yet. The GDT should be loaded here. Paging should be enabled here.
    ; C++ features such as global constructors and exceptions will require
    ; runtime support to work as well.

    ; Enter the high-level kernel. The ABI requires the stack is 16-byte
    ; aligned at the time of the call instruction (which afterwards pushes
    ; the return pointer of size 4 bytes). The stack was originally 16-byte
    ; aligned above and we've pushed a multiple of 16 bytes to the stack
    ; since (pushed 0 bytes so far), so the alignment has thus been preserved
    ; and the call is well defined.
    call kernel_main

    ; If the system has nothing more to do, put the computer into an
    ; infinite loop. To do that:
    ; 1) Disable interrupts with cli (clear interrupt enable in eflags).
    ;    They are already disabled by the bootloader, so this is not needed.
    ;    Mind that you might later enable interrupts and return from
    ;    kernel_main (which is sort of nonsensical to do).
    ; 2) Wait for the next interrupt to arrive with hlt (halt instruction).
    ;    Since they are disabled, this will lock up the computer.
    ; 3) Jump to the hlt instruction if it ever wakes up due to a
    ;    non-maskable interrupt occurring or due to system management mode.
    cli
.hang:	hlt
    jmp .hang

; Reserve a stack for the initial thread.
section .bss
align 16
stack_bottom:
resb 16384 ; 16 KiB
stack_top:

; Make symbols available to C code
global stack_top
global stack_bottom

; Dummy implementations for missing functions
section .text

global idt_load
idt_load:
    ret  ; Dummy implementation

; Simple IRQ stubs that just return
%macro SIMPLE_IRQ 1
global irq%1_stub
irq%1_stub:
    iret
%endmacro

; Generate simple IRQ stubs
%assign i 0
%rep 16
    SIMPLE_IRQ i
%assign i i+1
%endrep