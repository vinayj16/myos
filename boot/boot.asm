; Multiboot2 header
section .multiboot_header
header_start:
    dd 0xe85250d6                ; Magic number (multiboot 2)
    dd 0                         ; Architecture 0 (protected mode i386)
    dd header_end - header_start ; Header length
    ; Checksum
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))

    ; Optional multiboot tags here
    
    ; Required end tag
    dw 0    ; Type
    dw 0    ; Flags
    dd 8    ; Size
header_end:

; Kernel entry point
section .text
bits 32

global _start
extern kernel_main

_start:
    ; Set up stack
    mov esp, stack_top
    
    ; Save multiboot info pointer
    push ebx
    push eax
    
    ; Clear direction flag
    cld
    
    ; Enable SSE
    call enable_sse
    
    ; Call kernel main
    call kernel_main
    
    ; Halt if kernel returns
    cli
    hlt

enable_sse:
    ; Check for SSE
    mov eax, 0x1
    cpuid
    test edx, 1<<25
    jz .no_sse
    
    ; Enable SSE
    mov eax, cr0
    and ax, 0xFFFB      ; Clear coprocessor emulation
    or ax, 0x2          ; Set coprocessor monitoring
    mov cr0, eax
    
    mov eax, cr4
    or ax, 3 << 9      ; Set OSFXSR and OSXMMEXCPT
    mov cr4, eax
    ret
    
.no_sse:
    ; No SSE support - could handle this more gracefully
    mov dword [0xb8000], 0x4f534f45  ; Display 'NO SSE' in red
    mov dword [0xb8004], 0x4f204f20
    mov dword [0xb8008], 0x4f534f45
    hlt

; Stack
section .bss
align 16
stack_bottom:
    resb 16384  ; 16 KB stack
stack_top:
