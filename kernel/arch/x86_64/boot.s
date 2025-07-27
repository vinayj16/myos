; ============================================================================
; boot.s - Kernel entry point and early initialization
; ============================================================================

; Multiboot 1 header
section .multiboot

align 4
multiboot_header:
    ; Magic number for Multiboot 1
    dd 0x1BADB002
    
    ; Flags
    ; Bit 0: Align modules on page boundaries
    ; Bit 1: Provide memory map
    ; Bit 2: Provide video mode table
    dd 0x00010003
    
    ; Checksum
    dd -(0x1BADB002 + 0x00010003)
    
    ; Header address (filled by linker)
    dd 0x00000000
    dd 0x00000000  ; Load address
    dd 0x00000000  ; Load end address
    dd 0x00000000  ; BSS end address
    dd 0x00000000  ; Entry address

; ============================================================================
; Boot code
; ============================================================================
section .boot.text
bits 32
global _start
extern kernel_main

; Entry point - loaded by bootloader at 1MB
_start:
    ; Clear direction flag
    cld
    
    ; Set up stack (32-bit mode)
    mov eax, 0x100000  ; Base load address (1MB)
    mov ebx, stack_bottom - _start
    add eax, ebx
    add eax, 0x1000    ; Stack offset
    mov esp, eax
    mov ebp, eax

    ; Save Multiboot2 info pointer (passed in EAX by bootloader)
    push ebx  ; Multiboot2 info structure (32-bit)
    push eax  ; Magic number (32-bit)

    ; Check if CPU supports CPUID
    pushfd
    pop eax
    mov ecx, eax
    xor eax, 1 << 21
    push eax
    popfd
    pushfd
    pop eax
    push ecx
    popfd
    xor eax, ecx
    jz .no_cpuid

    ; Check for long mode support
    mov eax, 0x80000000
    cpuid
    cmp eax, 0x80000001
    jb .no_long_mode

    mov eax, 0x80000001
    cpuid
    test edx, 1 << 29
    jz .no_long_mode

    ; Set up page tables for long mode
    call setup_page_tables
    
    ; Enable PAE and PGE
    mov eax, cr4
    or eax, (1 << 5) | (1 << 7)  ; PAE | PGE
    mov cr4, eax

    ; Set up long mode
    mov ecx, 0xC0000080      ; EFER MSR
    rdmsr
    or eax, 1 << 8           ; LME bit
    wrmsr
    
    ; Ensure we're in compatibility mode before the far jump
    jmp $+2  ; Small delay for pipeline flush

    ; Enable paging and enter long mode
    mov eax, cr0
    or eax, (1 << 31) | (1 << 0)  ; PG | PE
    mov cr0, eax

    ; Calculate GDT pointer address and load GDT
    call .get_gdt_addr
.get_gdt_addr:
    pop ebx                     ; Get current EIP
    lea eax, [ebx + (gdt64 - .get_gdt_addr)]  ; Calculate GDT address
    mov ecx, gdt64.pointer - gdt64
    mov [eax + ecx + 2], eax    ; Store low 32 bits of GDT address
    
    ; Load GDT pointer (32-bit mode)
    lea edx, [eax + ecx]        ; Get pointer to GDT descriptor
    lgdt [edx]
    
    ; Far jump to long mode
    lea eax, [ebx + (long_mode_start - .get_gdt_addr)]
    push 0x08             ; Code segment selector (32-bit push)
    push eax                    ; 32-bit EIP
    retf

.no_cpuid:
    mov dword [0xB8000], 0x4F4F4F4F  ; Display 'NO CPUID' in red
    hlt

.no_long_mode:
    mov dword [0xB8000], 0x4F4F4F4E  ; Display 'NO 64BIT' in red
    hlt

; Set up initial page tables for long mode
setup_page_tables:
    ; Map first 2MB (identity map)
    mov eax, 0x1000
    mov cr3, eax
    
    ; PML4
    mov dword [0x1000], 0x2000 | 0x3  ; Present, Writable
    mov dword [0x1FF8], 0x1000 | 0x3  ; Recursive mapping
    
    ; PDP
    mov dword [0x2000], 0x3000 | 0x3  ; Present, Writable
    
    ; PD
    mov dword [0x3000], 0x83          ; 2MB page, Present, Writable
    
    ret

; 64-bit code
section .text
bits 64
long_mode_start:
    ; Update segment registers
    mov ax, gdt64.data_segment
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov gs, ax
    mov fs, ax
    
    ; Set up stack using 64-bit addressing
    mov rsp, stack_top - 0x1000  ; Leave some space for the stack
    mov rbp, rsp
    
    ; Load IDT
    lidt [idt64.pointer]
    
    ; Call kernel main with proper 64-bit calling convention
    pop rdi                     ; Load magic number
    pop rsi                     ; Load multiboot2 info pointer
    call kernel_main
    
    ; Halt if kernel returns
    cli
    hlt

; GDT for 64-bit mode
section .rodata
gdt64:
    dq 0x0000000000000000  ; Null descriptor
.code_segment: equ $ - gdt64
    dq 0x00209A0000000000  ; 64-bit code segment (CS)
.data_segment: equ $ - gdt64
    dq 0x0000920000000000  ; 64-bit data segment (DS/SS/ES/FS/GS)
.pointer:
    dw $ - gdt64 - 1       ; GDT limit
    dd 0                   ; Will be filled with GDT base (low 32 bits)
    dd 0                   ; Will be filled with GDT base (high 32 bits)

; Temporary storage for GDT pointer
section .data
gdt64_temp_ptr:
    dq 0

; IDT for 64-bit mode
section .data
align 16
idt64:
    times 256 dq 0, 0  ; 256 IDT entries
    .pointer:
        dw $ - idt64 - 1
        dq idt64

; Stack
section .bss
; Ensure proper alignment for the stack
section .bss
align 16
stack_bottom:
    resb 65536  ; 64KB kernel stack
stack_top:

; Page tables (must be page-aligned)
align 4096
page_tables:
    resb 4096 * 6  ; 6 pages: PML4, PDPT, PD, and 3 PTs

; Export symbols for C code
global idt64

; IDT loading function
global idt_load
idt_load:
    lidt [rdi]
    ret

; IRQ stubs section
section .text.irq

align 16
; IRQ stubs macro
%macro IRQ 1
global irq%1_stub
extern irq%1_handler
irq%1_stub:
    push rbp
    mov rbp, rsp
    push rax
    push rcx
    push rdx
    push rbx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    
    mov rdi, %1
    call irq%1_handler
    
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rbx
    pop rdx
    pop rcx
    pop rax
    pop rbp
    iretq
%endmacro

; Generate IRQ stubs for all 16 IRQs (0-15)
%assign i 0
%rep 16
    IRQ i
%assign i i+1
%endrep

; Kernel entry point (64-bit)
global long_mode_entry
extern kernel_main

long_mode_entry:
    ; Set up stack pointer (64-bit mode)
    mov rsp, stack_top     ; Use direct addressing for stack
    mov rbp, rsp
    
    ; Call kernel main (rdi = multiboot2 magic, rsi = multiboot2 info)
    call kernel_main
    
    ; Halt if kernel returns
    cli
.hang: 
    hlt
    jmp .hang

section .note.GNU-stack noalloc noexec nowrite progbits
