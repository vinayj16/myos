; ============================================================================
; boot_fixed.s - Fixed kernel entry point with proper multiboot header
; ============================================================================

; Multiboot 2 header (required by bootloader)
section .multiboot_header
align 8

; Constants for Multiboot 2
MULTIBOOT2_MAGIC        equ 0xE85250D6
MULTIBOOT2_ARCHITECTURE equ 0
MULTIBOOT2_HEADER_LENGTH equ multiboot_end - multiboot_start
MULTIBOOT2_CHECKSUM     equ -(MULTIBOOT2_MAGIC + MULTIBOOT2_ARCHITECTURE + MULTIBOOT2_HEADER_LENGTH)

multiboot_start:
    dd MULTIBOOT2_MAGIC
    dd MULTIBOOT2_ARCHITECTURE  
    dd MULTIBOOT2_HEADER_LENGTH
    dd MULTIBOOT2_CHECKSUM

    ; Information request tag
    dw 1    ; type
    dw 0    ; flags
    dd 20   ; size
    dd 4    ; MBI tag types - basic memory info
    dd 6    ; memory map
    dd 8    ; framebuffer info

    ; End tag
    dw 0    ; type
    dw 0    ; flags  
    dd 8    ; size
multiboot_end:

; ============================================================================
; Boot code
; ============================================================================
section .text
bits 32
global _start
extern kernel_main

_start:
    ; Clear direction flag
    cld
    
    ; Set up stack
    mov esp, stack_top
    mov ebp, esp

    ; Save multiboot info
    push ebx  ; Multiboot info structure
    push eax  ; Magic number

    ; Check if we have long mode support
    call check_multiboot
    call check_cpuid
    call check_long_mode

    ; Set up paging for long mode
    call setup_page_tables
    call enable_paging

    ; Load GDT
    lgdt [gdt64.pointer]

    ; Jump to long mode
    jmp gdt64.code:long_mode_start

; Check multiboot
check_multiboot:
    cmp eax, 0x36d76289
    jne .no_multiboot
    ret
.no_multiboot:
    mov al, "0"
    jmp error

; Check CPUID support
check_cpuid:
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
    ret
.no_cpuid:
    mov al, "1"
    jmp error

; Check long mode support
check_long_mode:
    mov eax, 0x80000000
    cpuid
    cmp eax, 0x80000001
    jb .no_long_mode

    mov eax, 0x80000001
    cpuid
    test edx, 1 << 29
    jz .no_long_mode
    ret
.no_long_mode:
    mov al, "2"
    jmp error

; Set up page tables
setup_page_tables:
    ; Map first 2MB
    mov eax, p3_table
    or eax, 0b11 ; present + writable
    mov [p4_table], eax

    mov eax, p2_table
    or eax, 0b11 ; present + writable
    mov [p3_table], eax

    ; Map each P2 entry to a 2MB page
    mov ecx, 0
.map_p2_table:
    mov eax, 0x200000  ; 2MB
    mul ecx
    or eax, 0b10000011 ; present + writable + huge
    mov [p2_table + ecx * 8], eax

    inc ecx
    cmp ecx, 512
    jne .map_p2_table

    ret

; Enable paging
enable_paging:
    ; Load P4 to cr3 register
    mov eax, p4_table
    mov cr3, eax

    ; Enable PAE flag in cr4
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; Set long mode bit in EFER MSR
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; Enable paging in cr0
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    ret

; Error handler
error:
    mov dword [0xb8000], 0x4f524f45
    mov dword [0xb8004], 0x4f3a4f52
    mov dword [0xb8008], 0x4f204f20
    mov byte  [0xb800a], al
    hlt

; 64-bit code
bits 64
long_mode_start:
    ; Load 64-bit data segment
    mov ax, gdt64.data
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Set up stack
    mov rsp, stack_top

    ; Call kernel main
    pop rdi  ; Magic number
    pop rsi  ; Multiboot info
    call kernel_main

    ; Halt
    cli
.hang:
    hlt
    jmp .hang

; GDT for 64-bit mode
section .rodata
gdt64:
    dq 0 ; zero entry
.code: equ $ - gdt64
    dq (1<<44) | (1<<47) | (1<<41) | (1<<43) | (1<<53) ; code segment
.data: equ $ - gdt64
    dq (1<<44) | (1<<47) | (1<<41) ; data segment
.pointer:
    dw $ - gdt64 - 1
    dq gdt64

; Page tables
section .bss
align 4096
p4_table:
    resb 4096
p3_table:
    resb 4096
p2_table:
    resb 4096

; Stack
stack_bottom:
    resb 64 * 1024 ; 64 KB
stack_top:

; IRQ stubs section
section .text

; IDT loading function
global idt_load
idt_load:
    lidt [rdi]
    ret

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

section .note.GNU-stack noalloc noexec nowrite progbits